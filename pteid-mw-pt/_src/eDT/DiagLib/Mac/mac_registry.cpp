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
#include "mac_helper.h"
#include <iostream>

#include "registry.h"
#include "error.h"
#include "log.h"
#include "folder.h"
#include "file.h"
#include "process.h"

#define G_BUFFER_SIZE 32767
static BYTE g_buffer[G_BUFFER_SIZE];

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PRIVATE FUNCTIONS DECLARATION ////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
/*HKEY getRootKey(Reg_Key_TYPE tyep);
int getValueType(Reg_Value_TYPE type);
int checkStringPattern(const wchar_t *wzPattern, const wchar_t *wzString) ;
int registryDeleteTree(HKEY hRootKey,const wchar_t *wzKey);
*/
////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PUBLIC FUNCTIONS /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
int registryExist(Reg_ID key, bool *exist)
{
	int iReturnCode = DIAGLIB_OK;

	return DIAGLIB_ERR_NOT_AVAILABLE;

/*	int err = ERROR_SUCCESS;

    HKEY hRegKey;
	HKEY hRootKey=getRootKey(key.Type);
	const wchar_t *wzKey=key.Key.c_str();
	const wchar_t *wzName=key.Name.c_str();

    *exist=false;

    if(ERROR_SUCCESS != (err = RegOpenKeyEx(hRootKey, wzKey, 0L, KEY_READ , &hRegKey)))
	{
        *exist=false;
    }
	else
	{
		if(key.Name.empty())
		{
			*exist=true;
		}
		else
		{
			DWORD dwType = REG_NONE;
			DWORD dwValDatLen = G_BUFFER_SIZE;
		    if(ERROR_SUCCESS != (err = RegQueryValueEx(hRegKey, wzName, 0L, &dwType, g_buffer, &dwValDatLen)))
			{
				if(ERROR_FILE_NOT_FOUND != err)
				{
 					LOG_ERRORCODE(L"RegQueryValueEx failed",err);
					iReturnCode = DIAGLIB_ERR_REGISTRY_READ_FAILED;
				}
				else
				{
					*exist=false;
				}
			}
			else
			{
				*exist=true;
			}
		}

		if(ERROR_SUCCESS != (err = RegCloseKey(hRegKey)))
		{
			LOG_ERRORCODE(L"RegCloseKey failed",err);
		}
	}

	return iReturnCode;
*/
}

////////////////////////////////////////////////////////////////////////////////////////////////
int registryGetInfo(Reg_ID key, Reg_INFO *info)
{
	int iReturnCode = DIAGLIB_OK;

	return DIAGLIB_ERR_NOT_AVAILABLE;

/*	int err = ERROR_SUCCESS;

    HKEY hRegKey;
	HKEY hRootKey=getRootKey(key.Type);
	const wchar_t *wzKey=key.Key.c_str();
	const wchar_t *wzName=key.Name.c_str();

	LOG_TIME(L"Ask for registry value (%ls\\%ls@%ls) --> ",hRootKey==HKEY_CURRENT_USER?L"HKCU":L"HKLM",wzKey,wzName);

	info->id=key;
	info->ValueType=STRING_VALUE_TYPE;
	info->ValueStr.clear();
	info->ValueInt=0;

	if(ERROR_SUCCESS != (err = RegOpenKeyEx(hRootKey, wzKey, 0L, KEY_READ , &hRegKey)))
	{
		LOG_ERRORCODE(L"RegOpenKeyEx failed",err);
		return DIAGLIB_ERR_REGISTRY_READ_FAILED;
    }

    //--- get the value
    DWORD dwType = REG_NONE;
	DWORD dwValDatLen = G_BUFFER_SIZE;
    if(ERROR_SUCCESS != (err = RegQueryValueEx(hRegKey, wzName, 0L, &dwType, g_buffer, &dwValDatLen)))
	{
 		LOG_ERRORCODE(L"RegQueryValueEx failed",err);
		iReturnCode = DIAGLIB_ERR_REGISTRY_READ_FAILED;
	}
	else
	{
		switch(dwType)
		{
		case REG_SZ:
		case REG_EXPAND_SZ:
			info->ValueStr.append((wchar_t*)g_buffer);
			LOG(L"DONE\n");
			break;
		case REG_DWORD:
			info->ValueType=INT_VALUE_TYPE;
			info->ValueInt=*(DWORD*)g_buffer;
			{
				wchar_t buf[16];
				if(-1==swprintf_s(buf,16,L"%ld",info->ValueInt))
				{
					LOG_ERROR(L"swprintf_s failed");
				}
				else
				{
					info->ValueStr.append(buf);
				}
			}
			LOG(L"DONE\n");
			break;
		default:
			LOG_ERROR(L"Unmanaged data type");
			iReturnCode = DIAGLIB_ERR_NOT_AVAILABLE;
		}
	}

	if(ERROR_SUCCESS != (err = RegCloseKey(hRegKey)))
	{
		LOG_ERRORCODE(L"RegCloseKey failed",err);
	}

	return iReturnCode;
*/
}
////////////////////////////////////////////////////////////////////////////////////////////////
int registrySetInfo(Reg_INFO info)
{
	int iReturnCode = DIAGLIB_OK;

	return DIAGLIB_ERR_NOT_AVAILABLE;

/*	int err = ERROR_SUCCESS;

    HKEY hRegKey;
	HKEY hRootKey=getRootKey(info.id.Type);
	const wchar_t *wzKey=info.id.Key.c_str();

	const wchar_t *wzName=info.id.Name.c_str();

	if(info.id.Name.empty())
		LOG_TIME(L"Set registry key (%ls\\%ls) --> ",hRootKey==HKEY_CURRENT_USER?L"HKCU":L"HKLM",wzKey);
	else
		LOG_TIME(L"Set registry value (%ls\\%ls@%ls) --> ",hRootKey==HKEY_CURRENT_USER?L"HKCU":L"HKLM",wzKey,wzName);

	if(ERROR_SUCCESS != (err = RegCreateKeyEx(hRootKey, wzKey, 0L, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hRegKey, NULL)))
	{
		LOG_ERRORCODE(L"RegCreateKeyEx failed",err);
		return DIAGLIB_ERR_REGISTRY_WRITE_FAILED;
	}

	//--- get the value
	DWORD dwValDatLen = G_BUFFER_SIZE;
	switch(getValueType(info.ValueType))
	{
	case REG_SZ:
		err = RegSetValueEx(hRegKey, wzName, NULL, REG_SZ, (const BYTE *)info.ValueStr.c_str(), ((DWORD)((info.ValueStr.length()+1)*sizeof(wchar_t))));
		break;
	case REG_DWORD:
		err = RegSetValueEx(hRegKey, wzName, NULL, REG_DWORD, (const BYTE *)&info.ValueInt, (DWORD)sizeof(info.ValueInt));
		break;
	default:
		err = ERROR_INVALID_FUNCTION;
		LOG_ERROR(L"Unmanaged data type");
		iReturnCode = DIAGLIB_ERR_NOT_AVAILABLE;
	}

	if(ERROR_SUCCESS != err)
	{
		LOG_ERRORCODE(L"RegSetValueEx failed", err);
		iReturnCode = DIAGLIB_ERR_REGISTRY_WRITE_FAILED;
	}
	else
	{
		LOG(L"DONE\n");
	}

	if(ERROR_SUCCESS != (err = RegCloseKey(hRegKey)))
	{
		LOG_ERRORCODE(L"RegCloseKey failed",err);
	}

	return iReturnCode;
*/
}

////////////////////////////////////////////////////////////////////////////////////////////////
int registryDelete(Reg_ID key)
{
	int iReturnCode = DIAGLIB_OK;

	return DIAGLIB_ERR_NOT_AVAILABLE;

/*	int err = ERROR_SUCCESS;

	HKEY hRootKey=getRootKey(key.Type);
	const wchar_t *wzKey=key.Key.c_str();

	if(key.Name.empty())
	{
		LOG_TIME(L"Ask for removing registry key (%ls\\%ls) --> ",hRootKey==HKEY_CURRENT_USER?L"HKCU":L"HKLM",wzKey);

		if(DIAGLIB_OK != (iReturnCode=registryDeleteTree(hRootKey,wzKey)))
		{
			LOG_ERRORCODE(L"registryDeleteTree failed",iReturnCode);
			return DIAGLIB_ERR_REGISTRY_DELETE_FAILED;
		}
		else
		{
			bool exist;
			if(DIAGLIB_OK != registryExist(key,&exist))
			{
				LOG_ERROR(L"registryExist failed");
				return DIAGLIB_ERR_REGISTRY_DELETE_FAILED;
			}
			else if(exist)
			{
				LOG_ERROR(L"Key not removed");
				return DIAGLIB_ERR_REGISTRY_DELETE_FAILED;
			}
			else
			{
				LOG(L"REMOVED\n");
			}
		}
	}
	else
	{
		HKEY hRegKey;
		const wchar_t *wzValue=key.Name.c_str();

		LOG_TIME(L"Ask for removing registry value (%ls\\%ls@%ls) --> ",hRootKey==HKEY_CURRENT_USER?L"HKCU":L"HKLM",wzKey,wzValue);
		if(ERROR_SUCCESS != (err = RegOpenKeyEx(hRootKey, wzKey, 0L, KEY_SET_VALUE | KEY_QUERY_VALUE, &hRegKey)))
		{
 			LOG_ERRORCODE(L"RegOpenKeyEx failed",err);
			return DIAGLIB_ERR_REGISTRY_DELETE_FAILED;
		}

		switch(err = RegQueryValueEx(hRegKey,wzValue,NULL,NULL,NULL,NULL))
		{
		case ERROR_SUCCESS:
			if(ERROR_SUCCESS != (err = RegDeleteValue(hRegKey,wzValue)))
			{
 				LOG_ERRORCODE(L"RegDeleteValue failed",err);
				iReturnCode = DIAGLIB_ERR_REGISTRY_DELETE_FAILED;
			}
			else
			{
				LOG(L"REMOVED\n");
			}
			break;
		case ERROR_FILE_NOT_FOUND:
			LOG(L"NOT FOUND\n");
			break;
		default:
 			LOG_ERRORCODE(L"RegQueryValueEx failed",err);
			iReturnCode = DIAGLIB_ERR_REGISTRY_DELETE_FAILED;
			break;
		}

		if(ERROR_SUCCESS != (err = RegCloseKey(hRegKey)))
		{
			LOG_ERRORCODE(L"RegCloseKey failed",err);
		}

	}
	return iReturnCode;
*/
}

////////////////////////////////////////////////////////////////////////////////////////////////
int registryGetSubKey(Reg_ID key, Reg_LIST *regList, const wchar_t *matchingPatern)
{
	int iReturnCode = DIAGLIB_OK;


	if(regList == NULL)
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;
	}

	return DIAGLIB_ERR_NOT_AVAILABLE;
/*
    int err = ERROR_SUCCESS;
	HKEY hRegKey;
	HKEY hRootKey=getRootKey(key.Type);
	const wchar_t *wzKey=key.Key.c_str();
	const wchar_t *wzName=key.Name.c_str();

	LOG_TIME(L"Ask for registry subkey (%ls\\%ls) --> ",hRootKey==HKEY_CURRENT_USER?L"HKCU":L"HKLM",wzKey);

	regList->clear();

	if(ERROR_SUCCESS != (err = RegOpenKeyEx(hRootKey, wzKey, 0L, KEY_READ , &hRegKey)))
	{
		LOG_ERRORCODE(L"RegOpenKeyEx failed",err);
		return DIAGLIB_ERR_REGISTRY_READ_FAILED;
    }

	DWORD nSubKeys;
    DWORD MaxSubKeyLen;

	if(ERROR_SUCCESS != (err = RegQueryInfoKey( hRegKey,		//__in         HKEY hKey,
												NULL,			//__out        LPTSTR lpClass,
												NULL,			//__inout_opt  LPDWORD lpcClass,
												NULL,			//__reserved   LPDWORD lpReserved,
												&nSubKeys,		//__out_opt    LPDWORD lpcSubKeys,
												&MaxSubKeyLen,	//__out_opt    LPDWORD lpcMaxSubKeyLen,
												NULL,			//__out_opt    LPDWORD lpcMaxClassLen,
												NULL,			//__out_opt    LPDWORD lpcValues,
												NULL,			//__out_opt    LPDWORD lpcMaxValueNameLen,
												NULL,			//__out_opt    LPDWORD lpcMaxValueLen,
												NULL,			//__out_opt    LPDWORD lpcbSecurityDescriptor,
												NULL			//__out_opt    PFILETIME lpftLastWriteTime
												)))
	{
		LOG_ERRORCODE(L"RegQueryInfoKey failed",err);
		iReturnCode = DIAGLIB_ERR_REGISTRY_READ_FAILED;
	}

	if(DIAGLIB_OK == iReturnCode)
	{
        DWORD SubKeyLen;
		wchar_t * const SubKeyBuff = (wchar_t*)g_buffer;
		Reg_ID id;

		for (unsigned int i = 0; i < nSubKeys; ++i)
		{
			SubKeyLen = G_BUFFER_SIZE/sizeof(wchar_t);
			if(ERROR_SUCCESS != (err = RegEnumKeyEx(	hRegKey,		//__in         HKEY hKey,
														i,				//__in         DWORD dwIndex,
														SubKeyBuff,		//__out        LPTSTR lpName,
														&SubKeyLen,		//__inout      LPDWORD lpcName,
														NULL,			//__reserved   LPDWORD lpReserved,
														NULL,			//__inout      LPTSTR lpClass,
														NULL,			//__inout_opt  LPDWORD lpcClass,
														NULL			//__out_opt    PFILETIME lpftLastWriteTime
														)))
			{
				if (ERROR_NO_MORE_ITEMS != err)
				{
					LOG_ERRORCODE(L"RegEnumKeyEx failed",err);
					iReturnCode = DIAGLIB_ERR_REGISTRY_READ_FAILED;
				}
				break;
			}

			if(matchingPatern == NULL || checkStringPattern(matchingPatern,SubKeyBuff))
			{
				id=key;
				id.Key.append(L"\\");
				id.Key.append(SubKeyBuff);
				regList->push_back(id);
			}

		}

	}

	if(ERROR_SUCCESS != (err = RegCloseKey(hRegKey)))
	{
		LOG_ERRORCODE(L"RegCloseKey failed",err);
	}

	return iReturnCode;
*/
}

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PRIVATE FUNCTIONS ////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
/*inline HKEY getRootKey(Reg_Key_TYPE type)
{
	if(type == SYSTEM_KEY_TYPE)
		return HKEY_LOCAL_MACHINE;
	else
		return HKEY_CURRENT_USER;
}

////////////////////////////////////////////////////////////////////////////////////////////////
inline int getValueType(Reg_Value_TYPE type)
{
	if(type == INT_VALUE_TYPE)
		return REG_DWORD;
	else
		return REG_SZ;
}

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
int registryDeleteTree(HKEY hRootKey,const wchar_t *wzKey)
{
	int iReturnCode = DIAGLIB_OK;

	File_ID fileTemp;

	//Get a temp file
 	if(DIAGLIB_OK != (iReturnCode=folderGetTempFileName(&fileTemp)))
	{
        return RETURN_LOG_INTERNAL_ERROR;
    }

	FILE *f=NULL;
	for(int i=0;i<3;i++)
	{
		_wfopen_s(&f,fileTemp.c_str(),L"w");

		if (f)
			break;
		else
			Sleep(50);
	}

	if (!f)
	{
		LOG_ERRORCODE(L"Failed to open the file",errno);
		return RETURN_LOG_INTERNAL_ERROR;
	}

	fwprintf(f,L"Windows Registry Editor Version 5.00\n\n");
	fwprintf(f,L"[-%ls\\%ls]",(hRootKey==HKEY_CURRENT_USER?L"HKEY_CURRENT_USER":L"HKEY_LOCAL_MACHINE"),wzKey);

	fclose(f);
	f=NULL;

	Proc_NAME CommandLine;
	CommandLine.clear();
	CommandLine.append(L"regedit.exe /s \"");
	CommandLine.append(fileTemp.c_str());
	CommandLine.append(L"\"");
	Proc_ID id;
	iReturnCode=processStart(CommandLine, &id);

	if(!fileTemp.empty())
	{
		fileDelete(fileTemp);
	}

	return iReturnCode;
}*/