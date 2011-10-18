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
#include "ErrorFmt.h"

#include "autovec.h"
#include "ProcessList.h"
#include "Wildcards.h"

using namespace sc;

CProcess::CProcess(void){}
CProcess::~CProcess(void){}

CProcessList::CProcessList(void) {
}

CProcessList::~CProcessList(void) {
}

ActiveProcessesObj& CProcessList::FindProcessesObj(const char *ProcessName){

  this->_ActiveProcessesObjList.clear();
  this->_LastError = "";

  HANDLE         hProcessSnap = NULL; 
  PROCESSENTRY32 pe32         = {0}; 

  //  Take a snapshot of all processes in the system. 

  hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); 

	if (hProcessSnap == INVALID_HANDLE_VALUE) {
    int e = GetLastError();
    _LastError = CErrorFmt::FormatError(e, "CreateToolHelp32Snapshot()");
    return this->_ActiveProcessesObjList ; 
	}
 
  //  Fill in the size of the structure before using it. 

  pe32.dwSize = sizeof(PROCESSENTRY32); 
 
  //  Walk the snapshot of the processes, and for each process, 
  //  display information. 

  if (Process32First(hProcessSnap, &pe32))   { 

    do { 
      string vProcessName = pe32.szExeFile;
      if ( Wildcard::wildcardfit(ProcessName, vProcessName.c_str()) ) {
		  CProcess cpr;
		  cpr.processName = vProcessName;
		  cpr.processID = pe32.th32ProcessID;

        this->_ActiveProcessesObjList.push_back(cpr);
      }
    } while (Process32Next(hProcessSnap, &pe32)); 
  } 
  else {
    int e = GetLastError();
    _LastError = CErrorFmt::FormatError(e, "CreateToolHelp32Snapshot()");
  }
 
  // Do not forget to clean up the snapshot object. 

  CloseHandle (hProcessSnap); 
  return this->_ActiveProcessesObjList ; 

} 


ActiveProcesses& CProcessList::FindProcesses(const char *ProcessName){

  this->_ActiveProcessesList.clear();
  this->_LastError = "";

  HANDLE         hProcessSnap = NULL; 
  PROCESSENTRY32 pe32         = {0}; 

  //  Take a snapshot of all processes in the system. 

  hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); 

	if (hProcessSnap == INVALID_HANDLE_VALUE) {
    int e = GetLastError();
    _LastError = CErrorFmt::FormatError(e, "CreateToolHelp32Snapshot()");
    return this->_ActiveProcessesList ; 
	}
 
  //  Fill in the size of the structure before using it. 

  pe32.dwSize = sizeof(PROCESSENTRY32); 
 
  //  Walk the snapshot of the processes, and for each process, 
  //  display information. 

  if (Process32First(hProcessSnap, &pe32))   { 

    do { 
      string vProcessName = pe32.szExeFile;
      if ( Wildcard::wildcardfit(ProcessName, vProcessName.c_str()) ) {
        this->_ActiveProcessesList.push_back(vProcessName);
      }
    } while (Process32Next(hProcessSnap, &pe32)); 
  } 
  else {
    int e = GetLastError();
    _LastError = CErrorFmt::FormatError(e, "CreateToolHelp32Snapshot()");
  }
 
  // Do not forget to clean up the snapshot object. 

  CloseHandle (hProcessSnap); 
  return this->_ActiveProcessesList ; 

} 

bool CProcessList::BuildActiveProcessesList(void) {
  
  this->_ActiveProcessesList.clear();
  this->_LastError = "";

  HANDLE         hProcessSnap = NULL; 
  bool           bRet         = false; 
  PROCESSENTRY32 pe32         = {0}; 
 
  //  Take a snapshot of all processes in the system. 

  hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); 

	if (hProcessSnap == INVALID_HANDLE_VALUE) {
    int e = GetLastError();
    _LastError = CErrorFmt::FormatError(e, "CreateToolHelp32Snapshot()");
    return (false); 
  }
 
  //  Fill in the size of the structure before using it. 

  pe32.dwSize = sizeof(PROCESSENTRY32); 
 
  //  Walk the snapshot of the processes, and for each process, 
  //  display information. 

  if (Process32First(hProcessSnap, &pe32)) { 
    do { 
      string ProcessName = pe32.szExeFile;
	    this->_ActiveProcessesList.push_back(ProcessName);
    } while (Process32Next(hProcessSnap, &pe32)); 
    bRet = true; 
  }	else {
    int e = GetLastError();
    _LastError = CErrorFmt::FormatError(e, "CreateToolHelp32Snapshot()");
		bRet = false;    // could not walk the list of processes 
  }
 
  // Do not forget to clean up the snapshot object. 

  CloseHandle (hProcessSnap); 
  return (bRet); 

}
string CProcessList::killProcess(DWORD processID) {
	string _exitCode = "";
	string _errorMessage = "";
	string _result = "";

	HANDLE hProcess = OpenProcess(PROCESS_TERMINATE,FALSE,processID);
	UINT uExitCode = 0;
	LPDWORD lpExitCode =0 ;
	if (TerminateProcess(hProcess,uExitCode)) 
	{
		  //GetExitCodeProcess(hProcess,lpExitCode);
		  _result = "KILLED";

	}
	else {
	    _errorMessage = CErrorFmt::FormatError(GetLastError(), "CreateToolHelp32Snapshot()");
		_result = "NOT KILLED";
	};
	CloseHandle(hProcess);

	//return "<result><exitCode>"+_exitCode+"</exitCode><errorMessage>"+_errorMessage+"</errorMessage></result>";
	return _result;
    
}