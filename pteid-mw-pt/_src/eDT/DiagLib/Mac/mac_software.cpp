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

#include "software.h"
#include "error.h"
#include "log.h"
#include "util.h"
#include "registry.h"
#include "progress.h"

#include <map>

struct Soft_ID_compare
{
	bool operator()(Soft_ID const &id1, Soft_ID const &id2) const
	{
		int cmp = wcscmp(id1.Guid.c_str(),id2.Guid.c_str());
		if(cmp == 0)
		{
			return (id1.Type < id2.Type);
		}
		else
		{
			return (cmp < 0);
		}
	}
};

typedef std::map<Soft_ID,Soft_INFO,Soft_ID_compare> Soft_CACHE;
static Soft_CACHE g_softwareCache;

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PRIVATE FUNCTIONS DECLARATION ////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
int softwareFillList(Soft_LIST *softList, const wchar_t *guid, const Soft_TYPE *type);
int softwareFillCacheFromDictionary(CFDictionaryRef inDictionary, const char * dataType, const Soft_GUID &currentGuid);
int softwareFillCache();

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
	LOG_TIME(L"Ask for info on Software '%ls' --> ",id.Guid.c_str());

	info->id = id;

	if(g_softwareCache.size() == 0)
	{
		if(DIAGLIB_OK != (iReturnCode = softwareFillCache()))
		{
			return RETURN_LOG_ERROR(iReturnCode);
		}
	}

	Soft_CACHE::const_iterator itr;
	itr = g_softwareCache.find(id);
	if(itr==g_softwareCache.end())
	{
		LOG(L"NOT FOUND\n");
		return DIAGLIB_ERR_SOFTWARE_NOT_FOUND;
	}
	else
	{
		*info = itr->second;
	}

	LOG(L"DONE\n");

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

	progressInit(2); //Main

	softList->clear();

	if(g_softwareCache.size() == 0)
	{
		if(DIAGLIB_OK != (iReturnCode = softwareFillCache()))
		{
			return RETURN_LOG_ERROR(iReturnCode);
		}
	}
	progressIncrement(); //Main

	progressInit(g_softwareCache.size()); //Loop

	Soft_ID id;
	Soft_INFO info;
	Soft_CACHE::const_iterator itr;
	for(itr=g_softwareCache.begin();itr!=g_softwareCache.end();itr++)
	{
		id = itr->first;
		info = itr->second;

		bool bAdd1=false;
		bool bAdd2=false;
		if(guid == NULL)
		{
			bAdd1=true;
		}
		else
		{
			if(0 == wcscmp(id.Guid.c_str(),guid))
			{
				bAdd1=true;
			}
		}
		if(type == NULL)
		{
			bAdd2=true;
		}
		else
		{
			if(id.Type == *type)
			{
				bAdd2=true;
			}
		}
		if(bAdd1 && bAdd2)
		{
			softList->push_back(id);
		}
		progressIncrement(); //Loop
	}
	progressRelease(); //Loop

	progressIncrement(); //Main
	progressRelease(); //Main

	return iReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
int softwareFillCache()
{
	int iReturnCode = DIAGLIB_OK;

	CFDictionaryRef	dataTypeDictionary = NULL;

	Soft_GUID guid;
	const char * dataType = "SPApplicationsDataType";
	//Get devices in a Dictionary
	if (DIAGLIB_OK != (iReturnCode = SystemProfiler(dataType, &dataTypeDictionary)))
	{
		LOG_LASTERROR(L"ProfileSystem failed");
		if (dataTypeDictionary != NULL) CFRelease(dataTypeDictionary);
		return RETURN_LOG_INTERNAL_ERROR;
	}

	g_softwareCache.clear();

	guid.assign(L"\\");
	guid.append(wstring_From_string(dataType));

	softwareFillCacheFromDictionary(dataTypeDictionary, dataType, guid);

	if (dataTypeDictionary != NULL) CFRelease(dataTypeDictionary);

	return iReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
int softwareFillCacheFromDictionary(CFDictionaryRef inDictionary, const char * dataType, const Soft_GUID &currentGuid)
{
	int iReturnCode = DIAGLIB_OK;

	CFArrayRef		itemsArray = NULL;
	CFIndex			arrayCount;

	//Extract usb bus in an array
	if(DIAGLIB_OK != (iReturnCode = SystemProfilerGetItemsArray(inDictionary, &itemsArray)) || itemsArray == NULL)
	{
		//End of recursive loop
		return DIAGLIB_OK;
	}

	arrayCount = CFArrayGetCount (itemsArray);
	Soft_ID id;
	Soft_ID searchId=id;

	progressInit(arrayCount);
	for (CFIndex i=0; i < arrayCount; i++)
	{
		CFDictionaryRef embeddedDictionary = (CFDictionaryRef)CFArrayGetValueAtIndex(itemsArray, i);

		CFStringRef nameValue=NULL;
		std::wstring wsName;
		if(CFDictionaryGetValueIfPresent(embeddedDictionary, CFSTR("_name"),(const void**)&nameValue))
		{
			wsName.assign(wstring_From_CFStringRef(nameValue));
			wstring_TrimR(&wsName);
			id.Guid.assign(currentGuid);
			id.Guid.append(L"\\");
			id.Guid.append(wsName);

			std::wstring Path;
			SystemProfilerGetInfoString(embeddedDictionary,"path", &Path);
			if(wcscmp(Path.substr(0,7).c_str(), L"/Users/") == 0)
				id.Type=PER_USER_SOFT_TYPE;
			else
				id.Type=PER_MACHINE_SOFT_TYPE;

			Soft_INFO info;
			wchar_t wzBuff[10];

			searchId.Guid.assign(id.Guid);
			searchId.Type = id.Type;

			Soft_CACHE::const_iterator itr;
			unsigned long count=0;
			do
			{
				itr = g_softwareCache.find(searchId);
				if(itr==g_softwareCache.end())
				{
					break;
				}

				count++;
				if( 0 > swprintf(wzBuff,10,L"%ld",count))
				{
					LOG_ERROR(L"sprintf failed\n");
					break;
				}

				searchId.Guid.assign(id.Guid);
				searchId.Guid.append(L"-");
				searchId.Guid.append(wzBuff);

			} while(1);

			//Fill info struct
			info.id = searchId;

			info.DisplayName.assign(wsName);
			SystemProfilerGetInfoString(embeddedDictionary,"version", &info.DisplayVersion);
			info.UninstallString.clear();
			info.Publisher.clear();
			info.InstallLocation.assign(Path);
			info.Version = 0;

			g_softwareCache[searchId] = info;
		}

		//Loop recursively through embedded dictionary
		softwareFillCacheFromDictionary(embeddedDictionary, dataType, searchId.Guid);

		progressIncrement();
	}
	progressRelease();

	if (itemsArray != NULL) CFRelease(itemsArray);

	return iReturnCode;
}

