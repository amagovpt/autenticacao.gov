/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2025 Daniel Dron - <daniel.dron@caixamagica.pt>
 *
 * This is free software; you can redistribute it and/or modify it
 * under the terms of the GNULesser General Public License version
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

#include "BacAuthentication.h"
#include "APDU.h"
#include "ByteArray.h"
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <openssl/des.h>
#include <openssl/rand.h>
#include <openssl/sha.h>

void dumpByteArray(unsigned char *ba, size_t len);

namespace eIDMW {

CByteArray paddedByteArray(CByteArray &input) {
	int padLen = 8 - input.Size() % 8;

	CByteArray paddedContent;
	paddedContent.Append(input);
	paddedContent.Append(0x80);

	// Padlen - 1 because one byte was already spent with 0x80
	for (int i = 0; i < padLen - 1; i++)
		paddedContent.Append(0x00);

	return paddedContent;
}

#define MAC_KEYSIZE 8
#define MAC_LEN 8

void BacAuthentication::SM_Keys::generate3DesKeysFromKeySeed(const unsigned char *keySeed16, size_t keySeedLen) {
	unsigned char k_seed[SHA_DIGEST_LENGTH];
	unsigned char k_seed_digest[SHA_DIGEST_LENGTH];
	if (keySeedLen != 16) {
		// TODO: Error fprintf(stderr, "Illegal input for %s: should be a 16-byte key_seed\n", __FUNCTION__);
		assert(false);
	}
	memcpy(k_seed, keySeed16, 16);
	// Only the 16 most significant bytes are used from kseed so we can overwrite the last 4
	k_seed[SHA_DIGEST_LENGTH - 4] = 0x00;
	k_seed[SHA_DIGEST_LENGTH - 3] = 0x00;
	k_seed[SHA_DIGEST_LENGTH - 2] = 0x00;
	k_seed[SHA_DIGEST_LENGTH - 1] = 0x01;

	SHA1(k_seed, SHA_DIGEST_LENGTH, k_seed_digest);
	memcpy(ksEnc, k_seed_digest, 16);

	k_seed[SHA_DIGEST_LENGTH - 1] = 0x02;

	SHA1(k_seed, SHA_DIGEST_LENGTH, k_seed_digest);
	memcpy(ksMac, k_seed_digest, 16);
}

void BacAuthentication::BacKeys::generateAccessKeys(const CByteArray &mrzInfo) {
	unsigned char k_seed[SHA_DIGEST_LENGTH];
	unsigned char k_seed_digest[SHA_DIGEST_LENGTH];

	SHA1(mrzInfo.GetBytes(), mrzInfo.Size(), k_seed);

	// Only the 16 most significant bytes are used from kseed so we can overwrite the last 4
	k_seed[SHA_DIGEST_LENGTH - 4] = 0x00;
	k_seed[SHA_DIGEST_LENGTH - 3] = 0x00;
	k_seed[SHA_DIGEST_LENGTH - 2] = 0x00;
	k_seed[SHA_DIGEST_LENGTH - 1] = 0x01;

	SHA1(k_seed, SHA_DIGEST_LENGTH, k_seed_digest);
	memcpy(kEnc, k_seed_digest, 16);

	k_seed[SHA_DIGEST_LENGTH - 1] = 0x02;

	SHA1(k_seed, SHA_DIGEST_LENGTH, k_seed_digest);
	memcpy(kMac, k_seed_digest, 16);
}

int triple_des_cbc_nopadding(const unsigned char *enc_input, size_t len, const unsigned char *k_enc, unsigned char *out,
							 int encrypt_mode) {
	unsigned char zero_iv[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	int outl = 0;
	EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
	if (!ctx) {
		goto cleanup;
	}
	EVP_CIPHER_CTX_init(ctx);
	if (!EVP_CipherInit_ex(ctx, EVP_des_ede_cbc(), NULL, (const unsigned char *)k_enc, zero_iv, encrypt_mode)) {
		goto cleanup;
	}
	EVP_CIPHER_CTX_set_padding(ctx, 0);
	if (!EVP_CipherUpdate(ctx, out, &outl, enc_input, len)) {
		goto cleanup;
	}
	fprintf(stderr, "DBG: %s %d bytes\n", encrypt_mode ? "Encrypted" : "Decrypted", outl);
	return outl;

cleanup:
	EVP_CIPHER_CTX_free(ctx);
	return outl;
}

unsigned char *retail_mac_des(const unsigned char *key, const unsigned char *mac_input, size_t mac_input_len) {

	unsigned char xx[MAC_KEYSIZE];
	unsigned char *des_out = reinterpret_cast<unsigned char *>(malloc(MAC_KEYSIZE));
	unsigned char padding[] = {0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

	if (mac_input_len % MAC_KEYSIZE != 0) {
		fprintf(stderr, "Warning: input data is not multiple of cipher block size!\n");
	}

	unsigned char *msg = NULL;
	DES_key_schedule ks_a;
	DES_key_schedule ks_b;
	const unsigned char *key_a = key;
	const unsigned char *key_b = key + 8;

	// Padding method 2 will always add some padding bytes
	size_t input_len = mac_input_len + MAC_KEYSIZE;
	msg = reinterpret_cast<unsigned char *>(malloc(input_len));

	memcpy(msg, mac_input, mac_input_len);
	memcpy(msg + mac_input_len, padding, sizeof(padding));

	memset(des_out, 0, MAC_KEYSIZE);

	DES_set_key_unchecked((const_DES_cblock *)key_a, &ks_a);
	DES_set_key_unchecked((const_DES_cblock *)key_b, &ks_b);

	for (size_t j = 0; j < (input_len / MAC_KEYSIZE); j++) {
		for (size_t i = 0; i < MAC_KEYSIZE; i++)
			xx[i] = msg[i + j * MAC_KEYSIZE] ^ des_out[i];

		DES_ecb_encrypt((const_DES_cblock *)xx, (DES_cblock *)des_out, &ks_a, 1);
	}

	memcpy(xx, des_out, MAC_KEYSIZE);
	DES_ecb_encrypt((const_DES_cblock *)xx, (DES_cblock *)des_out, &ks_b, 0);

	memcpy(xx, des_out, MAC_KEYSIZE);
	DES_ecb_encrypt((const_DES_cblock *)xx, (DES_cblock *)des_out, &ks_a, 1);

	return des_out;
}

// TODO: two next functions, error handling and logging
CByteArray encrypt_data_3des(CByteArray &key, CByteArray &in) {
	CByteArray cipher_text;
	unsigned char *out = (unsigned char *)malloc(in.Size());
	EVP_CIPHER_CTX *ctx = NULL;
	unsigned int len = 0;

	if (key.Size() == 0) {
		fprintf(stderr, "encrypt_data_3des(): Empty key!\n");
		goto err;
	}

	ctx = EVP_CIPHER_CTX_new();
	if (!ctx)
		goto err;

	// Null as the 5th param means that IV is empty
	//  1 as last param means encryption
	if (!EVP_CipherInit_ex(ctx, EVP_des_ede_cbc(), NULL, (unsigned char *)key.GetBytes(), NULL, 1) ||
		!EVP_CIPHER_CTX_set_padding(ctx, 0))
		goto err;

	if (EVP_EncryptUpdate(ctx, out, (int *)&len, in.GetBytes(), in.Size()) == 0) {
		fprintf(stderr, "Error in encrypt_data_3des() !\n");
	}

	if (len != in.Size()) {
		fprintf(stderr, "encrypt_data_3des() error: len < in.size()\n");
	}

	cipher_text = CByteArray((const unsigned char *)out, (unsigned long)len);
	free(out);
	return cipher_text;

err:
	free(out);
	return CByteArray();
}

CByteArray sm_retail_mac_des(CByteArray &key, CByteArray &mac_input, uint64_t ssc) {
	CByteArray macInputWithSSC;
	CByteArray secondKey = key.GetBytes(MAC_KEYSIZE, MAC_KEYSIZE);

	unsigned char ssc_block[] = {0, 0, 0, 0, 0, 0, 0, 0};
	unsigned char xx[MAC_KEYSIZE];
	unsigned char des_out[MAC_KEYSIZE];
	unsigned char *msg = NULL;
	DES_key_schedule ks_a;
	DES_key_schedule ks_b;
	size_t i, j;

	CByteArray in;

	// SSC is an 8-byte counter that serves as the first block of the MAC input
	for (i = 0; i < sizeof(ssc_block); i++)
		ssc_block[i] = (0xFF & ssc >> (7 - i) * 8);

	// unsigned char output[8];
	macInputWithSSC.Append(ssc_block, sizeof(ssc_block));
	macInputWithSSC.Append(mac_input);

	msg = macInputWithSSC.GetBytes();

	memset(des_out, 0, sizeof(des_out));

	DES_set_key_unchecked((const_DES_cblock *)key.GetBytes(), &ks_a);
	DES_set_key_unchecked((const_DES_cblock *)secondKey.GetBytes(), &ks_b);

	for (j = 0; j < (macInputWithSSC.Size() / MAC_KEYSIZE); j++) {
		// fprintf(stderr, "Running DES_ENCRYPT iteration: %d\n", j);
		for (i = 0; i < MAC_KEYSIZE; i++)
			xx[i] = msg[i + j * MAC_KEYSIZE] ^ des_out[i];

		DES_ecb_encrypt((const_DES_cblock *)xx, (DES_cblock *)des_out, &ks_a, 1);
	}

	memcpy(xx, des_out, MAC_KEYSIZE);
	DES_ecb_encrypt((const_DES_cblock *)xx, (DES_cblock *)des_out, &ks_b, 0);

	memcpy(xx, des_out, MAC_KEYSIZE);
	DES_ecb_encrypt((const_DES_cblock *)xx, (DES_cblock *)des_out, &ks_a, 1);

	CByteArray result(des_out, MAC_KEYSIZE);

	return result;
}

BacAuthentication::BacAuthentication(CContext *poContext) : m_context(poContext) {}
BacAuthentication::~BacAuthentication() {}

void BacAuthentication::authenticate(SCARDHANDLE hCard, const void *paramStructure, const CByteArray &mrzInfo) {
	// set temporary state
	m_card = hCard;
	m_param = paramStructure;

	long retValue = {0};

	unsigned char ifdBacData[40] = {0};
	unsigned char iccBacData[40] = {0}; // Returned from the card

	auto iccRandom = getRandomFromCard();
	BacKeys bacKeys = generateBacData(mrzInfo, iccRandom, ifdBacData);

	// --------------------
	// Send mutual authentication
	CByteArray mutual_auth;
	mutual_auth.Append(0x00);
	mutual_auth.Append(0x82);
	mutual_auth.Append(0x00);
	mutual_auth.Append(0x00);
	mutual_auth.Append(0x28); // 40
	mutual_auth.Append(ifdBacData, sizeof(ifdBacData));
	mutual_auth.Append(0x00); // Le
	auto resp = sendAPDU(mutual_auth, retValue);

	// TODO: Error
	assert(resp.Size() - 2 == sizeof(iccBacData));
	assert(resp.GetByte(resp.Size() - 2) == 0x90 && "Failed to perform mutual authentication");

	// Save response
	memcpy(iccBacData, resp.GetBytes(), resp.Size() - 2);

	// --------------------
	// Check ICC and store derived keys
	unsigned char decryptedIccData[40] = {0};
	const size_t MAC_OFFSET = sizeof(iccBacData) - MAC_KEYSIZE;
	unsigned char *mac = retail_mac_des(bacKeys.kMac, iccBacData, sizeof(iccBacData) - MAC_KEYSIZE);
	const unsigned char *iccMac = iccBacData + MAC_OFFSET;
	if (memcmp(mac, iccMac, MAC_KEYSIZE) != 0) {
		// TODO: Failed to verify ICC MAC
		assert(false);
	}
	int outl =
		triple_des_cbc_nopadding(iccBacData, sizeof(decryptedIccData) - MAC_KEYSIZE, bacKeys.kEnc, decryptedIccData, 0);
	int matchRndIcc = memcmp(bacKeys.rndIcc, decryptedIccData, 8);
	int matchRndIfd = memcmp(bacKeys.rndIfd, decryptedIccData + 8, 8);

	// TODO: Error
	assert(matchRndIcc == 0 && matchRndIfd == 0 && "Failed to verify Rnd ICC or IFD");

	// Store keys
	memcpy(bacKeys.kicc, decryptedIccData + 16, 16);

	// --------------------
	// Generate session keys and SSC
	unsigned char *rnd_icc = bacKeys.rndIcc;
	unsigned char *rnd_ifd = bacKeys.rndIfd;

	unsigned char *ssc = &(m_smKeys.ssc[0]);
	memcpy(ssc, rnd_icc + 4, 4);
	memcpy(ssc + 4, rnd_ifd + 4, 4);

	unsigned char kseed[16] = {0}; // Kseed = XOR of Kicc and Kifd
	for (int i = 0; i < sizeof(bacKeys.kifd); i++) {
		kseed[i] = bacKeys.kifd[i] ^ bacKeys.kicc[i];
	}
	m_smKeys.generate3DesKeysFromKeySeed(kseed, sizeof(kseed));

	auto bigEndianBytesToLong = [](const uint8_t *bytes, size_t length) {
		long value = 0;
		for (size_t i = 0; i < length; i++) {
			value = (value << 8) | bytes[i]; // Shift existing value and add next byte
		}
		return value;
	};
	m_ssc = bigEndianBytesToLong(m_smKeys.ssc, sizeof(m_smKeys.ssc));

	printf("Authenticated with back with success!\n");
}

CByteArray decrypt_data_3des(CByteArray &key, CByteArray &in) {
	CByteArray plain_text;
	unsigned char *out = (unsigned char *)malloc(in.Size());
	EVP_CIPHER_CTX *ctx = NULL;
	unsigned int len = 0;

	if (key.Size() == 0) {
		fprintf(stderr, "decrypt_data_3des(): Empty key!\n");
		goto err;
	}

	ctx = EVP_CIPHER_CTX_new();
	if (!ctx)
		goto err;

	if (!EVP_CipherInit_ex(ctx, EVP_des_ede_cbc(), NULL, (unsigned char *)key.GetBytes(), NULL, 0) ||
		!EVP_CIPHER_CTX_set_padding(ctx, 0))
		goto err;

	if (EVP_DecryptUpdate(ctx, out, (int *)&len, in.GetBytes(), in.Size()) == 0) {
		fprintf(stderr, "Error in decrypt_data_3des() !\n");
	}

	if (len != in.Size()) {
		fprintf(stderr, "decrypt_data_3des() Error: len < in.size()\n");
	}

	plain_text = CByteArray((const unsigned char *)out, (unsigned long)len);
	free(out);
	return plain_text;

err:
	free(out);
	return CByteArray();
}

CByteArray BacAuthentication::decryptData(const CByteArray &data) {
	CByteArray encryptionKey = {m_smKeys.ksEnc, sizeof(m_smKeys.ksEnc)};

	CByteArray cryptogram(data.GetBytes(3, data.GetByte(1)-1));

	CByteArray out = decrypt_data_3des(encryptionKey, cryptogram);
	dumpByteArray(out.GetBytes(), out.Size());
	
	return out; 
}

CByteArray BacAuthentication::sendSecureAPDU(const CByteArray &apdu) {
	CByteArray encryptionKey = {m_smKeys.ksEnc, sizeof(m_smKeys.ksEnc)};
	CByteArray macKey = {m_smKeys.ksMac, sizeof(m_smKeys.ksMac)};

	CByteArray final_apdu;
	const unsigned char Tcg = 0x87;
	const unsigned char Tcc = 0x8e;
	const unsigned char Tle = 0x97;
	const unsigned char paddingIndicator = 0x01;

	const unsigned char controlByte = 0x0C;

	// Contains data
	if (apdu.Size() > 5) {

		CByteArray command_header(apdu.GetBytes(0, 4));
		// Mark the APDU as SM
		command_header.SetByte(command_header.GetByte(0) | controlByte, 0);

		CByteArray input_data(apdu.GetBytes(5, apdu.Size() - 5));
		CByteArray paddedInput = paddedByteArray(input_data);

		CByteArray cryptogram = encrypt_data_3des(encryptionKey, paddedInput);
		// LCg = Len(Cg) + Len(PI = 1)
		int lcg = cryptogram.Size() + 1;
		CByteArray inputForMac = paddedByteArray(command_header);

		CByteArray paddedCryptogram;
		paddedCryptogram.Append(Tcg);
		// This is safe because Lcg will be always lower than
		paddedCryptogram.Append((unsigned char)lcg);
		paddedCryptogram.Append(paddingIndicator);
		paddedCryptogram.Append(cryptogram);

		paddedCryptogram = paddedByteArray(paddedCryptogram);

		// Padded Command Header + Padded [Tcg Lcg PI,CG]
		inputForMac.Append(paddedCryptogram);

		// Pre-increment SSC
		m_ssc++;

		CByteArray mac = sm_retail_mac_des(macKey, inputForMac, m_ssc);

		// Build final APDU including Cryptogram and MAC
		int lc_final = MAC_LEN + 2 + cryptogram.Size() + 3; // CG + Tcg + Lcg + PI
		final_apdu.Append(command_header);
		final_apdu.Append((unsigned char)lc_final);
		final_apdu.Append(Tcg);
		final_apdu.Append((unsigned char)lcg);
		final_apdu.Append(paddingIndicator);
		final_apdu.Append(cryptogram);
		final_apdu.Append(Tcc);
		final_apdu.Append(MAC_LEN); // Lcc
		final_apdu.Append(mac);
		final_apdu.Append(0x00); // Le
	} else {

		// For commands sending no data the format should be
		// XC INS P1 P2 Lc Tle 01 Le Tcc Lcc MAC
		CByteArray TlvLe;
		CByteArray command_header(apdu.GetBytes(0, 4));
		command_header.SetByte(command_header.GetByte(0) | controlByte, 0);
		int le = apdu.GetByte(4);
		CByteArray inputForMac = paddedByteArray(command_header);

		TlvLe.Append(Tle);
		TlvLe.Append(0x01);
		TlvLe.Append(le);

		inputForMac.Append(paddedByteArray(TlvLe));

		// Pre-increment SSC
		m_ssc++;

		CByteArray mac = sm_retail_mac_des(macKey, inputForMac, m_ssc);

		int lc_final = MAC_LEN + 2 + TlvLe.Size();
		final_apdu.Append(command_header);
		final_apdu.Append((unsigned char)lc_final);
		final_apdu.Append(TlvLe);
		final_apdu.Append(Tcc);
		final_apdu.Append(MAC_LEN); // Lcc
		final_apdu.Append(mac);
		final_apdu.Append(0x00); // Le
	}

	long returnValue = {0};
	auto response = sendAPDU(final_apdu, returnValue);

    if (!checkMacInResponse(response, macKey) ) {
        fprintf(stderr, "Failed to verifiy MAC on response");
    }

    printf("ssc -> %ld\n", m_ssc);

    return response;
}

CByteArray BacAuthentication::sendAPDU(const CByteArray &apdu, long &returnValue) {
	return m_context->m_oPCSC.Transmit(m_card, apdu, &returnValue, m_param);
}

CByteArray BacAuthentication::getRandomFromCard() {
	long returnValue = {0};

	APDU apdu;
	apdu.ins() = 0x84;
	apdu.p1() = 0x00;
	apdu.p2() = 0x00;
	apdu.setLe(0x08);
	return sendAPDU(apdu.ToByteArray(), returnValue);
}

BacAuthentication::BacKeys BacAuthentication::generateBacData(const CByteArray &mrzInfo, const CByteArray &random,
										   unsigned char *bac_data) {
	const int CIPHER_KEY_SIZE = 16;

	BacKeys bac_keys = {0};
	bac_keys.generateAccessKeys(mrzInfo);

	// Copy random ICC to bac keys
	memcpy(bac_keys.rndIcc, random.GetBytes(), random.Size());

	// Generate random IFD and KIFD
	RAND_bytes(bac_keys.rndIfd, 8);
	RAND_bytes(bac_keys.kifd, 16);

	unsigned char enc_input[32] = {0};
	memcpy(enc_input, bac_keys.rndIfd, 8);
	memcpy(enc_input + 8, random.GetBytes(), 8);
	memcpy(enc_input + 16, bac_keys.kifd, 16);

	// Encrypt SIFD = (RND.IFD || RND.ICC || KIFD) and generate MAC
	int outl = triple_des_cbc_nopadding(enc_input, sizeof(enc_input), bac_keys.kEnc, bac_data, 1);

	unsigned char *mac = retail_mac_des(bac_keys.kMac, bac_data, outl);
	memcpy(bac_data + 32, mac, MAC_KEYSIZE);

	return bac_keys;
}

bool BacAuthentication::checkMacInResponse(CByteArray &resp, CByteArray &macKey) {
	CByteArray receivedMac;
	CByteArray paddedInput;

	/*
		Deal with 2 different formats of response APDUs: with just SW12 or also with data
	*/
	if (resp.GetByte(0) == 0x99) {
		receivedMac = (resp.GetBytes(6, 8));
		CByteArray inputForMac = resp.GetBytes(0, 4);
		paddedInput = paddedByteArray(inputForMac);

	} else if (resp.GetByte(0) == 0x87 || resp.GetByte(0) == 0x81) {
		resp.Chop(2);
		int mac_offset = resp.Size() - 8;
		int lcg = resp.GetByte(1) == 0x81 ? resp.GetByte(2) : resp.GetByte(1);
		CByteArray inputForMac = resp.GetByte(1) == 0x81 ? resp.GetBytes(0, lcg + 3) : resp.GetBytes(0, lcg + 2);
		receivedMac = (resp.GetBytes(mac_offset, 8));
		paddedInput = paddedByteArray(inputForMac);
	}

	// Pre-increment SSC
	m_ssc++;

	CByteArray mac = sm_retail_mac_des(macKey, paddedInput, m_ssc);
	return mac.Equals(receivedMac);
}

} // namespace eIDMW
