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

#include "EmulationPkiCard.h"

namespace eIDMW
{

class CEmulationBeidCard : public CEmuationPkiCard
{
public:
	EIDMW_PCSCEMU_API CEmulationBeidCard(unsigned char ucVersion);

	virtual LONG SCardTransmitInternal(
		IN LPCBYTE pbSendBuffer,
		IN DWORD cbSendLength,
		OUT LPBYTE pbRecvBuffer,
		IN OUT LPDWORD pcbRecvLength);

protected:
	virtual CByteArray GetCardData(CByteArray oAPDU);
	virtual CByteArray SetSecurityEnv(const CByteArray & oAPDU);
	virtual CByteArray PSOSign(const CByteArray & oAPDU);
	virtual CByteArray ChangePin(const CByteArray & oAPDU);
	virtual CByteArray GetPinStatus(const CByteArray & oAPDU);
	virtual CByteArray InternalAuth(const CByteArray & oAPDU);
	virtual CByteArray MakeFakeSignature(const CByteArray &oData);
	virtual CByteArray GetSerialNr();

	bool m_bPreviousCommandWasNonRepVerify;
	bool m_bKeySelected;
	unsigned long m_ucSelectedAlgo;
	unsigned long m_ucSelectedKeyRef;
	unsigned char m_ucVersion;
};

}
