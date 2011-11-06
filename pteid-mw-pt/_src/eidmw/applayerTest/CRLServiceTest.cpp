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
#include "../applayer/CRLService.h"

#include <sys/stat.h>
#include <fstream>

using namespace eIDMW;


TEST(CrlDownload)
{
	APL_CrlDownload crl("http://crl.specimen-eid.portugal.be/government.crl","government.crl",true,180);

	crl.Start();

	crl.WaitTillStopped();

	FILE *f;
#ifdef WIN32
    fopen_s(&f,"government.crl", "rb");
#else
	f = fopen("government.crl", "rb");
#endif

    if(NULL != f)
    {
        size_t size = 0;
#ifdef WIN32
        struct _stat buf = {0};
        if(0 == _fstat(_fileno(f), &buf))
#else
		struct stat buf = {0};
        if(0 == fstat(fileno(f), &buf))
#endif
        fclose(f);

		CHECK_EQUAL(375,buf.st_size);
	}
	else
	{
		CHECK(0);
	}
}
