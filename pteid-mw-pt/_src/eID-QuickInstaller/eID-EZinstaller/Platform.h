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

// platform compatibility

#ifndef __PLATFORM_H__
#define __PLATFORM_H__

//#define PLATF_MAC

#ifdef WIN32
#include <windows.h>
#include <io.h>
// SIS plugin
#define SISPATH "%osSystemFolder%\\siscardplugins"
#define SISLIB  "siscardplugin1_BE_EID_35__ACS_ACR38U__.dll"
#define SISRES  ":/sis/sisplugin"

#else
#include <stdint.h>
#include <errno.h>
#define sprintf_s snprintf
typedef unsigned char BYTE;
typedef uint32_t UINT32;
typedef uint32_t DWORD;
typedef bool BOOL;
#define Sleep(n) (sleep(n / 1000))
#define GetLastError() (errno)
#define STILL_ACTIVE 66
#endif

#endif
