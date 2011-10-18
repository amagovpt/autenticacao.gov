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
#include <stdio.h>

#include "file.h"
#include "error.h"
#include "log.h"

#include "registry.h"

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PRIVATE FUNCTIONS ////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (void*, BOOL*);
LPFN_ISWOW64PROCESS fnIsWow64Process;

BOOL IsWow64Proc()
{
    BOOL bIsWow64 = false;

    fnIsWow64Process = (LPFN_ISWOW64PROCESS) GetProcAddress(GetModuleHandle(TEXT("kernel32")),"IsWow64Process");  
	if(fnIsWow64Process != NULL)
        fnIsWow64Process(GetCurrentProcess(),&bIsWow64);

    return bIsWow64;
}

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PUBLIC FUNCTIONS /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
int GetFolderPath(FolderType folder,wchar_t *path,unsigned int pathsize)
{
	int iReturnCode = RETURN_OK;

	switch(folder)
	{
	case FOLDER_SYSTEM32:
		if(0==GetSystemDirectory(path,pathsize))
		{
			LOG(L" --> ERROR - GetFolderPath - System32 folder not found (LastError=%d)\n", GetLastError());
			iReturnCode = RETURN_ERR_INTERNAL;
		}
		break;
	case FOLDER_WOWSYS64:
#ifdef WIN64
		if(0==GetSystemWow64Directory(path,pathsize))
		{
			LOG(L" --> ERROR - GetFolderPath - SystemWow64 folder not found (LastError=%d)\n", GetLastError());
			iReturnCode = RETURN_ERR_INTERNAL;
		}
#else
		if(IsWow64Proc())
		{
			if(0==GetSystemWow64Directory(path,pathsize))
			{
				LOG(L" --> ERROR - GetFolderPath - SystemWow64 folder not found (LastError=%d)\n", GetLastError());
				iReturnCode = RETURN_ERR_INTERNAL;
			}
		}
		else
		{
			iReturnCode = RETURN_SKIP_FOLDER;
		}
#endif
		break;
	case FOLDER_APP:
		if(-1==swprintf_s(path,pathsize,L"C:\\Program Files\\Belgium Identity Card"))
		{			
			LOG(L" --> ERROR - GetFolderPath - Buffer too small\n");
			iReturnCode = RETURN_ERR_INTERNAL;
		}
		break;
	default:
		if(0==GetTempPath(pathsize,path))
		{
			LOG(L" --> ERROR - GetFolderPath - Temp folder not found (LastError=%d)\n", GetLastError());
			iReturnCode = RETURN_ERR_INTERNAL;
		}
		break;
	}

	return iReturnCode;
}

int GetTempFileName(wchar_t *path,unsigned int pathsize)
{
	int iReturnCode = RETURN_OK;

	//Get the temp folder
     TCHAR wzTempPath[MAX_PATH];
	if(RETURN_OK != (iReturnCode=GetFolderPath(FOLDER_TEMP, wzTempPath, MAX_PATH)))
	{
        iReturnCode = RETURN_ERR_INTERNAL;
    }
	else
	{
		if(pathsize<wcslen(wzTempPath)+14)
		{
			LOG(L" --> ERROR - GetTempFileName - Buffer too small\n");
			iReturnCode = RETURN_ERR_INTERNAL;
		}
		// Create a temporary file. 
		if (0 == ::GetTempFileName(wzTempPath,L"",0,path))
		{
			LOG(L" --> ERROR - GetTempFileName failed (LastError=%d)\n", GetLastError());
			iReturnCode = RETURN_ERR_INTERNAL;
		}
	}

	return iReturnCode;
}

int DeleteFile(FolderType folder, const wchar_t *file)
{
	int nReturnCode = RETURN_OK;

	LOG(L"Ask for deleting file \"%s\"\n",file);

    TCHAR wzPath[MAX_PATH];
	if(RETURN_OK == (nReturnCode=GetFolderPath(folder, wzPath,MAX_PATH)))
	{
		TCHAR wzFile[MAX_PATH];
		if(-1==swprintf_s(wzFile,MAX_PATH,L"%s\\%s",wzPath,file))
		{			
			LOG(L" --> ERROR - Buffer too small\n");
			nReturnCode = RETURN_ERR_INTERNAL;
		}
		else
		{
			if(::DeleteFile(wzFile))
			{
				LOG(L" --> DELETED\n");
			}
			else
			{
				DWORD err = GetLastError();
				if(err==ERROR_FILE_NOT_FOUND || err==ERROR_PATH_NOT_FOUND)
				{
					LOG(L" --> NOT FOUND\n");
				}
				else
				{
					LOG(L" --> ERROR - DeleteFile  (return=%ld)\n",err);
					nReturnCode = RETURN_ERR_DELETEFILE_FAILED;
				}
			}
		}
		LOG(L"\n");

		if(nReturnCode == RETURN_OK)
			nReturnCode = RemoveRegistryValue(HKEY_LOCAL_MACHINE,L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\SharedDlls",wzFile);
	}
	else
	{
		if (nReturnCode == RETURN_SKIP_FOLDER)
			nReturnCode = RETURN_OK;
		else
			LOG(L"\n");
	}

	return nReturnCode;
}