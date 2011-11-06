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
#include "PinpadLibOldPteid.h"
#include "../CardLayerConst.h"
#include "../common/MWException.h"
#include "../common/eidErrors.h"
#include "../common/Util.h"

using namespace eIDMW;

static char *tcsLangs[4] = {"en", "nl", "fr", "de"};

static char *tcsUsageSig[4] = {
	"Signature",
	"Handtekening",
	"Signature",
	"Unterschrift"
};

static char *tcsUsageAuth[4] = {
	"Authentication",
	"Authentificatie",
	"Authentification",
	"Authentifizierung"
};

static unsigned char aid_belpic[] = {0xA0, 0x00, 0x00, 0x01, 0x77, 0x50, 0x4B, 0x43, 0x53, 0x2D, 0x31, 0x35};

static SCR_Application xApp = {
	{aid_belpic, sizeof(aid_belpic)},
	"ID",
	NULL
};

static int LangToId(unsigned long ulLanguage)
{
	switch(ulLanguage)
	{
	case 0x0813: return 1;//LANG_NL
	case 0x080c: return 2;//LANG_FR
	case 0x0407: return 3;//LANG_DE
	}
	return 0;
}

/////////////////////////////////////////////////////////

CPinpadLibOldPteid::CPinpadLibOldPteid()
{
	m_pVerifyPin = NULL;
	m_pChangePin = NULL;
	m_iLangIdx = 0;
}

CPinpadLibOldPteid::~CPinpadLibOldPteid()
{
}

bool CPinpadLibOldPteid::Load(unsigned long hContext, SCARDHANDLE hCard,
	const std::string & strReader, const std::string & strPinpadPrefix,
	unsigned long ulLanguage)
{
	bool bPinpadLibFound = false;
	char csSystemDir[_MAX_PATH];
	GetSystemDirectoryA(csSystemDir, sizeof(csSystemDir) - 50);

	// E.g. "C:\WINDOWS\System32\pteidpp\" or "/usr/local/lib/pteidpp/"
	std::string csPinpadDir = csSystemDir + std::string("\\") +
		strPinpadPrefix + "\\";

	const char *csReader = strReader.c_str();
	const char *csPinpadPrefix = strPinpadPrefix.c_str();

	std::string strSearchFor = csPinpadDir + "*.dll";
	const char *csSearchFor = strSearchFor.c_str();

	// Search for files in csPinpadDir that are candidate pinpad lib,
	// load them and ask them if they support this reader + pinpad lib version
    struct _finddata_t c_file;
	intptr_t hFile = _findfirst(csSearchFor, &c_file);
	if (hFile != -1)
	{
		int iFindRes;
		do
		{
			// Old pinpad lib are not assumed to start with "pteidpp"
			if (!StartsWithCI(c_file.name, csPinpadPrefix))
			{
				bPinpadLibFound = CheckLib(csPinpadDir, c_file.name,
					ulLanguage, hContext, hCard, csReader);
				if (bPinpadLibFound)
					break; // OK: a good pinpad lib was found and loaded
			}

			iFindRes = _findnext(hFile, &c_file);
		}
		while (iFindRes == 0);

		_findclose(hFile);
	}

	if (!bPinpadLibFound)
	{
		// Try the old locations (in the system dir)

		// Append a \ to the system dir name
		size_t len = strlen(csSystemDir);
		csSystemDir[len++] = '\\';
		csSystemDir[len] = '\0';

		if (StartsWith(csReader, "Xiring X"))
		{
			bPinpadLibFound = CheckLib(csSystemDir, "xireid.dll",
				ulLanguage, hContext, hCard, csReader);
		}
		else if (StartsWith(csReader, "Cherry "))
		{
			bPinpadLibFound = CheckLib(csSystemDir, "ch44xxeid.dll",
				ulLanguage, hContext, hCard, csReader);
		}
		else if (StartsWith(csReader, "SCM Microsystems Inc. SPRx32 USB Smart Card Reader"))
		{
			bPinpadLibFound = CheckLib(csSystemDir, "sprx32eid.dll",
				ulLanguage, hContext, hCard, csReader);
		}
	}

	if (bPinpadLibFound)
		m_iLangIdx = LangToId(ulLanguage);

	return bPinpadLibFound;
}

void CPinpadLibOldPteid::UnLoad()
{
	m_oPinpadLib.Close();
}

static unsigned char tucErrTimeout[] = {0x64, 0x00};
static unsigned char tucErrCancel[] = {0x64, 0x01};
static unsigned char tucErrPinsDiffer[] = {0x64, 0x02};
static unsigned char tucErrGeneral[] = {0x6B, 0x80};

CByteArray CPinpadLibOldPteid::PinCmd(SCARDHANDLE hCard, unsigned long ulControl,
	CByteArray oCmd, unsigned char ucPintype, unsigned char ucOperation)
{
	if (ulControl == CCID_IOCTL_GET_FEATURE_REQUEST)
	{
		unsigned char tucFeatures[] = {0x06, 0x04, 0x00, 0x31, 0x32, 0x33, 0x07, 0x04, 0x00 ,0x31, 0x32, 0x33};
		return CByteArray(tucFeatures, sizeof(tucFeatures));
	}

	SCR_Card xCard = {
		hCard,
		tcsLangs[m_iLangIdx],
		{NULL, 0},
		NULL
	};
	unsigned char tucStatus[2];
	long lRet = SCARD_F_INTERNAL_ERROR;

	if (ucOperation == EIDMW_PP_OP_VERIFY)
	{
		SCR_PinUsage xPinUsage = {
			ucPintype == EIDMW_PP_TYPE_SIGN ? SCR_USAGE_SIGN: SCR_USAGE_AUTH,
			ucPintype == EIDMW_PP_TYPE_SIGN ? "SIG" : "AUT",
			ucPintype == EIDMW_PP_TYPE_SIGN ? tcsUsageSig[m_iLangIdx] : tcsUsageAuth[m_iLangIdx]
		};

		lRet = m_pVerifyPin(&xCard, oCmd.GetByte(22), &xPinUsage, &xApp, tucStatus);
	}
	else if (ucOperation == EIDMW_PP_OP_CHANGE)
	{
		lRet = m_pChangePin(&xCard, oCmd.GetByte(27), &xApp, tucStatus);	
	}
	else
		throw CMWEXCEPTION(EIDMW_ERR_PIN_OPERATION);

	CByteArray oResp(2);
	switch(lRet)
	{
	case SCARD_S_SUCCESS:
		if (tucStatus[0] == 0xEC && tucStatus[1] == 0xD2)
			oResp.Append(tucErrTimeout, sizeof(tucErrTimeout));
		else if (tucStatus[0] == 0xEC && tucStatus[1] == 0xD6)
			oResp.Append(tucErrCancel, sizeof(tucErrCancel));
		else
			oResp.Append(tucStatus, 2);
		break;
	case SCARD_E_CANCELLED:
		oResp.Append(tucErrCancel, sizeof(tucErrCancel));
		break;
	case SCARD_W_REMOVED_CARD:
		throw CMWEXCEPTION(EIDMW_ERR_NO_CARD);
		break;
	case SCR_I_PIN_CHECK_FAILED:
		oResp.Append(tucErrPinsDiffer, sizeof(tucErrPinsDiffer));
		break;
	default:
		oResp.Append(tucErrGeneral, sizeof(tucErrGeneral));
	}

	return oResp;
}

bool CPinpadLibOldPteid::UseOldLib()
{
	return m_pVerifyPin != NULL && m_pChangePin != NULL;
}

bool CPinpadLibOldPteid::CheckLib(
	const std::string & csPinpadDir, const char *csFileName,
	unsigned long ulLanguage, unsigned long hContext,
	SCARDHANDLE hCard, const char *csReader)
{
	bool bRet = false;

	// This is no pinpad DLL (it doesn't hurt to try it out, but it's faster not to)
	if (StartsWithCI(csFileName, "belpicppgui"))
		return false;

	// Some known reader/lib pairs:
	if (StartsWithCI(csFileName, "xireid") && !StartsWith(csReader, "Xiring X"))
		return false;
	if (StartsWithCI(csFileName, "ch44xxeid") && !StartsWith(csReader, "Cherry "))
		return false;
	if (StartsWithCI(csFileName, "sprx32eid") && !StartsWith(csReader, "SCM Microsystems Inc. SPRx32 USB Smart Card Reader"))
		return false;

	PTEID_OLD_PP_INIT pInit = NULL;
	PTEID_OLD_VERIFY_PIN pVerifyPin = NULL;
	PTEID_OLD_CHANGE_PIN pChangePin = NULL;

	// Load the pinpad lib
	unsigned long ulRes = m_oPinpadLib.Open(csPinpadDir + csFileName);
	if (ulRes == EIDMW_OK)
	{
		// Get the 3 functions
		pInit = (PTEID_OLD_PP_INIT) m_oPinpadLib.GetAddress("SCR_Init");
		pVerifyPin = (PTEID_OLD_VERIFY_PIN) m_oPinpadLib.GetAddress("SCR_VerifyPIN");
		pChangePin = (PTEID_OLD_CHANGE_PIN) m_oPinpadLib.GetAddress("SCR_ChangePIN");

		if (pInit == NULL || pVerifyPin == NULL || pChangePin == NULL)
			m_oPinpadLib.Close();
		else
		{
			SCR_SupportConstants supported;
			long lRet = pInit(csReader, 1, &supported);
			if (lRet == SCARD_S_SUCCESS && supported == SCR_SUPPORT_OK)
				bRet = true; // OK, the pinpad lib supports this reader
			else
				m_oPinpadLib.Close();
		}
	}

	if (bRet)
	{
		m_pVerifyPin = pVerifyPin;
		m_pChangePin = pChangePin;
	}

	return bRet;
}
