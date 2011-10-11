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
#include "stdafx.h"
#include "Kill_Process.h"
#include "Tlhelp32.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//#define LOGFILE L"c:\\temp\\KillProcess.log"

using namespace std;

int APIENTRY _tWinMain(HINSTANCE hInstance,
                       HINSTANCE hPrevInstance,
                       LPTSTR    lpCmdLine,
                       int       nCmdShow) 
{

    int nRetCode = 0;

    LPWSTR *szArglist;
    int nArgs;

	bool bFound = false;

    szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);
    if( nArgs == 2 )  {
        LPWSTR filename = szArglist[1];

#ifdef LOGFILE
		FILE *f=NULL;
		_wfopen_s(&f, LOGFILE, L"a");
		if (f)
		{
			fwprintf_s(f,L"Ask for killing %s\n",filename);
			fclose(f);
			f=NULL;
		}
#endif

        HANDLE hndl = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
        DWORD dwsma = GetLastError();

        DWORD dwExitCode = 0;

        PROCESSENTRY32  procEntry={0};
        procEntry.dwSize = sizeof( PROCESSENTRY32 );
        Process32First(hndl,&procEntry);
        do 
		{
            if(!_wcsicmp(procEntry.szExeFile,filename))	
			{
				bFound = true;
                break;
            }
        } while(Process32Next(hndl,&procEntry));

        ///////////////////////////////////////////////////////////////	
		if(bFound)
		{
			HANDLE hHandle;
			hHandle = ::OpenProcess(PROCESS_TERMINATE,0,procEntry.th32ProcessID);

			::TerminateProcess(hHandle,dwExitCode);
			::GetExitCodeProcess(hHandle,&dwExitCode);

#ifdef LOGFILE
			_wfopen_s(&f, LOGFILE, L"a");
			if (f)
			{
				fwprintf_s(f,L" --> Found and killed (%ld)\n",dwExitCode);
				fclose(f);
				f=NULL;
			}
#endif
		}
		else
		{
#ifdef LOGFILE
			_wfopen_s(&f, LOGFILE, L"a");
			if (f)
			{
				fwprintf_s(f,L" --> NOT found");
				fclose(f);
				f=NULL;
			}
#endif
		}

        return dwExitCode;
    }

	return 0;
}


