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

#include "software.h"
#include "error.h"
#include "log.h"
#include "registry.h"
#include "progress.h"

#define REG_UNINSTALL_KEY L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall"

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PRIVATE FUNCTIONS DECLARATION ////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
int softwareFillList(Soft_LIST *softList, const wchar_t *guid, const Soft_TYPE *type);
Reg_Key_TYPE getRegistryType(Soft_TYPE type);
Soft_TYPE getSoftwareType(Reg_Key_TYPE type);

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PUBLIC FUNCTIONS /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
int softwareGetInfo(Soft_ID id, Soft_INFO *info)
{
	int iReturnCode = DIAGLIB_OK;

	if(info == NULL)
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;
	}

	Reg_ID regKey;
	regKey.Key=REG_UNINSTALL_KEY;
	regKey.Key.append(L"\\");
	regKey.Key.append(id.Guid);
	regKey.Type=getRegistryType(id.Type);

	info->id=id;

	Reg_INFO regInfo;

	regKey.Name=L"DisplayName";
	info->DisplayName.clear();
	if(DIAGLIB_OK == registryGetInfo(regKey,&regInfo))
		info->DisplayName=regInfo.ValueStr;


	regKey.Name=L"DisplayVersion";
	info->DisplayVersion.clear();
	if(DIAGLIB_OK == registryGetInfo(regKey,&regInfo))	
		info->DisplayVersion=regInfo.ValueStr;

	regKey.Name=L"UninstallString";
	info->UninstallString.clear();
	if(DIAGLIB_OK == registryGetInfo(regKey,&regInfo))	
		info->UninstallString=regInfo.ValueStr;

	regKey.Name=L"Publisher";
	info->Publisher.clear();
	if(DIAGLIB_OK == registryGetInfo(regKey,&regInfo))
		info->Publisher=regInfo.ValueStr;

	regKey.Name=L"InstallLocation";
	info->InstallLocation.clear();
	if(DIAGLIB_OK == registryGetInfo(regKey,&regInfo))	
		info->InstallLocation=regInfo.ValueStr;

	regKey.Name=L"Version";
	info->Version=0;
	if(DIAGLIB_OK == registryGetInfo(regKey,&regInfo))	
		info->Version=regInfo.ValueInt;

	return iReturnCode;
} 

////////////////////////////////////////////////////////////////////////////////////////////////
int softwareUninstall (Soft_ID id)
{
	int iReturnCode = DIAGLIB_OK;

	return RETURN_LOG_ERROR(DIAGLIB_ERR_NOT_AVAILABLE);

	return iReturnCode;
} 


////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PRIVATE FUNCTIONS ////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
int softwareFillList(Soft_LIST *softList, const wchar_t *guid, const Soft_TYPE *type)
{
	int iReturnCode = DIAGLIB_OK;

	if(softList == NULL)
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;
	}

	softList->clear();

	Reg_ID UninstallKey;
	UninstallKey.Key=REG_UNINSTALL_KEY;

	Soft_ID id;

	Reg_LIST subkeyList;

	progressInit(4);	//First loop (2*2)

	for(int i=0;i<2;i++)
	{
		if(type) 
		{
			id.Type=*type;
		}
		else
		{
			if(i==0)
				id.Type=PER_USER_SOFT_TYPE;
			else
				id.Type=PER_MACHINE_SOFT_TYPE;
		}
		UninstallKey.Type=getRegistryType(id.Type);
			
		if( DIAGLIB_OK != (iReturnCode = registryGetSubKey(UninstallKey,&subkeyList,guid)))
		{
			if(iReturnCode != DIAGLIB_ERR_REGISTRY_NOT_FOUND)
			{
				LOG_ERROR(L"registryGetSubKey failed");
			}
			else
			{
				iReturnCode = DIAGLIB_OK;
			}

			progressIncrement();			//First loop 1
		}
		else
		{
			progressIncrement();			//First loop 1

			progressInit(subkeyList.size()); //Second loop

			Reg_LIST::const_iterator itr;
			for(itr=subkeyList.begin();itr!=subkeyList.end();itr++)
			{
				id.Guid=itr->Key.substr(wcslen(REG_UNINSTALL_KEY)+1);

				softList->push_back(id);
				progressIncrement();	//Second loop
			}
			progressRelease();			//Second loop
		}

		progressIncrement();		//First loop 2

		if(type) //if a special type is requested,
			break;
	}

	progressRelease();				//First loop

	return iReturnCode;

}

////////////////////////////////////////////////////////////////////////////////////////////////
Reg_Key_TYPE getRegistryType(Soft_TYPE type)
{
	if(type==PER_USER_SOFT_TYPE)
		return USER_KEY_TYPE;
	else
		return SYSTEM_KEY_TYPE;
}

////////////////////////////////////////////////////////////////////////////////////////////////
Soft_TYPE getSoftwareType(Reg_Key_TYPE type)
{
	if(type==USER_KEY_TYPE)
		return PER_USER_SOFT_TYPE;
	else
		return PER_MACHINE_SOFT_TYPE;
}