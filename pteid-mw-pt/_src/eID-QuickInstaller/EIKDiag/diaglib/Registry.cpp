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

#include "StdAfx.h"
#include "registry.h"
#include "ErrorFmt.h"
#include <strsafe.h>

#include "autovec.h"
using namespace sc;

#pragma hdrstop

CRegistry::CRegistry(HKEY HKey, const char *SubKey, REGSAM SamDesired) {

    LONG hr = RegOpenKeyEx(HKey, SubKey, 0, SamDesired, &_hReg);
    if (hr != ERROR_SUCCESS) {
        _hReg = 0;
        _LastError = CErrorFmt::FormatError(GetLastError(), "CRegistry()");
    }

}

CRegistry::CRegistry(HKEY HKey, const char *SubKey) {

    LONG hr = RegOpenKeyEx(
        HKey, 
        SubKey, 
        0, 
        KEY_ENUMERATE_SUB_KEYS | KEY_QUERY_VALUE, 
        &_hReg);
    if (hr != ERROR_SUCCESS) {
        _hReg = 0;
        _LastError = CErrorFmt::FormatError(GetLastError(), "CRegistry()");
    }

}

CRegistry::~CRegistry(void) {

    if (_hReg != 0) {
        RegCloseKey(_hReg);
    }

}


bool CRegistry::GetIntValue(const char *Name) {
    if (_hReg == 0)
        return false;
    if (this->GetValue(Name) != REGTYPE_INT)
        return false;
    else
        return true;
}

bool CRegistry::GetStringValue(const char *Name){
    if (_hReg == 0)
        return false;
    if (this->GetValue(Name) != REGTYPE_SZ)
        return false;
    else
        return true;
}

bool CRegistry::GetMultiStringValue(const char *Name){
    if (_hReg == 0)
        return false;
    if (this->GetValue(Name) != REGTYPE_MULTISZ)
        return false;
    else
        return true;
}

bool CRegistry::GetExpandStringValue(const char* Name) {
    if (_hReg == 0)
        return false;
    if (this->GetValue(Name) != REGTYPE_EXPAND_SZ)
        return false;
    else
        return true;
}

bool CRegistry::GetBinaryValue(const char *Name){
    if (_hReg == 0)
        return false;
    if (this->GetValue(Name) != REGTYPE_BINARY)
        return false;
    else
        return true;
}

bool CRegistry::DeleteKey(const char *Name) {

    if (this->_hReg == 0)
        return false;
	long rv =  RegDeleteKey(this->_hReg,Name);

    if (rv != ERROR_SUCCESS) {
        this->_LastError = CErrorFmt::FormatError(::GetLastError(), "DeleteKey");
        return false;
    }
    return true;
   return true;
}

bool CRegistry::RegDelnodeRecurse (HKEY hKeyRoot, LPTSTR lpSubKey)
{
    LPTSTR lpEnd;
    LONG lResult;
    DWORD dwSize;
    TCHAR szName[MAX_PATH];
    HKEY hKey;
    FILETIME ftWrite;

    // First, see if we can delete the key without having
    // to recurse.

    lResult = RegDeleteKey(hKeyRoot, lpSubKey);

    if (lResult == ERROR_SUCCESS) 
        return TRUE;

    lResult = RegOpenKeyEx (hKeyRoot, lpSubKey, 0, KEY_READ, &hKey);

    if (lResult != ERROR_SUCCESS) 
    {
        if (lResult == ERROR_FILE_NOT_FOUND) {
            printf("Key not found.\n");
            return TRUE;
        } 
        else {
            printf("Error opening key.\n");
            return FALSE;
        }
    }

    // Check for an ending slash and add one if it is missing.

    lpEnd = lpSubKey + lstrlen(lpSubKey);

    if (*(lpEnd - 1) != TEXT('\\')) 
    {
        *lpEnd =  TEXT('\\');
        lpEnd++;
        *lpEnd =  TEXT('\0');
    }

    // Enumerate the keys

    dwSize = MAX_PATH;
    lResult = RegEnumKeyEx(hKey, 0, szName, &dwSize, NULL,
                           NULL, NULL, &ftWrite);

    if (lResult == ERROR_SUCCESS) 
    {
        do {

            StringCchCopy (lpEnd, MAX_PATH*2, szName);

            if (!RegDelnodeRecurse(hKeyRoot, lpSubKey)) {
                break;
            }

            dwSize = MAX_PATH;

            lResult = RegEnumKeyEx(hKey, 0, szName, &dwSize, NULL,
                                   NULL, NULL, &ftWrite);

        } while (lResult == ERROR_SUCCESS);
    }

    lpEnd--;
    *lpEnd = TEXT('\0');

    RegCloseKey (hKey);

    // Try again to delete the key.

    lResult = RegDeleteKey(hKeyRoot, lpSubKey);

    if (lResult == ERROR_SUCCESS) 
        return TRUE;

    return FALSE;
}


bool CRegistry::DeleteKeyTree(const char *Name) {
    TCHAR szDelKey[2 * MAX_PATH];

    StringCchCopy (szDelKey, MAX_PATH*2, Name);
	return RegDelnodeRecurse(this->_hReg, szDelKey);
}

bool CRegistry::DeleteEntry(const char *Name) {

    if (this->_hReg == 0)
        return false;
	long rv = RegDeleteValue(this->_hReg,Name);
    if (rv != ERROR_SUCCESS) {
        this->_LastError = CErrorFmt::FormatError(::GetLastError(), "DeleteEntry");
        return false;
    }
    return true;
   return true;
}


RegValueType CRegistry::GetValue(const char *Name) {

    DWORD dwSize;
    DWORD dwDataType;
    _LastError = "";

    LONG hr = RegQueryValueEx(
        this->_hReg,
        Name,
        NULL,
        NULL,
        NULL,
        &dwSize);
    if (hr != ERROR_SUCCESS) {
        this->_LastError = CErrorFmt::FormatError(::GetLastError(), "CRegistry::GetValue[size]");
        return REGTYPE_UNSUPPORTED;
    }
    auto_vec<BYTE> buf(new BYTE[dwSize + 2]);
    hr = RegQueryValueEx(
        this->_hReg,
        Name,
        NULL,
        &dwDataType,
        buf.get(),
        &dwSize);
    if (hr != ERROR_SUCCESS) {
        this->_LastError = CErrorFmt::FormatError(GetLastError(), "CRegistry::GetValue[data]");
        return REGTYPE_UNSUPPORTED;
    }
    BYTE *p = buf.get();
    switch(dwDataType) {
    case REG_DWORD: {
        this->_dwValue = *(reinterpret_cast<int *>(p));
        return REGTYPE_INT;
                    }
    case REG_SZ: {
        p[dwSize] = 0;
        this->_szValue = reinterpret_cast<char *>(p);
        return REGTYPE_SZ;
                 }
    case REG_MULTI_SZ: {
        p[dwSize] = 0;
        this->_mszValue.clear();
        char *psz = reinterpret_cast<char *>(p);
        while (true) {
            if (*psz == 0)
                break;
            string value = psz;
            this->_mszValue.push_back(value);
            while (*psz++ != 0)
                ;
        }
        return REGTYPE_MULTISZ;
                       }
    case REG_BINARY: {
        this->_binValue.clear();
        BYTE *pb = reinterpret_cast<BYTE *>(p);
        int l = dwSize;
        while (l-- > 0)
            _binValue.push_back(*pb++);
        return REGTYPE_BINARY;
                     }
    case REG_EXPAND_SZ: {
        p[dwSize] = 0;
        this->_szValue = reinterpret_cast<char *>(p);
        return REGTYPE_EXPAND_SZ;
                        }
    default: {
        this->_LastError = "CRegistry::GetValue(): Unsupported data type.";
        return REGTYPE_UNSUPPORTED;
             }
    }
    return REGTYPE_UNSUPPORTED;
}

bool CRegistry::SetExpandStringValue(const char* Name, const char* Value) {
    
    if (this->_hReg == 0)
        return false;
    long rv = RegSetValueEx(
        this->_hReg,
        Name,
        0,
        REG_EXPAND_SZ,
        (byte *)Value,
        (DWORD)strlen(Value) + 1);
    if (rv != ERROR_SUCCESS) {
        this->_LastError = CErrorFmt::FormatError(::GetLastError(), "SetExpandStringValue");
        return false;
    }
    return true;

}
bool CRegistry::SetDWordValue(const char* Name, const DWORD  Value) {
    
    if (this->_hReg == 0)
        return false;

	long rv = RegSetValueEx(this->_hReg,Name,0,REG_DWORD,(byte*)&Value,sizeof(DWORD));


	

    if (rv != ERROR_SUCCESS) {
        this->_LastError = CErrorFmt::FormatError(::GetLastError(), "SetDWordValue");
        return false;
    }
    return true;

}


bool CRegistry::GetSubKeys() {

    DWORD nSubKeys;
    DWORD MaxSubKeyLen;

    LONG hr = RegQueryInfoKey(
        this->_hReg,
        NULL,
        NULL,
        NULL,
        &nSubKeys,
        &MaxSubKeyLen, // NT,2000,XP... : including terminating 0
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL);
    if (hr != ERROR_SUCCESS) {
        this->_LastError = CErrorFmt::FormatError(GetLastError(), "CRegistry::GetSubKeys");
        return false;
    }

    this->_SubKeyList.clear();

    for (unsigned int i = 0; i < nSubKeys; ++i) {

        DWORD SubKeyLen = MaxSubKeyLen + 2;
        auto_vec<char> keybuf(new char[SubKeyLen ]);


        hr = RegEnumKeyEx(
            this->_hReg, 
            i, 
            keybuf.get(),
            &SubKeyLen,
            NULL,
            NULL,
            NULL,
            NULL);
        if (hr == ERROR_NO_MORE_ITEMS)
            break;
        if (hr != ERROR_SUCCESS) {
            this->_LastError = CErrorFmt::FormatError(GetLastError(), "CRegistry::GetSubKeys");
            return false;
        }

        string sp = keybuf.get();
        this->_SubKeyList.push_back(sp);

    }

    return true;

}

bool CRegistry::GetNames() {

    const int NAME_LENGTH = 256;

    this->_NameList.clear();

    DWORD hr = ERROR_SUCCESS;

    for (DWORD i = 0;  hr != ERROR_NO_MORE_ITEMS; ++i) {

        DWORD cbName = NAME_LENGTH;
        auto_vec<char> valuebuf(new char[cbName]);

        hr = RegEnumValue(
            _hReg,
            i, 
            valuebuf.get(),
            &cbName,
            NULL,
            NULL,
            NULL,
            NULL);
        if (hr == ERROR_SUCCESS) {
            this->_NameList.push_back(valuebuf.get());
        } else {
            if (hr != ERROR_NO_MORE_ITEMS) {
                this->_LastError = CErrorFmt::FormatError(hr, "CRegistry::GetNames()");
                return false;
            }
        }
    }

    return true;

}

