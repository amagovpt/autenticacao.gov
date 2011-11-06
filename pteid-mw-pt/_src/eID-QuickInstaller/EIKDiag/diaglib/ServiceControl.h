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

#ifndef __SERVICECONTROL__
#define __SERVICECONTROL__

#pragma once

#undef UNICODE

#include <windows.h>

#include <string>

using namespace std;

#include "Service.h"

typedef struct _ServiceInfo {
    string DisplayName;
    DWORD ServiceType;
    DWORD StartType;
    DWORD ErrorControl;
    string Pathname;
    string UserAccount;
    string Description;
    string LoadOrderGroup;
    DWORD TagID;
    string Dependencies;
} ServiceInfo;

class CServiceControl {
public:
	// methods
    CServiceControl(string serviceName);
    virtual ~CServiceControl(void);
    ServiceInfo DoGetServiceInfo();
    bool DoGetServiceState();
    bool DoEnableService(DWORD startType);
    bool DoDisableService();
    bool DoStartService();
    bool DoStopService();
    bool DoDeleteService();
	string CurrentStateAsString();

	// properties
    DWORD LastError() { return _lastError; }
    bool Succeeded() { return (_lastError == 0); }
	bool Failed() {return (_lastError != 0); }
    DWORD CurrentState() { return _currentState; }
    string ServiceName() { return _serviceName; }


private:
    bool _ControlService(DWORD dwControl);
    DWORD _currentState;
    string _serviceName;

    SC_HANDLE _schSCManager;
    SC_HANDLE _schService;
    DWORD _lastError;

};

#endif __SERVICECONTROL__
