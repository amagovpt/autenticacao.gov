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
#include "PteidCard.h"
#include "../common/Log.h"
#ifdef __APPLE__
#include "UnknownCard.h"
#endif

using namespace eIDMW;

static const unsigned char IAS_PTEID_APPLET_AID[] = {0x60, 0x46, 0x32, 0xFF, 0x00, 0x01, 0x02};
static const unsigned char GEMSAFE_PTEID_APPLET_AID[] = {0x60, 0x46, 0x32, 0xFF, 0x00, 0x00, 0x02};

static const tFileInfo DEFAULT_FILE_INFO = {-1, -1, -1};
static const tFileInfo PREFS_FILE_INFO_V1 = {-1, -1, 1};
static const tFileInfo PREFS_FILE_INFO_V2 = {-1, -1, 0x85};

/* martinho - the id must not be changed */
static const unsigned long PTEIDNG_ACTIVATION_CODE_ID = 4;
/* martinho - ANY_ID_BIGGER_THAN_6 will be the ulID in the tPin struct 1-6 are already taken */
static const unsigned long ANY_ID_BIGGER_THAN_6 = 7;
/* martinho - some meaningful label */
static const string LABEL = "ACTIVATION_CODE";
/* martinho - date in bcd format must have 4 bytes*/
static const unsigned long BCDSIZE = 4;
/* martinho - trace file*/
static const string TRACEFILE = "3F000003";

unsigned long ulVersion;	

// If we want to 'hardcode' this plugin internally in the CAL, this function
// can't be present because it's the same for all plugins
#ifndef CARDPLUGIN_IN_CAL
CCard *GetCardInstance(unsigned long ulVersion, const char *csReader,
	unsigned long hCard, CContext *poContext, CPinpad *poPinpad)
{
	return PteidCardGetInstance(ulVersion, csReader, hCard, poContext, poPinpad);
}
#endif

static bool PteidCardSelectApplet(CContext *poContext, SCARDHANDLE hCard)
{
	long lRetVal = 0; 
	unsigned char tucSelectApp[] = {0x00, 0xA4, 0x04, 0x00};
	CByteArray oCmd(40);
	oCmd.Append(tucSelectApp, sizeof(tucSelectApp));
	oCmd.Append((unsigned char) sizeof(GEMSAFE_PTEID_APPLET_AID));
	oCmd.Append(GEMSAFE_PTEID_APPLET_AID, sizeof(GEMSAFE_PTEID_APPLET_AID));

	CByteArray oResp = poContext->m_oPCSC.Transmit(hCard, oCmd, &lRetVal);

	return (oResp.Size() == 2 && (oResp.GetByte(0) == 0x61 || oResp.GetByte(0) == 0x90));
}

static CByteArray ReadInternal(CPCSC *poPCSC, SCARDHANDLE hCard, unsigned long ulOffset, unsigned long ulMaxLen)
{

	long lretVal = 0;
	CByteArray oCmd(40);
	unsigned char tucReadDat[] = {0x00, 0xA4, 0x04, 0x0C};
	oCmd.Append(tucReadDat, sizeof(tucReadDat));
	oCmd.Append((unsigned char) sizeof(GEMSAFE_PTEID_APPLET_AID));
	oCmd.Append(GEMSAFE_PTEID_APPLET_AID, sizeof(GEMSAFE_PTEID_APPLET_AID));

	CByteArray oData = poPCSC->Transmit(hCard, oCmd, &lretVal);

	//oData.Chop(2); // remove SW12

	MWLOG(LEV_INFO, MOD_CAL, L"   Read %d bytes from the PT eid-ng GemSafe card", oData.Size());
	return oData;

}

static CByteArray ReadInternalIAS(CPCSC *poPCSC, SCARDHANDLE hCard, unsigned long ulOffset, unsigned long ulMaxLen)
{
	long lretVal = 0;
	CByteArray oCmd(40);
	unsigned char tucReadDat[] = {0x00, 0xA4, 0x04, 0x0C};
	oCmd.Append(tucReadDat, sizeof(tucReadDat));
	oCmd.Append((unsigned char) sizeof(IAS_PTEID_APPLET_AID));
	oCmd.Append(IAS_PTEID_APPLET_AID, sizeof(IAS_PTEID_APPLET_AID));

	CByteArray oData = poPCSC->Transmit(hCard, oCmd, &lretVal);

	//oData.Chop(2); // remove SW12

	MWLOG(LEV_INFO, MOD_CAL, L"   Read %d bytes from the PT eid-ng IAS card", oData.Size());
	return oData;
}

CCard *PTeidCardGetVersion (unsigned long ulVersion, const char *csReader,
	SCARDHANDLE hCard, CContext *poContext, CPinpad *poPinpad)
{
	CCard *poCard = NULL;
	bool bIsPtgemCard = false;
	CByteArray oData, oDataias;

	poContext->m_oPCSC.BeginTransaction(hCard);

	unsigned long ulReadLen = 404; // read everything
	oData = ReadInternal(&poContext->m_oPCSC, hCard, 0, ulReadLen);
	bIsPtgemCard = (oData.Size() == 2) && (oData.GetByte(0) == 0x90) && (oData.GetByte(1) == 0x00);

	poContext->m_oPCSC.EndTransaction(hCard);

	if (bIsPtgemCard) 
	{
		ulVersion = 1;
		poContext->m_oPCSC.EndTransaction(hCard);
		poCard = PteidCardGetInstance(ulVersion, csReader, hCard, poContext, poPinpad);
	} else {
		ulVersion = 2;
		poContext->m_oPCSC.BeginTransaction(hCard);
		oData = ReadInternalIAS(&poContext->m_oPCSC, hCard, 0, ulReadLen);
		poContext->m_oPCSC.EndTransaction(hCard);
		poCard = PteidCardGetInstance(ulVersion, csReader, hCard, poContext, poPinpad);
	}

	poContext->m_oPCSC.EndTransaction(hCard);
	
	return poCard;
}

CCard *PteidCardGetInstance(unsigned long ulVersion, const char *csReader,
	SCARDHANDLE hCard, CContext *poContext, CPinpad *poPinpad)
{

	CCard *poCard = NULL;

	if (ulVersion == 1)
	{
		try {
			bool bNeedToSelectApplet = false;
			CByteArray oData;
			CByteArray oCmd(40);
			// lmedinas CONFIRMED
			unsigned char tucSelectApp[] = {0x00, 0xA4, 0x04, 0x00};
			oCmd.Append(tucSelectApp, sizeof(tucSelectApp));
			oCmd.Append((unsigned char) sizeof(GEMSAFE_PTEID_APPLET_AID));
			oCmd.Append(GEMSAFE_PTEID_APPLET_AID, sizeof(GEMSAFE_PTEID_APPLET_AID));
			long lRetVal;
			// Don't remove these brackets, CAutoLock dtor must be called!
			{
				CAutoLock oAutLock(&poContext->m_oPCSC, hCard);

				oData = poContext->m_oPCSC.Transmit(hCard, oCmd, &lRetVal);
				if (lRetVal == SCARD_E_COMM_DATA_LOST || lRetVal == SCARD_E_NOT_TRANSACTED)
				{
					unsigned long ulLockCount = 0;
					poContext->m_oPCSC.Recover(hCard, &ulLockCount);

					bNeedToSelectApplet = PteidCardSelectApplet(poContext, hCard);
					if (bNeedToSelectApplet)// try again to select the belpic app
						oData = poContext->m_oPCSC.Transmit(hCard, oCmd,&lRetVal);
				}
				if (oData.Size() == 2 && oData.GetByte(0) == 0x6A &&
						(oData.GetByte(1) == 0x82 || oData.GetByte(1) == 0x86))
				{
					// Perhaps the applet is no longer selected; so try to select it
					// first; and if successfull then try to select the Belpic AID again
					bNeedToSelectApplet = PteidCardSelectApplet(poContext, hCard);
					if (bNeedToSelectApplet)
						oData = poContext->m_oPCSC.Transmit(hCard, oCmd,&lRetVal);
				}

				bool bIsPteidCard = oData.Size() == 2 && oData.GetByte(0) == 0x90 && oData.GetByte(1) == 0x00;

				if (bIsPteidCard)
				{
					ulVersion = 1;
					poCard = new CPteidCard(hCard, poContext, poPinpad, oData,
							bNeedToSelectApplet ? ALW_SELECT_APPLET : TRY_SELECT_APPLET, ulVersion);
					//#ifdef __APPLE__
				} else {
					// On Mac, if an unknown asynchronous card is inserted,
					// we don't return NULL but a CUnknownCard instance.
					// Reason: if we return NULL then the SISCardPlugin who
					// will be consulted next in card of a ACR38U reader
					// causes the reader/driver to get in a strange state
					// (if no SIS card is present) and if then a CUnknownCard
					// is instantiated, it will throw an exception if e.g.
					// SCardStatus() is called.
					// Remark: this trick won't work if synchronous card
					// (other then the SIS card is inserted).
					//return new CUnknownCard(hCard, poContext, poPinpad, CByteArray());
				}
				//#endif
			}
		}
		catch(...)
		{
			cout << "Exception in cardPluginPteid.CardGetInstance() \(...)" << endl;
		}

	} else {
		try {
			bool bNeedToSelectApplet = false;
			CByteArray oData;
			CByteArray oCmd(40);
			unsigned char tucSelectApp[] = {0x00, 0xA4, 0x04, 0x0C};
			oCmd.Append(tucSelectApp, sizeof(tucSelectApp));
			oCmd.Append((unsigned char) sizeof(IAS_PTEID_APPLET_AID));
			oCmd.Append(IAS_PTEID_APPLET_AID, sizeof(IAS_PTEID_APPLET_AID));

			//std::cout << "--------------------------------------This is read: " << oCmd.ToString() << " with bytes: " << oCmd.Size() <<"\n";
			long lRetVal;
			// Don't remove these brackets, CAutoLock dtor must be called!
			{
				CAutoLock oAutLock(&poContext->m_oPCSC, hCard);
				oData = poContext->m_oPCSC.Transmit(hCard, oCmd, &lRetVal);

				if (lRetVal == SCARD_E_COMM_DATA_LOST || lRetVal == SCARD_E_NOT_TRANSACTED)
				{
					unsigned long ulLockCount = 0;
					poContext->m_oPCSC.Recover(hCard, &ulLockCount);

					bNeedToSelectApplet = PteidCardSelectApplet(poContext, hCard);
					if (bNeedToSelectApplet)// try again to select the belpic app
						oData = poContext->m_oPCSC.Transmit(hCard, oCmd, &lRetVal);
				}
				if (oData.Size() == 2 && oData.GetByte(0) == 0x6A &&
						(oData.GetByte(1) == 0x82 || oData.GetByte(1) == 0x86))
				{
					// Perhaps the applet is no longer selected; so try to select it
					// first; and if successfull then try to select the Belpic AID again
					bNeedToSelectApplet = PteidCardSelectApplet(poContext, hCard);
					if (bNeedToSelectApplet)
						oData = poContext->m_oPCSC.Transmit(hCard, oCmd,&lRetVal);
				}

				bool bIsPteidCard = oData.Size() == 2 && oData.GetByte(0) == 0x90 && oData.GetByte(1) == 0x00;

				if (bIsPteidCard) {
					ulVersion = 2;
					poCard = new CPteidCard(hCard, poContext, poPinpad, oData,
							bNeedToSelectApplet ? ALW_SELECT_APPLET : TRY_SELECT_APPLET, ulVersion);
					//#ifdef __APPLE__
				} else {
					// On Mac, if an unknown asynchronous card is inserted,
					// we don't return NULL but a CUnknownCard instance.
					// Reason: if we return NULL then the SISCardPlugin who
					// will be consulted next in card of a ACR38U reader
					// causes the reader/driver to get in a strange state
					// (if no SIS card is present) and if then a CUnknownCard
					// is instantiated, it will throw an exception if e.g.
					// SCardStatus() is called.
					// Remark: this trick won't work if synchronous card
					// (other then the SIS card is inserted).
					//return new CUnknownCard(hCard, poContext, poPinpad, CByteArray());
				}
				//#endif
			}
		}
		catch(...)
		{
			cout << "Exception in cardPluginPteid.CardGetInstance() \(...)" << endl;
		}
	}

	return poCard;
}

CPteidCard::CPteidCard(SCARDHANDLE hCard, CContext *poContext,
		     CPinpad *poPinpad, const CByteArray & oData, tSelectAppletMode selectAppletMode, unsigned long ulVersion) :
CPkiCard(hCard, poContext, poPinpad)
{
		//printf("++++ Pteid3\n");

	switch (ulVersion){
		case 1:
			m_cardType = CARD_PTEID_IAS07;
			break;
		case 2:
			m_cardType = CARD_PTEID_IAS101;
			break;
	}
  try {
	  m_ucCLA = 0x00;
	  if (ulVersion == 1 )
	  {
		  m_oCardData = SendAPDU(0xCA, 0xDF, 0x30, 0x08);
	  } else {
		  m_oCardData = SendAPDU(0xCA, 0x02, 0x5A, 0x0D);
	  }
	  m_ucCLA = 0x00;

		//if (m_oCardData.Size() < 23)
		//lmedinas -- fixme for gemsafe v2
		/*if(m_oCardData.Size() < 15) //PT have 15 not 23 serial number...
		{
			//printf("... OK exception\n");
			throw CMWEXCEPTION(EIDMW_ERR_APPLET_VERSION_NOT_FOUND);
			}*/

		m_oCardData.Chop(2); // remove SW12 = '90 00'

		m_oSerialNr = CByteArray(m_oCardData.GetBytes(), m_oCardData.Size());
		// Get Card Applet Version
		m_AppletVersion = ulVersion;


		//m_ucAppletVersion = m_oCardData.GetByte(21);
        /*if (m_ucAppletVersion < 0x20)
            m_ucAppletVersion = (unsigned char) (16 * m_oCardData.GetByte(21) + m_oCardData.GetByte(22));

		m_ul6CDelay = 0;
		if (m_oCardData.GetByte(22) == 0x00 && m_oCardData.GetByte(23) == 0x01)
			m_ul6CDelay = 50;*/

		//m_selectAppletMode = selectAppletMode;
	}
    catch(CMWException e)
    {
		MWLOG(LEV_CRIT, MOD_CAL, L"Failed to get CardData: 0x%0x", e.GetError());
        Disconnect(DISCONNECT_LEAVE_CARD);
    }
    catch(...)
    {
        MWLOG(LEV_CRIT, MOD_CAL, L"Failed to get CardData");
        Disconnect(DISCONNECT_LEAVE_CARD);
    }
}

CPteidCard::~CPteidCard(void)
{
}

tCardType CPteidCard::GetType()
{
    return m_cardType;
}

CByteArray CPteidCard::GetSerialNrBytes()
{
    return m_oSerialNr;
}

CByteArray CPteidCard::GetInfo()
{
    return m_oCardData;
}

std::string CPteidCard::GetPinpadPrefix()
{
	return "pteidpp";
}

unsigned long CPteidCard::PinStatus(const tPin & Pin)
{
	long ulSW12 = 0;
	
    try
    {
        CByteArray oResp = SendAPDU(0x20, 0x00, (unsigned char) Pin.ulPinRef, 0);
        
	ulSW12 = getSW12(oResp);
	MWLOG(LEV_DEBUG, MOD_CAL, L"PinStatus APDU returned: %x", ulSW12 );
	if (ulSW12 == 0x9000)
		return 3; //Maximum Try Counter for PteID Cards

        return ulSW12 % 16;
    }
    catch(...)
    {
        //m_ucCLA = 0x00;
	MWLOG(LEV_ERROR, MOD_CAL, L"Error in PinStatus", ulSW12);
        throw;
    }
}

CByteArray CPteidCard::RootCAPubKey(){
	CByteArray oResp;

	try
	{
		CByteArray select("3F00",true);
		oResp = SendAPDU(0xA4, 0x00, 0x0C, select);
		getSW12(oResp, 0x9000);

		//4D - extended header list, 04 - size, FFA001 - SDO root CA, 80 - give me all?
		CByteArray getData("4D04FFA00180",true);
		oResp = SendAPDU(0xCB, 0x3F, 0xFF, getData);
		getSW12(oResp, 0x9000);
		oResp.Chop(2); //martinho: remove the returning code 0x9000
	}
	catch(...)
	{
		MWLOG(LEV_ERROR, MOD_CAL, L"Error in RootCAPubKey");
		throw;
	}
	return oResp;
}


bool CPteidCard::Activate(const char *pinCode, CByteArray &BCDDate){
	/* the activation code is not listed in the internal card structures */
	tPin activationPin = {true,LABEL,0,0,0,ANY_ID_BIGGER_THAN_6,0,0,8,8,8,PTEIDNG_ACTIVATION_CODE_ID,0x20,PIN_ENC_ASCII,"",""};
	unsigned long ulRemaining;

	bool bOK = PinCmd(PIN_OP_VERIFY, activationPin, pinCode, "", ulRemaining, NULL);
	if (!bOK)
		return bOK;

	if (BCDDate.Size() != BCDSIZE)
		return false;

	CByteArray oResp;
	CByteArray data(BCDDate);
	data.Append(0);
	data.Append(1); // data = day month year 0 1   -- 6 bytes written to 3F000003 trace file

	WriteFile(TRACEFILE,0,data);

	return true;
}



DlgPinUsage CPteidCard::PinUsage2Dlg(const tPin & Pin, const tPrivKey *pKey)
{
		//printf("++++ Pteid5\n");
	DlgPinUsage usage = DLG_PIN_UNKNOWN;

	if (pKey != NULL)
	{
		if (pKey->ulID == 2)
			usage = DLG_PIN_AUTH;
		else if (pKey->ulID == 3)
			usage = DLG_PIN_SIGN;
	}
	else if (Pin.ulID == 2)
		usage = DLG_PIN_SIGN;
	else
		usage = DLG_PIN_AUTH;

	return usage;
}

void CPteidCard::showPinDialog(tPinOperation operation, const tPin & Pin,
        std::string & csPin1, std::string & csPin2,	const tPrivKey *pKey)
{

		//printf("++++ Pteid6\n");
	// Convert params
	wchar_t wsPin1[PIN_MAX_LENGTH+1];
	wchar_t wsPin2[PIN_MAX_LENGTH+1];
	DlgPinOperation pinOperation = PinOperation2Dlg(operation);
	DlgPinUsage usage = PinUsage2Dlg(Pin, pKey);
	DlgPinInfo pinInfo = {Pin.ulMinLen, Pin.ulMaxLen, PIN_FLAG_DIGITS};

	// The actual call
	DlgRet ret;
	std::wstring wideLabel = utilStringWiden(Pin.csLabel);
	if (operation != PIN_OP_CHANGE)
	{
		ret = DlgAskPin(pinOperation,
			usage, wideLabel.c_str(), pinInfo, wsPin1,PIN_MAX_LENGTH+1);
	}
	else
	{
		ret = DlgAskPins(pinOperation,
			usage, wideLabel.c_str(),
			pinInfo, wsPin1,PIN_MAX_LENGTH+1, 
			pinInfo, wsPin2,PIN_MAX_LENGTH+1);
	}

	// Convert back
	if (ret == DLG_OK)
	{
		csPin1 = utilStringNarrow(wsPin1);
		if (operation == PIN_OP_CHANGE)
			csPin2 = utilStringNarrow(wsPin2);
	}
	else if (ret == DLG_CANCEL)
		throw CMWEXCEPTION(EIDMW_ERR_PIN_CANCEL);
	else if (ret == DLG_BAD_PARAM)
		throw CMWEXCEPTION(EIDMW_ERR_PARAM_BAD);
	else
		throw CMWEXCEPTION(EIDMW_ERR_UNKNOWN);

}

bool CPteidCard::PinCmd(tPinOperation operation, const tPin & Pin,
        const std::string & csPin1, const std::string & csPin2,
        unsigned long & ulRemaining, const tPrivKey *pKey, bool bShowDlg)
{
	bool pincheck;
    tPin pteidPin = Pin;
    // There's a path in the EF(AODF) for the PINs, but it's
    // not necessary, so we can save a Select File command
    pteidPin.csPath = "";
	// Encoding is Global Platform, there is/was no way to encode
	// this in PKCS15 AODF so it says/said erroneously "BCD encoding".
	//pteidPtrqin.encoding = PIN_ENC_BCD;
	pteidPin.encoding = PIN_ENC_ASCII; //PT uses ASCII only for PIN
	if (m_AppletVersion == 1 ) {
		pincheck = CPkiCard::PinCmd(operation, pteidPin, csPin1, csPin2, ulRemaining, pKey,bShowDlg);
	} else {
		pincheck = CPkiCard::PinCmdIAS(operation, pteidPin, csPin1, csPin2, ulRemaining, pKey,bShowDlg);
	}

	return pincheck;
}

bool CPteidCard::LogOff(const tPin & Pin)
{
    m_ucCLA = 0x80;
	// No PIN has to be specified
    CByteArray oResp = SendAPDU(0xE6, 0x00, 0x00, 0);
    m_ucCLA = 0x00;

	getSW12(oResp, 0x9000);

	return true;
}

unsigned long CPteidCard::GetSupportedAlgorithms()
{
	unsigned long ulAlgos =
		SIGN_ALGO_RSA_PKCS | SIGN_ALGO_SHA1_RSA_PKCS;

	return ulAlgos;
}

CByteArray CPteidCard::Ctrl(long ctrl, const CByteArray & oCmdData)
{

    CAutoLock oAutoLock(this);

    switch(ctrl)
    {
    case CTRL_PTEID_GETCARDDATA:
        return m_oCardData;
    case CTRL_PTEID_GETSIGNEDCARDDATA:
		if (m_ucAppletVersion < 0x20)
			throw CMWEXCEPTION(EIDMW_ERR_NOT_SUPPORTED);
		else
		{
			if (m_selectAppletMode == ALW_SELECT_APPLET)
				SelectApplet();
			m_ucCLA = 0x80;
			CByteArray oRet = SendAPDU(0xE4, 0x02, 0x00, 0x9C);
			m_ucCLA = 0;
			getSW12(oRet, 0x9000);
			oRet.Chop(2);
			return oRet;
		}
    case CTRL_PTEID_GETSIGNEDPINSTATUS:
		// oCmdData must contain:
		// - the pin reference (1 byte)
		if (m_ucAppletVersion < 0x20)
			throw CMWEXCEPTION(EIDMW_ERR_NOT_SUPPORTED);
		else
		{
			if (m_selectAppletMode == ALW_SELECT_APPLET)
				SelectApplet();
			unsigned char ucPinRef = oCmdData.GetByte(0);
			m_ucCLA = 0x80;
			CByteArray oRet = SendAPDU(0xEA, 0x02, ucPinRef, 0x81);
			m_ucCLA = 0;
			if (ShouldSelectApplet(0xEA, getSW12(oRet)))
			{
				if (SelectApplet())
				{
					m_selectAppletMode = ALW_SELECT_APPLET;
					m_ucCLA = 0x80;
					CByteArray oRet = SendAPDU(0xEA, 0x02, ucPinRef, 0x81);
					m_ucCLA = 0;
				}
			}
			getSW12(oRet, 0x9000);
			oRet.Chop(2);
			return oRet;
		}
	case CTRL_PTEID_INTERNAL_AUTH:
		// oCmdData must contain:
		// - the key reference (1 byte)
		// - the challenge to be signed (20 bytes)
		if (oCmdData.Size() != 36)
		    throw CMWEXCEPTION(EIDMW_ERR_PARAM_BAD);
		else
		{
		    //if (m_selectAppletMode == ALW_SELECT_APPLET)
		    //SelectApplet();
			//unsigned char ucKeyRef = oCmdData.GetByte(0);
			CByteArray oData;
			oData.Append(0x08);
			oData.Append(oCmdData);
			oData.Append(0x88);
			m_ucCLA = 0x8C;
			CByteArray oRet = SendAPDU(0x88, 0x00, 0x00, oData);
			/*if (ShouldSelectApplet(0x88, getSW12(oRet)))
			{
				if (SelectApplet())
				{
					m_selectAppletMode = ALW_SELECT_APPLET;
					CByteArray oRet = SendAPDU(0x88, 0x02, ucKeyRef, oData);
				}
				}*/
			getSW12(oRet, 0x9000);
			oRet.Chop(2);
			return oRet;
			}
    default:
        MWLOG(LEV_WARN, MOD_CAL, L"Ctrl(): Unknown CRTL code %d (0x%0x) specified", ctrl, ctrl);
		throw CMWEXCEPTION(EIDMW_ERR_PARAM_BAD);
    }
}

CP15Correction* CPteidCard::GetP15Correction()
{
	return &p15correction;
}

tFileInfo CPteidCard::ParseFileInfo(CByteArray & oFCI)
{
	// We should never come here
	throw CMWEXCEPTION(EIDMW_ERR_CHECK);
}

void CPteidCard::IasSignatureHelper()
{

	CByteArray oData99;
	oData99.Append(0x60);
	oData99.Append(0x46);
	oData99.Append(0x32);
	oData99.Append(0xFF);
	oData99.Append(0x00);
	oData99.Append(0x01);
	oData99.Append(0x02);
	CByteArray oResp99 = SendAPDU(0xA4, 0x04, 0x00, oData99);

	CByteArray oDataspec;
	oDataspec.Append(0x00);
	oDataspec.Append(0xA4);
	oDataspec.Append(0x03);
	oDataspec.Append(0x0C);
	CByteArray oRespspec = SendAPDU(oDataspec);

	CByteArray oData98;
	oData98.Append(0x2F);
	oData98.Append(0x00);
	CByteArray oResp98 = SendAPDU(0xA4, 0x09, 0x00, oData98);
	oResp98.Chop(2);

	CByteArray oResp97 = SendAPDU(0xB0, 0x00, 0x00, 0x3E);

	CByteArray oDataroot;
	oDataroot.Append(0x4F);
	oDataroot.Append(0x00);
	CByteArray oResproot = SendAPDU(0xA4, 0x09, 0x0C, oDataroot);

	CByteArray oDataia98;
	oDataia98.Append(0x50);
	oDataia98.Append(0x32);
	CByteArray oRespia98 = SendAPDU(0xA4, 0x09, 0x00, oDataia98);

	CByteArray oRespia95 = SendAPDU(0xB0, 0x00, 0x00, 0x2F);

	//Get Challenge
	CByteArray oRespcha1 = SendAPDU(0x84, 0x00, 0x00, 0x08);
	oRespcha1.Chop(2);
	CByteArray oRespcha2 = SendAPDU(0x84, 0x00, 0x00, 0x08);
	oRespcha2.Chop(2);
	CByteArray oRespcha3 = SendAPDU(0x84, 0x00, 0x00, 0x08);
	oRespcha3.Chop(2);

	//Next blocks are helpers only for signature
	CByteArray oData123;
	oData123.Append(0x00);
	oData123.Append(0xA4);
	oData123.Append(0x03);
	oData123.Append(0x0C);
	CByteArray oResp123 = SendAPDU(oData123);

	CByteArray oData456;
	oData456.Append(0x5F);
	oData456.Append(0x00);
	CByteArray oResp456 = SendAPDU(0xA4, 0x09, 0x0C, oData456);

}

void CPteidCard::SetSecurityEnv(const tPrivKey & key, unsigned long algo,
    unsigned long ulInputLen)
{
    printf("++++ Pteid11\n");
	// Data = [04 80 <algoref> 84 <keyref>]  (5 bytes)
    CByteArray oDataias, oDatagem;
    unsigned char ucAlgo;
    CByteArray oResp;

    m_ucCLA = 0x00;

    if (m_AppletVersion == 1) {
	oDatagem.Append(0x80);
	oDatagem.Append(0x01);
    	oDatagem.Append(0x02); //Algorithm: RSA with PKCS#1 Padding
    	oDatagem.Append(0x84);
    	oDatagem.Append(0x01);
    	oDatagem.Append((unsigned char) key.ulKeyRef);
    	oResp = SendAPDU(0x22, 0x41, 0xB6, oDatagem);
    } else {

    	if (ulInputLen != 36)
    		IasSignatureHelper();

    	oDataias.Append(0x95);
    	oDataias.Append(0x01);
    	oDataias.Append(0x40);
    	oDataias.Append(0x84);
    	oDataias.Append(0x01);
    	oDataias.Append(key.ulKeyRef);
    	oDataias.Append(0x80);
    	oDataias.Append(0x01);
    	oDataias.Append(0x02);

    	oResp = SendAPDU(0x22, 0x41, 0xA4, oDataias);
    }

    unsigned long ulSW12 = getSW12(oResp);

    getSW12(oResp, 0x9000);
}

CByteArray CPteidCard::SignInternal(const tPrivKey & key, unsigned long algo,
    const CByteArray & oData, const tPin *pPin)
{
    // printf("++++ Pteid12\n");
    CAutoLock autolock(this);

    m_ucCLA = 0x00;

    // Pretty unique for smart cards: first MSE SET, then verify PIN
    // (needed for the nonrep key/pin, but also usable for the auth key/pin)
    if (pPin != NULL)
    {
        unsigned long ulRemaining = 0;
        bool bOK = PinCmd(PIN_OP_VERIFY, *pPin, "", "", ulRemaining, &key);
        if (!bOK)
			throw CMWEXCEPTION(ulRemaining == 0 ? EIDMW_ERR_PIN_BLOCKED : EIDMW_ERR_PIN_BAD);
    }
	

    SetSecurityEnv(key, algo, oData.Size());

    CByteArray oData1;
   
	oData1.Append(0x90); //SHA-1 Hash as Input
	oData1.Append(oData.Size());
    
    oData1.Append(oData);

    CByteArray oResp, oResp1;

    if (m_AppletVersion == 1) {
    	// PSO: Hash GEMSAFE
    	oResp1 = SendAPDU(0x2A, 0x90, 0xA0, oData1);
    	// PSO: Compute Digital Signature GEMSAFE
		oResp = SendAPDU(0x2A, 0x9E, 0x9A, 0x80);
    } else {
    	// PSO:Hash IAS - does CDS
    	oResp = SendAPDU(0x88, 0x02, 0x00, oData);
    }

    unsigned long ulSW12 = getSW12(oResp);
    MWLOG(LEV_INFO, MOD_CAL, L"Resp oResp PSO is: 0x%2X", ulSW12);
    if (ulSW12 != 0x9000)
    	throw CMWEXCEPTION(m_poContext->m_oPCSC.SW12ToErr(ulSW12));
	
    // Remove SW1-SW2 from the response
    oResp.Chop(2);

    return oResp;
}

bool CPteidCard::ShouldSelectApplet(unsigned char ins, unsigned long ulSW12)
{
		//printf("++++ Pteid13\n");
	if (m_selectAppletMode != TRY_SELECT_APPLET)
		return false;

	if (ins == 0xA4)
		return ulSW12 == 0x6A82 || ulSW12 == 0x6A86;
	return ulSW12 == 0x6A82 || ulSW12 == 0x6A86 || ulSW12 == 0x6D00;
}

bool CPteidCard::SelectApplet()
{
	return PteidCardSelectApplet(m_poContext, m_hCard);
}

tBelpicDF CPteidCard::getDF(const std::string & csPath, unsigned long & ulOffset)
{
		//printf("++++ Pteid14\n");
	ulOffset = 0;
	if (csPath.substr(0, 4) == "3F00")
		ulOffset = 4;

	/*if (ulOffset < csPath.size())
	{
		std::string csPartialPath = csPath.substr(ulOffset, 4);
		if (csPartialPath == "DF00")
			return BELPIC_DF;
		if (csPartialPath == "DF01" && m_ucAppletVersion >= 0x20)
			return ID_DF; // this AID doesn't exist for V1 cards
	}*/

	return UNKNOWN_DF;
}

tFileInfo CPteidCard::SelectFile(const std::string & csPath, bool bReturnFileInfo)
{
		//printf("++++ Pteid15 select file: ");
		CPkiCard::SelectFile(csPath, false);

	// The EF(Preferences) file can be written using the authentication PIN;
	// that's the only exception to the 'read always' - 'write never' ACs.
	if (csPath.substr(csPath.size() - 4, 4) == "4039")
	{
		if (m_ucAppletVersion < 0x20)
			return PREFS_FILE_INFO_V1;
		else
			return PREFS_FILE_INFO_V2;
	}
	else
		return DEFAULT_FILE_INFO;
}

/**
 * The Belpic card doesn't support select by path (only
 * select by File ID or by AID) , so we make a loop with
 * 'select by file' commands.
 * E.g. if path = AAAABBBCCC the we do
 *   Select(AAAA)
 *   Select(BBBB)
 *   Select(CCCC)
 * If the the path contains the Belpic DF (DF00) or
 * the ID DF (DF01) then we select by AID without
 * first selected the MF (3F00) even it it's specified
 * because selection by AID always works.
 */
CByteArray CPteidCard::SelectByPath(const std::string & csPath, bool bReturnFileInfo)
{
		//printf("++++ Pteid16\n");
	unsigned long ulOffset = 0;
	tBelpicDF belpicDF = getDF(csPath, ulOffset);

	if (belpicDF == UNKNOWN_DF)
	{
		// 1. Do a loop of "Select File by file ID" commands

		unsigned long ulPathLen = (unsigned long) csPath.size() / 2;
		for (ulOffset = 0; ulOffset < ulPathLen; ulOffset += 2)
		{
			CByteArray oPath(ulPathLen);
			oPath.Append(Hex2Byte(csPath, ulOffset));
			oPath.Append(Hex2Byte(csPath, ulOffset + 1));
			
			CByteArray oResp = SendAPDU(0xA4, 0x00, 0x0C, oPath);
			unsigned long ulSW12 = getSW12(oResp);
			if ((ulSW12 == 0x6A82 || ulSW12 == 0x6A86) && m_selectAppletMode == TRY_SELECT_APPLET)
			{
				// The file still wasn't found, so let's first try to select the applet
				/*if (SelectApplet())
				{
				m_selectAppletMode = ALW_SELECT_APPLET;*/
					oResp = SendAPDU(0xA4, 0x00, 0x0C, oPath);
					//}
			}
			getSW12(oResp, 0x9000);
		}
	}
	else
	{
		// 2.a Select the BELPIC DF or the ID DF by AID

		CByteArray oAID(20);
		if (belpicDF == BELPIC_DF) {
		  	if (ulVersion == 1)
			{
			  oAID.Append(GEMSAFE_PTEID_APPLET_AID, sizeof(GEMSAFE_PTEID_APPLET_AID));
			} else {
			  oAID.Append(IAS_PTEID_APPLET_AID, sizeof(IAS_PTEID_APPLET_AID));
			}
		} else {
		  cout << "Think what to do" << endl;
		}
		CByteArray oResp;
		if (ulVersion == 1) 
		{
		    oResp = SendAPDU(0xA4, 0x80, 0x00, oAID);
		} else {
		    oResp = SendAPDU(0xA4, 0x04, 0x0C, oAID);
		}
		  unsigned long ulSW12 = getSW12(oResp);
		if ((ulSW12 == 0x6A82 || ulSW12 == 0x6A86) && m_selectAppletMode == TRY_SELECT_APPLET)
		{
			// The file still wasn't found, so let's first try to select the applet
			if (SelectApplet())
			{
				m_selectAppletMode = ALW_SELECT_APPLET;
				if (ulVersion == 1) 
				{
				    oResp = SendAPDU(0xA4, 0x80, 0x00, oAID);
			        } else {
				    oResp = SendAPDU(0xA4, 0x04, 0x0C, oAID);
				}
			}
		}
		getSW12(oResp, 0x9000);

		// 2.b Select the file inside the DF, if needed

		ulOffset += 4;
		if (ulOffset + 4 == csPath.size())
		{
			CByteArray oPath(2);
			oPath.Append(Hex2Byte(csPath, ulOffset / 2));
			oPath.Append(Hex2Byte(csPath, ulOffset / 2 + 1));

			if (ulVersion == 1) 
			{
			    CByteArray oResp = SendAPDU(0xA4, 0x00, 0x00, oPath);
			} else {
			    CByteArray oResp = SendAPDU(0xA4, 0x02, 0x0C, oPath);
			}
			unsigned long ulSW12 = getSW12(oResp);
			if (ulSW12 != 0x9000)
				throw CMWEXCEPTION(m_poContext->m_oPCSC.SW12ToErr(ulSW12));
		}
	}

	// Normally we should put here the FCI, but since Belpic cards
	// don't return it, we just return the path that can be used
	// later on to return the harcoded FCI for that file.
	return CByteArray((unsigned char *) csPath.c_str(), (unsigned long) csPath.size());
}

unsigned long CPteidCard::Get6CDelay()
{
	return m_ul6CDelay;
}

tCacheInfo CPteidCard::GetCacheInfo(const std::string &csPath)
{
	tCacheInfo dontCache = {DONT_CACHE, 0};
	tCacheInfo simpleCache = {SIMPLE_CACHE, 0};
	tCacheInfo certCache = {CERT_CACHE, 0};
	tCacheInfo check16Cache = {CHECK_16_CACHE, 0}; // Check 16 bytes at offset 0
	tCacheInfo checkSerial = {CHECK_SERIAL, 0}; // Check if the card serial nr is present

	// csPath -> file ID
	unsigned int uiFileID = 0;
	unsigned long ulLen = (unsigned long) (csPath.size() / 2);
	if (ulLen >= 2)
	  uiFileID = Hex2Byte(csPath, ulLen - 2) + Hex2Byte(csPath, ulLen - 1);

	switch(uiFileID)
	{
	case 0x2F00: // EF(DIR)
	  return simpleCache;
	case 129: // EF(ODF) 4F005031 (Dont cache otherwise will cause issues on IAS cards)
	  return dontCache;
	case 0x5032: // EF(TokenInfo)
	case 69: // AOD (4401)
	case 3: // 0003 (TRACE)
	case 246: // EF07 (PersoData)
	  return simpleCache;
	case 244: // EF05 (Address)
	  return dontCache;
	case 241: // EF02 (ID)
	case 245: // EF06 (SOD)
#ifdef CAL_EMULATION  // the EF(ID#RN) of the emulated test cards have the same serial nr
	case 0x4031: // EF(ID#RN)
#endif
		return simpleCache;
#ifndef CAL_EMULATION
	case 0x4031: // EF(ID#RN)
		return checkSerial;
#endif	
	case 251: // EF0C (CertD)
	case 248: // EF09 (Cert Auth)
	case 247: // EF08 (Cert Sign)
	case 254: // EF0F (Cert Root Sign)
	case 255: // EF10 (Cert Root Auth)
	case 256: // EF11 (CERT ROOT CA)
		return certCache;
	}
}
