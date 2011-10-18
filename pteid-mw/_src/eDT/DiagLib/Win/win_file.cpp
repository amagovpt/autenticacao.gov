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

#include "file.h"

#include "error.h"
#include "log.h"

#include "registry.h"
#include "MD5Sum.h"
#include "util.h"
////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PRIVATE FUNCTIONS DECLARATION ////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PUBLIC FUNCTIONS /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
int fileGetInfo(const File_ID file, File_INFO *info)
{
	int iReturnCode = DIAGLIB_OK;

	if(info==NULL)
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;
	}

	info->id.assign(file);

	//Check if the file exist
	bool exist=false;
	if(DIAGLIB_OK != (iReturnCode=fileExists(file,&exist)))
	{
		return RETURN_LOG_ERROR(DIAGLIB_ERR_INTERNAL);
	}
	else
	{
		//If not exist return
		if(!exist)
		{
			return RETURN_LOG_ERROR(DIAGLIB_ERR_FILE_NOT_FOUND);
		}
	}

	//Get the size of the file
	struct _stat statbuff;
	if(0 != _wstat(file.c_str(),&statbuff))
	{
		return RETURN_LOG_ERROR(DIAGLIB_ERR_INTERNAL);
	}

	info->FileSize=statbuff.st_size;

	//{Reads version info from file}
	UINT Len;        // length of structs returned from API calls
	void *Ptr;       // points to version info structures
	DWORD dwSize;	// size of info buffer
	DWORD dwDummy = 0;     // stores 0 in call to GetFileVersionInfoSize
	VS_FIXEDFILEINFO infoFixed;
	wchar_t *infoBuffer=NULL;

	// Store zeros in fixed file info structure: this is used when no info
	::ZeroMemory(&infoFixed, sizeof(VS_FIXEDFILEINFO));

	// Record required size of version info buffer
	if (0 == (dwSize = GetFileVersionInfoSize(file.c_str(), &dwDummy))) 
	{
		LOG_LASTERROR(L"GetFileVersionInfoSize failed");
		iReturnCode = DIAGLIB_ERR_INTERNAL;
	}
	else
	{
		// Found info size OK
		// Ensure we have a sufficiently large buffer allocated
		wchar_t *infoBuffer = new wchar_t[dwSize];

		// Read file version info into storage and check success
		if (0 == GetFileVersionInfo(file.c_str(), dwDummy, dwSize, infoBuffer)) 
		{
			LOG_LASTERROR(L"GetFileVersionInfo failed");
			iReturnCode = DIAGLIB_ERR_INTERNAL;
		}
		else
		{
			// Get fixed file info & copy to own storage
			if(0 == VerQueryValue(infoBuffer, L"\\", &Ptr, &Len))
			{
				LOG_ERROR(L"VerQueryValue failed");
				iReturnCode = DIAGLIB_ERR_INTERNAL;
			}
			else
			{
				infoFixed = *(reinterpret_cast<VS_FIXEDFILEINFO*>(Ptr));
				//FileVersion
				wchar_t Buffer[25];
				if(-1==swprintf_s(Buffer,25,L"%ld.%ld.%ld.%ld",	infoFixed.dwFileVersionMS / 0x10000,
																infoFixed.dwFileVersionMS % 0x10000,
																infoFixed.dwFileVersionLS /0x10000,
																infoFixed.dwFileVersionLS % 0x10000))
				{			
					LOG_ERROR(L"Buffer too small");
					iReturnCode = DIAGLIB_ERR_INTERNAL;
				}
				else
				{
					info->FileVersion.assign(Buffer);
				}

				//ProductVersion
				if(-1==swprintf_s(Buffer,25,L"%ld.%ld.%ld.%ld",	infoFixed.dwProductVersionMS / 0x10000,
																infoFixed.dwProductVersionMS % 0x10000,
																infoFixed.dwProductVersionLS /0x10000,
																infoFixed.dwProductVersionLS % 0x10000))
				{			
					LOG_ERROR(L"Buffer too small");
					iReturnCode = DIAGLIB_ERR_INTERNAL;
				}
				else
				{
					info->ProductVersion.assign(Buffer);
				}
			}
		}
	}

	//MD5Sum sum;										this never gets called.. win_file is dead code.
	//	   sum.add_file(string_From_wstring(file));		// added md5sum directly in AnalyseMWFileInfo
	//	   sum.get_sum(info->md5sum);

	if(infoBuffer)
	{
		delete[] infoBuffer;
		infoBuffer=NULL;
	}

	return iReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
int fileDelete(const File_ID file)
{
	int iReturnCode = DIAGLIB_OK;

	bool exist;

	//Check if the file exist
	if(DIAGLIB_OK != (iReturnCode=fileExists(file,&exist)))
	{
		return RETURN_LOG_ERROR(DIAGLIB_ERR_INTERNAL);
	}
	else
	{
		//If not exist return
		if(!exist)
		{
			return RETURN_LOG_ERROR(DIAGLIB_ERR_FILE_NOT_FOUND);
		}
		else
		{
			if(!::DeleteFile(file.c_str()))
			{
				//If error occur
				DWORD err = GetLastError();
				if(err==ERROR_FILE_NOT_FOUND || err==ERROR_PATH_NOT_FOUND)
				{
					return RETURN_LOG_ERROR(DIAGLIB_ERR_FILE_NOT_FOUND);
				}
				else
				{
					LOG_LASTERROR(L"::DeleteFile failed");
					iReturnCode = DIAGLIB_ERR_FILE_DELETE_FAILED;
				}
			}
		}
	}

	return iReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PRIVATE FUNCTIONS ////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
