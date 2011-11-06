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
/**
 * Platform-independent loading of dynamic libraries
 * (.DLL on Windows, .so on Linux, .dylib or .so on Mac OS X).
 */

#ifndef __DYNAMICLIB_H__
#define __DYNAMICLIB_H__

#include "eidErrors.h"
#include "Export.h"
#include <string.h>
#include <string>

namespace eIDMW
{

class EIDMW_CMN_API CDynamicLib
{
public:
	CDynamicLib();
	~CDynamicLib();

	/** Load the dynamic lib called 'csLibPath' */
	unsigned long Open(const std::string & csLibPath);
	/** Get a pointer to function 'csFuntionName' in the currently loaded lib */
	void * GetAddress(const std::string & csFunctionName);
	/** Un-load the currently loaded lib (if one is loaded) */
	void Close();

private:
    // No copies allowed
    CDynamicLib(const CDynamicLib & oDynLib);
    CDynamicLib & operator = (const CDynamicLib & oDynLib);

	// Platform-dependent implementations
	unsigned long PlatformOpen(const char * csLibPath);
	void * PlatformGetAddress(const char * csFunctionName);
	void PlatformClose();

	void *m_module;
};

}

#endif
