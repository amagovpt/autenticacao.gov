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
#include "../common/DynamicLib.h"

using namespace eIDMW;

#ifdef WIN32
	static char *csPcscPath = "winscard.dll";
#else
	static char *csPcscPath = "libpcsclite.so";
#endif

TEST(LoadPCSC)
{
	CDynamicLib oDynLib;

	unsigned long ulErr = oDynLib.Open(csPcscPath);
	CHECK_EQUAL(EIDMW_OK, ulErr);

	if (ulErr == EIDMW_OK)
	{
		void *addr1 = oDynLib.GetAddress("SCardEstablishContext");
		CHECK(addr1 != NULL);

		void *addr2 = oDynLib.GetAddress("xx");
		CHECK_EQUAL((void *) NULL,  addr2);

		void *addr3 = oDynLib.GetAddress("SCardReleaseContext");
		CHECK(addr3 != NULL);
	}
}

TEST(LoadNonexisting)
{
	CDynamicLib oDynLib;

	unsigned long ulErr = oDynLib.Open("xxxxx");
	CHECK_EQUAL(EIDMW_CANT_LOAD_LIB, ulErr);

	ulErr = oDynLib.Open("yyy");
	CHECK_EQUAL(EIDMW_CANT_LOAD_LIB, ulErr);

	ulErr = oDynLib.Open(csPcscPath);
	CHECK_EQUAL(EIDMW_OK, ulErr);
}
