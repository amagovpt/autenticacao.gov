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
#pragma once

#include "Export.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <iostream>

namespace eIDMW
{

EIDMW_CMN_API void LogShitLevel(int x);

//--- string conversion between std::wstring and std::string
EIDMW_CMN_API std::wstring   utilStringWiden(const std::string& in, const std::locale& locale = std::locale());
EIDMW_CMN_API std::string    utilStringNarrow(const std::wstring& in, const std::locale& locale = std::locale());
EIDMW_CMN_API std::string	 IBM850_toUtf8( std::string const& inBuf );

/**
 * Case insensitve search, csSearch should be in lower case.
 * Returns true is csSearch is present in csData.
 */
EIDMW_CMN_API bool StartsWithCI(const char *csData, const char *csSearch);

/**
 * Returns true is csSearch is present in csData.
 */
EIDMW_CMN_API bool StartsWith(const char *csData, const char *csSearch);

int            Test(void);

EIDMW_CMN_API char *bin2AsciiHex(const unsigned char * pData, unsigned long ulLen);

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

