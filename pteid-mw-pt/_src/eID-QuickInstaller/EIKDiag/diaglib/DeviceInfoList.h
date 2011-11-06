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

#ifndef __DEVICEINFOLIST__
#define __DEVICEINFOLIST__

#pragma once


#undef UNICODE

#include <windows.h>
#include "DeviceInfo.h"

#include <string>
#include <vector>

using namespace std;

typedef vector<CDeviceInfo> DeviceList;
typedef DeviceList::iterator DevInfoIterator;

typedef vector<CDeviceInfo> DevicesFound;
typedef DevicesFound::iterator DevicesFoundIterator;

class CDeviceInfoList
{
public:
  CDeviceInfoList(void);
  ~CDeviceInfoList(void);

  bool BuildDeviceList();

  CDeviceInfo& getDevice(int index);
  size_t GetCount();
  DevInfoIterator getFirst();
  DevInfoIterator getLast();
  string LastError() { return _LastError; };
  DevicesFound& FindDevices(
    const char * _ClassGUID, 
    const char * _PhysicalObjName, 
    const char * _HardwareID, 
    const char * _Description, 
    const char * _DriverInfo, 
    const char * _FriendlyName, 
    const char * _Enumerator, 
    const char * _DeviceType, 
    const char * _Location, 
    const char * _Mfg, 
    const char * _Service, 
    const char * _ClassType);
private:
  DeviceList _DeviceInfoList;
  DevicesFound _DevicesFound;
  string _LastError;
};

#endif __DEVICEINFOLIST__

