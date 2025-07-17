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

#include "SecureMessaging.h"
#include "APDU.h"
#include "ByteArray.h"
#include "Log.h"
#include <cstring>
#include <fcntl.h>
#include <openssl/asn1.h>
#include <openssl/types.h>

namespace eIDMW {

const unsigned char Tcg = 0x87;
const unsigned char TcgOdd = 0x85;
const unsigned char Tcc = 0x8e;
const unsigned char Tle = 0x97;
const unsigned char paddingIndicator = 0x01;

const unsigned char controlByte = 0x0C;

CByteArray SecureMessaging::sendSecureAPDU(const APDU &apdu, long &retValue) {
	return sendSecureAPDU(apdu.ToByteArray(), retValue);
}

CByteArray SecureMessaging::sendAPDU(const CByteArray &apdu, long &retValue) {
	return m_context->m_oCardInterface->Transmit(m_card, apdu, &retValue, m_param);
}

CByteArray SecureMessaging::sendSecureAPDU(const CByteArray &apdu, long &retValue) {
	CByteArray encryptedAPDU;
	CByteArray mac;

	uint32_t lc = apdu.GetByte(4);		 // length of command data
	bool isInsOdd = apdu.GetByte(1) & 1; // whether or not the instruction byte is odd

	// command header
	auto commandHeader = apdu.GetBytes(0, 4);
	commandHeader.SetByte(commandHeader.GetByte(0) | controlByte, 0);
	auto paddedCommandHeader = addPadding(commandHeader);

	CByteArray tlvLe;
	if ((commandHeader.Size() + lc + 2 == apdu.Size()) || apdu.Size() == 5) {
		tlvLe.Append(Tle);
		tlvLe.Append(0x01);
		tlvLe.Append(apdu.GetByte(apdu.Size() - 1)); // add Le byte
	}

	incrementSSC();

	// apdu has data
	if (apdu.Size() > 5) {
		// encryption
		CByteArray data = apdu.GetBytes(5, apdu.Size() - 5 - (tlvLe.Size() > 0));
		auto paddedData = addPadding(data);
		auto encryptedData = encryptData(paddedData);
		uint8_t lcg = encryptedData.Size() + (isInsOdd ? 0 : 1);

		CByteArray cryptogram;
		//TLV-encoding of cryptogram
		cryptogram.Append(isInsOdd ? TcgOdd : Tcg);
		if (lcg >= 128) {
			cryptogram.Append(0x81);
		}
		cryptogram.Append(lcg);
		if (!isInsOdd)
			cryptogram.Append(paddingIndicator);
		cryptogram.Append(encryptedData);
		cryptogram.Append(tlvLe);
		auto paddedCryptogram = addPadding(cryptogram);

		// mac calculation
		CByteArray inputForMac;
		inputForMac.Append(paddedCommandHeader);
		inputForMac.Append(paddedCryptogram);
		mac = computeMac(inputForMac);

		// cryptogram || tcc || mac size || mac || le
		uint8_t lc = cryptogram.Size() + 2 + mac.Size();
		encryptedAPDU.Append(commandHeader);
		encryptedAPDU.Append(lc);
		encryptedAPDU.Append(cryptogram);
	} else { // apdu does not have data
		// mac calculation
		CByteArray inputForMac;
		inputForMac.Append(paddedCommandHeader);
		inputForMac.Append(addPadding(tlvLe));
		mac = computeMac(inputForMac);

		uint8_t lc = tlvLe.Size() + 2 + mac.Size();
		encryptedAPDU.Append(commandHeader);
		encryptedAPDU.Append(lc);
		encryptedAPDU.Append(tlvLe);
	}

	encryptedAPDU.Append(Tcc);
	encryptedAPDU.Append(mac.Size());
	encryptedAPDU.Append(mac);
	encryptedAPDU.Append(0x00);

	incrementSSC();

	auto response = sendAPDU(encryptedAPDU, retValue);
	return decryptAPDUResponse(response);
};

CByteArray SecureMessaging::decryptAPDUResponse(const CByteArray &encryptedResponse) {
	if (encryptedResponse.Size() <= 2) {
		MWLOG(LEV_ERROR, MOD_CAL, "Secure Messaging error");
		return encryptedResponse;
	}

	bool isOdd = encryptedResponse.GetByte(0) == TcgOdd;
	uint64_t startOfData = 0;
	CByteArray encryptedData;
	CByteArray inputForMac;

	if (encryptedResponse.GetByte(0) == Tcg || encryptedResponse.GetByte(0) == TcgOdd) {
		auto data = encryptedResponse.GetBytes();
		long sizeData = 0;
		int xclass = 0;
		int asn1Tag = 0;
		ASN1_get_object(&data, &sizeData, &asn1Tag, &xclass, encryptedResponse.Size());
		auto result =
			strstr(reinterpret_cast<const char *>(encryptedResponse.GetBytes()), reinterpret_cast<const char *>(data));
		startOfData = reinterpret_cast<const unsigned char *>(result) - encryptedResponse.GetBytes();
		encryptedData = encryptedResponse.GetBytes(startOfData, sizeData);
	}

	uint32_t indexStatusCode = 2 + (encryptedData.Size() > 0 ? (encryptedData.Size() + startOfData) : 0);
	auto status = encryptedResponse.GetBytes(indexStatusCode, 2);

	if (encryptedData.Size() > 0) {
		inputForMac.Append(encryptedResponse.GetBytes(0, startOfData));
		inputForMac.Append(encryptedData);
	}

	if (status.Size() > 0) {
		inputForMac.Append(0x99);
		inputForMac.Append(status.Size());
		inputForMac.Append(status);
	}

	auto responseMac = computeMac(addPadding(inputForMac));

	uint64_t macIndex = inputForMac.Size() + 2;
	auto mac = encryptedResponse.GetBytes(macIndex, encryptedResponse.GetByte(macIndex - 1));

	if (memcmp(responseMac.GetBytes(), mac.GetBytes(), mac.Size()) != 0) {
		MWLOG(LEV_ERROR, MOD_CAL, "Response from encrypted APDU is invalid! APDU: %s",
			  encryptedResponse.ToString().c_str());
		return encryptedResponse;
	}

	CByteArray decrypted;
	if (encryptedData.Size() > 0) {
		CByteArray inputToDecrypt = CByteArray(encryptedData.Size() - (isOdd ? 0 : 1), 0);
		memcpy(inputToDecrypt.GetBytes(), &encryptedData.GetBytes()[isOdd ? 0 : 1], inputToDecrypt.Size());

		decrypted = decryptData(inputToDecrypt);
		decrypted = removePadding(decrypted);
	}

	decrypted.Append(status);

	return decrypted;
}

} // namespace eIDMW
