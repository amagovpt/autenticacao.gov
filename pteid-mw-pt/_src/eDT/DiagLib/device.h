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
#ifndef __DIAGLIB_DEVICE_H__
#define __DIAGLIB_DEVICE_H__

#include <string.h>
#include <vector>

#include "report.h"
#include "reader.h"

typedef std::wstring Device_ID;
typedef std::vector<Device_ID> Device_LIST;

typedef std::wstring Device_NAME;
typedef std::wstring Device_CLASS;

typedef struct t_Device_INFO {
	Device_ID id;
    Device_NAME Name;
	Device_CLASS ClassType;
    std::wstring ClassGUID;
	std::wstring FriendlyName;
	std::wstring Enumerator;
	std::wstring DriverInfo;
	std::wstring HardwareID;
	std::wstring Mfg;
	std::wstring Location;
	std::wstring Service;
} Device_INFO;

int deviceGetList(Device_LIST *deviceList);
int deviceGetIDs(Device_NAME device, Device_LIST *deviceList);
int deviceGetIDsByClass (Device_CLASS deviceClass, Device_LIST *deviceList);
int deviceGetInfo(Device_ID device, Device_INFO *info);

int deviceGetReaderList(Reader_LIST *readersList);

int deviceReportInfo(Report_TYPE type, const Device_INFO &info);
int deviceReportList(Report_TYPE type, const Device_LIST &deviceList, const wchar_t *Title=NULL);

#endif //__DIAGLIB_DEVICE_H__

