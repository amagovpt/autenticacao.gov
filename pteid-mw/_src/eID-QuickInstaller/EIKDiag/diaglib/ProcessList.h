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

#ifndef __PROCESSLIST__
#define __PROCESSLIST__

#pragma once

#undef UNICODE

#include "stdafx.h"
#include <windows.h>
#include <string>
#include <tlhelp32.h>
#include <stdio.h>

#include <vector>

using namespace std;


typedef vector<string> ActiveProcesses;
typedef ActiveProcesses::iterator ActiveProcessesIterator;



class CProcess {
public:
    CProcess(void);
	virtual ~CProcess(void);

	string processName;
	DWORD processID;
};

typedef vector<CProcess> ActiveProcessesObj;
typedef ActiveProcessesObj::iterator ActiveProcessesObjIterator;


class CProcessList {
public:
  CProcessList(void);
  virtual ~CProcessList(void);

  ActiveProcessesIterator GetFirst()      { return _ActiveProcessesList.begin(); };
  ActiveProcessesIterator GetLast()       { return _ActiveProcessesList.end();   };
  size_t Count()                          { return _ActiveProcessesList.size();  };
  ActiveProcesses& ActiveProcessesList()  { return _ActiveProcessesList;         }; 

  ActiveProcessesObjIterator ObjGetFirst()      { return _ActiveProcessesObjList.begin(); };
  ActiveProcessesObjIterator ObjGetLast()       { return _ActiveProcessesObjList.end();   };
  size_t ObjCount()                          { return _ActiveProcessesObjList.size();  };
  ActiveProcessesObj& ActiveProcessesObjList()  { return _ActiveProcessesObjList;         }; 


  string& LastError()                     { return _LastError;                   };
  bool BuildActiveProcessesList(void);
  ActiveProcesses& FindProcesses(const char *ProcessName); 
  ActiveProcessesObj& CProcessList::FindProcessesObj(const char *ProcessName);

  string killProcess(DWORD processID);


private:
  ActiveProcesses _ActiveProcessesList;
  ActiveProcessesObj _ActiveProcessesObjList;
  string _LastError;
};


#endif __PROCESSLIST__