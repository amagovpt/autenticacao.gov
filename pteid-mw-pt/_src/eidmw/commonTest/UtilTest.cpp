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
#include "../common/Util.h"
#include "../common/Log.h"

using namespace eIDMW;

/*
TEST(utilStringNarrow)
{
//	MWLOG(LEV_WARN, MOD_TEST, "Util String Widen/Narrow, ...\n"); 

    // narrow & widen
    const std::string  sTest  = "Blabla";
    const std::wstring wsTest = L"WideBlabla";
 

//    CHECK_EQUAL(sTest, utilStringNarrow(utilStringWiden(sTest)));

//    std::wstring  wsResult = utilStringWiden(sTest);
//    std::string  sResult = utilStringNarrow(wsTest);
//    CHECK_EQUAL(sTest.c_str(), sResult.c_str());

//    CHECK_EQUAL(wsTest, utilStringWiden(utilStringNarrow(wsTest)));
}
*/

TEST(StartsWith)
{
	CHECK_EQUAL(true, StartsWith("abcdef", "a"));
	CHECK_EQUAL(true, StartsWith("abcdef", "ab"));
	CHECK_EQUAL(true, StartsWith("abcdef", "abcdef"));

	CHECK_EQUAL(false, StartsWith("abcdef", "A"));
	CHECK_EQUAL(false, StartsWith("abcdef", "b"));
	CHECK_EQUAL(false, StartsWith("abcdef", "ac"));
}

TEST(StartsWithCI)
{
	CHECK_EQUAL(true, StartsWithCI("aBcdEf", "a"));
	CHECK_EQUAL(true, StartsWithCI("abcdef", "ab"));
	CHECK_EQUAL(true, StartsWithCI("abcdef", "abcdef"));

	CHECK_EQUAL(false, StartsWithCI("abcdef", "x"));
	CHECK_EQUAL(false, StartsWithCI("abcdef", "ac"));
	CHECK_EQUAL(false, StartsWithCI("abcdef", "df"));
}

#ifndef WIN32

TEST(strcat_s)
{
	char csDest[5];

	csDest[0] = '\0';
	int r = strcat_s(csDest, sizeof(csDest), "abc");
	CHECK_EQUAL(0, r);
	CHECK_EQUAL(0, strcmp(csDest, "abc"));

	r = strcat_s(csDest, sizeof(csDest), "d");
	CHECK_EQUAL(0, r);
	CHECK_EQUAL(0, strcmp(csDest, "abcd"));

	r = strcat_s(csDest, sizeof(csDest), "");
	CHECK_EQUAL(0, r);
	CHECK_EQUAL(0, strcmp(csDest, "abcd"));

	r = strcat_s(csDest, sizeof(csDest), "e");
	CHECK_EQUAL(-1, r);
	CHECK_EQUAL(0, strcmp(csDest, "abcd"));
}

TEST(strcpy_s)
{
	char csDest[5];

	int r = strcpy_s(csDest, sizeof(csDest), "");
	CHECK_EQUAL(0, r);
	CHECK_EQUAL(0, strcmp(csDest, ""));

	r = strcpy_s(csDest, sizeof(csDest), "abcd");
	CHECK_EQUAL(0, r);
	CHECK_EQUAL(0, strcmp(csDest, "abcd"));

	r = strcpy_s(csDest, sizeof(csDest), "abcde");
	CHECK_EQUAL(-1, r);
}

TEST(strncpy_s)
{
	char csDest[5];

	int r = strncpy_s(csDest, sizeof(csDest), "", 1);
	CHECK_EQUAL(0, r);
	CHECK_EQUAL(0, strcmp(csDest, ""));

	r = strncpy_s(csDest, sizeof(csDest), "abcd", 0);
	CHECK_EQUAL(0, r);
	CHECK_EQUAL(0, strcmp(csDest, ""));

	r = strncpy_s(csDest, sizeof(csDest), "abcd", 10);
	CHECK_EQUAL(0, r);
	CHECK_EQUAL(0, strcmp(csDest, "abcd"));

	r = strncpy_s(csDest, sizeof(csDest), "abcdef", 4);
	CHECK_EQUAL(0, r);
	CHECK_EQUAL(0, strcmp(csDest, "abcd"));

	r = strncpy_s(csDest, sizeof(csDest), "abcde", 5);
	CHECK_EQUAL(-1, r);
	CHECK_EQUAL('\0', csDest[0]);
}

TEST(sprintf_s)
{
	char csDest[5];

	int r = sprintf_s(csDest, sizeof(csDest), "%s%s", "ab", "cd");
	CHECK_EQUAL(0, r);
	CHECK_EQUAL(0, strcmp(csDest, "abcd"));

	r = sprintf_s(csDest, sizeof(csDest), "%s%s", "ab", "cde");
	CHECK_EQUAL(-1, r);
}

#endif
