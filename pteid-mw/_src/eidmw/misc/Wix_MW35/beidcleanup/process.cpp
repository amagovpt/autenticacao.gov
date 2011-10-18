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
#include <windows.h>
#include <aclapi.h>
#include "Tlhelp32.h"

#include "process.h"
#include "registry.h"
#include "error.h"
#include "log.h"

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PRIVATE FUNCTIONS ////////////////////////////////////////////////
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

bool isModuleUsedByProcess(DWORD dwPID, LPCTSTR szLibrary)
{
    HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
    MODULEENTRY32 me32;
	bool bFound=false;

    EnablePrivilege(SE_DEBUG_NAME);
    hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID);

    if (hModuleSnap == (HANDLE) - 1)
    {
        return false;
    }

    me32.dwSize = sizeof(MODULEENTRY32);

    if (!Module32First (hModuleSnap, &me32))
    {
        CloseHandle(hModuleSnap);
        return false;
    }

    do
    {
        if (!lstrcmpi(szLibrary, me32.szModule))
        {
			bFound=true;
            break;
        }

    } while (Module32Next(hModuleSnap, &me32));

    CloseHandle(hModuleSnap);

    return bFound;
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
 
////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PUBLIC FUNCTIONS /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

int KillProcess(const wchar_t *process)
{
	int iReturnCode = RETURN_OK;

	LOG(L"Ask for killing %s\n",process);

	bool bNone= true;

    do 
	{
		bool bFound= false;
		DWORD pid=0;

		EnablePrivilege(SE_DEBUG_NAME);
		HANDLE hndl = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
		DWORD dwsma = GetLastError();

		PROCESSENTRY32  procEntry={0};
		procEntry.dwSize = sizeof( PROCESSENTRY32 );
		Process32First(hndl,&procEntry);
		do 
		{
			if(!_wcsicmp(procEntry.szExeFile,process))	
			{
				bFound = true;
				bNone = false;
				pid = procEntry.th32ProcessID;
				break;
			}
		} while(Process32Next(hndl,&procEntry));

		CloseHandle( hndl );

		//Kill the process
		if(bFound)
		{
			
			HANDLE hHandle = NULL;
			if(NULL != (hHandle = ::OpenProcess(PROCESS_TERMINATE,0,procEntry.th32ProcessID)))
			{
				DWORD dwExitCode = 0;

				if(::TerminateProcess(hHandle,dwExitCode))
				{
					::GetExitCodeProcess(hHandle,&dwExitCode);
					LOG(L" --> FOUND and KILLED pid=%ld (Return code = %ld)\n",pid,dwExitCode);
				}
				else
				{
					LOG(L" --> ERROR - Failed to kill pid=%ld (Error code from TerminateProcess = %ld)\n",pid,GetLastError());
					iReturnCode = RETURN_ERR_KILLPROCESS_FAILED;
				} 
				CloseHandle( hHandle );
				Sleep(500);
			}
			else
			{
				LOG(L" --> ERROR - Failed to kill pid=%ld (Error code from OpenProcess = %ld)\n",pid,GetLastError());
				iReturnCode = RETURN_ERR_KILLPROCESS_FAILED;
			}
		}
		else
		{
			break;
		}

		if(iReturnCode!=RETURN_OK)
			break;

	} while(1);

	if(bNone) LOG(L" --> NOT FOUND\n");

	LOG(L"\n");

	return iReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////

int LibraryUsage(LPCTSTR szLibrary,bool bForceRemove)
{
	LOG(L"Check for \"%s\" usage\n",szLibrary);

	HANDLE hProcessSnap;
    HANDLE hProcess;
    PROCESSENTRY32 pe32;
	bool bFound=false;
	bool bUsed=false;

    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (hProcessSnap == INVALID_HANDLE_VALUE)
    {
		LOG(L" --> ERROR - CreateToolhelp32Snapshot failed (LastError=%d)\n", GetLastError());
        return RETURN_ERR_INTERNAL;
    }

    pe32.dwSize = sizeof(PROCESSENTRY32);

    if (!Process32First(hProcessSnap, &pe32))
    {
 		LOG(L" --> ERROR - Process32First failed (LastError=%d)\n", GetLastError());
        CloseHandle(hProcessSnap);
        return RETURN_ERR_INTERNAL;
    }

    do
    {
        hProcess = AdvanceOpenProcess(pe32.th32ProcessID, PROCESS_ALL_ACCESS);
		//If we need the exe name, we need pe32.szExeFile
        if(isModuleUsedByProcess(pe32.th32ProcessID, szLibrary))
		{
			bUsed=true;
			DWORD pid = pe32.th32ProcessID;
			const wchar_t *wzExeFile = pe32.szExeFile;

			//If bForceRemove, we try to kill the process
			if(bForceRemove)
			{
				LOG(L" --> Used by \"%s\" (pid=%ld)\n",wzExeFile,pid);

				HANDLE hHandle = ::OpenProcess(PROCESS_TERMINATE,0,pid);

				DWORD dwExitCode = 0;

				if(::TerminateProcess(hHandle,dwExitCode))
				{
					::GetExitCodeProcess(hHandle,&dwExitCode);
					LOG(L" --> Killed process pid=%ld (Return code = %ld)\n",pid,dwExitCode);
				}
				else
				{
					LOG(L" --> ERROR - Could not kill process pid=%ld (LastError=%d)\n", pid, GetLastError());
					bFound=true;
					break;
				}
				CloseHandle( hHandle );

				Sleep(100);
			}
			else
			{
				LOG(L" --> ERROR - Used by \"%s\" (pid=%ld)\n",wzExeFile,pid);
				bFound=true;
				break;
			}
		}

    } while (Process32Next(hProcessSnap, &pe32));

    CloseHandle(hProcessSnap);

	if(!bUsed) LOG(L" --> NOT USED\n");

	LOG(L"\n");

	return (bFound?RETURN_ERR_FILELOCKED:RETURN_OK);
}

////////////////////////////////////////////////////////////////////////////////////////////////

int Execute(wchar_t *wzCommandLine,long lTimeout, bool *pbRebootNeeded, HKEY hRootKey,const wchar_t *wzKey)
{
	int iReturnCode = RETURN_OK;

	STARTUPINFO siStartupInfo;
    PROCESS_INFORMATION piProcessInfo;
    ZeroMemory(&siStartupInfo, sizeof(siStartupInfo));
    ZeroMemory(&piProcessInfo, sizeof(piProcessInfo));
    siStartupInfo.cb = sizeof(siStartupInfo);

    if (!CreateProcess(NULL,wzCommandLine,0,0,false,0,NULL,NULL, &siStartupInfo, &piProcessInfo)) 
	{ 
		LOG(L" --> ERROR - CreateProcess failed \"%s\" (LastError=%d)\n", wzCommandLine, GetLastError());
		iReturnCode = RETURN_ERR_EXECUTE_FAILED;
	}
	else
	{
        DWORD dwExitCode = WaitForSingleObject(piProcessInfo.hProcess, lTimeout*1000);
		if (dwExitCode == WAIT_TIMEOUT) 
		{
			LOG(L" --> ERROR - Timeout\n");
			iReturnCode = RETURN_ERR_EXECUTE_TIMOUT;
		}
		else if (dwExitCode != WAIT_OBJECT_0)
		{
			LOG(L" --> ERROR - Execution failed\n");
			iReturnCode = RETURN_ERR_EXECUTE_FAILED;
		}
		if(iReturnCode == RETURN_OK)
		{
			GetExitCodeProcess(piProcessInfo.hProcess,&dwExitCode);
            // Process ended with non-zero exit code
			if(dwExitCode==3010)
			{
				LOG(L" --> WARNING - Reboot is needed\n");
				*pbRebootNeeded=true;
			}

			if(hRootKey!=NULL && wzKey!=NULL)
			{
				long count=lTimeout;
				while(count>0 && RegistryKeyExist(hRootKey,wzKey))
				{
					Sleep(1000);
					count--;
				}
				if(RegistryKeyExist(hRootKey,wzKey))
				{
					LOG(L" --> ERROR - Timeout - Registry key still exist\n");
					iReturnCode = RETURN_ERR_EXECUTE_TIMOUT;
				}
			}
		}
	}

	

	return iReturnCode;
}