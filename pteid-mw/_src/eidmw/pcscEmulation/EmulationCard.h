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

#include <winscard.h>
#include <vector>

#include "../common/Export.h"
#include "../common/ByteArray.h"
#ifndef WIN32
#include "wintypes.h"
#include "../common/win_macros.h"
#endif

namespace eIDMW
{

extern unsigned char tucSW12WrongLength[2];
extern unsigned char tucSW12BadPin[2];
const unsigned char tucSW12OK[2] = {0x90, 0x00};
const unsigned char tucSW12WrongLengthP3[2] = {0x67, 0x00};
const unsigned char tucSW12SecStatusNotSatisfied[2] = {0x69, 0x82};
const unsigned char tucSW12CommandNotAllowed[2] = {0x69, 0x86};
const unsigned char tucSW12WrongData[2] = {0x6A, 0x80};
const unsigned char tucSW12FunctionNotSupported[2] = {0x6A, 0x81};
const unsigned char tucSW12FileNotFound[2] = {0x6A, 0x82};
const unsigned char tucSW12IncorrectParamsP1P2[2] = {0x6A, 0x86};
const unsigned char tucSW12RefDataNotFound[2] = {0x6A, 0x88};
const unsigned char tucSW12PinBlocked[2] = {0x69, 0x83};
const unsigned char tucSW12ConditionsOfUseNOK[2] = {0x69, 0x85};
const unsigned char tucSW12WrongParams[2] = {0x6B, 0x00};
const unsigned char tucSW12InsNotFound[2] = {0x6D, 0x00};
const unsigned char tucSW12BadClass[2] = {0x6E, 0x00};
const unsigned char tucSW12Unknown[2] = {0x64, 0x00};

const CByteArray oSW12OK(tucSW12OK, sizeof(tucSW12OK));

/** Little helper class */
class CCommand
{
public:
	CCommand(const CByteArray &oRequestAPDU,
		long lRet, const CByteArray &oResponseAPDU):
	m_oReq(oRequestAPDU), m_lRet(lRet), m_oResp(oResponseAPDU)
	{
	}

	CByteArray m_oReq;
	CByteArray m_oResp;
	LONG m_lRet;
};

/**
 * Parent of all classes that emulate a smart card.
 * Apart from the normal behaviour, you can specify
 * which responses to return to specific requests
 * (SpecifyCommand) or which default response to
 * return (SpecifyResponse) */
class CEmulationCard
{
public:
	EIDMW_PCSCEMU_API CEmulationCard();

	EIDMW_PCSCEMU_API virtual void Reset();

	EIDMW_PCSCEMU_API virtual void SetATR(CByteArray oATR);

	EIDMW_PCSCEMU_API virtual CByteArray GetATR();

	EIDMW_PCSCEMU_API virtual LONG SCardTransmit(
		IN LPCBYTE pbSendBuffer,
		IN DWORD cbSendLength,
		OUT LPBYTE pbRecvBuffer,
		IN OUT LPDWORD pcbRecvLength);

	EIDMW_PCSCEMU_API virtual void SpecifyCommand(const CByteArray &oCRequestAPDU,
		LONG lRet, const CByteArray oResponseAPDU);

	EIDMW_PCSCEMU_API virtual void ForceResponse(LONG lRet,
		const CByteArray oResponseAPDU);
	EIDMW_PCSCEMU_API virtual void RemoveForcedResponse();

	virtual LONG SCardTransmitInternal(
		IN LPCBYTE pbSendBuffer,
		IN DWORD cbSendLength,
		OUT LPBYTE pbRecvBuffer,
		IN OUT LPDWORD pcbRecvLength);

protected:
	virtual bool TransmitSpecified(
		IN LPCBYTE pbSendBuffer,
		IN DWORD cbSendLength,
		OUT LPBYTE pbRecvBuffer,
		IN OUT LPDWORD pcbRecvLength,
		LONG & lRet);

	CByteArray m_oATR;
	long m_lDefaultRet;
	CByteArray m_oDefaultResponseAPDU;
	std::vector <CCommand> m_Commands;
	bool m_bEnforce;

	CEmulationCard(const CEmulationCard &oCard);
	CEmulationCard operator = (const CEmulationCard &oCard);
};

}
