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
#include "../common/Log.h"

using namespace eIDMW;

TEST(LogSetLevel)
{
	MWLOG(LEV_WARN, MOD_TEST, L"LogTest, LogSetLevel, entry ...\n");
	//LogSetLevel(LEV_WARN);
	//CHECK_EQUAL(LogGetLevel(), LEV_WARN);
}

TEST(MWLOG)
{
	bool success = MWLOG(LEV_WARN, MOD_TEST, L"LogTest, entry ...\n");
}

