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
/****************************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

#include "globmdrv.h"
#include "log.h"

/****************************************************************************************************/


#define MAX_LOG_DIR_NAME	4800
#define MAX_LOG_FILE_NAME	5000
char    g_szLogFile[MAX_LOG_FILE_NAME]  = "C:\\SmartCardMinidriverLog\\pteidmdrv.LOG";

#ifdef _DEBUG
unsigned int   g_uiLogLevel      = LOGTYPE_TRACE;
#else
unsigned int   g_uiLogLevel      = LOGTYPE_INFO;
#endif

/****************************************************************************************************/
void LogInit()
{
	DWORD       dwRet;
	HKEY        hKey;
	BYTE        lpData[MAX_LOG_DIR_NAME];
	DWORD       dwData = 0; 

	dwRet = RegOpenKeyEx (HKEY_LOCAL_MACHINE, TEXT("Software\\PTEID\\Logging"), 0, KEY_READ, &hKey);

	if (dwRet != ERROR_SUCCESS) {
		//If the registry key is not defined write the logfile in TMP directory
		dwRet = GetEnvironmentVariable("TMP", lpData, MAX_LOG_DIR_NAME);
		if (dwRet > 0 && dwRet <= MAX_LOG_DIR_NAME)
		{
			lstrcpy(g_szLogFile, lpData);
			lstrcat(g_szLogFile, TEXT("\\pteidmdrv.log"));
		}
		return;
	}

	// getting log_level
	dwData = sizeof(lpData);
	dwRet = RegQueryValueEx( hKey,
		TEXT("log_level"),
		NULL,
		NULL,
		(LPBYTE) lpData,
		&dwData );

	if (dwRet == ERROR_SUCCESS) {
		// log_level found
		// Read loglevels from registry and map on pteid middleware loglevels
		// debug   -> LOGTYPE_TRACE
		// info    -> LOGTYPE_INFO
		// warning -> LOGTYPE_WARNING
		// error   -> LOGTYPE_ERROR
		// none    -> LOGTYPE_NONE

		if (!lstrcmp((LPTSTR)lpData,TEXT("debug")))
			g_uiLogLevel = LOGTYPE_TRACE;
		else if (!lstrcmp((LPTSTR)lpData,TEXT("info")))
			g_uiLogLevel = LOGTYPE_INFO;
		else if (!lstrcmp((LPTSTR)lpData,TEXT("warning")))
			g_uiLogLevel = LOGTYPE_WARNING;
		else if (!lstrcmp((LPTSTR)lpData,TEXT("error")))
			g_uiLogLevel = LOGTYPE_ERROR;
		else if (!lstrcmp((LPTSTR)lpData,TEXT("none")))
			g_uiLogLevel = LOGTYPE_NONE;
	}

	//getting log_dirname
	dwData = sizeof(lpData);
	dwRet = RegQueryValueEx( hKey,
		TEXT("log_dirname"),
		NULL,
		NULL,
		(LPBYTE) lpData,
		&dwData );

	if (dwRet == ERROR_SUCCESS && dwData != 0) {
		// log_dirname found
		// we are not sure the string is null-terminated
		if (dwData == sizeof(lpData))
			dwData--;//replace last character with \0

		lpData[dwData] = '\0';
		// put dirname in global var
		lstrcpy(g_szLogFile, lpData);
		// append file name
		lstrcat(g_szLogFile, TEXT("\\pteidmdrv.log"));
	}
}


/****************************************************************************************************/

void PteidLogInit()
{
    DWORD 		dwRet;
    HKEY 		hKey;
    BYTE        lpData[512];
    DWORD       dwData = 0;

    printf("\nRetrieving the data...");

    dwRet = RegOpenKeyEx (HKEY_LOCAL_MACHINE, TEXT("Software\\PTEID\\Logging"), 0, KEY_READ, &hKey);
    if (dwRet != ERROR_SUCCESS) {
    	// Key not found - return
    	return;
    }

    // getting log_level
    dwData = sizeof(lpData);
    dwRet = RegQueryValueEx( hKey,
                             TEXT("log_level"),
                             NULL,
                             NULL,
                             (LPBYTE) lpData,
                             &dwData );
    if (dwRet == ERROR_SUCCESS) {
    	// log_level found
        // Read loglevels from registry and map on pteid middleware loglevels
    	// debug   -> LOGTYPE_TRACE
    	// info    -> LOGTYPE_INFO
    	// warning -> LOGTYPE_WARNING
    	// error   -> LOGTYPE_ERROR
        // none    -> LOGTYPE_NONE
    	if (lstrcmp((LPTSTR)lpData,TEXT("debug")))
    		g_uiLogLevel = LOGTYPE_TRACE;
    	if (lstrcmp((LPTSTR)lpData,TEXT("info")))
    		g_uiLogLevel = LOGTYPE_INFO;
    	if (lstrcmp((LPTSTR)lpData,TEXT("warning")))
    		g_uiLogLevel = LOGTYPE_WARNING;
    	if (lstrcmp((LPTSTR)lpData,TEXT("error")))
    		g_uiLogLevel = LOGTYPE_ERROR;
    	if (lstrcmp((LPTSTR)lpData,TEXT("none")))
    		g_uiLogLevel = LOGTYPE_NONE;
    }
    //getting log_dirname
    dwData = sizeof(lpData);
    dwRet = RegQueryValueEx( hKey,
                             TEXT("log_dirname"),
                             NULL,
                             NULL,
                             (LPBYTE) lpData,
                             &dwData );
    if (dwRet == ERROR_SUCCESS
    		&& dwData != 0) {
    	// log_dirname found
    	// we are not sure the string is null-terminated
    	lpData[dwData/sizeof(TCHAR) -  1] = '\0';
    	// put dirname in global var
    	lstrcpy(g_szLogFile, lpData);
    	// append file name
    	lstrcat(g_szLogFile, TEXT("\\pteidmdrv.log"));
    }

}

void LogTrace(int info, const char *pWhere, const char *format,... )
{
	char           buffer[2048];
	BYTE baseName[512];
	DWORD baseNamseSize; 

	time_t         timer;
	struct tm      *t;
	char           timebuf  [26];
	unsigned int   uiYear;

	va_list        listArg;
	int            iLog = 0;

	FILE           *fp = NULL;

	switch (g_uiLogLevel)
	{
	case LOGTYPE_ERROR:
		if ( info == LOGTYPE_ERROR )
		{
			iLog++;
		}
		break;

	case LOGTYPE_WARNING:
		if ( info == LOGTYPE_WARNING )
		{
			iLog++;
		}
		break;

	case LOGTYPE_INFO:
		if ( info == LOGTYPE_INFO )
		{
			iLog++;
		}
		break;

	case LOGTYPE_TRACE:
		iLog++;
		break;

	default:
		/* No Logging */
		break;
	}

	if ( iLog == 0 )
	{
		return;
	}

	if ( pWhere == NULL )
	{
		return;
	}

	/* get the name of the file that started this process*/
	baseNamseSize = GetModuleFileName(NULL,(LPTSTR)baseName,512);
	if (baseNamseSize == 0)
		lstrcpy(baseName,TEXT("Unknown name"));
	//baseNamseSize = GetModuleBaseName(GetCurrentProcess(),NULL,(LPTSTR)baseName,512);
	//baseNamseSize = GetProcessImageFileName(NULL,(LPTSTR)baseName,512);

	/* Gets time of day */
	timer = time(NULL);

	/* Converts date/time to a structure */
	memset(timebuf, '\0', sizeof(timebuf));
	t = localtime(&timer);
	if (t != NULL)
	{
		uiYear = t->tm_year;

		/* Add century to year */
		uiYear += 1900;

		/* Converts date/time to string */
		_snprintf(timebuf, sizeof(timebuf)
			, "%02d/%02d/%04d - %02d:%02d:%02d"
			, t->tm_mday
			, t->tm_mon + 1
			, uiYear
			, t->tm_hour
			, t->tm_min
			, t->tm_sec);
	}

	memset (buffer, '\0', sizeof(buffer));
	va_start(listArg, format);
	_vsnprintf(buffer, sizeof(buffer), format, listArg);
	va_end(listArg);

	fp = fopen(g_szLogFile, "a");
	if ( fp != NULL )
	{
		fprintf (fp, "%s %d %d %s|%30s|%s\n",baseName, GetCurrentProcessId(), GetCurrentThreadId(), timebuf, pWhere, buffer);
		fclose(fp);
	}
}

/****************************************************************************************************/

#define TT_HEXDUMP_LZ      16

void LogDump (int iStreamLg, unsigned char *pa_cStream)
{
	FILE           *fp = NULL;

	int            i        = 0;
	int            iOffset  = 0;
	unsigned char  *p       = pa_cStream;

	if ( pa_cStream == NULL )
	{
		return;
	}

	fp = fopen(g_szLogFile, "a");
	if ( fp == NULL )
	{
		return;
	}

	for ( i = 0 ; ((i < iStreamLg) && (p != NULL)) ; i++ )
	{
		if ( ( i % TT_HEXDUMP_LZ ) == 0 )
		{
			fprintf (fp, "\n");
			fprintf (fp, "%08X: ", i);
		}

		fprintf (fp, "%02X ", *p++);
	}
	fprintf (fp, "\n\n");

	fclose(fp);
}

/****************************************************************************************************/

void LogDumpBin (char *pa_cName, int iStreamLg, unsigned char *pa_cStream)
{
	FILE           *fp = NULL;

	if ( ( pa_cName   == NULL ) ||
		( pa_cStream == NULL ) )
	{
		return;
	}

	fp = fopen(pa_cName, "wb");
	if ( fp != NULL )
	{
		fwrite(pa_cStream, sizeof(char), iStreamLg, fp);
		fclose(fp);
	}
}

/****************************************************************************************************/
