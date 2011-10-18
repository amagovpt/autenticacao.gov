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

#include "stdafx.h"
#include "DeviceinfoList.h"
#include "ErrorFmt.h"
#include "Wildcards.h"

#pragma hdrstop




string ToUpcase(const string& str){
  string result = "";
  char ch;

  string::const_iterator i1 = str.begin();

  while (i1 != str.end() ){
	  ch = toupper(*i1);
     result = result + ch;
     
     ++i1;
  }
  return result;
}



CDeviceInfoList::CDeviceInfoList(void) 
{
}

CDeviceInfoList::~CDeviceInfoList(void) 
{
}


bool CDeviceInfoList::BuildDeviceList()
{

  _LastError = "";

  HDEVINFO lhDevInfo = SetupDiGetClassDevs(
    NULL,
    NULL,
    0,
    DIGCF_ALLCLASSES | DIGCF_PRESENT);
  if (lhDevInfo == INVALID_HANDLE_VALUE) {
    _LastError = CErrorFmt::FormatError(GetLastError(), "CDeviceInfoList::BuildDeviceList");
    return false;
  }
  DWORD ldwIndex = 0;
  SP_DEVINFO_DATA lDevInfoData;
  memset(&lDevInfoData, 0, sizeof(SP_DEVINFO_DATA));
  lDevInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
  while (true) {
    BOOL lbRet = SetupDiEnumDeviceInfo(
      lhDevInfo, 
      ldwIndex,
      &lDevInfoData);
    if (!lbRet) {
      int lError = GetLastError();
      if (lError == ERROR_NO_MORE_ITEMS) {
        break;
      }
      if (lError != 0) {
        _DeviceInfoList.clear();
        _LastError = CErrorFmt::FormatError(lError, "CDeviceInfoList::BuildDeviceList");
        break;
      }
    }
    CDeviceInfo lDeviceInfo;
    lDeviceInfo.GetDeviceInfo(lhDevInfo, &lDevInfoData);
    this->_DeviceInfoList.push_back(lDeviceInfo);

    ++ldwIndex;
  }


  SetupDiDestroyDeviceInfoList(lhDevInfo);

  if (this->GetCount() == 0) {
    return false;
  } else {
    return true;
  }

}


CDeviceInfo& CDeviceInfoList::getDevice(int index) 
{

  return this->_DeviceInfoList.at(index);

}

size_t CDeviceInfoList::GetCount() 
{

  return this->_DeviceInfoList.size();

}

DevInfoIterator CDeviceInfoList::getFirst() 
{
  return this->_DeviceInfoList.begin();
}

DevInfoIterator CDeviceInfoList::getLast() 
{
  return this->_DeviceInfoList.end();
}


DevicesFound& CDeviceInfoList::FindDevices(
    const char *ClassGUID, 
    const char *PhysicalObjName, 
    const char *HardwareID, 
    const char *Description, 
    const char *DriverInfo, 
    const char *FriendlyName, 
    const char *Enumerator, 
    const char *DeviceType, 
    const char *Location, 
    const char *Mfg, 
    const char *Service, 
    const char *ClassType) 
{

  this->_DevicesFound.clear();
  
  if (this->_DeviceInfoList.size() == 0)
    return this->_DevicesFound;

  DevInfoIterator last = this->_DeviceInfoList.end();

  for (DevInfoIterator it = this->_DeviceInfoList.begin(); it != last; ++it) {
    if (ClassGUID != NULL) {
		string tmpClassGUID = ToUpcase(string(ClassGUID));
        string tmpitClassGUID = ToUpcase(it->getClassGUID());


        		

	  if (! Wildcard::wildcardfit( tmpClassGUID.c_str(),tmpitClassGUID.c_str())) {
        continue;
      }
    }
    if (PhysicalObjName != NULL) {
      if (! Wildcard::wildcardfit( PhysicalObjName,it->getPhysicalObjName().c_str())) {
        continue;
      }
    }
    if (HardwareID != NULL) {

	  string tmpHardwareID = ToUpcase(string(HardwareID));
      string tmpitHWID = ToUpcase(it->getHardwareID ());

	  if (! Wildcard::wildcardfit( tmpHardwareID.c_str(),tmpitHWID.c_str())) {
        continue;
      }
    }
    if (Description != NULL) {
      if (! Wildcard::wildcardfit( Description,it->getDescription().c_str())) {
        continue;
      }
    }
    if (DriverInfo != NULL) {
      if (! Wildcard::wildcardfit( DriverInfo,it->getDriverInfo().c_str())) {
        continue;
      }
    }
    if (FriendlyName != NULL) {
      if (! Wildcard::wildcardfit( FriendlyName, it->getFriendlyName().c_str())) {
        continue;
      }
    }
    if (Enumerator != NULL) {
      if (! Wildcard::wildcardfit( Enumerator,it->getEnumerator().c_str())) {
        continue;
      }
    }
    if (DeviceType != NULL) {
      if (! Wildcard::wildcardfit( DeviceType,it->getDeviceType().c_str())) {
        continue;
      }
    }
    if (Location != NULL) {
      if (! Wildcard::wildcardfit( Location, it->getLocation().c_str())) {
        continue;
      }
    }
    if (Mfg != NULL) {
      if (! Wildcard::wildcardfit( Mfg,it->getMfg().c_str())) {
        continue;
      }
    }
    if (Service != NULL) {
      if (! Wildcard::wildcardfit( Service, it->getService().c_str())) {
        continue;
      }
    }
    if (ClassType != NULL) {
      if (! Wildcard::wildcardfit( ClassType , it->getClassType().c_str())) {
        continue;
      }
    }

    this->_DevicesFound.push_back(*it);


  }
  
  return this->_DevicesFound;
    
}

