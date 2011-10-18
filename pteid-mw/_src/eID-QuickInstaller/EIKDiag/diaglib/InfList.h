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

#ifndef __INFLIST__
#define __INFLIST__

#pragma once

#undef UNICODE

#include <windows.h>
#include <setupapi.h>

#include "ErrorFmt.h"

#include <string>
#include <vector>

using namespace std;


typedef vector<string> InfFiles;
typedef InfFiles::iterator InfFilesIterator;

class CInfList {
public:
  CInfList(void);
  virtual ~CInfList(void);
  InfFilesIterator GetFirst() { return _InfFileList.begin(); };
  InfFilesIterator GetLast()  { return _InfFileList.end();   };
  size_t Count()              { return _InfFileList.size();  };
  InfFiles& InfFileList()     { return _InfFileList;         }; 
  string& LastError()         { return _LastError;           };
  bool BuildInfFileList(void);
private:

  InfFiles _InfFileList;
  string _LastError;
};


#endif __INFLIST__