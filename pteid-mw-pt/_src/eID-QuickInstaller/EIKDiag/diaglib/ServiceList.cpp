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

#include "StdAfx.h"
#include "ServiceList.h"
#include "Registry.h"
#include "Wildcards.h"

static const char *NT_SERVICEKEY   = "SYSTEM\\CurrentControlSet\\Services";
static const char *RUNKEY          = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run";
static const char *RUNONCEKEY      = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunOnce";
static const char *RUNONCEEXKEY    = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunOnceEx";

CServiceList::CServiceList(void) {

  this->_Services.reserve(256);

  enum_NT_Services();
  enum_Run_Services(HKEY_LOCAL_MACHINE, RUNKEY,          LM_RUN_Service);
  enum_Run_Services(HKEY_LOCAL_MACHINE, RUNONCEKEY,      LM_RUNONCE_Service);
  enum_Run_Services(HKEY_LOCAL_MACHINE, RUNONCEEXKEY,    LM_RUNONCEEX_Service);
  enum_Run_Services(HKEY_CURRENT_USER,  RUNKEY,          CU_RUN_Service);
  enum_Run_Services(HKEY_CURRENT_USER,  RUNONCEKEY,      CU_RUNONCE_Service);
  enum_Run_Services(HKEY_CURRENT_USER,  RUNONCEEXKEY,    CU_RUNONCEEX_Service);

}

CServiceList::~CServiceList(void) {
}

void CServiceList::enum_NT_Services() {

  // enumerate NT services
  CRegistry nt_services(HKEY_LOCAL_MACHINE, NT_SERVICEKEY);
  nt_services.GetSubKeys();
  RegSubKeys& subkeys = nt_services.SubKeys();
  RegSubKeysIterator last = subkeys.end();
  for (RegSubKeysIterator it = subkeys.begin(); it != last; ++it) {
    string key = NT_SERVICEKEY;
    key += "\\" + *it;
    CRegistry reg(HKEY_LOCAL_MACHINE, key.c_str());
    string displayname = "";
    if (reg.GetStringValue("DisplayName"))
      displayname = reg.StringValue();
    string imagepath = "";
    if (reg.GetStringValue("ImagePath"))
      imagepath = reg.StringValue();
    string objectname = "";
    if (reg.GetStringValue("ObjectName"))
      objectname = reg.StringValue();
    int start = -1;
    if (reg.GetIntValue("Start"))
      start = reg.IntValue();
    int type = -1;
    if (reg.GetIntValue("Type"))
      type = reg.IntValue();
    int errorcontrol = -1;
    if (reg.GetIntValue("ErrorControl"))
      errorcontrol = reg.IntValue();
    CService service(NT_Service, *it, displayname, imagepath, objectname, start, type, errorcontrol);
    this->_Services.push_back(service);
  }

}

void CServiceList::enum_Run_Services(HKEY hKey, const char *RunKey, ServiceKind Kind) {

  // enumerate NT services
  CRegistry run_services(hKey, RunKey);
  run_services.GetNames();
  RegNames& names = run_services.Names();
  RegNamesIterator last = names.end();
  for (RegNamesIterator it = names.begin(); it != last; ++it) {
    string imagepath = "";
    if (run_services.GetStringValue(it->c_str()))
      imagepath = run_services.StringValue();
    CService service(Kind, *it, imagepath);
    this->_Services.push_back(service);
  }

}

ServicesFound& CServiceList::FindServices(
    ServiceKind ServiceType, 
    const char *ServiceName,
    const char *DisplayName,
    const char *ImagePath,
    const char *ObjectName) 
{
  
  this->_ServicesFound.clear();

  if (this->_Services.size() == 0)
    return this->_ServicesFound;


  ServiceListIterator last = this->_Services.end();

  for (ServiceListIterator it = this->_Services.begin(); it != last; ++it) {

    if (ServiceType != ANY_Service) {
      if (! (it->ServiceType() == ServiceType) ) {
        continue;
      }
    }
    if (ServiceName != NULL) {
      if (! Wildcard::wildcardfit(ServiceName, it->ServiceName().c_str()) ) {
        continue;
      }
    }
    if (DisplayName != NULL) {
      if (! Wildcard::wildcardfit(DisplayName, it->DisplayName().c_str()) ) {
        continue;
      }
    }
    if (ImagePath != NULL) {
      if (! Wildcard::wildcardfit(ImagePath, it->ImagePath().c_str()) ) {
        continue;
      }
    }
    if (ObjectName != NULL) {
      if (! Wildcard::wildcardfit(ObjectName, it->ObjectName().c_str()) ) {
        continue;
      }
    }
    this->_ServicesFound.push_back(*it);
  }

  return this->_ServicesFound;

}


