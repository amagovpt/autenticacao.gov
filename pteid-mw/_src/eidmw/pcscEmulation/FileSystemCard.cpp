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
#include "FileSystemCard.h"
#include <time.h>

using namespace eIDMW;

CFileSystemCard::CFileSystemCard()
{
	PutDF("3F00");
	m_csCurrentDF = "3F00";
}

void CFileSystemCard::PutDF(const std::string & csPath, const CByteArray & oAID)
{
	tCardFile cardFile;

	cardFile.type = FT_DF;
	cardFile.csPath = csPath;
	cardFile.readAC = AC_ALWAYS;
	cardFile.ulReadPinRef = 0;
	cardFile.updateAC = AC_NEVER;
	cardFile.ulUpdatePinRef = 0;
	cardFile.oAID = oAID;

	m_files[csPath] = cardFile;
}

void CFileSystemCard::PutTransparentEF(const std::string & csPath,
	const std::string & csHexData,
	tAC readAC, unsigned long ulReadPinRef,
	tAC updateAC, unsigned long ulUpdatePinRef)
{
	PutTransparentEF(csPath, ToBin(csHexData),
		readAC, ulReadPinRef, updateAC, ulUpdatePinRef);
}

std::string CFileSystemCard::MakePinKey(const std::string & csPath, unsigned long ulPinRef)
{
	char csTmp[20];
	csTmp[0] = '_';
	for (int i = 0; ulPinRef != 0; i++)
	{
		csTmp[i] = static_cast<unsigned char> (ulPinRef % 256);
		ulPinRef /= 256;
	}

	return csPath + csTmp;
}

void CFileSystemCard::PutPin(const std::string & csPath,
	unsigned long ulPinRef,	const std::string &csPin,
	unsigned long ulMinLen, unsigned long ulMaxLen, unsigned long ulStoredLen,
	tPinEnc encoding, unsigned long ulMaxPinTries, unsigned char ucPadChar,
	const std::string & csPuk, unsigned long ulPukRef)
{
	tCardPin pin;

	pin.csPath = csPath;
	pin.bVerifyOK = false;
	pin.ulPinRef = ulPinRef;
	pin.ulMinLen = ulMinLen;
	pin.ulMaxLen = ulMaxLen;
	pin.ulStoredLen = ulStoredLen;
	pin.encoding = encoding;
	pin.ucPadChar = ucPadChar;
	pin.ulMaxPinTries = ulMaxPinTries;
	pin.ulPinTries = pin.ulMaxPinTries;
	pin.csPin = csPin;
	pin.csPuk = csPuk;
	pin.ulPukRef = ulPukRef;

	m_pins[MakePinKey(csPath, ulPinRef)] = pin;
}

////////////////////////////////////////////////////////////////////////

CByteArray CFileSystemCard::SelectFile(const CByteArray & oAPDU)
{
	tCardFile *poCardFile = NULL;

	unsigned char ucP1 = oAPDU.GetByte(2);
	unsigned char ucP2 = oAPDU.GetByte(3);
	unsigned char ucP3 = oAPDU.GetByte(4);

	std::string csPath;

	if (ucP3 < 2)
		return CByteArray(tucSW12WrongLengthP3, sizeof(tucSW12WrongLengthP3));

	if (ucP1 == 0x00) // Select by absolute path
	{
		if (ucP3 % 2 == 1)
			return CByteArray(tucSW12WrongLengthP3, sizeof(tucSW12WrongLengthP3));
		csPath = oAPDU.ToString(false, true, 5, ucP3);
	}
	else if (ucP1 == 0x01 || ucP1 == 0x02) // Select child DF resp. EF
	{
		if (ucP3 != 2)
			return CByteArray(tucSW12WrongLengthP3, sizeof(tucSW12WrongLengthP3));
		csPath = m_csCurrentDF + oAPDU.ToString(false, true, 5, ucP3);
	}
	else if (ucP1 == 0x08 || ucP1 == 0x09) // Select DF resp. EF under MF (by path)
	{
		if (ucP3 % 2 == 1)
			return CByteArray(tucSW12WrongLengthP3, sizeof(tucSW12WrongLengthP3));
		csPath = m_csCurrentDF + oAPDU.ToString(false, true, 5, ucP3);
	}

	if (csPath.size()  != 0)
	{
		tFileMap::iterator it = m_files.find(csPath);
		if (it == m_files.end())
			return CByteArray(tucSW12FileNotFound, sizeof(tucSW12FileNotFound));
		poCardFile = &(it->second);
	}
	else if (ucP1 == 0x04)
	{
		tFileMap::iterator it = m_files.begin();
		for ( ; it != m_files.end() && poCardFile == NULL; it++)
		{
			CByteArray oAID(oAPDU.GetBytes() + 5, ucP3);
			if (oAID.Equals(it->second.oAID))
			{
				poCardFile = &(it->second);
				csPath = poCardFile->csPath;
			}
		}
		if (poCardFile == NULL)
			return CByteArray(tucSW12FileNotFound, sizeof(tucSW12FileNotFound));
	}
	else
		return CByteArray(tucSW12IncorrectParamsP1P2, sizeof(tucSW12IncorrectParamsP1P2));

	// Update the current EF and DF
	if (poCardFile->type == FT_DF)
	{
		m_csCurrentDF = csPath;
		m_csCurrentEF = "";
	}
	else
	{
		m_csCurrentDF = csPath.substr(0, csPath.size() - 4);
		m_csCurrentEF = csPath;
	}

	// Update PIN status if we're went out of the PIN's DF
	tPinMap::iterator it = m_pins.begin();
	for ( ; it != m_pins.end(); it++)
	{
		if (!IsSubDir(m_csCurrentDF, it->second.csPath))
				it->second.bVerifyOK = false;
	}
	
	return ucP2 == 0x0C ? CByteArray(tucSW12OK, sizeof(tucSW12OK)) : GetFCI(*poCardFile);
}

CByteArray CFileSystemCard::GetFCI(const tCardFile & oCardFile)
{
	// Say by default P2 = 0x0C isn't allowed (no FCI to be returned);
	// this is de case for Belpic cards; but not for most others so
	// they have to implement this function in a subclass.
	return CByteArray(tucSW12IncorrectParamsP1P2, sizeof(tucSW12IncorrectParamsP1P2));
}

CByteArray CFileSystemCard::MakePinBuf(const std::string & csPin, const tCardPin & oCardPin)
{
	CByteArray oBuf(16);
    unsigned long i;
    unsigned long ulPinLen = (unsigned long) csPin.size();

	switch(oCardPin.encoding)
    {
    case PIN_ENC_ASCII:
        for (i = 0; i < ulPinLen; i++)
            oBuf.Append((unsigned char) csPin[i]);
        for ( ; i < oCardPin.ulStoredLen; i++)
            oBuf.Append(oCardPin.ucPadChar);
        break;
    case PIN_ENC_GP:
        oBuf.Append((unsigned char) (0x20 + ulPinLen));
        // no break here
    case PIN_ENC_BCD:
        i = 0;
        while (i < ulPinLen)
        {
            unsigned char uc = (unsigned char) (16 * (csPin[i] - '0'));
            i++;
            if (i < ulPinLen)
                uc += (unsigned char) (csPin[i] - '0');
            else
                uc += (unsigned char) (oCardPin.ucPadChar % 16);
            i++;
            oBuf.Append(uc);
        }
		while (oBuf.Size() < oCardPin.ulStoredLen)
            oBuf.Append((unsigned char)
                oCardPin.ucPadChar > 0x0F ? oCardPin.ucPadChar : oCardPin.ucPadChar % 16);
        break;
    default:
#ifdef WIN32
		throw std::exception("Invalid PIN encoding in CFileSystemCard::MakePinBuf()");
#else
		throw std::exception();
#endif
    }

	return oBuf;
}

CByteArray CFileSystemCard::VerifyPin(const CByteArray & oAPDU)
{
	unsigned char ucP1 = oAPDU.GetByte(2);
	unsigned char ucP2 = oAPDU.GetByte(3);
	unsigned char ucP3 = oAPDU.GetByte(4);

	if (ucP1 != 0x00)
		return CByteArray(tucSW12IncorrectParamsP1P2, sizeof(tucSW12IncorrectParamsP1P2));

	tCardPin *poCardPin = FindPin(ucP2);
	if (poCardPin == NULL)
		return CByteArray(tucSW12RefDataNotFound, sizeof(tucSW12RefDataNotFound));

	if (poCardPin->ulPinTries == 0)
		return CByteArray(tucSW12PinBlocked, sizeof(tucSW12PinBlocked));

	poCardPin->ulPinTries--;

	CByteArray oCardPinBuf = MakePinBuf(poCardPin->csPin, *poCardPin);
	CByteArray oPresentedPinBuf(oAPDU.GetBytes() + 5, ucP3);

	if (oCardPinBuf.Equals(oPresentedPinBuf))
	{
		poCardPin->ulPinTries = poCardPin->ulMaxPinTries;
		poCardPin->bVerifyOK = true;
		return CByteArray(tucSW12OK, sizeof(tucSW12OK));
	}

	tucSW12BadPin[1] = (unsigned char) (0xC0 + poCardPin->ulPinTries);

	return CByteArray(tucSW12BadPin, sizeof(tucSW12BadPin));
}

CByteArray CFileSystemCard::ReadBinary(const CByteArray & oAPDU)
{
	unsigned char ucP1 = oAPDU.GetByte(2);
	unsigned char ucP2 = oAPDU.GetByte(3);
	unsigned char ucP3 = oAPDU.GetByte(4);

	tCardFile & cardFile = m_files[m_csCurrentEF];

	// Check AC
	if (cardFile.readAC == AC_NEVER)
		return CByteArray(tucSW12SecStatusNotSatisfied, sizeof(tucSW12SecStatusNotSatisfied));
	else if (cardFile.readAC == AC_PIN)
	{
		tCardPin *poCardPin = FindPin(cardFile.ulReadPinRef);
		if (poCardPin == NULL || poCardPin->bVerifyOK)
			return CByteArray(tucSW12SecStatusNotSatisfied, sizeof(tucSW12SecStatusNotSatisfied));
	}

	if (cardFile.type != FT_TRANSP_EF)
		return CByteArray(tucSW12CommandNotAllowed, sizeof(tucSW12CommandNotAllowed));

	// Check P1, P2, P3
	unsigned long ulOffset = 256 * ucP1 + ucP2;
	if (ulOffset >= cardFile.data.Size())
		return CByteArray(tucSW12WrongParams, sizeof(tucSW12WrongParams));
	if (ulOffset + ucP3 > cardFile.data.Size())
	{
		unsigned char tucSW12[2] = {0x6C, (unsigned char) (cardFile.data.Size() - ulOffset)};
		return CByteArray(tucSW12, sizeof(tucSW12));
	}

	CByteArray oResp(cardFile.data.GetBytes() + ulOffset, ucP3);
	oResp.Append(tucSW12OK, sizeof(tucSW12OK));

	return oResp;
}

CByteArray CFileSystemCard::UpdateBinary(const CByteArray & oAPDU)
{
	unsigned char ucP1 = oAPDU.GetByte(2);
	unsigned char ucP2 = oAPDU.GetByte(3);
	unsigned char ucP3 = oAPDU.GetByte(4);

	tCardFile & cardFile = m_files[m_csCurrentEF];

	// Check AC
	if (cardFile.updateAC == AC_NEVER)
		return CByteArray(tucSW12SecStatusNotSatisfied, sizeof(tucSW12SecStatusNotSatisfied));
	else if (cardFile.updateAC == AC_PIN)
	{
		tCardPin *poCardPin = FindPin(cardFile.ulUpdatePinRef);
		if (poCardPin == NULL || !poCardPin->bVerifyOK)
			return CByteArray(tucSW12SecStatusNotSatisfied, sizeof(tucSW12SecStatusNotSatisfied));
	}

	if (cardFile.type != FT_TRANSP_EF)
		return CByteArray(tucSW12CommandNotAllowed, sizeof(tucSW12CommandNotAllowed));

	// Check P1, P2, P3
	unsigned long ulOffset = 256 * ucP1 + ucP2;
	if (ulOffset >= cardFile.data.Size())
		return CByteArray(tucSW12WrongParams, sizeof(tucSW12WrongParams));
	if (ulOffset + ucP3 > cardFile.data.Size())
		return CByteArray(tucSW12WrongLengthP3, sizeof(tucSW12WrongLengthP3));

	CByteArray oNewData(cardFile.data.GetBytes(), ulOffset);
	oNewData.Append(oAPDU.GetBytes() + 5, ucP3);
	oNewData.Append(cardFile.data.GetBytes() + ulOffset + ucP3, cardFile.data.Size() - ulOffset - ucP3);
	cardFile.data = oNewData;

	return CByteArray(tucSW12OK, sizeof(tucSW12OK));
}

CByteArray CFileSystemCard::GenRandom(const CByteArray & oAPDU)
{
	unsigned char ucP1 = oAPDU.GetByte(2);
	unsigned char ucP2 = oAPDU.GetByte(3);
	unsigned char ucP3 = oAPDU.GetByte(4);

	if (ucP1 != 0x00 || ucP2 != 0x00)
		return CByteArray(tucSW12IncorrectParamsP1P2, sizeof(tucSW12IncorrectParamsP1P2));

	CByteArray oResp(ucP3 + 2);

	static unsigned long ulClock = (unsigned long) clock();
	static unsigned long a = ulClock / 256;
	static unsigned long b = ulClock % 257;

	for (unsigned char i = 0; i < ucP3; i++)
	{
		oResp.Append((unsigned char) (a + i *b) % 259);
		a = a - b;
		b = (17 * b % 257);
	}

	oResp.Append(0x90);
	oResp.Append(0x00);

	return oResp;
}

////////////////////////////////////////////////////////////////////////

void CFileSystemCard::PutTransparentEF(const std::string & csPath,
	const CByteArray & data,
	tAC readAC, unsigned long ulReadPinRef,
	tAC updateAC, unsigned long ulUpdatePinRef)
{
	tCardFile cardFile;

	cardFile.type = FT_TRANSP_EF;
	cardFile.csPath = csPath;
	cardFile.readAC = readAC;
	cardFile.ulReadPinRef = ulReadPinRef;
	cardFile.updateAC = updateAC;
	cardFile.ulUpdatePinRef = ulUpdatePinRef;
	cardFile.data = data;

	m_files[csPath] = cardFile;
}

static inline bool IsHexDigit(char c)
{
    return ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'));
}

static inline unsigned char Hex2Byte(char cHex)
{
    if (cHex >= '0' && cHex <= '9')
        return (unsigned char) (cHex - '0');
    if (cHex >= 'A' && cHex <= 'F')
        return (unsigned char) (cHex - 'A' + 10);
    return (unsigned char) (cHex - 'a' + 10);
}

CByteArray CFileSystemCard::ToBin(const std::string & strHexData)
{
	const char *csHexData = strHexData.c_str();
	int iHexLen = (int) strHexData.size();
	CByteArray oData(iHexLen / 2);

	unsigned char uc = 0;
	bool bSecondHexDigit = true;
	for (int i = 0; i < iHexLen; i++)
	{
		if (IsHexDigit(csHexData[i]))
		{
			uc = 16 * uc + Hex2Byte(csHexData[i]);
			bSecondHexDigit = !bSecondHexDigit;
			if (bSecondHexDigit)
				oData.Append(uc);
		}
	}

	return oData;
}

tCardPin *CFileSystemCard::FindPin(unsigned long ulPinRef)
{
	tCardPin *poCardPin = NULL;
	std::string csPath;

	tPinMap::iterator it = m_pins.begin();
	for ( ; it != m_pins.end() && poCardPin == NULL; it++)
	{
		if (it->second.ulPinRef == ulPinRef && IsSubDir(m_csCurrentDF, it->second.csPath))
		{
			if (csPath == "" || IsSubDir(it->second.csPath, csPath))
			{
				poCardPin = &it->second;
				csPath = it->second.csPath;
			}
		}
	}

	return poCardPin;
}

inline bool CFileSystemCard::IsSubDir(const std::string &csPath1, const std::string &csPath2)
{
  bool bRet = csPath1.find(csPath2) != std::string::npos;
  return bRet;
}

LONG CFileSystemCard::SCardTransmitInternal(
	IN LPCBYTE pbSendBuffer,
	IN DWORD cbSendLength,
	OUT LPBYTE pbRecvBuffer,
	IN OUT LPDWORD pcbRecvLength)
{
	unsigned char ucINS = pbSendBuffer[1];
	CByteArray oReqAPDU(pbSendBuffer, cbSendLength);
	CByteArray oRespAPDU;

	switch(ucINS)
	{
	case 0x20:
		oRespAPDU = VerifyPin(oReqAPDU);
		break;
	case 0xA4:
		oRespAPDU = SelectFile(oReqAPDU);
		break;
	case 0xB0:
		oRespAPDU = ReadBinary(oReqAPDU);
		break;
	case 0xD6:
		oRespAPDU = UpdateBinary(oReqAPDU);
		break;
	case 0x84:
		oRespAPDU = GenRandom(oReqAPDU);
		break;
	default:
		oRespAPDU.Append(tucSW12InsNotFound, sizeof(tucSW12InsNotFound));
	}

	if (*pcbRecvLength < oRespAPDU.Size())
		return SCARD_E_INSUFFICIENT_BUFFER;

	*pcbRecvLength = oRespAPDU.Size();
	memcpy(pbRecvBuffer, oRespAPDU.GetBytes(), *pcbRecvLength);

	return SCARD_S_SUCCESS;
}
