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
#ifndef __MAC_HELPER_H__
#define __MAC_HELPER_H__

#include <iostream>
#include <errno.h>
#include <CoreFoundation/CoreFoundation.h>
#include <PCSC/wintypes.h>
#include <PCSC/winscard.h>

#include <vector>

#define _stat stat

#define Sleep(millisecond) usleep(millisecond * 1000)

long GetLastError();
typedef int errno_t;

int _wfopen_s(FILE** pFile, const wchar_t *filename, const wchar_t *mode);
int fwprintf_s(FILE *stream, const wchar_t *format, ...);
int vfwprintf_s(FILE *stream, const wchar_t *format, va_list argptr);
int strcpy_s(char *dest, size_t len, const char *src);
int wcscpy_s(wchar_t *dest, size_t len, const wchar_t *src);
int _wstat(const wchar_t *filename, struct stat *buffer);
int swprintf_s(wchar_t *buffer, size_t sizeOfBuffer, const wchar_t *format, ...);

std::wstring wstring_From_CFStringRef(CFStringRef const& in);
std::string string_From_CFStringRef(CFStringRef const& in);

void unloadResources();

//SystemProfiler
int SystemProfilerGetDataType (std::vector<std::string> *dataTypeList);
int SystemProfiler(const char *dataType, CFDictionaryRef	*dataTypeDictionary);
int SystemProfilerGetInfoString(CFDictionaryRef inDictionary, const char *property, std::wstring *value);
int SystemProfilerGetItemsArray(CFDictionaryRef inDictionary, CFArrayRef *pItemsArray);

#define __stdcall
typedef void* HMODULE;
typedef unsigned char BYTE;
typedef const char* LPCTSTR;
#define SCARD_PROTOCOL_Tx (SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1)
#define SCARD_E_NO_READERS_AVAILABLE 0x8010002EL

HMODULE LoadLibrary(const wchar_t *library);
bool FreeLibrary(HMODULE hLibrary);
void *GetProcAddress(HMODULE hLibrary, const char * csFunctionName);

//Bundle path
const char *GetBundlePath();
const char *GetOtoolPath();

#endif //__MAC_HELPER_H__
