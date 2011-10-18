/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2009 FedICT.
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
#pragma once

#include "EmulationCard.h"

#include <map>

namespace eIDMW
{

typedef enum {
	FT_DF,
	FT_TRANSP_EF,
} tFileType;

typedef enum {
	AC_ALWAYS,
	AC_NEVER,
	AC_PIN,
} tAC;

typedef struct {
	tFileType type;
	std::string csPath;
	CByteArray oAID;
	tAC readAC;
	unsigned long ulReadPinRef;
	tAC updateAC;
	unsigned long ulUpdatePinRef;
	CByteArray data;
} tCardFile;

typedef enum
{
    PIN_ENC_ASCII,
    PIN_ENC_BCD,
    PIN_ENC_GP,     // Global platorm encoding e.g. "12345" -> {0x25, 0x12, 0x34, 0x5F, 0xFF, ... 0xFF}
} tPinEnc;

typedef struct {
	std::string csPath;
	bool bVerifyOK;
	unsigned long ulPinRef;
	unsigned char ucPad;
	unsigned long ulMinLen;
	unsigned long ulMaxLen;
	unsigned long ulStoredLen;
	tPinEnc encoding;
	unsigned char ucPadChar;
	unsigned long ulMaxPinTries;
	unsigned long ulPinTries;
	std::string csPin;
	std::string csPuk;
	unsigned long ulPukRef;
}
tCardPin;

typedef std::map <std::string, tCardFile> tFileMap;
typedef std::map <std::string, tCardPin> tPinMap;

class CFileSystemCard : public CEmulationCard
{
public:
	EIDMW_PCSCEMU_API CFileSystemCard();

	EIDMW_PCSCEMU_API virtual void PutDF(const std::string & csPath,
		const CByteArray & oAID = CByteArray());

	EIDMW_PCSCEMU_API virtual void PutTransparentEF(const std::string & csPath,
		const CByteArray & data,
		tAC readAC, unsigned long ulReadPinRef,
		tAC writeAC, unsigned long ulWritePinRef);
	EIDMW_PCSCEMU_API virtual void PutTransparentEF(const std::string & csPath,
		const std::string & csHexData,
		tAC readAC, unsigned long ulReadPinRef,
		tAC writeAC, unsigned long ulWritePinRef);

	EIDMW_PCSCEMU_API virtual void PutPin(const std::string & csPath,
		unsigned long ulPinRef,	const std::string &csPin,
		unsigned long ulMinLen, unsigned long ulMaxLen, unsigned long ulStoredLen,
		tPinEnc encoding, unsigned long ulMaxPinTries, unsigned char ucPadChar,
		const std::string & csPuk, unsigned long ulPukRef);

	virtual LONG SCardTransmitInternal(
		IN LPCBYTE pbSendBuffer,
		IN DWORD cbSendLength,
		OUT LPBYTE pbRecvBuffer,
		IN OUT LPDWORD pcbRecvLength);

protected:
	virtual CByteArray SelectFile(const CByteArray & oAPDU);

	virtual CByteArray VerifyPin(const CByteArray & oAPDU);

	virtual CByteArray ReadBinary(const CByteArray & oAPDU);

	virtual CByteArray UpdateBinary(const CByteArray & oAPDU);

	virtual CByteArray GenRandom(const CByteArray & oAPDU);

	//CFileSystemCard(const CByteArray & oATR);
	virtual CByteArray ToBin(const std::string & csHexData);
	virtual std::string MakePinKey(const std::string & csPath, unsigned long ulPinRef);
	virtual CByteArray GetFCI(const tCardFile & oCardFile);
	virtual CByteArray MakePinBuf(const std::string & csPin, const tCardPin & oCardPin);
	virtual tCardPin *FindPin(unsigned long ulPinRef);
	/** Return true if csPath1 is equal to or a subdir of csPath2 */
	virtual bool IsSubDir(const std::string &csPath1, const std::string &csPath2);

	tFileMap m_files;
	tPinMap m_pins;
	std::string m_csCurrentDF;
	std::string m_csCurrentEF;
};

}

