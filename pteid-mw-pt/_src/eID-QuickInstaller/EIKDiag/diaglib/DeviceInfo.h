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

#ifndef __DEVICEINFO__
#define __DEVICEINFO__

#include "stdafx.h"

#define _SETUP_APIVER 0x0500
#include <SetupApi.h>

class CDeviceInfo  {
public:
  CDeviceInfo();
  ~CDeviceInfo(void);

  bool GetDeviceInfo(HDEVINFO hDevInfo, PSP_DEVINFO_DATA pdevInfoData);

  string getClassGUID()       {return _ClassGUID;} 
  string getPhysicalObjName() {return _PhysicalObjName;} 
  string getHardwareID()      {return _HardwareID;} 
  string getDescription()     {return _Description;} 
  string getDriverInfo()      {return _DriverInfo;} 
  string getFriendlyName()    {return _FriendlyName;} 
  string getEnumerator()      {return _Enumerator;} 
  string getDeviceType()      {return _DeviceType;} 
  string getLocation()        {return _Location;} 
  string getMfg()             {return _Mfg;} 
  string getService()         {return _Service;} 
  string getClassType()       {return _ClassType;} 

private:
  string _ClassGUID; 
  string _PhysicalObjName; 
  string _HardwareID; 
  string _Description; 
  string _DriverInfo; 
  string _FriendlyName; 
  string _Enumerator; 
  string _DeviceType; 
  string _Location; 
  string _Mfg; 
  string _Service; 
  string _ClassType; 

  string GetDevInfoString(HDEVINFO hDevInfo, PSP_DEVINFO_DATA pdevInfoData, DWORD devProperty);

};

#endif __DEVICEINFO__
