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
#include "../common/Config.h"
#include "../common/Log.h"
#include "../common/eidErrors.h"

using namespace eIDMW;

/*
TEST(Configuration)
{
	MWLOG(LEV_WARN, MOD_TEST, "ConfigTest, entry ...\n"); 

	CConfig *configuration = CConfig::GetConfiguration();
	CHECK_EQUAL((configuration == NULL), false);
}
*/

TEST(ConfigString)
{
    std::wstring     wsResult;

    //--- cleanup
    try{
        CConfig::DelString(CConfig::USER, L"TestString", L"screen");
    }
    catch(...){ //ignore errors if key does not exist
    }

    try{
        CConfig::DelString(CConfig::SYSTEM, L"TestString", L"screen");
    }
    catch(CMWException &e)
    {
#ifdef WIN32
		if(e.GetError() != EIDMW_CONF && e.GetError() != EIDMW_ERR_PARAM_BAD)
          CHECK (0);
#else
		if(e.GetError() == EIDMW_CONF)
          std::cout << "Must be root to pass full test\n";
		else
          CHECK (0);
#endif
    }
    catch(...){ //ignore errors if key does not exist
    }

    //--- check non-existing key 
    int iError = 0;
    try{
        CConfig::GetString(L"TestString", L"unknown");
    }
    catch(...){
        iError = 1;
    }
    CHECK_EQUAL(iError, 1);


    //--- check default value
    //wsResult = CConfig::GetString(L"TestString", L"unknown", std::wstring(L"default"));
    wsResult = CConfig::GetString(L"TestString", L"unknown", L"default");
    CHECK (L"default" == wsResult);

    //--- set/get from system-section
    try
    {
        CConfig::SetString(CConfig::SYSTEM, L"TestString", L"screen", L"green");
        //CHECK_EQUAL(sParmDat1, CConfig::GetString(L"TestString", L"screen"));
        wsResult = CConfig::GetString(L"TestString", L"screen");
        CHECK (L"green" == wsResult);
    }
    catch(CMWException &e)
    {
        if(e.GetError() == EIDMW_CONF)
          std::cout << "Must be root to pass full test\n";
	else
          CHECK (0);
    }

    //--- set/get from user-section
    CConfig::SetString(CConfig::USER, L"TestString", L"screen", L"yellow");
    wsResult = CConfig::GetString(L"TestString", L"screen");
    CHECK (L"yellow" == wsResult);


    //--- set/get with $home macro
    CConfig::SetString(CConfig::USER, L"TestString", L"screen", L"$home\\green");

    wsResult = CConfig::GetString(L"TestString", L"screen", L"default", false);
    CHECK (L"$home\\green" == wsResult);          //no expand, result should contain "$home\\green"

    wsResult = CConfig::GetString(L"TestString", L"screen", L"default", true);
    CHECK (L"$home\\green" != wsResult);          //"$home\\green" replaced, result should NOT contain "$home\\green" any more
    CHECK (L"default" != wsResult);               //it may not take the default value !"

    wsResult = CConfig::GetString(L"TestString", L"screen");
    CHECK (L"$home\\green" != wsResult);          //"$home\\green" replaced, result should NOT contain "$home\\green" any more
    CHECK (L"default" != wsResult);               //it may not take the default value !"

    //--- set/get with $install macro
    CConfig::SetString(CConfig::USER, L"TestString", L"screen", L"$install\\green");

    wsResult = CConfig::GetString(L"TestString", L"screen", L"default", false);
    CHECK (L"$install\\green" == wsResult);          //no expand, result should contain "$install\\green"

    wsResult = CConfig::GetString(L"TestString", L"screen", L"default", true);
    CHECK (L"$install\\green" != wsResult);          //"$install\\green" replaced, result should NOT contain "$install\\green" any more
  //CHECK (L"default" != wsResult);               //it may not take the default value !"

  //wsResult = CConfig::GetString(L"TestString", L"screen");
  //CHECK (L"$install\\green" != wsResult);          //"$install\\green" replaced, result should NOT contain "$install\\green" any more
  //CHECK (L"default" != wsResult);               //it may not take the default value !"

    //--- set/get with $common macro
    CConfig::SetString(CConfig::USER, L"TestString", L"screen", L"$common\\green");

    wsResult = CConfig::GetString(L"TestString", L"screen", L"default", false);
    CHECK (L"$common\\green" == wsResult);          //no expand, result should contain "$common\\green"

    wsResult = CConfig::GetString(L"TestString", L"screen", L"default", true);
    CHECK (L"$common\\green" != wsResult);          //"$common\\green" replaced, result should NOT contain "$common\\green" any more
    CHECK (L"default" != wsResult);               //it may not take the default value !"

    wsResult = CConfig::GetString(L"TestString", L"screen");
    CHECK (L"$common\\green" != wsResult);          //"$common\\green" replaced, result should NOT contain "$common\\green" any more
    CHECK (L"default" != wsResult);               //it may not take the default value !"

     //--- test delete key
     //should not give an error
     CConfig::DelString(CConfig::USER, L"TestString", L"screen");
     try
    {
      CConfig::DelString(CConfig::SYSTEM, L"TestString", L"screen");
    }
    catch(CMWException &e)
    {
        if(e.GetError() == EIDMW_CONF)
          std::cout << "Must be root to pass full test\n";
	else
          CHECK (0);
    }

     //should give an error, key does not exist
     iError = 0;
     try{
         CConfig::DelString(CConfig::USER, L"TestString", L"screen");
     }
     catch(...){ //ignore errors if key does not exist
         iError++;
     }
 
     try{
         CConfig::DelString(CConfig::SYSTEM, L"TestString", L"screen");
     }
     catch(...){ //ignore errors if key does not exist
         iError++;
     }

    CHECK_EQUAL(iError, 2);





 }


TEST(ConfigLong)
{
   //--- cleanup
   try{
     CConfig::DelLong(CConfig::USER, L"TestLong", L"keyboard");
   }
   catch(...){ //ignore errors if key does not exist
   }

   try{
     CConfig::DelLong(CConfig::SYSTEM, L"TestLong", L"keyboard");
   }
   catch(...){ //ignore errors if key does not exist
   }


   //--- check non-existing key  
   int iError = 0;
   try{
       CConfig::GetLong(L"TestLong", L"unknown");
   }
   catch(...){
       iError = 1;
   }
   CHECK_EQUAL(iError, 1);


   //--- check default value
   CHECK_EQUAL(12345678L, CConfig::GetLong(L"TestLong", L"unknown", 12345678L));

   //--- set/get from system-section
   try{
     CConfig::SetLong(CConfig::SYSTEM, L"TestLong", L"keyboard", 87654321L);
     CHECK_EQUAL(87654321L, CConfig::GetLong(L"TestLong", L"keyboard"));
   }
    catch(CMWException &e)
    {
        if(e.GetError() == EIDMW_CONF)
          std::cout << "Must be root to pass full test\n";
	else
          CHECK (0);
    }

   //--- set/get from user-section
   CConfig::SetLong(CConfig::USER, L"TestLong", L"keyboard", 789789789L);
   CHECK_EQUAL(789789789L, CConfig::GetLong(L"TestLong", L"keyboard"));


   //--- test delete key
   //should not give an error
   CConfig::DelLong(CConfig::USER, L"TestLong", L"keyboard");
   try{
     CConfig::DelLong(CConfig::SYSTEM, L"TestLong", L"keyboard");
   }
    catch(CMWException &e)
    {
        if(e.GetError() == EIDMW_CONF)
          std::cout << "Must be root to pass full test\n";
	else
          CHECK (0);
    }

   //should give an error, key does not exist
   iError = 0;
   try{
       CConfig::DelLong(CConfig::USER, L"TestLong", L"keyboard");
   }
   catch(...){ //ignore errors if key does not exist
       iError++;
   }

   try{
       CConfig::DelLong(CConfig::SYSTEM, L"TestLong", L"keyboard");
   }
   catch(...){ //ignore errors if key does not exist
       iError++;
   }

   CHECK_EQUAL(iError, 2);
}


