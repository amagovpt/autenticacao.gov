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
#ifdef WIN32
#include <windows.h>
#elif __APPLE__
#include "Mac/mac_helper.h"
#endif

#include "diaglib.h"

#include "device.h"
#include "error.h"
#include "log.h"
#include "progress.h"

#include "Repository.h"

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PRIVATE FUNCTIONS DECLARATION ////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
int deviceFillList(Device_LIST *deviceList, const wchar_t *deviceDescription, const wchar_t *deviceClass);

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PUBLIC FUNCTIONS /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
int deviceGetList (Device_LIST *deviceList)
{
	return deviceFillList(deviceList, NULL, NULL);
}

////////////////////////////////////////////////////////////////////////////////////////////////
int deviceGetIDs (Device_NAME device, Device_LIST *deviceList)
{
	return deviceFillList(deviceList, device.c_str(), NULL);
} 

////////////////////////////////////////////////////////////////////////////////////////////////
int deviceGetIDsByClass (Device_CLASS deviceClass, Device_LIST *deviceList)
{
	return deviceFillList(deviceList, NULL, deviceClass.c_str());
}

////////////////////////////////////////////////////////////////////////////////////////////////
int deviceGetReaderList(Reader_LIST *readersList)
{
	int iReturnCode = DIAGLIB_OK;

	if(readersList == NULL)
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;
	}

	readersList->clear();

	Device_CLASS device = L"SmartCardReader";
	Device_LIST deviceList;
    if(DIAGLIB_OK != (iReturnCode = deviceGetIDsByClass(device, &deviceList)))
	{
		LOG_ERROR(L"deviceGetIDsByClass failed");
		return iReturnCode;
	}

	Reader_ID id;
	id.Source=DEVICE_READER_SOURCE;

	Device_LIST::const_iterator itr;
	for(itr=deviceList.begin();itr!=deviceList.end();itr++)
	{
		id.Name=*itr;
		readersList->push_back(id);
	}

	return iReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
int deviceReportInfo(Report_TYPE type, const Device_INFO &info)
{
	int iReturnCode = DIAGLIB_OK;

	reportPrint(type,L"           id = %ls\n", info.id.c_str());
	reportPrint(type,L"         Name = %ls\n", info.Name.c_str());
	reportPrint(type,L"    ClassType = %ls\n", info.ClassType.c_str());
	reportPrint(type,L"    ClassGUID = %ls\n", info.ClassGUID.c_str());
	reportPrint(type,L" FriendlyName = %ls\n", info.FriendlyName.c_str());
	reportPrint(type,L"   Enumerator = %ls\n", info.Enumerator.c_str());
	reportPrint(type,L"   DriverInfo = %ls\n", info.DriverInfo.c_str());
	reportPrint(type,L"   HardwareID = %ls\n", info.HardwareID.c_str());
	reportPrint(type,L"          Mfg = %ls\n", info.Mfg.c_str());
	reportPrint(type,L"     Location = %ls\n", info.Location.c_str());
	reportPrint(type,L"      Service = %ls\n", info.Service.c_str());
	reportPrintSeparator(type, REPORT_DEVICE_SEPARATOR);

	return iReturnCode;
}
void deviceContributeInfo(const Device_INFO &info)
{
	REP_PREFIX(		L"%ls.%ls.%ls",	info.ClassType.c_str(),info.Enumerator.c_str(),info.Service.empty()?L"None":info.Service.c_str());
	REP_CONTRIBUTE(	L"id",			info.id.c_str());
	REP_CONTRIBUTE(	L"Name",		info.Name.c_str());
	REP_CONTRIBUTE(	L"ClassTypen",	info.ClassType.c_str());
	REP_CONTRIBUTE(	L"ClassGUID",	info.ClassGUID.c_str());
	REP_CONTRIBUTE(	L"FriendlyName",info.FriendlyName.c_str());
	REP_CONTRIBUTE(	L"Enumerator",	info.Enumerator.c_str());
	REP_CONTRIBUTE(	L"DriverInfo",	info.DriverInfo.c_str());
	REP_CONTRIBUTE(	L"HardwareID",	info.HardwareID.c_str());
	REP_CONTRIBUTE(	L"Mfg",			info.Mfg.c_str());
	REP_CONTRIBUTE(	L"Location",	info.Location.c_str());
	REP_CONTRIBUTE(	L"Service",		info.Service.c_str());
	REP_UNPREFIX();
}


////////////////////////////////////////////////////////////////////////////////////////////////
int deviceReportList(Report_TYPE type, const Device_LIST &deviceList, const wchar_t *TitleIn)
{
	int iReturnCode = DIAGLIB_OK;

	std::wstring Title;

	if(TitleIn!=NULL)
		Title=TitleIn;
	else
		Title=L"Device list";
	
	Title.append(L" (#");
	wchar_t buf[10];
	if(-1==swprintf_s(buf,10,L"%ld",deviceList.size()))
	{
		Title.append(L"???");
		LOG_ERROR(L"swprintf_s failed");
	}
	else
	{
		Title.append(buf);
	}
	Title.append(L")");

	reportPrintHeader2(type, Title.c_str(), REPORT_DEVICE_SEPARATOR);

	Device_INFO info;

	progressInit((int)deviceList.size());

	Device_LIST::const_iterator itr;
	for(itr=deviceList.begin();itr!=deviceList.end();itr++)
	{
		if(DIAGLIB_OK == deviceGetInfo(*itr,&info))
		{
			deviceReportInfo(type,info);
			deviceContributeInfo(info);
		}

		progressIncrement();
	}

	progressRelease();

	return iReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PRIVATE FUNCTIONS ////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
