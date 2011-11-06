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
#ifdef WIN32
#include <windows.h>
#elif defined __APPLE__
#include "Mac/mac_helper.h"
#endif

#include <string.h>
#include <iostream>
#include <time.h>
#include <errno.h>

#include "diaglib.h"
#include "error.h"
#include "log.h"
#include "folder.h"

#include "svn_revision.h"
#include "pteidversions.h"

#define LOGFILE L"diaglib.log"

static std::wstring g_wsLogFile;
static FILE *g_pfile=NULL;
static bool g_logFirstOpen=true;
static bool g_logAvailable=true;

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PRIVATE FUNCTIONS DECLARATION ////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
int logLoadResources();
void getLocalTime(std::wstring &timestamp);
int logWrite(bool time, const wchar_t *format, va_list argList);

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PUBLIC FUNCTIONS /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
const char *diaglibVersion() 
{ 
	return PTEID_PRODUCT_VERSION "." SVN_REVISION_STR; 
}

////////////////////////////////////////////////////////////////////////////////////////////////
int logGetFileName(const wchar_t **file)
{
	if(file==NULL)
		return DIAGLIB_ERR_BAD_CALL;

	if(g_wsLogFile.empty()) logInit(NULL);

	*file=g_wsLogFile.c_str();

	return DIAGLIB_OK;
}

////////////////////////////////////////////////////////////////////////////////////////////////
int logInit(const wchar_t *wzLogFile)
{
	int iReturnCode = DIAGLIB_OK;

	//Initialize g_wsLogFile
	if(wzLogFile==NULL)
	{
		if(DIAGLIB_OK != folderGetPath(FOLDER_TEMP,&g_wsLogFile,true))
		{
			return RETURN_LOG_INTERNAL_ERROR;
		}
		g_wsLogFile.append(LOGFILE);	
	}
	else if(wcslen(wzLogFile)==0)
	{
		return DIAGLIB_ERR_BAD_CALL;
	}
	else
	{
		g_wsLogFile.assign(wzLogFile);
	}
						
	return iReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
int logLoadResources()
{
	int iReturnCode = DIAGLIB_OK;
		
	if(g_pfile)  return DIAGLIB_OK;
	
	if(g_wsLogFile.empty()) logInit(NULL);
	
	errno_t err = 0;
	
	//Create the file (Erase previous file)
	for(int i=0;i<LOG_OPEN_ATTEMPT_COUNT;i++)
	{
		err = _wfopen_s(&g_pfile, g_wsLogFile.c_str(), (g_logFirstOpen?L"w":L"a"));
		
		if (g_pfile)
		{
			break;
		}
		else if( err == ENOENT ) //Folder does not exist
		{
			g_logAvailable = false;
			return DIAGLIB_ERR_NOT_AVAILABLE;
		}
		else
		{
			Sleep(50);
		}
	}
	
	if(g_pfile && g_logFirstOpen)
	{
#ifdef WIN32
		fwprintf_s(g_pfile,L"RUNNING DIAGLIB VERSION: %hs\n",diaglibVersion());
#elif __APPLE__
		fwprintf_s(g_pfile,L"RUNNING DIAGLIB VERSION: %s\n",diaglibVersion());
#endif	
	}
	
	g_logFirstOpen = false;
	
	if (!g_pfile)
	{
		return DIAGLIB_ERR_FILE_CREATE_FAILED;
	}
	
	return iReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
int logUnloadResources()
{
	if(g_pfile)
	{
		fclose(g_pfile);
		g_pfile=NULL;
	}
	return DIAGLIB_OK;
}
////////////////////////////////////////////////////////////////////////////////////////////////
int logFinalize(void)
{
	return logUnloadResources();
}


////////////////////////////////////////////////////////////////////////////////////////////////
int LOG(const wchar_t *format, ...)
{
	int iReturnCode = DIAGLIB_OK;
	
	if(!g_logAvailable)
		return DIAGLIB_OK;

	va_list args;
	va_start(args, format);
	iReturnCode = logWrite(false,format, args);
	va_end(args);

	return iReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
int LOG_TIME(const wchar_t *format, ...)
{
	int iReturnCode = DIAGLIB_OK;
	
	if(!g_logAvailable)
		return DIAGLIB_OK;

	va_list args;
	va_start(args, format);
	iReturnCode = logWrite(true,format, args);
	va_end(args);

	return iReturnCode;
}
////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PRIVATE FUNCTIONS ////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
void getLocalTime(std::wstring &timestamp)
{
	time_t rawtime;
	struct tm timeinfo;
	wchar_t buffer [20];

	time ( &rawtime );
#ifdef WIN32
	localtime_s( &timeinfo, &rawtime );
#else
	timeinfo = *(localtime(&rawtime));
#endif

	wcsftime (buffer,20,L"%Y-%m-%d %H:%M:%S",&timeinfo);

	timestamp.assign(buffer);
}

////////////////////////////////////////////////////////////////////////////////////////////////
int logWrite(bool time, const wchar_t *format, va_list argList)
{
	int iReturnCode = DIAGLIB_OK;
	
	if(!g_logAvailable)
		return DIAGLIB_OK;
	
	if(format==NULL)
		return DIAGLIB_ERR_BAD_CALL;

	if(!g_pfile) 
	{
		if(DIAGLIB_OK != (iReturnCode = logLoadResources()))
		{
		   return iReturnCode;
		}
	}
		   
	if(!g_pfile) return DIAGLIB_ERR_INTERNAL;
	
	if(time)
	{
		std::wstring timestamp;
		getLocalTime(timestamp);
		fwprintf_s(g_pfile,L"%ls - ",timestamp.c_str());
	}
	vfwprintf_s(g_pfile, format, argList);
	
	return DIAGLIB_OK;
}
