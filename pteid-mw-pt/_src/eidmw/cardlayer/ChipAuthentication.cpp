/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2025 Daniel Dron - <daniel.dron@caixamagica.pt>
 *
 * This is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License version
 * 3.0 as published by the Free Software Foundation.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this software; if not, see
 * http://www.gnu.org/licenses/.

**************************************************************************** */

#include "ChipAuthentication.h"
#include "Log.h"
#include <openssl/buffer.h>
#include <openssl/ec.h>
#include <openssl/evp.h>
#include <openssl/objects.h>
#include <eac/objects.h>

namespace eIDMW {

CAParams getCAParams(const ASN1_OBJECT *oid) {
	CAParams params = {EVP_sha1(), 16};

	int nid = OBJ_obj2nid(oid);
	params.nid = nid;

	if (nid == NID_id_CA_ECDH_3DES_CBC_CBC || nid == NID_id_CA_DH_3DES_CBC_CBC) {
		params.kdf_md = EVP_sha1();
		params.key_size = 16;
		params.cipher = EVP_des_ede_cbc();
	} else if (nid == NID_id_CA_ECDH_AES_CBC_CMAC_128 || nid == NID_id_CA_DH_AES_CBC_CMAC_128) {
		params.kdf_md = EVP_sha1();
		params.key_size = 16;
		params.cipher = EVP_aes_128_cbc();
	} else if (nid == NID_id_CA_ECDH_AES_CBC_CMAC_192 || nid == NID_id_CA_DH_AES_CBC_CMAC_192) {
		params.kdf_md = EVP_sha256();
		params.key_size = 24;
		params.cipher = EVP_aes_192_cbc();
	} else if (nid == NID_id_CA_ECDH_AES_CBC_CMAC_256 || nid == NID_id_CA_DH_AES_CBC_CMAC_256) {
		params.kdf_md = EVP_sha256();
		params.key_size = 32;
		params.cipher = EVP_aes_256_cbc();
	} else {
		MWLOG(LEV_ERROR, MOD_APL, "%s: Unknown protocol NID %d", __FUNCTION__, nid);
	}

	return params;
}

BUF_MEM *computeSharedSecret(EVP_PKEY *eph_pkey, EVP_PKEY *chip_pkey) {
	BUF_MEM *shared_secret = NULL;
	EC_KEY *ecdh = NULL;
	EC_KEY *chip_ec = NULL;
	const EC_GROUP *group = NULL;
	const EC_POINT *chip_pub_point = NULL;

	ecdh = EVP_PKEY_get1_EC_KEY(eph_pkey);
	if (!ecdh) {
		return NULL;
	}

	group = EC_KEY_get0_group(ecdh);
	if (!group) {
		goto err;
	}

	chip_ec = EVP_PKEY_get1_EC_KEY(chip_pkey);
	if (!chip_ec) {
		goto err;
	}
	chip_pub_point = EC_KEY_get0_public_key(chip_ec);

	shared_secret = BUF_MEM_new();
	shared_secret->length = ECDSA_size(ecdh);
	shared_secret->data = (char *)OPENSSL_malloc(shared_secret->length);
	shared_secret->max = shared_secret->length;

	// K = KA(SKPCD, PKPICC, DPICC)
	shared_secret->length = ECDH_compute_key(shared_secret->data, shared_secret->length, chip_pub_point, ecdh, NULL);
	if (shared_secret->length <= 0) {
		goto err;
	}

	EC_KEY_free(ecdh);
	EC_KEY_free(chip_ec);
	return shared_secret;

err:
	if (shared_secret)
		BUF_MEM_free(shared_secret);
	if (ecdh)
		EC_KEY_free(ecdh);
	if (chip_ec)
		EC_KEY_free(chip_ec);
	return NULL;
}

bool deriveSessionKeys(const unsigned char *shared_secret, size_t secret_len, const EVP_MD *md, size_t key_length,
					   unsigned char *ks_enc, unsigned char *ks_mac) {
	EVP_MD_CTX *mdctx = NULL;
	bool ret = false;
	unsigned int digest_len;

	unsigned char *concat_buffer = NULL;

	concat_buffer = (unsigned char *)malloc(secret_len + 4);
	if (concat_buffer == NULL) {
		goto cleanup;
	}

	mdctx = EVP_MD_CTX_new();
	if (mdctx == NULL) {
		goto cleanup;
	}

	memcpy(concat_buffer, shared_secret, secret_len);
	concat_buffer[secret_len] = 0x00;
	concat_buffer[secret_len + 1] = 0x00;
	concat_buffer[secret_len + 2] = 0x00;
	concat_buffer[secret_len + 3] = 0x01;

	if (!EVP_DigestInit_ex(mdctx, md, NULL) || !EVP_DigestUpdate(mdctx, concat_buffer, secret_len + 4) ||
		!EVP_DigestFinal_ex(mdctx, ks_enc, &digest_len)) {
		goto cleanup;
	}

	concat_buffer[secret_len + 3] = 0x02;

	if (!EVP_DigestInit_ex(mdctx, md, NULL) || !EVP_DigestUpdate(mdctx, concat_buffer, secret_len + 4) ||
		!EVP_DigestFinal_ex(mdctx, ks_mac, &digest_len)) {
		goto cleanup;
	}

	ret = true;

cleanup:
	if (concat_buffer) {
		OPENSSL_cleanse(concat_buffer, secret_len + 4); /* Secure zeroing */
		free(concat_buffer);
	}
	EVP_MD_CTX_free(mdctx);
	return ret;
}

bool ChipAuthentication::upgradeSecureMessaging(SecureMessaging *sm, EVP_PKEY *pkey, ASN1_OBJECT *oid) {
	bool status = true;
	long ret_value = 0;

	// Step 2 Variables
	int key_type = 0;
	int key_size = 0;
	EC_KEY *ec_key = nullptr;
	EC_KEY *eph_key = nullptr;
	EVP_PKEY *eph_pkey = nullptr;

	CByteArray enc, mac;

	// Step 3 Variables
	unsigned char *eph_pubkey_buf = NULL;

	// Step 4 Variables
	BUF_MEM *shared_secret = nullptr;
	CAParams params = getCAParams(oid);
	MWLOG(LEV_DEBUG, MOD_CAL, "%s: Chip Authentication OID: %s ", __FUNCTION__, OBJ_nid2sn(params.nid));

	// 1. Set security environment for chip authentication
	{
		unsigned char der_oid[20];
		unsigned char *p = der_oid;
		unsigned int der_len = i2d_ASN1_OBJECT(oid, &p);
		CByteArray oid_data = CByteArray(der_oid + 2, der_len - 2);

		CByteArray apdu_mse;
		apdu_mse.Append(0x00);
		apdu_mse.Append(0x22);
		apdu_mse.Append(0x41);
		apdu_mse.Append(0xA4);
		apdu_mse.Append(2 + der_len);
		apdu_mse.Append(0x80);
		apdu_mse.Append(0x0A);
		apdu_mse.Append(oid_data);
		auto res = sm->sendSecureAPDU(apdu_mse, ret_value);
		if (res.GetByte(0) != 0x90 || res.GetByte(1) != 0x00) {
			MWLOG(LEV_ERROR, MOD_CAL, "%s: Failed MSE-SET apdu 0x%x", __FUNCTION__, res.GetBytes());
			status = false;
			goto cleanup;
		}
		MWLOG(LEV_DEBUG, MOD_CAL, "%s: MSE-SET: Success", __FUNCTION__);
	}

	// 2. Generate ephemeral key-pair
	{
		key_type = EVP_PKEY_base_id(pkey);
		key_size = EVP_PKEY_bits(pkey);
		ec_key = (EC_KEY *)EVP_PKEY_get0_EC_KEY(pkey);
		if (!ec_key) {
			MWLOG(LEV_ERROR, MOD_CAL, "%s: Could not retrieve EC key from dg14 pkey", __FUNCTION__);
			status = false;
			goto cleanup;
		}

		eph_key = EC_KEY_new_by_curve_name(EC_GROUP_get_curve_name(EC_KEY_get0_group(ec_key)));
		if (!eph_key || !EC_KEY_generate_key(eph_key)) {
			MWLOG(LEV_ERROR, MOD_CAL, "%s: Failed to generate ephemeral keys", __FUNCTION__);
			status = false;
			goto cleanup;
		}

		eph_pkey = EVP_PKEY_new();
		if (!eph_pkey || !EVP_PKEY_assign_EC_KEY(eph_pkey, eph_key)) {
			MWLOG(LEV_ERROR, MOD_CAL, "%s: Failed to assign ephemeral key", __FUNCTION__);
			status = false;
			goto cleanup;
		}

		MWLOG(LEV_DEBUG, MOD_CAL, "%s: Ephemeral key-pair generation: Success", __FUNCTION__);
	}

	// 3. Send public key to card
	{
		// Convert ephemeral public key to octet string
		size_t eph_pubkey_len = 0;
		eph_pubkey_len =
			EC_KEY_key2buf(EVP_PKEY_get0_EC_KEY(eph_pkey), POINT_CONVERSION_UNCOMPRESSED, &eph_pubkey_buf, NULL);

		// Encoded ephemeral public key
		CByteArray eepk;
		eepk.Append(0x80);
		eepk.Append(eph_pubkey_len);
		eepk.Append(CByteArray(eph_pubkey_buf, eph_pubkey_len));

		CByteArray data;
		data.Append(0x7C);
		data.Append(eepk.Size());
		data.Append(eepk);

		CByteArray apdu_gen_auth;
		apdu_gen_auth.Append(0x00);
		apdu_gen_auth.Append(0x86);
		apdu_gen_auth.Append(0x00);
		apdu_gen_auth.Append(0x00);
		apdu_gen_auth.Append(data.Size());
		apdu_gen_auth.Append(data);
		apdu_gen_auth.Append(0x00);

		CByteArray dyn;
		dyn.Append(0x80);
		dyn.Append(eph_pubkey_len);
		dyn.Append(CByteArray(eph_pubkey_buf, eph_pubkey_len));

		CByteArray input;
		input.Append(0x7C);
		input.Append(dyn.Size());
		input.Append(dyn);

		CByteArray apdu_kat;
		apdu_kat.Append(0x00);
		apdu_kat.Append(0x86);
		apdu_kat.Append(0x00);
		apdu_kat.Append(0x00);
		apdu_kat.Append(input.Size());
		apdu_kat.Append(input);
		apdu_kat.Append(0x00);

		auto res = sm->sendSecureAPDU(apdu_kat, ret_value);
		if (!res.Equals({"7C009000", true})) {
			MWLOG(LEV_ERROR, MOD_CAL, "%s: Failed General Authentication step", __FUNCTION__);
			status = false;
			goto cleanup;
		}

		MWLOG(LEV_DEBUG, MOD_CAL, "%s: General Authentication step: Success", __FUNCTION__);
	}

	// 4. Compute shared secret
	shared_secret = computeSharedSecret(eph_pkey, pkey);
	if (!shared_secret || shared_secret->length <= 0) {
		MWLOG(LEV_ERROR, MOD_CAL, "%s: Failed to generate shared secret", __FUNCTION__);
		status = false;
		goto cleanup;
	}
	MWLOG(LEV_DEBUG, MOD_CAL, "%s: Shared Secret step: Success", __FUNCTION__);

	// 5. Derive session keys (enc & mac)
	unsigned char ks_enc[32]; // Max size possible
	unsigned char ks_mac[32];
	if (!deriveSessionKeys((unsigned char *)shared_secret->data, shared_secret->length, params.kdf_md, params.key_size,
						   ks_enc, ks_mac)) {
		MWLOG(LEV_ERROR, MOD_CAL, "%s: Session keys derivation failed", __FUNCTION__);
		status = false;
		goto cleanup;
	}
	MWLOG(LEV_DEBUG, MOD_CAL, "%s: Session Keys derivation step: Success", __FUNCTION__);

	// 6. Use keys for secure messaging
	enc = CByteArray(ks_enc, params.key_size);
	mac = CByteArray(ks_mac, params.key_size);
	sm->upgradeKeys(eph_pkey, shared_secret, enc, mac, params);
	MWLOG(LEV_DEBUG, MOD_CAL, "%s: Chip Authentication: Success", __FUNCTION__);

cleanup:
	if (pkey)
		EVP_PKEY_free(pkey);
	if (eph_pubkey_buf)
		free(eph_pubkey_buf);

	return status;
}

} // namespace eIDMW
