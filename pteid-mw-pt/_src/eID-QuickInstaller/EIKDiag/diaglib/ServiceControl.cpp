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
#include "ServiceControl.h"

CServiceControl::CServiceControl(string serviceName) {

    this->_serviceName = serviceName;
    this->_lastError = 0;
    this->_currentState = 0xffffffff;

    this->_schSCManager = ::OpenSCManager( 
        NULL,                    // local computer
        NULL,                    // ServicesActive database 
        SC_MANAGER_ALL_ACCESS);  // full access rights 
        if (this->_schSCManager != NULL) {
    } else {
        this->_lastError = ::GetLastError();
        this->_schService = NULL;
        return;
    }

        this->_schService = ::OpenService(this->_schSCManager, serviceName.c_str(), SERVICE_ALL_ACCESS);
    if (this->_schService != NULL) {
    } else {
        this->_lastError = ::GetLastError();
        return;
    }

    SERVICE_STATUS ss;
    if (::QueryServiceStatus(this->_schService, &ss)) {
        this->_currentState = ss.dwCurrentState;
    } else {
        this->_lastError = ::GetLastError();
    }


}

CServiceControl::~CServiceControl() {

    this->_lastError = 0;
    if (this->_schSCManager != NULL)
        ::CloseServiceHandle(this->_schSCManager);

    if (this->_schService != NULL)
        ::CloseServiceHandle(_schService);

}

bool CServiceControl::DoGetServiceState() {

    SERVICE_STATUS ss;
    if (::QueryServiceStatus(this->_schService, &ss)) {
        this->_lastError = 0;
    } else {
        this->_lastError = ::GetLastError();
    }
    this->_currentState = ss.dwCurrentState;

    return this->Succeeded();
}

string CServiceControl::CurrentStateAsString() {

    string state;
    switch(this->_currentState) {
        case SERVICE_STOPPED: 
            state = "stopped";
            break;
        case SERVICE_START_PENDING:                
            state = "start pending";
            break;
        case SERVICE_STOP_PENDING:                  
            state = "stop pending";
            break;
        case SERVICE_RUNNING:                        
            state = "running";
            break;
        case SERVICE_CONTINUE_PENDING:               
            state = "resume pending";
            break;
        case SERVICE_PAUSE_PENDING:                 
            state = "pause pending";
            break;
        case SERVICE_PAUSED:                         
            state = "paused";
            break;
        default:
            state = "unknwon";
            break;
    }
    return state;

}

bool CServiceControl::DoStartService() {
    this->_lastError = 0;
    if (!::StartService(this->_schService, 0, NULL)) {
        this->_lastError = ::GetLastError();
    }
    return this->Succeeded();
}

bool CServiceControl::_ControlService(DWORD dwControl) {

    SERVICE_STATUS ss;

    if (::ControlService(this->_schService, dwControl, &ss)) {
        this->_lastError = 0;
    } else {
        this->_lastError = ::GetLastError();
    }
    this->_currentState = ss.dwCurrentState;

    return this->Succeeded();

}
bool CServiceControl::DoStopService() {

    return this->_ControlService(SERVICE_CONTROL_STOP);

}

bool CServiceControl::DoDisableService() {
    this->_lastError = 0;
    if (!::ChangeServiceConfig(
        this->_schService,
        SERVICE_NO_CHANGE,
        SERVICE_DISABLED,
        SERVICE_NO_CHANGE,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL)) {
            this->_lastError = ::GetLastError();
    }
    return this->Succeeded();
}

bool CServiceControl::DoEnableService(DWORD startType) {
    this->_lastError = 0;
    if (!::ChangeServiceConfig(
        this->_schService,
        SERVICE_NO_CHANGE,
        startType,
        SERVICE_NO_CHANGE,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL)) {
            this->_lastError = ::GetLastError();
    }
    return this->Succeeded();
}

bool CServiceControl::DoDeleteService() {
    this->_lastError = 0;
    if (!::DeleteService(this->_schService)) {
        this->_lastError = ::GetLastError();
    }
    return this->Succeeded();
}

ServiceInfo CServiceControl::DoGetServiceInfo() {

    ServiceInfo Result;
    this->_lastError = 0;

    LPQUERY_SERVICE_CONFIG lpsc;
    DWORD dwNeeded = 0;
    if (::QueryServiceConfig(this->_schService, NULL, 0, &dwNeeded) == 0) {
        DWORD dwError = GetLastError();
        if (dwError == ERROR_INSUFFICIENT_BUFFER) {
            DWORD cbBufSize = dwNeeded;
            lpsc = (LPQUERY_SERVICE_CONFIG)::LocalAlloc(LMEM_FIXED, cbBufSize);
            if (::QueryServiceConfig(this->_schService, lpsc, cbBufSize, &dwNeeded)) {
                Result.DisplayName = lpsc->lpDisplayName;
                Result.ServiceType = lpsc->dwServiceType;
                Result.ErrorControl = lpsc->dwErrorControl;
                Result.Pathname = lpsc->lpBinaryPathName;
                Result.LoadOrderGroup = lpsc->lpLoadOrderGroup;
                Result.TagID = lpsc->dwTagId;
                Result.Dependencies = lpsc->lpDependencies;
                Result.UserAccount = lpsc->lpServiceStartName;
                Result.StartType = lpsc->dwStartType;
            } else {
                this->_lastError = ::GetLastError();
            }
            LocalFree(lpsc);
        } else {
            this->_lastError = dwError;
        }
    } else {
        this->_lastError = ::GetLastError();
    }

    return Result;

}
