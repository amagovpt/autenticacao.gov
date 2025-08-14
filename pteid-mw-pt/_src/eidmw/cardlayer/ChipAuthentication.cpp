/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2025 Daniel Dron - <daniel.dron@caixamagica.pt>
 * Copyright (C) 2025 André Guerreiro - <aguerreiro1985@gmail.com>>
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
#include "PaceAuthentication.h"
#include "Log.h"
#include "Crypto.h"
#include "SecureMessaging.h"
#include <eac/eac.h>
#include <openssl/buffer.h>
#include <openssl/ec.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/objects.h>
#include <eac/objects.h>
#include <openssl/types.h>

namespace eIDMW {

using namespace Crypto;

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

BUF_MEM *computeSharedSecret(EVP_PKEY *icc_pubkey, EVP_PKEY *ifd_key) {
	EVP_PKEY_CTX *ctx = NULL;
	BUF_MEM *shared_secret = BUF_MEM_new();
	size_t skeylen = 0;

	ctx = EVP_PKEY_CTX_new(ifd_key, NULL);
	if (!ctx) {
		MWLOG(LEV_ERROR, MOD_CAL, "%s: Failed to allocate EVP_PKEY_CTX: %ld", __FUNCTION__, ERR_get_error());
		goto cleanup;
	}
	if (EVP_PKEY_derive_init(ctx) <= 0) {
		MWLOG(LEV_ERROR, MOD_CAL, "%s: Error returned by EVP_PKEY_derive_init(): %ld", __FUNCTION__, ERR_get_error());
		goto cleanup;
	}
	if (EVP_PKEY_derive_set_peer(ctx, icc_pubkey) <= 0) {
		MWLOG(LEV_ERROR, MOD_CAL, "%s: Error returned by EVP_PKEY_derive_set_peer: %ld", __FUNCTION__, ERR_get_error());
		goto cleanup;
	}

	/* Determine shared secret buffer length */
	if (EVP_PKEY_derive(ctx, NULL, &skeylen) <= 0) {
		char *error_str = ERR_error_string(ERR_peek_last_error(), NULL);
		MWLOG(LEV_ERROR, MOD_CAL, "%s: EVP_PKEY_derive failed (1): %s", __FUNCTION__, error_str);
		goto cleanup;
	}

	shared_secret->length = skeylen;
	shared_secret->data = (char *)OPENSSL_malloc(skeylen);
	shared_secret->max = skeylen;

	if (EVP_PKEY_derive(ctx, (unsigned char *)shared_secret->data, &skeylen) <= 0) {
		const char *file = NULL;
		int line = 0;
		char *error_str = ERR_error_string(ERR_peek_last_error_line(&file, &line), NULL);
		MWLOG(LEV_ERROR, MOD_CAL, "%s: EVP_PKEY_derive failed (2): %s in (%s:%d)", __FUNCTION__, error_str, file, line);
		goto cleanup;
	}

cleanup:
	if (ctx) {
		EVP_PKEY_CTX_free(ctx);
	}
	return shared_secret;
}

/**
 * TODO: use key_length parameter
 */
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

ChipAuthSecureMessaging::ChipAuthSecureMessaging(PTEID_CardHandle hCard, CContext *poContext,
												 const void *paramStructure)
	: SecureMessaging(hCard, poContext, paramStructure) {
	m_ctx = nullptr;
}

ChipAuthSecureMessaging::~ChipAuthSecureMessaging() {
	if (m_ctx) {
		EAC_CTX_clear_free(m_ctx);
	}
}

CByteArray encodeBERTLVObject(unsigned char tag, const CByteArray &data) {
	CByteArray ber_object;
	ber_object.Append(tag);
	unsigned long data_l = data.Size();
	if (data_l < 128) {
		ber_object.Append((unsigned char)data_l);
	}
	else if (data_l < 256) {
		ber_object.Append(0x81);
		ber_object.Append((unsigned char)data_l);
	}
	else if (data.Size() < 65535) {
		ber_object.Append(0x82);
		unsigned char length_msb = (data_l & 0xFF00) >> 8;
		unsigned char length_lsb = data_l & 0xFF;
		ber_object.Append(length_msb);
		ber_object.Append(length_lsb);
	}
	ber_object.Append(data);

	return ber_object;
}

CByteArray buildMSEKetAPDU(unsigned char *pubkey, size_t pubkey_len) {

	CByteArray apdu_kat;
	apdu_kat.Append(0x00);
	apdu_kat.Append(0x22);
	apdu_kat.Append(0x41);
	apdu_kat.Append(0xA6);

	//Ephemeral public key tag
	CByteArray data = encodeBERTLVObject(0x91, CByteArray(pubkey, pubkey_len));

	apdu_kat.Append(data.Size());
	apdu_kat.Append(data);

	return apdu_kat;
}

bool ChipAuthSecureMessaging::sendIFDPublicKey(SecureMessaging *sm, EVP_PKEY *ifd_key, ASN1_OBJECT *oid) {
	OID_INFO oid1 = get_id_CA_ECDH_3DES_CBC_CBC();
	OID_INFO oid2 = get_id_CA_DH_3DES_CBC_CBC();
	bool status = true;
	long ret_value = 0;
	unsigned char *pubkey;
	size_t pubkey_len = EVP_PKEY_get1_encoded_public_key(ifd_key, &pubkey);
	if (pubkey_len == 0) {
		status = false;
		goto cleanup;
	}

	//Send Public key using MSE-SET KAT if CA OID contains 3DES for Secure Messaging
	if (OBJ_cmp(oid1.object, oid) == 0 || OBJ_cmp(oid2.object, oid) == 0) {
		auto resp_apdu = sm->sendSecureAPDU(buildMSEKetAPDU(pubkey, pubkey_len), ret_value);
		unsigned short sw12 = 256 * resp_apdu.GetByte(resp_apdu.Size() - 2) + resp_apdu.GetByte(resp_apdu.Size() - 1);
		status = (sw12 == 0x9000);
		if (!status) {
			MWLOG(LEV_ERROR, MOD_CAL, "%s: Failed MSE:SET KAT command. Response: 0x%x", __FUNCTION__, sw12);
		}
	} else {
		/*
		 For Chip Authentication with AES options use the 2 MSE-SET + General Authenticate variant */
		unsigned char der_oid[20];
		unsigned char *p = der_oid;
		unsigned int der_len = i2d_ASN1_OBJECT(oid, &p);
		//Skip ASN.1 tag and length bytes because the chip only needs the raw value
		CByteArray oid_data = CByteArray(der_oid + 2, der_len - 2);

		CByteArray apdu_mse;
		apdu_mse.Append(0x00);
		apdu_mse.Append(0x22);
		apdu_mse.Append(0x41);
		apdu_mse.Append(0xA4);
		apdu_mse.Append(2 + oid_data.Size());
		apdu_mse.Append(0x80);
		apdu_mse.Append(0x0A); //CA OIDs have constant size of 10 bytes
		apdu_mse.Append(oid_data);

		auto res = sm->sendSecureAPDU(apdu_mse, ret_value);

		if (res.GetByte(0) != 0x90 || res.GetByte(1) != 0x00) {
			MWLOG(LEV_ERROR, MOD_CAL, "%s: Failed MSE:SET AT for CA OID!", __FUNCTION__);
			status = false;
			goto cleanup;
		}
		MWLOG(LEV_DEBUG, MOD_CAL, "%s: MSE-SET: Success", __FUNCTION__);

		// Encoded Ephemeral public key
		CByteArray eepk = encodeBERTLVObject(0x80, CByteArray(pubkey, pubkey_len));

		CByteArray data = encodeBERTLVObject(0x7C, eepk);

		CByteArray apdu_gen_auth;
		apdu_gen_auth.Append(0x00);
		apdu_gen_auth.Append(0x86);
		apdu_gen_auth.Append(0x00);
		apdu_gen_auth.Append(0x00);
		apdu_gen_auth.Append(data.Size());
		apdu_gen_auth.Append(data);
		apdu_gen_auth.Append(0x00);

		res = sm->sendSecureAPDU(apdu_gen_auth, ret_value);
		if (res.GetByte(0) != 0x7C || res.GetByte(1) != 0x00 || res.GetByte(res.Size() - 2) != 0x90 ||
			res.GetByte(res.Size() - 1) != 0x00) {
			MWLOG(LEV_ERROR, MOD_CAL, "%s: Failed General Authentication step", __FUNCTION__);
			status = false;
			goto cleanup;
		}

		MWLOG(LEV_DEBUG, MOD_CAL, "%s: General Authentication step: Success", __FUNCTION__);
	}

cleanup:
	if (pubkey) {
		OPENSSL_free(pubkey);
	}
	return status;
}

bool ChipAuthSecureMessaging::authenticate(SecureMessaging *sm, EVP_PKEY *icc_pubkey, ASN1_OBJECT *oid) {
	bool status = true;

	EVP_PKEY_CTX *gctx = nullptr;
	EVP_PKEY *ifd_key = NULL;
	CByteArray enc, mac;

	BUF_MEM *shared_secret = nullptr;

	CAParams params = getCAParams(oid);
	MWLOG(LEV_DEBUG, MOD_CAL, "%s: Chip Authentication OID: %s ", __FUNCTION__, OBJ_nid2sn(params.nid));

	gctx = EVP_PKEY_CTX_new_from_pkey(NULL, icc_pubkey, NULL);

	// 1. Generate ephemeral DH/ECDH key from parameters in icc_pubkey
	EVP_PKEY_keygen_init(gctx);
	if (EVP_PKEY_generate(gctx, &ifd_key) <= 0) {
		MWLOG(LEV_INFO, MOD_CAL, "%s: Failed to generate privateKey: %s", __FUNCTION__,
			  ERR_error_string(ERR_peek_last_error(), NULL));
	}

	// 2. Send IFD public key to card
	if (!sendIFDPublicKey(sm, ifd_key, oid)) {
		status = false;
		goto cleanup;
	}

	// 3. Compute shared secret
	shared_secret = computeSharedSecret(icc_pubkey, ifd_key);
	if (!shared_secret || shared_secret->length <= 0) {
		MWLOG(LEV_ERROR, MOD_CAL, "%s: Failed to generate shared secret", __FUNCTION__);
		status = false;
		goto cleanup;
	}
	MWLOG(LEV_DEBUG, MOD_CAL, "%s: Shared Secret step: Success", __FUNCTION__);

	// 4. Derive session keys (enc & mac)
	unsigned char ks_enc[32]; // Max size possible
	unsigned char ks_mac[32];
	if (!deriveSessionKeys((unsigned char *)shared_secret->data, shared_secret->length, params.kdf_md, params.key_size,
						   ks_enc, ks_mac)) {
		MWLOG(LEV_ERROR, MOD_CAL, "%s: Session keys derivation failed", __FUNCTION__);
		status = false;
		goto cleanup;
	}
	MWLOG(LEV_DEBUG, MOD_CAL, "%s: Session Keys derivation step: Success", __FUNCTION__);

	// 5. Use keys for secure messaging
	enc = CByteArray(ks_enc, params.key_size);
	mac = CByteArray(ks_mac, params.key_size);

	initEACContext(ifd_key, shared_secret, enc, mac, params);
	MWLOG(LEV_DEBUG, MOD_CAL, "%s: Chip Authentication: Success", __FUNCTION__);

	m_authenticated = true;

cleanup:
	if (icc_pubkey)
		EVP_PKEY_free(icc_pubkey);
	if (gctx)
		EVP_PKEY_CTX_free(gctx);

	return status;
}

void ChipAuthSecureMessaging::initEACContext(EVP_PKEY *eph_pkey, BUF_MEM *shared_secret, CByteArray enc, CByteArray mac,
											 const CAParams &params) {
	EAC_init();
	m_ctx = EAC_CTX_new();
	if (!EAC_CTX_init_ca(m_ctx, params.nid, 0)) {
		MWLOG_CTX(LEV_ERROR, MOD_CAL, "Failed to initialize CA context");
	}

	if (!m_ctx->ca_ctx || !m_ctx->ca_ctx->ka_ctx) {
		MWLOG_CTX(LEV_ERROR, MOD_CAL, "CA context or KA context is NULL");
	}
	KA_CTX *ka = m_ctx->ca_ctx->ka_ctx;
	ka->shared_secret = shared_secret;
	ka->shared_secret->length = shared_secret->length;

	ka->k_enc = BUF_MEM_new();
	BUF_MEM_grow(ka->k_enc, params.key_size);
	memcpy(ka->k_enc->data, enc.GetBytes(), params.key_size);
	ka->k_enc->length = params.key_size;

	ka->k_mac = BUF_MEM_new();
	BUF_MEM_grow(ka->k_mac, params.key_size);
	memcpy(ka->k_mac->data, mac.GetBytes(), params.key_size);
	ka->k_mac->length = params.key_size;

	// EAC_CTX_init_ca initializes this key by default. We will overwrite it, so we should free it first
	if (ka->key)
		EVP_PKEY_free(ka->key);
	ka->key = eph_pkey;

	// Switch to CA secure messaging
	ka->enc_keylen = ka->mac_keylen = params.key_size;
	ka->cipher = params.cipher;
	ka->md = params.kdf_md;
	if (!EAC_CTX_set_encryption_ctx(m_ctx, EAC_ID_CA)) {
		MWLOG_CTX(LEV_ERROR, MOD_CAL, "Failed to set encryption CTX to CA");
	}

	EAC_set_ssc(m_ctx, 0);
}

CByteArray ChipAuthSecureMessaging::encryptData(const CByteArray &data) {
	auto dataMem = bufMemView(data);
	auto encrypted = Ref<BUF_MEM>::takeOwnership(EAC_encrypt(m_ctx, &dataMem));
	return encrypted.toByteArray();
};

CByteArray ChipAuthSecureMessaging::decryptData(const CByteArray &encryptedData) {
	auto dataBem = bufMemView(encryptedData);
	auto decrypted = Ref<BUF_MEM>::takeOwnership(EAC_decrypt(m_ctx, &dataBem));
	return decrypted.toByteArray();
};

CByteArray ChipAuthSecureMessaging::computeMac(const CByteArray &data) {
	auto dataMem = bufMemView(data);
	auto mac = Ref<BUF_MEM>::takeOwnership(EAC_authenticate(m_ctx, &dataMem));
	return mac.toByteArray();
};

CByteArray ChipAuthSecureMessaging::addPadding(const CByteArray &data) {
	auto dataMem = bufMemView(data);
	auto padded = Ref<BUF_MEM>::takeOwnership(EAC_add_iso_pad(m_ctx, &dataMem));
	return padded.toByteArray();
};

CByteArray ChipAuthSecureMessaging::removePadding(const CByteArray &data) {
	auto dataMem = bufMemView(data);
	auto unpadded = Ref<BUF_MEM>::takeOwnership(EAC_remove_iso_pad(&dataMem));
	return unpadded.toByteArray();
};

void ChipAuthSecureMessaging::incrementSSC() { EAC_increment_ssc(m_ctx); };

} // namespace eIDMW
