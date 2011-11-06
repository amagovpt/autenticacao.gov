/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2010 FedICT.
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
#include <windows.h>
#include <aclapi.h>
#include "Tlhelp32.h" 
#include <psapi.h>

#include "process.h"
#include "error.h"
#include "log.h"
#include "progress.h"

#define G_BUFFER_SIZE 32767
static TCHAR g_buffer[G_BUFFER_SIZE];

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PRIVATE FUNCTIONS DECLARATION ////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
int processFillList(Proc_LIST *processList, const wchar_t *processName);
int processWaitFromHandle(HANDLE hHandle, int *exitCode, int waitTimeSecs);
bool EnableTokenPrivilege(HANDLE htok, LPCTSTR szPrivilege, TOKEN_PRIVILEGES *tpOld);
bool EnablePrivilege(LPCTSTR szPrivilege);
int isModuleUsedByProcess(DWORD dwPID, LPCTSTR szLibrary, bool *found);
bool AdjustDacl(HANDLE h, DWORD dwDesiredAccess);
HANDLE AdvanceOpenProcess(DWORD pid, DWORD dwAccessRights);

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PUBLIC FUNCTIONS /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
int processUsingLibrary(Lib_ID library, Proc_LIST *processList)
{
	int iReturnCode = DIAGLIB_OK;

	if(library.empty() || processList==NULL)
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;
	}

	LOG_TIME(L"Check for use of library '%ls' --> ",library.c_str());

	processList->clear();

	HANDLE hProcessSnap;
    HANDLE hProcess;
    PROCESSENTRY32 pe32;
	bool bUsed=false;

	if(INVALID_HANDLE_VALUE == ( hProcessSnap=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0)))
	{
		LOG_LASTERROR(L"CreateToolhelp32Snapshot failed");
		return RETURN_LOG_INTERNAL_ERROR;
	}

    pe32.dwSize = sizeof(PROCESSENTRY32);

	//Get progressMax if request
	int progressMax = 0;
	if (!Process32First(hProcessSnap, &pe32))
	{
		LOG_LASTERROR(L"Process32First failed");
		if(!CloseHandle( hProcessSnap ))
		{
			LOG_LASTERROR(L"CloseHandle failed");
		}
		return RETURN_LOG_INTERNAL_ERROR;
	}

	do
	{
		progressMax++;
	} while (Process32Next(hProcessSnap, &pe32));


    if (!Process32First(hProcessSnap, &pe32))
    {
 		LOG_LASTERROR(L"Process32First failed");
		if(!CloseHandle( hProcessSnap ))
		{
			LOG_LASTERROR(L"CloseHandle failed");
		}
		return RETURN_LOG_INTERNAL_ERROR;
    }

	progressInit(progressMax);

	int count=0;
    do
    {
		//We avoid the process System because the isModuleUsedByProcess return an error
		if ( 0 != lstrcmpi(L"System", pe32.szExeFile))
		{
			hProcess = AdvanceOpenProcess(pe32.th32ProcessID, PROCESS_ALL_ACCESS);
			//If we need the exe name, we need pe32.szExeFile
			if(DIAGLIB_OK != isModuleUsedByProcess(pe32.th32ProcessID, library.c_str(),&bUsed))
			{
				LOG(L"isModuleUsedByProcess failed with Process '%ls' (pid=%ld)\n",pe32.szExeFile,pe32.th32ProcessID);
			}
			else
			{
				if(bUsed)
				{
					processList->push_back(pe32.th32ProcessID);
					LOG(L"FOUND Process '%ls' (pid=%ld)\n",pe32.szExeFile,pe32.th32ProcessID);
				}
			}
		}
		progressIncrement();

    } while (Process32Next(hProcessSnap, &pe32));

	progressRelease();

	if(!CloseHandle( hProcessSnap ))
	{
		LOG_LASTERROR(L"CloseHandle failed");
	}

	if(!bUsed) LOG(L"NOT USED\n");

	return iReturnCode;
} 

////////////////////////////////////////////////////////////////////////////////////////////////
int processGetInfo(Proc_ID process, Proc_INFO *info)
{
	int iReturnCode = DIAGLIB_OK;

	if(process == 0)
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;
	}

	if(info==NULL)
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;
	}

	LOG_TIME(L"Ask info on pid=%ld --> ",process);
	
	DWORD  bufferSize;

	HANDLE hHandle = NULL;
	if(NULL == (hHandle = ::OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,0,process)))
	{
		if(ERROR_ACCESS_DENIED != GetLastError())
		{
			LOG_LASTERROR(L"OpenProcess failed");
			return RETURN_LOG_INTERNAL_ERROR;
		}
		else
		{
			LOG(L"ACCESS DENIED\n");
			return DIAGLIB_ERR_PROCESS_ACCESS_DENIED;
		}
	}

	info->id=process;

	bufferSize = G_BUFFER_SIZE;
	if(!GetModuleBaseName(hHandle,NULL,g_buffer,bufferSize)) 
	{
		LOG_LASTERROR(L"GetModuleBaseName failed");
		iReturnCode = DIAGLIB_ERR_INTERNAL;
	}
	else
	{
		info->Name=g_buffer;
	}

	bufferSize = G_BUFFER_SIZE;
	if(!GetModuleFileNameEx(hHandle,NULL,g_buffer,bufferSize)) 
	{
		LOG_LASTERROR(L"GetModuleFileNameEx failed");
		iReturnCode = DIAGLIB_ERR_INTERNAL;
	}
	else
	{
		info->Path=g_buffer;
	}

	bufferSize = G_BUFFER_SIZE;
	if(!GetProcessImageFileName(hHandle,g_buffer,bufferSize))
	{
		LOG_LASTERROR(L"QueryFullProcessImageName failed");
		iReturnCode = DIAGLIB_ERR_INTERNAL;
	}
	else
	{
		info->FullPath=g_buffer;
	}

	HMODULE modulesFound[1024];
	DWORD spaceActuallyRequired;

	if(!EnumProcessModules(hHandle,modulesFound,1024,&spaceActuallyRequired))
	{
		LOG_LASTERROR(L"EnumProcessModules failed");
	}
	else
	{
		info->modulesLoaded.clear();
		for(unsigned int i=0;i<(spaceActuallyRequired/sizeof(HMODULE));i++)
		{
			TCHAR szModName[MAX_PATH];

			if(GetModuleBaseName(hHandle,modulesFound[i],szModName,sizeof(szModName)/sizeof(TCHAR)))
				info->modulesLoaded.insert(szModName);
            //if(GetModuleFileNameEx(hHandle,modulesFound[i],szModName,sizeof(szModName)/sizeof(TCHAR)))
				//modInfo.path=szModName;
		}
	}

	if(!CloseHandle( hHandle ))
	{
		LOG_LASTERROR(L"CloseHandle failed");
	}

	LOG(L"DONE\n");

	return iReturnCode;
} 

////////////////////////////////////////////////////////////////////////////////////////////////
int processKill(Proc_ID process)
{
	int iReturnCode = DIAGLIB_OK;

	if(process == 0)
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;
	}

	LOG_TIME(L"Ask for killing pid=%ld --> ",process);
			
	HANDLE hHandle = NULL;
	if(NULL == (hHandle = ::OpenProcess(PROCESS_TERMINATE | PROCESS_QUERY_INFORMATION,0,process)))
	{
		LOG_LASTERROR(L"OpenProcess failed");
		return RETURN_LOG_INTERNAL_ERROR;
	}
	DWORD dwExitCode = 0;

	if(!::TerminateProcess(hHandle,dwExitCode))
	{
		LOG_LASTERROR(L"TerminateProcess failed");
		iReturnCode = DIAGLIB_ERR_PROCESS_KILL_FAILED;
	} 
	else
	{
		if(!::GetExitCodeProcess(hHandle,&dwExitCode))
		{
			LOG_LASTERROR(L"GetExitCodeProcess failed");
			iReturnCode = RETURN_LOG_INTERNAL_ERROR;
		}
		else
		{
			LOG(L"FOUND and KILLED (Return code = %ld)\n",dwExitCode);
		}
	}

	if(!CloseHandle( hHandle ))
	{
		LOG_LASTERROR(L"CloseHandle failed");
	}

	return iReturnCode;
} 

////////////////////////////////////////////////////////////////////////////////////////////////
int processStart(Proc_NAME process, Proc_ID *id, int waitTimeSecs, int *exitCode)
{
	int iReturnCode = DIAGLIB_OK;

	if(process.empty() || id == NULL)
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;
	}

	LOG_TIME(L"Ask for start process named '%ls' --> ",process.c_str());

	STARTUPINFO siStartupInfo;
    PROCESS_INFORMATION piProcessInfo;
    ZeroMemory(&siStartupInfo, sizeof(siStartupInfo));
    ZeroMemory(&piProcessInfo, sizeof(piProcessInfo));
    siStartupInfo.cb = sizeof(siStartupInfo);

    if (!CreateProcess(NULL,(LPWSTR)process.c_str(),0,0,false,0,NULL,NULL, &siStartupInfo, &piProcessInfo)) 
	{ 
		LOG_LASTERROR(L"CreateProcess failed");
		iReturnCode = DIAGLIB_ERR_PROCESS_START_FAILED;
	}
	else
	{
		*id=piProcessInfo.dwProcessId;
		Sleep(100);
		LOG(L"STARTED with pid=%ld\n",*id);
		if(waitTimeSecs>0)
		{
			iReturnCode=processWait(*id, exitCode, waitTimeSecs);
		}
	}

	return iReturnCode;
} 

////////////////////////////////////////////////////////////////////////////////////////////////
int processStartAsAdmin(Proc_NAME process, Proc_ID *id, int waitTimeSecs, int *exitCode)
{
	int iReturnCode = DIAGLIB_OK;

	return RETURN_LOG_ERROR(DIAGLIB_ERR_NOT_AVAILABLE);

	return iReturnCode;
} 

////////////////////////////////////////////////////////////////////////////////////////////////
int processWait(Proc_ID process, int *exitCode, int waitTimeSecs)
{
	int iReturnCode = DIAGLIB_OK;

	if(process == 0)
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;
	}

	LOG_TIME(L"Waiting for process pid=%ld --> ",process);

	HANDLE hHandle;

	if(NULL == (hHandle = ::OpenProcess(SYNCHRONIZE | PROCESS_QUERY_INFORMATION,0,process)))
	{
		LOG_LASTERROR(L"OpenProcess failed");
		return RETURN_LOG_INTERNAL_ERROR;
	}

	if(DIAGLIB_OK == (iReturnCode = processWaitFromHandle(hHandle, exitCode, waitTimeSecs)))
	{
		LOG(L"DONE\n");
	}

	if(!CloseHandle( hHandle ))
	{
		LOG_LASTERROR(L"CloseHandle failed");
	}

	return iReturnCode;
} 

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PRIVATE FUNCTIONS ////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
int processFillList(Proc_LIST *processList, const wchar_t *processName)
{
	int iReturnCode = DIAGLIB_OK;

	if(processList == NULL)
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;
	}

	if(processName)
	{
		LOG_TIME(L"Ask for process list (with name = %ls) --> ",processName);
	}
	else
	{
		LOG_TIME(L"Ask for list of all process --> ");
	}

	processList->clear();

	EnablePrivilege(SE_DEBUG_NAME);
	HANDLE hndl;
	if(INVALID_HANDLE_VALUE == ( hndl=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0)))
	{
		LOG_LASTERROR(L"CreateToolhelp32Snapshot failed");
		return RETURN_LOG_INTERNAL_ERROR;
	}

	PROCESSENTRY32  procEntry={0};
	procEntry.dwSize = sizeof( PROCESSENTRY32 );

	//Get progressMax if request
	int progressMax = 0;
	if (!Process32First(hndl, &procEntry))
	{
		LOG_LASTERROR(L"Process32First failed");
		if(!CloseHandle( hndl ))
		{
			LOG_LASTERROR(L"CloseHandle failed");
		}
		return RETURN_LOG_INTERNAL_ERROR;
	}
	do
	{
		progressMax++;
	} while (Process32Next(hndl, &procEntry));

	if(!Process32First(hndl,&procEntry))
	{
		LOG_LASTERROR(L"Process32First failed");
		if(!CloseHandle( hndl ))
		{
			LOG_LASTERROR(L"CloseHandle failed");
		}
		return RETURN_LOG_INTERNAL_ERROR;
	}

	progressInit(progressMax);

	do 
	{
		if(procEntry.th32ProcessID != 0)
		{
			if(processName == NULL)
			{
				processList->push_back(procEntry.th32ProcessID);
			}
			else if(0 == _wcsicmp(procEntry.szExeFile,processName))	
			{
				processList->push_back(procEntry.th32ProcessID);
			}
		}

		progressIncrement();

	} while(Process32Next(hndl,&procEntry));

	progressRelease();

	if(!CloseHandle( hndl ))
	{
		LOG_LASTERROR(L"CloseHandle failed");
	}

	if(processList->size() == 0)
	{
		LOG(L"NO PROCESS FOUND\n");
	}
	else
	{
		LOG(L"pid=");
		for (unsigned long i=0; i < processList->size() ; i++)
		{
			LOG(L"%ld,",processList->at(i));
		}
		LOG(L"DONE\n");
	}

	return iReturnCode;
} 

////////////////////////////////////////////////////////////////////////////////////////////////
int processWaitFromHandle(HANDLE hHandle, int *exitCode, int waitTimeSecs)
{
	int iReturnCode = DIAGLIB_OK;

	if(hHandle == 0)
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;
	}

    DWORD dwExitCode = WaitForSingleObject(hHandle, waitTimeSecs*1000);
	if (dwExitCode == WAIT_TIMEOUT) 
	{
		LOG_LASTERROR(L"TIMEOUT");
		iReturnCode = DIAGLIB_ERR_PROCESS_WAIT_TIMEOUT;
	}
	else if (dwExitCode != WAIT_OBJECT_0)
	{
		LOG(L"WaitForSingleObject failed\n");
		iReturnCode = DIAGLIB_ERR_INTERNAL;
	}
	if(iReturnCode == DIAGLIB_OK)
	{
		if(!GetExitCodeProcess(hHandle,&dwExitCode))
		{
			LOG_LASTERROR(L"GetExitCodeProcess failed");
		}
		else if(dwExitCode==3010)
		{
			// Process ended with non-zero exit code
			LOG(L"WARNING: A reboot is needed\n");
			iReturnCode=DIAGLIB_REBOOT_NEEDED;
		}
		else
		{
			LOG(L"QUIT NORMALLY\n");
		}
	}

	if(exitCode) *exitCode=dwExitCode;

	return iReturnCode;
} 

////////////////////////////////////////////////////////////////////////////////////////////////
bool EnableTokenPrivilege(HANDLE htok, LPCTSTR szPrivilege, TOKEN_PRIVILEGES *tpOld)
{
    TOKEN_PRIVILEGES tp;
    tp.PrivilegeCount = 1;
    tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    if (LookupPrivilegeValue(0, szPrivilege, &tp.Privileges[0].Luid))
    {
        DWORD cbOld = sizeof (*tpOld);

        if (AdjustTokenPrivileges(htok, FALSE, &tp, cbOld, tpOld, &cbOld))
        {
            return (ERROR_NOT_ALL_ASSIGNED != GetLastError());
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////
bool EnablePrivilege(LPCTSTR szPrivilege)
{
    BOOL bReturn = FALSE;
    HANDLE hToken;
    TOKEN_PRIVILEGES tpOld;

    if (!OpenProcessToken(GetCurrentProcess(), 
         TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES, &hToken))
    {
        return false;
    }

    bReturn = (EnableTokenPrivilege(hToken, szPrivilege, &tpOld));
    CloseHandle(hToken);

	return (bReturn?true:false);
}

////////////////////////////////////////////////////////////////////////////////////////////////
int isModuleUsedByProcess(DWORD dwPID, LPCTSTR szLibrary, bool *found)
{
	int iReturnCode = DIAGLIB_OK;

    HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
    MODULEENTRY32 me32;
	*found=false;

    EnablePrivilege(SE_DEBUG_NAME);
	if(INVALID_HANDLE_VALUE == ( hModuleSnap=CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID)))
	{
		LOG_LASTERROR(L"CreateToolhelp32Snapshot failed");
		return RETURN_LOG_INTERNAL_ERROR;
	}

	me32.dwSize = sizeof(MODULEENTRY32);

    if (!Module32First (hModuleSnap, &me32))
    {
 		LOG_LASTERROR(L"Module32First failed");
		if(!CloseHandle( hModuleSnap ))
		{
			LOG_LASTERROR(L"CloseHandle failed");
		}
		return RETURN_LOG_INTERNAL_ERROR;
    }

    do
    {
        if (!lstrcmpi(szLibrary, me32.szModule))
        {
			*found=true;
            break;
        }

    } while (Module32Next(hModuleSnap, &me32));

	if(!CloseHandle( hModuleSnap ))
	{
		LOG_LASTERROR(L"CloseHandle failed");
	}

    return iReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
bool AdjustDacl(HANDLE h, DWORD dwDesiredAccess)
{
    SID world = { SID_REVISION, 1, SECURITY_WORLD_SID_AUTHORITY, 0 };

    EXPLICIT_ACCESS ea =
    {
        0,
            SET_ACCESS,
            NO_INHERITANCE,
        {
            0, NO_MULTIPLE_TRUSTEE,
                TRUSTEE_IS_SID,
                TRUSTEE_IS_USER,
                0
        }
    };

    ACL* pdacl = 0;
    DWORD err = SetEntriesInAcl(1, &ea, 0, &pdacl);

    ea.grfAccessPermissions = dwDesiredAccess;
    ea.Trustee.ptstrName = (LPTSTR)(&world);

    if (err == ERROR_SUCCESS)
    {
        err = SetSecurityInfo(h, SE_KERNEL_OBJECT, 
              DACL_SECURITY_INFORMATION, 0, 0, pdacl, 0);
        LocalFree(pdacl);

        return (err == ERROR_SUCCESS);
    }
    else
    {
        return false;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////
HANDLE AdvanceOpenProcess(DWORD pid, DWORD dwAccessRights)
{
    HANDLE hProcess = OpenProcess(dwAccessRights, FALSE, pid);

    if (hProcess == NULL)
    {
        HANDLE hpWriteDAC = OpenProcess(WRITE_DAC, FALSE, pid);

        if (hpWriteDAC == NULL)
        {
            HANDLE htok;
            TOKEN_PRIVILEGES tpOld;

            if (!OpenProcessToken(GetCurrentProcess(), 
                 TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES, &htok))
            {
                return false;
            }

            if (EnableTokenPrivilege(htok, SE_TAKE_OWNERSHIP_NAME, &tpOld))
            {
                HANDLE hpWriteOwner = OpenProcess(WRITE_OWNER, FALSE, pid);

                if (hpWriteOwner != NULL)
                {
                    BYTE buf[512];
                    DWORD cb = sizeof buf;

                    if (GetTokenInformation(htok, TokenUser, buf, cb, &cb))
                    {
                        DWORD err = SetSecurityInfo(hpWriteOwner, SE_KERNEL_OBJECT, 
                                    OWNER_SECURITY_INFORMATION, 
                                    ((TOKEN_USER *)(buf))->User.Sid, 0, 0, 0);
                        
                        if (err == ERROR_SUCCESS)
                        {
                            if (!DuplicateHandle(GetCurrentProcess(), hpWriteOwner, 
                                 GetCurrentProcess(), &hpWriteDAC, 
                                 WRITE_DAC, FALSE, 0))
                            {
                                hpWriteDAC = NULL;
                            }
                        }
                    }

                    CloseHandle(hpWriteOwner);
                }

                AdjustTokenPrivileges(htok, FALSE, &tpOld, 0, 0, 0);
            }

            CloseHandle(htok);
        }

        if (hpWriteDAC)
        {
            AdjustDacl(hpWriteDAC, dwAccessRights);

            if (!DuplicateHandle(GetCurrentProcess(), hpWriteDAC, 
                GetCurrentProcess(), &hProcess, dwAccessRights, FALSE, 0))
            {
                hProcess = NULL;
            }

            CloseHandle(hpWriteDAC);
        }
    }

    return (hProcess);
}
 
