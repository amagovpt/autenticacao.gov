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
#include "Card.h"
#include "../common/Log.h"

namespace eIDMW
{

CCard::CCard(SCARDHANDLE hCard, CContext *poContext, GenericPinpad *poPinpad) :
	m_hCard(hCard), m_poContext(poContext), m_poPinpad(poPinpad),
	m_oCache(poContext), m_ulLockCount(0), m_bSerialNrString(false),m_cardType(CARD_UNKNOWN)
{
}

CCard::~CCard(void)
{
    Disconnect(DISCONNECT_LEAVE_CARD);
}

void CCard::Disconnect(tDisconnectMode disconnectMode)
{
    if (m_hCard != 0)
    {
		SCARDHANDLE hTemp = m_hCard;
		m_hCard = 0;
		m_poContext->m_oPCSC.Disconnect(hTemp, disconnectMode);
    }
}

CByteArray CCard::GetATR()
{
	return m_poContext->m_oPCSC.GetATR(m_hCard);
}

CByteArray CCard::GetIFDVersion()
{
	return m_poContext->m_oPCSC.GetIFDVersion(m_hCard);
}

bool CCard::Status()
{
	return  m_poContext->m_oPCSC.Status(m_hCard);
}

void CCard::setSSO(bool value)
{
	m_poContext->m_bSSO = value;
	if (!value)
	{
		//Zero-out currently stored PINs
		for (size_t i = 0; i < m_verifiedPINs.size(); i++)
		{
			std::string &pin = m_verifiedPINs[i];
			std::fill(pin.begin(), pin.end(), 0);
		}
	}
}

bool CCard::IsPinpadReader()
{
	return false;
}

std::string CCard::GetSerialNr()
{
	if (!m_bSerialNrString)
	{
		m_csSerialNr = GetSerialNrBytes().ToString(false, true);
		m_bSerialNrString = true;
	}

	return m_csSerialNr;
}

CByteArray CCard::GetSerialNrBytes()
{
	throw CMWEXCEPTION(EIDMW_ERR_NOT_SUPPORTED);
}

std::string CCard::GetLabel()
{
	return "";
}

void CCard::Lock()
{
	if (m_ulLockCount == 0){
	    m_poContext->m_oPCSC.BeginTransaction(m_hCard);
	}
    m_ulLockCount++;
}

void CCard::Unlock()
{
    if (m_ulLockCount == 0)
        MWLOG(LEV_ERROR, MOD_CAL, L"More Unlock()s then Lock()s called!!");
    else
	{
        m_ulLockCount--;
		if (m_ulLockCount == 0)
	        m_poContext->m_oPCSC.EndTransaction(m_hCard);
	}
}

//Not supported for Unknown cards, only implemented in subclasses
void CCard::SelectApplication(const CByteArray & oAID)
{
	throw CMWEXCEPTION(EIDMW_ERR_NOT_SUPPORTED);
}

bool CCard::SelectApplet()
{
	throw CMWEXCEPTION(EIDMW_ERR_NOT_SUPPORTED);	
}

CByteArray CCard::ReadCachedFile(const std::string & csPath, std::string & csName,
	bool & bFound, unsigned long ulOffset, unsigned long ulMaxLen, bool &bFromDisk)
{
	bFromDisk = true;

	return m_oCache.GetFile(csName,
		bFound, bFromDisk, ulOffset, ulMaxLen);
}

bool CCard::SerialNrPresent(const CByteArray & oData)
{
	CByteArray oSerial = GetSerialNrBytes();
	const unsigned char *pucSerial = oSerial.GetBytes();
	unsigned long ulSerialLen = (unsigned long) oSerial.Size();

	const unsigned char *pucData = oData.GetBytes();
	unsigned long ulEnd = oData.Size() - ulSerialLen;
	//printf("serial: %s\n", CByteArray(csSerial).ToString(false, false).c_str());
	//printf("data: %s\n", oData.ToString(false, false).c_str());
	for (unsigned long i = 0; i < ulEnd; i++)
	{
		if (memcmp(pucData + i, pucSerial, ulSerialLen) == 0)
			return true;
	}

	return false;
}

/** Little helper function for ReadFile() */
static CByteArray ReturnData(CByteArray oData, unsigned long ulOffset, unsigned long ulMaxLen)
{
	if (ulOffset == 0 && ulMaxLen == FULL_FILE)
		return oData;
	else
	{
		if (ulOffset > oData.Size())
			throw CMWEXCEPTION(EIDMW_ERR_PARAM_RANGE);
		if (ulMaxLen > ulOffset + oData.Size())
		ulMaxLen = oData.Size() - ulOffset;
		return CByteArray(oData.GetBytes() + ulOffset, ulMaxLen);
	}
}

CByteArray CCard::ReadFile(const std::string & csPath,
	unsigned long ulOffset, unsigned long ulMaxLen,bool bDoNotCache)
{
	
	tCacheInfo cacheInfo = GetCacheInfo(csPath);
	//printf("Passed here...\n");
	if (cacheInfo.action == SIMPLE_CACHE || cacheInfo.action == CHECK_SERIAL)
	{
		//printf("=== Case 1\n");
		// Either read from cache or read from the card and store to cache.
		// We could optimise here if part of the file has already been
		// cached but not enough: in this case we can read the cached part
		// and the rest from the card and put both together (and then store
		// this to cache and return it).
		bool bFound;
		bool bFromDisk;
		std::string csName = m_oCache.GetSimpleName(GetSerialNr(), csPath);
		CByteArray oData = ReadCachedFile(csPath, csName, bFound,
			cacheInfo.action == CHECK_SERIAL ? 0         : ulOffset, 
			cacheInfo.action == CHECK_SERIAL ? FULL_FILE : ulMaxLen, 
			bFromDisk);
		if (bFound)
		{
			if (cacheInfo.action == CHECK_SERIAL && !SerialNrPresent(oData))
				throw CMWEXCEPTION(EIDMW_CACHE_TAMPERED);

			MWLOG(LEV_INFO, MOD_CAL, L"   Read file %ls (%d bytes) from cache",
				utilStringWiden(csPath).c_str(), oData.Size());
			if (cacheInfo.action == CHECK_SERIAL)
				return ReturnData(oData, ulOffset, ulMaxLen);
			else
				return oData;
		}
		else
		{
			oData = ReadUncachedFile(csPath, ulOffset, ulMaxLen);
			if(!bDoNotCache)
			{
				m_oCache.StoreFile(csName, oData, ulOffset == 0 && ulMaxLen == FULL_FILE);
				MWLOG(LEV_INFO, MOD_CAL, L"   Stored file %ls to cache",
					utilStringWiden(csPath).c_str());
			}
			return oData;
		}
	}
	else if (cacheInfo.action == CHECK_16_CACHE || cacheInfo.action == CERT_CACHE)
	{
		//printf("=== Case 2\n");
		// 1. In case of CHECK_16_CACHE:
		//    Compare 16 bytes starting at cacheInfo.ulOffset. If they are the
		//    same then return the cached data.
		// 2. In case of CERT_CACHE:
		//    Read the last 16 bytes of the cert from the card, and compare
		//    with the cached cert. If they are the same then return the cached
		//    data because at end of the cert is its signature, and if something
		//    changed in the cert then the signature will be completely different.
		// NOTE: this is only done if read from the hard disk; if the data has been
		// read from the hard disk before (and is already stored in memory) then
		// we won't do the above checks again.
		bool bFound;
		bool bFromDisk;
		unsigned long ulCheckOffset = 0xFFFFFFFF;
		std::string csName = m_oCache.GetSimpleName(GetSerialNr(), csPath);
		CByteArray oData = ReadCachedFile(csPath, csName, bFound,
			0, FULL_FILE, bFromDisk);

		if (bFound && !bFromDisk)
		{
			MWLOG(LEV_INFO, MOD_CAL, L"   Read file %ls (%d bytes) from memory cache",
				utilStringWiden(csPath).c_str(), oData.Size());
			return ReturnData(oData, ulOffset, ulMaxLen);
		}

		if (cacheInfo.action == CHECK_16_CACHE)
			ulCheckOffset = cacheInfo.ulOffset;
		else
		{
			// Our certs all start with 30 82 l1 l2 with [l1 l2] = the length of what follows
			if (oData.Size() > 4 && oData.GetByte(0) == 0x30 && oData.GetByte(1) == 0x82)
			{
				unsigned long ulCertLen = 256 * oData.GetByte(2) + oData.GetByte(3) + 4;
				if (ulCertLen > 16)
					ulCheckOffset = ulCertLen - 16;
			}
		}

		if (ulCheckOffset != 0xFFFFFFFF)
		{
			CByteArray oCertEnd = ReadUncachedFile(csPath, ulCheckOffset, 16);
			if (oCertEnd.Size() == 16 &&
				memcmp(oCertEnd.GetBytes(), oData.GetBytes() + ulCheckOffset, 16) == 0)
			{
				MWLOG(LEV_INFO, MOD_CAL, L"   Read file %ls (%d bytes) from disk cache",
					utilStringWiden(csPath).c_str(), oData.Size());
				m_oCache.StoreFileToMem(csName, oData, true);
				return ReturnData(oData, ulOffset, ulMaxLen);
			}
			else
			{
				MWLOG(LEV_INFO, MOD_CAL, L"   Cached file %ls differs from the "
					L"one on card, re-reading from card!", utilStringWiden(csPath).c_str());
			}
		}

		oData = ReadUncachedFile(csPath, ulOffset, ulMaxLen);
		if(!bDoNotCache)
		{
			m_oCache.StoreFile(csName, oData, ulMaxLen == FULL_FILE);
			MWLOG(LEV_INFO, MOD_CAL, L"   (Re)stored file %ls to cache",
				utilStringWiden(csPath).c_str());
		}

		return oData;
	}
	return ReadUncachedFile(csPath, ulOffset, ulMaxLen);
}

void CCard::WriteFile(const std::string &csPath, unsigned long ulOffset,
	const CByteArray & oData)
{
	WriteUncachedFile(csPath, ulOffset, oData);

	// We could overwrite the cache with the new data, but because
	// we don't know if it's the full file, we just clear the cached
	// data. This will cause a new ReadFile() to read again from the
	// card but as this probably won't happen that much...
	tCacheInfo cacheInfo = GetCacheInfo(csPath);
	if (cacheInfo.action == SIMPLE_CACHE)
		m_oCache.Delete(m_oCache.GetSimpleName(GetSerialNr(), csPath));
}

void CCard::WriteUncachedFile(const std::string &csPath, unsigned long ulOffset,
        const CByteArray & oData)
{
	throw CMWEXCEPTION(EIDMW_ERR_NOT_SUPPORTED);
}

tCacheInfo CCard::GetCacheInfo(const std::string &csPath)
{
	// By default no caching, card must implement this method
	// to allow certain files to be cached (in a certain way).
	tCacheInfo dontCache = {DONT_CACHE};

	return dontCache;
}

unsigned long CCard::PinStatus(const tPin & Pin)
{
	throw CMWEXCEPTION(EIDMW_ERR_NOT_SUPPORTED);
}

CByteArray CCard::RootCAPubKey(){
	throw CMWEXCEPTION(EIDMW_ERR_NOT_SUPPORTED);
}

bool CCard::Activate(const char *pinCode, CByteArray &BCDDate, bool blockActivationPIN) {
	throw CMWEXCEPTION(EIDMW_ERR_NOT_SUPPORTED);
}

bool CCard::unlockPIN(const tPin &pin, const tPin *puk, const char *pszPuk, const char *pszNewPin, unsigned long &triesLeft, 
	unsigned long unblockFlags) 
{
	throw CMWEXCEPTION(EIDMW_ERR_NOT_SUPPORTED);
}

bool CCard::PinCmd(tPinOperation operation, const tPin & Pin,
    const std::string & csPin1, const std::string & csPin2,
    unsigned long & ulRemaining, const tPrivKey *pKey,
    bool bShowDlg, void *wndGeometry, unsigned long unblockFlags)
{
	throw CMWEXCEPTION(EIDMW_ERR_NOT_SUPPORTED);
}

DlgPinUsage CCard::PinUsage2Dlg(const tPin & Pin, const tPrivKey *pKey)
{
	return DLG_PIN_UNKNOWN;
}

unsigned long CCard::GetSupportedAlgorithms()
{
	return 0;
}

CByteArray CCard::Sign(const tPrivKey & key, const tPin & Pin,
    unsigned long algo, const CByteArray & oData)
{
	throw CMWEXCEPTION(EIDMW_ERR_NOT_SUPPORTED);
}

CByteArray CCard::GetRandom(unsigned long ulLen)
{
	throw CMWEXCEPTION(EIDMW_ERR_NOT_SUPPORTED);
}

CByteArray CCard::SendAPDU(const CByteArray & oCmdAPDU)
{
	
	CAutoLock oAutoLock(this);
	long lRetVal = 0;

	CByteArray oResp = m_poContext->m_oPCSC.Transmit(m_hCard, oCmdAPDU, &lRetVal);

	if (lRetVal == SCARD_E_COMM_DATA_LOST || lRetVal == SCARD_E_NOT_TRANSACTED)
	{
		m_poContext->m_oPCSC.Recover(m_hCard, &m_ulLockCount);
		// try again to select the applet
		if (SelectApplet())
		{
			//try again, now that the card has been reset
			oResp = m_poContext->m_oPCSC.Transmit(m_hCard, oCmdAPDU, &lRetVal);
		}
	}

	if (oResp.Size() == 2)
	{
		// If SW1 = 0x61, then SW2 indicates the maximum value to be given to the
		// short Le  field (length of extra/ data still available) in a GET RESPONSE.
	    if (oResp.GetByte(0) == 0x61) {
		   return SendAPDU(0xC0, 0x00, 0x00, oResp.GetByte(1)); // Get Response
	    }
		// If SW1 = 0x6c, then SW2 indicates the value to be given to the short
		// Le field (exact length of requested data) when re-issuing the same command.
		if (oResp.GetByte(0) == 0x6c)
		{
			unsigned long ulCmdLen = oCmdAPDU.Size();
			const unsigned char *pucCmd = oCmdAPDU.GetBytes();
			CByteArray oNewCmdAPDU(ulCmdLen);
			oNewCmdAPDU.Append(pucCmd, 4);
			oNewCmdAPDU.Append(oResp.GetByte(1));
			if (ulCmdLen > 5)
			oNewCmdAPDU.Append(pucCmd + 5, ulCmdLen - 5);

			return SendAPDU(oNewCmdAPDU);
		}
	}

	return oResp;
}

CByteArray CCard::SendAPDU(unsigned char ucINS, unsigned char ucP1, unsigned char ucP2,
    unsigned long ulOutLen)
{
    
	CByteArray oAPDU(5);

    oAPDU.Append(m_ucCLA);
    oAPDU.Append(ucINS);
    oAPDU.Append(ucP1);
    oAPDU.Append(ucP2);
    oAPDU.Append((unsigned char) ulOutLen);

	//printf("%x : %x : %x : %x\n",oAPDU.GetByte(0),oAPDU.GetByte(1),oAPDU.GetByte(2),oAPDU.GetByte(3));
    
	return SendAPDU(oAPDU);
}

CByteArray CCard::SendAPDU(unsigned char ucINS, unsigned char ucP1, unsigned char ucP2,
        const CByteArray & oData)
{
    CByteArray oAPDU(5 + oData.Size());

    oAPDU.Append(m_ucCLA);
    oAPDU.Append(ucINS);
    oAPDU.Append(ucP1);
    oAPDU.Append(ucP2);
    oAPDU.Append((unsigned char) oData.Size());
    oAPDU.Append(oData);

    return SendAPDU(oAPDU);
}

unsigned char CCard::Hex2Byte(char cHex)
{
    if (cHex >= '0' && cHex <= '9')
        return (unsigned char) (cHex - '0');
    if (cHex >= 'A' && cHex <= 'F')
        return (unsigned char) (cHex - 'A' + 10);
    if (cHex >= 'a' && cHex <= 'f')
        return (unsigned char) (cHex - 'a' + 10);

    MWLOG(LEV_ERROR, MOD_CAL, L"Invalid hex character 0x%02X found", cHex);
    throw CMWEXCEPTION(EIDMW_ERR_BAD_PATH);
}

unsigned char CCard::Hex2Byte(const std::string & csHex, unsigned long ulIdx)
{
    return (unsigned char) (16 * Hex2Byte(csHex[2 * ulIdx]) + Hex2Byte(csHex[2 * ulIdx + 1]));
}

bool CCard::IsDigit(char c)
{
    return (c >= '0' && c <= '9');
}

unsigned long CCard::getSW12(const CByteArray & oRespAPDU, unsigned long ulExpected)
{
    unsigned long ulRespLen = oRespAPDU.Size();

    if (ulRespLen < 2)
    {
        MWLOG(LEV_ERROR, MOD_CAL, L"Only %d byte(s) returned from the card", ulRespLen);
        throw CMWEXCEPTION(EIDMW_ERR_CARD_COMM);
    }

    unsigned long ulSW12 = 256 * oRespAPDU.GetByte(ulRespLen - 2) + oRespAPDU.GetByte(ulRespLen - 1);

    if (ulExpected != 0 && ulExpected != ulSW12)
    {
        MWLOG(LEV_WARN, MOD_CAL, L"Card returned SW12 = %04X, expected %04X", ulSW12, ulExpected);
        throw CMWEXCEPTION(m_poContext->m_oPCSC.SW12ToErr(ulSW12));
    }

    return ulSW12;
}

////////////////////////////////////////////////////////////////:

CAutoLock::CAutoLock(CCard *poCard) : m_poCard(poCard), m_poPCSC(NULL), m_hCard(0)
{
	m_poCard->Lock();
}

CAutoLock::CAutoLock(CPCSC *poPCSC, SCARDHANDLE hCard) : m_poCard(NULL), m_poPCSC(poPCSC), m_hCard(hCard)
{
	poPCSC->BeginTransaction(hCard);
}

CAutoLock::~CAutoLock()
{
	if (m_poCard)
		m_poCard->Unlock();
	else
		m_poPCSC->EndTransaction(m_hCard);
}

}
