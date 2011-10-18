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
#include <SetupApi.h>

#include "device.h"
#include "error.h"
#include "log.h"
#include "progress.h"

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PRIVATE FUNCTIONS DECLARATION ////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
int deviceFillList(Device_LIST *deviceList, const wchar_t *deviceDescription, const wchar_t *deviceClass);
int deviceGetInfoString(HDEVINFO hDevInfo, PSP_DEVINFO_DATA pdevInfoData, DWORD devProperty, std::wstring *Value);

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PUBLIC FUNCTIONS /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
int deviceGetInfo(Device_ID device, Device_INFO *info)
{
	int iReturnCode = DIAGLIB_OK;
	int iTmpCode = DIAGLIB_OK;

	if(info==NULL)
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;
	}

	LOG_TIME(L"Ask for info on device '%ls' --> ",device.c_str());

	//Get a handle for this device
	HDEVINFO lhDevInfo = INVALID_HANDLE_VALUE;
	if( INVALID_HANDLE_VALUE == (lhDevInfo = SetupDiGetClassDevs(NULL,NULL,0,DIGCF_ALLCLASSES | DIGCF_PRESENT)))
	{
		LOG_LASTERROR(L"SetupDiGetClassDevs failed");
		return RETURN_LOG_INTERNAL_ERROR;
	}

	DWORD ldwIndex = 0;
	SP_DEVINFO_DATA lDevInfoData;
	ZeroMemory(&lDevInfoData, sizeof(SP_DEVINFO_DATA));
	lDevInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

	iReturnCode = DIAGLIB_ERR_DEVICE_NOT_FOUND;

	while(iReturnCode != DIAGLIB_OK) 
	{
		if(!SetupDiEnumDeviceInfo(lhDevInfo, ldwIndex++, &lDevInfoData))
		{
			if (ERROR_NO_MORE_ITEMS != GetLastError()) 
			{
				LOG_LASTERROR(L"SetupDiEnumDeviceInfo failed");
				iReturnCode = DIAGLIB_ERR_INTERNAL;
			}
			break;
		}
		else 
		{
			Device_ID id;
			if( DIAGLIB_OK == deviceGetInfoString(lhDevInfo, &lDevInfoData, SPDRP_PHYSICAL_DEVICE_OBJECT_NAME, &id))
			{
				if(0 == _wcsicmp(id.c_str(),device.c_str()))	
				{
					iReturnCode = DIAGLIB_OK;
				}
			}

		}
	}

	if(iReturnCode == DIAGLIB_ERR_DEVICE_NOT_FOUND)
	{
		LOG(L"NOT FOUND\n");
		return DIAGLIB_ERR_DEVICE_NOT_FOUND;
	}
	else if(iReturnCode == DIAGLIB_OK)
	{
		info->id = device;
		iTmpCode = deviceGetInfoString(lhDevInfo, &lDevInfoData, SPDRP_DEVICEDESC, &info->Name);			if(iTmpCode!=DIAGLIB_OK) iReturnCode=iTmpCode;
		iTmpCode = deviceGetInfoString(lhDevInfo, &lDevInfoData, SPDRP_CLASS, &info->ClassType);			if(iTmpCode!=DIAGLIB_OK) iReturnCode=iTmpCode;
		iTmpCode = deviceGetInfoString(lhDevInfo, &lDevInfoData, SPDRP_CLASSGUID, &info->ClassGUID);		if(iTmpCode!=DIAGLIB_OK) iReturnCode=iTmpCode;
		iTmpCode = deviceGetInfoString(lhDevInfo, &lDevInfoData, SPDRP_FRIENDLYNAME, &info->FriendlyName);	if(iTmpCode!=DIAGLIB_OK) iReturnCode=iTmpCode;
		iTmpCode = deviceGetInfoString(lhDevInfo, &lDevInfoData, SPDRP_ENUMERATOR_NAME, &info->Enumerator);	if(iTmpCode!=DIAGLIB_OK) iReturnCode=iTmpCode;
		iTmpCode = deviceGetInfoString(lhDevInfo, &lDevInfoData, SPDRP_DRIVER, &info->DriverInfo);			if(iTmpCode!=DIAGLIB_OK) iReturnCode=iTmpCode;
		iTmpCode = deviceGetInfoString(lhDevInfo, &lDevInfoData, SPDRP_HARDWAREID, &info->HardwareID);		if(iTmpCode!=DIAGLIB_OK) iReturnCode=iTmpCode;
		iTmpCode = deviceGetInfoString(lhDevInfo, &lDevInfoData, SPDRP_MFG, &info->Mfg);					if(iTmpCode!=DIAGLIB_OK) iReturnCode=iTmpCode;
		iTmpCode = deviceGetInfoString(lhDevInfo, &lDevInfoData, SPDRP_LOCATION_INFORMATION, &info->Location);if(iTmpCode!=DIAGLIB_OK) iReturnCode=iTmpCode;
		iTmpCode = deviceGetInfoString(lhDevInfo, &lDevInfoData, SPDRP_SERVICE, &info->Service);			if(iTmpCode!=DIAGLIB_OK) iReturnCode=iTmpCode;
	}

	if(!SetupDiDestroyDeviceInfoList(lhDevInfo))
	{
		LOG_LASTERROR(L"SetupDiDestroyDeviceInfoList failed");
	}

	LOG(L"DONE\n");

	return iReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PRIVATE FUNCTIONS ////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
int deviceFillList(Device_LIST *deviceList, const wchar_t *deviceDescription, const wchar_t *deviceClass)
{
	int iReturnCode = DIAGLIB_OK;

	if(deviceList == NULL)
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;
	}

	deviceList->clear();

	HDEVINFO lhDevInfo = INVALID_HANDLE_VALUE;
	if( INVALID_HANDLE_VALUE == (lhDevInfo = SetupDiGetClassDevs(NULL,NULL,0,DIGCF_ALLCLASSES | DIGCF_PRESENT)))
	{
		LOG_LASTERROR(L"SetupDiGetClassDevs failed");
		return RETURN_LOG_INTERNAL_ERROR;
	}

	DWORD ldwIndex = 0;
	SP_DEVINFO_DATA lDevInfoData;
	ZeroMemory(&lDevInfoData, sizeof(SP_DEVINFO_DATA));
	lDevInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

	DWORD progressMax = 0;
	while(true) 
	{
		if(!SetupDiEnumDeviceInfo(lhDevInfo, progressMax++, &lDevInfoData))
		{
			if (ERROR_NO_MORE_ITEMS != GetLastError()) 
			{
				LOG_LASTERROR(L"SetupDiEnumDeviceInfo failed");
				iReturnCode = DIAGLIB_ERR_INTERNAL;
			}
			break;
		}
	}

	progressInit(progressMax);

	while(true) 
	{
		if(!SetupDiEnumDeviceInfo(lhDevInfo, ldwIndex++, &lDevInfoData))
		{
			if (ERROR_NO_MORE_ITEMS != GetLastError()) 
			{
				LOG_LASTERROR(L"SetupDiEnumDeviceInfo failed");
				iReturnCode = DIAGLIB_ERR_INTERNAL;
			}
			break;
		}
		else 
		{
			//Store the Device_ID into the vector
			Device_ID id;

			if( DIAGLIB_OK == deviceGetInfoString(lhDevInfo, &lDevInfoData, SPDRP_PHYSICAL_DEVICE_OBJECT_NAME, &id))
			{
				bool bAdd1=false;
				bool bAdd2=false;
				if(deviceDescription == NULL)
				{
					bAdd1=true;
				}
				else 
				{
					Device_NAME value;
					if( DIAGLIB_OK == deviceGetInfoString(lhDevInfo, &lDevInfoData, SPDRP_DEVICEDESC, &value))
					{
						if(0 == _wcsicmp(value.c_str(),deviceDescription))	
						{
							bAdd1=true;
						}
					}		
				}
				if(deviceClass == NULL)
				{
					bAdd2=true;
				}
				else 
				{
					Device_CLASS value;
					if( DIAGLIB_OK == deviceGetInfoString(lhDevInfo, &lDevInfoData, SPDRP_CLASS, &value))
					{
						if(0 == _wcsicmp(value.c_str(),deviceClass))	
						{
							bAdd2=true;
						}
					}		
				}
				if(bAdd1 && bAdd2)
				{
					deviceList->push_back(id);
				}
			}
		}

		progressIncrement();
	}

	progressRelease();

	if(!SetupDiDestroyDeviceInfoList(lhDevInfo))
	{
		LOG_LASTERROR(L"SetupDiDestroyDeviceInfoList failed");
	}

	return iReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
int deviceGetInfoString(HDEVINFO hDevInfo, PSP_DEVINFO_DATA pdevInfoData, DWORD devProperty, std::wstring *Value) 
{
 	int iReturnCode = DIAGLIB_OK;

	if(Value == NULL)
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;
	}

	DWORD dwBytesNeeded = 0;
	DWORD dwBufSize = 0;
	PUCHAR pBuf = NULL;

	Value->clear();

	if(SetupDiGetDeviceRegistryProperty(hDevInfo, pdevInfoData, devProperty, NULL, NULL, 0, &dwBytesNeeded))
	{
		LOG_LASTERROR(L"SetupDiGetDeviceRegistryProperty failed to get size needed");
		iReturnCode = DIAGLIB_ERR_INTERNAL;
	}
	else
	{
		if (ERROR_INSUFFICIENT_BUFFER != GetLastError() || dwBytesNeeded < 1) 
		{
			if (ERROR_INVALID_DATA != GetLastError())
			{
				LOG_LASTERROR(L"SetupDiGetDeviceRegistryProperty failed to get size needed");
				iReturnCode = DIAGLIB_ERR_INTERNAL;
			}
			else 			
			{
				//LOG(L"Property %ld unknown for this device\n",devProperty);
				return iReturnCode;
			}
		}
	}

	if(iReturnCode == DIAGLIB_OK) 
	{
		//Allocate the buffer
  		dwBufSize = dwBytesNeeded + 0x10; 
		if(NULL == (pBuf  = (PUCHAR) malloc(dwBufSize)))
		{
			LOG_ERROR(L"Memory allocation failed");
			iReturnCode = DIAGLIB_ERR_INTERNAL;
		}
		else if(!SetupDiGetDeviceRegistryProperty(hDevInfo, pdevInfoData, devProperty, NULL, pBuf, dwBufSize, &dwBufSize))
		{
			if (ERROR_INVALID_DATA != GetLastError())
			{
				LOG_LASTERROR(L"SetupDiGetDeviceRegistryProperty failed");
				iReturnCode = DIAGLIB_ERR_INTERNAL;
			}
			else
			{
				//LOG(L"Property %ld unknown for this device\n",devProperty);
			}
		}
		else
		{
			*Value=(wchar_t *)pBuf;
		}
  		if(pBuf) free(pBuf);
	}

	return iReturnCode;
}
