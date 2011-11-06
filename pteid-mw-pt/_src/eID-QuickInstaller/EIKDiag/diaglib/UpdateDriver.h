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
#ifndef __UPDATEDRIVER__
#define __UPDATEDRIVER__

#include "stdafx.h"

#define _SETUP_APIVER 0x0500
#include <SetupApi.h>
#include <newdev.h>
#include <cfgmgr32.h>
#include "ErrorFmt.h"

typedef DWORD (WINAPI *CMP_WAITNOPENDINGINSTALLEVENTS_PROC)(DWORD time_out); 

class CUpdateDriver {
public:
    CUpdateDriver();
    virtual ~CUpdateDriver(void);

	bool PrepareDriver(string infFilePath);
	bool UpdateDriver(HWND hwndParent, string hardwareID, string infFilePath, DWORD flags, bool *bootRequired);
	bool IsDeviceInstallInprogress(DWORD dwTimeOutMillis);

    string get_LastError()              { return this->_lastError; }
    unsigned int get_LastErrorCode()    { return this->_lastErrorCode; }

protected:
private:
	bool CopyInf(string& infFilePath);
    string _lastError;
    unsigned int _lastErrorCode;
};


#endif __UPDATEDRIVER__
