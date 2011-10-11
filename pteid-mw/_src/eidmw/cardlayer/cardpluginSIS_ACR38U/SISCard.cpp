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
/** \file SISCard.h
This DLL reads all the data from the social security SIS-card.
*/

#include "../cardpluginSIS/SISplugin1.h"
#include "Log.h"
#include <string.h>

#ifdef WIN32
#define SISPLUGIN_EXPORT        __declspec(dllexport)
#define SIS_PROTOCOL            SCARD_PROTOCOL_UNDEFINED
#define IOCTL_SET_CARD_TYPE	SCARD_CTL_CODE(2060)
#define SIS_DISPOSITION         SCARD_LEAVE_CARD
const unsigned char tucSetSyncCard[] = {0x18, 0x00, 0x00, 0x00};
#else
#define SISPLUGIN_EXPORT
#define SIS_PROTOCOL            SCARD_PROTOCOL_RAW
#define IOCTL_SET_CARD_TYPE	0
#define SIS_DISPOSITION         SCARD_RESET_CARD
const unsigned char tucSetSyncCard[] = { 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00 };  // 4418/28
#endif

static long ConnectInSyncMode(tPCSCfunctions *pPCSCfunctions, const char *csReaderName,
	SCARDHANDLE *phCard, DWORD *pulProto);

static long BackToAsyncMode(tPCSCfunctions *pPCSCfunctions, const char *csReaderName,
	SCARDHANDLE *phCard);

static long ReadInternal(tPCSCfunctions *pPCSCfunctions, SCARDHANDLE hCard,
	LPCSCARD_IO_REQUEST pioSendPci, LPSCARD_IO_REQUEST pioRecvPci,
	unsigned long ulOffset, unsigned long ulMaxLen,
	unsigned char *pucData);

SISPLUGIN_EXPORT long SISPluginReadCard(
	unsigned long   ulVersion,              /**< In:     SW-version of the middleware */
	void            *pPCSCfunctions,        /**< In:     pointer to a tPCSCfunctions struct */
	const char      *csReaderName,          /**< In:     the reader name as returned by SCardListReaders() */
	SCARDHANDLE     *phCard,                /**< In:     handle to the card if it was succesful or 0 when it failed */
	unsigned char   *pucData,               /**< Out:    returns the 404 bytes of the SIS-card */
	unsigned long   ulReserved,             /**< In:     reserved for future extensions */
	void            *pReserved              /**< In/Out: reserved for future extensions */
	)             
{
	eIDMW::MWLOG(eIDMW::LEV_DEBUG, eIDMW::MOD_SIS, L"SISPluginReadCard(%d) called", *phCard);

	SCARD_IO_REQUEST ioSendPci, ioRecvPci;
	tPCSCfunctions *pPCSCfuncs = (tPCSCfunctions *) pPCSCfunctions;
	DWORD ulProto = SCARD_PROTOCOL_T0;
	long lRet = -11111;

	if (ulVersion/100 != 1)
		return lRet;         //only version 1.xx supported

	memset(pucData, 0, 404);

#ifdef __APPLE__
	if (0 != *phCard)
	{
		pPCSCfuncs->pCardDisconnect(*phCard, SCARD_RESET_CARD);
		*phCard = 0;
	}
#endif

	if (0 == *phCard)
	{
		lRet = ConnectInSyncMode(pPCSCfuncs, csReaderName, phCard, &ulProto);
		eIDMW::MWLOG(eIDMW::LEV_DEBUG, eIDMW::MOD_SIS, L"    ConnectInSyncMode return = 0x%0x",lRet);
		if (SCARD_S_SUCCESS != lRet)
			return lRet;
	}

	ioSendPci.cbPciLength = sizeof(SCARD_IO_REQUEST);
	ioRecvPci.cbPciLength = sizeof(SCARD_IO_REQUEST);
	ioSendPci.dwProtocol = ulProto;
	ioRecvPci.dwProtocol = ulProto;

	lRet = pPCSCfuncs->pCardBeginTransaction(*phCard);
	if (SCARD_S_SUCCESS == lRet)
	{
		lRet = ReadInternal(pPCSCfuncs, *phCard, &ioSendPci, &ioRecvPci, 0, 404, pucData);
		eIDMW::MWLOG(eIDMW::LEV_DEBUG, eIDMW::MOD_SIS, L"    ReadInternal return = 0x%0x",lRet);
		BackToAsyncMode(pPCSCfuncs, csReaderName, phCard);
		pPCSCfuncs->pCardEndTransaction(*phCard, SCARD_LEAVE_CARD);
	}

	// Check if it's a SIS card
	if (0 == lRet && (0xA0 != pucData[21] || 0x00 != pucData[22] || 0x00 != pucData[23] ||
		0x00 != pucData[24] || 0x33 != pucData[25]))
	{
		lRet = SCARD_E_CARD_UNSUPPORTED;
	}

	return lRet;
}

static long ConnectInSyncMode(tPCSCfunctions *pPCSCfunctions, const char *csReaderName,
	SCARDHANDLE *phCard, DWORD *pulProto)
{
	unsigned char tucRecv[258];
	DWORD ulRecvLen = (DWORD) sizeof(tucRecv);
	SCARD_IO_REQUEST ioSendPci, ioRecvPci;

	long lRet = pPCSCfunctions->pCardConnect(csReaderName, SCARD_SHARE_DIRECT,
		SIS_PROTOCOL, phCard, pulProto);
	if (SCARD_S_SUCCESS != lRet)
		return lRet;

#ifdef __APPLE__
	*pulProto = SCARD_PROTOCOL_RAW;
#endif

	ioSendPci.cbPciLength = sizeof(SCARD_IO_REQUEST);
	ioRecvPci.cbPciLength = sizeof(SCARD_IO_REQUEST);
	ioSendPci.dwProtocol = *pulProto;
	ioRecvPci.dwProtocol = *pulProto;

#ifdef __APPLE__
	lRet = pPCSCfunctions->pCardBeginTransaction(*phCard);
	if (SCARD_S_SUCCESS == lRet)
	{
		// Set cardtype in card-reader to accept synch cards
		int32_t tSetSyncCard[2] = {0, 5};

		lRet = pPCSCfunctions->pCardTransmit(*phCard,
			&ioSendPci, (unsigned char *) tSetSyncCard, (DWORD) sizeof(tSetSyncCard),
			&ioRecvPci, tucRecv, &ulRecvLen);

		pPCSCfunctions->pCardEndTransaction(*phCard, SCARD_LEAVE_CARD);
	}

	// Reconnect
	if (SCARD_S_SUCCESS == lRet)
	{
		lRet = pPCSCfunctions->pCardDisconnect(*phCard, SCARD_RESET_CARD);
		if (SCARD_S_SUCCESS == lRet)
		{
			lRet = pPCSCfunctions->pCardConnect(csReaderName, SCARD_SHARE_SHARED,
				SCARD_PROTOCOL_T0, phCard, pulProto);
		}
	}
#else
	// Set cardtype in card-reader to accept synch cards
	lRet = pPCSCfunctions->pCardControl(*phCard, IOCTL_SET_CARD_TYPE, 
		tucSetSyncCard, (unsigned long) sizeof(tucSetSyncCard),
		tucRecv, ulRecvLen, &ulRecvLen);

	// Reconnect
	if (SCARD_S_SUCCESS == lRet)
	{
		lRet = pPCSCfunctions->pCardDisconnect(*phCard, SIS_DISPOSITION);
		if (SCARD_S_SUCCESS == lRet)
		{
			lRet = pPCSCfunctions->pCardConnect(csReaderName, SCARD_SHARE_SHARED,
				SCARD_PROTOCOL_T0, phCard, pulProto);
		}
	}
#endif

	return lRet;
}

static long BackToAsyncMode(tPCSCfunctions *pPCSCfunctions, const char *csReaderName,
	SCARDHANDLE *phCard)
{
	unsigned char tucRecv[258];
	DWORD ulRecvLen = (DWORD) sizeof(tucRecv);
#ifdef __APPLE__
	int32_t tSetAsyncCard[2] = {0, 0};
	long lRet = pPCSCfunctions->pCardEndTransaction(*phCard, SCARD_LEAVE_CARD);
	if (SCARD_S_SUCCESS == lRet)
	{
		lRet = pPCSCfunctions->pCardDisconnect(*phCard, SCARD_RESET_CARD);
		if (SCARD_S_SUCCESS == lRet)
		{
			DWORD dwAP;
			lRet = pPCSCfunctions->pCardConnect(csReaderName, SCARD_SHARE_DIRECT,
				SIS_PROTOCOL, phCard, &dwAP);
			if (SCARD_S_SUCCESS == lRet)
			{
#ifdef __APPLE__
				dwAP = SCARD_PROTOCOL_RAW;
#endif
				lRet = pPCSCfunctions->pCardBeginTransaction(*phCard);
				if (SCARD_S_SUCCESS == lRet)
				{
					SCARD_IO_REQUEST ioSendPci, ioRecvPci;
				        ioSendPci.cbPciLength = sizeof(SCARD_IO_REQUEST);
        				ioRecvPci.cbPciLength = sizeof(SCARD_IO_REQUEST);
        				ioSendPci.dwProtocol = dwAP;
        				ioRecvPci.dwProtocol = dwAP;   
					long lRet = pPCSCfunctions->pCardTransmit(*phCard,
						&ioSendPci, (unsigned char *) tSetAsyncCard, sizeof(tSetAsyncCard),
						&ioRecvPci, tucRecv, &ulRecvLen);
				}
			}
		}
	}
	return lRet;
#else
	const unsigned char tucSetAsyncCard[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	return pPCSCfunctions->pCardControl(*phCard, IOCTL_SET_CARD_TYPE,
		tucSetAsyncCard, sizeof(tucSetAsyncCard),
		tucRecv, ulRecvLen, &ulRecvLen);
#endif
}

static long ReadInternal(tPCSCfunctions *pPCSCfunctions, SCARDHANDLE hCard,
	LPCSCARD_IO_REQUEST pioSendPci, LPSCARD_IO_REQUEST pioRecvPci,
	unsigned long ulOffset, unsigned long ulMaxLen,
	unsigned char *pucData)
{
	unsigned long ulLen = ulMaxLen > 252 ? 252 : ulMaxLen;
	unsigned long ulTotLen = 0;

	unsigned char tucReadDat[5] = {0xFF, 0xB2};
	tucReadDat[2] = (unsigned char)(ulOffset/256);
	tucReadDat[3] = (unsigned char)(ulOffset%256);
	tucReadDat[4] = (unsigned char)(ulLen);

	unsigned char tucRecv[258];
	DWORD ulRecvLen = (DWORD) sizeof(tucRecv);

	long lRet = pPCSCfunctions->pCardTransmit(hCard,
		pioSendPci, tucReadDat, (DWORD) sizeof(tucReadDat),
		pioRecvPci, tucRecv, &ulRecvLen);
	if (SCARD_S_SUCCESS != lRet)
		return lRet;
	if (ulRecvLen < 2)
		return SCARD_F_COMM_ERROR;

	ulRecvLen -= 2; // remove SW1 SW2
	memcpy(pucData, tucRecv, ulRecvLen);
	ulTotLen = ulRecvLen;

	// If we read enough then return
	if (ulMaxLen == ulLen)
		return 0;

	ulLen = ulRecvLen;
	ulOffset += ulLen;
	ulMaxLen -= ulLen;
	ulLen = ulMaxLen > 252 ? 252 : ulMaxLen;
	tucReadDat[2] = (unsigned char)(ulOffset/256);
	tucReadDat[3] = (unsigned char)(ulOffset%256);
	tucReadDat[4] = (unsigned char)(ulLen);

	ulRecvLen = (unsigned long) sizeof(tucRecv);
	lRet = pPCSCfunctions->pCardTransmit(hCard,
		pioSendPci, tucReadDat, sizeof(tucReadDat),
		pioRecvPci, tucRecv, &ulRecvLen);
	if (SCARD_S_SUCCESS != lRet)
		return lRet;
	if (ulRecvLen < 2)
		return SCARD_F_COMM_ERROR;

	ulRecvLen -= 2; // remove SW1 SW2
	if (ulTotLen + ulRecvLen > 404)
		return SCARD_E_INSUFFICIENT_BUFFER;
	memcpy(pucData + ulTotLen, tucRecv, ulRecvLen);

	return 0;
}
