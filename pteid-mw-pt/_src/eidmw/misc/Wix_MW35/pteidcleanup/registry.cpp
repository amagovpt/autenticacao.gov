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

#include "registry.h"
#include "error.h"
#include "log.h"

#include "process.h"
#include "file.h"

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PRIVATE FUNCTIONS ////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

int checkStringPattern(const wchar_t *wzPattern, const wchar_t *wzString) 
{
	// Written by Jack Handy - jakkhandy@hotmail.com
	//Handle the wild character * ?
	const wchar_t *cp = NULL, *mp = NULL;

	while ((*wzString) && (*wzPattern != L'*')) 
	{
		if ((*wzPattern != *wzString) && (*wzPattern != L'?')) 
		{
			return 0;
		}
		wzPattern++;
		wzString++;
	}

	while (*wzString) 
	{
		if (*wzPattern == L'*') 
		{
			if (!*++wzPattern) 
			{
				return 1;
			}
			mp = wzPattern;
			cp = wzString+1;
		} 
		else if ((*wzPattern == *wzString) || (*wzPattern == L'?')) 
		{
			wzPattern++;
			wzString++;
		} 
		else 
		{
			wzPattern = mp;
			wzString = cp++;
		}
	}

	while (*wzPattern == L'*') 
	{
		wzPattern++;
	}

	return !*wzPattern;
}

////////////////////////////////////////////////////////////////////////////////////////////////

int MyRegDeleteTree(HKEY hRootKey,const wchar_t *wzKey) 
{ 
	int iReturnCode = RETURN_OK;

    TCHAR wzTempFile[MAX_PATH];
	wzTempFile[0]=0;
    TCHAR wzCommandLine[MAX_PATH];
	FILE *f=NULL;

	//Get the temp folder
 	if(RETURN_OK != (iReturnCode=GetTempFileName(wzTempFile, MAX_PATH)))
	{
        iReturnCode = RETURN_ERR_INTERNAL;
		goto cleaning;
    }

	errno_t err=_wfopen_s(&f,wzTempFile,L"w");

	if(f==NULL || err!=0)
	{
		LOG(L" --> ERROR - MyRegDeleteTree failed while opening temp file for write (err=%d)\n", err);
		iReturnCode = RETURN_ERR_INTERNAL;
		goto cleaning;
	}

	fwprintf(f,L"Windows Registry Editor Version 5.00\n\n");
	fwprintf(f,L"[-%s\\%s]",(hRootKey==HKEY_CURRENT_USER?L"HKEY_CURRENT_USER":L"HKEY_LOCAL_MACHINE"),wzKey);

	fclose(f);
	f=NULL;

	bool bRebootNeeded;//not used here
	swprintf_s(wzCommandLine,sizeof(wzCommandLine)/sizeof(wchar_t),L"regedit.exe /s \"%s\"",wzTempFile);
	iReturnCode=Execute(wzCommandLine, 20, &bRebootNeeded);

cleaning:
	if(f) fclose(f);
	if(wzTempFile[0]!=0) DeleteFile(wzTempFile);

	return iReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PUBLIC FUNCTIONS /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
bool RegistryKeyExist(HKEY hRootKey,const wchar_t *wzKey)
{
    HKEY hRegKey;

    long lRes = RegOpenKeyEx(hRootKey, wzKey, 0L, KEY_READ , &hRegKey);
    if (lRes != ERROR_SUCCESS)
	{
        return false;
    }

    RegCloseKey(hRegKey);

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////

int RemoveRegistryKey(HKEY hRootKey,const wchar_t *wzKey)
{
	int nReturnCode = RETURN_OK;

	LOG(L"Ask for removing registry key (%s\\%s)\n",hRootKey==HKEY_CURRENT_USER?L"HKCU":L"HKLM",wzKey);

	LONG lErr = ERROR_SUCCESS; 
	if(ERROR_SUCCESS != (lErr=MyRegDeleteTree(hRootKey,wzKey)))
	{
		LOG(L" --> ERROR - MyRegDeleteTree  (return=%ld)\n",lErr);
		nReturnCode = RETURN_ERR_DELETEREG_FAILED;
	}
	else
	{
		if(RegistryKeyExist(hRootKey,wzKey))
		{
			LOG(L" --> ERROR - Key not removed\n");
			nReturnCode = RETURN_ERR_DELETEREG_FAILED;
		}
		else
		{
			LOG(L" --> REMOVED\n");
		}
	}

	LOG(L"\n");
	return nReturnCode;
}
////////////////////////////////////////////////////////////////////////////////////////////////

int RemoveRegistryValue(HKEY hRootKey,const wchar_t *wzKey,const wchar_t *wzValue)
{
	int nReturnCode = RETURN_OK;
	HKEY hRegKey;

	LOG(L"Ask for removing registry value (%s\\%s@%s)\n",hRootKey==HKEY_CURRENT_USER?L"HKCU":L"HKLM",wzKey,wzValue);
    long lRes = RegOpenKeyEx(hRootKey, wzKey, 0L, KEY_SET_VALUE | KEY_QUERY_VALUE, &hRegKey);
    if (lRes != ERROR_SUCCESS)
	{
 		LOG(L" --> ERROR - Could not open registry key (%s\\%s)\n",hRootKey==HKEY_CURRENT_USER?L"HKCU":L"HKLM",wzKey);;
		return RETURN_ERR_DELETEREG_FAILED;
    }

	int iStatus = RegQueryValueEx(hRegKey,wzValue,NULL,NULL,NULL,NULL);
	int iStatus2 = ERROR_SUCCESS;
	switch(iStatus)
	{
	case ERROR_SUCCESS:
		iStatus2 = RegDeleteValue(hRegKey,wzValue);
		if(iStatus2 == ERROR_SUCCESS)
		{
			LOG(L" --> REMOVED\n");
		}
		else
		{
 			LOG(L" --> ERROR - Could not delete registry value (Error=%ld)\n",iStatus2);
			nReturnCode = RETURN_ERR_DELETEREG_FAILED;
		}
		break;
	case ERROR_FILE_NOT_FOUND:
		LOG(L" --> NOT FOUND\n");
		break;
	default:
 		LOG(L" --> ERROR - Unknown error with RegQueryValueEx (Error=%ld)\n",iStatus);
		nReturnCode = RETURN_ERR_DELETEREG_FAILED;
		break;
	}

    RegCloseKey(hRegKey);

	LOG(L"\n");
	return nReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////

bool ReadRegistryKey(HKEY hRootKey,const wchar_t *wzKey,const wchar_t *wzName,wchar_t *valueBuffer)
{
    HKEY hRegKey;

	valueBuffer[0]=0;

	long lRes = RegOpenKeyEx(hRootKey, wzKey, 0L, KEY_READ , &hRegKey);
    if (lRes != ERROR_SUCCESS){
		LOG(L" --> ERROR - ReadRegistryKey - Could not open registry key (%s\\%s)\n",hRootKey==HKEY_CURRENT_USER?L"HKCU":L"HKLM",wzKey);
		return false;
    }

    //--- get the value
    DWORD dwType = REG_SZ;
    BYTE  abValueDat[512];
    DWORD dwValDatLen = sizeof(abValueDat); 
    lRes        = RegQueryValueEx(hRegKey, wzName, 0L, &dwType, abValueDat, &dwValDatLen);
    if (lRes != ERROR_SUCCESS)
	{     
 		LOG(L" --> ERROR - ReadRegistryKey - Could not read registry value (%s)\n",wzName);
		return false;
	}

    RegCloseKey(hRegKey);
	
	wsprintf(valueBuffer,L"%s",(wchar_t*)abValueDat);
	
	return true;
}
////////////////////////////////////////////////////////////////////////////////////////////////
static HKEY g_hSearchHandle = 0;
static wchar_t g_wzSearchPattern[50];
static int g_iSearchIndex;

int OpenSearchPattern(HKEY hRootKey,const wchar_t *wzKey,const wchar_t *wzPattern)
{
	int nReturnCode = RETURN_OK;

	if(g_hSearchHandle != 0)
	{
		LOG(L" --> ERROR - OpenSearchPattern - Bad function call: SearchHandle already exists\n");
		return RETURN_ERR_INTERNAL;
	}

	LONG errcode = RegOpenKeyEx( hRootKey, wzKey, 0, KEY_READ,&g_hSearchHandle);
	if(ERROR_FILE_NOT_FOUND == errcode)
	{
		g_hSearchHandle = 0;
		return RETURN_OK;
	}

	if(ERROR_SUCCESS != errcode)
	{
		LOG(L" --> ERROR - OpenSearchPattern - Could not open registry key (%s\\%s) - errocode=%ld\n",hRootKey==HKEY_CURRENT_USER?L"HKCU":L"HKLM",wzKey,errcode);
		g_hSearchHandle = 0;
		return RETURN_ERR_INTERNAL;
	}

	if(-1==swprintf_s(g_wzSearchPattern,sizeof(g_wzSearchPattern)/sizeof(wchar_t),L"%s",wzPattern))
	{
		LOG(L" --> ERROR - OpenSearchPattern - Bad function call: Buffer too small\n");
		CloseSearchPattern();
		return RETURN_ERR_INTERNAL;
	}

	g_iSearchIndex=0;

	return nReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////

bool NextSearchPattern(wchar_t *wzKey, int iKeyLen, int *nReturnCode)
{
	*nReturnCode = RETURN_OK;

	if(g_hSearchHandle == 0)
	{
		*nReturnCode = RETURN_OK;
		return false;
	}

	wchar_t wzKeyTemp[255];
	DWORD iKeyTempLen;

	while(1)
	{	
		iKeyTempLen=sizeof(wzKeyTemp)/sizeof(wchar_t);
		ZeroMemory(wzKeyTemp, sizeof(wzKeyTemp));
		int iStatus = RegEnumKeyEx(g_hSearchHandle, g_iSearchIndex++, wzKeyTemp, &iKeyTempLen, NULL, NULL, NULL, NULL);
		switch(iStatus)
		{
		case ERROR_SUCCESS: 
			if(checkStringPattern(g_wzSearchPattern,wzKeyTemp))
			{
				if(-1==swprintf_s(wzKey,iKeyLen,L"%s",wzKeyTemp))
				{
					LOG(L" --> ERROR - Bad function call: Buffer too small\n");
					*nReturnCode = RETURN_ERR_INTERNAL;
					return false;
				}
				return true;
			}
			break;
		case ERROR_NO_MORE_ITEMS: 
			return false;
		case ERROR_MORE_DATA: 
			LOG(L" --> ERROR - Buffer too small in RegEnumKeyEx\n");
			*nReturnCode = RETURN_ERR_INTERNAL;
			return false;
		default:
			LOG(L" --> ERROR - Unknown error(%ld) in RegEnumKeyEx\n",iStatus);
			*nReturnCode = RETURN_ERR_INTERNAL;
			return false;
		}
	}

	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////
void CloseSearchPattern()
{
	if(g_hSearchHandle != 0)
	{
		RegCloseKey(g_hSearchHandle);

		g_hSearchHandle=0;
		ZeroMemory(g_wzSearchPattern, sizeof(g_wzSearchPattern));
		g_iSearchIndex=0;
	}
}

