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
/** \file ConfigReg.cpp
Class to set and get configuration-data from the registry(Windows) or the ini-file(Linux and Mac).

*/

//#include <utility>
#include <algorithm>
#include <fstream>
#include <functional>
#include <iostream>
#include <iterator>
#include <locale>
#include <string>
#include <shlobj.h>


#include <stdlib.h>
#include <exception>
#include <windows.h>

#include "MWException.h"
#include "Config.h"
#include "eidErrors.h"
#include "Util.h"




const wchar_t *SC_CONF_REG = L"Software\\PTEID";

namespace eIDMW
{ 

//constructor
CConfig::CConfig(void)
{
}

CConfig::~CConfig(void)
{
}

/** Expand $HOME expression in the section string

Search the first occurence of the $-sign, retrieve the $-string, replace with the correct value
    $home\eidpath

\return std::wstring            requested parameter
\retval EIMDW_NOT_OK            exception; when section   is not found
\retval EIDMW_ERR_PARAM_BAD     exception; when parameter is not found

\sa SetString(), DelString,   SetLong(), GetLong(), DelLong()  
*************************************************************************************/

std::wstring ExpandSection(
	const std::wstring & czSectionOriginal             /**< In:     the configuration-section where you can find above specified parameter */
    ) 
{
    HRESULT                          hResult;
    int                              iResult;
    basic_string <char>::size_type   iTotLenght = czSectionOriginal.length();
    basic_string <char>::size_type   iStrLenght;
    wchar_t                          wsSectionCustom[256]; 


    //--- Find if anything to expand
    if ((iTotLenght == 0)||(czSectionOriginal[0] != '$'))
    {
        //nothing to replace
        return(czSectionOriginal);
    }

    //--- check for EIDMW_CNF_MACRO_INSTALL
    iStrLenght = wcslen(EIDMW_CNF_MACRO_INSTALL);
    iResult = czSectionOriginal.compare(0, iStrLenght, EIDMW_CNF_MACRO_INSTALL);
    if (iResult == 0)
    {
        //replace EIDMW_CNF_MACRO_INSTALL
        std::wstring czSectionExpanded = CConfig::GetString(EIDMW_CNF_GENERAL_INSTALLDIR, EIDMW_CNF_SECTION_GENERAL);
        
        //add part after the $-macro
        czSectionExpanded.append(czSectionOriginal.substr(iStrLenght, iTotLenght-iStrLenght));//add part after the $-macro
        return(czSectionExpanded);
    }

    //--- check for EIDMW_CNF_MACRO_HOME
    // returns by default "C:\WINDOWS\system32\config\systemprofile\Application Data" for services.
    iStrLenght = wcslen(EIDMW_CNF_MACRO_HOME);
    iResult = czSectionOriginal.compare(0, iStrLenght, EIDMW_CNF_MACRO_HOME);
    if (iResult == 0)
    {
        //replace EIDMW_CNF_MACRO_HOME
        hResult = SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_DEFAULT, wsSectionCustom);

        //non-user SW(eg.: services) returns: C:\WINDOWS\system32\config\systemprofile\Application Data, replace by common dir
        if((hResult != S_OK) || ((hResult == S_OK) && (wcsstr(wsSectionCustom, L":\\WINDOWS") != NULL)))
        {
            //try common path when problems or when no user found
            hResult = SHGetFolderPath(NULL, CSIDL_COMMON_APPDATA, NULL, SHGFP_TYPE_DEFAULT, wsSectionCustom);
            if(hResult != S_OK)
            {
                //can not replace, return original string
                return(czSectionOriginal);
            }
        }
        std::wstring czSectionExpanded(wsSectionCustom);    

 	//	czSectionExpanded.append(WDIRSEP);
		//czSectionExpanded.append(EIDMW_CNF_MACRO_COMMON_SUBDIR);
       
        //add part after the $-macro
        czSectionExpanded.append(czSectionOriginal.substr(iStrLenght, iTotLenght-iStrLenght));//add part after the $-macro
        return(czSectionExpanded);
    }

    //--- check for EIDMW_CNF_MACRO_COMMON
    iStrLenght = wcslen(EIDMW_CNF_MACRO_COMMON);
    iResult = czSectionOriginal.compare(0, iStrLenght, EIDMW_CNF_MACRO_COMMON);
    if (iResult == 0)
    {
        //replace EIDMW_CNF_MACRO_COMMON

        //hResult = SHGetFolderPath(NULL, CSIDL_COMMON_APPDATA, NULL, SHGFP_TYPE_DEFAULT, wsSectionCustom);
        //if(hResult != S_OK)
        //{
        //    //can not replace, return original string
        //    return(czSectionOriginal);
        //}
        //std::wstring czSectionExpanded(wsSectionCustom);   

//////////////////////////////////////////////////////
//Problem of access right for the user with limited right
        hResult = SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_DEFAULT, wsSectionCustom);

        //non-user SW(eg.: services) returns: C:\WINDOWS\system32\config\systemprofile\Application Data, replace by common dir
        if((hResult != S_OK) || ((hResult == S_OK) && (wcsstr(wsSectionCustom, L":\\WINDOWS") != NULL)))
        {
            //try common path when problems or when no user found
            hResult = SHGetFolderPath(NULL, CSIDL_COMMON_APPDATA, NULL, SHGFP_TYPE_DEFAULT, wsSectionCustom);
            if(hResult != S_OK)
            {
                //can not replace, return original string
                return(czSectionOriginal);
            }
        }
        std::wstring czSectionExpanded(wsSectionCustom);    
//////////////////////////////////////////////////////

		czSectionExpanded.append(WDIRSEP);
		czSectionExpanded.append(EIDMW_CNF_MACRO_COMMON_SUBDIR);

        //add part after the $-macro
        czSectionExpanded.append(czSectionOriginal.substr(iStrLenght, iTotLenght-iStrLenght));//add part after the $-macro
        return(czSectionExpanded);
    }

    return(czSectionOriginal);
}

/** Retrieve from the configuration a parameter of the type STRING in the user settings

\return std::wstring            requested parameter
\retval EIMDW_NOT_OK            exception; when section   is not found
\retval EIDMW_ERR_PARAM_BAD     exception; when parameter is not found

\sa SetString(), DelString,   SetLong(), GetLong(), DelLong()  
*************************************************************************************/
std::wstring GetStringInt(
	CConfig::tLocation location, 
	const std::wstring & csName,                /**< In:     parameter name */
	const std::wstring & czSection,             /**< In:     the configuration-section where you can find above specified parameter */
    bool                 bExpand                /**< In:     expand $home macro's to the value for this user/PC */
    ) 
{
    wchar_t     wcsKeyRegName[MAX_PATH];
    BYTE        abValueDat[512];

    DWORD       dwValDatLen = 0;
    DWORD       dwType;
    LONG        lRes;
    HKEY        hRegKey;
    HKEY		hRegKeyRoot;

    if(location == CConfig::SYSTEM)
        hRegKeyRoot = HKEY_LOCAL_MACHINE;
    else
        hRegKeyRoot = HKEY_CURRENT_USER ;

    //--- Open the KeyInfo entry
    swprintf_s(wcsKeyRegName, sizeof(wcsKeyRegName)/sizeof(wchar_t), L"%s\\%s", SC_CONF_REG, czSection.c_str());

    lRes = RegOpenKeyEx(hRegKeyRoot, wcsKeyRegName, 0L, KEY_READ , &hRegKey);
    if (lRes != ERROR_SUCCESS){
        throw CMWEXCEPTION(EIDMW_CONF);
    }

    //--- get the value
    dwValDatLen = sizeof(abValueDat); 
    dwType      = REG_SZ;
    lRes        = RegQueryValueEx(hRegKey, csName.c_str(), 0L, &dwType, abValueDat, &dwValDatLen);
    if ((lRes != ERROR_SUCCESS)){     
        //try current user if no value found and not yet in current user
        RegCloseKey(hRegKey);
        throw CMWEXCEPTION(EIDMW_ERR_PARAM_BAD);
    }
 
    if (dwValDatLen > 2)            //remove ending "00"
        dwValDatLen -= 2;

    RegCloseKey(hRegKey);

	//Correctly handle empty strings, RegQueryValueEx() returns empty strings as having len=2 for 2 terminating NULL-bytes
	if (abValueDat[0] == 0)
		dwValDatLen = 0;
    const std::wstring wsResult((const wchar_t*)abValueDat, (size_t) (dwValDatLen/2));  //convert byte to double byte
    if(bExpand)
        return( ExpandSection(wsResult) );
    else
        return( wsResult );
}


/** Retrieve from the configuration a parameter of the type STRING

\return std::wstring            requested parameter
\retval EIMDW_NOT_OK            exception; when section   is not found
\retval EIDMW_ERR_PARAM_BAD     exception; when parameter is not found

\sa SetString(), DelString,   SetLong(), GetLong(), DelLong()  
*************************************************************************************/
std::wstring GetStringInt(
    const std::wstring & csName,                /**< In:     parameter name */
	const std::wstring & czSection,             /**< In:     the configuration-section where you can find above specified parameter */
    bool                 bExpand                /**< In:     expand $home macro's to the value for this user/PC */
    ) 
{
	try
	{
		return GetStringInt(CConfig::USER,csName,czSection,bExpand);
	}
	catch(...)
	{
		return GetStringInt(CConfig::SYSTEM,csName,czSection,bExpand);
	}
}

std::wstring CConfig::GetString(
	tLocation location, 
    const struct Param_Str param               /**< In:     parameter struct */
    ) 
{
	return(GetString(location,param.csParam,param.csSection,param.csDefault));
}

std::wstring CConfig::GetString(
	tLocation location, 
    const std::wstring & csName,                /**< In:     parameter name */
	const std::wstring & czSection             /**< In:     the configuration-section where you can find above specified parameter */
    ) 
{
        return(GetStringInt(location,csName, czSection, true));
}



/** Retrieve from the configuration a parameter of the type STRING, use the default-value if nothing is found

\return std::wstring        requested parameter

\sa SetString(), DelString,   SetLong(), GetLong(), DelLong()  
*************************************************************************************/
std::wstring CConfig::GetString(
	tLocation location, 
    const std::wstring &csName,                 /**< In:     parameter name */
	const std::wstring &czSection,              /**< In:     the configuration-section where you can find above specified parameter */
    const std::wstring &csDefaultValue,         /**< In:     default value when the parameter or section is not found */
    bool               bExpand                  /**< In:     expand $home macro's to the value for this user/PC */
    )
{
    try{
        return(GetStringInt(location,csName, czSection, bExpand));
    }
    catch(...){                 //return default value if any error happen
		if(bExpand)
			return( ExpandSection(csDefaultValue) );
		else
			return( csDefaultValue );
        //return(csDefaultValue);
    }
}

std::wstring CConfig::GetString(
    const struct Param_Str param               /**< In:     parameter struct */
    ) 
{
	return(GetString(param.csParam,param.csSection,param.csDefault));
}

std::wstring CConfig::GetString(
    const std::wstring & csName,                /**< In:     parameter name */
	const std::wstring & czSection             /**< In:     the configuration-section where you can find above specified parameter */
    ) 
{
        return(GetStringInt(csName, czSection, true));
}



/** Retrieve from the configuration a parameter of the type STRING, use the default-value if nothing is found

\return std::wstring        requested parameter

\sa SetString(), DelString,   SetLong(), GetLong(), DelLong()  
*************************************************************************************/
std::wstring CConfig::GetString(
    const std::wstring &csName,                 /**< In:     parameter name */
	const std::wstring &czSection,              /**< In:     the configuration-section where you can find above specified parameter */
    const std::wstring &csDefaultValue,         /**< In:     default value when the parameter or section is not found */
    bool               bExpand                  /**< In:     expand $home macro's to the value for this user/PC */
    )
{
    try{
        return(GetStringInt(csName, czSection, bExpand));
    }
    catch(...){                 //return default value if any error happen
		if(bExpand)
			return( ExpandSection(csDefaultValue) );
		else
			return( csDefaultValue );
        //return(csDefaultValue);
    }
}



/** Retrieve from the configuration a parameter of the type LONG

\return long                    requested parameter
\retval EIMDW_NOT_OK            exception; when section   is not found
\retval EIDMW_ERR_PARAM_BAD     exception; when parameter is not found

\sa SetString(), GetString(),DelString,   SetLong(), DelLong()  
*************************************************************************************/
long CConfig::GetLong(
    const struct Param_Num param               /**< In:     parameter struct */
    ) 
{
	return(GetLong(param.csParam,param.csSection,param.lDefault));
}

long CConfig::GetLong(
    const std::wstring & csName,                /**< In:     parameter name */
    const std::wstring &czSection               /**< In:     the configuration-section where you can find above specified parameter */
    )
{
	try
	{
		return GetLong(CConfig::USER,csName,czSection);
	}
	catch(...)
	{
		return GetLong(CConfig::SYSTEM,csName,czSection);
	}
}


/** Retrieve from the configuration a parameter of the type LONG, use the default-value if nothing is found

\return long:   requested parameter

\sa SetString(), GetString(),DelString,   SetLong(), DelLong()  
*************************************************************************************/
long CConfig::GetLong(const std::wstring & csName, const std::wstring &czSection, long lDefaultValue)
{
    try{
        return(GetLong(csName, czSection));
    }
    catch(...){
        return(lDefaultValue);
    }
}

/** Retrieve from the configuration a parameter of the type LONG

\return long                    requested parameter
\retval EIMDW_NOT_OK            exception; when section   is not found
\retval EIDMW_ERR_PARAM_BAD     exception; when parameter is not found

\sa SetString(), GetString(),DelString,   SetLong(), DelLong()  
*************************************************************************************/
long CConfig::GetLong(
	tLocation location, 
	const struct Param_Num param               /**< In:     parameter struct */
    ) 
{
	return(GetLong(location, param.csParam,param.csSection,param.lDefault));
}

long CConfig::GetLong(
	tLocation location, 
	const std::wstring & csName,                /**< In:     parameter name */
    const std::wstring &czSection               /**< In:     the configuration-section where you can find above specified parameter */
    )
{

    wchar_t     wcsKeyRegName[MAX_PATH];

    DWORD       dwValDatLen;
    DWORD       dwType;
    LONG        lRes;
    HKEY        hRegKey;
    long        lResult = 0;
    HKEY		hRegKeyRoot;

    if(location == CConfig::SYSTEM)
        hRegKeyRoot = HKEY_LOCAL_MACHINE;
    else
        hRegKeyRoot = HKEY_CURRENT_USER ;

    //--- Open the KeyInfo entry
    swprintf_s(wcsKeyRegName, sizeof(wcsKeyRegName)/sizeof(wchar_t), L"%s\\%s", SC_CONF_REG, czSection.c_str());

    lRes = RegOpenKeyEx(hRegKeyRoot, wcsKeyRegName, 0L, KEY_READ , &hRegKey);
    if (lRes != ERROR_SUCCESS){
        throw CMWEXCEPTION(EIDMW_CONF);
    }

    //--- get the value
    dwValDatLen = sizeof(lResult); 
    dwType      = REG_DWORD;
    lRes        = RegQueryValueEx(hRegKey, csName.c_str(), 0L, &dwType, (LPBYTE)&lResult, &dwValDatLen);
    if ((lRes != ERROR_SUCCESS)){     
        //try current user if no value found and not yet in current user
        RegCloseKey(hRegKey);
        throw CMWEXCEPTION(EIDMW_ERR_PARAM_BAD);
    }
 
    RegCloseKey(hRegKey);
    return( lResult );
}


/** Retrieve from the configuration a parameter of the type LONG, use the default-value if nothing is found

\return long:   requested parameter

\sa SetString(), GetString(),DelString,   SetLong(), DelLong()  
*************************************************************************************/
long CConfig::GetLong(tLocation location, const std::wstring & csName, const std::wstring &czSection, long lDefaultValue)
{
    try{
        return(GetLong(location, csName, czSection));
    }
    catch(...){
        return(lDefaultValue);
    }
}


/** Modify/add into the configuration a parameter of the type STRING

\retval EIMDW_NOT_OK            exception; when section   is not found
\retval EIDMW_ERR_PARAM_BAD     exception; when parameter could not be modified

\sa GetString(), DelString,   SetLong(), GetLong(), DelLong()  
*************************************************************************************/
void CConfig::SetString(
    tLocation location,                     /**< In:    tells to use the SYSTEM or USER configuration = CConfig::SYSTEM or CConfig::USER*/
    const struct Param_Str param,              /**< In:     parameter struct */
    const std::wstring &csValue             /**< In:    Value that the parameter in the configuration should be set to */
   ) 
{
	return(SetString(location,param.csParam,param.csSection,csValue));
}

void CConfig::SetString(
    tLocation location,                     /**< In:    tells to use the SYSTEM or USER configuration = CConfig::SYSTEM or CConfig::USER*/
    const std::wstring &csName,             /**< In:    parameter name */
    const std::wstring &czSection,          /**< In:    the configuration-section where you can find above specified parameter */
    const std::wstring &csValue             /**< In:    Value that the parameter in the configuration should be set to */
    )
{
    wchar_t     wcsKeyRegName[MAX_PATH];

    LONG    lRes;
    HKEY    hRegKeyTree;
    HKEY    hRegKey;

    if(location == SYSTEM)
        hRegKeyTree = HKEY_LOCAL_MACHINE;
    else
        hRegKeyTree = HKEY_CURRENT_USER ;


    swprintf_s(wcsKeyRegName, sizeof(wcsKeyRegName)/sizeof(wchar_t), L"%s\\%s", SC_CONF_REG, czSection.c_str());

    //--- Open the KeyInfo entry
    lRes = RegCreateKeyEx(hRegKeyTree, wcsKeyRegName, 0L, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hRegKey, NULL);
    if (lRes != ERROR_SUCCESS){
        RegCloseKey(hRegKey);
        throw CMWEXCEPTION(EIDMW_CONF);
    }

    //--- Set the value
    lRes = RegSetValueEx(hRegKey, csName.c_str(), NULL, REG_SZ, (const BYTE *)csValue.c_str(), (DWORD)((csValue.length()+1)*sizeof(wchar_t)));  //terminating 0 must also be included !
    if (lRes != ERROR_SUCCESS){
        RegCloseKey(hRegKey);
        throw CMWEXCEPTION(EIDMW_CONF);
    }
    RegCloseKey(hRegKey);
}

/** Modify/add into the configuration a parameter of the type LONG

\retval EIMDW_NOT_OK            exception; when section   is not found
\retval EIDMW_ERR_PARAM_BAD     exception; when parameter could not be modified

\sa GetString(), SetString(), DelString,   GetLong(), DelLong()  
*************************************************************************************/
void CConfig::SetLong(
    tLocation location,                     /**< In:    tells to use the SYSTEM or USER configuration = CConfig::SYSTEM or CConfig::USER*/
    const struct Param_Num param,               /**< In:     parameter struct */
    long lValue                             /**< In:    Value that the parameter in the configuration should be set to */
    ) 
{
	return(SetLong(location,param.csParam,param.csSection,lValue));
}

void CConfig::SetLong(
    tLocation location,                     /**< In:    tells to use the SYSTEM or USER configuration = CConfig::SYSTEM or CConfig::USER*/
    const std::wstring &csName,             /**< In:    parameter name */
    const std::wstring &czSection,          /**< In:    the configuration-section where you can find above specified parameter */
    long lValue                             /**< In:    Value that the parameter in the configuration should be set to */
    )
{
    wchar_t     wcsKeyRegName[MAX_PATH];

    LONG    lRes;
    HKEY    hRegKeyTree;
    HKEY    hRegKey;

    if(location == SYSTEM)
        hRegKeyTree = HKEY_LOCAL_MACHINE;
    else
        hRegKeyTree = HKEY_CURRENT_USER ;


    swprintf_s(wcsKeyRegName, sizeof(wcsKeyRegName)/sizeof(wchar_t), L"%s\\%s", SC_CONF_REG, czSection.c_str());

    //--- Open the KeyInfo entry
    lRes = RegCreateKeyEx(hRegKeyTree, wcsKeyRegName, 0L, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hRegKey, NULL);
    if (lRes != ERROR_SUCCESS){
        RegCloseKey(hRegKey);
        throw CMWEXCEPTION(EIDMW_CONF);
    }

    //--- Set the value
    lRes = RegSetValueEx(hRegKey, csName.c_str(), NULL, REG_DWORD, (const BYTE *)&lValue, (DWORD)sizeof(long));
    if (lRes != ERROR_SUCCESS){
        RegCloseKey(hRegKey);
        throw CMWEXCEPTION(EIDMW_ERR_PARAM_BAD);
    }

    RegCloseKey(hRegKey);
} // namespace eidMW





/** Remove in the configuration a parameter of the type STRING

\retval EIMDW_NOT_OK            exception; when section   is not found
\retval EIDMW_ERR_PARAM_BAD     exception; when parameter could not be modified

\sa SetString(), GetString(),   SetLong(), GetLong(), DelLong()  
*************************************************************************************/
void CConfig::DelString(
    tLocation location,                     /**< In:    tells to use the SYSTEM or USER configuration = CConfig::SYSTEM or CConfig::USER*/
    const struct Param_Str param               /**< In:     parameter struct */
    ) 
{
	return(DelString(location,param.csParam,param.csSection));
}

void CConfig::DelString(
    tLocation location,                     /**< In:    tells to use the SYSTEM or USER configuration = CConfig::SYSTEM or CConfig::USER*/
    const std::wstring &csName,             /**< In:    parameter name */
    const std::wstring &czSection           /**< In:    the configuration-section where you can find above specified parameter */
    )
{

    wchar_t     wcsKeyRegName[MAX_PATH];

    LONG    lRes;
    HKEY    hRegKeyTree;
    HKEY    hRegKey;

    if(location == SYSTEM)
        hRegKeyTree = HKEY_LOCAL_MACHINE;
    else
        hRegKeyTree = HKEY_CURRENT_USER ;


    //--- Open the KeyInfo entry
    swprintf_s(wcsKeyRegName, sizeof(wcsKeyRegName)/sizeof(wchar_t), L"%s\\%s", SC_CONF_REG, czSection.c_str());
    lRes = RegOpenKeyEx(hRegKeyTree, wcsKeyRegName, 0L, KEY_SET_VALUE, &hRegKey);
    if (lRes != ERROR_SUCCESS){
        RegCloseKey(hRegKey);
        throw CMWEXCEPTION(EIDMW_CONF);
    }

    //--- delete the value
    lRes = RegDeleteValue(hRegKey, csName.c_str());
    if (lRes != ERROR_SUCCESS){
        RegCloseKey(hRegKey);
        throw CMWEXCEPTION(EIDMW_ERR_PARAM_BAD);
    }

    RegCloseKey(hRegKey);
}




/** Remove in the configuration a parameter of the type LONG

\retval EIMDW_NOT_OK            exception; when section   is not found
\retval EIDMW_ERR_PARAM_BAD     exception; when parameter could not be modified

\sa SetString(), GetString(), DelString,   SetLong(), GetLong()  
*************************************************************************************/
void CConfig::DelLong(
    tLocation location,                     /**< In:    tells to use the SYSTEM or USER configuration = CConfig::SYSTEM or CConfig::USER*/
    const struct Param_Num param               /**< In:     parameter struct */
    ) 
{
	return(DelLong(location,param.csParam,param.csSection));
}

void CConfig::DelLong(
    tLocation location,                     /**< In:    tells to use the SYSTEM or USER configuration = CConfig::SYSTEM or CConfig::USER*/
    const std::wstring &csName,             /**< In:    parameter name */
    const std::wstring &czSection           /**< In:    the configuration-section where you can find above specified parameter */
    )
{

    DelString(location, csName, czSection);
}

}