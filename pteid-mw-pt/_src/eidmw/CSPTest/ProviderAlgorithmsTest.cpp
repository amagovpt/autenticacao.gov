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
#include "../CSP/ProviderAlgorithms.h"

using namespace eIDMW;

TEST(EnumAlgs)
{
	CProviderAlgorithms oProvAlgs;
	PROV_ENUMALGS algs;
	DWORD dwAlgsLen = sizeof(PROV_ENUMALGS);

	// MD5 is first, let's assume we don't change this
	BOOL bOK = oProvAlgs.EnumAlgs((LPBYTE) &algs, &dwAlgsLen, CRYPT_FIRST);
	CHECK_EQUAL(TRUE, bOK);
	CHECK_EQUAL(CALG_MD5, algs.aiAlgid);
	CHECK_EQUAL(128, algs.dwBitLen);

	// If we set the CRYPT_FIRST flag again, we should get MD5 again
	bOK = oProvAlgs.EnumAlgs((LPBYTE) &algs, &dwAlgsLen, CRYPT_FIRST);
	CHECK_EQUAL(TRUE, bOK);
	CHECK_EQUAL(CALG_MD5, algs.aiAlgid);
	CHECK_EQUAL(128, algs.dwBitLen);

	// Next should be SHA-1
	bOK = oProvAlgs.EnumAlgs((LPBYTE) &algs, &dwAlgsLen, 0);
	CHECK_EQUAL(TRUE, bOK);
	CHECK_EQUAL(CALG_SHA1, algs.aiAlgid);
	CHECK_EQUAL(160, algs.dwBitLen);

	// Next should be CALG_SSL3_SHAMD5
	bOK = oProvAlgs.EnumAlgs((LPBYTE) &algs, &dwAlgsLen, 0);
	CHECK_EQUAL(TRUE, bOK);
	CHECK_EQUAL(CALG_SSL3_SHAMD5, algs.aiAlgid);
	CHECK_EQUAL(288, algs.dwBitLen);

	// Next should be CALG_RSA_SIGN
	bOK = oProvAlgs.EnumAlgs((LPBYTE) &algs, &dwAlgsLen, 0);
	CHECK_EQUAL(TRUE, bOK);
	CHECK_EQUAL(CALG_RSA_SIGN, algs.aiAlgid);
	CHECK_EQUAL(1024, algs.dwBitLen);

	// Next should return FALSE and GetLastError() should return ERROR_NO_MORE_ITEMS
	bOK = oProvAlgs.EnumAlgs((LPBYTE) &algs, &dwAlgsLen, 0);
	CHECK_EQUAL(FALSE, bOK);
	CHECK_EQUAL(ERROR_NO_MORE_ITEMS, GetLastError());
}

TEST(EnumAlgsEx)
{
	CProviderAlgorithms oProvAlgs;
	PROV_ENUMALGS_EX algs;
	DWORD dwAlgsLen = sizeof(PROV_ENUMALGS_EX);

	// MD5 is first, let's assume we don't change this
	BOOL bOK = oProvAlgs.EnumAlgsEx((LPBYTE) &algs, &dwAlgsLen, CRYPT_FIRST);
	CHECK_EQUAL(TRUE, bOK);
	CHECK_EQUAL(CALG_MD5, algs.aiAlgid);
	CHECK_EQUAL(128, algs.dwDefaultLen);
	CHECK_EQUAL(128, algs.dwMaxLen);
	CHECK_EQUAL(128, algs.dwMinLen);

	// If we set the CRYPT_FIRST flag again, we should get MD5 again
	bOK = oProvAlgs.EnumAlgsEx((LPBYTE) &algs, &dwAlgsLen, CRYPT_FIRST);
	CHECK_EQUAL(TRUE, bOK);
	CHECK_EQUAL(CALG_MD5, algs.aiAlgid);
	CHECK_EQUAL(128, algs.dwDefaultLen);
	CHECK_EQUAL(128, algs.dwMaxLen);
	CHECK_EQUAL(128, algs.dwMinLen);

	// Next should be SHA-1
	bOK = oProvAlgs.EnumAlgsEx((LPBYTE) &algs, &dwAlgsLen, 0);
	CHECK_EQUAL(TRUE, bOK);
	CHECK_EQUAL(CALG_SHA1, algs.aiAlgid);
	CHECK_EQUAL(160, algs.dwDefaultLen);
	CHECK_EQUAL(160, algs.dwMaxLen);
	CHECK_EQUAL(160, algs.dwMinLen);

	// Next should be CALG_SSL3_SHAMD5
	bOK = oProvAlgs.EnumAlgsEx((LPBYTE) &algs, &dwAlgsLen, 0);
	CHECK_EQUAL(TRUE, bOK);
	CHECK_EQUAL(CALG_SSL3_SHAMD5, algs.aiAlgid);
	CHECK_EQUAL(288, algs.dwDefaultLen);
	CHECK_EQUAL(288, algs.dwMaxLen);
	CHECK_EQUAL(288, algs.dwMinLen);

	// Next should be CALG_RSA_SIGN
	bOK = oProvAlgs.EnumAlgsEx((LPBYTE) &algs, &dwAlgsLen, 0);
	CHECK_EQUAL(TRUE, bOK);
	CHECK_EQUAL(CALG_RSA_SIGN, algs.aiAlgid);
	CHECK_EQUAL(1024, algs.dwDefaultLen);
	CHECK_EQUAL(4096, algs.dwMaxLen);
	CHECK_EQUAL(1024, algs.dwMinLen);

	// Next should return FALSE and GetLastError() should return ERROR_NO_MORE_ITEMS
	bOK = oProvAlgs.EnumAlgsEx((LPBYTE) &algs, &dwAlgsLen, 0);
	CHECK_EQUAL(FALSE, bOK);
	CHECK_EQUAL(ERROR_NO_MORE_ITEMS, GetLastError());
}

// If pbData == NULL -> the length needed for pbData should be returned
TEST(GetLength)
{
	CProviderAlgorithms oProvAlgs;
	DWORD dwAlgsLen;

	BOOL bOK = oProvAlgs.EnumAlgs((LPBYTE) NULL, &dwAlgsLen, CRYPT_FIRST);
	CHECK_EQUAL(TRUE, bOK);
	CHECK_EQUAL(sizeof(PROV_ENUMALGS), dwAlgsLen);

	bOK = oProvAlgs.EnumAlgsEx((LPBYTE) NULL, &dwAlgsLen, CRYPT_FIRST);
	CHECK_EQUAL(TRUE, bOK);
	CHECK_EQUAL(sizeof(PROV_ENUMALGS_EX), dwAlgsLen);
}

TEST(WrongLength)
{
	CProviderAlgorithms oProvAlgs;
	PROV_ENUMALGS algs;
	DWORD dwAlgsLen;

	// Length too small -> ERROR_MORE_DATA must be returned
	dwAlgsLen = sizeof(PROV_ENUMALGS) - 1;
	BOOL bOK = oProvAlgs.EnumAlgs((LPBYTE) &algs, &dwAlgsLen, CRYPT_FIRST);
	CHECK_EQUAL(FALSE, bOK);
	CHECK_EQUAL(ERROR_MORE_DATA, GetLastError());

	// Length too large -> The correct length should be returned
	dwAlgsLen = sizeof(PROV_ENUMALGS) + 1;
	bOK = oProvAlgs.EnumAlgs((LPBYTE) &algs, &dwAlgsLen, CRYPT_FIRST);
	CHECK_EQUAL(TRUE, bOK);
	CHECK_EQUAL(sizeof(PROV_ENUMALGS), dwAlgsLen);
}
