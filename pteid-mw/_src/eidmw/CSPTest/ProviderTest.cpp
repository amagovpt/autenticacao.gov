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
/**
 * Some tests assume certain virtual eID cards to
 * be present.
 * If you would replace these virtual cards by others,
 * these test won' work anymore.
 */

#include "UnitTest++/src/UnitTest++.h"
#include <windows.h>
#include "../CSP/eidmwcspdk.h"
#include "../CSP/cspdefines.h"
#include "../common/ByteArray.h"

using namespace eIDMW;

static bool VerifyPIN(const char *csReaderName);

// Test CPAcquireContexg(), eidmwReleaseContext()
TEST(CPAcquireReleaseContext)
{
	VTableProvStruc VTable; // Not used
	VTableProvStrucW VTableW; // Not used
	unsigned char tucData[200];
	DWORD dwDataLen;

	// Test eidmwAcquireContext() different dwFlags

	HCRYPTPROV hProv1 = 0;
	BOOL bRet = eidmwAcquireContext(&hProv1, NULL, CRYPT_VERIFYCONTEXT, &VTable);
	CHECK_EQUAL(TRUE, bRet);
	CHECK_EQUAL(1, hProv1);

	HCRYPTPROV hProv2 = 0;
	const char *csContainer2 = "Authentication(534C494E336600296CFF2491AA090425)";
	bRet = eidmwAcquireContext(&hProv2, csContainer2, CRYPT_SILENT, &VTable);
	CHECK_EQUAL(TRUE, bRet);
	CHECK_EQUAL(2, hProv2);

	HCRYPTPROV hProv3 = 0;
	const char *csContainer3 = "Signature(534C494E336600296CFF2623660B0826)";
	bRet = eidmwAcquireContext(&hProv3, csContainer3, 0, &VTable);
	CHECK_EQUAL(TRUE, bRet);
	CHECK_EQUAL(3, hProv3);

	HCRYPTPROV hProv4 = 0;
	bRet = eidmwAcquireContext(&hProv4, NULL, CRYPT_NEWKEYSET, &VTable);
	CHECK_EQUAL(FALSE, bRet);
	CHECK_EQUAL(NTE_BAD_FLAGS, GetLastError());

	HCRYPTPROV hProv5 = 0;
	bRet = eidmwAcquireContext(&hProv5, NULL, CRYPT_MACHINE_KEYSET, &VTable);
	CHECK_EQUAL(FALSE, bRet);
	CHECK_EQUAL(NTE_BAD_FLAGS, GetLastError());

	HCRYPTPROV hProv6 = 0;
	bRet = eidmwAcquireContext(&hProv6, NULL, CRYPT_DELETEKEYSET, &VTable);
	CHECK_EQUAL(FALSE, bRet);
	CHECK_EQUAL(NTE_BAD_FLAGS, GetLastError());

	// Check if we get the correct context for each handle

	dwDataLen = sizeof(tucData);
	bRet = eidmwGetProvParam(hProv1, PP_CONTAINER, tucData, &dwDataLen, 0);
	CHECK_EQUAL(TRUE, bRet);
	CHECK_EQUAL(1, dwDataLen);

	dwDataLen = sizeof(tucData);
	bRet = eidmwGetProvParam(hProv2, PP_CONTAINER, tucData, &dwDataLen, 0);
	CHECK_EQUAL(TRUE, bRet);
	CHECK_EQUAL(strlen(csContainer2) + 1, dwDataLen);
	CHECK_EQUAL(csContainer2, (char *) tucData);

	dwDataLen = sizeof(tucData);
	bRet = eidmwGetProvParam(hProv3, PP_CONTAINER, tucData, &dwDataLen, 0);
	CHECK_EQUAL(TRUE, bRet);
	CHECK_EQUAL(strlen(csContainer3) + 1, dwDataLen);
	CHECK_EQUAL(csContainer3, (char *) tucData);

	dwDataLen = sizeof(tucData);
	bRet = eidmwGetProvParam(hProv4, PP_CONTAINER, tucData, &dwDataLen, 0);
	CHECK_EQUAL(FALSE, bRet);

	// Test eidmwReleaseContext()

	bRet = eidmwReleaseContext(hProv2, 0);
	CHECK_EQUAL(TRUE, bRet);
	bRet = eidmwReleaseContext(hProv2, 0);
	CHECK_EQUAL(FALSE, bRet);

	dwDataLen = sizeof(tucData);
	bRet = eidmwGetProvParam(hProv1, PP_CONTAINER, tucData, &dwDataLen, 0);
	CHECK_EQUAL(TRUE, bRet);
	CHECK_EQUAL(1, dwDataLen);

	dwDataLen = sizeof(tucData);
	bRet = eidmwGetProvParam(hProv3, PP_CONTAINER, tucData, &dwDataLen, 0);
	CHECK_EQUAL(TRUE, bRet);
	CHECK_EQUAL(strlen(csContainer3) + 1, dwDataLen);
	CHECK_EQUAL(csContainer3, (char *) tucData);

	bRet = eidmwReleaseContext(hProv4, 0);
	CHECK_EQUAL(FALSE, bRet);

	bRet = eidmwAcquireContext(&hProv4, NULL, CRYPT_VERIFYCONTEXT, &VTable);
	CHECK_EQUAL(TRUE, bRet);
	CHECK_EQUAL(4, hProv4);

	hProv4 = 0;
	const wchar_t *csContainer4 = L"Signature(534C494E336600296CFF2623660B0826)";
	bRet = eidmwAcquireContextW(&hProv4, csContainer4, 0, &VTableW);
	CHECK_EQUAL(TRUE, bRet);
	CHECK_EQUAL(5, hProv4);

	bRet = eidmwReleaseContext(hProv1, 0);
	CHECK_EQUAL(TRUE, bRet);
	bRet = eidmwReleaseContext(hProv1, 0);
	CHECK_EQUAL(FALSE, bRet);

	bRet = eidmwReleaseContext(hProv4, 0);
	CHECK_EQUAL(TRUE, bRet);
	bRet = eidmwReleaseContext(hProv4, 0);
	CHECK_EQUAL(FALSE, bRet);
}

// Test eidmwGetProvParam(), eidmwSetProvParam()
TEST(CPGetSetProvParam)
{
	VTableProvStruc VTable; // Not used
	const char *csContainer = "Authentication(534C494E336600296CFF2623000000E1)";

	HCRYPTPROV hProv1 = 0;
	BOOL bRet = eidmwAcquireContext(&hProv1, csContainer, 0, &VTable);
	CHECK_EQUAL(TRUE, bRet);

	unsigned char tucData[1000];
	DWORD dwDataLen;

	// eidmwGetProvParam(PP_CONTAINER), eidmwGetProvParam(PP_UNIQUE_CONTAINER);
	bRet = eidmwGetProvParam(hProv1, PP_CONTAINER, NULL, &dwDataLen, 0);
	CHECK_EQUAL(TRUE, bRet);
	CHECK_EQUAL(strlen(csContainer) + 1, dwDataLen);
	bRet = eidmwGetProvParam(hProv1, PP_UNIQUE_CONTAINER, tucData, &dwDataLen, 0);
	CHECK_EQUAL(TRUE, bRet);
	CHECK_EQUAL(0, strcmp(csContainer, (char *) tucData));

	// eidmwGetProvParam(length too small)
	dwDataLen = 1;
	bRet = eidmwGetProvParam(hProv1, PP_UNIQUE_CONTAINER, tucData, &dwDataLen, 0);
	CHECK_EQUAL(FALSE, bRet);
	CHECK_EQUAL(ERROR_MORE_DATA, GetLastError());

	// eidmwGetProvParam(PP_ENUMALGS), eidmwGetProvParam(PP_ENUMALGS_EX)
	PROV_ENUMALGS xAlg;
	PROV_ENUMALGS_EX xAlgEx;
	dwDataLen = sizeof(xAlg);
	bRet = eidmwGetProvParam(hProv1, PP_ENUMALGS, (LPBYTE) &xAlg, &dwDataLen, CRYPT_FIRST);
	CHECK_EQUAL(TRUE, bRet);
	dwDataLen = sizeof(xAlgEx);
	bRet = eidmwGetProvParam(hProv1, PP_ENUMALGS_EX, (LPBYTE) &xAlgEx, &dwDataLen, CRYPT_FIRST);
	CHECK_EQUAL(TRUE, bRet);
	CHECK_EQUAL(xAlg.aiAlgid, xAlgEx.aiAlgid);
	while (bRet)
	{
		dwDataLen = sizeof(xAlg);
		bRet = eidmwGetProvParam(hProv1, PP_ENUMALGS, (LPBYTE) &xAlg, &dwDataLen, 0);
		dwDataLen = sizeof(xAlgEx);
		CHECK_EQUAL(bRet, eidmwGetProvParam(hProv1, PP_ENUMALGS_EX, (LPBYTE) &xAlgEx, &dwDataLen, 0));
		if (bRet)
			CHECK_EQUAL(xAlg.aiAlgid, xAlgEx.aiAlgid);
	}
	CHECK_EQUAL(ERROR_NO_MORE_ITEMS, GetLastError());

	// eidmwGetProvParam(PP_ENUMCONTAINERS)
	bRet = eidmwGetProvParam(hProv1, PP_ENUMCONTAINERS, NULL, &dwDataLen, CRYPT_FIRST);
	if (bRet)
	{
		DWORD dwMaxLen = dwDataLen;
		bRet = eidmwGetProvParam(hProv1, PP_ENUMCONTAINERS, tucData, &dwDataLen, CRYPT_FIRST);
		CHECK_EQUAL(TRUE, bRet);
		while (bRet)
		{
			dwDataLen = dwMaxLen;
			bRet = eidmwGetProvParam(hProv1, PP_ENUMCONTAINERS, tucData, &dwDataLen, 0);
		}
	}
	CHECK_EQUAL(ERROR_NO_MORE_ITEMS, GetLastError());

	// eidmwGetProvParam(PP_NAME)
	dwDataLen = sizeof(tucData);
	bRet = eidmwGetProvParam(hProv1, PP_NAME, tucData, &dwDataLen, 0);
	CHECK_EQUAL(TRUE, bRet);
	CHECK_EQUAL(strlen(CSP_NAME) + 1, dwDataLen);
	CHECK_EQUAL(0, strcmp(CSP_NAME, (char *) tucData));

	// eidmwGetProvParam(PP_VERSION)
	dwDataLen = sizeof(DWORD);
	bRet = eidmwGetProvParam(hProv1, PP_VERSION, tucData, &dwDataLen, 0);
	CHECK_EQUAL(TRUE, bRet);
	CHECK_EQUAL(sizeof(DWORD), dwDataLen);
	CHECK_EQUAL(CSP_VERSION, *((DWORD *) tucData));

	// eidmwGetProvParam(PP_PROVTYPE)
	dwDataLen = sizeof(DWORD);
	bRet = eidmwGetProvParam(hProv1, PP_PROVTYPE, tucData, &dwDataLen, 0);
	CHECK_EQUAL(TRUE, bRet);
	CHECK_EQUAL(sizeof(DWORD), dwDataLen);
	CHECK_EQUAL(PROV_RSA_FULL, *((DWORD *) tucData));

	// eidmwGetProvParam(PP_IMPTYPE)
	dwDataLen = sizeof(DWORD);
	bRet = eidmwGetProvParam(hProv1, PP_IMPTYPE, tucData, &dwDataLen, 0);
	CHECK_EQUAL(TRUE, bRet);
	CHECK_EQUAL(sizeof(DWORD), dwDataLen);
	CHECK_EQUAL(CRYPT_IMPL_MIXED, *((DWORD *) tucData));

	// eidmwGetProvParam(unexisting param)
	dwDataLen = sizeof(tucData);
	bRet = eidmwGetProvParam(hProv1, 123456, tucData, &dwDataLen, 0);
	CHECK_EQUAL(FALSE, bRet);
	CHECK_EQUAL(NTE_BAD_TYPE, GetLastError());

	// eidmwSetProvParam(PP_KEYSET_SEC_DESCR)
	bRet = eidmwSetProvParam(hProv1, PP_KEYSET_SEC_DESCR, tucData, 0);
	CHECK_EQUAL(TRUE, bRet);

	// eidmwSetProvParam(unexisting param)
	bRet = eidmwSetProvParam(hProv1, 123456, tucData, 0);
	CHECK_EQUAL(FALSE, bRet);
	CHECK_EQUAL(NTE_BAD_TYPE, GetLastError());

	bRet = eidmwReleaseContext(hProv1, 0);
	CHECK_EQUAL(TRUE, bRet);
}

// Test eidmwCreateHash(), eidmwDuplicateHash(), eidmwHashData(), eidmwSetHashParam(),
// eidmwGetHashParam(), eidmwDestroyHash()
TEST(Hashing)
{
	HCRYPTPROV hProv1 = 0;
	VTableProvStruc VTable; // Not used
	BOOL bRet = eidmwAcquireContext(&hProv1, NULL, CRYPT_VERIFYCONTEXT, &VTable);
	CHECK_EQUAL(TRUE, bRet);

	// Test eidmwCreateHash()

	HCRYPTHASH hHash1 = 0;
	bRet = eidmwCreateHash(hProv1, CALG_MD5, 0, 0, &hHash1);
	CHECK_EQUAL(TRUE, bRet);
	CHECK_EQUAL(1, hHash1);

	// Test:
	//   eidmwDuplicateHash() after data has been added
	//   eidmwHashData()
	//   eidmwGetHashParam

	HCRYPTHASH hHash2 = 0;
	bRet = eidmwDuplicateHash(hProv1, hHash1, NULL, 0, &hHash2);
	CHECK_EQUAL(TRUE, bRet);
	CHECK_EQUAL(2, hHash2);

	ALG_ID Algid;
	DWORD dwAlgidLen = sizeof(ALG_ID);
	bRet = eidmwGetHashParam(hProv1, hHash2, HP_ALGID,
		(LPBYTE) &Algid, &dwAlgidLen, 0);
	CHECK_EQUAL(CALG_MD5, Algid);

	unsigned char tuc2MD5[] = {0x90,0x01,0x50,0x98,0x3c,0xd2,
		0x4f,0xb0,0xd6,0x96,0x3f,0x7d,0x28,0xe1,0x7f,0x72};
	CByteArray o2MD5(tuc2MD5, sizeof(tuc2MD5));
	bRet = eidmwHashData(hProv1, hHash2, (const BYTE *) "abc", 3, 0);
	CHECK_EQUAL(TRUE, bRet);

	BYTE tucHash2[100];
	DWORD dwHash2Len;
	DWORD dwHash2LenLen = sizeof(DWORD);
	bRet = eidmwGetHashParam(hProv1, hHash2, HP_HASHSIZE,
		(LPBYTE) &dwHash2Len, &dwHash2LenLen, 0);
	CHECK_EQUAL(TRUE, bRet);
	CHECK_EQUAL(16, dwHash2Len);
	bRet = eidmwGetHashParam(hProv1, hHash2, HP_HASHVAL,
		tucHash2, &dwHash2Len, 0);
	CHECK_EQUAL(TRUE, bRet);
	CHECK_EQUAL(o2MD5.ToString(), CByteArray(tucHash2, dwHash2Len).ToString());

	// Test:
	//   eidmwDuplicateHash() after data has been added
	//   eidmwHashData()
	//   eidmwGetHashParam

	bRet = eidmwHashData(hProv1, hHash1, (const BYTE *) "12345", 5, 0);
	CHECK_EQUAL(TRUE, bRet);

	HCRYPTHASH hHash3 = 0;
	bRet = eidmwDuplicateHash(hProv1, hHash1, NULL, 0, &hHash3);
	CHECK_EQUAL(TRUE, bRet);
	CHECK_EQUAL(3, hHash3);

	bRet = eidmwHashData(hProv1, hHash1, (const BYTE *) "67890", 5, 0);
	CHECK_EQUAL(TRUE, bRet);
	bRet = eidmwHashData(hProv1, hHash3, (const BYTE *) "67890", 5, 0);
	CHECK_EQUAL(TRUE, bRet);

	BYTE tucHash1[100];
	DWORD dwHash1Len = sizeof(tucHash1);
	bRet = eidmwGetHashParam(hProv1, hHash1, HP_HASHVAL,
		tucHash1, &dwHash1Len, 0);
	CHECK_EQUAL(TRUE, bRet);
	CHECK_EQUAL(16, dwHash1Len);

	BYTE tucHash3[100];
	DWORD dwHash3Len = sizeof(tucHash3);
	bRet = eidmwGetHashParam(hProv1, hHash3, HP_HASHVAL,
		tucHash3, &dwHash3Len, 0);
	CHECK_EQUAL(TRUE, bRet);
	CHECK_EQUAL(16, dwHash3Len);

	CHECK_EQUAL(CByteArray(tucHash1, dwHash1Len).ToString(),
		CByteArray(tucHash3, dwHash3Len).ToString());

	dwHash3Len = 5; // too short
	bRet = eidmwGetHashParam(hProv1, hHash3, HP_HASHVAL,
		tucHash3, &dwHash3Len, 0);
	CHECK_EQUAL(FALSE, bRet);
	CHECK_EQUAL(ERROR_MORE_DATA, GetLastError());

	// Test:
	//  eidmwDestroyHash()
	//  eidmwSetHashParam()

	bRet = eidmwDestroyHash(hProv1, hHash2);
	CHECK_EQUAL(TRUE, bRet);
	bRet = eidmwDestroyHash(hProv1, hHash2);
	CHECK_EQUAL(FALSE, bRet);

	HCRYPTHASH hHash4 = 0;
	bRet = eidmwDuplicateHash(hProv1, hHash1, NULL, 0, &hHash4);
	CHECK_EQUAL(TRUE, bRet);
	CHECK_EQUAL(4, hHash4);

	unsigned char tucHash4[16] = {0x11};
	bRet = eidmwSetHashParam(hProv1, hHash4, HP_HASHVAL, tucHash4, 0);
	CHECK_EQUAL(TRUE, bRet);

	unsigned char tucHash[16];
	DWORD dwHashLen = sizeof(tucHash);
	bRet = eidmwGetHashParam(hProv1, hHash4, HP_HASHVAL,
		tucHash, &dwHashLen, 0);
	CHECK_EQUAL(TRUE, bRet);
	CHECK_EQUAL(CByteArray(tucHash4, sizeof(tucHash4)).ToString(),
		CByteArray(tucHash, dwHashLen).ToString());

	dwHash3Len = sizeof(tucHash3);
	bRet = eidmwGetHashParam(hProv1, hHash3, HP_HASHVAL,
		tucHash3, &dwHash3Len, 0);
	CHECK_EQUAL(TRUE, bRet);
	CHECK_EQUAL(CByteArray(tucHash1, dwHash1Len).ToString(),
		CByteArray(tucHash3, dwHash3Len).ToString());

	bRet = eidmwDestroyHash(hProv1, hHash1);
	CHECK_EQUAL(TRUE, bRet);
	bRet = eidmwDestroyHash(hProv1, hHash1);
	CHECK_EQUAL(FALSE, bRet);

	bRet = eidmwDestroyHash(hProv1, hHash4);
	CHECK_EQUAL(TRUE, bRet);
	bRet = eidmwDestroyHash(hProv1, hHash4);
	CHECK_EQUAL(FALSE, bRet);

	bRet = eidmwReleaseContext(hProv1, 0);
	CHECK_EQUAL(TRUE, bRet);
}

// Test eidmwGenRandom()
// "Emulated reader 2" contains a Belpic card with serial
// number 534C494E336600296CFF2623660B0826.
TEST(Random)
{
	unsigned char tucRand[20];
	VTableProvStruc VTable; // Not used

	// Using the old container name format

	HCRYPTPROV hProv1 = 0;
	BOOL bRet = eidmwAcquireContext(&hProv1,
		"Signature(534C494E336600296CFF2623660B0826)", 0, &VTable);
	CHECK_EQUAL(TRUE, bRet);
	if (!bRet)
		printf("LastError(): 0x%0x\n", GetLastError());

	bRet = eidmwGenRandom(hProv1, sizeof(tucRand), tucRand);
	CHECK_EQUAL(TRUE, bRet);
	if (!bRet)
		printf("LastError(): 0x%0x\n", GetLastError());

	// Using the new container name format

	HCRYPTPROV hProv2 = 0;
	bRet = eidmwAcquireContext(&hProv2,
		"534C494E336600296CFF2623660B0826_2", 0, &VTable);
	CHECK_EQUAL(TRUE, bRet);

	bRet = eidmwGenRandom(hProv2, sizeof(tucRand), tucRand);
	CHECK_EQUAL(TRUE, bRet);

	// No container name

	HCRYPTPROV hProv3 = 0;
	bRet = eidmwAcquireContext(&hProv3, NULL, 0, &VTable);
	CHECK_EQUAL(TRUE, bRet);

	bRet = eidmwGenRandom(hProv3, sizeof(tucRand), tucRand);
	CHECK_EQUAL(FALSE, bRet);
	CHECK_EQUAL(NTE_NO_KEY, GetLastError());

	// Non-existant container name

	HCRYPTPROV hProv4 = 0;
	bRet = eidmwAcquireContext(&hProv4, "xyz", 0, &VTable);
	CHECK_EQUAL(TRUE, bRet);

	bRet = eidmwGenRandom(hProv4, sizeof(tucRand), tucRand);
	CHECK_EQUAL(FALSE, bRet);
	CHECK_EQUAL(NTE_FAIL, GetLastError());

	bRet = eidmwReleaseContext(hProv1, 0);
	CHECK_EQUAL(TRUE, bRet);
}

// Test eidmwSignHash(), eidmwGetUserKey(), eidmwGetKeyParam(),
// eidmwVerifySignature(), eidmwDestroyKey(), eidmwExportKey()
// Here we use the emulated card in "Emulated reader 0"
// which contains real keys and corresponding certs.
TEST(Keys)
{
	unsigned char tucData[] = {'t', 'e', 's', 't', 'j', 'e'};

	VTableProvStruc VTable; // Not used

	// Using the old container name format

	HCRYPTPROV hProv1 = 0;
	BOOL bRet = eidmwAcquireContext(&hProv1,
		"Authentication(534C494E336600296CFF2623000000E1)", 0, &VTable);
	CHECK_EQUAL(TRUE, bRet);

	HCRYPTHASH hHash1 = 0;
	bRet = eidmwCreateHash(hProv1, CALG_SHA1, 0, 0, &hHash1);
	CHECK_EQUAL(TRUE, bRet);

	bRet = eidmwHashData(hProv1, hHash1, tucData, sizeof(tucData), 0);
	CHECK_EQUAL(TRUE, bRet);
/*
	// We do this to avoid a PIN dialog being popped up for each run of these unit tests
	CHECK_EQUAL(true, VerifyPIN("Emulated reader 0"));

	// Test eidmwSignHash()
	unsigned char tucSig[512];
	DWORD dwSigLen = sizeof(tucSig);
	bRet = eidmwSignHash(hProv1, hHash1, AT_KEYEXCHANGE, NULL, 0, NULL, &dwSigLen);
	CHECK_EQUAL(TRUE, bRet);
	bRet = eidmwSignHash(hProv1, hHash1, AT_KEYEXCHANGE, NULL, 0, tucSig, &dwSigLen);
	CHECK_EQUAL(TRUE, bRet);

	bRet = eidmwDestroyHash(hProv1, hHash1);
	CHECK_EQUAL(TRUE, bRet);

	// Normally the container named "Authentication(534C494E336600296CFF2623000000E1)"
	// should exist in the MY certificate store. But to allow unit tests (on all PCs)
	// the FOR_UNIT_TESTS_ONLY #define makes the CSP accept this container name and
	// return the hardcoded public key info corresponding to the Auth cert of this
	// emuated card.
	HCRYPTKEY hKey1 = 0;
	bRet = eidmwGetUserKey(hProv1, AT_KEYEXCHANGE, &hKey1);
	CHECK_EQUAL(TRUE, bRet);

	// eidmwGetKeyParam()
	ALG_ID algid;
	DWORD dwLen;
	bRet = eidmwGetKeyParam(hProv1, hKey1, KP_ALGID, NULL, &dwLen, 0);
	CHECK_EQUAL(TRUE, bRet);
	CHECK_EQUAL(sizeof(ALG_ID), dwLen);
	bRet = eidmwGetKeyParam(hProv1, hKey1, KP_ALGID, (LPBYTE) &algid, &dwLen, 0);
	CHECK_EQUAL(TRUE, bRet);
	CHECK_EQUAL(CALG_RSA_KEYX, algid); // TODO(?): current CSP returns CALG_RSA_SIGN !!

	HCRYPTHASH hHash2 = 0;
	bRet = eidmwCreateHash(hProv1, CALG_SHA1, 0, 0, &hHash2);
	CHECK_EQUAL(TRUE, bRet);

	bRet = eidmwHashData(hProv1, hHash2, tucData, sizeof(tucData), 0);
	CHECK_EQUAL(TRUE, bRet);

	// eidmwVerifySignature
	bRet = eidmwVerifySignature(hProv1, hHash2, tucSig, dwSigLen, hKey1, NULL, 0);
	//CHECK_EQUAL(TRUE, bRet);
	//if (!bRet)
	//	printf("=> GetLastError() = 0x%0x\n", GetLastError()); // TODO: return NTE_BAD_SIGNATURE

	bRet = eidmwDestroyHash(hProv1, hHash2);
	CHECK_EQUAL(TRUE, bRet);

	// eidmwExportKey
	unsigned char tucBlob[1000];
	dwLen = sizeof(tucBlob);
	bRet = eidmwExportKey(hProv1, hKey1, NULL, PUBLICKEYBLOB, 0, tucBlob, &dwLen);
	CHECK_EQUAL(TRUE, bRet);

	// eidmwDestroyKey
	bRet = eidmwDestroyKey(hProv1, hKey1);
	CHECK_EQUAL(TRUE, bRet);
	bRet = eidmwDestroyKey(hProv1, hKey1);
	CHECK_EQUAL(FALSE, bRet);

	bRet = eidmwReleaseContext(hProv1, 0);
	CHECK_EQUAL(TRUE, bRet);
*/
}

/////////////////////// Help function ////////////////////////

#include "../cardlayer/CardLayer.h"

static bool VerifyPIN(const char *csReaderName)
{
	CCardLayer oCardLayer;

	CReader &oReader = oCardLayer.getReader(csReaderName);

	oReader.Connect();

	tPin pin = oReader.GetPin(0);

	unsigned long ulRemaining;

	return oReader.PinCmd(PIN_OP_VERIFY, pin, "1234", "", ulRemaining);
}
