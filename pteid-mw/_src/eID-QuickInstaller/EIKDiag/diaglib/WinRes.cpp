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

#include "WinRes.h"

#include <fstream>
#include <iostream>
#include <sstream>


WinRes::WinRes()
{

}

WinRes::~WinRes()
{

}

string WinRes:: LoadStringResource( string CustomResName, int ResourceId)
{
    HGLOBAL hResourceLoaded;		// handle to loaded resource 
    HRSRC hRes;						      // handle/ptr. to res. info. 
    char *lpResLock;				    // pointer to resource data 
    DWORD dwSizeRes;

    typedef basic_stringstream<TCHAR> stringstream;


    // find location of the resource and get handle to it
    hRes = FindResource( NULL, MAKEINTRESOURCE(ResourceId), CustomResName.c_str() );

    // loads the specified resource into global memory. 
    hResourceLoaded = LoadResource(NULL, hRes); 

    // get a pointer to the loaded resource!
    lpResLock = reinterpret_cast<char*>(LockResource(hResourceLoaded)); 

    // determine the size of the resource, so we know how much to write out to file!  
    dwSizeRes = SizeofResource(NULL, hRes);

    stringstream ss; 

    ss.write(lpResLock, dwSizeRes);

    return ss.str();

}

BinRes WinRes::LoadBinaryResource(string CustomResName, int ResourceId) {

    HGLOBAL hResourceLoaded;		// handle to loaded resource 
    HRSRC hRes;					    // handle/ptr. to res. info. 
    BYTE *lpResLock;				// pointer to resource data 
    DWORD dwSizeRes;
    BinRes Result;

    // find location of the resource and get handle to it
    hRes = FindResource( NULL, MAKEINTRESOURCE(ResourceId), CustomResName.c_str() );

    // loads the specified resource into global memory. 
    hResourceLoaded = LoadResource(NULL, hRes); 

    // get a pointer to the loaded resource!
    lpResLock = reinterpret_cast<byte*>(LockResource(hResourceLoaded)); 

    // determine the size of the resource, so we know how much to write out to file!  
    dwSizeRes = SizeofResource(NULL, hRes);

    for (int i = 0; i < (int)dwSizeRes; ++i)
        Result.push_back(lpResLock[i]);

    return Result;   

}


void WinRes::SaveBinaryResource( string CustomResName, int ResourceId, string OutputName )
{
    HGLOBAL hResourceLoaded;		// handle to loaded resource 
    HRSRC hRes;					    // handle/ptr. to res. info. 
    char *lpResLock;			    // pointer to resource data 
    DWORD dwSizeRes;
    string strOutputLocation;
    string strAppLocation; 

    // find location of the resource and get handle to it
    hRes = FindResource( NULL, MAKEINTRESOURCE(ResourceId), CustomResName.c_str() );

    // loads the specified resource into global memory. 
    hResourceLoaded = LoadResource( NULL, hRes ); 

    // get a pointer to the loaded resource!
    lpResLock = reinterpret_cast<char*>(LockResource(hResourceLoaded)); 

    // determine the size of the resource, so we know how much to write out to file!  
    dwSizeRes = SizeofResource( NULL, hRes );

    ofstream outputFile(OutputName.c_str(), ios::binary);
    if ( ! outputFile.is_open() )
        throw "SaveBinaryResource write error.";

    outputFile.write((const char*)lpResLock, dwSizeRes);
    outputFile.close();
}


// retrieves the full path and file name for our executable file 
string WinRes::getAppLocation()
{

    TCHAR szPathName[MAX_PATH];
    string strPath;

    GetModuleFileName(NULL, szPathName, MAX_PATH);

    strPath = szPathName;

    int slashPos = static_cast<int>(strPath.rfind('\\'));

    if(slashPos == strPath.npos)
        throw "Unable to get exe location";

    strPath = strPath.substr(0, slashPos);

    return strPath;
}

