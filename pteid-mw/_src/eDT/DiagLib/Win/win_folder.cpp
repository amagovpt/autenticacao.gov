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
#include <windows.h>
#include <iostream>
#include <shlobj.h>

#include "folder.h"

#include "error.h"
#include "log.h"

#include "registry.h"

#define G_BUFFER_SIZE 32767
static TCHAR g_buffer[G_BUFFER_SIZE];

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PRIVATE FUNCTIONS DECLARATION ////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PUBLIC FUNCTIONS /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
int folderGetPath(const Folder_TYPE folder, Folder_ID *path, bool bInternalUse)
{
	//Warning no logging if intenal use

	int iReturnCode = DIAGLIB_OK;

	if(path==NULL)
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;	
	}

	DWORD size=0;
	DWORD  bufferSize = G_BUFFER_SIZE;

	switch(folder)
	{
	case FOLDER_WINDOWS:
		if(0 == (size=GetWindowsDirectory(g_buffer,bufferSize)))
		{
			if(!bInternalUse) LOG_LASTERROR(L"Windows folder not found");
			iReturnCode = DIAGLIB_ERR_INTERNAL;
		}
		else if(size>bufferSize-1)
		{
			if(!bInternalUse) LOG_ERROR(L"Buffer too small\n");
			iReturnCode = DIAGLIB_ERR_INTERNAL;
		}
		else if(0 != wcscat_s(g_buffer,bufferSize,L"\\")) //Ending by slash
		{			
			if(!bInternalUse) LOG_ERROR(L"Buffer too small\n");
			iReturnCode = DIAGLIB_ERR_INTERNAL;
		}

		break;
	case FOLDER_SYSTEM:
		if(0 == (size=GetSystemDirectory(g_buffer,bufferSize)))
		{
			if(!bInternalUse) LOG_LASTERROR(L"System folder not found");
			iReturnCode = DIAGLIB_ERR_INTERNAL;
		}
		else if(size>bufferSize-1)
		{
			if(!bInternalUse) LOG_ERROR(L"Buffer too small\n");
			iReturnCode = DIAGLIB_ERR_INTERNAL;
		}
		else if(0 != wcscat_s(g_buffer,bufferSize,L"\\")) //Ending by slash
		{			
			if(!bInternalUse) LOG_ERROR(L"Buffer too small\n");
			iReturnCode = DIAGLIB_ERR_INTERNAL;
		}

		break;
	case FOLDER_APP:
		if(!SHGetSpecialFolderPath(NULL,g_buffer,CSIDL_PROGRAM_FILES,FALSE))
		{
			if(!bInternalUse) LOG_LASTERROR(L"Program files folder not found");
			iReturnCode = DIAGLIB_ERR_INTERNAL;
		}
		else if(0 != wcscat_s(g_buffer,bufferSize,L"\\Belgium Identity Card\\")) //Ending by slash
		{			
			if(!bInternalUse) LOG_ERROR(L"Buffer too small\n");
			iReturnCode = DIAGLIB_ERR_INTERNAL;
		}
		break;
	case FOLDER_HOME:
		if(!SHGetSpecialFolderPath(NULL,g_buffer,CSIDL_MYDOCUMENTS,FALSE))
		{
			if(!bInternalUse) LOG_LASTERROR(L"My Documents folder not found");
			iReturnCode = DIAGLIB_ERR_INTERNAL;
		}
		else if(0 != wcscat_s(g_buffer,bufferSize,L"\\")) //Ending by slash
		{			
			if(!bInternalUse) LOG_ERROR(L"Buffer too small\n");
			iReturnCode = DIAGLIB_ERR_INTERNAL;
		}
		break;
	default:
		if(0 == (size=GetTempPath(bufferSize,g_buffer)))
		{
			if(!bInternalUse) LOG_LASTERROR(L"Temp folder not found");
			iReturnCode = DIAGLIB_ERR_INTERNAL;
		}
		else if(size>bufferSize)
		{
			if(!bInternalUse) LOG_ERROR(L"Buffer too small\n");
			iReturnCode = DIAGLIB_ERR_INTERNAL;
		}
		break;
	}

	if(iReturnCode == DIAGLIB_OK)
	{
		path->assign(g_buffer);
	}
	else
	{
		path->clear();
	}

	return iReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
int folderGetTempFileName(File_ID *file, bool bInternalUse)
{
	int iReturnCode = DIAGLIB_OK;

	if(file==NULL)
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;	
	}

	//Get the temp folder
	if(DIAGLIB_OK != (iReturnCode=folderGetPath(FOLDER_TEMP, file, bInternalUse)))
	{
        iReturnCode = DIAGLIB_ERR_INTERNAL;
    }
	else
	{
		if( MAX_PATH-14 < file->length() )
		{
			if(!bInternalUse) LOG_ERROR(L"Buffer too small\n");
			iReturnCode = DIAGLIB_ERR_INTERNAL;
		}
		else
		{
			TCHAR wzTempPath[MAX_PATH];
			TCHAR wzTempFile[MAX_PATH];
			if(-1==swprintf_s(wzTempPath,MAX_PATH,file->c_str()))
			{			
				if(!bInternalUse) LOG_ERROR(L"Buffer too small");
				iReturnCode = DIAGLIB_ERR_INTERNAL;
			}
			// Create a temporary file. 
			if (0 == ::GetTempFileName(wzTempPath,L"",0,wzTempFile))
			{
				if(!bInternalUse) LOG_LASTERROR(L"GetTempFileName failed");
				iReturnCode = DIAGLIB_ERR_INTERNAL;
			}

			if(iReturnCode == DIAGLIB_OK)
			{
				file->assign(wzTempFile);
			}
		}
	}

	if(iReturnCode != DIAGLIB_OK)
	{
		file->clear();
	}

	return iReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PRIVATE FUNCTIONS ////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
