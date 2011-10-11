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

#ifndef __REGISTRY__
#define __REGISTRY__

#pragma once

#include <windows.h>
#include <string>
#include <vector>

using namespace std;

typedef vector<string> MSZArray;
typedef MSZArray::iterator MZSIterator;
typedef vector<BYTE> ByteArray;
typedef ByteArray::iterator ByteArrayIterator;
typedef vector <string>RegSubKeys;
typedef RegSubKeys::iterator RegSubKeysIterator;
typedef vector <string>RegNames;
typedef RegNames::iterator RegNamesIterator;

typedef enum _RegValuetype {
  REGTYPE_INT,
  REGTYPE_SZ,
  REGTYPE_MULTISZ,
  REGTYPE_BINARY,
  REGTYPE_EXPAND_SZ,
  REGTYPE_UNSUPPORTED
} RegValueType;

class CRegistry {
public:
  CRegistry(HKEY Hkey, const char *SubKey, REGSAM SamDesired);
  CRegistry(HKEY Hkey, const char *SubKey);
  virtual ~CRegistry(void);
  string& LastError() { return _LastError; };
  bool GetIntValue(const char *Name);
  bool GetStringValue(const char *Name);
  bool GetMultiStringValue(const char *Name);
  bool GetExpandStringValue(const char *Name);
  bool GetBinaryValue(const char *Name);
  bool SetExpandStringValue(const char* Name, const char* Value);
  bool SetDWordValue(const char* Name, const DWORD  Value) ;
  bool GetSubKeys();
  bool GetNames();
  bool DeleteKey(const char *Name);
  bool DeleteEntry(const char *Name);
  bool DeleteKeyTree(const char *Name);

  bool IsOpen()               { return (_hReg != 0); };

  DWORD IntValue()            { return _dwValue;     };
  string StringValue()        { return _szValue;     };
  MSZArray StringArrayValue() { return _mszValue;    };
  ByteArray BinValue()        { return _binValue;    }; 
  RegSubKeys& SubKeys()       { return _SubKeyList;  };
  RegNames& Names()           { return _NameList;    };
private:
  bool CRegistry::RegDelnodeRecurse (HKEY hKeyRoot, LPTSTR lpSubKey);
  RegValueType GetValue(const char *Name);
  DWORD _dwValue;
  string _szValue;
  MSZArray _mszValue;
  ByteArray _binValue;
  HKEY _hReg;
  string _LastError;
  RegSubKeys _SubKeyList;
  RegNames _NameList;
};

#endif __REGISTRY__
