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
/****************************************************************************************************/

#include "globmdrv.h"

#include "log.h"
#include "util.h"

/****************************************************************************************************/

#define WHERE "DllMain"
BOOL WINAPI DllMain(HINSTANCE    hinstDLL,   // handle to DLL module
	DWORD        fdwReason,  // reason for calling function
	LPVOID       lpReserved) // reserved
{
	// Perform actions based on the reason for calling.
	switch (fdwReason) 
	{ 
	case DLL_PROCESS_ATTACH:
		// Initialize once for each new process.
		// Return FALSE to fail DLL load.
		LogInit();
		DisableThreadLibraryCalls(hinstDLL);

		LogTrace(LOGTYPE_INFO, WHERE, "DLL_PROCESS_ATTACH...");
		break;

	case DLL_THREAD_ATTACH:
		// Do thread-specific initialization.
		LogTrace(LOGTYPE_INFO, WHERE, "DLL_THREAD_ATTACH...");
		break;

	case DLL_THREAD_DETACH:
		// Do thread-specific cleanup.
		LogTrace(LOGTYPE_INFO, WHERE, "DLL_THREAD_DETACH...");
		break;

	case DLL_PROCESS_DETACH:
		// Perform any necessary cleanup.
		LogTrace(LOGTYPE_INFO, WHERE, "DLL_PROCESS_DETACH...");
		break;
	default:
		LogTrace(LOGTYPE_INFO, WHERE, "Unknown Reason [%d]", fdwReason);
		break;
	}

	return TRUE;  // Successful DLL_PROCESS_ATTACH.
}
#undef WHERE

