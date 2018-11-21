
/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2012 FedICT, Caixa Magica Software
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
#include "GenericPinpad.h"
#include "Context.h"
#include "CardLayer.h"
#include "pinpad2.h"
#include "../dialogs/dialogs.h"
#include "../common/Log.h"
#include "../common/Config.h"
#include "../common/Util.h"

namespace eIDMW
{


GenericPinpad::GenericPinpad(CContext *poContext, SCARDHANDLE hCard,
			const std::string & csReader): m_poContext(poContext), m_csReader(csReader), m_hCard(hCard)
{
	//USB LANGID for English (US): it should work for all readers
	//TODO: we should query the reader for supported languages and then select PT_pt or PT_br
	m_ulLangCode = 0x0409;
	GetFeatureList();
};
// See par 4.1.11.3 bmFormatString description

unsigned char GenericPinpad::ToFormatString(const tPin & pin)
{
	switch(pin.encoding)
	{
	case PIN_ENC_ASCII:
		return 0x00 | 0x00 | 0x00 | 0x02;
	case PIN_ENC_BCD:
		return 0x00 | 0x00 | 0x00 | 0x01;
	case PIN_ENC_GP:
		// READER FIX:
		// The SPR532 reader wants this value to be as for BCD
		const char *csReader = m_csReader.c_str();
		if ( (m_usReaderFirmVers != 0x0000) && (m_usReaderFirmVers < 0x0506) &&
			(strstr(csReader, "SPRx32 USB") != NULL) )
		{
			return 0x00 | 0x00 | 0x00 | 0x01;
		}
		return 0x80 | 0x08 | 0x00 | 0x01;
	}
	return 0;
}

bool GenericPinpad::IsGemsafe(CByteArray &atr)
{

	char * GemsafeATRs[] =
	{
	    "\x3B\x7D\x95\x00\x00\x80\x31\x80\x65\xb0\x83\x11\x00\xc8\x83\x00\x90\x00",
	    "\x3B\x7D\x95\x00\x00\x80\x31\x80\x65\xb0\x83\x11\x00\xa9\x83\x00\x90\x00",
	    "\x3B\x7D\x95\x00\x00\x80\x31\x80\x65\xb0\x83\x11\x00\xc8\x83\x00",
	    "\x3B\x7D\x95\x00\x00\x80\x31\x80\x65\xB0\x83\x11\xC0\xA9\x83\x00\x90\x00",
	    "\x3B\xFF\x96\x00\x00\x81\x31\x80\x43\x80\x31\x80\x65\xB0\x85\x03\x00\xEF\x12\x0F\xFF\x82\x90\x00\x67",
		"\x3B\xFF\x96\x00\x00\x81\x31\xFE\x43\x80\x31\x80\x65\xB0\x85\x04\x01\x20\x12\x0F\xFF\x82\x90\x00\xD0",
	    "\x3B\x7D\x95\x00\x00\x80\x31\x80\x65\xb0\x83\x11\x00\xa9\x83\x00"

	};
	int i = 0;
	int number_of_entries = sizeof(GemsafeATRs) / sizeof(char *);
	while (i != number_of_entries)
	{
	if (memcmp(GemsafeATRs[i], atr.GetBytes(), atr.Size()) == 0)
		return true;
	   i++;
	}

	return false;
}



// See par 4.1.11.4 bmPINBlockString description
unsigned char GenericPinpad::ToPinBlockString(const tPin & pin)
{
	switch(pin.encoding)
	{
	case PIN_ENC_ASCII:
		return (unsigned char ) pin.ulStoredLen;
	case PIN_ENC_BCD:
		return (unsigned char ) pin.ulStoredLen;
	case PIN_ENC_GP:
		unsigned char ulStoredLen = (unsigned char)(pin.ulStoredLen - 1);
		ulStoredLen |= 0x40;
		return ulStoredLen;
	}
	return (unsigned char ) pin.ulStoredLen;
}

// See par 4.1.11.5 bmPINLengthFormat
unsigned char GenericPinpad::ToPinLengthFormat(const tPin & pin)
{
	switch(pin.encoding)
	{
	case PIN_ENC_ASCII:
		return 0x00 | 0x00;
	case PIN_ENC_BCD:
		return 0x00 | 0x00;
	case PIN_ENC_GP:
		return 0x00 | 0x04;
	}
	return 0x00;
}

unsigned char GenericPinpad::GetMaxPinLen(const tPin & pin)
{
	unsigned char ucRes = (unsigned char)
		(pin.ulMaxLen > 0 ? pin.ulMaxLen : pin.ulStoredLen);

	// READER FIX:
	// Limitation of the GemPC reader: at most 8 PIN digits
	const char *csReader = m_csReader.c_str();
	if (strstr(csReader, "Gemplus GemPC Pinpad") == csReader)
		ucRes = (ucRes > 8) ? 8 : ucRes;

	return ucRes;
}

CByteArray GenericPinpad::PinCmd(tPinOperation operation,
	const tPin & pin, unsigned char ucPinType,
    const CByteArray & oAPDU, unsigned long & ulRemaining, void *wndGeometry)
{

	CByteArray oResp;
	if (operation == PIN_OP_VERIFY)
		oResp = PinCmd1(operation, pin, ucPinType, oAPDU, ulRemaining, wndGeometry );
	else
		oResp = PinCmd2(operation, pin, ucPinType, oAPDU, ulRemaining, wndGeometry );

	if (oResp.Size() != 2)
	{
	  MWLOG(LEV_ERROR, MOD_CAL, L"pinpad reader returned %ls\n", oResp.ToWString().c_str());
	  return EIDMW_ERR_UNKNOWN; // should never happen
	}

	/* moved to PkiCard
	const unsigned char *pucSW12 = oResp.GetBytes();
	if (pucSW12[0] == 0x64 && pucSW12[1] == 0x00)
		throw CMWEXCEPTION(EIDMW_ERR_TIMEOUT);
	if (pucSW12[0] == 0x64 && pucSW12[1] == 0x01)
		throw CMWEXCEPTION(EIDMW_ERR_PIN_CANCEL);
	if (pucSW12[0] == 0x64 && pucSW12[1] == 0x02)
		throw CMWEXCEPTION(EIDMW_NEW_PINS_DIFFER);
	if (pucSW12[0] == 0x64 && pucSW12[1] == 0x03)
		throw CMWEXCEPTION(EIDMW_WRONG_PIN_FORMAT);
	if (pucSW12[0] == 0x6B && pucSW12[1] == 0x80){
		throw CMWEXCEPTION(EIDMW_PINPAD_ERR);

	}
	*/
	return oResp;
}

/** For operations involving 1 PIN */
CByteArray GenericPinpad::PinCmd1(tPinOperation operation,
	const tPin & pin, unsigned char ucPinType,
    const CByteArray & oAPDU, unsigned long & ulRemaining,
    void *wndGeometry )
{
	EIDMW_PP_VERIFY_CCID xVerifyCmd;
	unsigned long ulVerifyCmdLen;

	memset(&xVerifyCmd, 0, sizeof(xVerifyCmd));
	xVerifyCmd.bTimerOut = 30;
	xVerifyCmd.bTimerOut2 = 30;
	xVerifyCmd.bmFormatString = ToFormatString(pin);
	xVerifyCmd.bmPINBlockString = ToPinBlockString(pin);
	xVerifyCmd.bmPINLengthFormat = ToPinLengthFormat(pin);
	xVerifyCmd.wPINMaxExtraDigit[0] = GetMaxPinLen(pin);
	xVerifyCmd.wPINMaxExtraDigit[1] = (unsigned char) pin.ulMinLen;
	xVerifyCmd.bEntryValidationCondition = 0x02;
	xVerifyCmd.bNumberMessage = 0x01;
	ToUchar2(m_ulLangCode, xVerifyCmd.wLangId),
	xVerifyCmd.bMsgIndex = 0;
	ToUchar4(oAPDU.Size(), xVerifyCmd.ulDataLength);
	memcpy(xVerifyCmd.abData, oAPDU.GetBytes(), oAPDU.Size());
	ulVerifyCmdLen = sizeof(xVerifyCmd) - PP_APDU_MAX_LEN + oAPDU.Size();

	CByteArray oCmd((unsigned char *) &xVerifyCmd, ulVerifyCmdLen);
	if (m_ioctlVerifyDirect)
	{
		return PinpadControl(m_ioctlVerifyDirect, oCmd, operation,
			ucPinType, pin.csLabel, wndGeometry );
	}
	else
	{
		PinpadControl(m_ioctlVerifyStart, oCmd, operation,
			ucPinType, pin.csLabel);
		return PinpadControl(m_ioctlVerifyFinish, CByteArray(), operation,
			ucPinType, "", wndGeometry );
	}
}

/** For operations involving 2 PINs */
CByteArray GenericPinpad::PinCmd2(tPinOperation operation,
	const tPin & pin, unsigned char ucPinType,
    const CByteArray & oAPDU, unsigned long & ulRemaining,
    void *wndGeometry )
{
	EIDMW_PP_CHANGE_CCID xChangeCmd;
	unsigned long ulChangeCmdLen;
	//Gemsafe and IAS need different parameters for VERIFY control interaction
	bool includes_verify = oAPDU.Size() == 21;

	memset(&xChangeCmd, 0, sizeof(xChangeCmd));
	xChangeCmd.bTimerOut = 30;
	xChangeCmd.bTimerOut2 = 30;
	xChangeCmd.bmFormatString = ToFormatString(pin);
	xChangeCmd.bmPINBlockString = ToPinBlockString(pin);
	xChangeCmd.bmPINLengthFormat = ToPinLengthFormat(pin);
	xChangeCmd.bInsertionOffsetOld = 0x00;
	xChangeCmd.bInsertionOffsetNew = includes_verify? 0x08 : 0x00;
	xChangeCmd.wPINMaxExtraDigit[0] = GetMaxPinLen(pin);
	xChangeCmd.wPINMaxExtraDigit[1] = (unsigned char) pin.ulMinLen;
	xChangeCmd.bConfirmPIN = includes_verify? 0x03 : 0x01;
	xChangeCmd.bEntryValidationCondition = 0x02;
	xChangeCmd.bNumberMessage = includes_verify? 0x03 : 0x02;
	ToUchar2(m_ulLangCode, xChangeCmd.wLangId);
	xChangeCmd.bMsgIndex1 = 0x00;
	xChangeCmd.bMsgIndex2 = 0x01;
	xChangeCmd.bMsgIndex3 = 0x02;
	ToUchar4(oAPDU.Size(), xChangeCmd.ulDataLength);
	memcpy(xChangeCmd.abData, oAPDU.GetBytes(), oAPDU.Size());
	ulChangeCmdLen = sizeof(xChangeCmd) - PP_APDU_MAX_LEN + oAPDU.Size();

	CByteArray oCmd((unsigned char *) &xChangeCmd, ulChangeCmdLen);
	if (m_ioctlChangeDirect)
	{
		return PinpadControl(m_ioctlChangeDirect, oCmd, operation,
                            ucPinType, pin.csLabel, wndGeometry);
	}
	else
	{
		PinpadControl(m_ioctlChangeStart, oCmd, operation,
			ucPinType, pin.csLabel);
		return PinpadControl(m_ioctlChangeFinish, CByteArray(), operation,
			ucPinType, "", wndGeometry);
	}
}

bool GenericPinpad::ShowDlg(tPinOperation operation, unsigned char ucPintype,
	const std::string & csPinLabel, const std::string & csReader,
	unsigned long *pulDlgHandle, void *wndGeometry)
{

	const char *csMesg = "";
	DlgPinUsage dlgUsage = DLG_PIN_ACTIVATE;
	switch(ucPintype)
	{
		case EIDMW_PP_TYPE_AUTH: dlgUsage = DLG_PIN_AUTH; break;
		case EIDMW_PP_TYPE_SIGN: dlgUsage = DLG_PIN_SIGN; break;
		case EIDMW_PP_TYPE_ADDR: dlgUsage = DLG_PIN_ADDRESS; break;
	}

	DlgPinOperation dlgOperation = PinOperation2Dlg(operation);
	std::wstring wideReader = utilStringWiden(csReader);
	std::wstring widePinLabel = utilStringWiden(csPinLabel);
	std::wstring wideMesg = utilStringWiden(csMesg);
	return EIDMW_OK == DlgDisplayPinpadInfo(dlgOperation,
			wideReader.c_str(), dlgUsage,
			widePinLabel.c_str(), wideMesg.c_str(), pulDlgHandle, wndGeometry );
}

void GenericPinpad::CloseDlg(unsigned long ulDlgHandle)
{
	DlgClosePinpadInfo(ulDlgHandle);
}
CByteArray GenericPinpad::PinpadControl(unsigned long ulControl, const CByteArray & oCmd,
	tPinOperation operation, unsigned char ucPintype,
	const std::string & csPinLabel,	void *wndGeometry )
{
	bool showDlg = ulControl != CCID_IOCTL_GET_FEATURE_REQUEST;
	unsigned long ulDlgHandle;
	
	if (showDlg)
		ShowDlg(operation, ucPintype, csPinLabel, m_csReader, &ulDlgHandle, wndGeometry);

	CByteArray oResp;
	try
	{
			oResp = m_poContext->m_oPCSC.Control(m_hCard, ulControl, oCmd);

			// give some time for the dialog process to fork() (it was killing too fast :-) )
			unsigned long ulSW12 = 256 * oResp.GetByte(oResp.Size() - 2) + oResp.GetByte(oResp.Size() - 1);
			if (ulSW12 == 0x6B80)
				CThread::SleepMillisecs(500);

	}
	catch (...)
	{
		if(showDlg)
			CloseDlg(ulDlgHandle);
		throw;
	}
	if (showDlg)
		CloseDlg(ulDlgHandle);

	return oResp;
}

#define CHECK_FEATURE(feature, featureID, iotcl) \
	if (feature[0] == featureID) \
		iotcl = 256 * (256 * ((256 * feature[2]) + feature[3]) + feature[4]) + feature[5];

void GenericPinpad::GetFeatureList()
{
	m_bCanVerifyUnlock = false;
	m_bCanChangeUnlock = false;
	m_ioctlVerifyStart = m_ioctlVerifyFinish = m_ioctlVerifyDirect = 0;
	m_ioctlChangeStart = m_ioctlChangeFinish = m_ioctlChangeDirect = 0;

	try {
		CByteArray oFeatures = PinpadControl(CCID_IOCTL_GET_FEATURE_REQUEST,
			CByteArray(), PIN_OP_VERIFY, 0, "");

		// Example of a feature list: 06 04 00 31 20 30 07 04 00 31 20 34
		// Which means:
		//  - IOCTL for CCID_VERIFY_DIRECT = 0x00312030
		//  - IOCTL for CCID_CHANGE_DIRECT = 0x00312034
		unsigned long ulFeatureLen = oFeatures.Size();
		if ((ulFeatureLen % 6) == 0)
		{
			const unsigned char *pucFeatures = oFeatures.GetBytes();
			ulFeatureLen /= 6;
			for (unsigned long i = 0; i < ulFeatureLen; i++)
			{
				CHECK_FEATURE(pucFeatures, CCID_VERIFY_START, m_ioctlVerifyStart)
				CHECK_FEATURE(pucFeatures, CCID_VERIFY_FINISH, m_ioctlVerifyFinish)
				CHECK_FEATURE(pucFeatures, CCID_VERIFY_DIRECT, m_ioctlVerifyDirect)
				CHECK_FEATURE(pucFeatures, CCID_CHANGE_START, m_ioctlChangeStart)
				CHECK_FEATURE(pucFeatures, CCID_CHANGE_FINISH, m_ioctlChangeFinish)
				CHECK_FEATURE(pucFeatures, CCID_CHANGE_DIRECT, m_ioctlChangeDirect)
				pucFeatures += 6;
			}

			m_bCanVerifyUnlock = (m_ioctlVerifyStart && m_ioctlVerifyFinish) || m_ioctlVerifyDirect;
			m_bCanChangeUnlock = (m_ioctlChangeStart && m_ioctlChangeFinish) || m_ioctlChangeDirect;

		}
	}
	catch(const CMWException & e)
	{
		// very likely CCID_IOCTL_GET_FEATURE_REQUEST isn't supported
		// by this reader -> nothing to do
		e.GetError();
	}

}

}
