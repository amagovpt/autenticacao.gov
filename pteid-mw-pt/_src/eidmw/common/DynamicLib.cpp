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
#include "DynamicLib.h"

using namespace eIDMW;

CDynamicLib::CDynamicLib()
{
	m_module = NULL;
}

CDynamicLib::~CDynamicLib()
{
	if (m_module != NULL)
		Close();
}

unsigned long CDynamicLib::Open(const std::string & csLibPath)
{
	if (m_module != NULL)
		Close();

	return PlatformOpen(csLibPath.c_str());
}

void * CDynamicLib::GetAddress(const std::string & csFunctionName)
{
	if (m_module != m_module)
		return NULL;

	return PlatformGetAddress(csFunctionName.c_str());
}

void CDynamicLib::Close()
{
	if (m_module != m_module)
		return PlatformClose();

	m_module = NULL;
}

///////////////////////////////// Windows ////////////////////////

#ifdef WIN32

#include <windows.h>

unsigned long CDynamicLib::PlatformOpen(const char * csLibPath)
{
	m_module = (void *) LoadLibraryA(csLibPath);

	return (m_module == NULL) ? EIDMW_CANT_LOAD_LIB : EIDMW_OK;
}

void * CDynamicLib::PlatformGetAddress(const char * csFunctionName)
{
	return GetProcAddress((HMODULE) m_module, csFunctionName);
}

void CDynamicLib::PlatformClose()
{
	FreeLibrary((HMODULE) m_module);
}


#endif

////////////////////////////////// Linux /////////////////////////

#ifdef USING_DL_OPEN

#include <dlfcn.h>

unsigned long CDynamicLib::PlatformOpen(const char * csLibPath)
{
	m_module = dlopen(csLibPath, RTLD_NOW);

	return m_module == NULL ? EIDMW_CANT_LOAD_LIB : EIDMW_OK;
}

void * CDynamicLib::PlatformGetAddress(const char * csFunctionName)
{
	char csSymName[4096];

	csSymName[0] = '_';
	csSymName[1] = '\0';
	strncat(csSymName, csFunctionName, sizeof(csSymName) - 2);
	csSymName[sizeof(csSymName) - 1] = '\0';

	return dlsym(m_module, csFunctionName);
}

void CDynamicLib::PlatformClose()
{
	dlclose(m_module);
}

#endif

/////////////////////////////////// Mac //////////////////////////

#ifdef __APPLE__

#include <Carbon/Carbon.h>
#include <mach-o/dyld.h>

unsigned long CDynamicLib::PlatformOpen(const char * csLibPath)
{
	m_module = (struct mach_header *) NSAddImage(csLibPath,
		NSADDIMAGE_OPTION_WITH_SEARCHING);

	return m_module == NULL ? EIDMW_CANT_LOAD_LIB : EIDMW_OK;
}

void * CDynamicLib::PlatformGetAddress(const char * csFunctionName)
{
	char csSymName[4096];

	csSymName[0] = '_';
	csSymName[1] = '\0';
	strncat(csSymName, csFunctionName, sizeof(csSymName) - 2);
	csSymName[sizeof(csSymName) - 1] = '\0';

	NSSymbol nssym = NSLookupSymbolInImage(
		(const struct mach_header *) m_module, csSymName,
		NSLOOKUPSYMBOLINIMAGE_OPTION_BIND_NOW);

	return nssym == NULL ? NULL : NSAddressOfSymbol(nssym);
}

void CDynamicLib::PlatformClose()
{
	// It seems unpossible to unload a library on Mac OS X
	// http://lists.apple.com/archives/Carbon-development/2004/Mar/msg00250.html
}

#endif

