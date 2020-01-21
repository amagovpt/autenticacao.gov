/*
***************************************************************************
Multicert KSP - Logging functions
***************************************************************************
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <strsafe.h>

#include <Windows.h>
#include <KnownFolders.h>
#include <Shlobj.h>

#include "../Inc/log.h"
#include "../Inc/KSP.h"

/****************************************************************************************************/


#define MAX_LOG_DIR_NAME	4800
#define MAX_LOG_FILE_NAME	5000

//This default value can be overriden by the registry value log_dirname in key Software\PTEID\logging
char g_szLogFile[MAX_LOG_FILE_NAME] = "C:\\Program Files\\Portugal Identity Card\\log\\.PTEID_0.log";

#ifdef _DEBUG
unsigned int   g_uiLogLevel = LOGTYPE_TRACE;
#else
unsigned int   g_uiLogLevel = LOGTYPE_INFO;
#endif

/*
Destination pointer must point to a suitable buffer
*/
void WideStringToAnsi(wchar_t *source, char *destination, int destination_len) {

    int rc = WideCharToMultiByte(CP_ACP, 0, source, -1, destination, destination_len, NULL, NULL);

    if (rc == 0) {
        //This log is useless but in a debugger we'll see the call to GetLastError()
        fprintf(stderr, "WideCharToMultiByte failed with error code: %08x\n", GetLastError());
    }

}

void LogInit()
{
    BYTE pbDirnameCurrentUser[MAX_LOG_FILE_NAME];
    BYTE pbDirnameLocalMachine[MAX_LOG_FILE_NAME];
    INT iLogLevelCurrentUser = -1;
    INT iLogLevelLocalMachine = -1;
    GetRegLogLevelAndDir(HKEY_CURRENT_USER, &iLogLevelCurrentUser, pbDirnameCurrentUser, MAX_LOG_FILE_NAME);
    GetRegLogLevelAndDir(HKEY_LOCAL_MACHINE, &iLogLevelLocalMachine, pbDirnameLocalMachine, MAX_LOG_FILE_NAME);

    /* HKEY_CURRENT_USER overrides HKEY_LOCAL_MACHINE. HKEY_LOCAL_MACHINE overrides default.*/
    if (iLogLevelCurrentUser >= 0)
    {
        g_uiLogLevel = iLogLevelCurrentUser;
    }
    else if (iLogLevelLocalMachine >= 0)
    {
        g_uiLogLevel = iLogLevelLocalMachine;
    }

    if (lstrcmp((LPTSTR)pbDirnameCurrentUser, TEXT("")))
    {
        StringCchCopyA(g_szLogFile, MAX_LOG_FILE_NAME, pbDirnameCurrentUser);
    }
    else if (lstrcmp((LPTSTR)pbDirnameLocalMachine, TEXT("")))
    {
        StringCchCopyA(g_szLogFile, MAX_LOG_FILE_NAME, pbDirnameLocalMachine);
    }
    else 
    {
        //If the registry key is not defined write the logfile in TMP directory
        DWORD       dwRet;
        BYTE        lpData[MAX_LOG_DIR_NAME];

        dwRet = GetEnvironmentVariable("TMP", lpData, MAX_LOG_DIR_NAME);
        if (dwRet > 0 && dwRet <= MAX_LOG_DIR_NAME)
        {
            lstrcpy(g_szLogFile, lpData);
            lstrcat(g_szLogFile, TEXT("\\.PTEID_0.log"));
        }
    }
}

char * getLogLevelLabel(int logLevel) {

    switch (logLevel)
    {
    case LOGTYPE_ERROR:
        return "ERROR: ";
        break;
    case LOGTYPE_INFO:
        return "INFO: ";
        break;
    case LOGTYPE_WARNING:
        return "WARN: ";
        break;
    case LOGTYPE_TRACE:
        return "TRACE: ";
        break;
    default:
        return  "";
    }

}


void LogTrace(int info, const char *pWhere, const char *format, ...)
{
    char buffer[32768];
    BYTE baseName[512];
    DWORD baseNamseSize;

    time_t         timer;
    struct tm      *t;
    char           timebuf[26];
    unsigned int   uiYear;

    va_list        listArg;
    BOOL           bShouldLog = FALSE;

    FILE           *fp = NULL;

    if (info == LOGTYPE_NONE)
    {
        return;
    }
    switch (g_uiLogLevel)
    {
    case LOGTYPE_ERROR:
        if (info == LOGTYPE_ERROR)
        {
            bShouldLog = TRUE;
        }
        break;

    case LOGTYPE_WARNING:
        if (info <= LOGTYPE_WARNING)
        {
            bShouldLog = TRUE;
        }
        break;

    case LOGTYPE_INFO:
        if (info <= LOGTYPE_INFO)
        {
            bShouldLog = TRUE;
        }
        break;

    case LOGTYPE_TRACE:
        bShouldLog = TRUE;
        break;

    default:
        /* No Logging */
        break;
    }

    if (!bShouldLog)
    {
        return;
    }

    if (pWhere == NULL)
    {
        return;
    }

    /* get the name of the file that started this process*/
    baseNamseSize = GetModuleFileName(NULL, (LPTSTR)baseName, 512);
    if (baseNamseSize == 0)
        lstrcpy(baseName, TEXT("Unknown name"));
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

    memset(buffer, '\0', sizeof(buffer));
    va_start(listArg, format);
    _vsnprintf(buffer, sizeof(buffer), format, listArg);
    va_end(listArg);


    fp = fopen(g_szLogFile, "a");
    if (fp != NULL)
    {
        fprintf(fp, "%s %d %d %s| %s %30s|%s\n", baseName, GetCurrentProcessId(), GetCurrentThreadId(), timebuf, getLogLevelLabel(info), pWhere, buffer);
        fflush(fp);
        fclose(fp);
    }
}

/****************************************************************************************************/

#define TT_HEXDUMP_LZ      16

void LogDump(int iStreamLg, unsigned char *pa_cStream)
{
    FILE           *fp = NULL;

    int            i = 0;
    int            iOffset = 0;
    unsigned char  *p = pa_cStream;

    if (pa_cStream == NULL)
    {
        return;
    }

    fp = fopen(g_szLogFile, "a");
    if (fp == NULL)
    {
        return;
    }

    for (i = 0; ((i < iStreamLg) && (p != NULL)); i++)
    {
        if ((i % TT_HEXDUMP_LZ) == 0)
        {
            fprintf(fp, "\n");
            fprintf(fp, "%08X: ", i);
        }

        fprintf(fp, "%02X ", *p++);
    }
    fprintf(fp, "\n\n");

    fclose(fp);
}

/****************************************************************************************************/

void LogDumpBin(char *pa_cName, int iStreamLg, unsigned char *pa_cStream)
{
    FILE           *fp = NULL;

    if ((pa_cName == NULL) ||
        (pa_cStream == NULL))
    {
        return;
    }

    fp = fopen(pa_cName, "wb");
    if (fp != NULL)
    {
        fwrite(pa_cStream, sizeof(char), iStreamLg, fp);
        fclose(fp);
    }
}

/****************************************************************************************************/

void GetRegLogLevelAndDir(HKEY hKey, INT *dwLogLevel, LPBYTE pbLogDirname, DWORD cbLogDirname)
{
    DWORD       dwRet;
    BYTE        lpData[MAX_LOG_DIR_NAME];
    DWORD       dwData = 0;
    REGSAM flag = KEY_READ;

#if !_WIN64
    // For the 32 bit dll check if it is running under WOW64
    // If it is, we need KEY_WOW64_64KEY to access 64-bit registry view.
    BOOL isWOW64Process;
    IsWow64Process(GetCurrentProcess(), &isWOW64Process);
    if (isWOW64Process)
        flag |= KEY_WOW64_64KEY;
#endif

    dwRet = RegOpenKeyEx(hKey, TEXT("Software\\PTEID\\logging"), 0, flag, &hKey);

    //getting log_dirname
    dwData = sizeof(lpData);
    dwRet = RegQueryValueEx(hKey,
        TEXT("log_dirname"),
        NULL,
        NULL,
        (LPBYTE)lpData,
        &dwData);

    if (dwRet == ERROR_SUCCESS && dwData != 0) {
        // log_dirname found
        // we are not sure the string is null-terminated
        if (dwData == sizeof(lpData))
            dwData--;//replace last character with \0

        lpData[dwData] = '\0';
        // put dirname in global var
        StringCchCopyA(pbLogDirname, cbLogDirname, lpData);
        // append file name
        StringCchCatA(pbLogDirname, cbLogDirname, TEXT("\\.PTEID_0.log"));
    }
    else
    {
        StringCchCopyA(pbLogDirname, cbLogDirname, "");
    }

    // getting log_level
    dwData = sizeof(lpData);
    dwRet = RegQueryValueEx(hKey,
        TEXT("log_level"),
        NULL,
        NULL,
        (LPBYTE)lpData,
        &dwData);

    if (dwRet == ERROR_SUCCESS) {
        // log_level found
        // Read loglevels from registry and map on pteid middleware loglevels
        // debug   -> LOGTYPE_TRACE
        // info    -> LOGTYPE_INFO
        // warning -> LOGTYPE_WARNING
        // error   -> LOGTYPE_ERROR
        // none    -> LOGTYPE_NONE

        if (!lstrcmp((LPTSTR)lpData, TEXT("debug")))
            *dwLogLevel = LOGTYPE_TRACE;
        else if (!lstrcmp((LPTSTR)lpData, TEXT("info")))
            *dwLogLevel = LOGTYPE_INFO;
        else if (!lstrcmp((LPTSTR)lpData, TEXT("warning")))
            *dwLogLevel = LOGTYPE_WARNING;
        else if (!lstrcmp((LPTSTR)lpData, TEXT("error")))
            *dwLogLevel = LOGTYPE_ERROR;
        else if (!lstrcmp((LPTSTR)lpData, TEXT("none")))
            *dwLogLevel = LOGTYPE_NONE;
    }
    else
    {
        *dwLogLevel = -1;
    }
}