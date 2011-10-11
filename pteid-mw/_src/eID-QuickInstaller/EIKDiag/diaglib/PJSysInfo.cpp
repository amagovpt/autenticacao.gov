//
// $Id: PJSysInfo.cpp,v 1.15 2008/08/22 08:04:41 DH Exp $
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

#include "stdafx.h"

#include "PJSysInfo.h"
#include ".\pjsysinfo.h"

#pragma hdrstop

CPJSysInfo::CPJSysInfo(){

  memset(&this->_OSV, 0, sizeof(OSVERSIONINFOEX));
  if (!this->GetOsVerInfo()) {
    this->_OSV.dwOSVersionInfoSize = -1;
  }

}

CPJSysInfo::~CPJSysInfo(){
}


string CPJSysInfo::GetCurrentVersionRegStr(const string& ValName) {

//{Gets string info from Windows current version key in registry}

  static const char *cCurrentVersionRegKey = "Software\\Microsoft\\Windows\\CurrentVersion";
  
  string Result = "";

  CRegistry reg(HKEY_LOCAL_MACHINE, cCurrentVersionRegKey, KEY_QUERY_VALUE);
  if (! reg.IsOpen()) {
    return Result;
  }
  if (! reg.GetStringValue(ValName.c_str())) {
    return Result;
  }

  Result = reg.StringValue();

  return Result;

}


string CPJSysInfo::GetCommonFilesFolder() {
  //{Read access method for CommonFilesFolder property}
  //{Returns fully qualified name of Common Files folder}

  string Result = GetCurrentVersionRegStr("CommonFilesDir");
  
  return Result;
}

string CPJSysInfo::GetSystemFolder() {
  //{Read access method for SystemFolder property}
  //{Returns fully qualified name of Windows system folder}
  char PFolder[MAX_PATH + 1];  // buffer to hold name returned from API

  string Result;

  if (::GetSystemDirectory(PFolder, MAX_PATH + 1) != 0) 
    Result = &PFolder[0];
  else
    Result = "";

  return Result;
}

string CPJSysInfo::GetWindowsFolder() {
  //{Read access method for WindowsFolder property}
  //{Returns fully qualified name of Windows folder}

  char PFolder[MAX_PATH + 1];  // buffer to hold name returned from API
  string Result;

  if (::GetWindowsDirectory(PFolder, MAX_PATH + 1) != 0) 
    Result = &PFolder[0];
  else
    Result = "";

  return Result;

}

string CPJSysInfo::GetTempFolder() {
  //{Read access method for TempFolder property}
  //{Returns fully qualified name of current temporary folder}

  char PathBuf[MAX_PATH];  // buffer to hold name returned from API
  string Result = "";

  if (::GetTempPath(MAX_PATH, PathBuf) != 0) 
    Result = &PathBuf[0];

  return Result;

}


string CPJSysInfo::GetProgramFilesFolder() {
  //{Read access method for ProgramFilesFolder property}
  //{Returns fully qualified name of Program Files folder}

  string Result = GetCurrentVersionRegStr("ProgramFilesDir");
  return Result;

}

string CPJSysInfo::GetComputerName() {
  //{Read access method for ComputerName property}

  char PComputerName[MAX_COMPUTERNAME_LENGTH + 1];  // buffer for name returned from API
  DWORD Size;                                       // size of name buffer
  string Result = "";

  Size = MAX_COMPUTERNAME_LENGTH + 1;

  if (::GetComputerName(PComputerName, &Size))
    Result = &PComputerName[0];

  return Result;

}

string CPJSysInfo::_GetUserName() {
  //{Read access method for UserName property}
  //{Returns name of currently logged on user}

  char PUserName[UNLEN + 1]; // buffer for name returned from API
  string Result = "";
  DWORD Size;                // size of name buffer

  Size = UNLEN + 1;
  if (::GetUserName(PUserName, &Size)) 
    Result = &PUserName[0];

  return Result;

}


bool CPJSysInfo::GetOsVerInfo() {
  //{Fills a TOSVersionInfoEx or TOSVersionInfo structure with information about
  //operating system - return true if success

  bool Result = true;

  // has the constructor filled it in already ?
  if (this->_OSV.dwOSVersionInfoSize > 0)
    return Result;
    
  // no - call by constructor
  this->_OSV.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
  // We first try to get exended information
  if (! ::GetVersionEx(reinterpret_cast<OSVERSIONINFO *>(&this->_OSV))) {
    // We failed to get extended info: try again with old structure
    this->_OSV.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    if (! ::GetVersionEx(reinterpret_cast<OSVERSIONINFO *>(&this->_OSV))) {
      Result = false;
    }
  }
  return Result;
}

int CPJSysInfo::GetOSBuildNumber() {
  //{Read access method for OSBuildNumber property}
  //{Returns build number of operating system}

  int Result = -1;

  if (GetOsVerInfo()) {
    Result = this->_OSV.dwBuildNumber & 0x0000FFFF;
  }
  return Result;
}

int CPJSysInfo::GetOSMajorVersion() {
  //{Read access method for OSMajorVersion property}
  //{Returns major version of OS}

  int Result = -1;

  if (GetOsVerInfo()) {
    Result = this->_OSV.dwMajorVersion;
  }
  return Result;

}

int CPJSysInfo::GetOSMinorVersion() {
  //{Read access method for OSMinorVersion property}
  //{Returns minor version of OS}

  int Result = -1;

  if (GetOsVerInfo()) {
    Result = this->_OSV.dwMinorVersion;
  }
  return Result;

}

string CPJSysInfo::GetDefaultLanguage() {
   string result = "";

   switch (PRIMARYLANGID(GetSystemDefaultLangID())) {
	  case LANG_DUTCH : {result = "Nl";break;}
	  case LANG_FRENCH : {result = "Fr";break;}
	  case LANG_ENGLISH : {result = "En";break;}
	  case LANG_GERMAN : {result = "De";break;}
	  default : {result =  "NoTranslation";break;}
   }
   return result;
}

CPJOSPlatform CPJSysInfo::GetOSPlatformEnum() {
  //{Returns OS platform }

  CPJOSPlatform Result;

  if (GetOsVerInfo()) {

    switch(this->_OSV.dwPlatformId) {
      case VER_PLATFORM_WIN32_NT: {
        Result = ospWinNT;
        break;
      }
      case VER_PLATFORM_WIN32_WINDOWS: {
        Result = ospWin9x;
        break;
      }
      case VER_PLATFORM_WIN32s: {
        Result = ospWin32s;
        break;
      }
      default: {
        Result = ospUnknow;
        break;
      }               
    }
  }

  return Result;

}




CPJOSProduct CPJSysInfo::GetOSProductEnum() {
  //{Returns code identifying OS product from OS ver info structure}

  CPJOSProduct Result = osUnknownOs;

  if (this->GetOsVerInfo()) {
    switch (this->_OSV.dwPlatformId) {
      case VER_PLATFORM_WIN32_NT: {
        // We have an NT OS
        if (this->_OSV.dwMajorVersion <= 4) 
          Result = osWinNT;
        else if ((this->_OSV.dwMajorVersion == 5) && (this->_OSV.dwMinorVersion == 0))
          Result = osWin2K;
        else if ((this->_OSV.dwMajorVersion == 5) && (this->_OSV.dwMinorVersion == 1))
          Result = osWinXP;
        else if ((this->_OSV.dwMajorVersion == 5) && (this->_OSV.dwMinorVersion == 2))
          Result = osWinSvr2003;
        else if ((this->_OSV.dwMajorVersion == 6) && (this->_OSV.dwMinorVersion == 0) && (this->_OSV.wProductType == 1))
            Result = osWinVista;
        else if ((this->_OSV.dwMajorVersion == 6) && (this->_OSV.dwMinorVersion == 0) && (this->_OSV.wProductType == 3))
            Result = OsWin2008;
		else if ((this->_OSV.dwMajorVersion == 6) && (this->_OSV.dwMinorVersion == 1) && (this->_OSV.wProductType != VER_NT_WORKSTATION))
            Result = OsWinSvr2008;
		else if ((this->_OSV.dwMajorVersion == 6) && (this->_OSV.dwMinorVersion == 1) && (this->_OSV.wProductType == VER_NT_WORKSTATION))
            Result = osWin7;
        else
            Result = osUnknownWinNT; 
        break;
      }
      case VER_PLATFORM_WIN32_WINDOWS: {
        // We have a Win 95 line OS
        if ((this->_OSV.dwMajorVersion == 4) && (this->_OSV.dwMinorVersion == 0))
          Result = osWin95;
		else if ((this->_OSV.dwMajorVersion == 4) && (this->_OSV.dwMinorVersion == 10)) {
			if (this->_OSV.szCSDVersion [1] == 'A') {Result = osWin98SE;}  else {Result = osWin98;}  
		}
        else if ((this->_OSV.dwMajorVersion == 4) && (this->_OSV.dwMinorVersion == 90))
          Result = osWinMe;
        else
          Result = osUnknownWin9x;
        break;
      }
      default: {
        // This is a Win32s enabled OS
        Result = osUnknownWin32s;
        break;
      }
    }
  }

  return Result;

}

string CPJSysInfo::GetOSProductName()  {
  //{Return product name of OS from given OS ver info structure}
  
  string Result = "";

  if (this->GetOsVerInfo()) {
    Result = cOSProductNames[GetOSProductEnum()];
  }

  return Result;
}

string CPJSysInfo::GetOSProductType() {

  string Result = "";

  if (this->GetOsVerInfo()) {
    //{Return type of OS product from given OS ver info structure. IsOSVersionInfoEx
    //indicates if OSV is an extended structure (true) or is older style structure
    //(false). We only return a value for NT}

    HKEY RegKey;                       // registry key
    BYTE ProductType[80];              // buffer to store product type from reg
    DWORD BufLen;                      // length of ProductType buffer

    switch (this->_OSV.dwPlatformId) {
      case VER_PLATFORM_WIN32_NT: {
        // This is an NT OS
        // check  product type
        if (IsOsVersionInfoEx) {
          // we have extended OS info: product type info is in here
          if (this->_OSV.wProductType == VER_NT_WORKSTATION) {
            if (this->_OSV.dwMajorVersion == 4)
              Result = "Workstation";
            else if ((this->_OSV.wSuiteMask & VER_SUITE_PERSONAL) != 0)
              Result = "Home Edition";
            else
              Result = "Professional";
          }
          else if (this->_OSV.wProductType == VER_NT_SERVER) {
            // This is an NT server OS
            if ((this->_OSV.dwMajorVersion == 5) && (this->_OSV.dwMinorVersion == 2)) {
              // Windows Serer 2003
              if ((this->_OSV.wSuiteMask & VER_SUITE_DATACENTER) != 0) 
                Result = "Datacenter Edition";
              else if ((this->_OSV.wSuiteMask && VER_SUITE_ENTERPRISE) != 0)
                Result = "Enterprise Edition";
              else if (this->_OSV.wSuiteMask == VER_SUITE_BLADE)
                Result = "Web Edition";
              else
                Result = "Standard Edition";
            }
            else if ((this->_OSV.dwMajorVersion == 5) && (this->_OSV.dwMinorVersion == 0)) {
              // Windows 2000 Server
              if ((this->_OSV.wSuiteMask & VER_SUITE_DATACENTER) != 0)
                Result = "Datacenter Server";
              else if ((this->_OSV.wSuiteMask & VER_SUITE_ENTERPRISE) != 0)
                Result = "Advanced Server";
              else
                Result = "Server";
            } else {
              // Windows NT 4.0
              if ((this->_OSV.wSuiteMask & VER_SUITE_ENTERPRISE) != 0)
                Result = "Server, Enterprise Edition";
              else
                Result = "Server";
            }
          }
        } else { // (! IsVersionInfoEx)
          // we have not got extended OS info: read product type from registry
          RegOpenKeyEx( HKEY_LOCAL_MACHINE,
             "SYSTEM\\CurrentControlSet\\Control\\ProductOptions",
             0, KEY_QUERY_VALUE, &RegKey );
          RegQueryValueEx(RegKey, "ProductType", NULL, NULL,
             &ProductType[0], &BufLen);
          RegCloseKey(RegKey);
          if (_stricmp("WINNT", reinterpret_cast<PCHAR>(ProductType)) == 0)
             Result = "Professional";
          if (_stricmp("LANMANNT", reinterpret_cast<PCHAR>(ProductType)) == 0)
             Result = "Server";
          if (_stricmp("SERVERNT", reinterpret_cast<PCHAR>(ProductType)) == 0)
             Result = "Advanced Server";
        }  // IsVersionInfoEx
      }  // case VER_PLATFORM_WIN32_NT
    } // switch
  }

  return Result;

}



string CPJSysInfo::GetOSServicePack() {
  //{Returns name of any service pack associated with OS (if NT) or additional
  // product info (if Win9x)}

  HKEY RegKey; // registry key

  string Result = "";

  if (this->GetOsVerInfo()) {
    switch ( this->_OSV.dwPlatformId ) {
      case VER_PLATFORM_WIN32_NT:  {
        // An NT OS
        if ((this->_OSV.dwMajorVersion == 4)
          && (_stricmp(this->_OSV.szCSDVersion, "Service Pack 6") == 0)) {
          // Special test for SP6 versus SP6a.
          if (RegOpenKeyEx(
            HKEY_LOCAL_MACHINE,
            "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Hotfix\\Q246009",
            0,
            KEY_QUERY_VALUE,
            &RegKey) == ERROR_SUCCESS)
            // Windows NT4 SP6a
            Result = "Service Pack 6a";
          else
            // Window NT4 earlier than SP6a
            Result = this->_OSV.szCSDVersion;
          RegCloseKey(RegKey);
        } else {
          Result = this->_OSV.szCSDVersion;
        }
        break;
      }
      case VER_PLATFORM_WIN32_WINDOWS: {
        // A Win9x OS
        if (_OSV.dwMajorVersion == 4) {
          if ((this->_OSV.dwMinorVersion == 0)
            && ((this->_OSV.szCSDVersion[1] == 'C') || (this->_OSV.szCSDVersion[1] == 'B')))
            Result = "OSR2";
          else if ((this->_OSV.dwMinorVersion == 10) && (this->_OSV.szCSDVersion[1] == 'A')) 
            Result = "SE";
        }
      }
    } // switch
  } // have versioninfo

  return Result;

}


string CPJSysInfo::GetOSDesc() {
  //{Read access method for OSDesc property}

  string Result = "";
  string s = "";
  char tmp[128];

  Result = cOSProductNames[GetOSProductEnum()];
  switch ( _OSV.dwPlatformId ) {
    case VER_PLATFORM_WIN32_NT: {
      // We have an NT OS
      // check for product type
      s = this->GetOSProductType();
      if (s != "")
        Result = Result + " " + s;
      // display version, service pack (if any), and build number.
      if (_OSV.dwMajorVersion <= 4) {
        sprintf_s(tmp, sizeof(tmp), "version %d.%d %s (Build %d)",
          this->_OSV.dwMajorVersion,
          this->_OSV.dwMinorVersion,
          this->GetOSServicePack(),
          this->GetOSBuildNumber());
        Result = Result + " " + tmp;
      } else {
        sprintf_s(tmp, sizeof(tmp), "%s (Build %d)",
          this->GetOSServicePack().c_str(),
          this->GetOSBuildNumber());
        Result = Result + " " + tmp;
      }
      break;
    }
    case VER_PLATFORM_WIN32_WINDOWS:  {
      // We have a Win 95 line OS
      s = GetOSServicePack();
      if (s != "")
        Result = Result + s;
      break;
    }
  }

  return Result;
}


string CPJSysInfo::GetDesktopFolder(void) {
  //{Read access method for DesktopFolder property}

  char lpszPath[MAX_PATH + 2];

  string Result = "";

  if (SHGetSpecialFolderPath(
    0,
    lpszPath,
    CSIDL_DESKTOPDIRECTORY,
    FALSE)
    ) {
      Result = lpszPath;
  }

  return Result;
}
