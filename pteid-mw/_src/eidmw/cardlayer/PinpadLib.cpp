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
#include "PinpadLib.h"
#include "../common/MWException.h"
#include "../common/prefix.h"
#include "../common/Util.h"

using namespace eIDMW;

CPinpadLib::CPinpadLib()
{
	m_guiInfo.csVerifyInfo = NULL;
	m_guiInfo.csChangeInfo = NULL;
	m_guiInfo.csUnblockNoChangeInfo = NULL;
	m_guiInfo.csUnblockChangeInfo = NULL;
	m_guiInfo.csUnblockMergeNoChangeInfo = NULL;
	m_guiInfo.csUnblockMergeChangeInfo = NULL;
	m_ppCmd2 = NULL;
	ClearGuiInfo();
}

CPinpadLib::~CPinpadLib()
{
	ClearGuiInfo();
}

void CPinpadLib::Unload()
{
	m_oPinpadLib.Close();

#if defined WIN32 && defined BEID_OLD_PINPAD
	m_oPinpadLibOldBeid.UnLoad();
#endif
}

CByteArray CPinpadLib::PinCmd(SCARDHANDLE hCard, unsigned long ulControl,
	CByteArray oCmd, unsigned char ucPintype, unsigned char ucOperation)
{
#if defined WIN32 && defined BEID_OLD_PINPAD
	if (m_oPinpadLibOldBeid.UseOldLib())
		return m_oPinpadLibOldBeid.PinCmd(hCard, ulControl, oCmd, ucPintype, ucOperation);
#endif

	if (m_ppCmd2 == NULL)
		throw CMWEXCEPTION(EIDMW_ERR_UNKNOWN); // shouldn't happen

	unsigned char tucOut[258];
	DWORD dwOutLen = sizeof(tucOut);
	long lRet = m_ppCmd2(hCard, (int) ulControl, oCmd.GetBytes(), oCmd.Size(),
		tucOut, sizeof(tucOut), &dwOutLen,
		ucPintype, ucOperation, 0, NULL);
	if (lRet != SCARD_S_SUCCESS)
		throw CMWEXCEPTION(EIDMW_ERR_PINPAD);

	return CByteArray(tucOut, dwOutLen);
}

bool CPinpadLib::CheckLib(const std::string & csPinpadDir, const char *csFileName,
	unsigned long ulLanguage, int iVersion,
	unsigned long hContext, SCARDHANDLE hCard, const char *csReader)
{
	bool bRet = false;
	// Load the pinpad lib
	unsigned long ulRes = m_oPinpadLib.Open(csPinpadDir + csFileName);
	if (ulRes == EIDMW_OK)
	{
		// Get the 2 functions
		EIDMW_PP2_INIT ppInit2 = (EIDMW_PP2_INIT) m_oPinpadLib.GetAddress("EIDMW_PP2_Init");
		m_ppCmd2 = (EIDMW_PP2_COMMAND) m_oPinpadLib.GetAddress("EIDMW_PP2_Command");
		if (ppInit2 == NULL || m_ppCmd2 == NULL)
			m_oPinpadLib.Close();
		else
		{
			long lRet = ppInit2(PTEID_MINOR_VERSION, hContext, hCard, csReader,
				ulLanguage, InitGuiInfo(), 0, NULL);
			if (lRet == SCARD_S_SUCCESS)
				bRet = true; // OK, the pinpad lib supports this reader
			else
				m_oPinpadLib.Close();
		}
	}

	if (!bRet)
		m_ppCmd2 = NULL;

	return bRet;
}

bool CPinpadLib::ShowDlg(unsigned char pinpadOperation, unsigned char ucPintype,
	const std::string & csPinLabel, const std::string & csReader,
	unsigned long *pulDlgHandle)
{
	const char *csMesg = GetGuiMesg(pinpadOperation);
	if (csMesg == NULL)
		csMesg = "";

	// If the pinpad lib said "r", it means don't display a dialog
	if (strcmp(csMesg, "r") == 0)
		return false;
	else
	{
		DlgPinUsage dlgUsage = DLG_PIN_UNKNOWN;
		switch(ucPintype)
		{
		case EIDMW_PP_TYPE_AUTH: dlgUsage = DLG_PIN_AUTH; break;
		case EIDMW_PP_TYPE_SIGN: dlgUsage = DLG_PIN_SIGN; break;
		case EIDMW_PP_TYPE_ADDR: dlgUsage = DLG_PIN_ADDRESS; break;
		}

		DlgPinOperation dlgOperation;
		switch(pinpadOperation)
		{
		case EIDMW_PP_OP_VERIFY: dlgOperation = DLG_PIN_OP_VERIFY; break;
		case EIDMW_PP_OP_CHANGE: dlgOperation = DLG_PIN_OP_CHANGE; break;
		default: throw CMWEXCEPTION(EIDMW_ERR_CHECK);
		}
		std::wstring wideReader = utilStringWiden(csReader);	
		std::wstring widePinLabel = utilStringWiden(csPinLabel);
		std::wstring wideMesg = utilStringWiden(csMesg);
		return EIDMW_OK == DlgDisplayPinpadInfo(dlgOperation,
			wideReader.c_str(), dlgUsage,
			widePinLabel.c_str(), wideMesg.c_str(), pulDlgHandle);
	}
}

void CPinpadLib::CloseDlg(unsigned long ulDlgHandle)
{
	DlgClosePinpadInfo(ulDlgHandle);
}

const char *CPinpadLib::GetGuiMesg(unsigned char ucOperation)
{
	switch(ucOperation)
	{
	case EIDMW_PP_OP_VERIFY: return m_guiInfo.csVerifyInfo;
	case EIDMW_PP_OP_CHANGE: return m_guiInfo.csChangeInfo;
	// Others to be added later when needed
	default: throw CMWEXCEPTION(EIDMW_ERR_CHECK);
	}
}

#define DO_ALLOC(cs) \
	if (!(cs)) \
		cs = new char[DLG_INFO_CHARS + 1];

tGuiInfo *CPinpadLib::InitGuiInfo()
{
	DO_ALLOC(m_guiInfo.csVerifyInfo)
	DO_ALLOC(m_guiInfo.csChangeInfo)
	DO_ALLOC(m_guiInfo.csUnblockNoChangeInfo)
	DO_ALLOC(m_guiInfo.csUnblockChangeInfo)
	DO_ALLOC(m_guiInfo.csUnblockMergeNoChangeInfo)
	DO_ALLOC(m_guiInfo.csUnblockMergeChangeInfo)

	if (!m_guiInfo.csVerifyInfo || ! m_guiInfo.csChangeInfo ||
		!m_guiInfo.csUnblockNoChangeInfo || !m_guiInfo.csUnblockChangeInfo ||
		!m_guiInfo.csUnblockMergeNoChangeInfo || !m_guiInfo.csUnblockMergeChangeInfo)
	{
		ClearGuiInfo();
		throw CMWEXCEPTION(EIDMW_ERR_MEMORY);
	}

	return &m_guiInfo;
}

#define DO_FREE_CLEAR(cs) \
	if (cs) \
		free(cs); \
	cs = NULL;

void CPinpadLib::ClearGuiInfo()
{
	DO_FREE_CLEAR (m_guiInfo.csVerifyInfo)
	DO_FREE_CLEAR(m_guiInfo.csChangeInfo)
	DO_FREE_CLEAR(m_guiInfo.csUnblockNoChangeInfo)
	DO_FREE_CLEAR(m_guiInfo.csUnblockChangeInfo)
	DO_FREE_CLEAR(m_guiInfo.csUnblockMergeNoChangeInfo)
	DO_FREE_CLEAR(m_guiInfo.csUnblockMergeChangeInfo)
}

/** Little helper function for the Load() method */
static inline std::string GetSearchString(const std::string & csDir,
	const std::string & csPinpadPrefix, int iVersion)
{
	// E.g. C:\WINDOWS\System32\beidpp\beidpp2*.*
#ifdef WIN32
	char csBuf[20];
	_itoa_s(iVersion, csBuf, sizeof(csBuf), 10);
	return csDir + csPinpadPrefix + csBuf + std::string("*.*");
#else
	return csDir ;
#endif
}

//////////////////////////////////////////////////////////////////////

// The Load() method lists the files in a directory, for which
// no standard platform-independent API seems to exist...

#ifdef WIN32

#include <io.h>
#include <Windows.h>

bool CPinpadLib::Load(unsigned long hContext, SCARDHANDLE hCard,
		const std::string & strReader, const std::string & csPinpadPrefix,
		unsigned long ulLanguage)
{
	bool bPinpadLibFound = false;
	char csSystemDir[_MAX_PATH];
	GetSystemDirectoryA(csSystemDir, sizeof(csSystemDir) - 50);

	// E.g. "C:\WINDOWS\System32\beidpp\" or "/usr/local/lib/pteidpp/"
	std::string csPinpadDir = csSystemDir + std::string("\\") +
		csPinpadPrefix + "\\";

	const char *csReader = strReader.c_str();

	// For each supported pinpad lib version (starting with the most recent one)
	for (int iVersion = 2; iVersion >= 2 && !bPinpadLibFound; iVersion--)
	{
		std::string strSearchFor = GetSearchString(csPinpadDir,
			csPinpadPrefix, iVersion);
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
				bPinpadLibFound = CheckLib(csPinpadDir, c_file.name,
					ulLanguage, iVersion, hContext, hCard, csReader);
				if (bPinpadLibFound)
					break; // OK: a good pinpad lib was found and loaded

				iFindRes = _findnext(hFile, &c_file);
			}
			while (iFindRes == 0);

			_findclose(hFile);
		}
	}

#ifdef BEID_OLD_PINPAD
	if (!bPinpadLibFound)
	{
		bPinpadLibFound = m_oPinpadLibOldBeid.Load(hContext, hCard, strReader,
			csPinpadPrefix, ulLanguage);
		if (bPinpadLibFound)
		{
			InitGuiInfo();
			// We'll display a notification dialog, even if the old pinpad lib
			// would show one by itself.
			m_guiInfo.csVerifyInfo[0] = '\0';
			m_guiInfo.csChangeInfo[0] = '\0';
		}
	}
#endif

	return bPinpadLibFound;
}

#else  // For Linux and Mac OS X

#include <dirent.h>

bool CPinpadLib::Load(unsigned long hContext, SCARDHANDLE hCard,
		      const std::string & strReader, const std::string & csPinpadPrefix,
		      unsigned long ulLanguage)
{
	bool bPinpadLibFound = false;
	std::string csPinpadDir = std::string(STRINGIFY(EIDMW_PREFIX)) + "/lib/" + csPinpadPrefix + "/";
	const char *csReader = strReader.c_str();

	// For each supported pinpad lib version (starting with the most recent one)
	for (int iVersion = 2; iVersion >= 2 && !bPinpadLibFound; iVersion--)
	{
		std::string strSearchFor = GetSearchString(csPinpadDir,
			csPinpadPrefix, iVersion);
		const char *csSearchFor = strSearchFor.c_str();

		// Search for files in csPinpadDir that are candidate pinpad lib,
		// load them and ask them if they support this reader + pinpad lib version
		DIR *pDir = opendir(csSearchFor);
		// If pDir is NULL then the dir doesn't exist
		if(pDir != NULL)
		{
			struct dirent *pFile = readdir(pDir);
			char csBuf[50];
			sprintf_s(csBuf,sizeof(csBuf), "lib%s%i", csPinpadPrefix.c_str(), iVersion);
			csBuf[sizeof(csBuf) - 1] = '\0';
			for ( ;pFile != NULL; pFile = readdir(pDir))
			{
			  // only look at files called libbeidpp<version> 
			  if(strstr(pFile->d_name, csBuf) == NULL ) continue;
			    
				bPinpadLibFound = CheckLib(csPinpadDir, pFile->d_name,
							   ulLanguage, iVersion, hContext, hCard, csReader);
			  
			  if (bPinpadLibFound)
				break; // OK: a good pinpad lib was found and loaded
			}
			closedir(pDir);
		}
	}
	return bPinpadLibFound;
}

#endif
