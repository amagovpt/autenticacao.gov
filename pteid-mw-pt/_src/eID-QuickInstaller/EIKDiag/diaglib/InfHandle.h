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

#ifndef __INFHANDLE__
#define __INFHANDLE__


#pragma once

#undef UNICODE

#include <windows.h>
#include <setupapi.h>

#include <string>

using namespace std;


class CInfHandle {

public:

  CInfHandle(const string& InfFileName);

  ~CInfHandle();

  bool FindFirstLine(const string& Section, const string& Key);

  bool FindNextLine();

  string GetLineText();

  string GetLineText(const string &Section, const string& Key);

private:
  HINF _InfHandle; 
  UINT _ErrorLine;
  INFCONTEXT _Context;
};

#endif __INFHANDLE__