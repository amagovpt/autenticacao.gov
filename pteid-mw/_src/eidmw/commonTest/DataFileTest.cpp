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
#include "UnitTest++/src/UnitTest++.h"
#include "../common/datafile.h"
#include "../common/Log.h"
#include "../common/eidErrors.h"
#include "../common/MWException.h"
#include <iostream>

#ifndef WIN32
// taken from ThirdParty/MSPlatformSDK/Include/crt/xstddef 
#define _TRY_BEGIN     try {
#define _CATCH_ALL     } catch (...) {
#define _CATCH_END     }
#endif

using namespace eIDMW;

TEST(save)
{
  MWLOG(LEV_WARN, MOD_TEST, L"DataFileTest, entry ...\n");
  bool res = true;
  {
    _TRY_BEGIN
      {
	CDataFile aDataFile(L"config.txt"); 
	res &= aDataFile.SetValue(L"ConfigurationFileName", L"config.txt", L"Name of the configuration file", L"text section");
	res &= aDataFile.SetInt(L"VersionNo", 1, L"Version number of this file", L"number section");
	res &= aDataFile.SetInt(L"SubVersionNo", 0, L"Sub-version number of this file", L"number section");
	res &= aDataFile.SetSectionComment(L"number section", L"some numbers in this section");
	res &= aDataFile.Save();
	if (!res) throw CMWEXCEPTION(EIDMW_ERR_PARAM_BAD);
      }
    {
      CDataFile aDataFile(L"config.txt"); 
      int	iValue;		t_Str sValue;
      iValue = aDataFile.GetInt(L"VersionNo", L"number section");
      res &= (iValue == 1);
      sValue = aDataFile.GetString(L"ConfigurationFileName", L"text section");
      res &= (sValue.compare(L"config.txt") == 0);
      if (!res) throw CMWEXCEPTION(EIDMW_ERR_PARAM_BAD);
    }
    {	// Build a template configuration file for real life usage ...
     CDataFile aDataFile(L"configuration.txt"); 
      res &= aDataFile.SetValue(L"ConfigurationFileName", L"configuration.txt", L"Name of the configuration file", L"system section");
      res &= aDataFile.SetValue(L"ConfigurationFileVersion", L"0.1", L"Configuration file version", L"system section");
      res &= aDataFile.SetValue(L"LogFileName", L"C:\\a_mw_log.log", L"Logging file name", L"Logging settings");
      res &= aDataFile.SetInt(L"LogSeverity", LEV_WARN, L"Logging severity", L"Logging settings");
      res &= aDataFile.SetSectionComment(L"system section", L"... System related settings ...");
      res &= aDataFile.SetSectionComment(L"Logging settings", L"... Next section contains all logging related settings ...");
      res &= aDataFile.Save();
      if (!res) throw CMWEXCEPTION(EIDMW_ERR_PARAM_BAD);
    }
    _CATCH_ALL
      MWLOG(LEV_ERROR, MOD_TEST, L"Exception catched while testing datafile unit\n");
    CHECK_EQUAL(0, 1);
    _CATCH_END
      }
}

