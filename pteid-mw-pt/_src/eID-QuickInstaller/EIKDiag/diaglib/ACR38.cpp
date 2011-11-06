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

#include "ACR38.h"
#include "wildcards.h"

#pragma hdrstop


CACR38::CACR38(void) {
  
  this->_LastError = "";

  if ( ! this->_Devices.BuildDeviceList()) {
    this->_LastError = this->_Devices.LastError();
  }

}

CACR38::~CACR38(void) {
}

bool CACR38::FindInfFile(const string& ClassName,     //[VERSION]
                         const string& ClassGuid,     //[VERSION]
                         const string& DeviceDesc,    //[STRINGS] 
                         const string& MfgSection,    //[ACS]  
                         const string& DeviceID) {    //[ACS] %DeviceDesc%

  this->_LastError = "";

  CInfList inflist;

  if (!inflist.BuildInfFileList()) {
    this->_LastError = inflist.LastError();
	  return false;
  }

  InfFilesIterator last = inflist.GetLast();
  for (InfFilesIterator it = inflist.GetFirst(); it != last; ++it) {

    CInfHandle inffile(*it);

    //eg: "SmartCardReader" 
    string lClassName = inffile.GetLineText(string("VERSION"), string("Class"));
    if (lClassName.size() == 0)
      continue;
    if (! Wildcard::wildcardfit(lClassName.c_str(), ClassName.c_str()))
      continue;
    //eg: "{50DD5230-BA8A-11D1-BF5D-0000F805F530} 
    string lClassGuid = inffile.GetLineText(string("VERSION"), string("ClassGuid"));
    if (lClassGuid.size() == 0)
      continue;
    if (! Wildcard::wildcardfit(lClassGuid.c_str(), ClassGuid.c_str())) 
      continue;
    //expand device description string from STRINGS section (eg: "ACS.DeviceDesc")
    string lDeviceDesc = inffile.GetLineText(string("STRINGS"), DeviceDesc); 
    if (lDeviceDesc.size() == 0)
      continue;
    //find device description in [MFG] section (eg: [ACS])
    if (! inffile.FindFirstLine(MfgSection, lDeviceDesc))
      continue;
    while (true) {
      string USBVID = inffile.GetLineText();
      if (USBVID.size() != 0) {
        if (USBVID.find(DeviceID) != -1) {
          this->_InfFileName = *it;
          return true;
        }
      }
      if (! inffile.FindNextLine())
        break;
      // ik denk dat de volgende paragraaf hier overbodig is
      // maar ik zou het moeten opnieuw testen
      USBVID = inffile.GetLineText();
      if (USBVID.size() != 0) {
        if (USBVID.find(DeviceID) != -1) {
          this->_InfFileName = *it;
          return true;
        }
      }
      // tot hier dus
    }

  }

  this->_LastError = "FindInfFile(" + ClassName + ", " + ClassGuid + ", " + DeviceID + "): not found.";
  
  return false;

}


bool CACR38::CheckDeviceGUID(const string& ClassGUID){

  if (this->_Devices.GetCount() == 0)
    return false;

  this->_LastError = "";

  DevicesFound& df = this->_Devices.FindDevices(
    ClassGUID.c_str(), NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
  );
  if (df.size() != 0) {
      return true;
  }

  return false;

}

bool CACR38::CheckDeviceHWID(const string& ClassGUID, const string& DevHWID) {

  if (this->_Devices.GetCount() == 0)
    return false;

  this->_LastError = "";

  DevicesFound& df = this->_Devices.FindDevices(
    ClassGUID.c_str(), 
    NULL,
    DevHWID.c_str(),
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
  );
  if (df.size() != 0) {
      return true;
  }

  return false;

}
