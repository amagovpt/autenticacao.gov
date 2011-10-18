//
// $Id: PJSysInfo.h,v 1.8 2008/08/21 06:40:51 DH Exp $
//

/*
 {##
  @PROJECT_NAME             System information unit
  @PROJECT_DESC             A component and routines that provide information
                            about the user's computer and operating system.
  @FILE                     PJSysInfo.pas
  @COMMENTS                 Component and public routines source file.
  @AUTHOR                   Peter Johnson, LLANARTH, Ceredigion, Wales, UK
  @EMAIL                    peter.johnson@openlink.org
  @WEBSITE                  http://www.delphidabbler.com/
  @COPYRIGHT                © Peter D Johnson, 2001-2003.
  @LEGAL_NOTICE             This component, related component and property
                            editors and source code are placed in the public
                            domain. They may be freely copied and circulated on
                            a not-for-profit basis providing that the code is
                            unmodified and this notice and information about the
                            author and his copyright remains attached to the
                            source code.
 }
 */

#ifndef __PJSYSINFO__
#define __PJSYSINFO__

#pragma once

#undef UNICODE

#include <windows.h>
#include <lmcons.h>
#include <shlobj.h>
#include <string>

using namespace std;

#include "Registry.h"


#define IsOsVersionInfoEx \
  (this->_OSV.dwOSVersionInfoSize == sizeof(OSVERSIONINFOEX) ? true : false)

/*{
TPJOSPlatform:
  Enumerated type for OS platforms
}*/
enum CPJOSPlatform {
  ospUnknow,      // unknown platform
  ospWinNT,       // Windows NT platform
  ospWin9x,       // Windows 9x platform
  ospWin32s       // Win32s platform
};

/*{
TPJOSProduct:
  Enumerated type indentifying OS product
}*/
enum CPJOSProduct {
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
  OsWinSvr2008,
  osWin7			// Windows 7
};

// Lookup table of product names
static const char *cOSProductNames[] = {
    "Unknown OS",
    "Unknown Windows NT Release",
    "Windows NT",
    "Windows 2000",
    "Windows XP",
    "Unknown Windows 9x Release",
    "Windows 95",
    "Windows 98",
    "Windows 98SE",
    "Windows Me",
    "Win32s",
    "Windows Server 2003",
    "Windows Vista",
    "Windows 2008",
	"Windows Server 2008",
	"Windows 7"
  };


/*{
CPJSysInfo:
  Class that provides system information.
}*/
class CPJSysInfo {
public:
  CPJSysInfo();
  virtual ~CPJSysInfo(); 
  string CommonFilesFolder()    { return GetCommonFilesFolder(); };
    //{Fully qualified name of common files folder}
  string ComputerName()         { return GetComputerName(); };
    //{Name of computer}
  int OSBuildNumber()           { return GetOSBuildNumber(); };
    //{Operating system build number}
  string OSDesc()               { return GetOSDesc(); };
    //{Full description of operating system: included product name, suite and
    // build numbers as applicable}
  int OSMajorVersion()          { return GetOSMajorVersion(); };
    //{Major version number of operating system}
  int OSMinorVersion()          { return GetOSMinorVersion(); };
    //{Minor version number of operating system}
  CPJOSPlatform OSPlatform()    { return GetOSPlatformEnum(); };
    //{Operating system platform identifier}
  CPJOSProduct OSProduct()      { return GetOSProductEnum(); };
    //{Operating system product identifier}
  string OSProductName()        { return GetOSProductName(); };
    //{Name of operating system}
  string OSProductType()        { return GetOSProductType(); };
    //{Type of operating system - for NT. Always empty string for Win9x}
  string OSServicePack()        { return GetOSServicePack(); };
    //{Name of any service pack for NT or additional product info for Win9x}
  string ProgramFilesFolder()   { return GetProgramFilesFolder(); };
    //{Fully qualified name of program files folder}
  string SystemFolder()         { return GetSystemFolder(); };
    //{Fully qualified name of Windows system folder}
  string TempFolder()           { return GetTempFolder(); };
    //{Fully qualified name of current temporary folder}
  string UserName()             { return this->_GetUserName(); };
    //{Name of currently logged on user}
  string WindowsFolder()        { return GetWindowsFolder(); };
    //{Fully qualified name of Windows folder}
  string DesktopFolder()        { return GetDesktopFolder(); };
  string DefaultLanguage()      { return GetDefaultLanguage(); };
private:
  // properties
  string GetSystemFolder();
  string GetTempFolder();
  string GetWindowsFolder();
  string GetComputerName();
  string GetOSDesc();
  string _GetUserName();
  int GetOSBuildNumber();
  int GetOSMajorVersion();
  int GetOSMinorVersion();
  CPJOSPlatform GetOSPlatformEnum();
  string GetOSProductName();
  string GetOSProductType();
  string GetOSServicePack();
  string GetCommonFilesFolder();
  string GetProgramFilesFolder();
  string GetDefaultLanguage();
  // utility
  string GetCurrentVersionRegStr(const string& ValName);
  bool GetOsVerInfo();
  CPJOSProduct GetOSProductEnum();
  string GetOsProductType();

  // internal data
  OSVERSIONINFOEX _OSV;
  string GetDesktopFolder(void);
};

#endif __PJSYSINFO__
