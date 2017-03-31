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
/**
 * The CardConnect() function returns a pointer to a CCard object
 * (that should free()-ed when no longer used) that can be used
 * to communicate to a specific card.
 *
 */
#include "CardFactory.h"
#include "UnknownCard.h"
#include "../common/Log.h"
#include "../common/Util.h"
#include "../common/prefix.h"
#include "../common/DynamicLib.h"
#ifdef CAL_PTEID
#include "PteidCard.h"
#endif
#include <vector>
#include <string>

typedef class eIDMW::CCard * (*GETCARDINSTANCE)(unsigned long,char const*,SCARDHANDLE,class eIDMW::CContext *,class eIDMW::GenericPinpad *);
typedef class eIDMW::CCard * (*CONNECTGETCARDINSTANCE)(unsigned long,char const*,class eIDMW::CContext *,class eIDMW::GenericPinpad *);

namespace eIDMW
{

static bool m_bPluginNamesOK = false;
static void GetPluginNames();
static void AddPluginName(const char *csPluginName, const std::string & csPath);

typedef struct {
	std::string csPath;   // path of the card plugin
	std::string csReader; // first part of the reader name, or ""
	bool bFull;           // true if both GetCardInstance() and ConnectGetCardInstance() are supported
} tPluginInfo;

static std::vector <tPluginInfo> m_Plugins;

static CCard *GetCardInstance(const char *csReader, SCARDHANDLE hCard,
	CContext *poContext, GenericPinpad *poPinpad,
	const std::string &csPluginPath, CDynamicLib &oCardPluginLib);

static CCard *ConnectGetCardInstance(const char *csReader,
	CContext *poContext, GenericPinpad *poPinpad,
	const std::string &csPluginPath, CDynamicLib &oCardPluginLib);

CCard * CardConnect(const std::string &csReader,
	CContext *poContext, GenericPinpad *poPinpad, CDynamicLib &oCardPluginLib)
{
	CCard *poCard = NULL;
	long lErrCode = EIDMW_ERR_CHECK; // should never be returned
	const char *strReader = NULL;

	if (poContext->m_ulConnectionDelay != 0)
		CThread::SleepMillisecs(poContext->m_ulConnectionDelay);

	// Try if we can connect to the card via a normal SCardConnect()
	SCARDHANDLE hCard = 0;
	try 
	{
		hCard = poContext->m_oPCSC.Connect(csReader);
		if (hCard == 0){
			goto done;
		}
	}
	catch(CMWException &e)
	{
		if (e.GetError() == (long)EIDMW_ERR_NO_CARD)
			goto done;
		if (e.GetError() != (long)EIDMW_ERR_CANT_CONNECT && e.GetError() != (long)EIDMW_ERR_CARD_COMM)
			throw;
		lErrCode = e.GetError();
		hCard = 0;
	}

	// So a card is present, get the names of the available plugin libs
	if (!m_bPluginNamesOK)
		GetPluginNames();

	strReader = csReader.c_str();

	if (hCard != 0)
	{
		// 1. A card is present and we could connect to it via a normal SCardConnect()
		for (size_t i = 0; poCard == NULL && i < m_Plugins.size(); i++)
		{
			//printf("--- %d ");
			tPluginInfo &plugin = m_Plugins.at(i);
			if (plugin.csReader.size() != 0 && !StartsWith(strReader, plugin.csReader.c_str()))
				continue;
			poCard = GetCardInstance(strReader, hCard, poContext, poPinpad,
				plugin.csPath, oCardPluginLib);
		}

#ifdef CAL_PTEID
		if (poCard == NULL) {
		  StartsWith(csReader.c_str(), "G");
	 	  MWLOG(LEV_DEBUG, MOD_CAL, "Using Reader: %s", csReader.c_str());
		  poCard = PTeidCardGetVersion(PLUGIN_VERSION, strReader, hCard, poContext, poPinpad);
		}

#ifdef CAL_EMULATION
		// Emulated reader doesn't start with "ACS ACR38"

#endif

#endif // CAL_PTEID

#ifndef __APPLE__
		// If no other CCard subclass could be found
        if (poCard == NULL)
        {
			poCard = new CUnknownCard(hCard, poContext, poPinpad, CByteArray());
		}
#else
		hCard = 0;
#endif
	}

	if (hCard == 0)
	{
		// 2. A card is present, but connecting to it is reader-specific (e.g. synchron. cards)
		strReader = csReader.c_str();
		for (size_t i = 0; poCard == NULL && i < m_Plugins.size(); i++)
		{
			tPluginInfo &plugin = m_Plugins.at(i);
			if (!plugin.bFull || (plugin.csReader.size() != 0 && !StartsWith(strReader, plugin.csReader.c_str())))
				continue;
			poCard = ConnectGetCardInstance(strReader, poContext, poPinpad,
				plugin.csPath, oCardPluginLib);
		}

#ifdef CAL_PTEID
		if (poCard == NULL)
			poCard = new CUnknownCard(hCard, poContext, poPinpad, CByteArray());

#endif // CAL_PTEID

		// If the card is still not recognized here, then it may as well
		// be an badly inserted card, so we'll throw the exception that we
		// caught in the beginnin of this function
		if (poCard == NULL)
			throw CMWEXCEPTION(lErrCode);
	}

done:
	return poCard;
}

////////////////////////////////////////////////////////////////////////////////////////

#ifdef WIN32
// Use Dependency Walker (depends.exe) to get these strings
#define csGetCardInstanceName        "?GetCardInstance@@YAPAVCCard@eIDMW@@KPBDKPAVCContext@2@PAVGenericPinpad@2@@Z"
#define csConnectGetCardInstanceName "?ConnectGetCardInstance@@YAPAVCCard@eIDMW@@KPBDPAVCContext@2@PAVGenericPinpad@2@@Z"

#else
// Use nm <dll> to get these strings
#define csGetCardInstanceName        "_Z15GetCardInstancemPKcmPN5eIDMW8CContextEPNS1_7GenericPinpadE"
#define csConnectGetCardInstanceName "_Z22ConnectGetCardInstancemPKcPN5eIDMW8CContextEPNS1_7GenericPinpadE"
#endif

static CCard *GetCardInstance(const char *csReader, SCARDHANDLE hCard,
	CContext *poContext, GenericPinpad *poPinpad,
	const std::string &csPluginPath, CDynamicLib &oCardPluginLib)
{
	CCard * poCard = NULL;

	unsigned long ulErr = oCardPluginLib.Open(csPluginPath);
	if (ulErr != EIDMW_OK)
	{
		MWLOG(LEV_ERROR, MOD_CAL, L"Couldn't load card plugin \"%ls\", err = 0x%0x",
			utilStringWiden(csPluginPath).c_str(), ulErr);
	}
	else
	{
		GETCARDINSTANCE pGetCardInstance =
			(GETCARDINSTANCE) oCardPluginLib.GetAddress(csGetCardInstanceName);
		if (pGetCardInstance == NULL)
		{
			MWLOG(LEV_ERROR, MOD_CAL, L"Function \"GetCardInstance\" not found in \"%ls\"",
				utilStringWiden(csPluginPath).c_str());
		}
		else
		{
			poCard = pGetCardInstance(PLUGIN_VERSION, csReader, hCard, poContext, poPinpad);
			if (poCard)
			{
				MWLOG(LEV_DEBUG, MOD_CAL, L"    CardFactory: using plugin \"%ls\"",
					utilStringWiden(csPluginPath).c_str());
			}
		}
	}

	return poCard;
}

static CCard *ConnectGetCardInstance(const char *csReader,
	CContext *poContext, GenericPinpad *poPinpad,
	const std::string &csPluginPath, CDynamicLib &oCardPluginLib)
{
	CCard * poCard = NULL;

	try
	{
		unsigned long ulErr = oCardPluginLib.Open(csPluginPath);
		if (ulErr != EIDMW_OK)
		{
			MWLOG(LEV_ERROR, MOD_CAL, L"Couldn't load reader specific plugin \"%ls\", err = 0x%0x",
				utilStringWiden(csPluginPath).c_str(), ulErr);
		}
		else
		{
			CONNECTGETCARDINSTANCE pConnectGetCardInstance =
				(CONNECTGETCARDINSTANCE) oCardPluginLib.GetAddress(csConnectGetCardInstanceName);
			if (pConnectGetCardInstance == NULL)
			{
				MWLOG(LEV_ERROR, MOD_CAL, L"Function \"ConnectGetCardInstance\" not found in \"%ls\"",
					utilStringWiden(csPluginPath).c_str());
			}
			else
				poCard = pConnectGetCardInstance(PLUGIN_VERSION, csReader, poContext, poPinpad);
			if (poCard)
			{
				MWLOG(LEV_DEBUG, MOD_CAL, L"    CardFactory: connected via plugin \"%ls\"",
					utilStringWiden(csPluginPath).c_str());
			}		}
	}
	catch(CMWException &e)
	{
		MWLOG(LEV_ERROR, MOD_CAL, L"Exception in ConnectGetCardInstance(): err = 0x%0x\n", e.GetError());
	}
	catch (...)
	{
		MWLOG(LEV_ERROR, MOD_CAL, L"Unknown error in ConnectGetCardInstance()");
	}

	return poCard;
}

////////////////////////////////////////////////////////////////////////////////////////

#ifdef CAL_PTEID
#define EIDMW_PLUGINS_DIR "pteidcardplugins"
#endif

// The function below lists the files in a directory, for which
// no standard platform-independent API seems to exist...

#ifdef WIN32

#include <io.h>
#include <Windows.h>

static void GetPluginNames()
{
	char csSystemDir[_MAX_PATH];
	GetSystemDirectoryA(csSystemDir, sizeof(csSystemDir) - 50);

	// E.g. "C:\WINDOWS\System32\pteidcardplugins\"
	std::string csPluginsDir = csSystemDir + std::string("\\") +
		EIDMW_PLUGINS_DIR + std::string("\\");
	std::string strSearchFor = csPluginsDir + std::string("*.dll");
	const char *csSearchFor = strSearchFor.c_str();

    struct _finddata_t c_file;
	intptr_t hFile = _findfirst(csSearchFor, &c_file);
	if (hFile != -1)
	{
		int iFindRes;
		do
		{
			// On Windows, card plugins are linked to the CAL.
			// So emulation plugins need to be used with the emulation CAL,
			// and 'normal' plugins need to be used with the 'normal' CAL.
#ifdef CAL_EMULATION
			if (strstr(c_file.name, "Emulation") != NULL)
				AddPluginName(c_file.name, csPluginsDir + c_file.name);
#else
			if (strstr(c_file.name, "Emulation") == NULL)
				AddPluginName(c_file.name, csPluginsDir + c_file.name);
#endif
			iFindRes = _findnext(hFile, &c_file);
		}
		while (iFindRes == 0);

		_findclose(hFile);
	}

	m_bPluginNamesOK = true;
}

#else

#include <dirent.h>

static void GetPluginNames()
{
	std::string csPluginsDir = STRINGIFY(EIDMW_PREFIX) + std::string("/lib/") +
		EIDMW_PLUGINS_DIR + std::string("/");
	const char *csSearchFor = csPluginsDir.c_str();

	DIR *pDir = opendir(csSearchFor);
	// If pDir is NULL then the dir doesn't exist
	if(pDir != NULL)
	{
		struct dirent *pFile = readdir(pDir);
		for ( ;pFile != NULL; pFile = readdir(pDir))
		{
			// On Mac, card plugins are linked to the CAL.
			// So emulation plugins need to be used with the emulation CAL,
			// and 'normal' plugins need to be used with the 'normal' CAL.
#if defined CAL_EMULATION && defined __APPLE__
			if (strstr(pFile->d_name, "Emulation") != NULL)
				AddPluginName(pFile->d_name, csPluginsDir + pFile->d_name);
#else
			if (strstr(pFile->d_name, "Emulation") == NULL)
				AddPluginName(pFile->d_name, csPluginsDir + pFile->d_name);
#endif
		}

		closedir(pDir);
	}

	m_bPluginNamesOK = true;
}

#endif

static void AddPluginName(const char *csPluginName, const std::string & csPath)
{
	if (StartsWith(csPluginName, "cardplugin") || StartsWith(csPluginName, "libcardplugin"))
	{
		tPluginInfo plugin;

		plugin.csPath = csPath;

		plugin.bFull = StartsWith(csPluginName, "cardpluginFull") ||
			StartsWith(csPluginName, "libcardpluginFull");
		const char *ptr1 = strstr(csPluginName, "__");
		const char *ptr2 = (ptr1 == NULL ? NULL : strstr(ptr1 + 2, "__"));
		if (ptr2 != NULL && ptr2 - ptr1 < 200)
		{
			ptr1 += 2;
			char csReaderName[200];
			memcpy(csReaderName, ptr1, ptr2 - ptr1);
			csReaderName[ptr2 - ptr1] = '\0';
			plugin.csReader = csReaderName;
		}

		m_Plugins.push_back(plugin);
	}
}

}
