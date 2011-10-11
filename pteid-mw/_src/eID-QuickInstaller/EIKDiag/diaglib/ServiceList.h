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

#ifndef __SERVICELIST__
#define __SERVICELIST__

#pragma once

#undef UNICODE

#include <windows.h>

#include <string>

using namespace std;

#include "Service.h"

typedef vector<CService> ServiceList;
typedef ServiceList::iterator ServiceListIterator;
typedef vector<CService> ServicesFound;
typedef ServicesFound::iterator ServicesFoundIterator;

class CServiceList {
public:
  CServiceList(void);
  virtual ~CServiceList(void);
  ServiceList& Services() { return _Services; };
  ServicesFound& FindServices(
    ServiceKind ServiceType, 
    const char *ServiceName,
    const char *DisplayName,
    const char *ImagePath,
    const char *ObjectName);

private:
  void enum_NT_Services();
  void enum_Run_Services(HKEY hKey, const char *RunKey, ServiceKind Kind);
  ServiceList _Services;
  ServicesFound _ServicesFound;
};

#endif __SERVICELIST__
