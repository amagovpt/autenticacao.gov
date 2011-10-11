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
#include "infhandle.h"

#include "autovec.h"
using namespace sc;

CInfHandle::CInfHandle(const string& InfFileName) {
  _ErrorLine = 0;
  _InfHandle = SetupOpenInfFile(
    reinterpret_cast<PCTSTR>(InfFileName.c_str()),
    NULL,
    INF_STYLE_OLDNT | INF_STYLE_WIN4,
    &_ErrorLine);
}

CInfHandle::~CInfHandle(void) {
  if (_InfHandle != NULL) {
    SetupCloseInfFile(_InfHandle);
  }
}

bool CInfHandle::FindFirstLine(const string& Section, const string& Key) {
  if (SetupFindFirstLine(_InfHandle, Section.c_str(), Key.c_str(), &_Context)) {
    return true;
  }
  return false;
}

bool CInfHandle::FindNextLine() {
  if (SetupFindNextLine(&_Context, &_Context)) {
    return true;
  }
  return false;
}

string CInfHandle::GetLineText() {

  string Result = "";
  DWORD dwSize;

  if (!SetupGetLineText(&_Context, NULL, NULL, NULL, 
                        NULL, 0, &dwSize))
    return Result;

  auto_vec<char> buf(new char[dwSize]);

  if (!SetupGetLineText(&_Context, NULL, NULL, NULL, 
                        buf.get(), dwSize, &dwSize))
    return Result;

  Result = buf.get();
  return Result;

}

string CInfHandle::GetLineText(const string &Section, const string& Key) {

  string Result = "";
  DWORD dwSize;

  if (!SetupGetLineText(NULL, _InfHandle, Section.c_str(), Key.c_str(), 
                        NULL, 0, &dwSize))
    return Result;

  auto_vec<char> buf(new char[dwSize]);

  if (!SetupGetLineText(NULL, _InfHandle, Section.c_str(), Key.c_str(), 
                        buf.get(), dwSize, &dwSize))
    return Result;

  Result = buf.get();
  return Result;

}
