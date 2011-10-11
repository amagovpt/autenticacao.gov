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
Read the social security SIS-card and decode the data into useable fields

*/

/* To switch back to Asynchronous mode while the SIS card is still inserted.
 * It implies reading everything at onces in GetCardInstance() so it's no
 * longer needed to read from the card; and then switch to Asynchr. mode.
 * The downside is that we can't take advantage of caching this way..
 */
#if !defined WIN32
#define SWITCH_TO_ASYNC_AFTER_READ    // Needed on Linux
#endif

#include "SISCard.h"
#include "../common/Log.h"

using namespace eIDMW;

#ifdef WIN32
#define SIS_PROTOCOL            SCARD_PROTOCOL_UNDEFINED
#define IOCTL_SET_CARD_TYPE	SCARD_CTL_CODE(2060)
#define SIS_DISPOSITION         DISCONNECT_LEAVE_CARD
const unsigned char tucSetSyncCard[] = {0x18, 0x00, 0x00, 0x00};
#else
#define SIS_PROTOCOL            SCARD_PROTOCOL_RAW
#define IOCTL_SET_CARD_TYPE	0
#define SIS_DISPOSITION         DISCONNECT_RESET_CARD
const unsigned char tucSetSyncCard[] = { 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00 };  // 4418/28
#endif

// If we want to 'hardcode' this plugin internally in the CAL, this function
// can't be present because it's the same for all plugins
#ifndef CARDPLUGIN_IN_CAL
CCard * ConnectGetCardInstance(unsigned long ulVersion,const char *csReader,
	CContext *poContext, CPinpad *poPinpad)
{
	return SISCardConnectGetInstance(ulVersion, csReader, poContext, poPinpad);
}

CCard *GetCardInstance(unsigned long ulVersion, const char *csReader,
	SCARDHANDLE hCard, CContext *poContext, CPinpad *poPinpad)
{
	return SISCardGetInstance(ulVersion, csReader, hCard, poContext, poPinpad);
}
#endif

CCard * SISCardConnectGetInstance(unsigned long ulVersion,const char *csReader,
	CContext *poContext, CPinpad *poPinpad)
{
	CCard *poCard = NULL;
	CPCSC *poPCSC = &poContext->m_oPCSC;

	if ((ulVersion % 100) == (PLUGIN_VERSION % 100))
	{
		try
		{
			// Synchr. cards require temporary direct-connection set the card-type in the card-reader
			SCARDHANDLE hCard = poPCSC->Connect(csReader, SCARD_SHARE_DIRECT, SIS_PROTOCOL);

#ifdef __APPLE__
			// Set cardtype in card-reader to accept synch cards
			int32_t tSetSyncCard[2] = {0, 5};
			{
				CAutoLock oAutoLock(poPCSC, hCard);
				long lretVal = 0;
				CByteArray oCmd((unsigned char *)tSetSyncCard, sizeof(tSetSyncCard));
				poPCSC->Transmit(hCard, oCmd, &lretVal, SCARD_PCI_RAW);

			}

			// Reconnect
			poPCSC->Disconnect(hCard, DISCONNECT_RESET_CARD );
			hCard = poPCSC->Connect(csReader, SCARD_SHARE_SHARED, SCARD_PROTOCOL_T0);
#else
			// Set cardtype in card-reader to accept synch cards
			CByteArray oCmd(tucSetSyncCard, sizeof(tucSetSyncCard));
			poPCSC->Control(hCard, IOCTL_SET_CARD_TYPE, oCmd);

			// Reconnect
			poPCSC->Disconnect(hCard, SIS_DISPOSITION);
			hCard = poPCSC->Connect(csReader, SCARD_SHARE_SHARED, SCARD_PROTOCOL_T0);
#endif
			poCard = SISCardGetInstance(ulVersion, csReader, hCard, poContext, poPinpad);
		}
		catch(...)
		{
			//printf("Exception in cardPluginSIS.CardConnect()\n");
		}
	}

	return poCard;
}

static CByteArray ReadInternal(CPCSC *poPCSC, SCARDHANDLE hCard, unsigned long ulOffset, unsigned long ulMaxLen)
{
	unsigned long ulLen = ulMaxLen > 252 ? 252 : ulMaxLen;
	long lretVal = 0;
	unsigned char tucReadDat[5] = {0xFF, 0xB2};
	tucReadDat[2] = (unsigned char)(ulOffset/256);
	tucReadDat[3] = (unsigned char)(ulOffset%256);
	tucReadDat[4] = (unsigned char)(ulLen);
	CByteArray oCmd(tucReadDat, sizeof(tucReadDat));
	CByteArray oData = poPCSC->Transmit(hCard, oCmd, &lretVal);

	oData.Chop(2); // remove SW12

	// If we read enough, or there isn't more to read, return
	if (ulMaxLen == ulLen || oData.Size() < ulLen)
	{
		MWLOG(LEV_INFO, MOD_CAL, L"   Read %d bytes from the SIS card", oData.Size());
		return oData;
	}

	ulLen = oData.Size();
	ulOffset += ulLen;
	ulMaxLen -= ulLen;
	ulLen = ulMaxLen > 252 ? 252 : ulMaxLen;
	tucReadDat[2] = (unsigned char)(ulOffset/256);
	tucReadDat[3] = (unsigned char)(ulOffset%256);
	tucReadDat[4] = (unsigned char)(ulLen);
	oCmd = CByteArray(tucReadDat, sizeof(tucReadDat));
	CByteArray oData2 = poPCSC->Transmit(hCard, oCmd,&lretVal);

	oData2.Chop(2); // remove SW12

	oData.Append(oData2);

	MWLOG(LEV_INFO, MOD_CAL, L"   Read %d bytes from the SIS card", oData.Size());

	return oData;
}

static inline void BackToAsyncMode(SCARDHANDLE &hCard, CPCSC *poPCSC, const char *csReader)
{
#ifdef __APPLE__
	int32_t tSetAsyncCard[2] = {0, 0};
	long lretVal = 0;
	CByteArray oCmd((unsigned char *) tSetAsyncCard, sizeof(tSetAsyncCard));
	SCARD_IO_REQUEST ioRecvPci;
	poPCSC->EndTransaction(hCard);
	poPCSC->Disconnect(hCard, SIS_DISPOSITION);
	hCard = poPCSC->Connect(csReader, SCARD_SHARE_DIRECT, SIS_PROTOCOL);
	poPCSC->BeginTransaction(hCard);
	poPCSC->Transmit(hCard, oCmd, &lretVal, SCARD_PCI_RAW, &ioRecvPci);
#else
	const unsigned char tucSetAyncCard[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	CByteArray oCmd(tucSetAyncCard, sizeof(tucSetAyncCard));
	poPCSC->Control(hCard, IOCTL_SET_CARD_TYPE, oCmd);
#endif
}

CCard *SISCardGetInstance(unsigned long ulVersion, const char *csReader,
	SCARDHANDLE hCard, CContext *poContext, CPinpad *poPinpad)
{
	CCard *poCard = NULL;
	bool bIsSisCard = false;
	CByteArray oData;

	// Check if it's a SIS card by reading the ATR and serial number
	try
	{
		poContext->m_oPCSC.BeginTransaction(hCard);
#ifndef SWITCH_TO_ASYNC_AFTER_READ
		unsigned long ulReadLen = 26;  // read only the first part
#else
		unsigned long ulReadLen = 404; // read everything
		try {
#endif
		oData = ReadInternal(&poContext->m_oPCSC, hCard, 0, ulReadLen);
		bIsSisCard =
			(oData.Size() >= 26) &&
			(oData.GetByte(21) == 0xA0) && (oData.GetByte(22) == 0x00) &&
			(oData.GetByte(23) == 0x00) && (oData.GetByte(24) == 0x00) && (oData.GetByte(25) == 0x33);
#ifdef SWITCH_TO_ASYNC_AFTER_READ
		} catch (...) {
			// Switch back to asynchronous mode.
			BackToAsyncMode(hCard, &poContext->m_oPCSC, csReader);
			throw;
		}
		// Switch back to asynchronous mode.
		BackToAsyncMode(hCard, &poContext->m_oPCSC, csReader);
#endif

		if (bIsSisCard)
			poCard = new CSISCard(hCard, poContext, poPinpad, oData);

		poContext->m_oPCSC.EndTransaction(hCard);
	}
	catch(...)
	{
		poContext->m_oPCSC.EndTransaction(hCard);
		//printf("Exception in cardPluginSIS.SISCardGetInstance()\n");
	}

	return poCard;
}

///////////////////////////////////////////////////////////////////////////////////

CSISCard::CSISCard(
        SCARDHANDLE       hCard, 
        CContext            *poContext,
	    CPinpad             *poPinpad, 
        const CByteArray    &  oData    // is either the first 26 bytes, or the whole 404 bytes
        ) :CCard(hCard, poContext, poPinpad)
{
	if (oData.Size() == 26)
		m_oCardAtr = oData;
	else
	{
	    m_oCardAtr = CByteArray(oData.GetBytes(), 26);
		m_oCardData = oData;
	}
}

CSISCard::~CSISCard(void)
{
}

/** Read all data from the sis-card

\return CByteArray oData            requested data from the SIS-card
\retval EIDMW_ERR_FILE_NOT_FOUND    exception; when unknown file-path is given
\retval EIDMW_ERR_CARD              exception; when the SIS-card can not supply the requested data

\sa 
*************************************************************************************/
CByteArray CSISCard::ReadUncachedFile(
        const std::string &csPath,          /**< In:     file-path of the requested data, currently supported: "data" */
        unsigned long ulOffset,             /**< In:     start-offset within sis-card data, range 0-404 */
        unsigned long ulMaxLen              /**< In:     requested data-length, range 0-404  */
        )
{
	if (m_oCardData.Size() == 0)
		m_oCardData = ReadInternal(&m_poContext->m_oPCSC, m_hCard, 0, 404);

	CAutoLock oAutoLock(this);

	if (ulOffset == 0 && ulMaxLen == FULL_FILE)
		return m_oCardData;

	if (ulOffset > m_oCardData.Size())
		throw CMWEXCEPTION(EIDMW_ERR_PARAM_RANGE);

	if (ulMaxLen > ulOffset + m_oCardData.Size())
	ulMaxLen = m_oCardData.Size() - ulOffset;
	return CByteArray(m_oCardData.GetBytes() + ulOffset, ulMaxLen);
}

tCardType CSISCard::GetType()
{
    return CARD_SIS;
}

std::string CSISCard::GetSerialNr()
{
    CByteArray oSerial(m_oCardAtr.GetBytes()+6, 11);
    return oSerial.ToString(false);
}

std::string CSISCard::GetLabel()
{
    return "SIS";
}

CByteArray CSISCard::GetATR()
{
    //first 4 card-bytes
    return CByteArray(m_oCardAtr.GetBytes(), 4);
}

tCacheInfo CSISCard::GetCacheInfo(const std::string &csPath)
{
	// There's only 1 file and it is cache-able
	tCacheInfo simpleCache = {SIMPLE_CACHE, 0};

	return simpleCache;
}
