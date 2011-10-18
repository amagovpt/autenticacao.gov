/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2010 FedICT.
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
#include "ezWorker.h"

#include "Platform.h"
#ifdef WIN32
#include "ServiceControl.h"
#include "Registry.h"
#include "DeviceInfolist.h"
#include "UpdateDriver.h"
#endif
#include "SysDiagnost.h"


#include <sys/stat.h>

#include <fstream>
#include <string>
using namespace std;


class systemClass
{
private:
		ezWorker ezw;	
        CSysDiagnost _SD;
        string _errorMsg;

#ifdef WIN32    // install devices ...
		CDeviceInfoList lDevList;
		CUpdateDriver ud;
#endif

public:

	systemClass(void);
	virtual ~systemClass(void);
    // AuthSign error msg
    string& ErrorMsg() { return this->_errorMsg; }
    
	// about system and hardware
	string getSystemInfo(string inputXml);
	string usbMainHubAvailable(string inputXml);
	string isAdministrator(string inputXml);
	bool reboot(void) {return _SD.reboot();}
	bool isRebootNeeded(void) {return _SD.isRebootNeeded();}

#ifdef WIN32
	bool isMiddlewareLocked(void) {return _SD.isMiddlewareLocked();};
	bool IsUserAdmin(void) {return _SD.IsUserAdmin();};
	string regCerts(string inputXml);
#elif __APPLE__
	string pcscWaitForCardReaders(string inputXml);
#endif
	bool is64bitOS();

	// about pcsc
	string pcscContextIsAvailable(string inputXml); 
	string pcscEnumerateCardReaders(string inputXml);

	// about smartcardreaders
	string smartCardReaderInstallFilesFound (string inputXml); 
	string canSeeSmartCardReaderAsUsbDevice (string inputXml); 

	// about files
	string fileExists (string inputXml);
	string deleteFile (string inputXml); 
    string deleteFolder (string inputXml);
	string fileIsLocked (string inputXml); 
	string versionInfo (string inputXml); 

	// about processes
	string findRunningProcess (string inputXml); 
	string libraryIsLoaded (string inputXml); 
	string killProcess (string inputXml); 
	string startProcess (string inputXml); 
	string getLibraryDependencies (string inputXml);

	string killProcess (const char *ProcessName);

#ifdef WIN32

	// about services or deamons
	string isServiceRunning (string inputXml); 
	string startService (string inputXml); 
	string stopService (string inputXml);
	string removeService (string inputXml);

	string stopService (const char *ServiceName);

    // about Registry
	string readRegistryEntry(string inputXml);
	string writeRegistryEntry (string inputXml);
	string deleteRegistryEntry (string inputXml);

	// about devices
	string getdevicesList (string inputXml);
	string isDevicePresent (string inputXml);
	string installDevice (string inputXml, string myOS);
	bool IsDeviceInstallInprogress (DWORD dwTimeOutMillis);
#endif

	// about installed software
	string getSoftwareList(string inputXml);
	string isSoftwareInstalled (string inputXml);
#ifdef WIN32
    string WaitForUninstall(string inputXml);
#endif

    // about using cards
	string readCard(string inputXml);
	string AuthSign(string inputXml);



};