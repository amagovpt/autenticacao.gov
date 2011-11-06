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

#include "UpdateDriver.h"

#pragma hdrstop

CUpdateDriver::CUpdateDriver(): _lastErrorCode(0), _lastError("") {

}

CUpdateDriver::~CUpdateDriver() {

}

bool CUpdateDriver::CopyInf(string& infFilePath) {

	const char* inffile = infFilePath.c_str();

	if (SetupCopyOEMInf(
		inffile,				// full path to .inf file
		NULL,					// path of .inf is path of driver files
		SPOST_PATH,				// copy driver path
		SP_COPY_NOOVERWRITE,	// don't repolace if already present
		NULL,					// NA not interested in destination .inf filename
		0,						// NA size of .inf filename returned buffer
		NULL,					// NA not interested in returned size
		NULL)) {				// NA not interested in DestinationInfFileNameComponent 
			return true;
	}
	this->_lastErrorCode = GetLastError();
	if (this->_lastErrorCode == ERROR_FILE_EXISTS)
		return true;
	else
		return false;

}


bool CUpdateDriver::IsDeviceInstallInprogress(DWORD dwTimeOutMillis) {

	HMODULE hModule;
	CMP_WAITNOPENDINGINSTALLEVENTS_PROC pCMP_WaitNoPendingInstallEvents;

	hModule = GetModuleHandle(TEXT("setupapi.dll"));
	if(!hModule) {
		// Should never happen since we're linked to SetupAPI, but...
		return FALSE;
	}

	pCMP_WaitNoPendingInstallEvents = (CMP_WAITNOPENDINGINSTALLEVENTS_PROC)GetProcAddress(hModule, "CMP_WaitNoPendingInstallEvents");
	if(!pCMP_WaitNoPendingInstallEvents) {
		// We're running on a release of the operating system that doesn't supply this function.
		// Trust the operating system to suppress AutoRun when appropriate.
		return FALSE;
	}
	return (pCMP_WaitNoPendingInstallEvents(dwTimeOutMillis) == WAIT_TIMEOUT);

}

bool CUpdateDriver::PrepareDriver(string infFilePath) {

	// er mag geen driver installatie bezig zijn, anders kan het spel hangen
	if (this->IsDeviceInstallInprogress(5000)) {
		this->_lastError = "UpdateDriver: Device install in progress - please retry later";
		return false;
	}
	// informeer Windows & PnP over de nieuwe driver zodat hij kan geïnstallerd worden door in te pluggen
	if (!this->CopyInf(infFilePath)) {
		this->_lastError = CErrorFmt::FormatError(this->_lastErrorCode, string(string("UpdateDriver: ") + infFilePath).c_str());
		return false;
	}
	return true;
}

bool CUpdateDriver::UpdateDriver(HWND hwndParent, string hardwareID, string infFilePath, DWORD flags, bool *bootRequired) {


	// er mag geen driver installatie bezig zijn, anders kan het spel hangen
	if (this->IsDeviceInstallInprogress(5000)) {
		this->_lastError = "UpdateDriver: Device install in progress - please retry later";
		return false;
	}
	// informeer Windows & PnP over de nieuwe driver zodat hij kan geïnstallerd worden door in te pluggen
	if (!this->CopyInf(infFilePath)) {
		this->_lastError = CErrorFmt::FormatError(this->_lastErrorCode, string(string("UpdateDriver: ") + infFilePath).c_str());
		return false;
	}

	BOOL BootReqd = false;
    *bootRequired = false;

	if (UpdateDriverForPlugAndPlayDevices(
		hwndParent, 
		hardwareID.c_str(),
		infFilePath.c_str(),
		flags,
		&BootReqd)) {
			this->_lastError = "";
			this->_lastErrorCode = 0;
			if (bootRequired != NULL)
				*bootRequired = (BootReqd == TRUE);
			return true;
	} else {
		this->_lastErrorCode = GetLastError();
		this->_lastError = CErrorFmt::FormatError(this->_lastErrorCode, "UpdateDriver");
		if (this->_lastErrorCode == ERROR_NO_SUCH_DEVINST) {
			this->_lastError += " [Device not connected.]";
			return true;
		}
		else if (this->_lastErrorCode == ERROR_NO_MORE_ITEMS) {
			this->_lastError += " [No better driver found.]";
			return true;
		}
		return false;
	}

}
