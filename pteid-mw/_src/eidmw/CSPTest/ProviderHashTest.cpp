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
#include "../CSP/ProviderHash.h"

using namespace eIDMW;

TEST(CProviderHash)
{
	CProviderHash oProvHash1(CALG_MD5, ALGO_MD5);
	CHECK_EQUAL(CALG_MD5, oProvHash1.GetAlgid());
	CHECK_EQUAL(16, oProvHash1.GetLengthBytes());

	CProviderHash oProvHash2(CALG_SHA1, ALGO_SHA1);
	CHECK_EQUAL(CALG_SHA1, oProvHash2.GetAlgid());
	CHECK_EQUAL(20, oProvHash2.GetLengthBytes());

	CProviderHash oProvHash3(CALG_SSL3_SHAMD5, ALGO_MD5_SHA1);
	CHECK_EQUAL(CALG_SSL3_SHAMD5, oProvHash3.GetAlgid());
	CHECK_EQUAL(36, oProvHash3.GetLengthBytes());
}

TEST(Duplicate)
{
	CProviderHash oProvHash1(CALG_SSL3_SHAMD5, ALGO_MD5_SHA1);

	CProviderHash oProvHash2(oProvHash1);
	CHECK_EQUAL(CALG_SSL3_SHAMD5, oProvHash2.GetAlgid());

	CByteArray data("abc");
	oProvHash1.Update(data);
	oProvHash2.Update(data);

	CProviderHash oProvHash3(oProvHash1);

	CByteArray h1 = oProvHash1.GetHashValue();
	CByteArray h2 = oProvHash2.GetHashValue();
	CByteArray h3 = oProvHash3.GetHashValue();

	CHECK_EQUAL(h1.ToString(), h2.ToString());
	CHECK_EQUAL(h1.ToString(), h3.ToString());

	CByteArray h("1234567890");
	oProvHash1.SetHashValue(h);

	CProviderHash oProvHash4(oProvHash1);
	h1 = oProvHash1.GetHashValue();
	CByteArray h4 = oProvHash4.GetHashValue();
	CHECK_EQUAL(h1.ToString(), h4.ToString());
}
