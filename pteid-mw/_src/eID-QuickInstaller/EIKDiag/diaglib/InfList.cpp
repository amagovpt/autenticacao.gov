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
#include "inflist.h"
#include "ErrorFmt.h"

#include "autovec.h"
using namespace sc;

CInfList::CInfList(void) {
}

CInfList::~CInfList(void) {
}

bool CInfList::BuildInfFileList(void) {

  DWORD dwSize;

  this->_InfFileList.clear();
  this->_LastError = "";

  if (SetupGetInfFileList(
    NULL, 
    INF_STYLE_OLDNT | INF_STYLE_WIN4,
    NULL,
    0,
    &dwSize) == 0) {
      int e = GetLastError();
      if (e != ERROR_INSUFFICIENT_BUFFER) {
        _LastError = CErrorFmt::FormatError(e, "SetupGetInfFileList()");
        return false; 
      }
  }

  const auto_vec<char> buf(new char[dwSize]);

  if (SetupGetInfFileList(
    NULL, 
    INF_STYLE_OLDNT | INF_STYLE_WIN4,
    reinterpret_cast<PTSTR>(buf.get()),
    dwSize,
    &dwSize) == 0) {
      int e = GetLastError();
      this->_LastError = CErrorFmt::FormatError(e, "SetupGetInfFileList()");
      return false; 
  }


  char *pInfName = buf.get();
  while (true) {
    if (*pInfName == 0)
      break;
    string infname = pInfName;
    this->_InfFileList.push_back(infname);
    while (*pInfName++ != 0)
      ;
  }

  // inf file location: %windir%\inf of %windir%\system32

  return true;

}

