/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2009 FedICT.
 * Copyright (C) 2019 Caixa Magica Software.
 * Copyright (C) 2011 Vasco Silva - <vasco.silva@caixamagica.pt>
 * Copyright (C) 2011-2012, 2016-2018 André Guerreiro - <aguerreiro1985@gmail.com>
 * Copyright (C) 2012 lmcm - <lmcm@caixamagica.pt>
 * Copyright (C) 2012 Rui Martinho - <rui.martinho@ama.pt>
 * Copyright (C) 2016-2017 Luiz Lemos - <luiz.lemos@caixamagica.pt>
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

#include "CardLayerConst.h"
#include "PkiCard.h"
#include "Log.h"
#include "Thread.h"
#include "pinpad2.h"
#include "FciData.h"

#include <algorithm>
#include <openssl/asn1.h>

namespace eIDMW {

CPkiCard::CPkiCard(SCARDHANDLE hCard, CContext *poContext, GenericPinpad *poPinpad)
	: CCard(hCard, poContext, poPinpad) {
	m_ucCLA = 0;
	m_selectAppletMode = DONT_SELECT_APPLET;
}

CPkiCard::~CPkiCard(void) {}

bool CPkiCard::SelectApplet() {
	// Don't do anything by default
	// Subclasses that implement this functionality should set the
	// m_selectAppletMode to TRY_SELECT_APPLET
	return false;
}

void CPkiCard::ResetApplication() { m_lastSelectedApplication.ClearContents(); }

void CPkiCard::SelectApplication(const CByteArray &oAID) {

	if (m_lastSelectedApplication.Size() > 0 && oAID.Size() > 0 &&
		memcmp(oAID.GetBytes(), m_lastSelectedApplication.GetBytes(), oAID.Size()) == 0) {
		return;
	}

	long lRetVal = 0;
	unsigned char tucSelectApp[] = {0x00, 0xA4, 0x04, 0x0C};
	CByteArray oCmd(sizeof(oAID) + 5);
	oCmd.Append(tucSelectApp, sizeof(tucSelectApp));
	oCmd.Append((unsigned char)oAID.Size());
	oCmd.Append(oAID.GetBytes(), oAID.Size());

	CByteArray oResp = SendAPDU(oCmd);

	if (getSW12(oResp) == 0x9000) {
		// If select application was a success, update the state
		m_lastSelectedApplication = oAID;
	} else {
		throw CMWEXCEPTION(EIDMW_ERR_FILE_NOT_FOUND);
	}
}

CByteArray CPkiCard::ReadUncachedFile(const std::string &csPath, unsigned long ulOffset, unsigned long ulMaxLen) {
	CAutoLock autolock(this);
	// We use max_block_read_length as 223 because of a limit on SM layer
	const int MAX_BLOCK_READ_LENGTH = m_pace.get() != NULL ? 223 : MAX_APDU_READ_LEN;

	MWLOG(LEV_INFO, MOD_CAL, L"   SelectUncachedFile %ls", utilStringWiden(csPath).c_str());

	tFileInfo fileInfo = SelectFile(csPath, true);
	unsigned long realMaxLen = (std::min)(fileInfo.lFileLen, ulMaxLen);
	unsigned long offsetByte = ulOffset;

	CByteArray fileArray(realMaxLen);

	MWLOG(LEV_DEBUG, MOD_CAL, "%s: file length parsed from FCI info: %lu realMaxLen: %lu", __FUNCTION__,
		  fileInfo.lFileLen, realMaxLen);

	// loop while you don't get to the end or maxLen
	while ((offsetByte != fileInfo.lFileLen) && (fileArray.Size() < realMaxLen)) {
		unsigned long maxLength = (std::min)(fileInfo.lFileLen - offsetByte, (unsigned long)MAX_BLOCK_READ_LENGTH);
		CByteArray response = ReadBinary(offsetByte, maxLength);
		offsetByte += maxLength;

		unsigned long ulSW12 = getSW12(response);

		// If the file is a multiple of the block read size, you will get
		// an SW12 = 6B00 (at least with PT eID) but that OK then..
		if (ulSW12 == 0x9000 || (offsetByte != 0 && ulSW12 == 0x6B00))
			fileArray.Append(response.GetBytes(), response.Size() - 2);
		else if (ulSW12 == 0x6982) {
			throw CNotAuthenticatedException(EIDMW_ERR_NOT_AUTHENTICATED, fileInfo.lReadPINRef);
		} else if (ulSW12 == 0x6B00)
			throw CMWEXCEPTION(EIDMW_ERR_PARAM_RANGE);
		else
			throw CMWEXCEPTION(m_poContext->m_oPCSC.SW12ToErr(ulSW12));
	}

	MWLOG(LEV_INFO, MOD_CAL, L"   Read file %ls (%d bytes) from card", utilStringWiden(csPath).c_str(),
		  fileArray.Size());

	return fileArray;
}

void CPkiCard::WriteUncachedFile(const std::string &csPath, unsigned long ulOffset, const CByteArray &oData) {
	CAutoLock autolock(this);

	CByteArray oDatan = oData;

	tFileInfo fileInfo = SelectFile(csPath, true);

	const unsigned char *pucData = oDatan.GetBytes();
	unsigned long ulDataLen = oDatan.Size();

	if (ulDataLen > PERSODATAFILESIZE)
		throw CMWEXCEPTION(EIDMW_ERR_PARAM_RANGE);

	bool bEOF = false;

	unsigned long ulLen = ulDataLen;
	unsigned long ulSendLen = 0;

	for (unsigned long ulTotalSentLen = 0; (ulTotalSentLen <= PERSODATAFILESIZE) && (!bEOF) && (ulLen > 0);
		 ulTotalSentLen += ulSendLen) {
		ulSendLen = (ulLen > MAX_APDU_WRITE_LEN) ? MAX_APDU_WRITE_LEN : ulLen;
		unsigned long pos = ulOffset + ulTotalSentLen;

		CByteArray oResp = UpdateBinary(pos, CByteArray(pucData + ulTotalSentLen, ulSendLen));
		unsigned long ulSW12 = getSW12(oResp);

		if ((ulSW12 == 0x9000) || ((ulTotalSentLen > 0) && (ulSW12 == 0x6B00))) {
			/*oDatan.Chop(2);*/
		} else if (ulSW12 == 0x6982)
			throw CNotAuthenticatedException(EIDMW_ERR_NOT_AUTHENTICATED, fileInfo.lReadPINRef);
		else if (ulSW12 == 0x6B00)
			throw CMWEXCEPTION(EIDMW_ERR_PARAM_RANGE);
		// EOF for Gemsafe cards
		else if (ulSW12 == 0x6282)
			bEOF = false; /* false */
		// Avoid problems with IAS cards (file not found)
		else if (ulSW12 == 0x6D80)
			bEOF = false;
		// Comment to Avoid problems with IAS cards
		// else
		// throw CMWEXCEPTION(m_poContext->m_oPCSC.SW12ToErr(ulSW12));

		ulLen -= ulSendLen;
	}

	MWLOG(LEV_INFO, MOD_CAL, L"Written file %ls to card", utilStringWiden(csPath).c_str());
}

unsigned char CPkiCard::PinUsage2Pinpad(const tPin &Pin, const tPrivKey *pKey) {
	// Its hackish but works for IAS and Gemsafe Cards
	return (unsigned char)Pin.ulID;
}

bool CPkiCard::PinCmd(tPinOperation operation, const tPin &Pin, const std::string &csPin1, const std::string &csPin2,
					  unsigned long &ulRemaining, const tPrivKey *pKey, bool bShowDlg, void *wndGeometry,
					  unsigned long unblockFlags) {

	// TODO (DEV-CC2): make this work for cc2.0
	if (this->GetType() == CARD_PTEID_IAS5)
		SelectApplication({PTEID_2_APPLET_EID, sizeof(PTEID_2_APPLET_EID)});

	bool bRet = false;
	std::string csReadPin1, csReadPin2;
	const std::string *pcsPin1 = &csPin1;
	const std::string *pcsPin2 = &csPin2;

	bool bPukMerge = unblockFlags & UNBLOCK_FLAG_PUK_MERGE;
	bool defineNewPin = unblockFlags & UNBLOCK_FLAG_NEW_PIN || operation == PIN_OP_CHANGE;
	bool bAskPIN = true;

	if ((operation == PIN_OP_VERIFY) && !csPin1.empty())
		bAskPIN = false;
	if (operation == PIN_OP_CHANGE && !csPin1.empty())
		bAskPIN = false;
	// Ask for PIN in RESET also in the PUK merge case
	if (operation == PIN_OP_RESET && !csPin1.empty() && !csPin2.empty() && !bPukMerge)
		bAskPIN = false;
	if (operation == PIN_OP_RESET_NO_PUK && !defineNewPin)
		bAskPIN = false;

	bool bUsePinpad = bAskPIN ? m_poPinpad != NULL : false;

bad_pin:
	MWLOG(LEV_DEBUG, MOD_CAL, L"DEBUG PinCmd: operation: %d, bUsePinpad:%d, bPukMerge: %d defineNewPin=%d",
		  (int)operation, bUsePinpad, bPukMerge, defineNewPin);

	// If no Pin(s) provided and it's no Pinpad reader -> ask Pins
	if (bAskPIN && !bUsePinpad) {
		showPinDialog(operation, Pin, csReadPin1, csReadPin2, pKey, wndGeometry);
		if (operation != PIN_OP_RESET_NO_PUK)
			pcsPin1 = &csReadPin1;
		pcsPin2 = &csReadPin2;
	}

	if (!bUsePinpad && bPukMerge) {
		// Explicitly merge the 2 halves of the PUK
		// In the pinpad case the operation is performed by the reader before
		// generating the final APDU
		csReadPin1 += csPin1;
	}

	CByteArray oPinBuf = MakePinBuf(Pin, *pcsPin1, pcsPin1->size() == 0, bPukMerge);
	// There is one case for PIN_OP_RESET where we don't have a new PIN
	if (defineNewPin)
		oPinBuf.Append(MakePinBuf(Pin, *pcsPin2, bUsePinpad, false));

	// add CLA, INS, P1, P2 (we only need a special P1 value if Unblocking PIN without defining new PIN)
	CByteArray oAPDU =
		MakePinCmd(operation, Pin, (operation == PIN_OP_RESET || operation == PIN_OP_RESET_NO_PUK) && !defineNewPin);
	// add Lc
	oAPDU.Append((unsigned char)oPinBuf.Size());
	oAPDU.Append(oPinBuf);

	CByteArray oResp;
	bool bSelected = false;

	// Don't remove these brackets!!
	{
		CAutoLock autolock(this);

		// Select the path where the Pin is, if necessary
		if (!Pin.csPath.empty() && !bSelected && Pin.csPath != "3F00") {
			SelectFile(Pin.csPath);
			bSelected = true;
		}

		// Send the command
		if (bUsePinpad)
			oResp = m_poPinpad->PinCmd(operation, Pin, PinUsage2Pinpad(Pin, pKey), oAPDU, ulRemaining, wndGeometry);
		else
			oResp = SendAPDU(oAPDU);
	}

	unsigned long ulSW12 = getSW12(oResp);
	if (ulSW12 == 0x9000) {
		bRet = true;
		ulRemaining = 3;
	}
	// 2 different return codes for PIN blocked
	else if (ulSW12 == 0x6983)
		ulRemaining = 0;
	else if (ulSW12 == 0x6984)
		ulRemaining = 0;
	// Special case for the GemPC Pinpad Reader
	else if (operation == PIN_OP_VERIFY && ulSW12 == 0x6402)
		ulRemaining = 0;
	else if (ulSW12 / 16 == 0x63C)
		ulRemaining = ulSW12 % 16;
	else
		throw CMWEXCEPTION(m_poContext->m_oPCSC.SW12ToErr(ulSW12));

	// Wrong PIN with no user interaction: return false and don't ask for retries
	// For PIN unlock we don't ask for retries
	if (!bRet && !bShowDlg || operation == PIN_OP_RESET || operation == PIN_OP_RESET_NO_PUK) {
		return bRet;
	}
	// Bad PIN: show a dialog to ask the user to try again
	// PIN blocked: show a dialog to tell the user
	else if (bAskPIN && !bRet) {
		DlgPinUsage usage = PinUsage2Dlg(Pin, pKey);
		DlgRet dlgret = DlgBadPin(usage, utilStringWiden(Pin.csLabel).c_str(), ulRemaining, wndGeometry);

		MWLOG(LEV_DEBUG, MOD_CAL, L"DlgBadPin returned %d", dlgret);

		if (dlgret == DLG_CANCEL)
			throw CMWEXCEPTION(EIDMW_ERR_PIN_CANCEL);

		if (0 != ulRemaining && DLG_RETRY == dlgret)
			goto bad_pin;
	}

	// If PIN command OK and no SSO, then state that we have now
	// verified this PIN, this info is needed in the Sign() method
	if (bRet) {
		m_verifiedPINs[Pin.ulID] = *pcsPin1; // Caching PIN
	}

	return bRet;
}

bool CPkiCard::PinCmdIAS(tPinOperation operation, const tPin &Pin, const std::string &csPin1, const std::string &csPin2,
						 unsigned long &ulRemaining, const tPrivKey *pKey, bool bShowDlg, void *wndGeometry) {

	bool bRet = false;
	std::string csReadPin1, csReadPin2;
	const std::string *pcsPin1 = &csPin1;
	const std::string *pcsPin2 = &csPin2;
	// martinho: usually each party have half of the puk (current puk size = 8) [puk ulMinLen = 8, ulMaxLen = 12]
	// martinho: condition to puk merge: csPin1 size < current puk size. the condition Pin.ulPinRef & 0x10 is a way to
	// identify puks.
	bool bPukMerge = (Pin.ulPinRef & 0x10) && !csPin1.empty() && csPin1.length() < Pin.ulMinLen;
	bool bAskPIN = csPin1.empty() || bPukMerge;
	bool bUsePinpad = bAskPIN ? m_poPinpad != NULL : false;

bad_pin:
	// If no Pin(s) provided and it's no Pinpad reader -> ask Pins
	if (bAskPIN && !bUsePinpad) {
		showPinDialog(operation, Pin, csReadPin1, csReadPin2, pKey, wndGeometry);
		pcsPin1 = &csReadPin1;
		pcsPin2 = &csReadPin2;
	}

	CByteArray oPinBuf = MakePinBuf(Pin, *pcsPin1, bUsePinpad, bPukMerge);
	CByteArray oAPDU;
	CByteArray oAPDUCHANGE;
	CByteArray oResp;

	if (Pin.ulPinRef == 130 || Pin.ulPinRef == 131) { // signature and address pins need the next apdu first
		// 00 A4 04 00 07 60 46 32 FF 00 01 02
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
	}

	switch (operation) {
	case PIN_OP_VERIFY:
	case PIN_OP_RESET:
		oAPDU = MakePinCmdIAS(operation, Pin);		 // add CLA, INS, P1, P2
		oAPDU.Append((unsigned char)oPinBuf.Size()); // add P3
		oAPDU.Append(oPinBuf);
		break;
	case PIN_OP_CHANGE:
		oAPDU = MakePinCmdIAS(PIN_OP_VERIFY, Pin);	 // add CLA, INS, P1, P2
		oAPDU.Append((unsigned char)oPinBuf.Size()); // add P3
		oAPDU.Append(oPinBuf);
		oAPDUCHANGE = MakePinCmdIAS(operation, Pin);	   // add CLA, INS, P1, P2
		oAPDUCHANGE.Append((unsigned char)oPinBuf.Size()); // add P3
		oAPDUCHANGE.Append(MakePinBuf(Pin, *pcsPin2, bUsePinpad, bPukMerge));
		break;
	}

	bool bSelected = false;

	// Don't remove these brackets!!
	{
		CAutoLock autolock(this);

		// Select the path where the Pin is, if necessary
		if (!Pin.csPath.empty() && !bSelected && Pin.csPath != "3F00") {
			SelectFile(Pin.csPath);
			bSelected = true;
		}

		// Send the command
		if (bAskPIN && bUsePinpad) {

			if (operation == PIN_OP_CHANGE) {
				oResp =
					m_poPinpad->PinCmd(PIN_OP_VERIFY, Pin, PinUsage2Pinpad(Pin, pKey), oAPDU, ulRemaining, wndGeometry);

				unsigned long ulSW12 = getSW12(oResp);
				if (ulSW12 == 0x9000)
					oResp = m_poPinpad->PinCmd(operation, Pin, PinUsage2Pinpad(Pin, pKey), oAPDUCHANGE, ulRemaining,
											   wndGeometry);
			} else
				oResp = m_poPinpad->PinCmd(operation, Pin, PinUsage2Pinpad(Pin, pKey), oAPDU, ulRemaining, wndGeometry);

		} else {
			switch (operation) {
			case PIN_OP_VERIFY:
			case PIN_OP_RESET:
				oResp = SendAPDU(oAPDU);
				break;
			case PIN_OP_CHANGE:
				oResp = SendAPDU(oAPDU);

				unsigned long ulSW12 = getSW12(oResp);
				if (ulSW12 == 0x9000)
					oResp = SendAPDU(oAPDUCHANGE);
				break;
			}
		}
	}

	unsigned long ulSW12 = getSW12(oResp);
	if (ulSW12 == 0x9000) {
		bRet = true;
		ulRemaining = 3;
	} else if (ulSW12 == 0x6983)
		ulRemaining = 0;
	// Special case for the GemPC Pinpad Reader
	else if (operation == PIN_OP_VERIFY && ulSW12 == 0x6402)
		ulRemaining = 0;
	else if (ulSW12 / 16 == 0x63C)
		ulRemaining = ulSW12 % 16;
	else
		throw CMWEXCEPTION(m_poContext->m_oPCSC.SW12ToErr(ulSW12));

	// Wrong PIN with no user interaction: return false and don't ask for retries
	if (!bRet && !bShowDlg) {
		return bRet;
	}
	// Bad PIN: show a dialog to ask the user to try again
	// PIN blocked: show a dialog to tell the user
	else if (bAskPIN && !bRet) {
		DlgPinUsage usage = PinUsage2Dlg(Pin, pKey);
		DlgRet dlgret = DlgBadPin(usage, utilStringWiden(Pin.csLabel).c_str(), ulRemaining, wndGeometry);
		if (0 != ulRemaining && DLG_RETRY == dlgret)
			goto bad_pin;
	}

	// If PIN command OK and no SSO, then state that we have now
	// verified this PIN, this info is needed in the Sign() method
	if (bRet) {
		/*bool bFound = false;
		for (size_t i = 0; i < m_verifiedPINs.size() && !bFound; i++)
			bFound = (m_verifiedPINs[i] == Pin.ulID);
		if (!bFound)
			m_verifiedPINs.push_back(Pin.ulID); */
		m_verifiedPINs[Pin.ulID] = *pcsPin1; // Caching PIN
	}

	return bRet;
}

CByteArray CPkiCard::Sign(const tPrivKey &key, const tPin &Pin, unsigned long paddingType, const CByteArray &oData) {

	MWLOG(LEV_INFO, MOD_CAL,
		  L"PKiCard::Sign(): ask PIN and sign (key: ID=0x%0x, algo=0x%0x, "
		  L"%d bytes input)",
		  key.ulID, paddingType, oData.Size());
	return SignInternal(key, paddingType, oData, &Pin);
}

CByteArray CPkiCard::GetRandom(unsigned long ulLen) {
	CAutoLock oAutoLock(this);

	bool bAppletSelectDone = false;

	if (m_selectAppletMode == ALW_SELECT_APPLET) {
		SelectApplet();
		bAppletSelectDone = true;
	}

	CByteArray oRandom(ulLen);

	// Use a Get Challenge command to gather 8 bytes with each loop
	for (unsigned long i = 0; i < ulLen; i += 20) {
		unsigned char ucLen = (unsigned char)(ulLen - i > 20 ? 20 : ulLen - i);

		// Get challenge command
		m_ucCLA = 0x80;
		CByteArray oResp = SendAPDU(0x84, 0x00, 0x00, 0x08);
		getSW12(oResp, 0x9000);

		oRandom.Append(oResp.GetBytes(), oResp.Size() - 2);
	}

	return oRandom;
}


tFileInfo CPkiCard::SelectFile(const std::string &csPath, const unsigned char *oAID, bool bReturnFileInfo) {
	auto ulPathLen = static_cast<unsigned long>(csPath.size());
	tFileInfo info;
	info.lFileLen = info.lReadPINRef = info.lWritePINRef = 0;
	// path must not contain any incomplete directory or file id
	if (ulPathLen % 4 != 0 || ulPathLen == 0)
		throw CMWEXCEPTION(EIDMW_ERR_BAD_PATH);

	// each byte is 2 characters
	ulPathLen /= 2;
	CByteArray responseSelection;
	CAutoLock autolock(this);
	{
		// Try to select from current application
		responseSelection = SelectByPath(csPath, bReturnFileInfo);

		auto ulSW12 = getSW12(responseSelection);
		if ((ulSW12 >> 0x8) & 0x6A) // Select File any error
		{
			// If failed, try to select the respective application
			SelectApplication({oAID, sizeof(oAID)});

			// Select by path again
			responseSelection = SelectByPath(csPath, bReturnFileInfo);

			// Should be expecting 0x9000 (success)
			getSW12(responseSelection, 0x9000);
		}
		// TODO: return lReadPINRef and lWritePINRef, from security attributes tag 0x8C
		if (bReturnFileInfo) {
			CByteArray fci_data = responseSelection.GetBytes(0, responseSelection.GetByte(1));
			char *fci_data_hex = bin2AsciiHex(responseSelection.GetBytes(), responseSelection.Size());
			MWLOG(LEV_DEBUG, MOD_CAL, "%s: FCI data: %s", __FUNCTION__, fci_data_hex);
			delete[] fci_data_hex;

			info.lFileLen = extractEFSize(fci_data);
		}
	}

	return info;
}

tFileInfo CPkiCard::SelectFile(const std::string &csPath, bool bReturnFileInfo) {
	CByteArray oResp;
	tFileInfo xFileInfo = {0};

	unsigned long ulPathLen = (unsigned long)csPath.size();
	if (ulPathLen % 4 != 0 || ulPathLen == 0)
		throw CMWEXCEPTION(EIDMW_ERR_BAD_PATH);
	ulPathLen /= 2;

	unsigned char ucP2 = bReturnFileInfo ? 0x00 : 0x0C;

	CAutoLock autolock(this);

	if (m_selectAppletMode == ALW_SELECT_APPLET) {
		SelectApplet();
		oResp = SelectByPath(csPath, bReturnFileInfo);
	} else {
		// First try to select the file by ID, assuming we're in the correct DF

		CByteArray oPath(ulPathLen);
		oPath.Append(Hex2Byte(csPath, ulPathLen - 2));
		oPath.Append(Hex2Byte(csPath, ulPathLen - 1));

		// Select File
		oResp = SendAPDU(0xA4, 0x02, ucP2, oPath);
		unsigned long ulSW12 = getSW12(oResp);
		if (ulSW12 == 0x6A82 || ulSW12 == 0x6A86) {
			if (ulPathLen == 2)
				throw CMWEXCEPTION(m_poContext->m_oPCSC.SW12ToErr(ulSW12));

			// The file wasn't found in this DF, so let's select by full path
			oResp = SelectByPath(csPath, bReturnFileInfo);
		} else {
			getSW12(oResp, 0x9000);
		}
	}

	return xFileInfo;
}

CByteArray CPkiCard::SelectByPath(const std::string &csPath, bool bReturnFileInfo) {
	//
	// Old version of CC
	// Only accepts path length of 2-4 bytes (1 file from root directory OR 1 DF and 1 file from DF)
	// Example 1: apdu 00 A4 08 04 02 2f 00 		- selects file 2f00 from root directory 3f00
	// Example 2: apdu 00 A4 08 04 04 5f 00 ef 12 	- selects file ef12 from DF 5f00 from root directory 3f00
	// Note: DF MUST NOT BE ROOT DIRECTORY (3f 00). Operations like (apdu 00 A4 08 04 04 3f 00 2f 00) will return 0x6A
	// Operation not supported
	//
	std::string csPathCopy = csPath;
	bool select_by_path_from_mf = false;
	MWLOG(LEV_DEBUG, MOD_CAL, "%s: csPath: %s", __FUNCTION__, csPath.c_str());
	if (csPath.find("3F00") != std::string::npos || csPath.find("3f00") != std::string::npos) {
		csPathCopy.erase(0, 4);
		select_by_path_from_mf = true;
	}

	unsigned long ulPathLen = (unsigned long)csPathCopy.size() / 2;
	CByteArray oPath(ulPathLen);
	for (unsigned long ulOffset = 0; ulOffset < ulPathLen; ulOffset += 2) {
		oPath.Append(Hex2Byte(csPathCopy, ulOffset));
		oPath.Append(Hex2Byte(csPathCopy, ulOffset + 1));
	}

	//
	// Send APDU and validate response
	//
	CByteArray selectByPathAPDU(6);
	selectByPathAPDU.Append(m_ucCLA);
	selectByPathAPDU.Append(0xA4);
	selectByPathAPDU.Append(oPath.Size() > 2 || select_by_path_from_mf ? 0x08 : 0x02);
	selectByPathAPDU.Append(bReturnFileInfo ? 0x00 : 0x0C);
	selectByPathAPDU.Append(oPath.Size());
	selectByPathAPDU.Append(oPath);
	selectByPathAPDU.Append(0x00);

	auto oResp = SendAPDU(selectByPathAPDU);
	getSW12(oResp, 0x9000);

	return oResp;
}

CByteArray CPkiCard::ReadBinary(unsigned long ulOffset, unsigned long ulLen) {

	// Read Binary
	return SendAPDU(0xB0, (unsigned char)(ulOffset / 256), (unsigned char)(ulOffset % 256), (unsigned char)(ulLen));
}

CByteArray CPkiCard::UpdateBinary(unsigned long ulOffset, const CByteArray &oData) {

	return SendAPDU(0xD6, (unsigned char)(ulOffset / 256), (unsigned char)(ulOffset % 256), oData);
}

CByteArray CPkiCard::MakePinCmd(tPinOperation operation, const tPin &Pin, bool specialP1Value) {
	CByteArray oCmd(5 + 32);

	oCmd.Append(m_ucCLA);

	switch (operation) {
	case PIN_OP_VERIFY:
		oCmd.Append(0x20);
		break;
	case PIN_OP_CHANGE:
		oCmd.Append(0x24);
		break;
	case PIN_OP_RESET:
	case PIN_OP_RESET_NO_PUK:
		oCmd.Append(0x2C);
		break;
	default:
		throw CMWEXCEPTION(EIDMW_ERR_PIN_OPERATION);
	}

	// Reset retry counter only
	if (specialP1Value)
		oCmd.Append(0x01); // P1
	else
		oCmd.Append(0x00); // P1 (always 0 for Verify and Change, RESET counter and replace PIN)

	oCmd.Append((unsigned char)Pin.ulPinRef); // P2
	return oCmd;
}

CByteArray CPkiCard::MakePinCmdIAS(tPinOperation operation, const tPin &Pin, void *wndGeometry) {
	CByteArray oCmd(5 + 32);

	oCmd.Append(m_ucCLA);

	switch (operation) {
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

	oCmd.Append((unsigned char)Pin.ulPinRef); // P2
	return oCmd;
}

CByteArray CPkiCard::MakePinBuf(const tPin &Pin, const std::string &csPin, bool bEmptyPin, bool bPukMerge) {
	CByteArray oBuf(16);
	unsigned long i;

	unsigned long ulPinLen = bEmptyPin && !bPukMerge ? 0 : (unsigned long)csPin.size();

	if (!bEmptyPin) {
		// Test if it's a valid PIN value
		if (Pin.ulMinLen != 0 && ulPinLen < Pin.ulMinLen) {
			MWLOG(LEV_WARN, MOD_CAL, L"PIN length is %d, should be at least %d", ulPinLen, Pin.ulMinLen);
			throw CMWEXCEPTION(EIDMW_ERR_PIN_FORMAT);
		}
		if (Pin.ulMaxLen != 0 && ulPinLen > Pin.ulMaxLen) {
			MWLOG(LEV_WARN, MOD_CAL, L"PIN length is %d, should be at most %d", ulPinLen, Pin.ulMaxLen);
			throw CMWEXCEPTION(EIDMW_ERR_PIN_FORMAT);
		}
	}
	for (i = 0; i < ulPinLen; i++) {
		if (!IsDigit(csPin[i])) {
			MWLOG(LEV_WARN, MOD_CAL, L"The PIN contains non-digit values");
			throw CMWEXCEPTION(EIDMW_ERR_PIN_FORMAT);
		}
	}

	switch (Pin.encoding) {
	case PIN_ENC_ASCII:
		if (bPukMerge) {
			for (i = 0; i < Pin.ulStoredLen - ulPinLen; i++)
				oBuf.Append(Pin.ucPadChar);
			for (; i < Pin.ulStoredLen; i++) {
				oBuf.Append((unsigned char)csPin[i - (Pin.ulStoredLen - ulPinLen)]);
			}
		} else {
			for (i = 0; i < ulPinLen; i++)
				oBuf.Append((unsigned char)csPin[i]);
			for (; i < Pin.ulStoredLen; i++)
				oBuf.Append(Pin.ucPadChar);
		}
		break;
	case PIN_ENC_GP:
		oBuf.Append((unsigned char)(0x20 + ulPinLen));
		// no break here
	case PIN_ENC_BCD:
		i = 0;
		while (i < ulPinLen) {
			unsigned char uc = (unsigned char)(16 * (csPin[i] - '0'));
			i++;
			if (i < ulPinLen)
				uc += (unsigned char)(csPin[i] - '0');
			else
				uc += (unsigned char)(Pin.ucPadChar % 16);
			i++;
			oBuf.Append(uc);
		}
		while (oBuf.Size() < Pin.ulStoredLen)
			oBuf.Append((unsigned char)0xFF);
		// Pin.ucPadChar > 0x0F ? Pin.ucPadChar : Pin.ucPadChar % 16);
		break;
	default:
		throw CMWEXCEPTION(EIDMW_ERR_PARAM_BAD);
	}

	return oBuf;
}

} // namespace eIDMW
