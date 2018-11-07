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
#ifndef __WINMACROS__
#define __WINMACROS__


// taken from WinDef.h
#ifndef MAX_PATH
#define MAX_PATH 260
#endif

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif


// taken from WinDev.h
#undef pascal
#if (!defined(_MAC)) && ((_MSC_VER >= 800) || defined(_STDCALL_SUPPORTED))
#define pascal __stdcall
#else
#define pascal
#endif
#ifdef _MAC
#define CALLBACK    PASCAL
#ifdef _68K_
#define PASCAL      __pascal
#else
#define PASCAL
#endif
#elif (_MSC_VER >= 800) || defined(_STDCALL_SUPPORTED)
#define PASCAL      __stdcall
#else
#define PASCAL      pascal
#endif

#include "pcsclite.h"
//typedef unsigned long *PDWORD;

// taken from WinDef.h
#define FAR                
#define MAKEWORD(a, b)      ((WORD)(((BYTE)((DWORD_PTR)(a) & 0xff)) | ((WORD)((BYTE)((DWORD_PTR)(b) & 0xff))) << 8))
// derived from BaseTsd.h
typedef unsigned long int UINT_PTR, *PUINT_PTR;


// taken from WinSock.h
typedef UINT_PTR  SOCKET;

// taken from WinSock.h
#define INVALID_SOCKET  (SOCKET)(~0)

// taken from WinSCard.h
#define WINSCARDAPI
// taken from WinDef.h
#define WINAPI
  
// taken from SCardErr.h
#define SCARD_E_UNEXPECTED               ((DWORD)0x8010001FL)

#endif

