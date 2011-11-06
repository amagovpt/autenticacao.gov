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
#include "Pinpad.h"
#include "Context.h"
#include "pinpad2.h"
#include "../common/Log.h"
#include "../common/Util.h"
#include "../common/Config.h"

// Make sure PP_DUMP_CMDS is not defined in a release!
#define PP_DUMP_CMDS
#undef PP_DUMP_CMDS

namespace eIDMW
{

CPinpad::CPinpad(void)
{
	m_bNewCard = true;
	m_ulLangCode = 0;
	m_usReaderFirmVers = 0;
}

void CPinpad::Init(CContext *poContext, SCARDHANDLE hCard,
	const std::string & csReader, const std::string & csPinpadPrefix)
{
    m_poContext = poContext;
    m_hCard = hCard;
	m_csReader = csReader;

	// We only unload the pinpad lib that is currently
	// loaded if another type of card is inserted then
	// the card before the current one.
	if (csPinpadPrefix != m_csPinpadPrefix)
		UnloadPinpadLib();

	m_csPinpadPrefix = csPinpadPrefix;
}

void CPinpad::Init(CContext *poContext, SCARDHANDLE hCard,
	const std::string & csReader, const std::string & csPinpadPrefix,
	CByteArray usReaderFirmVers)
{
	this->Init(poContext, hCard,csReader,csPinpadPrefix);

	m_usReaderFirmVers = usReaderFirmVers.GetByte(3)*256 + usReaderFirmVers.GetByte(2);
}

bool CPinpad::UsePinpad(tPinOperation operation)
{
	if (m_bNewCard)
	{
		m_bUsePinpadLib = m_oPinpadLib.Load((unsigned long) m_poContext->m_oPCSC.m_hContext,
			m_hCard, m_csReader, m_csPinpadPrefix, GetLanguage());

		// The GemPC pinpad reader does a "Verify PIN" with empty buffer in an attempt
		// to get and display the remainings attempts. But the BE eID card takes this
		// empty buffer to be a bad PIN, so you quickly end up with a blocked card.
		// Therefore, we don't allow this reader to be used as a pinpad reader..
		if (!m_bUsePinpadLib && StartsWith(m_csReader.c_str(), "Gemplus GemPC Pinpad"))
			return false;

		GetFeatureList();
	}

	switch (operation)
	{
	case PIN_OP_VERIFY: return m_bCanVerifyUnlock;
	case PIN_OP_CHANGE: return m_bCanChangeUnlock;
	default: return false;
	}
}

// See par 4.1.11.3 bmFormatString description
unsigned char CPinpad::ToFormatString(const tPin & pin)
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

// See par 4.1.11.4 bmPINBlockString description
unsigned char CPinpad::ToPinBlockString(const tPin & pin)
{
	switch(pin.encoding)
	{
	case PIN_ENC_ASCII:
		return (unsigned char ) pin.ulStoredLen;
	case PIN_ENC_BCD:
		return (unsigned char ) pin.ulStoredLen;
	case PIN_ENC_GP:
		return 0x40 | (unsigned char ) (pin.ulStoredLen - 1);
	}
	return (unsigned char ) pin.ulStoredLen;
}

// See par 4.1.11.5 bmPINLengthFormat
unsigned char CPinpad::ToPinLengthFormat(const tPin & pin)
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

unsigned char CPinpad::GetMaxPinLen(const tPin & pin)
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

CByteArray CPinpad::PinCmd(tPinOperation operation,
	const tPin & pin, unsigned char ucPinType,
    const CByteArray & oAPDU, unsigned long & ulRemaining)
{
	if (!UsePinpad(operation))
		throw CMWEXCEPTION(EIDMW_ERR_PIN_OPERATION);

	CByteArray oResp;

	if (operation == PIN_OP_VERIFY)
		oResp = PinCmd1(operation, pin, ucPinType, oAPDU, ulRemaining);
	else
		oResp = PinCmd2(operation, pin, ucPinType, oAPDU, ulRemaining);

	if (oResp.Size() != 2)
	{
	  MWLOG(LEV_ERROR, MOD_CAL, L"pinpad reader returned %ls\n", oResp.ToWString().c_str());
	  return EIDMW_ERR_UNKNOWN; // should never happen
	}

	const unsigned char *pucSW12 = oResp.GetBytes();
	if (pucSW12[0] == 0x64 && pucSW12[1] == 0x00)
		throw CMWEXCEPTION(EIDMW_ERR_TIMEOUT);
	if (pucSW12[0] == 0x64 && pucSW12[1] == 0x01)
		throw CMWEXCEPTION(EIDMW_ERR_PIN_CANCEL);
	if (pucSW12[0] == 0x64 && pucSW12[1] == 0x02)
		throw CMWEXCEPTION(EIDMW_NEW_PINS_DIFFER);
	if (pucSW12[0] == 0x64 && pucSW12[1] == 0x03)
		throw CMWEXCEPTION(EIDMW_WRONG_PIN_FORMAT);
	if (pucSW12[0] == 0x6B && pucSW12[1] == 0x80)
		throw CMWEXCEPTION(EIDMW_PINPAD_ERR);

	return oResp;
}

/** For operations involving 1 PIN */
CByteArray CPinpad::PinCmd1(tPinOperation operation,
	const tPin & pin, unsigned char ucPinType,
    const CByteArray & oAPDU, unsigned long & ulRemaining)
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
			ucPinType, pin.csLabel, true);
	}
	else
	{
		PinpadControl(m_ioctlVerifyStart, oCmd, operation,
			ucPinType, pin.csLabel, false);
		return PinpadControl(m_ioctlVerifyFinish, CByteArray(), operation,
			ucPinType, "", true);
	}
}

/** For operations involving 2 PINs */
CByteArray CPinpad::PinCmd2(tPinOperation operation,
	const tPin & pin, unsigned char ucPinType,
    const CByteArray & oAPDU, unsigned long & ulRemaining)
{
	EIDMW_PP_CHANGE_CCID xChangeCmd;
	unsigned long ulChangeCmdLen;

	memset(&xChangeCmd, 0, sizeof(xChangeCmd));
	xChangeCmd.bTimerOut = 30;
	xChangeCmd.bTimerOut2 = 30;
	xChangeCmd.bmFormatString = ToFormatString(pin);
	xChangeCmd.bmPINBlockString = ToPinBlockString(pin);
	xChangeCmd.bmPINLengthFormat = ToPinLengthFormat(pin);
	xChangeCmd.bInsertionOffsetOld = 0x00;
	xChangeCmd.bInsertionOffsetNew = (unsigned char) pin.ulStoredLen;
	xChangeCmd.wPINMaxExtraDigit[0] = GetMaxPinLen(pin);
	xChangeCmd.wPINMaxExtraDigit[1] = (unsigned char) pin.ulMinLen;
	xChangeCmd.bConfirmPIN = 0x03;
	xChangeCmd.bEntryValidationCondition = 0x02;
	xChangeCmd.bNumberMessage = 0x03;
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
			ucPinType, pin.csLabel, true);
	}
	else
	{
		PinpadControl(m_ioctlChangeStart, oCmd, operation,
			ucPinType, pin.csLabel, false);
		return PinpadControl(m_ioctlChangeFinish, CByteArray(), operation,
			ucPinType, "", true);
	}
}

void CPinpad::UnloadPinpadLib()
{
	m_bNewCard = true;
	m_bCanVerifyUnlock = false;
	m_bCanChangeUnlock = false;
	m_bUsePinpadLib = false;
	m_oPinpadLib.Unload();
}

CByteArray CPinpad::PinpadControl(unsigned long ulControl, const CByteArray & oCmd,
	tPinOperation operation, unsigned char ucPintype,
	const std::string & csPinLabel,	bool bShowDlg)
{
	unsigned char pinpadOperation = PinOperation2Lib(operation);

#ifdef PP_DUMP_CMDS
	printf("PP ctrl (%svia pinpad lib): 0x%0x\n", (m_bUsePinpadLib ? "" : "not "), ulControl);
	printf("PP IN: %s\n", oCmd.ToString(true, false, 0, 0xFFFFFFFF).c_str());
#endif

	unsigned long ulDlgHandle;
	bool bCloseDlg = bShowDlg;
	if (bShowDlg)
		bCloseDlg = m_oPinpadLib.ShowDlg(pinpadOperation,
		ucPintype, csPinLabel, m_csReader, &ulDlgHandle);

	CByteArray oResp;
	try
	{
		if (!m_bUsePinpadLib)
		{
			oResp = m_poContext->m_oPCSC.Control(m_hCard, ulControl, oCmd);
		}
		else
		{
			oResp = m_oPinpadLib.PinCmd(m_hCard, ulControl, oCmd, ucPintype,
				pinpadOperation);
		}
	}
	catch (...)
	{
		if (bShowDlg)
			m_oPinpadLib.CloseDlg(ulDlgHandle);
		throw ;
	}
	if (bShowDlg)
		m_oPinpadLib.CloseDlg(ulDlgHandle);

#ifdef PP_DUMP_CMDS
	printf("PP OUT: %s\n", oResp.ToString(true, false, 0, 0xFFFFFFFF).c_str());
#endif

	return oResp;
}

#define CHECK_FEATURE(feature, featureID, iotcl) \
	if (feature[0] == featureID) \
		iotcl = 256 * (256 * ((256 * feature[2]) + feature[3]) + feature[4]) + feature[5];

void CPinpad::GetFeatureList()
{
	m_bCanVerifyUnlock = false;
	m_bCanChangeUnlock = false;
	m_ioctlVerifyStart = m_ioctlVerifyFinish = m_ioctlVerifyDirect = 0;
	m_ioctlChangeStart = m_ioctlChangeFinish = m_ioctlChangeDirect = 0;

	try {
		CByteArray oFeatures = PinpadControl(CCID_IOCTL_GET_FEATURE_REQUEST,
			CByteArray(), PIN_OP_VERIFY, 0, "", false);

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

			if (m_bCanVerifyUnlock || m_bCanChangeUnlock)
				m_ulLangCode = GetLanguage();
		}
	}
	catch(const CMWException & e)
	{
		// very likely CCID_IOCTL_GET_FEATURE_REQUEST isn't supported
		// by this reader -> nothing to do
		e.GetError();
	}

	m_bNewCard = false;
}

unsigned char CPinpad::PinOperation2Lib(tPinOperation operation)
{
	switch(operation)
	{
	case PIN_OP_VERIFY: return EIDMW_PP_OP_VERIFY;
	case PIN_OP_CHANGE: return EIDMW_PP_OP_CHANGE;
	// Add others when needed
	default: throw CMWEXCEPTION(EIDMW_ERR_CHECK);
	}
}

unsigned long CPinpad::GetLanguage()
{
	if (m_ulLangCode == 0)
	{
		m_ulLangCode =  PP_LANG_EN;
		std::wstring csLang = CConfig::GetString(CConfig::EIDMW_CONFIG_PARAM_GENERAL_LANGUAGE);
		if (csLang == L"nl")
			m_ulLangCode = PP_LANG_NL;
		else if (csLang == L"fr")
			m_ulLangCode = PP_LANG_FR;
		else if (csLang == L"de")
			m_ulLangCode = PP_LANG_DE;
	}
	return m_ulLangCode;
}

}

/**************************** logs *******************

1. SPR532 (driver 2.14_2.11, firmware 5.05)

Verify:
	1E 1E 01 47 04 08 04 02 01 CD CD 00 00 00 00 0D
	00 00 00 00 20 00 01 08 20 FF FF FF FF FF FF FF

Change:
	1E 1E 01 47 04 00 08 0C 04 03 02 03 CD CD 00 00
	00 00 00 00 15 00 00 00 00 24 00 01 10 20 FF FF
	FF FF FF FF FF 20 FF FF FF FF FF FF FF

Note: bmFormatString must be 01 (instead of 89) !!!


2. GemPC pinpad

Verify:
	1E 1E 89 47 04 08 04 02 01 CD CD 00 00 00 00 0D
	00 00 00 00 20 00 01 08 20 FF FF FF FF FF FF FF

Change:
	1E 1E 89 47 04 00 08 08 04 03 02 03 CD CD 00 00
	00 00 00 00 15 00 00 00 00 24 00 01 10 20 FF FF
	FF FF FF FF FF 20 FF FF FF FF FF FF FF
00
Note: wPINMaxExtraDigit[0] must be at most 8 (instead of 12) !!

******************************************************/
