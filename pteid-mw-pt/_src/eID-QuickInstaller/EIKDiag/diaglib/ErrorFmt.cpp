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
#include "errorfmt.h"

#include "autovec.h"
using namespace sc;

CErrorFmt::CErrorFmt(void) {
}

CErrorFmt::~CErrorFmt(void) {
}

string CErrorFmt::FormatError(int ErrorCode,const char *FunctionName) {
  char *msgBuf = NULL;
  string Result = "";

  int msgLen = FormatMessage(
    FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
    NULL,
    ErrorCode,
    0,
    reinterpret_cast<LPTSTR>(&msgBuf),
    0,
    NULL);
  size_t bufsize = strlen(FunctionName) + 64;
  auto_vec<char> buf(new char[bufsize]);
  if (msgLen > 0) {
    sprintf_s(buf.get(), bufsize, "%s: 0x%x(%d): ", FunctionName, ErrorCode, ErrorCode);
    Result = buf.get();
    Result = Result + msgBuf;
    LocalFree(msgBuf);
  } else {
    sprintf_s(buf.get(), bufsize, "%s: 0x%x (%d)", FunctionName, ErrorCode, ErrorCode);
    Result = Result + buf.get();
  }

  return Result;

}

