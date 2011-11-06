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
#include "../CSP/ProviderContextPool.h"

using namespace eIDMW;

TEST(CProviderContextPool)
{
	CProviderContextPool oProvCtxPool;
	VTableProvStruc VTable;

	HCRYPTPROV hProv1 = oProvCtxPool.AddProviderContext("1", 0, &VTable);
	CHECK_EQUAL((HCRYPTPROV) 1, hProv1);

	HCRYPTPROV hProv2 = oProvCtxPool.AddProviderContext("2", 0, &VTable);
	CHECK_EQUAL((HCRYPTPROV) 2, hProv2);

	HCRYPTPROV hProv3 = oProvCtxPool.AddProviderContext("3", 0, &VTable);
	CHECK_EQUAL((HCRYPTPROV) 3, hProv3);

	BOOL bDeleted = oProvCtxPool.DeleteProviderContext(hProv2);
	CHECK_EQUAL(TRUE, bDeleted);

	// Already deleted
	bDeleted = oProvCtxPool.DeleteProviderContext(hProv2);
	CHECK_EQUAL(FALSE, bDeleted);

	// Non-existing HCRYPTPROV handle
	oProvCtxPool.DeleteProviderContext((HCRYPTPROV) 4);
	CHECK_EQUAL(FALSE, bDeleted);

	CProviderContext *poProvCtx = oProvCtxPool.GetProviderContext(hProv1);
	CHECK_EQUAL(false, poProvCtx == NULL);

	poProvCtx = oProvCtxPool.GetProviderContext(hProv2, false);
	CHECK_EQUAL(true, poProvCtx == NULL);
	CHECK_THROW(oProvCtxPool.GetProviderContext(hProv2), CMWException);

	poProvCtx = oProvCtxPool.GetProviderContext(hProv3);
	CHECK_EQUAL(false, poProvCtx == NULL);

	poProvCtx = oProvCtxPool.GetProviderContext((HCRYPTPROV) 4, false);
	CHECK_EQUAL(true, poProvCtx == NULL);
	CHECK_THROW(oProvCtxPool.GetProviderContext((HCRYPTPROV) 4), CMWException);

	HCRYPTPROV hProv4 = oProvCtxPool.AddProviderContext("4", 0, &VTable);
	CHECK_EQUAL((HCRYPTPROV) 4, hProv4);
}
