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
#include "mac_helper.h"
#include <string.h>
#include <iostream>
#include "util.h"
#include "error.h"
#include "log.h"
#include "report.h"
#include "progress.h"
#include <CoreFoundation/CoreFoundation.h>
#include <ApplicationServices/ApplicationServices.h>
#include "sys/stat.h"
#include <dlfcn.h>

////////////////////////////////////////////////////////////////////////////////////////////////
long GetLastError()
{
	return errno;
}

////////////////////////////////////////////////////////////////////////////////////////////////
std::string string_From_wstring(std::wstring const& in)
{
    std::string out(in.size(), 0);
	// OS X uses 32-bit wchar
	const int bytes = in.length() * sizeof(wchar_t);
	// comp_bLittleEndian is in the lib I use in order to detect PowerPC/Intel
#ifdef __LITTLE_ENDIAN__
	CFStringEncoding encoding = kCFStringEncodingUTF32LE;
#else
	CFStringEncoding encoding =  kCFStringEncodingUTF32BE;
#endif
	CFStringRef str = CFStringCreateWithBytes(NULL, (const UInt8*)in.c_str(), bytes, encoding, false);

	const int bytesUtf8 = CFStringGetMaximumSizeOfFileSystemRepresentation(str);
	char *utf8 = new char[bytesUtf8];
	CFStringGetFileSystemRepresentation(str, utf8, bytesUtf8);
	CFRelease(str);
	out.assign(utf8);
	delete[] utf8;
	return out;
}

////////////////////////////////////////////////////////////////////////////////////////////////
int _wfopen_s(FILE** pFile, const wchar_t *filename, const wchar_t *mode)
{
	int r = 0;

	if (pFile == NULL)
		return -1;

	std::wstring wFileName=filename;
	std::wstring wMode=mode;
	FILE *f = fopen(string_From_wstring(wFileName).c_str(), string_From_wstring(wMode).c_str());

	if (f != NULL)
		*pFile = f;
	else
		r = errno;

	return r;
}

////////////////////////////////////////////////////////////////////////////////////////////////
int fwprintf_s(FILE *stream, const wchar_t *format, ...)
{
	va_list args;
	char *csTmp = NULL;
	int r = -1;

	va_start(args, format);
	std::wstring wFormat=format;
	r = vasprintf(&csTmp, string_From_wstring(wFormat).c_str(), args);
	va_end(args);

	if (r != -1 && csTmp != NULL)
	{
		r = fprintf(stream, csTmp);
		free(csTmp);
	}

	return r;
}

////////////////////////////////////////////////////////////////////////////////////////////////
int vfwprintf_s(FILE *stream, const wchar_t *format, va_list argptr)
{
	char *csTmp = NULL;
	int r = -1;

	std::wstring wFormat=format;
	r = vasprintf(&csTmp, string_From_wstring(wFormat).c_str(), argptr);

	if (r != -1 && csTmp != NULL)
	{
		r = fprintf(stream, csTmp);
		free(csTmp);
	}

	return r;
}

////////////////////////////////////////////////////////////////////////////////////////////////
int strcpy_s(char *dest, size_t len, const char *src)
{
	if (dest == NULL)
		return -1;

	for ( ; len > 1 && *src != '\0'; dest++, src++, len--)
		*dest = *src;

	*dest = '\0';

	return *src == '\0' ? 0 : -1; // 0: OK, -1: NOK
}

////////////////////////////////////////////////////////////////////////////////////////////////
int wcscpy_s(wchar_t *dest, size_t len, const wchar_t *src)
{
	if (dest == NULL)
		return -1;

	for ( ; len > 1 && *src != L'\0'; dest++, src++, len--)
		*dest = *src;

	*dest = L'\0';

	return *src == L'\0' ? 0 : -1; // 0: OK, -1: NOK
}

////////////////////////////////////////////////////////////////////////////////////////////////
int _wstat(const wchar_t *filename, struct stat *buffer)
{
	std::wstring wFilename=filename;

	return stat(string_From_wstring(wFilename).c_str(),buffer);
}

////////////////////////////////////////////////////////////////////////////////////////////////
int swprintf_s(wchar_t *buffer, size_t sizeOfBuffer, const wchar_t *format, ...)
{
	va_list args;
	char *csTmp = NULL;
	int r = -1;

	va_start(args, format);
	std::wstring wFormat=format;
	r = vasprintf(&csTmp, string_From_wstring(wFormat).c_str(), args);
	va_end(args);

	if (r != -1 && csTmp != NULL)
	{
		std::string sTmp=csTmp;
		r = wcscpy_s(buffer, sizeOfBuffer, wstring_From_string(sTmp).c_str());
		free(csTmp);
	}

	return r;
}

////////////////////////////////////////////////////////////////////////////////////////////////
std::wstring wstring_From_CFStringRef(CFStringRef const& in)
{
	unsigned long len = CFStringGetLength(in);

	char *buffer = new char[len+1];

	CFStringGetCString(in,buffer,len+1,kCFStringEncodingUTF8);
	std::string out(buffer);

	delete[] buffer;

	return wstring_From_string(out);

}

////////////////////////////////////////////////////////////////////////////////////////////////
std::string string_From_CFStringRef(CFStringRef const& in)
{
	unsigned long len = CFStringGetLength(in);

	char *buffer = new char[len+1];

	CFStringGetCString(in,buffer,len+1,kCFStringEncodingUTF8);
	std::string out(buffer);

	delete[] buffer;

	return out;
}

////////////////////////////////////////////////////////////////////////////////////////////////
void unloadResources()
{
	logUnloadResources();
	reportUnloadResources();
}

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////// System Profiler function helper /////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

#define SYSTEM_PROFILER_BUFFER_SIZE 1024*512
static char g_SystemProfilerBuffer[SYSTEM_PROFILER_BUFFER_SIZE];

int FindDictionaryForDataType(const CFArrayRef inArray, CFStringRef inDataType, CFDictionaryRef *pDictionary);

////////////////////////////////////////////////////////////////////////////////////////////////
int SystemProfilerGetDataType (std::vector<std::string> *dataTypeList)
{
	int iReturnCode = DIAGLIB_OK;

	if(dataTypeList==NULL)
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;
	}

	FILE *sys_profile;
	size_t bytesRead = 0;

	char buffer[2000];
	// popen will fork and invoke the system_profiler command and return a stream reference with its result data
	// See the Darwin man page for system_profiler for options.
	const char * command ="system_profiler -listDataTypes";
	if (NULL == (sys_profile = popen(command, "r")))
	{
		LOG_LASTERROR(L"popen failed");
		return RETURN_LOG_INTERNAL_ERROR;
	}

	// Read the stream into a memory buffer
	if(0 == (bytesRead = fread(buffer, sizeof(char), 2000, sys_profile)))
	{
		LOG_LASTERROR(L"fread failed");
		pclose (sys_profile);
		return RETURN_LOG_INTERNAL_ERROR;
	}
	buffer[bytesRead-1] = 0;

	// Close the stream
	pclose (sys_profile);

	std::vector<std::string> list;
	TokenizeS(buffer,list,"\n");

	if(list.size() <= 1)
	{
		LOG(L"system_profiler -listDataTypes failed\n");
	}
	else
	{
		progressInit(list.size());
		for (unsigned long i=1; i < list.size() ; i++)
		{
			dataTypeList->push_back(list[i]);
			progressIncrement();
		}
		progressRelease();
	}

	return iReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
int SystemProfiler (const char *dataType, CFDictionaryRef *dataTypeDictionary)
{
	int iReturnCode = DIAGLIB_OK;

	if(dataTypeDictionary==NULL)
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;
	}

	FILE *sys_profile;
	size_t bytesRead = 0;

	// popen will fork and invoke the system_profiler command and return a stream reference with its result data
	// See the Darwin man page for system_profiler for options.
	char command[50];
	sprintf(command, "system_profiler %s -xml", dataType);
	if (NULL == (sys_profile = popen(command, "r")))
	{
		LOG_LASTERROR(L"popen failed");
		return RETURN_LOG_INTERNAL_ERROR;
	}
	LOG(L"Run command: %s\n", command);

	// Read the stream into a memory buffer
	if(0 == (bytesRead = fread(g_SystemProfilerBuffer, sizeof(char), SYSTEM_PROFILER_BUFFER_SIZE, sys_profile)))
	{
		LOG_LASTERROR(L"fread failed");
		pclose (sys_profile);
		return RETURN_LOG_INTERNAL_ERROR;
	}
	g_SystemProfilerBuffer[bytesRead-1] = 0;

	// Close the stream
	pclose (sys_profile);

	CFDataRef xmlData = NULL;

	// Create a CFDataRef with the xml data
	if(NULL == (xmlData = CFDataCreate (kCFAllocatorDefault, (UInt8 *)g_SystemProfilerBuffer, bytesRead)))
	{
		LOG_ERROR(L"CFDataCreate failed");
		if(xmlData != NULL) CFRelease(xmlData);
		return  DIAGLIB_ERR_INTERNAL;
	}

	CFArrayRef SystemProfilerArray;

	// CFPropertyListCreateFromXMLData reads in the XML data and will parse it into a CFArrayRef for us.
	CFStringRef errorString;
	if(NULL == (SystemProfilerArray = (CFArrayRef)CFPropertyListCreateFromXMLData (kCFAllocatorDefault, xmlData, kCFPropertyListImmutable, &errorString)))
	{
		LOG_CFERROR(L"CFPropertyListCreateFromXMLData failed (%s)",errorString);
		if(xmlData != NULL) CFRelease(xmlData);
		if(SystemProfilerArray != NULL) CFRelease(SystemProfilerArray);
		return  DIAGLIB_ERR_INTERNAL;
	}

	if(xmlData != NULL) CFRelease(xmlData);

	// Find the CFDictionary with the key/data pair of "_dataType"/ <input dataType>
	// This will be the dictionary that contains all the information regarding
	// devices that system_profiler knows about.
	CFStringRef cfDataType;
	if(NULL == (cfDataType = CFStringCreateWithCString(kCFAllocatorDefault, dataType, kCFStringEncodingASCII)))
	{
		LOG_ERROR(L"CFStringCreateWithCString failed");
		if(SystemProfilerArray != NULL) CFRelease(SystemProfilerArray);
		if(cfDataType != NULL) CFRelease(cfDataType);
		return DIAGLIB_ERR_INTERNAL;
	}

	if(DIAGLIB_OK != (iReturnCode = FindDictionaryForDataType (SystemProfilerArray, cfDataType, dataTypeDictionary)))
	{
		LOG_ERROR(L"FindDictionaryForDataType failed");
		if(SystemProfilerArray != NULL) CFRelease(SystemProfilerArray);
		if(cfDataType != NULL) CFRelease(cfDataType);
		return DIAGLIB_ERR_INTERNAL;
	}

	if(SystemProfilerArray != NULL) CFRelease(SystemProfilerArray);
	if(cfDataType != NULL) CFRelease(cfDataType);

	return iReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
int SystemProfilerGetInfoString(CFDictionaryRef inDictionary, const char *property, std::wstring *value)
{
	int iReturnCode = DIAGLIB_OK;

	value->clear();

	CFStringRef cfValue=NULL;

	CFStringRef cfProperty = NULL;
	if(NULL == (cfProperty = CFStringCreateWithCString(kCFAllocatorDefault, property, kCFStringEncodingASCII)))
	{
		LOG_ERROR(L"CFStringCreateWithCString failed");
		if(cfProperty != NULL) CFRelease(cfProperty);
		return DIAGLIB_ERR_INTERNAL;
	}

	if(CFDictionaryGetValueIfPresent(inDictionary, cfProperty,(const void**)&cfValue))
	{
		value->assign(wstring_From_CFStringRef(cfValue));
	}

	if(cfProperty != NULL) CFRelease(cfProperty);

	return iReturnCode;

}

////////////////////////////////////////////////////////////////////////////////////////////////
int SystemProfilerGetItemsArray(CFDictionaryRef inDictionary, CFArrayRef *pItemsArray)
{
	int iReturnCode = DIAGLIB_OK;

	if(pItemsArray == NULL)
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;
	}

    // Retrieve the CFDictionary that has a key/value pair with the key equal to "_items".
	if(!CFDictionaryGetValueIfPresent(inDictionary, CFSTR("_items"),(const void**)pItemsArray))
	{
		*pItemsArray = NULL;
	}
	else
	{
		CFRetain(*pItemsArray);
	}

    return iReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
int FindDictionaryForDataType(const CFArrayRef inArray, CFStringRef inDataType, CFDictionaryRef *pDictionary)
{
	int iReturnCode = DIAGLIB_OK;

	if(pDictionary == NULL)
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;
	}

	progressInit(CFArrayGetCount(inArray));

    // Search the array of dictionaries for a CFDictionary that matches
    for (UInt8 i = 0; i<CFArrayGetCount(inArray); i++)
    {
        *pDictionary = (CFDictionaryRef)CFArrayGetValueAtIndex(inArray, i);

        // If the CFDictionary at this index has a key/value pair with the value equal to inDataType, retain and return it.
        if (!CFDictionaryContainsValue(*pDictionary, inDataType))
		{
			LOG_CFERROR(L"CFDictionaryContainsValue - Dictionnary '%s' contains no value",inDataType);
			iReturnCode = DIAGLIB_ERR_INTERNAL;
			break;
		}
		else
        {
            // Retain the dictionary.  Caller is responsible for releasing it.
            CFRetain(*pDictionary);
        }
		progressIncrement();
    }
	progressRelease();

    return iReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////// Dynamic loading library helper /////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
HMODULE LoadLibrary(const wchar_t *library)
{
	HMODULE hLib = NULL;
	if(NULL == (hLib = dlopen(string_From_wstring(library).c_str(),RTLD_LAZY)))
	{
		char *err = dlerror();
	}

	return hLib;
}

////////////////////////////////////////////////////////////////////////////////////////////////
bool FreeLibrary(HMODULE hLibrary)
{
	if(!dlclose(hLibrary))
		return true;
	else
		return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////
void *GetProcAddress(HMODULE hLibrary, const char * csFunctionName)
{
	char csSymName[4096];

	csSymName[0] = '_';
	csSymName[1] = '\0';
	strncat(csSymName, csFunctionName, sizeof(csSymName) - 2);
	csSymName[sizeof(csSymName) - 1] = '\0';

	return dlsym(hLibrary, csFunctionName);
}

////////////////////////////////////////////////////////////////////////////////////////////////
static std::string g_bundlepath="";

const char *GetBundlePath()
{
	if(g_bundlepath.length()==0)
	{
		char buffer[1024];
		ProcessSerialNumber xPSN = {kNoProcess, kCurrentProcess};
		FSRef location;
		OSStatus ret = GetProcessBundleLocation (&xPSN, &location);
		if (noErr == ret)
		{
			ret = FSRefMakePath(&location, (unsigned char*)buffer, sizeof(buffer));
			if (noErr == ret)
			{
				g_bundlepath.assign(buffer);
				g_bundlepath.append("/");
			}
		}
	}
	if(g_bundlepath.length()!=0)
	{
		LOG(L"Bundle path is %s\n",g_bundlepath.c_str());
	}

	return g_bundlepath.c_str();
}

////////////////////////////////////////////////////////////////////////////////////////////////
static std::string g_otoolpath="";

const char *GetOtoolPath()
{
	if(g_otoolpath.length()==0)
	{
		FILE *			pF;
		size_t			bytesRead = 0;
		char buffer[1024];

		pF = popen("which otool", "r");
		if (pF == NULL )
		{
			LOG_LASTERROR(L"popen failed");
		}

		// Read the stream into a memory buffer
		if(0 != (bytesRead = fread(buffer, sizeof(char), sizeof(buffer), pF)))
		{
			pclose (pF); // Close the stream
			buffer[bytesRead-1] = 0x00;
			if(strncmp(buffer,"no otool in ",12) != 0)
			{
				g_otoolpath.assign(buffer);
			}
		}
		else
		{
			pclose (pF);
		}

		if(g_otoolpath.length()==0)
		{
			g_otoolpath.assign(GetBundlePath());
			g_otoolpath.append("Contents/Resources/otool");
		}
		if(g_otoolpath.length()!=0)
		{
			LOG(L"Otool path set to %s\n",g_otoolpath.c_str());
		}
	}

	return g_otoolpath.c_str();
}