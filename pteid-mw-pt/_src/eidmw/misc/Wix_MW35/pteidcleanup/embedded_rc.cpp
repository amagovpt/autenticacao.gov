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

#include "embedded_rc.h"
#include "error.h"
#include "log.h"

#include "file.h"

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PRIVATE FUNCTIONS ////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PUBLIC FUNCTIONS /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

int CleanResource(wchar_t *zTempName)
{
	if(DeleteFile(zTempName))
		return RETURN_OK; 

	LOG(L" --> ERROR - CleanResource failed in DeleteFile (LastError=%d)\n", GetLastError());
    return RETURN_ERR_INTERNAL;
}

////////////////////////////////////////////////////////////////////////////////////////////////

int ExtractResource(int resource,wchar_t *wzTempFile)
{
	FILE *f=NULL;
	wzTempFile[0]=0;

	int iReturnCode = RETURN_OK;

    // Create a temporary file. 
 	if(RETURN_OK != (iReturnCode=GetTempFileName(wzTempFile, MAX_PATH)))
	{
        iReturnCode = RETURN_ERR_INTERNAL;
		goto cleaning;
    }

	//Extract the resource in the temp file
    HRSRC hResource;
    if (NULL == (hResource= FindResource(NULL, MAKEINTRESOURCE(resource), TEXT("ISS"))))
    {
		LOG(L" --> ERROR - ExtractResource failed in FindResource (LastError=%d)\n", GetLastError());
		iReturnCode = RETURN_ERR_INTERNAL;
		goto cleaning;
	}

	//Load the resource
    HGLOBAL hResourceLoaded;
    if (NULL == (hResourceLoaded = LoadResource(NULL, hResource)))       
    {
		LOG(L" --> ERROR - ExtractResource failed in LoadResource (LastError=%d)\n", GetLastError());
		iReturnCode = RETURN_ERR_INTERNAL;
		goto cleaning;
	}

	//Get the resource size
	DWORD dwFileSize = SizeofResource(NULL, hResource);

	//Access the byte data
    LPBYTE lpBuffer;            
    if (NULL == (lpBuffer = (LPBYTE) LockResource(hResourceLoaded)))            
    {                
		LOG(L" --> ERROR - ExtractResource failed in LockResource (LastError=%d)\n", GetLastError());
		iReturnCode = RETURN_ERR_INTERNAL;
		goto cleaning;
    }

	//Write to the file
	errno_t err=_wfopen_s(&f,wzTempFile,L"wb");

	if(f==NULL || err!=0)
	{
		LOG(L" --> ERROR - ExtractResource failed while opening temp file for write (err=%d)\n", err);
		iReturnCode = RETURN_ERR_INTERNAL;
		goto cleaning;
	}
	
	if(dwFileSize != fwrite(lpBuffer,1,dwFileSize,f))
	{
		LOG(L" --> ERROR - ExtractResource failed while writing into temp file\n");
		iReturnCode = RETURN_ERR_INTERNAL;
		goto cleaning;
	}
	fclose(f);
	f=NULL;

	return iReturnCode;

cleaning:
	if(f) fclose(f);
	if(wzTempFile[0]!=0) CleanResource(wzTempFile);

	return iReturnCode;
}
