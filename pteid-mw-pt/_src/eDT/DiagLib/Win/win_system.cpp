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
#include <windows.h>

#include "system.h"
#include "error.h"
#include "log.h"

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PRIVATE FUNCTIONS DECLARATION ////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);

typedef enum e_Platform {
  ospUnknow,      // unknown platform
  ospWinNT,       // Windows NT platform
  ospWin9x,       // Windows 9x platform
  ospWin32s       // Win32s platform
} Platform;

typedef enum e_OSProduct {
  osUnknownOs,      // unknown OS
  osUnknownWinNT,   // Unknown Windows NT OS
  osWinNT,          // Windows NT (up to v4)
  osWin2K,          // Windows 2000
  osWinXP,          // Windows XP
  osUnknownWin9x,   // Unknown Windows 9x OS
  osWin95,          // Windows 95
  osWin98,          // Windows 98
  osWin98SE,        // Windows 98
  osWinMe,          // Windows Me
  osUnknownWin32s,  // Unknown OS running Win32s
  osWinSvr2003,     // Windows Server 2003
  osWinVista,
  OsWin2008,
  OsWinSvr2008,		// Windows Server 2008
  osWin7			// Windows 7
} OSProduct;

static const wchar_t *g_OSProductNames[] = {
    L"Unknown OS",
    L"Unknown Windows NT Release",
    L"Windows NT",
    L"Windows 2000",
    L"Windows XP",
    L"Unknown Windows 9x Release",
    L"Windows 95",
    L"Windows 98",
    L"Windows 98SE",
    L"Windows Me",
    L"Win32s",
    L"Windows Server 2003",
    L"Windows Vista",
    L"Windows 2008",
	L"Windows Server 2008",
	L"Windows 7"
};

int systemGetLang(std::wstring *lang);

Platform GetOSPlatformEnum(const OSVERSIONINFOEX &osv) ;
OSProduct GetOSProductEnum(const OSVERSIONINFOEX &osv);
std::wstring GetOSProductType(const OSVERSIONINFOEX &osv);
std::wstring GetOSDesc(const OSVERSIONINFOEX &osv);
std::wstring GetOSServicePack(const OSVERSIONINFOEX &osv);
std::wstring GetOSArchitecture();

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PUBLIC FUNCTIONS /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
int systemGetInfo(System_INFO *info)
{
	int iReturnCode = DIAGLIB_OK;

	LOG_TIME(L"Ask for system info --> ");

	info->OsType=L"Windows";

	OSVERSIONINFOEX osv;
	ZeroMemory(&osv,sizeof(OSVERSIONINFOEX));
	osv.dwOSVersionInfoSize=sizeof(OSVERSIONINFOEX);
	// We first try to get exended information
	if (!::GetVersionEx(reinterpret_cast<OSVERSIONINFO *>(&osv))) 
	{
		// We failed to get extended info: try again with old structure
		OSVERSIONINFO	osv_old;
		osv_old.dwOSVersionInfoSize=sizeof(OSVERSIONINFO);
		if (! ::GetVersionEx(&osv_old)) 
		{
			LOG_LASTERROR(L"GetVersionEx failed");
			return RETURN_LOG_INTERNAL_ERROR;
		}
		else
		{
			CopyMemory(&osv,&osv_old,sizeof(OSVERSIONINFO));
			osv.wProductType=-1;
		}
	}

	wchar_t buff[25];
	if( -1 == swprintf_s(buff,25,L"%d",osv.dwMajorVersion))
	{
		LOG_ERRORCODE(L"swprintf_s failed",errno);
		return RETURN_LOG_INTERNAL_ERROR;
	}
	else
	{
		info->MajorVersion.assign(buff);
	}
	if( -1 == swprintf_s(buff,25,L"%d",osv.dwMinorVersion))
	{
		LOG_ERRORCODE(L"swprintf_s failed",errno);
		return RETURN_LOG_INTERNAL_ERROR;
	}
	else
	{
		info->MinorVersion.assign(buff);
	}
	if( -1 == swprintf_s(buff,25,L"%d",osv.dwBuildNumber & 0x0000FFFF))
	{
		LOG_ERRORCODE(L"swprintf_s failed",errno);
		return RETURN_LOG_INTERNAL_ERROR;
	}
	else
	{
		info->BuildNumber.assign(buff);
	}
	info->PlatformId=osv.dwPlatformId;
	info->ProductType=osv.wProductType;

	info->ProductName=g_OSProductNames[GetOSProductEnum(osv)];
	info->ServicePack=GetOSServicePack(osv);
	info->Description=GetOSDesc(osv);
	info->Architecture=GetOSArchitecture();

	systemGetLang(&info->DefaultLanguage);

	LOG(L"DONE\n");

	return iReturnCode;
} 

////////////////////////////////////////////////////////////////////////////////////////////////
int systemReboot (void)
{
	int iReturnCode = DIAGLIB_OK;

	HANDLE hToken; 
	TOKEN_PRIVILEGES tkp; 
 
	// Get a token for this process. 
	if(!OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) 
	{
		LOG_LASTERROR(L"OpenProcessToken failed");
		return DIAGLIB_ERR_INTERNAL;
	}
 
	// Get the LUID for the shutdown privilege. 
	if(!LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid)) 
 	{
		LOG_LASTERROR(L"LookupPrivilegeValue failed");
		return DIAGLIB_ERR_INTERNAL;
	}

	tkp.PrivilegeCount = 1;  // one privilege to set    
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; 
 
	// Get the shutdown privilege for this process. 
	if(!AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0))
 	{
		LOG_LASTERROR(L"AdjustTokenPrivileges failed");
		return DIAGLIB_ERR_INTERNAL;
	}
	else if (GetLastError() != ERROR_SUCCESS) 
 	{
		LOG_LASTERROR(L"AdjustTokenPrivileges failed for some privilege");
		return DIAGLIB_ERR_INTERNAL;
	}
 
   // Shut down the system and force all applications to close. 
 
	if (!ExitWindowsEx(EWX_REBOOT | EWX_FORCE, 
               SHTDN_REASON_MAJOR_APPLICATION |
               SHTDN_REASON_MINOR_INSTALLATION |
               SHTDN_REASON_FLAG_PLANNED)) 
 	{
		LOG_LASTERROR(L"ExitWindowsEx failed");
		return DIAGLIB_ERR_INTERNAL;
	}

	return iReturnCode;
} 

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PRIVATE FUNCTIONS ////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
int systemGetLang(std::wstring *lang)
{
	int iReturnCode = DIAGLIB_OK;

	if(lang == NULL)
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;
	}
	
	lang->clear();

	wchar_t buff[6];
	bool bAddSubLang=true;

	int PrimaryLang=PRIMARYLANGID(GetSystemDefaultLangID());
	int SubLang=SUBLANGID(GetSystemDefaultLangID());

	if(SubLang==0x01) bAddSubLang=false;

	switch (PrimaryLang) 
	{
	case LANG_DUTCH:	
		lang->append(L"Nl"); 
		if(SubLang == SUBLANG_DUTCH_BELGIAN)
		{
			lang->append(L"-be"); bAddSubLang=false;
		}
		break;
	case LANG_FRENCH:	
		lang->append(L"Fr"); 
		if(SubLang == SUBLANG_FRENCH_BELGIAN)
		{
			lang->append(L"-be"); bAddSubLang=false;
		}
		break;
	case LANG_ENGLISH: 
		lang->append(L"En"); 
		if(SubLang == SUBLANG_ENGLISH_UK)
		{
			lang->append(L"-uk"); bAddSubLang=false;
		}
		break;
	case LANG_GERMAN:	
		lang->append(L"De"); 
		break;
	default :
		if( -1 == swprintf_s(buff, 6, L"%x-%x", PrimaryLang, SubLang))
		{
			LOG_ERRORCODE(L"Failed to get language",errno);
			return RETURN_LOG_INTERNAL_ERROR;
		}
		else
		{
			lang->append(buff);
		}
		bAddSubLang=false;
		break;
    }

	if(bAddSubLang)
	{
		if( -1 == swprintf_s(buff, 6, L"-%x", SubLang))
		{
			LOG_ERRORCODE(L"Failed to get language",errno);
			return RETURN_LOG_INTERNAL_ERROR;
		}
		else
		{
			lang->append(buff);
		}
    }

	return iReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
Platform GetOSPlatformEnum(const OSVERSIONINFOEX &osv) 
{
	//{Returns OS platform }
	switch(osv.dwPlatformId) 
	{
    case VER_PLATFORM_WIN32_NT:
        return ospWinNT;
    case VER_PLATFORM_WIN32_WINDOWS:
        return ospWin9x;
    case VER_PLATFORM_WIN32s:
		return ospWin32s;
    default:
        return ospUnknow;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////
OSProduct GetOSProductEnum(const OSVERSIONINFOEX &osv) 
{
	//{Returns code identifying OS product from OS ver info structure}

    switch (osv.dwPlatformId) 
	{
	case VER_PLATFORM_WIN32_NT:
		// We have an NT OS
		if (osv.dwMajorVersion <= 4) 
			return osWinNT;
		else if ((osv.dwMajorVersion == 5) && (osv.dwMinorVersion == 0))
			return osWin2K;
		else if ((osv.dwMajorVersion == 5) && (osv.dwMinorVersion == 1))
			return osWinXP;
		else if ((osv.dwMajorVersion == 5) && (osv.dwMinorVersion == 2))
			return osWinSvr2003;
		else if ((osv.dwMajorVersion == 6) && (osv.dwMinorVersion == 0) && (osv.wProductType == 1))
			return osWinVista;
		else if ((osv.dwMajorVersion == 6) && (osv.dwMinorVersion == 0) && (osv.wProductType == 3))
			return OsWin2008;
		else
			return osUnknownWinNT; 

	case VER_PLATFORM_WIN32_WINDOWS:
		// We have a Win 95 line OS
		if ((osv.dwMajorVersion == 4) && (osv.dwMinorVersion == 0))
			return osWin95;
		else if ((osv.dwMajorVersion == 4) && (osv.dwMinorVersion == 10)) 
		{
			if (osv.szCSDVersion [1] == L'A') 
				return osWin98SE;  
			else 
				return osWin98;  
		}
		else if ((osv.dwMajorVersion == 4) && (osv.dwMinorVersion == 90))
			return osWinMe;
		else
			return osUnknownWin9x;
		break;
	
	default: 
		// This is a Win32s enabled OS
		return osUnknownWin32s;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////
std::wstring GetOSProductType(const OSVERSIONINFOEX &osv) 
{
    //{Return type of OS product from given OS ver info structure. IsOSVersionInfoEx
    //indicates if OSV is an extended structure (true) or is older style structure
    //(false). We only return a value for NT}

    switch (osv.dwPlatformId) 
	{
	case VER_PLATFORM_WIN32_NT:
		// This is an NT OS
        // check  product type
        if (osv.wProductType != -1) 
		{
			// we have extended OS info: product type info is in here
			if (osv.wProductType == VER_NT_WORKSTATION) 
			{
				if (osv.dwMajorVersion == 4)
					return L"Workstation";
				else if ((osv.wSuiteMask & VER_SUITE_PERSONAL) != 0)
					return L"Home Edition";
				else
					return L"Professional";
			}
			else if (osv.wProductType == VER_NT_SERVER) 
			{
				// This is an NT server OS
				if ((osv.dwMajorVersion == 5) && (osv.dwMinorVersion == 2)) 
				{
					// Windows Serer 2003
					if ((osv.wSuiteMask & VER_SUITE_DATACENTER) != 0) 
                		return L"Datacenter Edition";
					else if ((osv.wSuiteMask && VER_SUITE_ENTERPRISE) != 0)
                		return L"Enterprise Edition";
					else if (osv.wSuiteMask == VER_SUITE_BLADE)
                		return L"Web Edition";
					else
                		return L"Standard Edition";
				}
				else if ((osv.dwMajorVersion == 5) && (osv.dwMinorVersion == 0)) 
				{
					// Windows 2000 Server
					if ((osv.wSuiteMask & VER_SUITE_DATACENTER) != 0)
                		return L"Datacenter Server";
					else if ((osv.wSuiteMask & VER_SUITE_ENTERPRISE) != 0)
                		return L"Advanced Server";
					else
                		return L"Server";
				} 
				else 
				{
					// Windows NT 4.0
					if ((osv.wSuiteMask & VER_SUITE_ENTERPRISE) != 0)
                		return L"Server, Enterprise Edition";
					else
                		return L"Server";
				}
			}
        } 
		else 
		{ // (! IsVersionInfoEx)
          // we have not got extended OS info: read product type from registry
			HKEY RegKey;                       // registry key
			BYTE ProductType[80];              // buffer to store product type from reg
			DWORD BufLen;                      // length of ProductType buffer

			if(ERROR_SUCCESS == RegOpenKeyEx( HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Control\\ProductOptions",0, KEY_QUERY_VALUE, &RegKey ))
			{
				RegQueryValueEx(RegKey, L"ProductType", NULL, NULL,ProductType, &BufLen);
				RegCloseKey(RegKey);
				const std::wstring wsValue((const wchar_t*)ProductType, (size_t) (BufLen/2));  //convert byte to double byte

				if (_wcsicmp(L"WINNT", wsValue.c_str()) == 0)
					return L"Professional";
				if (_wcsicmp(L"LANMANNT", wsValue.c_str()) == 0)
					return L"Server";
				if (_wcsicmp(L"SERVERNT", wsValue.c_str()) == 0)
					return L"Advanced Server";
			}
		}  // IsVersionInfoEx
	} // switch

	return L"";

}

////////////////////////////////////////////////////////////////////////////////////////////////
std::wstring GetOSServicePack(const OSVERSIONINFOEX &osv) 
{
  //{Returns name of any service pack associated with OS (if NT) or additional
  // product info (if Win9x)}

	HKEY RegKey; // registry key

	std::wstring Result = L"";

    switch ( osv.dwPlatformId ) 
	{
    case VER_PLATFORM_WIN32_NT:
        // An NT OS
        if ((osv.dwMajorVersion == 4) && (_wcsicmp(osv.szCSDVersion, L"Service Pack 6") == 0)) 
		{
			// Special test for SP6 versus SP6a.
			if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE,
												L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Hotfix\\Q246009",
												0,
												KEY_QUERY_VALUE,
												&RegKey))
			{
				// Windows NT4 SP6a
				Result = L"Service Pack 6a";
				RegCloseKey(RegKey);
			}
			else
			{
				// Window NT4 earlier than SP6a
				Result = osv.szCSDVersion;
			}
        } 
		else 
		{
			Result = osv.szCSDVersion;
        }
        break;
	case VER_PLATFORM_WIN32_WINDOWS:
        // A Win9x OS
        if (osv.dwMajorVersion == 4) 
		{
			if ((osv.dwMinorVersion == 0) && ((osv.szCSDVersion[1] == L'C') || (osv.szCSDVersion[1] == L'B')))
			{
				Result = L"OSR2";
			}
			else if ((osv.dwMinorVersion == 10) && (osv.szCSDVersion[1] == 'A')) 
			{
				Result = L"SE";
			}
		}
	} // switch

	return Result;
}

////////////////////////////////////////////////////////////////////////////////////////////////
std::wstring GetOSDesc(const OSVERSIONINFOEX &osv) 
{
	//{Read access method for OSDesc property}

	std::wstring Result = L"";
	std::wstring s = L"";
	wchar_t tmp[128];

	Result = g_OSProductNames[GetOSProductEnum(osv)];
	switch ( osv.dwPlatformId ) 
	{
		case VER_PLATFORM_WIN32_NT: {
		// We have an NT OS
		// check for product type
		s = GetOSProductType(osv);
		if (!s.empty())
		{
			Result.append(L" ");
			Result.append(s);
		}
		// display version, service pack (if any), and build number.
		if (osv.dwMajorVersion <= 4) 
		{
			swprintf_s(tmp, 128, L"version %d.%d %ls (Build %d)",
									osv.dwMajorVersion,
									osv.dwMinorVersion,
									GetOSServicePack(osv).c_str(),
									osv.dwBuildNumber & 0x0000FFFF);
			Result.append(L" ");
			Result.append(tmp);
		} 
		else 
		{
			swprintf_s(tmp, 128, L"%ls (Build %d)",
									GetOSServicePack(osv).c_str(),
									osv.dwBuildNumber & 0x0000FFFF);
			Result.append(L" ");
			Result.append(tmp);
		}
		break;
    }
	case VER_PLATFORM_WIN32_WINDOWS:  
		// We have a Win 95 line OS
		s = GetOSServicePack(osv);
		if (!s.empty())
		{
			Result.append(L" ");
			Result.append(s);
		}
		break;
	}

	return Result;
}

std::wstring GetOSArchitecture()
{
	LPFN_ISWOW64PROCESS fnIsWow64Process;
    BOOL bIsWow64 = FALSE;
	std::wstring Result = L"";

#ifdef WIN64
	Result = L"64 bit application";
#elif WIN32
    fnIsWow64Process = (LPFN_ISWOW64PROCESS) GetProcAddress(
        GetModuleHandle(TEXT("kernel32")),"IsWow64Process");

    if (NULL != fnIsWow64Process)
    {
        if (!fnIsWow64Process(GetCurrentProcess(),&bIsWow64))
        {
            LOG_LASTERROR(L"fnIsWow64Process failed");
			Result = L"Could not determine architecture";
        }
		if(bIsWow64)
		{
			Result = L"32 bit application running on 64 bit Windows";
		}
		else
		{
			Result = L"32 bit application running on 32 bit Windows";
		}
    }
	else
	{
		Result = L"32 bit application running on 32 bit Windows";
	}
	
#endif
    return Result;
}
