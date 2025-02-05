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
#include "Crypto.h"
#include "Log.h"
#include "Util.h"
#include "eidErrors.h"
#include <array>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <memory>
#include <openssl/des.h>
#include <openssl/rand.h>
#include <openssl/sha.h>

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
	if (keySeedLen != 16) {
		LOG_AND_THROW(LEV_ERROR, MOD_CAL, EIDMW_ERR_BAC_NOT_INITIALIZED, "%s: Illegal input. Key seed must be 16 bytes",
					  __FUNCTION__);
	}

	std::array<unsigned char, SHA_DIGEST_LENGTH> kSeed = {0};
	std::array<unsigned char, SHA_DIGEST_LENGTH> kSeedDigest = {0};

	memcpy(kSeed.data(), keySeed16, 16);

	// Only the 16 most significant bytes are used from kseed so we can overwrite the last 4
	kSeed[SHA_DIGEST_LENGTH - 4] = 0x00;
	kSeed[SHA_DIGEST_LENGTH - 3] = 0x00;
	kSeed[SHA_DIGEST_LENGTH - 2] = 0x00;
	kSeed[SHA_DIGEST_LENGTH - 1] = 0x01;

	SHA1(kSeed.data(), SHA_DIGEST_LENGTH, kSeedDigest.data());
	memcpy(ksEnc.data(), kSeedDigest.data(), 16);

	kSeed[SHA_DIGEST_LENGTH - 1] = 0x02;

	SHA1(kSeed.data(), SHA_DIGEST_LENGTH, kSeedDigest.data());
	memcpy(ksMac.data(), kSeedDigest.data(), 16);
}

void BacAuthentication::BacKeys::generateAccessKeys(const CByteArray &mrzInfo) {
	assert(mrzInfo.Size() != 0 && "Empty MRZ when generating access keys");

	std::array<unsigned char, SHA_DIGEST_LENGTH> kSeed;
	std::array<unsigned char, SHA_DIGEST_LENGTH> kSeedDigest;

	SHA1(mrzInfo.GetBytes(), mrzInfo.Size(), kSeed.data());

	// Only the 16 most significant bytes are used from kseed so we can overwrite the last 4
	kSeed[SHA_DIGEST_LENGTH - 4] = 0x00;
	kSeed[SHA_DIGEST_LENGTH - 3] = 0x00;
	kSeed[SHA_DIGEST_LENGTH - 2] = 0x00;
	kSeed[SHA_DIGEST_LENGTH - 1] = 0x01;

	SHA1(kSeed.data(), SHA_DIGEST_LENGTH, kSeedDigest.data());
	memcpy(kEnc.data(), kSeedDigest.data(), 16);

	kSeed[SHA_DIGEST_LENGTH - 1] = 0x02;

	SHA1(kSeed.data(), SHA_DIGEST_LENGTH, kSeedDigest.data());
	memcpy(kMac.data(), kSeedDigest.data(), 16);
}

enum EncryptMode {
	Decrypt = 0,
	Encrypt,
};

std::array<unsigned char, MAC_KEYSIZE> retailMacDes(const unsigned char *key, const unsigned char *macInput,
													size_t macInputLen) {
	assert(key);
	assert(macInput);

	if (macInputLen % MAC_KEYSIZE != 0) {
		LOG_AND_THROW(LEV_ERROR, MOD_CAL, EIDMW_ERR_BAC_NOT_INITIALIZED,
					  "%s: Input data is not multiple of cipher block size (MAC_KEYSIZE: $ld)", __FUNCTION__,
					  MAC_KEYSIZE);
	}

	unsigned char xx[MAC_KEYSIZE];
	unsigned char padding[] = {0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

	DES_key_schedule ks_a;
	DES_key_schedule ks_b;
	const unsigned char *key_a = key;
	const unsigned char *key_b = key + 8;

	// Padding method 2 will always add some padding bytes
	size_t inputLen = macInputLen + MAC_KEYSIZE;
	auto msg = std::make_unique<unsigned char[]>(inputLen);

	memcpy(msg.get(), macInput, macInputLen);
	memcpy(msg.get() + macInputLen, padding, sizeof(padding));

	auto des_out = std::array<unsigned char, MAC_KEYSIZE>{};
	memset(des_out.data(), 0, MAC_KEYSIZE);

	// TODO: is doing DES unchecked a good idea??
	DES_set_key_unchecked((const_DES_cblock *)key_a, &ks_a);
	DES_set_key_unchecked((const_DES_cblock *)key_b, &ks_b);

	for (size_t j = 0; j < (inputLen / MAC_KEYSIZE); j++) {
		if (j >= SIZE_MAX / MAC_KEYSIZE) {
			LOG_AND_THROW(LEV_ERROR, MOD_CAL, EIDMW_ERR_BAC_CRYPTO_ERROR, "%s: Loop counter would overflow",
						  __FUNCTION__);
		}

		for (size_t i = 0; i < MAC_KEYSIZE; i++)
			xx[i] = msg[i + j * MAC_KEYSIZE] ^ des_out[i];

		DES_ecb_encrypt((const_DES_cblock *)xx, (DES_cblock *)des_out.data(), &ks_a, 1);
	}

	memcpy(xx, des_out.data(), MAC_KEYSIZE);
	DES_ecb_encrypt((const_DES_cblock *)xx, (DES_cblock *)des_out.data(), &ks_b, 0);

	memcpy(xx, des_out.data(), MAC_KEYSIZE);
	DES_ecb_encrypt((const_DES_cblock *)xx, (DES_cblock *)des_out.data(), &ks_a, 1);

	return des_out;
}

CByteArray smRetailMacDes(CByteArray &key, CByteArray &macInput, uint64_t ssc) {
	CByteArray secondKey = key.GetBytes(MAC_KEYSIZE, MAC_KEYSIZE);

	auto desOut = std::array<unsigned char, MAC_KEYSIZE>{0};
	unsigned char ssc_block[] = {0, 0, 0, 0, 0, 0, 0, 0};
	auto xx = std::array<unsigned char, MAC_KEYSIZE>{0};

	DES_key_schedule ks_a;
	DES_key_schedule ks_b;

	// SSC is an 8-byte counter that serves as the first block of the MAC input
	for (size_t i = 0; i < sizeof(ssc_block); i++)
		ssc_block[i] = (0xFF & ssc >> (7 - i) * 8);

	CByteArray macInputWithSSC;
	macInputWithSSC.Append(ssc_block, sizeof(ssc_block));
	macInputWithSSC.Append(macInput);

	if (macInputWithSSC.Size() > SIZE_MAX - MAC_KEYSIZE) {
		LOG_AND_THROW(LEV_ERROR, MOD_CAL, EIDMW_ERR_BAC_CRYPTO_ERROR, "%s: mac input too large", __FUNCTION__);
	}

	unsigned char *msg = macInputWithSSC.GetBytes();

	// TODO: again, des uncheced... maybe look into it
	DES_set_key_unchecked((const_DES_cblock *)key.GetBytes(), &ks_a);
	DES_set_key_unchecked((const_DES_cblock *)secondKey.GetBytes(), &ks_b);

	for (size_t j = 0; j < (macInputWithSSC.Size() / MAC_KEYSIZE); j++) {
		for (size_t i = 0; i < MAC_KEYSIZE; i++)
			xx[i] = msg[i + j * MAC_KEYSIZE] ^ desOut[i];

		DES_ecb_encrypt((const_DES_cblock *)xx.data(), (DES_cblock *)desOut.data(), &ks_a, 1);
	}

	memcpy(xx.data(), desOut.data(), MAC_KEYSIZE);
	DES_ecb_encrypt((const_DES_cblock *)xx.data(), (DES_cblock *)desOut.data(), &ks_b, 0);

	memcpy(xx.data(), desOut.data(), MAC_KEYSIZE);
	DES_ecb_encrypt((const_DES_cblock *)xx.data(), (DES_cblock *)desOut.data(), &ks_a, 1);

	CByteArray result(desOut.data(), MAC_KEYSIZE);
	return result;
}

BacAuthentication::BacAuthentication(CContext *poContext) : m_context(poContext) {}
BacAuthentication::~BacAuthentication() {}

void BacAuthentication::authenticate(SCARDHANDLE hCard, const void *paramStructure, const CByteArray &mrzInfo) {
	if (mrzInfo.Size() == 0) {
		LOG_AND_THROW(LEV_ERROR, MOD_CAL, EIDMW_ERR_BAC_INVALID_MRZ, "Invalid MRZ info. Size should be > 0");
	}

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

	if (resp.Size() - 2 != sizeof(iccBacData)) {
		LOG_AND_THROW(LEV_ERROR, MOD_CAL, EIDMW_ERR_BAC_NOT_INITIALIZED,
					  "Returned data from mutual authentication does not match icc bac data size");
	}

	if (resp.GetByte(resp.Size() - 2) != 0x90 || resp.GetByte(resp.Size() - 1) != 0x00) {
		LOG_AND_THROW(LEV_ERROR, MOD_CAL, EIDMW_ERR_BAC_NOT_INITIALIZED, "Failed to perform mutual authentication");
	}

	// Save response
	memcpy(iccBacData, resp.GetBytes(), resp.Size() - 2);

	// --------------------
	// Check ICC and store derived keys
	unsigned char decryptedIccData[40] = {0};
	const size_t MAC_OFFSET = sizeof(iccBacData) - MAC_KEYSIZE;
	auto mac = retailMacDes(bacKeys.kMac.data(), iccBacData, sizeof(iccBacData) - MAC_KEYSIZE);
	const unsigned char *iccMac = iccBacData + MAC_OFFSET;
	if (memcmp(mac.data(), iccMac, MAC_KEYSIZE) != 0) {
		LOG_AND_THROW(LEV_ERROR, MOD_CAL, EIDMW_ERR_BAC_NOT_INITIALIZED, "Failed to verify ICC MAC");
	}

	auto decrypted = TripleDesCipher::decrypt(bacKeys.kEnc.data(), iccBacData, sizeof(decryptedIccData) - MAC_KEYSIZE);
	std::memcpy(decryptedIccData, decrypted.data(), decrypted.size());

	if (memcmp(bacKeys.rndIcc.data(), decryptedIccData, 8) != 0 ||
		memcmp(bacKeys.rndIfd.data(), decryptedIccData + 8, 8) != 0) {
		LOG_AND_THROW(LEV_ERROR, MOD_CAL, EIDMW_ERR_BAC_NOT_INITIALIZED, "Failed to verify Rnd ICC or IFD");
	}

	// Store keys
	memcpy(bacKeys.kicc.data(), decryptedIccData + 16, 16);

	// --------------------
	// Generate session keys and SSC
	unsigned char *rnd_icc = bacKeys.rndIcc.data();
	unsigned char *rnd_ifd = bacKeys.rndIfd.data();

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
	m_ssc = bigEndianBytesToLong(m_smKeys.ssc.data(), m_smKeys.ssc.size());
}

CByteArray BacAuthentication::decryptData(const CByteArray &data) {
	CByteArray encryptionKey = {m_smKeys.ksEnc.data(), m_smKeys.ksEnc.size()};
	CByteArray cryptogram(data.GetBytes(3, data.GetByte(1) - 1));

	auto decrypted = TripleDesCipher::decrypt(encryptionKey.GetBytes(), cryptogram.GetBytes(), cryptogram.Size());

	return {decrypted.data(), decrypted.size()};
}

CByteArray BacAuthentication::sendSecureAPDU(const CByteArray &apdu) {
	CByteArray encryptionKey = {m_smKeys.ksEnc.data(), m_smKeys.ksEnc.size()};
	CByteArray macKey = {m_smKeys.ksMac.data(), m_smKeys.ksMac.size()};

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

		auto encrypted = TripleDesCipher::encrypt(encryptionKey.GetBytes(), paddedInput.GetBytes(), paddedInput.Size());
		CByteArray cryptogram = {encrypted.data(), encrypted.size()};
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

		CByteArray mac = smRetailMacDes(macKey, inputForMac, m_ssc);

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

		CByteArray mac = smRetailMacDes(macKey, inputForMac, m_ssc);

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

	if (!checkMacInResponse(response, macKey)) {
		MWLOG(LEV_ERROR, MOD_CAL, "Failed to verifiy MAC on response");
	}

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

	BacKeys bacKeys = {0};
	bacKeys.generateAccessKeys(mrzInfo);

	// Copy random ICC to bac keys
	memcpy(bacKeys.rndIcc.data(), random.GetBytes(), random.Size());

	// Generate random IFD and KIFD
	RAND_bytes(bacKeys.rndIfd.data(), 8);
	RAND_bytes(bacKeys.kifd.data(), 16);

	unsigned char enc_input[32] = {0};
	memcpy(enc_input, bacKeys.rndIfd.data(), 8);
	memcpy(enc_input + 8, random.GetBytes(), 8);
	memcpy(enc_input + 16, bacKeys.kifd.data(), 16);

	// Encrypt SIFD = (RND.IFD || RND.ICC || KIFD) and generate MAC
	auto encrypted = TripleDesCipher::encrypt(bacKeys.kEnc.data(), enc_input, sizeof(enc_input));
	std::memcpy(bac_data, encrypted.data(), encrypted.size());

	auto mac = retailMacDes(bacKeys.kMac.data(), bac_data, encrypted.size());
	memcpy(bac_data + 32, mac.data(), MAC_KEYSIZE);

	return bacKeys;
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

	CByteArray mac = smRetailMacDes(macKey, paddedInput, m_ssc);
	return mac.Equals(receivedMac);
}

} // namespace eIDMW
