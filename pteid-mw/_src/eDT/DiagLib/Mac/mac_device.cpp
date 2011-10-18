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
#include <CoreFoundation/CoreFoundation.h>

#include "device.h"
#include "error.h"
#include "log.h"
#include "util.h"
#include "progress.h"

#include <map>

typedef std::map<Device_ID,Device_INFO> Device_CACHE;
static Device_CACHE g_deviceCache;

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PRIVATE FUNCTIONS DECLARATION ////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
int deviceFillList(Device_LIST *deviceList, const wchar_t *deviceDescription, const wchar_t *deviceClass);
int deviceFillCache();
int deviceFillCacheFromDictionary(CFDictionaryRef inDictionary, const char * dataType, const Device_ID &currentId);
const wchar_t *UsbDeviceGetClassType(std::wstring const &VendorId, std::wstring const &ProductId);

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PUBLIC FUNCTIONS /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
int deviceGetInfo(Device_ID device, Device_INFO *info)
{
	int iReturnCode = DIAGLIB_OK;

	if(info==NULL)
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;
	}

	LOG_TIME(L"Ask for info on device '%ls' --> ",device.c_str());

	info->id = device;

	if(g_deviceCache.size() == 0)
	{
		if(DIAGLIB_OK != (iReturnCode = deviceFillCache()))
		{
			return RETURN_LOG_ERROR(iReturnCode);
		}
	}

	Device_CACHE::const_iterator itr;
	itr = g_deviceCache.find(device);
	if(itr==g_deviceCache.end())
	{
		LOG(L"NOT FOUND\n");
		return DIAGLIB_ERR_DEVICE_NOT_FOUND;
	}
	else
	{
		*info = itr->second;
	}

	LOG(L"DONE\n");

	return iReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PRIVATE FUNCTIONS ////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
int deviceFillList(Device_LIST *deviceList, const wchar_t *deviceName, const wchar_t *deviceClass)
{
	int iReturnCode = DIAGLIB_OK;

	if(deviceList == NULL)
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;
	}

	if(deviceClass != NULL) //Not available on Mac because the ClassType must be extract from vendorId and productId
	{
		return DIAGLIB_ERR_NOT_AVAILABLE;
	}

	progressInit(2); //Main

	deviceList->clear();

	if(g_deviceCache.size() == 0)
	{
		if(DIAGLIB_OK != (iReturnCode = deviceFillCache()))
		{
			return RETURN_LOG_ERROR(iReturnCode);
		}
	}

	progressIncrement(); //Main

	progressInit(g_deviceCache.size()); //Loop

	Device_ID id;
	Device_INFO info;
	Device_CACHE::const_iterator itr;
	for(itr=g_deviceCache.begin();itr!=g_deviceCache.end();itr++)
	{
		id = itr->first;
		info = itr->second;

		bool bAdd1=false;
		bool bAdd2=false;
		if(deviceName == NULL)
		{
			bAdd1=true;
		}
		else
		{
			if(0 == wcscmp(info.Name.c_str(),deviceName))
			{
				bAdd1=true;
			}
		}
		if(deviceClass == NULL)
		{
			bAdd2=true;
		}
		else
		{
			if(0 == wcscmp(info.ClassType.c_str(),deviceClass))
			{
				bAdd2=true;
			}
		}
		if(bAdd1 && bAdd2)
		{
			deviceList->push_back(id);
		}
		progressIncrement(); //Loop
	}
	progressRelease();	//Loop

	progressIncrement(); //Main
	progressRelease(); //Main

	return iReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
int deviceFillCache()
{
	int iReturnCode = DIAGLIB_OK;

	CFDictionaryRef	dataTypeDictionary = NULL;

	Device_ID id;
	const char * dataType = "SPUSBDataType";
	//Get devices in a Dictionary ("SPUSBDataType")
	if (DIAGLIB_OK != (iReturnCode = SystemProfiler(dataType, &dataTypeDictionary)))
	{
		LOG_LASTERROR(L"ProfileSystem failed");
		if (dataTypeDictionary != NULL) CFRelease(dataTypeDictionary);
		return RETURN_LOG_INTERNAL_ERROR;
	}

	g_deviceCache.clear();

	id.assign(L"\\");
	id.append(wstring_From_string(dataType));

	deviceFillCacheFromDictionary(dataTypeDictionary, dataType, id);

	if (dataTypeDictionary != NULL) CFRelease(dataTypeDictionary);

	return iReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
int deviceFillCacheFromDictionary(CFDictionaryRef inDictionary, const char * dataType, const Device_ID &currentId)
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
	Device_ID id;
	Device_ID searchId=id;

	progressInit(arrayCount);
	for (UInt8 i=0; i < arrayCount; i++)
	{
		CFDictionaryRef embeddedDictionary = (CFDictionaryRef)CFArrayGetValueAtIndex(itemsArray, i);

		CFStringRef nameValue=NULL;
		std::wstring wsName;
		if(CFDictionaryGetValueIfPresent(embeddedDictionary, CFSTR("_name"),(const void**)&nameValue))
		{
			wsName.assign(wstring_From_CFStringRef(nameValue));
			wstring_TrimR(&wsName);

			id.assign(currentId);
			id.append(L"\\");
			id.append(wsName);

			Device_INFO info;
			wchar_t wzBuff[10];
			searchId.assign(id);

			Device_CACHE::const_iterator itr;
			unsigned long count=0;
			do
			{
				itr = g_deviceCache.find(searchId);
				if(itr==g_deviceCache.end())
				{
					break;
				}

				count++;
				if( 0 > swprintf(wzBuff,10,L"%ld",count))
				{
					LOG_ERROR(L"sprintf failed\n");
					break;
				}

				searchId.assign(id);
				searchId.append(L"-");
				searchId.append(wzBuff);

			} while(1);

			//Fill info struct
			info.id = searchId;
			info.Name = wsName;

			std::wstring VendorId;
			std::wstring ProductId;
			SystemProfilerGetInfoString(embeddedDictionary,"vendor_id", &VendorId);
			if(VendorId.empty()) SystemProfilerGetInfoString(embeddedDictionary,"b_vendor_id", &VendorId);

			SystemProfilerGetInfoString(embeddedDictionary,"a_product_id", &ProductId);
			if(ProductId.empty()) SystemProfilerGetInfoString(embeddedDictionary,"product_id", &ProductId);

			info.ClassType.assign(UsbDeviceGetClassType(VendorId, ProductId));
			VendorId.append(L"\\");
			VendorId.append(ProductId);
			info.ClassGUID.assign(VendorId);

			info.FriendlyName.clear();
			info.Enumerator.assign(wstring_From_string(dataType));
			info.DriverInfo.clear();

			SystemProfilerGetInfoString(embeddedDictionary,"bcd_device", &info.HardwareID);
			if(info.HardwareID.empty()) SystemProfilerGetInfoString(embeddedDictionary,"c_bcd_device", &info.HardwareID);

			SystemProfilerGetInfoString(embeddedDictionary,"manufacturer", &info.Mfg);
			if(info.Mfg.empty()) SystemProfilerGetInfoString(embeddedDictionary,"f_manufacturer", &info.Mfg);
			wstring_TrimR(&info.Mfg);

			SystemProfilerGetInfoString(embeddedDictionary,"location_id", &info.Location);
			if(info.Location.empty()) SystemProfilerGetInfoString(embeddedDictionary,"g_location_id", &info.Location);

			info.Service.clear();

			g_deviceCache[searchId] = info;
		}

		//Loop recursively through embedded dictionary
		deviceFillCacheFromDictionary(embeddedDictionary, dataType, searchId);

		progressIncrement();
	}
	progressRelease();

	if (itemsArray != NULL) CFRelease(itemsArray);

	return iReturnCode;
}


////////////////////////////////////////////////////////////////////////////////////////////////
const wchar_t *UsbDeviceGetClassType(std::wstring const &VendorId, std::wstring const &ProductId)
{
/*	wchar_t *Stop=NULL;
	unsigned long ulVendorId = wcstol(VendorId.c_str(),&Stop,16);
	unsigned long ulProductId = wcstol(ProductId.c_str(),&Stop,16);

	switch(ulVendorId)
	{
	case 0x072f:	//ACS
	case 0x076b:	//Omnikey
	case 0x04e6:	//SCM Microsystem
		return L"SmartCardReader";
	}
*/
	return L"";
}