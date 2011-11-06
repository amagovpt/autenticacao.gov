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
#include "EmulationCard.h"

#include <cstring>

namespace eIDMW
{
unsigned char tucSW12WrongLength[2] = {0x6C, 'x'}; // put in 'x' the correct length
unsigned char tucSW12BadPin[2] = {0x63, 0x00};

CEmulationCard::CEmulationCard():
	m_lDefaultRet(0),
	m_oDefaultResponseAPDU(tucSW12FunctionNotSupported, sizeof(tucSW12FunctionNotSupported)),
	m_bEnforce(false)
{
	unsigned char tucDefaultATR[] =
		{0x3B,0x98,0x13,0x40,0x0A,0xA5,0x03,0x01,0x01,0x01,0xAD,0x13,0x11};
	m_oATR = CByteArray(tucDefaultATR, sizeof(tucDefaultATR));
}

CEmulationCard::CEmulationCard(const CEmulationCard &oCard):
  m_lDefaultRet(oCard.m_lDefaultRet),
  m_oDefaultResponseAPDU(oCard.m_oDefaultResponseAPDU),
  m_bEnforce(oCard.m_bEnforce),
  m_oATR(oCard.m_oATR)
{ 
}

void CEmulationCard::Reset()
{
}

void CEmulationCard::SetATR(CByteArray oATR)
{
	m_oATR = oATR;
}

CByteArray CEmulationCard::GetATR()
{
	return m_oATR;
}

LONG CEmulationCard::SCardTransmit(
	IN LPCBYTE pbSendBuffer,
	IN DWORD cbSendLength,
	OUT LPBYTE pbRecvBuffer,
	IN OUT LPDWORD pcbRecvLength)
{
	LONG lRet;
	if (TransmitSpecified(pbSendBuffer, cbSendLength,
		pbRecvBuffer, pcbRecvLength, lRet))
	{
		return lRet;
	}
	else
		return SCardTransmitInternal(pbSendBuffer, cbSendLength,
			pbRecvBuffer, pcbRecvLength);
}

void CEmulationCard::SpecifyCommand(const CByteArray &oRequestAPDU,
	LONG lRet, const CByteArray oResponseAPDU)
{
	m_Commands.push_back(CCommand(oRequestAPDU, lRet, oResponseAPDU));
}

void CEmulationCard::ForceResponse(LONG lRet,
	const CByteArray oResponseAPDU)
{
	m_oDefaultResponseAPDU = oResponseAPDU;
	m_lDefaultRet = true;
	m_bEnforce = true;
}

void CEmulationCard::RemoveForcedResponse()
{
	m_bEnforce = false;
}

bool CEmulationCard::TransmitSpecified(
		IN LPCBYTE pbSendBuffer,
		IN DWORD cbSendLength,
		OUT LPBYTE pbRecvBuffer,
		IN OUT LPDWORD pcbRecvLength,
		LONG & lRet)
{
return false;

	CByteArray *poResp = NULL;
	bool bFound = true;

	if (m_bEnforce)
	{
		poResp = &m_oDefaultResponseAPDU;
		lRet = m_lDefaultRet;
	}
	else
	{
		CByteArray oReq(pbSendBuffer, cbSendLength);
		int i;
		for (i = (int) m_Commands.size() - 1; i >= 0; i--)
		{
			if (m_Commands[i].m_oReq.Equals(oReq))
			{
				poResp = &m_Commands[i].m_oReq;
				lRet = m_Commands[i].m_lRet;
				break;
			}
		}
		if (i < 0)
			bFound = false;
		else
		{
			if (poResp->Size() > *pcbRecvLength)
				lRet = SCARD_E_INSUFFICIENT_BUFFER;
			else
			{
				*pcbRecvLength = m_oDefaultResponseAPDU.Size();
				memcpy(pbRecvBuffer, m_oDefaultResponseAPDU.GetBytes(), *pcbRecvLength);
				lRet = SCARD_S_SUCCESS;
			}
		}
	}

	return bFound;
}

// To be overwritten by the subclasses
LONG CEmulationCard::SCardTransmitInternal(
					   IN LPCBYTE pbSendBuffer,
					   IN DWORD cbSendLength,
					   OUT LPBYTE pbRecvBuffer,
					   IN OUT LPDWORD pcbRecvLength)
{
  return SCARD_E_UNEXPECTED;
}



CEmulationCard CEmulationCard::operator = (const CEmulationCard &oCard)
{
	if (&oCard != this)
	{
		this->m_lDefaultRet = oCard.m_lDefaultRet;
		this->m_oDefaultResponseAPDU = oCard.m_oDefaultResponseAPDU;
		this->m_bEnforce = oCard.m_bEnforce;
		this->m_oATR = oCard.m_oATR;
	}

	return *this;
}

}
