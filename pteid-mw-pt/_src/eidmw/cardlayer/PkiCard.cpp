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
#include <fstream>
#include <iostream>

#include "PkiCard.h"
#include "../common/Log.h"
#include "../common/Thread.h"
#include "pinpad2.h"


unsigned char clearbinary[] = { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                                0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                                0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                                0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                                0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                                0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                                0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                                0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                                0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                                0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                                0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                                0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                                0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                                0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                                0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                                0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

namespace eIDMW
{

CPkiCard::CPkiCard(SCARDHANDLE hCard, CContext *poContext, CPinpad *poPinpad) :
CCard(hCard, poContext, poPinpad)
{
   	m_ucCLA = 0;
	m_selectAppletMode = DONT_SELECT_APPLET;

}

CPkiCard::~CPkiCard(void)
{
}

bool CPkiCard::IsPinpadReader()
{
    return m_poPinpad->UsePinpad(PIN_OP_VERIFY);
}

bool CPkiCard::ShouldSelectApplet(unsigned char ins, unsigned long ulSW12)
{
	// Don't do anything by default
	return false;
}

bool CPkiCard::SelectApplet()
{
	// Don't do anything by default
	// Subclasses that implement this functionality should set the 
	// m_selectAppletMode to TRY_SELECT_APPLET
	return false;
}

void CPkiCard::SelectApplication(const CByteArray & oAID)
{
    	CAutoLock autolock(this);

	if (m_selectAppletMode == ALW_SELECT_APPLET)
		SelectApplet();

	// Select File command to select the Application by AID
    	CByteArray oResp = SendAPDU(0xA4, 0x04, 0x0C, oAID);

	if (ShouldSelectApplet(0xA4, getSW12(oResp)))
	{
		// First try to select the applet
		if (SelectApplet())
		{
			m_selectAppletMode = ALW_SELECT_APPLET;
			oResp = SendAPDU(0xA4, 0x04, 0x0C, oAID);
		}
	}

	getSW12(oResp, 0x9000);
}

CByteArray CPkiCard::ReadUncachedFile(const std::string & csPath,
    unsigned long ulOffset, unsigned long ulMaxLen)
{
	//printf("CPkiCard ulOffset: %ld - ulMaxLen: %ld\n",ulOffset,ulMaxLen);
    	//std::cout << "csPath: " << csPath << "\n";

	CByteArray oData(ulMaxLen);

	CAutoLock autolock(this);

    tFileInfo fileInfo = SelectFile(csPath, true);
	
    // Loop until we've read ulMaxLen bytes or until EOF (End Of File)
    bool bEOF = false;
    for (unsigned long i = 0; i < ulMaxLen && !bEOF; i += MAX_APDU_READ_LEN)
    {
        unsigned long ulLen = ulMaxLen - i <= MAX_APDU_READ_LEN ?
	    ulMaxLen - i : 0;
	
	
        CByteArray oResp = ReadBinary(ulOffset + i, ulLen);


        unsigned long ulSW12 = getSW12(oResp);


		// If the file is a multiple of the block read size, you will get
		// an SW12 = 6B00 (at least with PT eID) but that OK then..
        if (ulSW12 == 0x9000 || (i != 0 && ulSW12 == 0x6B00))
            oData.Append(oResp.GetBytes(), oResp.Size() - 2);
		else if (ulSW12 == 0x6982) {
			throw CNotAuthenticatedException(
				EIDMW_ERR_NOT_AUTHENTICATED, fileInfo.lReadPINRef);
		}
		else if (ulSW12 == 0x6B00)
			throw CMWEXCEPTION(EIDMW_ERR_PARAM_RANGE);
		else if (ulSW12 == 0x6D00)
			throw CMWEXCEPTION(EIDMW_ERR_NOT_ACTIVATED);
		else
            throw CMWEXCEPTION(m_poContext->m_oPCSC.SW12ToErr(ulSW12));

        // If the driver/reader itself did the 6CXX handling,
        // we assume we're at the EOF
        if (oResp.Size() < MAX_APDU_READ_LEN)
            bEOF = true;
    }

	MWLOG(LEV_INFO, MOD_CAL, L"   Read file %ls (%d bytes) from card",
		utilStringWiden(csPath).c_str(), oData.Size());

	//printf(".... Read file %ls (%d bytes) from card\n",utilStringWiden(csPath).c_str(),oData.Size());

    return oData;
}

void CPkiCard::WriteUncachedFile(const std::string & csPath,
    unsigned long ulOffset, const CByteArray & oData)
{
    CAutoLock autolock(this);

    CByteArray oDatan = oData;

    tFileInfo fileInfo = SelectFile(csPath, true);

    const unsigned char *pucData = oDatan.GetBytes();
    unsigned long ulDataLen = oDatan.Size();

    if(ulDataLen == 0)
    {
        CByteArray oDataVoid;
        oDataVoid.Append(clearbinary, sizeof(clearbinary));
        SendAPDU(0xD6, 0x00, 0x00, oDataVoid);

    }

    bool bEOF = false;

    for (unsigned long i = 0; i < PERSODATAFILESIZE && !bEOF && ulDataLen != 0; i += MAX_APDU_WRITE_LEN)
    {
        unsigned long ulLen = ulDataLen - i + 2;
        if (ulLen > MAX_APDU_WRITE_LEN)
            ulLen = MAX_APDU_WRITE_LEN;

        CByteArray oResp = UpdateBinary(ulOffset + i, CByteArray(pucData + i, ulLen));
        unsigned long ulSW12 = getSW12(oResp);

        if (ulSW12 == 0x9000 || (i != 0 && ulSW12 == 0x6B00))
        {
            oDatan.Append(oResp.GetBytes(), oResp.Size());
        }
        else if (ulSW12 == 0x6982) {
            throw CNotAuthenticatedException(
                        EIDMW_ERR_NOT_AUTHENTICATED, fileInfo.lReadPINRef);
        }
        else if (ulSW12 == 0x6B00)
            throw CMWEXCEPTION(EIDMW_ERR_PARAM_RANGE);
        else if (ulSW12 == 0x6D00)
            throw CMWEXCEPTION(EIDMW_ERR_NOT_ACTIVATED);

        else if (ulSW12 == 0x6282)
            bEOF = false;
        else
            throw CMWEXCEPTION(m_poContext->m_oPCSC.SW12ToErr(ulSW12));
    }

        MWLOG(LEV_INFO, MOD_CAL, L"Written file %ls to card", utilStringWiden(csPath).c_str());
}

unsigned char CPkiCard::PinUsage2Pinpad(const tPin & Pin, const tPrivKey *pKey)
{
	//Its hackish but works for IAS and Gemsafe Cards
	return (char)Pin.ulID;
}

bool CPkiCard::PinCmd(tPinOperation operation, const tPin & Pin,
        const std::string & csPin1, const std::string & csPin2,
        unsigned long & ulRemaining, const tPrivKey *pKey, bool bShowDlg)
{
	// No standard for Logoff, so each card has to implement
	// it's own command here.
	if (operation == PIN_OP_LOGOFF )
		return LogOff(Pin);

	bool bRet = false;
	std::string csReadPin1, csReadPin2;
	const std::string *pcsPin1 = &csPin1;
	const std::string *pcsPin2 = &csPin2;
	bool bAskPIN = csPin1.empty();
	bool bUsePinpad = bAskPIN ? m_poPinpad->UsePinpad(operation) : false;

bad_pin:
    // If no Pin(s) provided and it's no Pinpad reader -> ask Pins
    if (bAskPIN && !bUsePinpad)
	{
        showPinDialog(operation, Pin, csReadPin1, csReadPin2, pKey);
		pcsPin1 = &csReadPin1;
		pcsPin2 = &csReadPin2;
	}

    CByteArray oPinBuf = MakePinBuf(Pin, *pcsPin1, bUsePinpad);
    if (operation != PIN_OP_VERIFY)
        oPinBuf.Append(MakePinBuf(Pin, *pcsPin2, bUsePinpad));

    CByteArray oAPDU = MakePinCmd(operation, Pin); // add CLA, INS, P1, P2
    oAPDU.Append((unsigned char) oPinBuf.Size());  // add P3
    oAPDU.Append(oPinBuf);

	CByteArray oResp;
	bool bSelected = false;

	// Don't remove these brackets!!
	{
		CAutoLock autolock(this);

		// Select the path where the Pin is, if necessary
		if (!Pin.csPath.empty() && !bSelected && Pin.csPath != "3F00")
		{
			SelectFile(Pin.csPath);
			bSelected = true;
		}

		// Send the command
		if (csPin1.empty() && bUsePinpad)
			oResp = m_poPinpad->PinCmd(operation, Pin,
			PinUsage2Pinpad(Pin, pKey), oAPDU, ulRemaining, bShowDlg);
		else
			oResp = SendAPDU(oAPDU);
	}

    unsigned long ulSW12 = getSW12(oResp);
    if (ulSW12 == 0x9000)
        bRet = true;
    else if (ulSW12 == 0x6983)
        ulRemaining = 0;
    else if (ulSW12 / 16 == 0x63C)
        ulRemaining = ulSW12 % 16;
	else
		throw CMWEXCEPTION(m_poContext->m_oPCSC.SW12ToErr(ulSW12));

	// Bad PIN: show a dialog to ask the user to try again
	// PIN blocked: show a dialog to tell the user
	if (bAskPIN && !bRet)
	{
		DlgPinUsage usage = PinUsage2Dlg(Pin, pKey);
		DlgRet dlgret = DlgBadPin(usage, utilStringWiden(Pin.csLabel).c_str(), ulRemaining);
		if (0 != ulRemaining && DLG_RETRY == dlgret)
			goto bad_pin;
	}

	// If PIN command OK and no SSO, then state that we have now
	// verified this PIN, this info is needed in the Sign() method
	if (bRet && !m_poContext->m_bSSO)
	{
		bool bFound = false;
		for (size_t i = 0; i < m_verifiedPINs.size() && !bFound; i++)
			bFound = (m_verifiedPINs[i] == Pin.ulID);
		if (!bFound)
			m_verifiedPINs.push_back(Pin.ulID);
	}

	return bRet;
}

bool CPkiCard::PinCmdIAS(tPinOperation operation, const tPin & Pin,
        const std::string & csPin1, const std::string & csPin2,
        unsigned long & ulRemaining, const tPrivKey *pKey, bool bShowDlg)
{
	// No standard for Logoff, so each card has to implement
	// it's own command here.
	if (operation == PIN_OP_LOGOFF )
		return LogOff(Pin);

	bool bRet = false;
	std::string csReadPin1, csReadPin2;
	const std::string *pcsPin1 = &csPin1;
	const std::string *pcsPin2 = &csPin2;
	bool bAskPIN = csPin1.empty();
	bool bUsePinpad = bAskPIN ? m_poPinpad->UsePinpad(operation) : false;

bad_pin:
    // If no Pin(s) provided and it's no Pinpad reader -> ask Pins
    if (bAskPIN && !bUsePinpad)
	{
        showPinDialog(operation, Pin, csReadPin1, csReadPin2, pKey);
		pcsPin1 = &csReadPin1;
		pcsPin2 = &csReadPin2;
	}

    CByteArray oPinBuf = MakePinBuf(Pin, *pcsPin1, bUsePinpad);
    CByteArray oAPDU;
    CByteArray oAPDUCHANGE;
    CByteArray oResp;

	//00 A4 04 00 07 60 46 32 FF 00 01 02
	CByteArray init;
	init.Append(0x00);
	init.Append(0xA4);
	init.Append(0x04);
	init.Append(0x00);
	init.Append(0x07);
	init.Append(0x60);
	init.Append(0x46);
	init.Append(0x32);
	init.Append(0xFF);
	init.Append(0x00);
	init.Append(0x01);
	init.Append(0x02);
	SendAPDU(init);


	switch(operation){
	case PIN_OP_VERIFY:
	case PIN_OP_RESET:
		oAPDU = MakePinCmdIAS(operation, Pin); // add CLA, INS, P1, P2
		oAPDU.Append((unsigned char) oPinBuf.Size());  // add P3
		oAPDU.Append(oPinBuf);
		break;
	case PIN_OP_CHANGE:
		oAPDU = MakePinCmdIAS(PIN_OP_VERIFY, Pin); // add CLA, INS, P1, P2
		oAPDU.Append((unsigned char) oPinBuf.Size());  // add P3
		oAPDU.Append(oPinBuf);
		oAPDUCHANGE = MakePinCmdIAS(operation, Pin); // add CLA, INS, P1, P2
		oAPDUCHANGE.Append((unsigned char) oPinBuf.Size());  // add P3
		oAPDUCHANGE.Append(MakePinBuf(Pin, *pcsPin2, bUsePinpad));
		break;
	}

	bool bSelected = false;

	// Don't remove these brackets!!
	{
		CAutoLock autolock(this);

		// Select the path where the Pin is, if necessary
		if (!Pin.csPath.empty() && !bSelected && Pin.csPath != "3F00")
		{
			SelectFile(Pin.csPath);
			bSelected = true;
		}

		// Send the command
		if (csPin1.empty() && bUsePinpad) {
			oResp = m_poPinpad->PinCmd(operation, Pin,
			PinUsage2Pinpad(Pin, pKey), oAPDU, ulRemaining, bShowDlg);
		} else {
			switch(operation){
			case PIN_OP_VERIFY:
			case PIN_OP_RESET:
				oResp = SendAPDU(oAPDU);
				break;
			case PIN_OP_CHANGE:
				oResp = SendAPDU(oAPDU);
				oResp = SendAPDU(oAPDUCHANGE);
				break;
			}
		}
	}

    unsigned long ulSW12 = getSW12(oResp);
    if (ulSW12 == 0x9000)
        bRet = true;
    else if (ulSW12 == 0x6983)
        ulRemaining = 0;
    else if (ulSW12 / 16 == 0x63C)
        ulRemaining = ulSW12 % 16;
	else
		throw CMWEXCEPTION(m_poContext->m_oPCSC.SW12ToErr(ulSW12));

	// Bad PIN: show a dialog to ask the user to try again
	// PIN blocked: show a dialog to tell the user
	if (bAskPIN && !bRet)
	{
		DlgPinUsage usage = PinUsage2Dlg(Pin, pKey);
		DlgRet dlgret = DlgBadPin(usage, utilStringWiden(Pin.csLabel).c_str(), ulRemaining);
		if (0 != ulRemaining && DLG_RETRY == dlgret)
			goto bad_pin;
	}

	// If PIN command OK and no SSO, then state that we have now
	// verified this PIN, this info is needed in the Sign() method
	if (bRet && !m_poContext->m_bSSO)
	{
		bool bFound = false;
		for (size_t i = 0; i < m_verifiedPINs.size() && !bFound; i++)
			bFound = (m_verifiedPINs[i] == Pin.ulID);
		if (!bFound)
			m_verifiedPINs.push_back(Pin.ulID);
	}

	return bRet;
}


bool CPkiCard::LogOff(const tPin & Pin)
{
	return false;
}

CByteArray CPkiCard::Sign(const tPrivKey & key, const tPin & Pin,
        unsigned long algo, const CByteArray & oData)
{
	// If SSO (Single Sign-On) is false and we didn't verify the
	// PIN yet, then we do this first without trying if it's
	// realy needed.

    	if (!m_poContext->m_bSSO)
	{
		bool bFound = false;
		for (size_t i = 0; i < m_verifiedPINs.size() && !bFound; i++)
			bFound = (m_verifiedPINs[i] == Pin.ulID);

		if (!bFound)
		{
			MWLOG(LEV_INFO, MOD_CAL, L"     No SSO: ask PIN and sign (key: ID=0x%0x, algo=0x%0x, "
				L"%d bytes input)", key.ulID, algo, oData.Size());
			return SignInternal(key, algo, oData, &Pin);
		}
	}

	// First try to sign.
    // If this returns a "Security conditions not satisfied"
    // then first do a Pin verify and then try again
	MWLOG(LEV_INFO, MOD_CAL, L"     Trying to Sign (key: ID=0x%0x, algo=0x%0x, "
		L"%d bytes input)", key.ulID, algo, oData.Size());
	//printf ("Trying to Sign (key: ID=0x%0x, algo=0x%0x, %d bytes input\n", key.ulID, algo, oData.Size());
    try
    {
        return SignInternal(key, algo, oData, &Pin);
    }
    catch(CMWException & e)
    {
      if ((unsigned)e.GetError() == EIDMW_ERR_NOT_AUTHENTICATED)
        {
			MWLOG(LEV_INFO, MOD_CAL, L"     Couldn't sign, asking PIN and trying again");
            return SignInternal(key, algo, oData);
        }
        else
            throw e;
    }
}

CByteArray CPkiCard::Sign(const tPrivKey & key, const tPin & Pin,
    unsigned long algo, CHash & oHash)
{
	CByteArray oHashResult = oHash.GetHash();

	return Sign(key, Pin, algo, oHashResult);
}

CByteArray CPkiCard::GetRandom(unsigned long ulLen)
{
    cout << "GetRandom Get Challenge" << endl;
	CAutoLock oAutoLock(this);

	bool bAppletSelectDone = false;

	if (m_selectAppletMode == ALW_SELECT_APPLET)
	{
		SelectApplet();
		bAppletSelectDone = true;
	}

	CByteArray oRandom(ulLen);

try_again:
    // Use a Get Challenge command to gather 8 bytes with each loop
    for (unsigned long i = 0; i < ulLen; i += 20)
    {
        unsigned char ucLen = (unsigned char) (ulLen - i > 20 ? 20 : ulLen - i);

        // Get challenge command
	m_ucCLA = 0x80;
        CByteArray oResp = SendAPDU(0x84, 0x00, 0x00, 0x08);
		if (ShouldSelectApplet(0x84, getSW12(oResp)))
		    /*{
			// First try to select 
			if (SelectApplet())
			{
				m_selectAppletMode = ALW_SELECT_APPLET;
				bAppletSelectDone = true;
				goto try_again;
			}
		}*/
		getSW12(oResp, 0x9000);

		oRandom.Append(oResp.GetBytes(), oResp.Size() - 2);
    }

    return oRandom;
}

tFileInfo CPkiCard::SelectFile(const std::string & csPath, bool bReturnFileInfo)
{
	CByteArray oResp;
    tFileInfo xFileInfo = {0};
    
	unsigned long ulPathLen = (unsigned long) csPath.size();
    if (ulPathLen % 4 != 0 || ulPathLen == 0)
        throw CMWEXCEPTION(EIDMW_ERR_BAD_PATH);
    ulPathLen /= 2;

    unsigned char ucP2 = bReturnFileInfo ? 0x00 : 0x0C;

    CAutoLock autolock(this);

	if (m_selectAppletMode == ALW_SELECT_APPLET)
	{
		SelectApplet();
        oResp = SelectByPath(csPath, bReturnFileInfo);
	}
	else
	{
		// First try to select the file by ID, assuming we're in the correct DF

		CByteArray oPath(ulPathLen);
		oPath.Append(Hex2Byte(csPath, ulPathLen - 2));
		oPath.Append(Hex2Byte(csPath, ulPathLen - 1));

		// Select File
		oResp = SendAPDU(0xA4, 0x02, ucP2, oPath);
		unsigned long ulSW12 = getSW12(oResp);
		if (ulSW12 == 0x6A82 || ulSW12 == 0x6A86)
		{
			if (ulPathLen == 2)
				throw CMWEXCEPTION(m_poContext->m_oPCSC.SW12ToErr(ulSW12));

			// The file wasn't found in this DF, so let's select by full path
			oResp = SelectByPath(csPath, bReturnFileInfo);
		}
		else
		{
			getSW12(oResp, 0x9000);
		}
	}

    if (bReturnFileInfo)
        xFileInfo = ParseFileInfo(oResp);

    return xFileInfo;
}

// Only called from SelectFile(), no locking is done here
CByteArray CPkiCard::SelectByPath(const std::string & csPath, bool bReturnFileInfo)
{
    unsigned char ucP2 = bReturnFileInfo ? 0x00 : 0x0C;

    unsigned long ulPathLen = (unsigned long) (csPath.size() / 2);

    CByteArray oPath(ulPathLen);
    for (unsigned long i = 0; i < ulPathLen; i++)
        oPath.Append(Hex2Byte(csPath, i));

    CByteArray oResp = SendAPDU(0xA4, 0x00, ucP2, oPath);
    if (ShouldSelectApplet(0xA4, getSW12(oResp)))
    {
        // The file still wasn't found, so let's first try to select the applet
        if (SelectApplet())
        {
            m_selectAppletMode = ALW_SELECT_APPLET;
            oResp = SendAPDU(0xA4, 0x80, ucP2, oPath);
        }
    }

	getSW12(oResp, 0x9000);

	return oResp;
}

CByteArray CPkiCard::ReadBinary(unsigned long ulOffset, unsigned long ulLen)
{

	// Read Binary
    return SendAPDU(0xB0, (unsigned char) (ulOffset / 256),
        (unsigned char) (ulOffset % 256), (unsigned char) (ulLen));
}

CByteArray CPkiCard::UpdateBinary(unsigned long ulOffset, const CByteArray & oData)
{
    // Update Binary
	CByteArray oDataVoid;

    if (ulOffset == 0)
    {
        oDataVoid.Append(clearbinary, sizeof(clearbinary));
        SendAPDU(0xD6, 0x00, 0x00, oDataVoid);
    }

    CByteArray ret = SendAPDU(0xD6, (unsigned char) (ulOffset / 256),
                              (unsigned char) (ulOffset % 256), oData);

    return ret;
}

DlgPinOperation CPkiCard::PinOperation2Dlg(tPinOperation operation)
{
	switch(operation)
	{
	case PIN_OP_CHANGE: return DLG_PIN_OP_CHANGE;
	default: return DLG_PIN_OP_VERIFY;
	}
}

CByteArray CPkiCard::MakePinCmd(tPinOperation operation, const tPin & Pin)
{
    //DEBUG
    //cout << "MakePin" << endl;
    CByteArray oCmd(5 + 32);

    oCmd.Append(m_ucCLA);

    switch (operation)
    {
    case PIN_OP_VERIFY:
        oCmd.Append(0x20);
        break;
    case PIN_OP_CHANGE:
        oCmd.Append(0x24);
        break;
    default:
        throw CMWEXCEPTION(EIDMW_ERR_PIN_OPERATION);
    }

    oCmd.Append(0x00); // P1

    oCmd.Append((unsigned char) Pin.ulPinRef); // P2
    return oCmd;
}

CByteArray CPkiCard::MakePinCmdIAS(tPinOperation operation, const tPin & Pin)
{
    //DEBUG
    //cout << "MakePin" << endl;
    CByteArray oCmd(5 + 32);

    oCmd.Append(m_ucCLA);

    switch (operation)
    {
    case PIN_OP_VERIFY:
        oCmd.Append(0x20);
        oCmd.Append(0x00); // P1
        break;
    case PIN_OP_CHANGE:
        oCmd.Append(0x24);
        oCmd.Append(0x01); // P1
        break;
    case PIN_OP_RESET:
    	oCmd.Append(0x2C);
    	oCmd.Append(0x02); // P1
    	break;
    default:
        throw CMWEXCEPTION(EIDMW_ERR_PIN_OPERATION);
    }

    oCmd.Append((unsigned char) Pin.ulPinRef); // P2
    return oCmd;
}

CByteArray CPkiCard::MakePinBuf(const tPin & Pin, const std::string & csPin,
	bool bEmptyPin)
{
    CByteArray oBuf(16);
    unsigned long i;

	unsigned long ulPinLen = bEmptyPin ? 0 : (unsigned long) csPin.size();

	if (!bEmptyPin)
	{
		// Test if it's a valid PIN value
		if (Pin.ulMinLen != 0 && ulPinLen < Pin.ulMinLen)
		{
			MWLOG(LEV_WARN, MOD_CAL, L"PIN length is %d, should be at least %d", ulPinLen, Pin.ulMinLen);
			throw CMWEXCEPTION(EIDMW_ERR_PIN_FORMAT);
		}
		if (Pin.ulMaxLen != 0 && ulPinLen > Pin.ulMaxLen)
		{
			MWLOG(LEV_WARN, MOD_CAL, L"PIN length is %d, should be at most %d", ulPinLen, Pin.ulMaxLen);
			throw CMWEXCEPTION(EIDMW_ERR_PIN_FORMAT);
		}
	}

	for (i = 0; i < ulPinLen; i++)
    {
        if (!IsDigit(csPin[i]))
        {
            MWLOG(LEV_WARN, MOD_CAL, L"The PIN contains non-digit values");
            throw CMWEXCEPTION(EIDMW_ERR_PIN_FORMAT);
        }
    }

    switch(Pin.encoding)
    {
    case PIN_ENC_ASCII:
        for (i = 0; i < ulPinLen; i++)
            oBuf.Append((unsigned char) csPin[i]);
        for ( ; i < Pin.ulStoredLen; i++)
            oBuf.Append(Pin.ucPadChar);
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
                uc += (unsigned char) (Pin.ucPadChar % 16);
            i++;
            oBuf.Append(uc);
        }
		while (oBuf.Size() < Pin.ulStoredLen)
            oBuf.Append((unsigned char) 0xFF);
                //Pin.ucPadChar > 0x0F ? Pin.ucPadChar : Pin.ucPadChar % 16);
        break;
    default:
        throw CMWEXCEPTION(EIDMW_ERR_PARAM_BAD);
    }
    return oBuf;
}

}
