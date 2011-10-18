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
#include "../CSP/ProviderContext.h"

using namespace eIDMW;

TEST(CProviderContext)
{
	VTableProvStruc vTable;
	const char *csContainer = "Authentication(534C494E336600296CFF2491AA090425)";
	CProviderContext oProvCtx(1, csContainer, 0, &vTable);

	std::string csCont = oProvCtx.GetContainerName();
	CHECK_EQUAL(std::string(csContainer), csCont);
}

TEST(GetSerialNr)
{
	VTableProvStruc vTable;
	const char *csContainer = "Authentication(534C494E336600296CFF2491AA090425)";
	CProviderContext oProvCtx(1, csContainer, 0, &vTable);

	std::string csSerialNr = oProvCtx.GetSerialNr();
	CHECK_EQUAL(std::string("534C494E336600296CFF2491AA090425"), csSerialNr);
}

TEST(Hashes)
{
	VTableProvStruc vTable;
	const char *csContainer = "Authentication(534C494E336600296CFF2491AA090425)";
	CProviderContext oProvCtx(1, csContainer, 0, &vTable);

	CByteArray data("abc");
	unsigned char tucSHA1[] = {
		0xa9,0x99,0x3e,0x36,0x47,0x06,0x81,0x6a,0xba,0x3e,0x25,0x71,0x78,0x50,0xc2,0x6c,0x9c,0xd0,0xd8,0x9d};
	CByteArray sha1Res(tucSHA1, sizeof(tucSHA1));

	// Test AddHash
	HCRYPTHASH hash1 = oProvCtx.AddHash(CALG_SHA1);

	// Test GetHash()
	CProviderHash *pProvHash = oProvCtx.GetHash(hash1);
	CHECK_EQUAL(CALG_SHA1, pProvHash->GetAlgid());
	CProviderHash *pNoHash = oProvCtx.GetHash(12345, false);
	CHECK_EQUAL(true, NULL == pNoHash);
	CHECK_THROW(oProvCtx.GetHash(12345), CMWException);

	pProvHash->Update(data);

	// Test GetHashValue() without setting a hash value
	CByteArray oHashData = pProvHash->GetHashValue();
	CHECK_EQUAL(sha1Res.ToString(), oHashData.ToString());

	// Test GetHashValue() after setting a hash value
	CByteArray oNewHash("1234");
	pProvHash->SetHashValue(oNewHash);
	CByteArray oNewHashData = pProvHash->GetHashValue();
	CHECK_EQUAL(oNewHash.ToString(), oNewHashData.ToString());

	// Test DuplicateHash() with a hash value set
	HCRYPTHASH hash2 = oProvCtx.DuplicateHash(hash1);
	CProviderHash *pProvHash2 = oProvCtx.GetHash(hash2);
	CByteArray oNewHashData2 = pProvHash2->GetHashValue();
	CHECK_EQUAL(oNewHash.ToString(), oNewHashData2.ToString());

	// Test DuplicateHash() without setting a hash value
	HCRYPTHASH hash3 = oProvCtx.AddHash(CALG_SHA1);
	pProvHash = oProvCtx.GetHash(hash3);
	pProvHash->Update(data);
	HCRYPTHASH hash4 = oProvCtx.DuplicateHash(hash3);
	CProviderHash *pProvHash4 = oProvCtx.GetHash(hash4);
	oHashData = pProvHash4->GetHashValue();
	CHECK_EQUAL(sha1Res.ToString(), oHashData.ToString());

	// Test DeleteHash()
	BOOL bDeleted = oProvCtx.DeleteHash(hash2);
	CHECK_EQUAL(TRUE, bDeleted);
	bDeleted = oProvCtx.DeleteHash(hash2);
	CHECK_EQUAL(FALSE, bDeleted);
	bDeleted = oProvCtx.DeleteHash(12345);
	CHECK_EQUAL(FALSE, bDeleted);
	bDeleted = oProvCtx.DeleteHash(hash4);
	CHECK_EQUAL(TRUE, bDeleted);
	bDeleted = oProvCtx.DeleteHash(hash1);
	CHECK_EQUAL(TRUE, bDeleted);
	bDeleted = oProvCtx.DeleteHash(hash3);
	CHECK_EQUAL(TRUE, bDeleted);
}

/*
TEST(BaseProvider)
{
	VTableProvStruc vTable;
	CProviderContext oProvCtx1(1, "", 0, &vTable);
	CProviderContext oProvCtx2(2, "", 0, &vTable);

	CBaseProvider oBaseProv1 = oProvCtx1.GetBaseProvider();
	HCRYPTPROV hBaseProv1 = oBaseProv1.GetBaseProviderHandle();

	CBaseProvider oBaseProv2 = oProvCtx2.GetBaseProvider();
	HCRYPTPROV hBaseProv2 = oBaseProv2.GetBaseProviderHandle();

	CHECK_EQUAL(true, hBaseProv1 != hBaseProv2);
}
*/
