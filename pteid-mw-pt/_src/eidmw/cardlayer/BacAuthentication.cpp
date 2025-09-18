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
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <memory>
#include <openssl/des.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <openssl/sha.h>
#include <openssl/ssl.h>
#include <string>

namespace eIDMW {

using namespace Crypto;

#define MAC_LEN 8

BacAuthentication::BacAuthentication(PTEID_CardHandle hCard, CContext *poContext, PTEID_CardProtocol protocol)
	: SecureMessaging(hCard, poContext, protocol) {}

void BacAuthentication::authenticate(const CByteArray &mrzInfo) {

#ifdef _WIN32
	loadProviders();
#endif
	if (mrzInfo.Size() == 0) {
		LOG_AND_THROW(LEV_ERROR, MOD_CAL, EIDMW_ERR_BAC_INVALID_MRZ, "Invalid MRZ info. Size should be > 0");
	}

	long retValue = {0};

	CByteArray ifdBacData(40, 0);
	CByteArray iccBacData(40, 0);

	CByteArray rndIfd(8, 0);
	CByteArray rndIcc(8, 0);
	CByteArray keyIfd(16, 0);
	CByteArray keyIcc(16, 0);

	// --------------------
	// Generate bac data and derive session keys
	rndIcc = getRandomFromCard();
	RAND_bytes(rndIfd.GetBytes(), rndIfd.Size());
	RAND_bytes(keyIfd.GetBytes(), keyIfd.Size());

	auto hashMrz = MessageDigestCtx::hash<Sha1>(mrzInfo);
	hashMrz.Chop(hashMrz.Size() - 16);
	m_sm.deriveKeys(hashMrz);

	// --------------------
	// 3des encrypt mutual authentication data
	CByteArray encInput;
	encInput.Append(rndIfd);
	encInput.Append(rndIcc);
	encInput.Append(keyIfd);

	auto encKey = m_sm.getCBAEncKey();
	auto encrypted = BlockCipherCtx::encrypt<TripleDesCipher>(encKey.GetBytes(), encInput);
	memcpy(ifdBacData.GetBytes(), encrypted.GetBytes(), encrypted.Size());

	// retail mac
	auto mac = retailMacWithPadding(m_sm.getCBAMacKey(), encrypted);
	memcpy(ifdBacData.GetBytes() + 32, mac.GetBytes(), MAC_KEYSIZE);

	// --------------------
	// Send mutual authentication
	CByteArray mutual_auth;
	mutual_auth.Append(0x00);
	mutual_auth.Append(0x82);
	mutual_auth.Append(0x00);
	mutual_auth.Append(0x00);
	mutual_auth.Append(ifdBacData.Size());
	mutual_auth.Append(ifdBacData);
	mutual_auth.Append(0x00); // Le
	iccBacData = sendAPDU(mutual_auth, retValue);

	if (iccBacData.GetByte(iccBacData.Size() - 2) != 0x90 || iccBacData.GetByte(iccBacData.Size() - 1) != 0x00) {
		LOG_AND_THROW(LEV_ERROR, MOD_CAL, EIDMW_ERR_BAC_NOT_INITIALIZED, "Failed to perform mutual authentication");
	}

	iccBacData.Chop(2); // Remove the status code
	if (iccBacData.Size() != ifdBacData.Size()) {
		LOG_AND_THROW(LEV_ERROR, MOD_CAL, EIDMW_ERR_BAC_NOT_INITIALIZED,
					  "Returned data from mutual authentication does not match icc "
					  "bac data size");
	}

	// --------------------
	// Check ICC and store derived keys
	const size_t MAC_OFFSET = iccBacData.Size() - MAC_KEYSIZE;

	auto data = CByteArray(iccBacData.GetBytes(), iccBacData.Size() - MAC_KEYSIZE);
	mac = retailMacWithPadding(m_sm.getCBAMacKey(), data);

	const unsigned char *iccMac = iccBacData.GetBytes() + MAC_OFFSET;
	if (memcmp(mac.GetBytes(), iccMac, MAC_KEYSIZE) != 0) {
		LOG_AND_THROW(LEV_ERROR, MOD_CAL, EIDMW_ERR_BAC_NOT_INITIALIZED, "Failed to verify ICC MAC");
	}

	auto decryptedIccData = BlockCipherCtx::decrypt<TripleDesCipher>(encKey.GetBytes(), iccBacData);

	if (memcmp(rndIcc.GetBytes(), decryptedIccData.GetBytes(), 8) != 0 ||
		memcmp(rndIfd.GetBytes(), decryptedIccData.GetBytes() + 8, 8) != 0) {
		LOG_AND_THROW(LEV_ERROR, MOD_CAL, EIDMW_ERR_BAC_NOT_INITIALIZED, "Failed to verify Rnd ICC or IFD");
	}

	// Store keys
	memcpy(keyIcc.GetBytes(), decryptedIccData.GetBytes() + 16, 16);

	// --------------------
	// Generate session keys and SSC
	unsigned char kseed[16] = {0}; // Kseed = XOR of Kicc and Kifd
	for (int i = 0; i < keyIfd.Size(); i++) {
		kseed[i] = keyIfd.GetByte(i) ^ keyIcc.GetByte(i);
	}
	m_sm.deriveKeys({kseed, sizeof(kseed)});

	CByteArray ssc(8);
	ssc.Append(rndIcc.GetBytes() + 4, 4);
	ssc.Append(rndIfd.GetBytes() + 4, 4);
	m_sm.setSSC(bigEndianBytesToLong(ssc.GetBytes(), ssc.Size()));

	m_authenticated = true;
}

BUF_MEM *bufMemFromByteArray(const CByteArray &array) {
	BUF_MEM *buf = BUF_MEM_new();
	if (buf == NULL) {
		// no memory
		return NULL;
	}

	buf->data = (char *)malloc(array.Size() * sizeof(char));
	if (buf->data == NULL) {
		// no memory
		return NULL;
	}
	memcpy(buf->data, (char *)array.GetBytes(), array.Size());
	buf->length = array.Size();
	buf->max = buf->length;

	return buf;
}

CByteArray BacAuthentication::decryptData(const CByteArray &data) {
	auto encryptionKey = m_sm.getCBAEncKey();
	return BlockCipherCtx::decrypt<TripleDesCipher>(encryptionKey.GetBytes(), data);
}

CByteArray arrayFromBufMem(BUF_MEM *mem) { return CByteArray((unsigned char *)mem->data, mem->length); }

CByteArray BacAuthentication::encryptData(const CByteArray &data) {
	auto encryptionKey = m_sm.getCBAEncKey();
	return BlockCipherCtx::encrypt<TripleDesCipher>(encryptionKey.GetBytes(), data);
}

CByteArray BacAuthentication::addPadding(const CByteArray &data) { return withIso7816Padding(data); }

CByteArray BacAuthentication::removePadding(const CByteArray &data) { return removeIso7816Padding(data); };

CByteArray BacAuthentication::computeMac(const CByteArray &data) { return retailMacWithSSC(data, m_sm.getSSC()); }

void BacAuthentication::incrementSSC() { m_sm.incrementSSC(); }

CByteArray BacAuthentication::getRandomFromCard() {
	long returnValue = {0};

	APDU apdu;
	apdu.ins() = 0x84;
	apdu.p1() = 0x00;
	apdu.p2() = 0x00;
	apdu.setLe(0x08);
	auto out = sendAPDU(apdu.ToByteArray(), returnValue);
	out.Chop(2);

	return out;
}

bool BacAuthentication::checkMacInResponse(CByteArray &resp) {
	CByteArray receivedMac;
	CByteArray paddedInput;

	// Response only contains the status
	if (resp.GetByte(0) == 0x99) {
		receivedMac = (resp.GetBytes(6, 8));
		CByteArray inputForMac = resp.GetBytes(0, 4);
		paddedInput = addPadding(inputForMac);
	}

	// Response contains data
	else if (resp.GetByte(0) == 0x87) {
		int macOffset = resp.Size() - 10; // 2 for sw, 8 for mac size

		bool hasEncSize = resp.GetByte(1) == 0x81;
		size_t dataLength = hasEncSize ? resp.GetByte(2) : resp.GetByte(1);

		receivedMac = resp.GetBytes(macOffset, 8);
		CByteArray inputForMac = resp.GetBytes(0, macOffset - 2); // From the start, up until mac tag
		paddedInput = addPadding(inputForMac);
	}

	auto mac = retailMacWithSSC(paddedInput, m_sm.getSSC());
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

	return BlockCipherCtx::retailMac(m_sm.getCBAMacKey(), macInputWithSSC);
}

CByteArray BacAuthentication::retailMacWithPadding(const CByteArray &key, const CByteArray &macInput) {
	if (macInput.Size() % MAC_KEYSIZE != 0) {
		LOG_AND_THROW(LEV_ERROR, MOD_CAL, EIDMW_ERR_BAC_NOT_INITIALIZED,
					  "%s: Input data is not multiple of cipher block size "
					  "(MAC_KEYSIZE: %ld)",
					  __FUNCTION__, MAC_KEYSIZE);
	}

	unsigned char padding[] = {0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

	// Padding method 2 will always add some padding bytes
	size_t inputLen = macInput.Size() + MAC_KEYSIZE;
	auto msg = std::make_unique<unsigned char[]>(inputLen);

	memcpy(msg.get(), macInput.GetBytes(), macInput.Size());
	memcpy(msg.get() + macInput.Size(), padding, sizeof(padding));

	return BlockCipherCtx::retailMac(key, {msg.get(), (unsigned long)inputLen});
}

/**
 * Calculate ICAO check digit as specified in ICAO doc 9303
 * @param mrz_field The MRZ field to calculate check digit for
 * @return Check digit (0-9)
 */
int icao_check_digit(const char *mrz_field) {
	int sum = 0;
	int weight = 0;
	const char *str = mrz_field;

	for (size_t i = 0; i < strlen(str); i++) {
		if (i % 3 == 0) {
			weight = 7;
		} else if (i % 3 == 1) {
			weight = 3;
		} else {
			weight = 1;
		}
		if (isdigit(str[i])) {
			sum += weight * (str[i] - '0');
		} else if (isalpha(str[i])) {
			sum += weight * (tolower(str[i]) - 'a' + 10);
		}
	}

	return sum % 10;
}

BacAuthentication::MrzInfo BacAuthentication::mrzFromBytes(const CByteArray &data) {
	size_t hex_len = data.Size() * 2 + 1;
	auto hex_string = std::vector<char>(hex_len);
	OPENSSL_buf2hexstr_ex(hex_string.data(), hex_len, NULL, data.GetBytes(), data.Size(), '\0');

	// Split the string in 3 fields to mimic the standard MRZ fields used in BAC
	std::string hex_string_str(hex_string.data()); // Convert to std::string first
	std::string field1 = hex_string_str.substr(0, 12);
	std::string field2 = hex_string_str.substr(12, 6);
	std::string field3 = hex_string_str.substr(18, 6);

	// Add check digits
	field1 += ('0' + icao_check_digit(field1.c_str()));
	field2 += ('0' + icao_check_digit(field2.c_str()));
	field3 += ('0' + icao_check_digit(field3.c_str()));

	std::string mrz_info_str = field1 + field2 + field3;
	return CByteArray(mrz_info_str);
}

} // namespace eIDMW
