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
#include "EmulationPteidCard.h"
#include "../common/Hash.h"

using namespace eIDMW;

CEmulationPteidCard::CEmulationPteidCard(unsigned char ucVersion)
{
	m_ucVersion = ucVersion;
	m_bKeySelected = false;
}

CByteArray CEmulationPteidCard::GetCardData(CByteArray oAPDU)
{
	unsigned char ucCLA = oAPDU.GetByte(0);
	unsigned char ucP1 = oAPDU.GetByte(2);
	unsigned char ucP2 = oAPDU.GetByte(3);
	unsigned char ucP3 = oAPDU.GetByte(4);

	if (ucCLA != 0x80)
		return CByteArray(tucSW12BadClass, sizeof(tucSW12BadClass));
	if (ucP2 != 0x00)
		return CByteArray(tucSW12IncorrectParamsP1P2, sizeof(tucSW12IncorrectParamsP1P2));

	if (ucP1 == 0x00 || ucP1 == 0x02)
	{
		if ((ucP1 == 0x00 && ucP3 != 0x1C) || (ucP1 == 0x02 && ucP3 != 0x9C))
			return CByteArray(tucSW12IncorrectParamsP1P2, sizeof(tucSW12IncorrectParamsP1P2));
		// Signed card data not for V1 cards
		if (ucP1 == 0x02 && m_ucVersion < 0x20)
			return CByteArray(tucSW12InsNotFound, sizeof(tucSW12InsNotFound));

		unsigned char tucDataV11[] = {
			0xA5,0x03,0x01,0x01,0x01,0x11,0x00,0x02,0x00,0x01,0x01,0x0F};
		unsigned char tucDataV20[] = {
			0xD0,0x00,0x48,0x01,0x01,0x20,0x00,0x03,0x01,0x0F,0x02,0x8A};

		CByteArray oResp(0x9C + 2);

		oResp.Append(GetSerialNr());

		// Add the rest, this depends on the version
		if (m_ucVersion < 0x20)
			oResp.Append(tucDataV11, sizeof(tucDataV11));
		else
			oResp.Append(tucDataV20, sizeof(tucDataV20));

		if (ucP1 == 0x02)
			oResp.Append(MakeFakeSignature(oResp));

		oResp.Append(tucSW12OK, sizeof(tucSW12OK));

		return oResp;
	}
	else
		return CByteArray(tucSW12IncorrectParamsP1P2, sizeof(tucSW12IncorrectParamsP1P2));
}

/* For signature that can't/won't be checked */
CByteArray CEmulationPteidCard::MakeFakeSignature(const CByteArray &oData)
{
	CByteArray oFakeSignature(128);

	CHash oHash;
	oFakeSignature.Append(GetSerialNr()); // to diversify for each card
	oFakeSignature.Append(oHash.Hash(ALGO_MD5, oData));
	for (int i = 0; i < 6; i++)
		oFakeSignature.Append(oHash.Hash(ALGO_MD5, oFakeSignature));

	return oFakeSignature;
}

/**Serial Nr = first 16 bytes, get them from the EF(TokenInfo) */
CByteArray CEmulationPteidCard::GetSerialNr()
{
	CByteArray oSerial(16);

	tCardFile & cardFile = m_files["3F00DF005032"];
	if (cardFile.data.Size() > 23)
		oSerial.Append(cardFile.data.GetBytes() + 7, 16);
	else
	{
		// In case the EF(TokenInfo) wouldn't exist (which is an error)
		unsigned char tucZeros[16] = {0x00};
		oSerial.Append(tucZeros, sizeof(tucZeros));
	}

	return oSerial;
}

CByteArray CEmulationPteidCard::ChangePin(const CByteArray & oAPDU)
{
	unsigned char ucCLA = oAPDU.GetByte(0);
	unsigned char ucP1 = oAPDU.GetByte(2);
	unsigned char ucP2 = oAPDU.GetByte(3);
	unsigned char ucP3 = oAPDU.GetByte(4);

	if (ucCLA != 0x00)
		return CByteArray(tucSW12BadClass, sizeof(tucSW12BadClass));
	if (ucP1 == 0x01)
		return CByteArray(tucSW12FunctionNotSupported, sizeof(tucSW12FunctionNotSupported));
	if (ucP1 != 0x00)
		return CByteArray(tucSW12IncorrectParamsP1P2, sizeof(tucSW12IncorrectParamsP1P2));
	if (ucP3 != 0x10)
		return CByteArray(tucSW12WrongLengthP3, sizeof(tucSW12WrongLengthP3));

	tCardPin *poCardPin = FindPin(ucP2);
	if (poCardPin == NULL)
		return CByteArray(tucSW12RefDataNotFound, sizeof(tucSW12RefDataNotFound));

	if (poCardPin->ulPinTries == 0)
		return CByteArray(tucSW12PinBlocked, sizeof(tucSW12PinBlocked));

	char csNewPin[13] = {0};
	int iCount = oAPDU.GetByte(13) & 0x0F;
	if (iCount < 4 || iCount > 12 || oAPDU.GetByte(20) != 0xFF)
		return CByteArray(tucSW12WrongData, sizeof(tucSW12WrongData));

	iCount = 0;
	for (int i = 14; i < 20; i++)
	{
		unsigned char uc = oAPDU.GetByte(i);
		if ((uc & 0xF0) == 0xF0)
			break;
		csNewPin[iCount++] = 0x30 + ((uc & 0xF0) / 16);
		if ((uc & 0x0F) == 0x0F)
			break;
		csNewPin[iCount++] = 0x30 + (uc & 0x0F);
	}

	poCardPin->ulPinTries--;

	CByteArray oCardPinBuf = MakePinBuf(poCardPin->csPin, *poCardPin);
	CByteArray oPresentedPinBuf(oAPDU.GetBytes() + 5, 8);

	if (!oCardPinBuf.Equals(oPresentedPinBuf))
	{
		tucSW12BadPin[1] = (unsigned char) (0xC0 + poCardPin->ulPinTries);
		return CByteArray(tucSW12BadPin, sizeof(tucSW12BadPin));
	}

	poCardPin->ulPinTries = poCardPin->ulMaxPinTries;
	poCardPin->bVerifyOK = true;
	poCardPin->csPin = csNewPin;

	return CByteArray(tucSW12OK, sizeof(tucSW12OK));
}

CByteArray CEmulationPteidCard::SetSecurityEnv(const CByteArray & oAPDU)
{
	unsigned char ucCLA = oAPDU.GetByte(0);
	unsigned char ucP1 = oAPDU.GetByte(2);
	unsigned char ucP2 = oAPDU.GetByte(3);
	unsigned char ucP3 = oAPDU.GetByte(4);

	m_bKeySelected = false;

	if (ucCLA != 0x00)
		return CByteArray(tucSW12BadClass, sizeof(tucSW12BadClass));
	if (ucP1 != 0x41 || ucP2 != 0xB6)
		return CByteArray(tucSW12IncorrectParamsP1P2, sizeof(tucSW12IncorrectParamsP1P2));
	if (ucP3 != 0x05)
		return CByteArray(tucSW12WrongLengthP3, sizeof(tucSW12WrongLengthP3));

	if (oAPDU.GetByte(5) != 0x04 || oAPDU.GetByte(6) != 0x80 || oAPDU.GetByte(8) != 0x84)
		return CByteArray(tucSW12WrongData, sizeof(tucSW12WrongData));

	m_ucSelectedAlgo = oAPDU.GetByte(7);
	unsigned char ulKeyRef = oAPDU.GetByte(9);

	tCardKey *key = FindKey(ulKeyRef);
	if (key == NULL)
		return CByteArray(tucSW12RefDataNotFound, sizeof(tucSW12RefDataNotFound));

	if (m_ucSelectedAlgo == 0x01)
		m_ucSelectedAlgo = ALGO_RSA_PKCS1;
	else if (m_ucSelectedAlgo == 0x02)
		m_ucSelectedAlgo = ALGO_RSA_PKCS1_SHA1;
	else if (m_ucSelectedAlgo == 0x04)
		m_ucSelectedAlgo = ALGO_RSA_PKCS1_MD5;
	else if (m_ucVersion >= 0x20 && m_ucSelectedAlgo == 0x10)
		m_ucSelectedAlgo = ALGO_RSA_PSS_SHA1;
	else
		return CByteArray(tucSW12WrongData, sizeof(tucSW12WrongData));

	if (!(m_ucSelectedAlgo & key->ulAlgos))
		return CByteArray(tucSW12WrongData, sizeof(tucSW12WrongData));

	m_ucSelectedKeyRef = ulKeyRef;
	m_bKeySelected = true;

	return CByteArray(tucSW12OK, sizeof(tucSW12OK));
}

CByteArray CEmulationPteidCard::PSOSign(const CByteArray & oAPDU)
{
	unsigned char ucCLA = oAPDU.GetByte(0);
	unsigned char ucP1 = oAPDU.GetByte(2);
	unsigned char ucP2 = oAPDU.GetByte(3);
	unsigned char ucP3 = oAPDU.GetByte(4);

	if (ucCLA != 0x00)
		return CByteArray(tucSW12BadClass, sizeof(tucSW12BadClass));
	if (ucP1 != 0x9E || ucP2 != 0x9A)
		return CByteArray(tucSW12IncorrectParamsP1P2, sizeof(tucSW12IncorrectParamsP1P2));

	if (!m_bKeySelected)
		return CByteArray(tucSW12ConditionsOfUseNOK, sizeof(tucSW12ConditionsOfUseNOK));

	tCardKey *poKey = FindKey(m_ucSelectedKeyRef);
	if (poKey == NULL)
		return CByteArray(tucSW12RefDataNotFound, sizeof(tucSW12RefDataNotFound));

	tCardPin *pin = FindPin(poKey->ulPinRef);
	if (pin != NULL)
	{
		if (!pin->bVerifyOK)
			return CByteArray(tucSW12SecStatusNotSatisfied, sizeof(tucSW12SecStatusNotSatisfied));

		if (poKey->bUserConsent && !m_bPreviousCommandWasNonRepVerify)
			return CByteArray(tucSW12SecStatusNotSatisfied, sizeof(tucSW12SecStatusNotSatisfied));
	}

	return Sign(CByteArray(oAPDU.GetBytes() + 5, oAPDU.Size() - 5),
		poKey, m_ucSelectedAlgo);
}

CByteArray CEmulationPteidCard::GetPinStatus(const CByteArray & oAPDU)
{
	unsigned char ucCLA = oAPDU.GetByte(0);
	unsigned char ucP1 = oAPDU.GetByte(2);
	unsigned char ucP2 = oAPDU.GetByte(3);
	unsigned char ucP3 = oAPDU.GetByte(4);

	// Not for V1 cards
	if (m_ucVersion < 0x20)
		return CByteArray(tucSW12InsNotFound, sizeof(tucSW12InsNotFound));

	if (ucCLA != 0x80)
		return CByteArray(tucSW12BadClass, sizeof(tucSW12BadClass));
	if (ucP1 != 0x00 && ucP1 != 0x02)
		return CByteArray(tucSW12FunctionNotSupported, sizeof(tucSW12FunctionNotSupported));
	if ((ucP1 == 0x00 && ucP3 != 0x01) || (ucP1 == 0x02 && ucP3 != 0x81))
		return CByteArray(tucSW12WrongLengthP3, sizeof(tucSW12WrongLengthP3));

	tCardPin *poCardPin = FindPin(ucP2);
	if (poCardPin == NULL)
		return CByteArray(tucSW12RefDataNotFound, sizeof(tucSW12RefDataNotFound));

	CByteArray oResp(0x81 + 2);

	oResp.Append((unsigned char) poCardPin->ulPinTries);

	if (ucP1 == 0x02)
		oResp.Append(MakeFakeSignature(oResp));

	oResp.Append(tucSW12OK, sizeof(tucSW12OK));

	return oResp;
}

CByteArray CEmulationPteidCard::InternalAuth(const CByteArray & oAPDU)
{
	unsigned char ucCLA = oAPDU.GetByte(0);
	unsigned char ucP1 = oAPDU.GetByte(2);
	unsigned char ucP2 = oAPDU.GetByte(3);
	unsigned char ucP3 = oAPDU.GetByte(4);

	if (ucCLA != 0x00)
		return CByteArray(tucSW12BadClass, sizeof(tucSW12BadClass));
	if (ucP1 != 0x02)
		return CByteArray(tucSW12WrongParams, sizeof(tucSW12WrongParams));
	if (ucP2 != 0x81)
		return CByteArray(tucSW12RefDataNotFound, sizeof(tucSW12RefDataNotFound));
	if ((ucP3 != 0x16))
		return CByteArray(tucSW12WrongLengthP3, sizeof(tucSW12WrongLengthP3));

	CByteArray oResp = MakeFakeSignature(CByteArray(oAPDU.GetBytes() + 7, 20));

	oResp.Append(tucSW12OK, sizeof(tucSW12OK));

	return oResp;
}

LONG CEmulationPteidCard::SCardTransmitInternal(
	IN LPCBYTE pbSendBuffer,
	IN DWORD cbSendLength,
	OUT LPBYTE pbRecvBuffer,
	IN OUT LPDWORD pcbRecvLength)
{
	unsigned char ucINS = pbSendBuffer[1];
	CByteArray oReqAPDU(pbSendBuffer, cbSendLength);
	CByteArray oRespAPDU;

	if (ucINS == 0x2A)
	{
			oRespAPDU = PSOSign(oReqAPDU);
			m_bPreviousCommandWasNonRepVerify = false;
	}
	else
	{
		m_bPreviousCommandWasNonRepVerify = false;

		switch(ucINS)
		{
		case 0xE4:
			oRespAPDU = GetCardData(oReqAPDU);
			break;
		case 0xA4:
			// A little special to the Belpic applet: select 3F00 always means
			// selecting the root..
			if (pbSendBuffer[2] == 0x02 && pbSendBuffer[4] == 0x02 &&
				pbSendBuffer[5] == 0x3F && pbSendBuffer[6] == 0x00)
			{
				m_csCurrentDF = "3F00";
				m_csCurrentEF = "";
				oRespAPDU = CByteArray(tucSW12OK, sizeof(tucSW12OK));
				break;
			}
			return CFileSystemCard::SCardTransmitInternal(pbSendBuffer, cbSendLength,
				pbRecvBuffer, pcbRecvLength);
		case 0x20:
			oRespAPDU = VerifyPin(oReqAPDU);
			m_bPreviousCommandWasNonRepVerify = (oRespAPDU.Equals(oSW12OK) &&
				((m_ucVersion < 0x20) || oReqAPDU.GetByte(3) == 0x86));
			break;
		case 0x22:
			oRespAPDU = SetSecurityEnv(oReqAPDU);
			break;
		case 0x24:
			oRespAPDU = ChangePin(oReqAPDU);
			break;
		case 0xEA:
			oRespAPDU = GetPinStatus(oReqAPDU);
			break;
		case 0x88:
			oRespAPDU = InternalAuth(oReqAPDU);
			break;
		default:
			return CFileSystemCard::SCardTransmitInternal(pbSendBuffer, cbSendLength,
				pbRecvBuffer, pcbRecvLength);
		}
	}

	if (*pcbRecvLength < oRespAPDU.Size())
		return SCARD_E_INSUFFICIENT_BUFFER;

	*pcbRecvLength = oRespAPDU.Size();
	memcpy(pbRecvBuffer, oRespAPDU.GetBytes(), *pcbRecvLength);

	return SCARD_S_SUCCESS;

}
