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
#ifdef UNICODE
#undef UNICODE
#endif

#include "../PCSC.h"
#include "SISPluginHandler.h"
#include "SISCard.h"
#include "SISplugin1.h"
#include "../../common/Log.h"
#include "../../common/prefix.h"

namespace eIDMW
{

static bool m_bPluginInfosOK = false;
static void GetPluginInfos();

static void AddPluginName(const char *csPluginName, const std::string & csPath);

static void GetPCSCFunctions(CPCSC *poPCSC, tPCSCfunctions *pxPCSCFunctions);

static void AddPluginInfo(const std::string & csPath, const std::string & csReader);
static size_t PlugInCount();
static std::string & GetPlugInPath(size_t idx);
static std::string & GetPlugInReader(size_t idx);

typedef long (*SISPLUGINREADCARD)(unsigned long, void *,
	const char *, SCARDHANDLE *,
	unsigned char *,
	unsigned long, void *);


/** Obtain the SIS-data via the plugin library
*/
CCard * SISPluginReadData(const char *csReader, SCARDHANDLE hCard,
	CContext *poContext, CPinpad *poPinpad,
	CDynamicLib &oCardPluginLib)
{
	CCard *poCard = NULL;

	if (!m_bPluginInfosOK)
		GetPluginInfos();

	for (size_t i = 0; poCard == NULL && i < PlugInCount(); i++)
	{
		if (!StartsWith(csReader, GetPlugInReader(i).c_str()))
			continue;

		std::string csPluginPath = GetPlugInPath(i);
		unsigned long ulErr = oCardPluginLib.Open(csPluginPath);
		if (ulErr != EIDMW_OK)
		{
			MWLOG(LEV_ERROR, MOD_CAL, L"Couldn't load SIS plugin \"%ls\", err = 0x%0x",
				utilStringWiden(csPluginPath).c_str(), ulErr);
			continue;
		}

		SISPLUGINREADCARD pSisPluginReadCard =
				(SISPLUGINREADCARD) oCardPluginLib.GetAddress("SISPluginReadCard");
		if (pSisPluginReadCard == NULL)
		{
			MWLOG(LEV_ERROR, MOD_CAL, L"Function \"SISPluginReadCard\" not found in \"%ls\"",
				utilStringWiden(csPluginPath).c_str(), ulErr);
			continue;
		}

		tPCSCfunctions xPCSCFunctions;
		GetPCSCFunctions(&poContext->m_oPCSC, &xPCSCFunctions);

		unsigned char tucData[500];
		SCARDHANDLE hCrd = hCard;
		long lErr = pSisPluginReadCard(SISPLUGIN_VERSION, &xPCSCFunctions,
			csReader, &hCrd, tucData, 0, NULL);
		hCard = (unsigned long) hCrd;

		if (lErr != 0)
		{
			MWLOG(LEV_ERROR, MOD_CAL, L"Function \"SISPluginReadCard\" in \"%ls\" returned 0x%0x (%d)",
				utilStringWiden(csPluginPath).c_str(), lErr, lErr);
			continue;
		}

		MWLOG(LEV_DEBUG, MOD_CAL, L"Using SIS plugin \"%ls\"",
			utilStringWiden(csPluginPath).c_str());
		poCard = new CSISCard(hCard, poContext, poPinpad, CByteArray(tucData, 404));

		break;
	}

	return poCard;;
}

//////////////////////////////////////////////////////////////////////////////////
// retrieve all available plug-in's
// The function below lists the files in a directory, for which
// no standard platform-independent API seems to exist...

#ifdef WIN32

#include <io.h>
#include <Windows.h>

static void GetPluginInfos()
{
	char csSystemDir[_MAX_PATH];
	GetSystemDirectoryA(csSystemDir, sizeof(csSystemDir) - 50);

	// E.g. "C:\WINDOWS\System32\siscardplugins\"
	std::string csPluginsDir = csSystemDir + std::string("\\siscardplugins\\");
	std::string strSearchFor = csPluginsDir + std::string("*.dll");
	const char *csSearchFor = strSearchFor.c_str();

    struct _finddata_t c_file;
	intptr_t hFile = _findfirst(csSearchFor, &c_file);
	if (hFile != -1)
	{
		int iFindRes;
		do
		{
			AddPluginName(c_file.name, csPluginsDir + c_file.name);

			iFindRes = _findnext(hFile, &c_file);
		}
		while (iFindRes == 0);

		_findclose(hFile);
	}

	m_bPluginInfosOK = true;
}

#else

#include <dirent.h>

static void GetPluginInfos()
{
	std::string csPluginsDir = STRINGIFY(EIDMW_PREFIX) + std::string("/lib/siscardplugins/");
	const char *csSearchFor = csPluginsDir.c_str();

	DIR *pDir = opendir(csSearchFor);
	// If pDir is NULL then the dir doesn't exist
	if(pDir != NULL)
	{
		struct dirent *pFile = readdir(pDir);
		for ( ;pFile != NULL; pFile = readdir(pDir))
		{
			AddPluginName(pFile->d_name, csPluginsDir + pFile->d_name);
		}

		closedir(pDir);
	}

	m_bPluginInfosOK = true;
}

#endif

static void AddPluginName(const char *csPluginName, const std::string & csPath)
{
	if (StartsWith(csPluginName, "siscardplugin1") || StartsWith(csPluginName, "libsiscardplugin1"))
	{
		const char *ptr1 = strstr(csPluginName, "__");
		const char *ptr2 = (ptr1 == NULL ? NULL : strstr(ptr1 + 2, "__"));
		if (ptr2 != NULL && ptr2 - ptr1 < 200)
		{
			ptr1 += 2;
			char csReaderName[200];
			memcpy(csReaderName, ptr1, ptr2 - ptr1);
			csReaderName[ptr2 - ptr1] = '\0';
			if (memcmp(csReaderName, "ACS_ACR38U", sizeof("ACS_ACR38U"))==0)
			{
				memcpy(csReaderName, "ACS ACR38U", sizeof("ACS ACR38U"));
			}
			AddPluginInfo(csPath, csReaderName);
		}
	}
}

////////////////

#define MAX_PLUGINS 50

std::string g_tcsPaths[MAX_PLUGINS];
std::string g_tcsReaders[MAX_PLUGINS];

static size_t g_pluginCount = 0;

static std::string g_csEmpty = "";

static void AddPluginInfo(const std::string & csPath, const std::string & csReader)
{
	if (g_pluginCount >= MAX_PLUGINS)
		return;

	g_tcsPaths[g_pluginCount] = csPath;
	g_tcsReaders[g_pluginCount] = csReader;

	g_pluginCount++;
}

static size_t PlugInCount()
{
	return g_pluginCount;
}

static std::string & GetPlugInPath(size_t idx)
{
	if (idx < g_pluginCount)
		return g_tcsPaths[idx];
	return g_csEmpty;
}

static std::string & GetPlugInReader(size_t idx)
{
	if (idx < g_pluginCount)
		return g_tcsReaders[idx];
	return g_csEmpty;
}

//////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>

static CPCSC *g_poPCSC = NULL;

long CardConnect(LPCSTR szReader,
  DWORD dwShareMode,
  DWORD dwPreferredProtocols,
  LPSCARDHANDLE phCard,
  LPDWORD pdwActiveProtocol)
{
	long lRet = SCardConnect(g_poPCSC->GetContext(), szReader,
			dwShareMode, dwPreferredProtocols, phCard, pdwActiveProtocol);

	//printf("SCardConnect(sharemode=0x%0x, prefprotocols=0x%0x): r = 0x%0x (%d), protocol=0x%0x\n\n",
	//	dwShareMode, dwPreferredProtocols, lRet, lRet, * pdwActiveProtocol);

	return lRet;
}

long CardDisconnect(SCARDHANDLE hCard, DWORD dwDisposition)
{
	long lRet = SCardDisconnect(hCard, dwDisposition);

	//printf("SCardDisconnect(hCard=0x%0x, disp=0x%0x): r = 0x%0x (%d)\n\n",
	//	hCard, dwDisposition, lRet, lRet);

	return lRet;
}

long CardBeginTransaction(SCARDHANDLE hCard)
{
	return SCardBeginTransaction(hCard);
}

long CardEndTransaction(SCARDHANDLE hCard, DWORD dwDisposition)
{
	return SCardEndTransaction(hCard, dwDisposition);
}

long CardTransmit(SCARDHANDLE hCard,
	LPCSCARD_IO_REQUEST pioSendPci, LPCBYTE pbSendBuffer, DWORD cbSendLength,
	LPSCARD_IO_REQUEST pioRecvPci, LPBYTE pbRecvBuffer, LPDWORD pcbRecvLength)
{
	long lRet = SCardTransmit(hCard,
		pioSendPci, pbSendBuffer, cbSendLength,
		pioRecvPci, pbRecvBuffer, pcbRecvLength);

	//printf("SCardTransmit(hCard=0x%0x, sendprot=0x%0x, sent %d bytes, recvprot=0x%0x): r = 0x%0x (%d), recv %d bytes\n",
	//hCard, pioSendPci->dwProtocol, cbSendLength, pioRecvPci->dwProtocol, lRet, lRet, * pcbRecvLength);

	return lRet;
}

long CardControl(SCARDHANDLE hCard, DWORD dwControlCode,
	LPCVOID lpInBuffer, DWORD nInBufferSize,
	LPVOID lpOutBuffer, DWORD nOutBufferSize, LPDWORD lpBytesReturned)
{
#ifndef __OLD_PCSC_API__
	long lRet = SCardControl(hCard, dwControlCode,
		lpInBuffer, nInBufferSize,
		lpOutBuffer, nOutBufferSize, lpBytesReturned);

	//printf("SCardControl()\n");

	return lRet;
#else
        long lRet = SCardControl(hCard,
                (const unsigned char *) lpInBuffer, nInBufferSize,
                (unsigned char*) lpOutBuffer, lpBytesReturned);

	//printf("SCardControl()\n");

	return lRet;
#endif
}

long Ctrl(unsigned long ulControl, const void *pIn, void *pOut)
{
	return -1;
}

static void GetPCSCFunctions(CPCSC *poPCSC, tPCSCfunctions *pxPCSCFunctions)
{
	g_poPCSC = poPCSC;

	pxPCSCFunctions->pCardConnect = CardConnect;
	pxPCSCFunctions->pCardDisconnect = CardDisconnect;
	pxPCSCFunctions->pCardBeginTransaction = CardBeginTransaction;
	pxPCSCFunctions->pCardEndTransaction = CardEndTransaction;
	pxPCSCFunctions->pCardTransmit = CardTransmit;
	pxPCSCFunctions->pCardControl = CardControl;
	pxPCSCFunctions->pCtrl = Ctrl;
}

}
