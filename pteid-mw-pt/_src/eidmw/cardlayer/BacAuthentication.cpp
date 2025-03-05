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
#include <openssl/rand.h>
#include <openssl/sha.h>

namespace eIDMW {

using namespace Crypto;

#define MAC_LEN 8

BacAuthentication::BacAuthentication(SCARDHANDLE hCard, CContext *poContext, const void *paramStructure)
	: SecureMessaging(hCard, poContext, paramStructure) {}

BacAuthentication::~BacAuthentication() {}

void BacAuthentication::authenticate(const CByteArray &mrzInfo) {
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
					  "Returned data from mutual authentication does not match icc bac data size");
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

CByteArray BacAuthentication::decryptData(const CByteArray &data) {
	if (!m_authenticated) {
		LOG_AND_THROW(LEV_ERROR, MOD_CAL, EIDMW_ERR_BAC_NOT_INITIALIZED, "BAC not initialized");
	}

	if (data.GetByte(0) != 0x87) {
		return data;
	}

	CByteArray encryptionKey = m_sm.getCBAEncKey();

	// Either a) length is byte1 or b) byte1 is 0x81 and length is encoded on the next byte
	bool hasEncodedLength = data.GetByte(1) == 0x81;
	size_t dataLength = hasEncodedLength ? data.GetByte(2) : data.GetByte(1);

	size_t encDataOffset = 3 + hasEncodedLength;
	CByteArray cryptogram(data.GetBytes(encDataOffset, dataLength - 1)); // -1 for padding indicator
	return BlockCipherCtx::decrypt<TripleDesCipher>(encryptionKey.GetBytes(), cryptogram);
}

CByteArray BacAuthentication::sendSecureAPDU(const APDU &apdu, long &retValue) {
	return sendSecureAPDU(apdu.ToByteArray(), retValue);
};

CByteArray BacAuthentication::sendSecureAPDU(const CByteArray &apdu, long &retValue) {
	if (!m_authenticated) {
		LOG_AND_THROW(LEV_ERROR, MOD_CAL, EIDMW_ERR_BAC_NOT_INITIALIZED, "BAC not initialized");
	}

	CByteArray encryptionKey = m_sm.getCBAEncKey();
	CByteArray macKey = m_sm.getCBAMacKey();

	CByteArray final_apdu;
	const unsigned char Tcg = 0x87;
	const unsigned char Tcc = 0x8e;
	const unsigned char Tle = 0x97;
	const unsigned char paddingIndicator = 0x01;

	const unsigned char controlByte = 0x0C;

	CByteArray TlvLe;
	TlvLe.Append(Tle);
	TlvLe.Append(0x01);
	TlvLe.Append(0x00);

	CByteArray response;
	CByteArray mac;

	// Contains data
	if (apdu.Size() > 5) {
		CByteArray command_header(apdu.GetBytes(0, 4));
		// Mark the APDU as SM
		command_header.SetByte(command_header.GetByte(0) | controlByte, 0);

		CByteArray input_data(apdu.GetBytes(5, apdu.Size() - 6));
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
		paddedCryptogram.Append(TlvLe);

		paddedCryptogram = withIso7816Padding(paddedCryptogram);

		// Padded Command Header + Padded [Tcg Lcg PI,CG]
		inputForMac.Append(paddedCryptogram);

		// Pre-increment SSC
		m_sm.incrementSSC();

		mac = retailMacWithSSC(inputForMac, m_sm.getSSC());

		// Build final APDU including Cryptogram and MAC
		// int lc_final = MAC_LEN + 2 + cryptogram.Size() + 3; // CG + Tcg + Lcg + PI
		int lc_final = 3 + cryptogram.Size() + TlvLe.Size() + 2 + MAC_LEN;
		final_apdu.Append(command_header);
		final_apdu.Append((unsigned char)lc_final);
		final_apdu.Append(Tcg);
		final_apdu.Append((unsigned char)lcg);
		final_apdu.Append(paddingIndicator);
		final_apdu.Append(cryptogram);
		final_apdu.Append(TlvLe);
		final_apdu.Append(Tcc);
		final_apdu.Append(MAC_LEN); // Lcc
		final_apdu.Append(mac);
		final_apdu.Append(0x00); // Le

		response = sendAPDU(final_apdu, retValue);
	} else {
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
		m_sm.incrementSSC();

		mac = retailMacWithSSC(inputForMac, m_sm.getSSC());

		int lc_final = MAC_LEN + 2 + TlvLe.Size();
		final_apdu.Append(command_header);
		final_apdu.Append((unsigned char)lc_final);
		final_apdu.Append(TlvLe);
		final_apdu.Append(Tcc);
		final_apdu.Append(MAC_LEN); // Lcc
		final_apdu.Append(mac);
		final_apdu.Append(0x00); // Le

		response = sendAPDU(final_apdu, retValue);
	}

	if (response.GetByte(0) == 0x87) {
		auto status = response.GetBytes(response.Size() - 2, 2);

		size_t enc_length = response.GetByte(1);
		auto decrypted = removeIso7816Padding(decryptData(response));

		// check if MAC
		if (!checkMacInResponse(response)) {
			MWLOG(tLevel::LEV_ERROR, tModule::MOD_CAL, "Mac check failed for [APDU] with [MAC]:\n APDU: %s\n MAC: %s\n",
				  response.ToString(true, false).c_str(), mac.ToString(true, false).c_str());
		}

		auto response = decrypted;
		response.Append(status);
		return response;
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
		paddedInput = withIso7816Padding(inputForMac);
	}

	// Response contains data
	else if (resp.GetByte(0) == 0x87) {
		int macOffset = resp.Size() - 10; // 2 for sw, 8 for mac size

		bool hasEncSize = resp.GetByte(1) == 0x81;
		size_t dataLength = hasEncSize ? resp.GetByte(2) : resp.GetByte(1);

		receivedMac = resp.GetBytes(macOffset, 8);
		CByteArray inputForMac = resp.GetBytes(0, macOffset - 2); // From the start, up until mac tag
		paddedInput = withIso7816Padding(inputForMac);
	}

	// Pre-increment SSC
	m_sm.incrementSSC();

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
