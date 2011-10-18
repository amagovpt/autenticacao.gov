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
#include "ProviderAlgorithms.h"

using namespace eIDMW;

typedef struct
{
	ALG_ID aiAlgid;
	DWORD dwDefaultLen;
	DWORD dwMinLen;
	DWORD dwMaxLen;
	const char *csShortName;
	const char *csLongName;
} ProvAlg;

static ProvAlg tProvAlgs[] =
{
	{CALG_MD5, 128, 128, 128, "MD5", "Message Digest 5 (MD5)"},
	{CALG_SHA1, 160, 160, 160, "SHA-1", "Secure Hash Algorithm (SHA-1)"},
	{CALG_SSL3_SHAMD5, 288, 288, 288, "SSL3 SHAMD5", "SSL3 SHAMD5"},
	{CALG_RSA_SIGN, 1024, 1024, 4096, "RSA_SIGN", "RSA Signature"},
};

#define ALGO_COUNT (sizeof(tProvAlgs) / sizeof(ProvAlg))

CProviderAlgorithms::CProviderAlgorithms()
{
	iEnumAlgsIndex = 0;
	iEnumAlgsExIndex = 0;
}

BOOL CProviderAlgorithms::CheckArgs(int index, DWORD dwDataLen,
	LPBYTE pbData, LPDWORD pcbDataLen)
{
	BOOL bReturnVal = FALSE;

	if (index >= ALGO_COUNT)
		SetLastError(ERROR_NO_MORE_ITEMS);
	else if (pbData == NULL)
	{
		bReturnVal = TRUE;
		*pcbDataLen = dwDataLen;
	}
	else if (dwDataLen > *pcbDataLen)
		SetLastError(ERROR_MORE_DATA);
	else
		bReturnVal = TRUE;

	if (!bReturnVal)
		*pcbDataLen = 0;

	return bReturnVal;
}

BOOL CProviderAlgorithms::EnumAlgs(LPBYTE pbData, LPDWORD pcbDataLen, DWORD dwFlags)
{
	if (dwFlags & CRYPT_FIRST)
		iEnumAlgsIndex = 0;

	BOOL bReturnVal = CheckArgs(iEnumAlgsIndex, sizeof(PROV_ENUMALGS),
		pbData, pcbDataLen);

	if (bReturnVal && pbData != NULL)
	{
		PROV_ENUMALGS *pProvAlgs = (PROV_ENUMALGS *) pbData;
		memset(pProvAlgs, 0, sizeof(PROV_ENUMALGS));
		pProvAlgs->aiAlgid = tProvAlgs[iEnumAlgsIndex].aiAlgid;
		pProvAlgs->dwBitLen = tProvAlgs[iEnumAlgsIndex].dwDefaultLen;
		pProvAlgs->dwNameLen = (DWORD) strlen(tProvAlgs[iEnumAlgsIndex].csShortName) + 1;
		memcpy(pProvAlgs->szName, tProvAlgs[iEnumAlgsIndex].csShortName, pProvAlgs->dwNameLen);

		*pcbDataLen = sizeof(PROV_ENUMALGS);

		iEnumAlgsIndex++;
	}	

	return bReturnVal;
}

BOOL CProviderAlgorithms::EnumAlgsEx(LPBYTE pbData, LPDWORD pcbDataLen, DWORD dwFlags)
{
	if (dwFlags & CRYPT_FIRST)
		iEnumAlgsExIndex = 0;

	BOOL bReturnVal = CheckArgs(iEnumAlgsExIndex, sizeof(PROV_ENUMALGS_EX),
		pbData, pcbDataLen);

	if (bReturnVal && pbData != NULL)
	{
		PROV_ENUMALGS_EX *pProvAlgs = (PROV_ENUMALGS_EX *) pbData;
		memset(pProvAlgs, 0, sizeof(PROV_ENUMALGS_EX));
		pProvAlgs->aiAlgid = tProvAlgs[iEnumAlgsExIndex].aiAlgid;
		pProvAlgs->dwDefaultLen = tProvAlgs[iEnumAlgsExIndex].dwDefaultLen;
		pProvAlgs->dwMinLen = tProvAlgs[iEnumAlgsExIndex].dwMinLen;
		pProvAlgs->dwMaxLen = tProvAlgs[iEnumAlgsExIndex].dwMaxLen;
		pProvAlgs->dwProtocols = 0; // ???
		pProvAlgs->dwNameLen = (DWORD) strlen(tProvAlgs[iEnumAlgsExIndex].csShortName) + 1;
		memcpy(pProvAlgs->szName, tProvAlgs[iEnumAlgsExIndex].csShortName, pProvAlgs->dwNameLen);
		pProvAlgs->dwLongNameLen = (DWORD) strlen(tProvAlgs[iEnumAlgsExIndex].csLongName) + 1;
		memcpy(pProvAlgs->szLongName, tProvAlgs[iEnumAlgsExIndex].csLongName, pProvAlgs->dwLongNameLen);

		*pcbDataLen = sizeof(PROV_ENUMALGS_EX);

		iEnumAlgsExIndex++;
	}	

	return bReturnVal;
}
