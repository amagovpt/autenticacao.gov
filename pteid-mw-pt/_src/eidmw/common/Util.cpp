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
#include <algorithm>
#include <fstream>
#include <functional>
#include <vector>
#include <iostream>
#include <iterator>
#include <locale>
#ifndef WIN32
#include <unistd.h>
#endif
#include <stdlib.h>

#ifdef WIN32
#include <windows.h>
#endif

#include "Util.h"

char a_cHexChars[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

namespace eIDMW
{

  std::wstring utilStringWiden(const std::string& in, const std::locale& locale)
  {
#ifndef WIN32
    std::wstring out(in.size(), 0);

    for(std::string::size_type i = 0; in.size() > i; ++i)
      out[i] = std::use_facet<std::ctype<wchar_t> >(locale).widen(in[i]);
    return out;
#else
	int required_size = MultiByteToWideChar(CP_UTF8, 0, in.c_str(), (int)in.size(),
      NULL, 0);

    if(required_size == 0)
		return std::wstring();

    std::vector<wchar_t> buf(++required_size);

    ::MultiByteToWideChar(CP_UTF8, 0,in.c_str(), (int)in.size(),
       &buf[0], required_size);

    return std::wstring(&buf[0]);
#endif

  }


  std::string utilStringNarrow(const std::wstring& in, const std::locale& locale)
  {
    std::string out(in.size(), 0);

    for(std::wstring::size_type i = 0; in.size() > i; ++i)
#ifdef WIN32
      out[i] = std::use_facet<std::ctype<wchar_t> >(locale).narrow(in[i]);
#else
      // in the unix implementation of std::locale narrow needs 2 arguments
      // (the second is a default char, here the choice is random)
      out[i] = std::use_facet<std::ctype<wchar_t> >(locale).narrow(in[i],'x');
#endif
    return out;
  }

/**
 * Case insensitve search, csSearch should be in lower case.
 * Returns true is csSearch is present in csData.
 */
bool StartsWithCI(const char *csData, const char *csSearch)
{
	for (const char *pc1 = csData, *pc2 = csSearch; *pc2 != '\0'; pc1++, pc2++)
	{
		if ((*pc1 != *pc2) && (*pc1 - 'A' + 'a' != *pc2))
			return false;
	}

	return true;
}

/**
 * Returns true is csSearch is present in csData.
 */
bool StartsWith(const char *csData, const char *csSearch)
{
	for (const char *pc1 = csData, *pc2 = csSearch; *pc2 != '\0'; pc1++, pc2++)
	{
		if (*pc1 != *pc2)
			return false;
	}

	return true;
}
//--------------------------------------------
// conversion table to convert IBM 850 to UTF8
// only the highest 128 characters can/will be converted
// The UTF8 version is for simplicity divided per 3 bytes.
//--------------------------------------------
static unsigned char IBM850_to_utf8[]=
{
	(unsigned char) 0xc3,(unsigned char) 0x87,(unsigned char) 0x00,		// (unsigned char) 0x80
	(unsigned char) 0xc3,(unsigned char) 0xbc,(unsigned char) 0x00,
	(unsigned char) 0xc3,(unsigned char) 0xa9,(unsigned char) 0x00,
	(unsigned char) 0xc3,(unsigned char) 0xa2,(unsigned char) 0x00,
	(unsigned char) 0xc3,(unsigned char) 0xa4,(unsigned char) 0x00,
	(unsigned char) 0xc3,(unsigned char) 0xa0,(unsigned char) 0x00,
	(unsigned char) 0xc3,(unsigned char) 0xa5,(unsigned char) 0x00,
	(unsigned char) 0xc3,(unsigned char) 0xa7,(unsigned char) 0x00,
	(unsigned char) 0xc3,(unsigned char) 0xaa,(unsigned char) 0x00,
	(unsigned char) 0xc3,(unsigned char) 0xab,(unsigned char) 0x00,
	(unsigned char) 0xc3,(unsigned char) 0xa8,(unsigned char) 0x00,
	(unsigned char) 0xc3,(unsigned char) 0xaf,(unsigned char) 0x00,
	(unsigned char) 0xc3,(unsigned char) 0xae,(unsigned char) 0x00,
	(unsigned char) 0xc3,(unsigned char) 0xac,(unsigned char) 0x00,
	(unsigned char) 0xc3,(unsigned char) 0x84,(unsigned char) 0x00,
	(unsigned char) 0xc3,(unsigned char) 0x85,(unsigned char) 0x00,
	(unsigned char) 0xc3,(unsigned char) 0x89,(unsigned char) 0x00,		// (unsigned char) 0x90
	(unsigned char) 0xc3,(unsigned char) 0xa6,(unsigned char) 0x00,
	(unsigned char) 0xc3,(unsigned char) 0x86,(unsigned char) 0x00,
	(unsigned char) 0xc3,(unsigned char) 0xb4,(unsigned char) 0x00,
	(unsigned char) 0xc3,(unsigned char) 0xb6,(unsigned char) 0x00,
	(unsigned char) 0xc3,(unsigned char) 0xb2,(unsigned char) 0x00,
	(unsigned char) 0xc3,(unsigned char) 0xbb,(unsigned char) 0x00,
	(unsigned char) 0xc3,(unsigned char) 0xb9,(unsigned char) 0x00,
	(unsigned char) 0xc3,(unsigned char) 0xbf,(unsigned char) 0x00,
	(unsigned char) 0xc3,(unsigned char) 0x96,(unsigned char) 0x00,
	(unsigned char) 0xc3,(unsigned char) 0x9c,(unsigned char) 0x00,
	(unsigned char) 0xc3,(unsigned char) 0xb8,(unsigned char) 0x00,
	(unsigned char) 0xc2,(unsigned char) 0xa3,(unsigned char) 0x00,
	(unsigned char) 0xc3,(unsigned char) 0x98,(unsigned char) 0x00,
	(unsigned char) 0xc3,(unsigned char) 0x97,(unsigned char) 0x00,
	(unsigned char) 0xc6,(unsigned char) 0x92,(unsigned char) 0x00,
	(unsigned char) 0xc3,(unsigned char) 0xa1,(unsigned char) 0x00,
	(unsigned char) 0xc3,(unsigned char) 0xad,(unsigned char) 0x00,
	(unsigned char) 0xc3,(unsigned char) 0xb3,(unsigned char) 0x00,
	(unsigned char) 0xc3,(unsigned char) 0xba,(unsigned char) 0x00,
	(unsigned char) 0xc3,(unsigned char) 0xb1,(unsigned char) 0x00,
	(unsigned char) 0xc3,(unsigned char) 0x91,(unsigned char) 0x00,
	(unsigned char) 0xc2,(unsigned char) 0xaa,(unsigned char) 0x00,
	(unsigned char) 0xc2,(unsigned char) 0xba,(unsigned char) 0x00,
	(unsigned char) 0xc2,(unsigned char) 0xbf,(unsigned char) 0x00,
	(unsigned char) 0xc2,(unsigned char) 0xae,(unsigned char) 0x00,
	(unsigned char) 0xc2,(unsigned char) 0xac,(unsigned char) 0x00,
	(unsigned char) 0xc2,(unsigned char) 0xbd,(unsigned char) 0x00,
	(unsigned char) 0xc2,(unsigned char) 0xbc,(unsigned char) 0x00,
	(unsigned char) 0xc2,(unsigned char) 0xa1,(unsigned char) 0x00,
	(unsigned char) 0xc2,(unsigned char) 0xab,(unsigned char) 0x00,
	(unsigned char) 0xc2,(unsigned char) 0xbb,(unsigned char) 0x00,
	(unsigned char) 0xc2,(unsigned char) 0xa6,(unsigned char) 0x00,
	(unsigned char) 0xc2,(unsigned char) 0xa6,(unsigned char) 0x00,
	(unsigned char) 0xc2,(unsigned char) 0xa6,(unsigned char) 0x00,
	(unsigned char) 0xc2,(unsigned char) 0xa6,(unsigned char) 0x00,
	(unsigned char) 0xc2,(unsigned char) 0xa6,(unsigned char) 0x00,
	(unsigned char) 0xc3,(unsigned char) 0x81,(unsigned char) 0x00,
	(unsigned char) 0xc3,(unsigned char) 0x82,(unsigned char) 0x00,
	(unsigned char) 0xc3,(unsigned char) 0x80,(unsigned char) 0x00,
	(unsigned char) 0xc2,(unsigned char) 0xa9,(unsigned char) 0x00,
	(unsigned char) 0xc2,(unsigned char) 0xa6,(unsigned char) 0x00,
	(unsigned char) 0xc2,(unsigned char) 0xa6,(unsigned char) 0x00,
	(unsigned char) 0x2b,(unsigned char) 0x00,(unsigned char) 0x00,
	(unsigned char) 0x2b,(unsigned char) 0x00,(unsigned char) 0x00,
	(unsigned char) 0xc2,(unsigned char) 0xa2,(unsigned char) 0x00,
	(unsigned char) 0xc2,(unsigned char) 0xa5,(unsigned char) 0x00,
	(unsigned char) 0x2b,(unsigned char) 0x00,(unsigned char) 0x00,
	(unsigned char) 0x2b,(unsigned char) 0x00,(unsigned char) 0x00,
	(unsigned char) 0x2d,(unsigned char) 0x00,(unsigned char) 0x00,
	(unsigned char) 0x2d,(unsigned char) 0x00,(unsigned char) 0x00,
	(unsigned char) 0x2b,(unsigned char) 0x00,(unsigned char) 0x00,
	(unsigned char) 0x2d,(unsigned char) 0x00,(unsigned char) 0x00,
	(unsigned char) 0x2b,(unsigned char) 0x00,(unsigned char) 0x00,
	(unsigned char) 0xc3,(unsigned char) 0xa3,(unsigned char) 0x00,
	(unsigned char) 0xc3,(unsigned char) 0x83,(unsigned char) 0x00,
	(unsigned char) 0x2b,(unsigned char) 0x00,(unsigned char) 0x00,
	(unsigned char) 0x2b,(unsigned char) 0x00,(unsigned char) 0x00,
	(unsigned char) 0x2d,(unsigned char) 0x00,(unsigned char) 0x00,
	(unsigned char) 0x2d,(unsigned char) 0x00,(unsigned char) 0x00,
	(unsigned char) 0xc2,(unsigned char) 0xa6,(unsigned char) 0x00,
	(unsigned char) 0x2d,(unsigned char) 0x00,(unsigned char) 0x00,
	(unsigned char) 0x2b,(unsigned char) 0x00,(unsigned char) 0x00,
	(unsigned char) 0xc2,(unsigned char) 0xa4,(unsigned char) 0x00,
	(unsigned char) 0xc3,(unsigned char) 0xb0,(unsigned char) 0x00,
	(unsigned char) 0xc3,(unsigned char) 0x90,(unsigned char) 0x00,
	(unsigned char) 0xc3,(unsigned char) 0x8a,(unsigned char) 0x00,
	(unsigned char) 0xc3,(unsigned char) 0x8b,(unsigned char) 0x00,
	(unsigned char) 0xc3,(unsigned char) 0x88,(unsigned char) 0x00,
	(unsigned char) 0x69,(unsigned char) 0x00,(unsigned char) 0x00,
	(unsigned char) 0xc3,(unsigned char) 0x8d,(unsigned char) 0x00,
	(unsigned char) 0xc3,(unsigned char) 0x8e,(unsigned char) 0x00,
	(unsigned char) 0xc3,(unsigned char) 0x8f,(unsigned char) 0x00,
	(unsigned char) 0x2b,(unsigned char) 0x00,(unsigned char) 0x00,
	(unsigned char) 0x2b,(unsigned char) 0x00,(unsigned char) 0x00,
	(unsigned char) 0xc2,(unsigned char) 0xa6,(unsigned char) 0x00,
	(unsigned char) 0x5f,(unsigned char) 0x00,(unsigned char) 0x00,
	(unsigned char) 0xc2,(unsigned char) 0xa6,(unsigned char) 0x00,
	(unsigned char) 0xc3,(unsigned char) 0x8c,(unsigned char) 0x00,
	(unsigned char) 0xc2,(unsigned char) 0xaf,(unsigned char) 0x00,
	(unsigned char) 0xc3,(unsigned char) 0x93,(unsigned char) 0x00,
	(unsigned char) 0xc3,(unsigned char) 0x9f,(unsigned char) 0x00,
	(unsigned char) 0xc3,(unsigned char) 0x94,(unsigned char) 0x00,
	(unsigned char) 0xc3,(unsigned char) 0x92,(unsigned char) 0x00,
	(unsigned char) 0xc3,(unsigned char) 0xb5,(unsigned char) 0x00,
	(unsigned char) 0xc3,(unsigned char) 0x95,(unsigned char) 0x00,
	(unsigned char) 0xc2,(unsigned char) 0xb5,(unsigned char) 0x00,
	(unsigned char) 0xc3,(unsigned char) 0xbe,(unsigned char) 0x00,
	(unsigned char) 0xc3,(unsigned char) 0x9e,(unsigned char) 0x00,
	(unsigned char) 0xc3,(unsigned char) 0x9a,(unsigned char) 0x00,
	(unsigned char) 0xc3,(unsigned char) 0x9b,(unsigned char) 0x00,
	(unsigned char) 0xc3,(unsigned char) 0x99,(unsigned char) 0x00,
	(unsigned char) 0xc3,(unsigned char) 0xbd,(unsigned char) 0x00,
	(unsigned char) 0xc3,(unsigned char) 0x9d,(unsigned char) 0x00,
	(unsigned char) 0xc2,(unsigned char) 0xaf,(unsigned char) 0x00,
	(unsigned char) 0xc2,(unsigned char) 0xb4,(unsigned char) 0x00,
	(unsigned char) 0xc2,(unsigned char) 0xad,(unsigned char) 0x00,
	(unsigned char) 0xc2,(unsigned char) 0xb1,(unsigned char) 0x00,
	(unsigned char) 0x3d,(unsigned char) 0x00,(unsigned char) 0x00,
	(unsigned char) 0xc2,(unsigned char) 0xbe,(unsigned char) 0x00,
	(unsigned char) 0xc2,(unsigned char) 0xb6,(unsigned char) 0x00,
	(unsigned char) 0xc2,(unsigned char) 0xa7,(unsigned char) 0x00,
	(unsigned char) 0xc3,(unsigned char) 0xb7,(unsigned char) 0x00,
	(unsigned char) 0xc2,(unsigned char) 0xb8,(unsigned char) 0x00,
	(unsigned char) 0xc2,(unsigned char) 0xb0,(unsigned char) 0x00,
	(unsigned char) 0xc2,(unsigned char) 0xa8,(unsigned char) 0x00,
	(unsigned char) 0xc2,(unsigned char) 0xb7,(unsigned char) 0x00,
	(unsigned char) 0xc2,(unsigned char) 0xb9,(unsigned char) 0x00,
	(unsigned char) 0xc2,(unsigned char) 0xb3,(unsigned char) 0x00,
	(unsigned char) 0xc2,(unsigned char) 0xb2,(unsigned char) 0x00,
	(unsigned char) 0xc2,(unsigned char) 0xa6,(unsigned char) 0x00,
};

//--------------------------------------------
// convert the IBM850 encoded inbuf to UTF8
//--------------------------------------------
std::string IBM850_toUtf8( std::string const& inBuf )
{
	std::string in_utf8;
	for (int x=0;x<(int) inBuf.length();x++)
	{
		int  idx = 0;
		char ch  = inBuf[x];

		//--------------------------------------------
		// only highest 128 characters should be converted
		//--------------------------------------------
		if ( (unsigned)ch > (unsigned)0x80)
		{
			ch      = ch - 0x80;
			idx		= ch*3;

			do 
			{
				in_utf8 += IBM850_to_utf8[idx];
				idx++;

			} while(IBM850_to_utf8[idx]!=0);

		}
		else
		{
			in_utf8 += ch;
		}
	}
	return in_utf8;
}

/* convert binary to ascii-hexadecimal, terminate with a 00-byte
   You have to free the returned buffer yourself !!
 */
char* bin2AsciiHex(const unsigned char * pData, unsigned long ulLen) 
{
    char *pszHex = new char[ulLen*2 + 1];
    if(pData != NULL)
    {
        int j = 0;
        for(unsigned long i = 0; i < ulLen; i++) 
        {
            pszHex[j++] = a_cHexChars[pData[i]>>4 & 0x0F];
            pszHex[j++] = a_cHexChars[pData[i] & 0x0F];
         }
         pszHex[j] = 0;
    }
    return pszHex;
}

}


#ifndef WIN32

#include "assert.h"

int sprintf_s(char *buffer, size_t sizeOfBuffer, const char *format, ...)
{
	va_list args;
	char *csTmp = NULL;
	int r = -1;

	va_start(args, format);
	r = vasprintf(&csTmp, format, args);
	va_end(args);

	if (r != -1 && csTmp != NULL)
	{
		r = strcpy_s(buffer, sizeOfBuffer, csTmp);
		free(csTmp);
	}

	return r;
}

int strcat_s(char *dest, size_t len, const char *src)
{
	if (dest == NULL)
		return -1;

	for ( ; *dest != '\0' && len > 1 ; dest++, len--)
		;

	for ( ; len > 1 && *src != '\0'; dest++, src++, len--)
		*dest = *src;

	*dest = '\0';

	return *src == '\0' ? 0 : -1; // 0: OK, -1: NOK
}

int strcpy_s(char *dest, size_t len, const char *src)
{
	if (dest == NULL)
		return -1;

	for ( ; len > 1 && *src != '\0'; dest++, src++, len--)
		*dest = *src;

	*dest = '\0';

	return *src == '\0' ? 0 : -1; // 0: OK, -1: NOK
}

int strncpy_s(char *dest, size_t len, const char *src, long count)
{

	if (dest == NULL)
		return -1;

	//On windows _TRUNCATE means that we could copy the maximum of character available
	if(count==_TRUNCATE) 	
	{
		for ( ; len > 1 && *src != '\0'; dest++, src++, len--)
			*dest = *src;

		*dest = '\0';

		return 0; //OK
	}
	else
	{
		char *dest_start = dest;
		size_t orig_len = len;

		for ( ; len > 1 && *src != '\0' && count > 0; dest++, src++, len--, count--)
			*dest = *src;

		*dest = '\0';

		if (*src == '\0' || count == 0)
			return 0; // OK

		if (orig_len > 0)
			*dest_start = '\0';
	}

	return -1;
}

int fopen_s(FILE** pFile, const char *filename, const char *mode)
{
	int r = 0;

	if (pFile == NULL)
		return -1;

	FILE *f = fopen(filename, mode);

	if (f != NULL)
		*pFile = f;
	else
		r = -1;

	return r;
}

int wcscpy_s(wchar_t *dest, size_t len, const wchar_t *src)
{
	if (dest == NULL)
		return -1;

	for ( ; len > 1 && *src != '\0'; dest++, src++, len--)
		*dest = *src;

	*dest = '\0';

	return *src == '\0' ? 0 : -1; // 0: OK, -1: NOK
}

EIDMW_CMN_API int fprintf_s(FILE *stream, const char *format, ...)
{
	va_list args;
	char *csTmp = NULL;
	int r = -1;

	va_start(args, format);
	r = vasprintf(&csTmp, format, args);
	va_end(args);

	if (r != -1 && csTmp != NULL)
	{
		r = fprintf(stream, "%s", csTmp);
		free(csTmp);
	}

	return r;
}


EIDMW_CMN_API int vfprintf_s(FILE *stream, const char *format, va_list argptr)
{
	char *csTmp = NULL;
	int r = -1;

	r = vasprintf(&csTmp, format, argptr);

	if (r != -1 && csTmp != NULL)
	{
		r = fprintf(stream, "%s", csTmp);
		free(csTmp);
	}

	return r;
}

#endif



