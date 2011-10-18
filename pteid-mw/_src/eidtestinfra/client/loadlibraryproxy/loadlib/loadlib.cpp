/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2010 FedICT.
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
 * Override the LoadLibrary() with a version that will replace
 * the loading of "winscard.dll" by the loading of our PCSC
 * proxy ("C:\\Program Files\\Eid Test Infrastructure\\winscard.dll").
 *
 * This code is based on the Windows Detours Express 2.1 examples.
 */

#include <string.h>
#include <stdio.h>
#include <windows.h>

#include "detours.h"

static HMODULE (WINAPI * TrueLoadLibraryA)(const char *lpFileName) = LoadLibraryA;
static HMODULE (WINAPI * TrueLoadLibraryW)(const wchar_t *lpFileName) = LoadLibraryW;

HMODULE WINAPI MyLoadLibraryA(const char *lpFileName)
{
	const char *lpNewFileName = lpFileName;

	// The PCSC proxy will first try to call "REALPCSCLIB"
	if (lpFileName == NULL)
		lpNewFileName = NULL;
	else if (strcmp("REALPCSCLIB", lpFileName) == 0)
	{
		lpNewFileName = "C:\\WINDOWS\\system32\\winscard.dll";
		printf("==> proxy LoadLibrary(): PCSC proxy is loading the real PCSC lib\n");
	}
	else if (strstr(lpFileName, "winscard.dll") != NULL)
	{
		lpNewFileName = "C:\\Program Files\\Eid Test Infrastructure\\winscard.dll";
		printf("==> proxy LoadLibrary(): loading the proxy PCSC lib\n");
	}

	return TrueLoadLibraryA(lpNewFileName);
}

HMODULE WINAPI MyLoadLibraryW(const wchar_t *lpFileName)
{
	const wchar_t *lpNewFileName = lpFileName;

	// The PCSC proxy will first try to call "REALPCSCLIB"
	if (lpFileName == NULL)
		lpNewFileName = NULL;
	else if (wcscmp(L"REALPCSCLIB", lpFileName) == 0)
	{
		lpNewFileName = L"C:\\WINDOWS\\system32\\winscard.dll";
		printf("==> proxy LoadLibrary(): PCSC proxy is loading the real PCSC lib\n");
	}
	else if (wcsstr(lpFileName,L"winscard.dll") != NULL)
	{
		lpNewFileName = L"C:\\Program Files\\Eid Test Infrastructure\\winscard.dll";
		printf("==> proxy LoadLibrary(): loading the proxy PCSC lib\n");
	}

	return TrueLoadLibraryW(lpNewFileName);
}

BOOL WINAPI DllMain(HINSTANCE hinst, DWORD dwReason, LPVOID reserved)
{
    LONG error;
    (void)hinst;
    (void)reserved;

    if (dwReason == DLL_PROCESS_ATTACH) {
        printf("loadlib.dll: Starting.\n");
        fflush(stdout);

        DetourRestoreAfterWith();

        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        DetourAttach(&(PVOID&)TrueLoadLibraryA, MyLoadLibraryA);
        DetourAttach(&(PVOID&)TrueLoadLibraryW, MyLoadLibraryW);
        error = DetourTransactionCommit();

        if (error == NO_ERROR) {
            printf("loadlib.dll: Detoured LoadLibrary().\n");
        }
        else {
            printf("loadlib.dll: Error detouring LoadLibrary(): %d\n", error);
        }
    }
    else if (dwReason == DLL_PROCESS_DETACH) {
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        DetourDetach(&(PVOID&)TrueLoadLibraryA, MyLoadLibraryA);
        DetourDetach(&(PVOID&)TrueLoadLibraryW, MyLoadLibraryW);
        error = DetourTransactionCommit();

        printf("loadlib.dll: Removed LoadLibrary()\n");
        fflush(stdout);
    }
    return TRUE;
}
