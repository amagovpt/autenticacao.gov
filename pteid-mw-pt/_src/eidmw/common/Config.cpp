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
#include <iostream>
#include <stdlib.h>
#include <limits.h>
#include <sys/stat.h>

#include "Config.h"
#include "Util.h"
#include "datafile.h"
#include "eidErrors.h"
#include "MWException.h"
#include "prefix.h"

// open the user config file and also the general config file
// search first in the user config file and then in the general one

// setters: set always in the user config file unless the admin is executing the programme
// ---> now the choice of the file is made via the location::USER and location::SYSTEM,
//      still one should check the permissions for the requested location
namespace eIDMW
{ 

  bool bIsInitialized = false;

  CDataFile CConfig::o_userDataFile;
  CDataFile CConfig::o_systemDataFile;
  CMutex CConfig::m_Mutex;

  std::wstring home_path;

  CConfig::CConfig(void)  {
  }

  CConfig::~CConfig(){
  }

  void CConfig::Init(){

    if (! bIsInitialized){
      const std::wstring system_datafilePath = utilStringWiden(STRINGIFY(EIDMW_PREFIX)) + L"/etc/";
      home_path = utilStringWiden(std::string(getenv("HOME")));
#ifdef __APPLE__
      const std::wstring user_datafilePath = L"Library/Preferences/";
#else
      const std::wstring user_datafilePath = L".config/";
      mkdir(utilStringNarrow(home_path + L"/" + user_datafilePath).c_str(), 0700);  //Create the directory if it doesn't exist
#endif

      const std::wstring userFile = home_path + L"/" + user_datafilePath + L"pteid.conf";
      const std::wstring systemFile = system_datafilePath + L"pteid.conf";

      o_userDataFile.SetFileName(userFile) ;
 
      o_systemDataFile.SetFileName(systemFile);
    
      bIsInitialized = true;
    }
  }

/**
 * Macro expansion:
 *  - $install -> $PREFIX (default: /usr/local)
 *  - $home    -> user home dir
 *  - $common  -> /tmp
 */
std::wstring ExpandSection(
	const std::wstring & czSectionOriginal
    ) 
{
    int                              iResult;
    basic_string <char>::size_type   iTotLenght = czSectionOriginal.length();
    basic_string <char>::size_type   iStrLenght;

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
        std::wstring czSectionExpanded(utilStringWiden(STRINGIFY(EIDMW_PREFIX)));

        //add part after the $-macro
        czSectionExpanded.append(czSectionOriginal.substr(iStrLenght, iTotLenght-iStrLenght));//add part after the $-macro
        return(czSectionExpanded);
    }

    //--- check for EIDMW_CNF_MACRO_HOME
    iStrLenght = wcslen(EIDMW_CNF_MACRO_HOME);
    iResult = czSectionOriginal.compare(0, iStrLenght, EIDMW_CNF_MACRO_HOME);
    if (iResult == 0)
    {
        std::wstring czSectionExpanded(home_path);

        //add part after the $-macro
        czSectionExpanded.append(czSectionOriginal.substr(iStrLenght, iTotLenght-iStrLenght));//add part after the $-macro
        return(czSectionExpanded);
    }

    //--- check for EIDMW_CNF_MACRO_COMMON
    iStrLenght = wcslen(EIDMW_CNF_MACRO_COMMON);
    iResult = czSectionOriginal.compare(0, iStrLenght, EIDMW_CNF_MACRO_COMMON);
    if (iResult == 0)
    {
        std::wstring czSectionExpanded(L"/tmp");

        //add part after the $-macro
        czSectionExpanded.append(czSectionOriginal.substr(iStrLenght, iTotLenght-iStrLenght));//add part after the $-macro
        return(czSectionExpanded);
    }

    return(czSectionOriginal);
}

  std::wstring CConfig::GetStringInt(tLocation location, const std::wstring &csName, const std::wstring &csSection,bool bExpand)
  {
    CAutoMutex autoMutex(&m_Mutex);

    if (! bIsInitialized) Init();

    std::wstring csResult= L"";

     if(location == CConfig::SYSTEM)
        csResult = o_systemDataFile.GetString(csName,csSection);
    else
        csResult = o_userDataFile.GetString(csName,csSection);

    if( csResult  != L"" )
         return bExpand ? ExpandSection(csResult) : csResult;

   throw CMWEXCEPTION(EIDMW_CONF);
  }

 std::wstring CConfig::GetStringInt(const std::wstring &csName, const std::wstring &csSection,bool bExpand)
  {
	try
	{
		return GetStringInt(CConfig::USER,csName,csSection,bExpand);
	}
	catch(...)
	{
		return GetStringInt(CConfig::SYSTEM,csName,csSection,bExpand);
	}
  }

  // std::wstring CConfig::GetString(t_Str szKey, t_Str szSection)
  std::wstring CConfig::GetString(const Param_Str param) 
  {
	return(GetString(param.csParam, param.csSection, param.csDefault, true));
  }


  std::wstring CConfig::GetString(const std::wstring & csName,
				  const std::wstring & szSection) 
  {
    return GetStringInt(csName, szSection, true);
  }

  std::wstring CConfig::GetString(const std::wstring &csName, 
				  const std::wstring &czSection, 
				  const std::wstring &csDefaultValue,
				  bool bExpand)
  {
    try 
    {
      return GetStringInt(csName, czSection, bExpand);
    }
    catch(...)
    {
      return bExpand ? ExpandSection(csDefaultValue) : csDefaultValue;
    }
};

  std::wstring CConfig::GetString(tLocation location, const Param_Str param) 
  {
	return(GetString(location,param.csParam, param.csSection, param.csDefault));
  }


  std::wstring CConfig::GetString(tLocation location, 
				  const std::wstring & csName,
				  const std::wstring & szSection) 
  {
    return GetStringInt(location,csName, szSection, true);
  }

  std::wstring CConfig::GetString(tLocation location, 
				  const std::wstring &csName, 
				  const std::wstring &czSection, 
				  const std::wstring &csDefaultValue,
				  bool bExpand)
  {
    try 
    {
      return GetStringInt(location,csName, czSection,bExpand);
    }
    catch(...)
    {
      return bExpand ? ExpandSection(csDefaultValue) : csDefaultValue;
    }
  };

  long CConfig::GetLong(tLocation location, const Param_Num param) 
  {
	return(GetLong(location,param.csParam, param.csSection, param.lDefault));
  }

  long CConfig::GetLong(  tLocation location, const std::wstring &csName, const std::wstring &czSection) { 
    CAutoMutex autoMutex(&m_Mutex);

    if (! bIsInitialized) Init();

    long lResult = LONG_MIN;

     if(location == CConfig::SYSTEM)
        lResult = o_systemDataFile.GetLong(csName,czSection);
    else
        lResult = o_userDataFile.GetLong(csName,czSection) ;

    if(lResult != LONG_MIN)
      return lResult;

    throw CMWEXCEPTION(EIDMW_CONF);
  };


  long CConfig::GetLong(  tLocation location, const std::wstring &csName, const std::wstring &czSection, long lDefaultValue) { 

     try {
      return  GetLong(location,csName,czSection);
    } catch (...) {
      return lDefaultValue;
    }
  };

 long CConfig::GetLong(const Param_Num param) 
  {
	return(GetLong(param.csParam, param.csSection, param.lDefault));
  }

  long CConfig::GetLong(  const std::wstring &csName, const std::wstring &czSection) { 
 	try
	{
		return GetLong(CConfig::USER,csName,czSection);
	}
	catch(...)
	{
		return GetLong(CConfig::SYSTEM,csName,czSection);
	}
  };


  long CConfig::GetLong(  const std::wstring &csName, const std::wstring &czSection, long lDefaultValue) { 

     try {
      return  GetLong(csName,czSection);
    } catch (...) {
      return lDefaultValue;
    }
  };

void CConfig::SetString(tLocation location,const struct Param_Str param,const std::wstring &csValue)
{
	return(SetString(location,param.csParam,param.csSection,csValue));
}

  void CConfig::SetString(tLocation location, 
			  const std::wstring &csName, 
			  const std::wstring &czSection, 
			  const std::wstring &csValue) {
    CAutoMutex autoMutex(&m_Mutex);

    if (! bIsInitialized) Init();
        
    if (location == SYSTEM)
    {
      o_systemDataFile.SetValue(csName,csValue,L"",czSection);
      if( ! o_systemDataFile.Save() )
        throw CMWEXCEPTION(EIDMW_CONF);
    } else {
      o_userDataFile.SetValue(csName,csValue,L"",czSection);
      if( ! o_userDataFile.Save())
        throw CMWEXCEPTION(EIDMW_CONF);
    }
  };

void CConfig::SetLong(tLocation location,const struct Param_Num param,long lValue)
{
	return(SetLong(location,param.csParam,param.csSection,lValue));
}

  void CConfig::SetLong(  tLocation location, const std::wstring &csName, const std::wstring &czSection, long lValue) {
  CAutoMutex autoMutex(&m_Mutex);
   
  if (! bIsInitialized) Init();

    if (location == SYSTEM){
      o_systemDataFile.SetLong(csName,lValue,L"",czSection);
      if( ! o_systemDataFile.Save() )
         throw CMWEXCEPTION(EIDMW_CONF);
   } else {
      o_userDataFile.SetLong(csName,lValue,L"",czSection);
      if( ! o_userDataFile.Save() )
         throw CMWEXCEPTION(EIDMW_CONF);
   }
  };

void CConfig::DelString(tLocation location, const struct Param_Str param)
{
	return(DelString(location,param.csParam,param.csSection));
}

   void CConfig::DelString(tLocation location, const std::wstring &csName, const std::wstring &czSection) {
    CAutoMutex autoMutex(&m_Mutex);

    if (! bIsInitialized) Init();

    if (location == SYSTEM){
      if( ! o_systemDataFile.DeleteKey(csName,czSection) )
	throw CMWEXCEPTION(EIDMW_CONF);
      if( ! o_systemDataFile.Save() )
         throw CMWEXCEPTION(EIDMW_CONF);
  } else {
     if( !  o_userDataFile.DeleteKey(csName,czSection) )
	throw CMWEXCEPTION(EIDMW_CONF);
     if( ! o_userDataFile.Save() )
        throw CMWEXCEPTION(EIDMW_CONF);
    }
  };

void CConfig::DelLong(tLocation location, const struct Param_Num param) 
{
	return(DelLong(location,param.csParam,param.csSection));
}

void CConfig::DelLong(  tLocation location, const std::wstring &csName, const std::wstring &czSection) 
{
    DelString(location,csName,czSection);
};

} // namespace eidMW
