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
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <memory>
#include <openssl/des.h>
#include <openssl/rand.h>
#include <openssl/sha.h>

namespace eIDMW {

using namespace Crypto;

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

	CByteArray iccBacData;
	iccBacData.Resize(40);

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
	iccBacData = sendAPDU(mutual_auth, retValue);

	if (iccBacData.GetByte(iccBacData.Size() - 2) != 0x90 || iccBacData.GetByte(iccBacData.Size() - 1) != 0x00) {
		LOG_AND_THROW(LEV_ERROR, MOD_CAL, EIDMW_ERR_BAC_NOT_INITIALIZED, "Failed to perform mutual authentication");
	}

	iccBacData.Chop(2); // Remove the status code
	if (iccBacData.Size() != sizeof(ifdBacData)) {
		LOG_AND_THROW(LEV_ERROR, MOD_CAL, EIDMW_ERR_BAC_NOT_INITIALIZED,
					  "Returned data from mutual authentication does not match icc bac data size");
	}

	// --------------------
	// Check ICC and store derived keys
	const size_t MAC_OFFSET = iccBacData.Size() - MAC_KEYSIZE;

	auto key = CByteArray(bacKeys.kMac.data(), bacKeys.kMac.size());
	auto data = CByteArray(iccBacData.GetBytes(), iccBacData.Size() - MAC_KEYSIZE);
	auto mac = retailMacWithPadding(key, data);

	const unsigned char *iccMac = iccBacData.GetBytes() + MAC_OFFSET;
	if (memcmp(mac.GetBytes(), iccMac, MAC_KEYSIZE) != 0) {
		LOG_AND_THROW(LEV_ERROR, MOD_CAL, EIDMW_ERR_BAC_NOT_INITIALIZED, "Failed to verify ICC MAC");
	}

	auto decryptedIccData = BlockCipherCtx::decrypt<TripleDesCipher>(bacKeys.kEnc.data(), iccBacData);

	if (memcmp(bacKeys.rndIcc.data(), decryptedIccData.GetBytes(), 8) != 0 ||
		memcmp(bacKeys.rndIfd.data(), decryptedIccData.GetBytes() + 8, 8) != 0) {
		LOG_AND_THROW(LEV_ERROR, MOD_CAL, EIDMW_ERR_BAC_NOT_INITIALIZED, "Failed to verify Rnd ICC or IFD");
	}

	// Store keys
	memcpy(bacKeys.kicc.data(), decryptedIccData.GetBytes() + 16, 16);

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

	return BlockCipherCtx::decrypt<TripleDesCipher>(encryptionKey.GetBytes(), cryptogram);
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
		CByteArray paddedInput = withIso7816Padding(input_data);
		auto cryptogram = BlockCipherCtx::encrypt<TripleDesCipher>(encryptionKey.GetBytes(), paddedInput);
		// LCg = Len(Cg) + Len(PI = 1)
		int lcg = cryptogram.Size() + 1;
		CByteArray inputForMac = withIso7816Padding(command_header);

		CByteArray paddedCryptogram;
		paddedCryptogram.Append(Tcg);
		// This is safe because Lcg will be always lower than
		paddedCryptogram.Append((unsigned char)lcg);
		paddedCryptogram.Append(paddingIndicator);
		paddedCryptogram.Append(cryptogram);

		paddedCryptogram = withIso7816Padding(paddedCryptogram);

		// Padded Command Header + Padded [Tcg Lcg PI,CG]
		inputForMac.Append(paddedCryptogram);

		// Pre-increment SSC
		m_ssc++;

		auto mac = retailMacWithSSC(inputForMac, m_ssc);

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
		CByteArray inputForMac = withIso7816Padding(command_header);

		TlvLe.Append(Tle);
		TlvLe.Append(0x01);
		TlvLe.Append(le);

		inputForMac.Append(withIso7816Padding(TlvLe));

		// Pre-increment SSC
		m_ssc++;

		auto mac = retailMacWithSSC(inputForMac, m_ssc);

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

	if (!checkMacInResponse(response)) {
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
	auto encrypted = BlockCipherCtx::encrypt<TripleDesCipher>(bacKeys.kEnc.data(), {enc_input, sizeof(enc_input)});
	std::memcpy(bac_data, encrypted.GetBytes(), encrypted.Size());

	auto key = CByteArray(bacKeys.kMac.data(), bacKeys.kMac.size());
	auto data = CByteArray(bac_data, encrypted.Size());
	auto mac = retailMacWithPadding(key, data);

	memcpy(bac_data + 32, mac.GetBytes(), MAC_KEYSIZE);

	return bacKeys;
}

bool BacAuthentication::checkMacInResponse(CByteArray &resp) {
	CByteArray receivedMac;
	CByteArray paddedInput;

	/*
		Deal with 2 different formats of response APDUs: with just SW12 or also with data
	*/
	if (resp.GetByte(0) == 0x99) {
		receivedMac = (resp.GetBytes(6, 8));
		CByteArray inputForMac = resp.GetBytes(0, 4);
		paddedInput = withIso7816Padding(inputForMac);

	} else if (resp.GetByte(0) == 0x87 || resp.GetByte(0) == 0x81) {
		resp.Chop(2);
		int mac_offset = resp.Size() - 8;
		int lcg = resp.GetByte(1) == 0x81 ? resp.GetByte(2) : resp.GetByte(1);
		CByteArray inputForMac = resp.GetByte(1) == 0x81 ? resp.GetBytes(0, lcg + 3) : resp.GetBytes(0, lcg + 2);
		receivedMac = (resp.GetBytes(mac_offset, 8));
		paddedInput = withIso7816Padding(inputForMac);
	}

	// Pre-increment SSC
	m_ssc++;

	auto mac = retailMacWithSSC(paddedInput, m_ssc);
	return mac.Equals(receivedMac);
}

CByteArray BacAuthentication::retailMacWithSSC(const CByteArray &macInput, uint64_t ssc) {
	unsigned char ssc_block[] = {0, 0, 0, 0, 0, 0, 0, 0};
	// SSC is an 8-byte counter that serves as the first block of the MAC input
	for (size_t i = 0; i < sizeof(ssc_block); i++)
		ssc_block[i] = (0xFF & ssc >> (7 - i) * 8);

	CByteArray macInputWithSSC;
	macInputWithSSC.Append(ssc_block, sizeof(ssc_block));
	macInputWithSSC.Append(macInput);
	if (macInputWithSSC.Size() > SIZE_MAX - MAC_KEYSIZE) {
		LOG_AND_THROW(LEV_ERROR, MOD_CAL, EIDMW_ERR_BAC_CRYPTO_ERROR, "%s: mac input too large", __FUNCTION__);
	}

	auto key = CByteArray{m_smKeys.ksMac.data(), m_smKeys.ksMac.size()};
	return BlockCipherCtx::retailMac(key, macInputWithSSC);
}

CByteArray BacAuthentication::retailMacWithPadding(const CByteArray &key, const CByteArray &macInput) {
	if (macInput.Size() % MAC_KEYSIZE != 0) {
		LOG_AND_THROW(LEV_ERROR, MOD_CAL, EIDMW_ERR_BAC_NOT_INITIALIZED,
					  "%s: Input data is not multiple of cipher block size (MAC_KEYSIZE: $ld)", __FUNCTION__,
					  MAC_KEYSIZE);
	}

	unsigned char padding[] = {0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

	// Padding method 2 will always add some padding bytes
	size_t inputLen = macInput.Size() + MAC_KEYSIZE;
	auto msg = std::make_unique<unsigned char[]>(inputLen);

	memcpy(msg.get(), macInput.GetBytes(), macInput.Size());
	memcpy(msg.get() + macInput.Size(), padding, sizeof(padding));

	return BlockCipherCtx::retailMac(key, {msg.get(), inputLen});
}

} // namespace eIDMW
