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
// MarkHidden.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "MarkHidden.h"


int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow) {

   LPWSTR *szArglist;
   int nArgs;

   szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);
   if( nArgs == 2 )  {
       LPWSTR filename = szArglist[1];
       //HANDLE hf = CreateFile(L"c:\\temp\\logwix.txt", GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
       //if (hf != INVALID_HANDLE_VALUE) {
       //    DWORD nw;
       //    WriteFile(hf, filename, wcslen(filename) * 2, &nw, NULL); 
       //    CloseHandle(hf);
       //}
       DWORD dwAttrs = GetFileAttributes(filename);
       if (dwAttrs != INVALID_FILE_ATTRIBUTES) {
           dwAttrs |= FILE_ATTRIBUTE_HIDDEN;
           SetFileAttributes(filename, dwAttrs);
       }
   }

   // Free memory allocated for CommandLineToArgvW arguments.

   LocalFree(szArglist);

   return(0);

    

}



