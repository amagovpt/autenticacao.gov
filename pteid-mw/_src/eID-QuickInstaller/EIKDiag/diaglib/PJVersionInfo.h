//
// $Id: PJVersionInfo.h,v 1.4 2008/08/21 06:40:51 DH Exp $
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


#ifndef __PJVERSIONINFO__
#define __PJVERSIONINFO__


#pragma once

#undef UNICODE

#include <windows.h>

#include <string>

using namespace std;

#pragma pack(2)
typedef struct _CPJVersionNumber {
	WORD V1;
	WORD V2;
	WORD V3;
	WORD V4;
} CPJVersionNumber;

typedef struct _CTransRec { // record to hold translation information
    WORD Lang;
    WORD CharSet;
} CTransRec, *PTransRec;
#pragma pack()



class CPJVersionInfo {
public:
  CPJVersionInfo(void);
  CPJVersionInfo(const string& FileName);
  ~CPJVersionInfo(void);
public:
    // properties
  bool HaveInfo() { return _HaveInfo; };
    //{Property true if file version info for the file per FileName property has
    // been successfully read}
  VS_FIXEDFILEINFO FixedFileInfo() { return _FixedFileInfo; };
    //{Exposes the whole fixed file info record: following properties expose
    // the various fields of it}
  CPJVersionNumber FileVersionNumber() { return GetFileVersionNumber(); };
    //{Version number of file, in numeric format, from fixed file info}
  CPJVersionNumber ProductVersionNumber() {return GetProductVersionNumber(); };
    //{Version number of product, in numeric format, from fixed file info}
  DWORD FileOS() { return GetFixedFileInfoItemByIdx(0); };
    //{Code describing operating system to be used by file}
  DWORD FileType()  { return GetFixedFileInfoItemByIdx(1); };
    //{Code descibing type of file}
  DWORD FileSubType()  { return GetFixedFileInfoItemByIdx(2); };
    //{Code describing sub-type of file - only used for certain values of
    // FileType property}
  DWORD FileFlagsMask()  { return GetFixedFileInfoItemByIdx(3); };
    //{Code describing which FileFlags are valid}
  DWORD FileFlags()  { return GetFixedFileInfoItemByIdx(4); };
    //{Flags describing file state}
  string Comments() { return GetStringFileInfoByIdx(0); };
    //{String file info property giving user defined comments for current
    // translation}
  string CompanyName() { return GetStringFileInfoByIdx(1); };
    //{String file info property giving name of company for current translation}
  string FileDescription()  { return GetStringFileInfoByIdx(2); };
    //{String file info property giving description of file for current translation}
  string FileVersion() { return GetStringFileInfoByIdx(3); };
    //{String file info property giving version number of file in string format
    // for current translation}
  string InternalName() { return GetStringFileInfoByIdx(4); };
    //{String file info property giving internal name of file for current
    // translation}
  string LegalCopyright() { return GetStringFileInfoByIdx(5); };
    //{String file info property giving copyright message for current
    // translation}
  string LegalTrademarks() { return GetStringFileInfoByIdx(6); };
    //{String file info property giving trademark info for current translation}
  string OriginalFileName() { return GetStringFileInfoByIdx(7); };
    //{String file info property giving original name of file for current
    // translation}
  string PrivateBuild() { return GetStringFileInfoByIdx(8); };
    //{String file info property giving information about a private build of
    // file for current translation}
  string ProductName() { return GetStringFileInfoByIdx(9); };
    //{String file info property giving name of product for current translation}
  string ProductVersion() { return GetStringFileInfoByIdx(10); };
    //{String file info property giving version number of product in string
    // format for current translation}
  string SpecialBuild() { return GetStringFileInfoByIdx(11); };
    //{String file info property giving information about a special build of
    // file for current translation}
  string StringFileInfo(const string& Name) { return GetStringFileInfo(Name); };
    //{Returns the value for string file info with given name for current
    // translation. This property can return both standard and custom string
    // info}
  string Language() { return GetLanguage(); };
    //{Name of language in use in current translation}
  string CharSet() { return GetCharSet(); };
    //{Name of character set in use in current translation}
  WORD LanguageCode() { return GetLanguageCode(); };
    //{Code of language in use in current translation}
  WORD CharSetCode() { return GetCharSetCode(); };
    //{Character set code in use in current translation}
  int NumTranslations() { return _NumTranslations; };
    //{The number of difference translations (ie languages and char sets) in
    // the version information}
  int CurrentTranslation() { return GetCurrentTranslation(); };
  void setCurrentTransLation(const int Value) { SetCurrentTranslation(Value); };
    //{Zero-based index of the current translation: this is 0 when a file is
    // first accessed. Set to a value in range 0..NumTranslations-1 to access
    // other translations. All string info, language and char set properties
    // return information for the current translation}
  string FileName() { return _FileName; };
  void setFileName(const string& Value);
    //{Name of file to which version information relates}


protected:
	virtual void ClearProperties();
	  //{Forces properties to return cleared values}
	virtual void ReadVersionInfo();
	  //{Reads version info from file}

private:
	string _FileName;
	bool _HaveInfo;
	int _NumTranslations;
	int _CurrentTranslation;
	VS_FIXEDFILEINFO _FixedFileInfo;
	CPJVersionNumber GetProductVersionNumber();
	CPJVersionNumber GetFileVersionNumber();

	string GetLanguage();
	string GetCharSet();
	WORD GetCharSetCode();
	WORD GetLanguageCode();
	int GetCurrentTranslation();
	void SetCurrentTranslation(const int Value);
	string GetStringFileInfo(const string& Name);
	string GetStringFileInfoByIdx(int Index);
	DWORD GetFixedFileInfoItemByIdx(int Index);
private:
	void *_PInfoBuffer;
	  //{Pointer to info buffer}
	PTransRec _PTransBuffer;
	  //{Pointer to translation buffer}
	void GetInfoBuffer(DWORD Len);
	  //{Creates an info buffer of required size}
	void GetTransBuffer(UINT Len);
	  //{Creates a translation table buffer of required size}
	string GetTransStr();
	  //{Translation info encoded in string: takes into account current
	  // translation}
};

#endif __PJVERSIONINFO__
