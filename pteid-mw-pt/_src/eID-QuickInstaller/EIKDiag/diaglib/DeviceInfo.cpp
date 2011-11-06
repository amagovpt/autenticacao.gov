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
#pragma once

#include "DeviceInfo.h"
#include "ErrorFmt.h"

#pragma hdrstop

CDeviceInfo::CDeviceInfo() {
}

bool CDeviceInfo::GetDeviceInfo(HDEVINFO hDevInfo, PSP_DEVINFO_DATA pdevInfoData) {

  _ClassType        = GetDevInfoString(hDevInfo, pdevInfoData, SPDRP_CLASS);
  _ClassGUID        = GetDevInfoString(hDevInfo, pdevInfoData, SPDRP_CLASSGUID);
  _Description      = GetDevInfoString(hDevInfo, pdevInfoData, SPDRP_DEVICEDESC);
  _Enumerator       = GetDevInfoString(hDevInfo, pdevInfoData, SPDRP_ENUMERATOR_NAME);
  _FriendlyName     = GetDevInfoString(hDevInfo, pdevInfoData, SPDRP_FRIENDLYNAME);
  _DriverInfo       = GetDevInfoString(hDevInfo, pdevInfoData, SPDRP_DRIVER);
  _PhysicalObjName  = GetDevInfoString(hDevInfo, pdevInfoData, SPDRP_PHYSICAL_DEVICE_OBJECT_NAME);
  _HardwareID       = GetDevInfoString(hDevInfo, pdevInfoData, SPDRP_HARDWAREID);
  _Mfg              = GetDevInfoString(hDevInfo, pdevInfoData, SPDRP_MFG);
  _Location         = GetDevInfoString(hDevInfo, pdevInfoData, SPDRP_LOCATION_INFORMATION);
  _Service          = GetDevInfoString(hDevInfo, pdevInfoData, SPDRP_SERVICE);

  return true;

}

CDeviceInfo::~CDeviceInfo(void) {
}

string CDeviceInfo::GetDevInfoString(HDEVINFO hDevInfo, PSP_DEVINFO_DATA pdevInfoData, DWORD devProperty) {
  
  DWORD ldwSize = 0;
/* 
 * problems with size determination in Win98
 *
  BOOL lbRet = SetupDiGetDeviceRegistryProperty(
    hDevInfo, pdevInfoData, devProperty, NULL, NULL, 0, &ldwSize);
  if (! lbRet) {
    int lError = GetLastError();
    if (lError != ERROR_INSUFFICIENT_BUFFER) {
      string e = CErrorFmt::FormatError(GetLastError(), "SetupDiGetDeviceRegistryProperty");
      return e;
    }
  }
*/

  ldwSize = 1024; // Windows 98 

  const auto_vec<char> cResult(new char[ldwSize]);

  BOOL lbRet = SetupDiGetDeviceRegistryProperty(
    hDevInfo, 
    pdevInfoData, 
    devProperty, 
    NULL, 
    reinterpret_cast<PBYTE>(cResult.get()), 
    ldwSize, 
    &ldwSize);

  if (!lbRet) {
    int le = GetLastError();
    if (le == ERROR_INVALID_DATA) 
      return "";
    string e = CErrorFmt::FormatError(le, "SetupDiGetDeviceRegistryProperty");
    
    return e;
  }

  string sResult = cResult.get();
  return sResult;

}

/*
  std::string s("Hello");
  char *array = new char[s.size()];
  std::copy(s.begin(), s.end(), array, array + s.size());
  delete [] array; 
*/


