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
#include <windows.h>
#include <iostream>
#include "log.h"

#define LOGFILE L"beidcleanup.log"

static wchar_t g_wzLogFile[MAX_PATH];

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PRIVATE FUNCTIONS ////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PUBLIC FUNCTIONS /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
const wchar_t *getLogName()
{
	const wchar_t *filename=LOGFILE;
	if(g_wzLogFile[0]!=0)
		filename=g_wzLogFile;

	return filename;
}

void InitLog(const wchar_t *wzLogFile)
{
	ZeroMemory(g_wzLogFile,sizeof(g_wzLogFile));

	if(wcslen(wzLogFile)<MAX_PATH)
	{
		swprintf_s(g_wzLogFile,MAX_PATH,L"%s",wzLogFile);
	}

	FILE *f=NULL;
	_wfopen_s(&f, getLogName(), L"w");
	if (f)
	{
		fclose(f);
		f=NULL;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////

void LOG(const wchar_t *format, ...)
{
	va_list args;
	va_start(args, format);

	FILE *f=NULL;
	_wfopen_s(&f, getLogName(), L"a");
	if (f)
	{
		vfwprintf_s(f, format, args);
		fclose(f);
		f=NULL;
	}
	else
	{
		vwprintf_s(format, args);
	}

	va_end(args);
}