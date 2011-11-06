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

#include "system.h"
#include "error.h"
#include "log.h"

#include "registry.h"
#include "process.h"
#include "embedded_rc.h"

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PRIVATE FUNCTIONS ////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


int RunUninstall(HKEY hKey,const wchar_t *wzKey,const wchar_t *defaultname,const wchar_t *guid, InstallType type, int issResource, long lTimeout, bool *pbRebootNeeded)
{
	int iReturnCode = RETURN_OK;

	LOG(L"Ask for uninstall \"%s\" (%s: GUID=%s)\n",hKey==HKEY_CURRENT_USER?L"per-user":L"per-machine",defaultname,guid);
	if((type==INSTALLTYPE_IS || type==INSTALLTYPE_IS_MSI) && issResource==0)
	{
		LOG(L" --> ERROR - Bad function call: ISS file is missing\n");
		return RETURN_ERR_INTERNAL;
	}

	bool bCleanTemp=false;
	TCHAR wzTempName[MAX_PATH];  

	if(!RegistryKeyExist(hKey,wzKey))
	{
		LOG(L" --> NOT INSTALLED\n");
		return iReturnCode;
	}

	TCHAR wzUninstall[1024]; 
	TCHAR wzKeyValueBuffer[1024]; 

	if(type==INSTALLTYPE_MSI)
	{
		wsprintf(wzUninstall,L"MsiExec.exe /X %s /quiet /norestart",guid);
	}
	else
	{

		ReadRegistryKey(hKey,wzKey,L"UninstallString",wzKeyValueBuffer);  
		if(wzKeyValueBuffer[0]==0)
		{
			LOG(L" --> WARNING - Uninstall string not found in %s\n",wzKey);
			RemoveRegistryKey(hKey,wzKey);
			return iReturnCode;
		}

		//Extract ISS file if needed
		if(issResource>0)
		{
			if(RETURN_OK!=(iReturnCode = ExtractResource(issResource,wzTempName)))
				return iReturnCode;

			bCleanTemp=true;
		}

		wsprintf(wzUninstall,L"%s /s /f1\"%s\"",wzKeyValueBuffer,wzTempName);
	}

	//Execute uninstall command
	iReturnCode = Execute(wzUninstall,lTimeout,pbRebootNeeded,hKey,wzKey);

	//if execution succeed, we check that the key has been remove, else we remove it
	if(iReturnCode == RETURN_OK)	
	{
		LOG(L" --> UNINSTALLED\n");
		LOG(L"\n");
		if(RegistryKeyExist(hKey,wzKey))
		{
			iReturnCode = RemoveRegistryKey(hKey,wzKey);
		}
	}
	else
	{
		LOG(L"\n");
	}

	//Cleanup ISS file if needed
	if(bCleanTemp)
	{
		CleanResource(wzTempName);
	}

	return iReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PUBLIC FUNCTIONS /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

bool isUserAdmin()
/*++ 
Routine Description: This routine returns TRUE if the caller's
process is a member of the Administrators local group. Caller is NOT
expected to be impersonating anyone and is expected to be able to
open its own process and process token. 
Arguments: None. 
Return Value: 
   TRUE - Caller has Administrators local group. 
   FALSE - Caller does not have Administrators local group. --
*/ 
{
	BOOL b;
	SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
	PSID AdministratorsGroup; 
	b = AllocateAndInitializeSid(
		&NtAuthority,
		2,
		SECURITY_BUILTIN_DOMAIN_RID,
		DOMAIN_ALIAS_RID_ADMINS,
		0, 0, 0, 0, 0, 0,
		&AdministratorsGroup); 
	if(b) 
	{
		if (!CheckTokenMembership( NULL, AdministratorsGroup, &b)) 
		{
			 b = FALSE;
		} 
		FreeSid(AdministratorsGroup); 
	}
	return (b?true:false);
} 

////////////////////////////////////////////////////////////////////////////////////////////////

int Uninstall(const wchar_t *defaultname,const wchar_t *guid, InstallType type, int issResource, long lTimeout, const wchar_t *keepguid, bool *pbRebootNeeded)
{
	int iReturnCode = RETURN_OK;

	if(wcscmp(guid,keepguid)==0)
	{
		LOG(L"Keeping \"%s\" (GUID=%s)\n",defaultname,guid);
		LOG(L"\n");
		return iReturnCode;
	}

	TCHAR wzKey[250];
	if(type==INSTALLTYPE_IS_MSI)
	{
		wsprintf(wzKey,L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\InstallShield_%s",guid);
	}
	else
	{
		wsprintf(wzKey,L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\%s",guid);
	}

	for(int iScope=0;iScope<2;iScope++)
	{
		//Get unsinstall string
		HKEY hKey;
		if(iScope == 0)
		{
			hKey=HKEY_CURRENT_USER;
		}
		else
		{
			hKey=HKEY_LOCAL_MACHINE;
		}

		if(RETURN_OK!=(iReturnCode = RunUninstall(hKey, wzKey, defaultname, guid, type, issResource, lTimeout, pbRebootNeeded)))
			return iReturnCode;
	}

	return iReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////

int SearchAndUninstall(const wchar_t *defaultname,const wchar_t *wzGuidPattern, long lTimeout, const wchar_t *keepguid, bool *pbRebootNeeded)
{
	int iReturnCode = RETURN_OK;

	if(wcscmp(wzGuidPattern,keepguid)==0)
	{
		LOG(L"Keeping \"%s\" (GUID=%s)\n",defaultname,wzGuidPattern);
		LOG(L"\n");
		return iReturnCode;
	}

	TCHAR wzKey[250];
	TCHAR wzGuid[50];

	for(int iScope=0;iScope<2;iScope++)
	{
		//Get unsinstall string
		HKEY hRootKey;
		if(iScope == 0)
		{
			hRootKey=HKEY_CURRENT_USER;
		}
		else
		{
			hRootKey=HKEY_LOCAL_MACHINE;
		}
		
		bool bFound=true;

		while(bFound)
		{
			bFound=false;

			if(RETURN_OK!=(iReturnCode = OpenSearchPattern(hRootKey,L"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall",wzGuidPattern)))
				return iReturnCode;

			while(NextSearchPattern(wzGuid,sizeof(wzGuid),&iReturnCode))
			{
				if(wcscmp(wzGuid,keepguid)==0)
				{
					LOG(L"Keeping \"%s\" (GUID=%s)\n",defaultname,wzGuid);
					LOG(L"\n");
					continue;
				}
				bFound=true;
				break;
			}

			CloseSearchPattern();

			if(iReturnCode != RETURN_OK)
				return iReturnCode;

			if(bFound)
			{
				wsprintf(wzKey,L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\%s",wzGuid);
				if(RETURN_OK!=(iReturnCode = RunUninstall(hRootKey, wzKey, defaultname, wzGuid, INSTALLTYPE_MSI, 0, lTimeout, pbRebootNeeded)))
					return iReturnCode;
			}
		}
	}

	return iReturnCode;
}