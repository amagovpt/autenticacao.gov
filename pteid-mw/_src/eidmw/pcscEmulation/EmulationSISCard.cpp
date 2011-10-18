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
#include "EmulationSISCard.h"	

// taken from WinError.h
#if (!defined WIN32) && (!defined SCARD_E_COMM_DATA_LOST)
#define SCARD_E_COMM_DATA_LOST ((DWORD)0x8010002F)
#endif

using namespace eIDMW;

CEmulationSISCard::CEmulationSISCard(const CByteArray & oData)
{
	m_oData = oData;
}

LONG CEmulationSISCard::SCardTransmitInternal(
		IN LPCBYTE pbSendBuffer,
		IN DWORD cbSendLength,
		OUT LPBYTE pbRecvBuffer,
		IN OUT LPDWORD pcbRecvLength)
{
	long lRet = SCARD_E_COMM_DATA_LOST;

	// First check if it's an ACR38U 'control' command on Mac 
	if (cbSendLength == 8 && memcmp(pbSendBuffer, "\x00\x00\x00\x00", 4) == 0)
	{
		if (*pcbRecvLength < 2)
			return SCARD_E_INSUFFICIENT_BUFFER;
		pbRecvBuffer[0] = 0x90;
		pbRecvBuffer[1] = 0x00;
		*pcbRecvLength = 2;
		return SCARD_S_SUCCESS;
	}

	if (cbSendLength == 5 && pbSendBuffer[0] == 0xFF && pbSendBuffer[1] == 0xB2)
	{
		unsigned long ulOffset = 256 * pbSendBuffer[2] + pbSendBuffer[3];
		unsigned long ulLen = pbSendBuffer[4];

		if (*pcbRecvLength < ulLen + 2)
			return SCARD_E_INSUFFICIENT_BUFFER;

		memcpy(pbRecvBuffer, m_oData.GetBytes() + ulOffset, ulLen);
		pbRecvBuffer[ulLen] = 0x90;
		pbRecvBuffer[ulLen + 1] = 0x00;
		*pcbRecvLength = ulLen + 2;

		lRet = SCARD_S_SUCCESS;
	}

	return lRet;
}

