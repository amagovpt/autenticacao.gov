/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2009 FedICT.
 * Copyright (C) 2019 Caixa Magica Software.
 * Copyright (C) 2011 Vasco Silva - <vasco.silva@caixamagica.pt>
 * Copyright (C) 2016, 2019 André Guerreiro - <aguerreiro1985@gmail.com>
 * Copyright (C) 2019 Miguel Figueira - <miguelblcfigueira@gmail.com>
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
#pragma once

#include "Export.h"

#include <stdio.h>
#include <stdarg.h>
#include <cstring>
#include <iostream>
#include <string>
#include <functional>

#ifdef WIN32
#include <windows.h>
#endif

namespace eIDMW
{

//--- string conversion between std::wstring and std::string
EIDMW_CMN_API std::wstring   utilStringWiden(const std::string& in, const std::locale& locale = std::locale());
EIDMW_CMN_API std::string    utilStringNarrow(const std::wstring& in, const std::locale& locale = std::locale());
EIDMW_CMN_API std::string	 IBM850_toUtf8( std::string const& inBuf );
#ifdef _WIN32
EIDMW_CMN_API std::wstring windowsANSIToWideString(const std::string& in);
#endif

/**
 * Case insensitve search, csSearch should be in lower case.
 * Returns true is csSearch is present in csData.
 */
EIDMW_CMN_API bool StartsWithCI(const char *csData, const char *csSearch);

/**
 * Returns true is csSearch is present in csData.
 */
EIDMW_CMN_API bool StartsWith(const char *csData, const char *csSearch);

EIDMW_CMN_API void SubstringInplace(char *buffer, size_t from, size_t to);

EIDMW_CMN_API char *bin2AsciiHex(const unsigned char * pData, unsigned long ulLen);

// Get the first n utf8 encoded chars in a string
EIDMW_CMN_API void truncateUtf8String(std::string &utf8String, size_t numberOfChars);

#ifndef WIN32
    std::u32string stringWidenUTF32(std::string utf8_str);
#endif

#ifdef WIN32
// Read/Write Registry value
EIDMW_CMN_API void ReadReg(HKEY hive, const wchar_t *subKey, const wchar_t *leafKey, DWORD *dwType, void* output, DWORD *outputSize);
EIDMW_CMN_API void WriteReg(HKEY hive, const wchar_t *subKey, const wchar_t *leafKey, DWORD dwType, void* input, DWORD inputSize);
#endif


/**
 * Scan the directory and call the call back function for each file corresponding to the extension
*/
EIDMW_CMN_API void scanDir(const char *Dir, const char *SubDir, const char *ext, bool &bStopRequest, void *param, std::function<void(const char* SubDir, const char* File, void* param)>);
}

#ifndef WIN32

#define _TRUNCATE -1

EIDMW_CMN_API int sprintf_s(char *buffer, size_t sizeOfBuffer, const char *format, ...);

EIDMW_CMN_API int strcat_s(char *dest, size_t len, const char *src);

EIDMW_CMN_API int strcpy_s(char *dest, size_t len, const char *src);

EIDMW_CMN_API int strncpy_s(char *dest, size_t len, const char *src, long count);

EIDMW_CMN_API int fopen_s(FILE** pFile, const char *filename, const char *mode);

EIDMW_CMN_API int wcscpy_s(wchar_t *dest, size_t len, const wchar_t *src);

EIDMW_CMN_API int fprintf_s(FILE *stream, const char *format, ...);

EIDMW_CMN_API int vfprintf_s(FILE *stream, const char *format, va_list argptr);

#endif
