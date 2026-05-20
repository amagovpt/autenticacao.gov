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
#include <openssl/asn1.h>
#include <openssl/types.h>
#include <eac/eac.h>

namespace eIDMW {

const unsigned char Tcg = 0x87;
const unsigned char TcgOdd = 0x85;
const unsigned char Tcc = 0x8e;
const unsigned char Tle = 0x97;
const unsigned char paddingIndicator = 0x01;

const unsigned char controlByte = 0x0C;

SecureMessaging::SecureMessaging(PTEID_CardHandle hCard, CContext *poContext)
	: m_card(hCard), m_context(poContext), m_authenticated(false) {
	EAC_init();
}

CByteArray SecureMessaging::sendSecureAPDU(const APDU &apdu, long &retValue) {
	return sendSecureAPDU(apdu.ToByteArray(), retValue);
}

CByteArray SecureMessaging::sendAPDU(const CByteArray &apdu, long &retValue) {
	return m_context->m_oCardInterface->Transmit(m_card, apdu, &retValue, m_context->getProtocol(m_card));
}

CByteArray SecureMessaging::sendSecureAPDU(const CByteArray &apdu, long &retValue) {
	CByteArray encryptedAPDU;
	CByteArray mac;

	bool isInsOdd = apdu.GetByte(1) & 1; // whether or not the instruction byte is odd

	bool isExtended = (apdu.Size() >= 7 && apdu.GetByte(4) == 0x00);
	uint32_t headerLen = isExtended ? 7 : 5; // 4-byte header + length field size
	uint32_t lc = 0;
	uint32_t dataOffset = headerLen;
	uint32_t le = 0;
	bool hasLe = false;
	bool hasData = false;

	if (apdu.Size() > 4) {
		// first length field after the header
		uint32_t firstField = isExtended
			? (apdu.GetByte(5) << 8) | apdu.GetByte(6)
			: apdu.GetByte(4);

		uint32_t remaining = apdu.Size() - headerLen;

		if (remaining == 0) {
			// no bytes after the field. it encodes Le (no command data)
			le = firstField;
			hasLe = true;
		} else {
			// bytes follow, the field encodes Lc, data starts at headerLen
			lc = firstField;
			hasData = true;

			// check if Le is appended after the data
			if (remaining > lc) {
				hasLe = true;
				if (isExtended)
					le = (apdu.GetByte(headerLen + lc) << 8) | apdu.GetByte(headerLen + lc + 1);
				else
					le = apdu.GetByte(apdu.Size() - 1);
			}
		}
	}

	// command header (first 4 bytes with SM control byte set)
	auto commandHeader = apdu.GetBytes(0, 4);
	commandHeader.SetByte(commandHeader.GetByte(0) | controlByte, 0);
	auto paddedCommandHeader = addPadding(commandHeader);

	// Build Le TLV: 1-byte Le for short, 2-byte Le for extended
	CByteArray tlvLe;
	if (hasLe) {
		tlvLe.Append(Tle);
		if (isExtended) {
			tlvLe.Append(0x02);
			tlvLe.Append((unsigned char)(le >> 8));
			tlvLe.Append((unsigned char)(le & 0xFF));
		} else {
			tlvLe.Append(0x01);
			tlvLe.Append((unsigned char)le);
		}
	}

	incrementSSC();

	if (hasData) {
		// encryption
		CByteArray data = apdu.GetBytes(dataOffset, lc);
		auto paddedData = addPadding(data);
		auto encryptedData = encryptData(paddedData);
		uint32_t lcg = encryptedData.Size() + (isInsOdd ? 0 : 1);

		CByteArray cryptogram;
		// TLV-encoding of cryptogram
		cryptogram.Append(isInsOdd ? TcgOdd : Tcg);
		if (lcg >= 256) {
			cryptogram.Append(0x82);
			cryptogram.Append((unsigned char)(lcg >> 8));
			cryptogram.Append((unsigned char)(lcg & 0xFF));
		} else if (lcg >= 128) {
			cryptogram.Append(0x81);
			cryptogram.Append((unsigned char)lcg);
		} else {
			cryptogram.Append((unsigned char)lcg);
		}
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

		// Build wrapped APDU: header + Lc + cryptogram + MAC TLV + Le
		uint32_t wrappedLc = cryptogram.Size() + 2 + mac.Size();
		encryptedAPDU.Append(commandHeader);
		if (isExtended) {
			encryptedAPDU.Append(0x00); // extended Lc marker
			encryptedAPDU.Append((unsigned char)(wrappedLc >> 8));
			encryptedAPDU.Append((unsigned char)(wrappedLc & 0xFF));
		} else {
			encryptedAPDU.Append((unsigned char)wrappedLc);
		}
		encryptedAPDU.Append(cryptogram);
	} else { // apdu does not have data (e.g. READ BINARY)
		// mac calculation
		CByteArray inputForMac;
		inputForMac.Append(paddedCommandHeader);
		inputForMac.Append(addPadding(tlvLe));
		mac = computeMac(inputForMac);

		uint32_t wrappedLc = tlvLe.Size() + 2 + mac.Size();
		encryptedAPDU.Append(commandHeader);
		if (isExtended) {
			encryptedAPDU.Append(0x00); // extended Lc marker
			encryptedAPDU.Append((unsigned char)(wrappedLc >> 8));
			encryptedAPDU.Append((unsigned char)(wrappedLc & 0xFF));
		} else {
			encryptedAPDU.Append((unsigned char)wrappedLc);
		}
		encryptedAPDU.Append(tlvLe);
	}

	encryptedAPDU.Append(Tcc);
	encryptedAPDU.Append(mac.Size());
	encryptedAPDU.Append(mac);

	// Le for the wrapped APDU: extended (2 bytes) or short (1 byte)
	if (isExtended) {
		encryptedAPDU.Append(0x00);
	}
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
		// data pointer was advanced past the TLV header by ASN1_get_object
		startOfData = data - encryptedResponse.GetBytes();
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
