//
// $Id: PJVersionInfo.cpp,v 1.3 2008/07/02 07:58:38 DH Exp $
//
/*
{ ##
  @FILE                     PJVersionInfo.pas
  @COMMENTS                 Version Information Component (32 bit) source code
                            (development split from 16 bit version after v1.0).
  @PROJECT_NAME             Version Information Component
  @PROJECT_DESC             Component that reads version information from files.
  @OTHER_NAMES              + Original unit name was VerInfo.pas
                            + Changed to VInfo.pas at v2.1
                            + Changed to PJVersionInfo.pas at v3.0
  @AUTHOR                   Peter Johnson, LLANARTH, Ceredigion, Wales, UK
  @EMAIL                    peter.johnson@openlink.org
  @WEBSITE                  http://www.delphidabbler.com/
  @COPYRIGHT                Copyright © Peter D Johnson, 1998-2003.
  @LEGAL_NOTICE             This component is placed in the public domain. It
                            may be freely copied and circulated on a not for
                            profit basis providing that the code is unmodified
                            and this notice and information about the author and
                            his copyright remains attached to the source code.
  @CREDITS                  + In producing this component some techniques were
                              used which were learned from FVersion by PJ Veger,
                              Best, The Netherlands (Feb/96). In particular the
                              method of accessing language and char-set tables
                              was taken from PJ Veger's code.
                            + Roland Beduerftig provided a fix (from v3.1) to
                              make the component compatible with C++ Builder.
*/

#pragma once

#include "stdafx.h"

#include "PJVersionInfo.h"

#pragma hdrstop
//---------------------------------------------------------------------------

CPJVersionInfo::CPJVersionInfo(void) {
  // Default is no file name - refers to executable file for application
  this->_FileName = "";
  this->_PInfoBuffer = NULL;
  this->_PTransBuffer = NULL;
  ClearProperties();
  ReadVersionInfo();
}

CPJVersionInfo::CPJVersionInfo(const string& FileName) {
  // Default is no file name - refers to executable file for application
  this->_FileName = FileName;
  this->_PInfoBuffer = NULL;
  this->_PTransBuffer = NULL;
  ClearProperties();
  ReadVersionInfo();
}



CPJVersionInfo::~CPJVersionInfo(void) {
  // Ensure that translation buffer is free if allocated
  if (_PTransBuffer != NULL)
    delete[] _PTransBuffer;
  // don't forget InfoBuffer
  if (_PInfoBuffer != NULL)
    delete[] _PInfoBuffer;
}

void CPJVersionInfo::ClearProperties() {
  //{Forces properties to return cleared values}
  // Record that we haven't read ver info: this effectively clears properties
  // since each property read access method checks this flag before returning
  // result

  _HaveInfo = false;

}

typedef struct _CharSet {
  int Code;        // char set code
  char *Str;        // associated name of char set
} CharSet, *PCharSet;

static const CharSet cCharSets[] = {
  {     0,  "7-bit ASCII"                        },
  {   932,  "Windows, Japan (Shift - JIS X-0208)"},
  {   949,  "Windows, Korea (Shift - KSC 5601)"  },
  {   950,  "Windows, Taiwan (GB5)"              },
  {  1200,  "Unicode"                            },
  {  1250,  "Windows, Latin-2 (Eastern European)"},
  {  1251,  "Windows, Cyrillic"                  },
  {  1252,  "Windows, Multilingual"              },
  {  1253,  "Windows, Greek"                     },
  {  1254,  "Windows, Turkish"                   },
  {  1255,  "Windows, Hebrew"                    },
  {  1256,  "Windows, Arabic"                    }
 };

#define nCharSets (sizeof cCharSets / sizeof cCharSets[0])

string CPJVersionInfo::GetCharSet() {

  string Result = "";

  if (_HaveInfo) {
    // We have ver info: scan table of codes looking for required entry
    for (int I = 0; I < nCharSets; I++) {
      if (GetCharSetCode() == cCharSets[I].Code) {
        // found one - record its name
        Result = cCharSets[I].Str;
        return Result;
      }
    }
  }
  return Result;
}

WORD CPJVersionInfo::GetCharSetCode() {

  //{Read access for CharSetCode property: returns char set code for current
  // translation or 0 if there is no translation or we have no version info}

  if (_HaveInfo && (GetCurrentTranslation() >= 0))
    return _PTransBuffer[GetCurrentTranslation()].CharSet;
  else
    return 0;

}

int CPJVersionInfo::GetCurrentTranslation() {

  //{Read access method for CurrentTranslation property: returns index to current
  // translation if we have version info or -1 if no version info}

  if (_HaveInfo) 
    return _CurrentTranslation;
  else
    return -1;

}


CPJVersionNumber CPJVersionInfo::GetFileVersionNumber() {
  //{Read access method for FileVersionNumber property: fill version info
  // structure and return it - if there's no version info all values will be zero}
  CPJVersionNumber Result;
  Result.V1 = HIWORD(_FixedFileInfo.dwFileVersionMS);
  Result.V2 = LOWORD(_FixedFileInfo.dwFileVersionMS);
  Result.V3 = HIWORD(_FixedFileInfo.dwFileVersionLS);
  Result.V4 = LOWORD(_FixedFileInfo.dwFileVersionLS);
  return Result;
}



DWORD CPJVersionInfo::GetFixedFileInfoItemByIdx(int Index) {
  //{Read access method for various DWORD fields of the fixed file information
  // record accessed by index.
  // NOTE: This is a fix for C++ Builder. Delphi is able to access the fields of
  // the TVSFixedFileInfo record directly in the read clause of the property
  // declaration but this is not possible in C++ Builder}

  DWORD Result;

  switch( Index ) {
    case 0:  { Result = _FixedFileInfo.dwFileOS;         break; }
    case 1:  { Result = _FixedFileInfo.dwFileType;       break; }
    case 2:  { Result = _FixedFileInfo.dwFileSubtype;    break; }
    case 3:  { Result = _FixedFileInfo.dwFileFlagsMask;  break; }
    case 4:  { Result = _FixedFileInfo.dwFileFlags;      break; }
    default: { Result = 0;                               break; }
  }

  return Result;
  
}

void CPJVersionInfo::GetInfoBuffer(DWORD Len) {
  //{Creates an info buffer of required size}

  // Clear any existing buffer
  if (_PInfoBuffer != NULL)
    delete[] _PInfoBuffer;
  // Create the new one
  _PInfoBuffer = new char[Len];

}

string CPJVersionInfo::GetLanguage() {
  //{Read access method for Language property: return string describing language
  // if we have some version info and empty string if we haven't}

  char Buf[256];   // stores langauge string from API call
  // Assume failure
  string Result = "";

  // Try to get language name from Win API if we have ver info
  if (_HaveInfo && (VerLanguageName(GetLanguageCode(), Buf, 255) > 0))
    Result = Buf;

  return Result;

}


WORD CPJVersionInfo::GetLanguageCode() {
  //{Read access for LanguageCode property: returns language code for current
  // translation or 0 if there is no translation or we have no version info}

  WORD Result = 0;
  if (_HaveInfo && (GetCurrentTranslation() >= 0))
    Result = _PTransBuffer[GetCurrentTranslation()].Lang;

  return Result;

}

CPJVersionNumber CPJVersionInfo::GetProductVersionNumber() {
  //{Read access method for ProductVersionNumber property: fill version info
  //structure and return it - if there's no version info all values will be zero}

  CPJVersionNumber Result;

  Result.V1 = HIWORD(_FixedFileInfo.dwProductVersionMS);
  Result.V2 = LOWORD(_FixedFileInfo.dwProductVersionMS);
  Result.V3 = HIWORD(_FixedFileInfo.dwProductVersionLS);
  Result.V4 = LOWORD(_FixedFileInfo.dwProductVersionLS);

  return Result;

}

string CPJVersionInfo::GetStringFileInfo(const string& Name) {
  //{Read access method for StringFileInfo array property: returns string
  //associated with given name or empty string if we have no version info}

  char CommandBuf[256]; // buffer to build API call command str
  void *Ptr;             // pointer to result of API call
  UINT Len;              // length of structure returned from API

  // Set default failure result to empty string
  string Result = "";
  // Check if we have valid information recorded in info buffer - exit if not
  if (_HaveInfo) {
    // Build API call command string for reading string file info:
    // this uses info string + language and character set    
    string s = "\\StringFileInfo\\" + GetTransStr() + "\\" + Name;
    strcpy_s(CommandBuf, sizeof(CommandBuf), s.c_str());
    // Call API to get required string and return it if successful
    if (VerQueryValue(_PInfoBuffer, CommandBuf, &Ptr, &Len))
      Result = reinterpret_cast<char *>(Ptr);
    else {
      int e = GetLastError();
      if (e != ERROR_RESOURCE_TYPE_NOT_FOUND) {
        sprintf_s(CommandBuf, sizeof(CommandBuf), "GetStringFileInfo error = %d (0x%x)", e, e);
        Result = CommandBuf;
      }
    }
  }
  return Result;

}

static const char *cNames[] = {
    "Comments",
    "CompanyName",
    "FileDescription",
    "FileVersion",
    "InternalName",
    "LegalCopyright",
    "LegalTrademarks",
    "OriginalFileName",
    "PrivateBuild",
    "ProductName",
    "ProductVersion",
    "SpecialBuild"
  };
    //{names of predefined string file info strings}

string CPJVersionInfo::GetStringFileInfoByIdx(int Index) {
  //{Read access method for all string file info properties: returns appropriate
  // string for the given property or empty string if we have no version info}

  return GetStringFileInfo(cNames[Index]);
}


void CPJVersionInfo::GetTransBuffer(UINT Len) {
  //{Creates a translation table buffer of required size}

  // Clear any existing buffer
  if (_PTransBuffer != NULL)
    delete[] _PTransBuffer;
  // Create the new one
  _PTransBuffer = new CTransRec[Len];
}

string CPJVersionInfo::GetTransStr() {
  //{Translation info encoded in string: takes into account current translation}
  CTransRec TransRec;  // translation record in array of translations
  string Result = "";  // return empty string if no valid translation
  char buf[256];

  if (GetCurrentTranslation() >= 0) {
    // There is a valid current translation: return hex string related to it
    TransRec = _PTransBuffer[GetCurrentTranslation()];
    sprintf_s(buf, sizeof(buf), "%4.4x%4.4x", TransRec.Lang, TransRec.CharSet);
    Result = buf;
  }

  return Result;

}

void CPJVersionInfo::ReadVersionInfo() {
  //{Reads version info from file}
  UINT Len;        // length of structs returned from API calls
  void *Ptr;       // points to version info structures
  DWORD InfoSize;  // size of info buffer
  DWORD Dummy;     // stores 0 in call to GetFileVersionInfoSize

  // Record default value of HaveInfo property - no info read
  _HaveInfo = false;
  // Store zeros in fixed file info structure: this is used when no info
  memset(&_FixedFileInfo, 0, sizeof(VS_FIXEDFILEINFO));
  // Set NumTranslations property to 0: this is value if no info
  _NumTranslations = 0;
  // Record required size of version info buffer
  InfoSize = GetFileVersionInfoSize(_FileName.c_str(), &Dummy);
  // Check that there was no error
  if (InfoSize > 0) {
    // Found info size OK
    // Ensure we have a sufficiently large buffer allocated
    GetInfoBuffer(InfoSize);
    // Read file version info into storage and check success
    if (GetFileVersionInfo(_FileName.c_str(), Dummy, InfoSize, _PInfoBuffer)) {
      // Success: we've read file version info to storage OK
      _HaveInfo = true;
      // Get fixed file info & copy to own storage
      VerQueryValue(_PInfoBuffer, "\\", &Ptr, &Len);
      _FixedFileInfo = *(reinterpret_cast<VS_FIXEDFILEINFO*>(Ptr));
      // Get first translation table info from API
      VerQueryValue(_PInfoBuffer, "\\VarFileInfo\\Translation", &Ptr, &Len);
      // Ptr is to block of translation records each of size Len:
      // work out number of translations
      _NumTranslations = (Len / sizeof(CTransRec));
      // store translation array in a buffer
      GetTransBuffer(Len);
      // Move(Ptr^, fPTransBuffer^, Len);
      memcpy(&_PTransBuffer[0], Ptr, Len);
      // make first translation in block current one (-1 if no translations)
      SetCurrentTranslation(0);   // adjusts value to -1 if no translations
    }
  }
}

void CPJVersionInfo::SetCurrentTranslation(const int Value) {
  //{Write accees method for CurrentTranslation property: if value is out of
  // translation range then it is set to -1 to indicate no translation}

  if ((Value >= 0) && (Value < _NumTranslations))
    _CurrentTranslation = Value;
  else
    _CurrentTranslation = -1;
}

void CPJVersionInfo::setFileName(const string& AName) {

  // Delphi designtime component stuff removed here -- dh -- 

  _FileName = AName;
  // clear all properties and read file version info for new file
  ClearProperties();
  ReadVersionInfo();
}









