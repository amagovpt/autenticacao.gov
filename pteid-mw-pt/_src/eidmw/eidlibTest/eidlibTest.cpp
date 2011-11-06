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
#include "../eidlib/eidlib.h"

#include <stdio.h>

int main()
{
    printf("Note: for these test, you should be online, and\n");
    printf("the \"cert_allow_testcard\" config option in the\n");
    printf("\"certificatevalidation\" section should be set to 1\n");

    int result = UnitTest::RunAllTests();
	//while (true) {}
	
	eIDMW::PTEID_ReleaseSDK();

	return result;
}
