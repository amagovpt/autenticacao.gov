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
#include "csputil.h"
#include "../common/Log.h"

namespace eIDMW
{

BOOL FillProvParam(LPBYTE pbData, LPDWORD pcbDataLen,
	  const void *dataIn, size_t dataLen)
{
	BOOL bReturnVal = TRUE;

	if (pbData == NULL)
		*pcbDataLen = (DWORD) dataLen;
	else if (*pcbDataLen < dataLen)
	{
		SetLastError(ERROR_MORE_DATA);
		bReturnVal = FALSE;
	}
	else
	{
		memcpy(pbData, dataIn, dataLen);
		*pcbDataLen = (DWORD) dataLen;
	}

	return bReturnVal;
}

tHashAlgo GetHashAlgo(ALG_ID Algid)
{
	switch (Algid)
	{
	case CALG_MD5:
		return ALGO_MD5;
	case CALG_SHA1:
		return ALGO_SHA1;
	case CALG_SHA_256:
		return ALGO_SHA256;
	case CALG_SSL3_SHAMD5:
		return ALGO_MD5_SHA1;
	default:
		MWLOG(LEV_INFO, MOD_CSP, L"  Hash algo: unknown (returning an error)");
		throw CMWEXCEPTION(EIDMW_ERR_PARAM_BAD);
	}
}

unsigned long GetSignAlgo(ALG_ID Algid)
{
	switch (Algid)
	{
	case CALG_MD5:
		return SIGN_ALGO_MD5_RSA_PKCS;
	case CALG_SHA1:
		return SIGN_ALGO_SHA1_RSA_PKCS;
	case CALG_SHA_256:
		return SIGN_ALGO_SHA256_RSA_PKCS;
	case CALG_SSL3_SHAMD5:
		return SIGN_ALGO_RSA_PKCS;
	default:
		MWLOG(LEV_INFO, MOD_CSP, L"  Hash algo: unknown (returning an error)");
		throw CMWEXCEPTION(EIDMW_ERR_PARAM_BAD);
	}
}

void ReverseByteOrder (BYTE *pByte, DWORD dwLen)
{
   for (DWORD i = 0; i < (dwLen / 2); i++)
   {
      BYTE ucTemp = pByte[dwLen-1-i];
      pByte [dwLen-1-i] = pByte[i];
      pByte[i] = ucTemp;
   }
}

BOOL DuplicatePubKeyInfo(CERT_PUBLIC_KEY_INFO *pDest, const CERT_PUBLIC_KEY_INFO *pSrc)
{
	BOOL bOK = FALSE;

	*pDest = *pSrc;

	pDest->Algorithm.Parameters.pbData = new unsigned char[pSrc->Algorithm.Parameters.cbData];
	pDest->PublicKey.pbData = new unsigned char[pSrc->PublicKey.cbData];

	bOK = pDest->Algorithm.Parameters.pbData && pDest->PublicKey.pbData;

	if (bOK)
	{
		memcpy(pDest->Algorithm.Parameters.pbData, pSrc->Algorithm.Parameters.pbData,
			pSrc->Algorithm.Parameters.cbData);
		memcpy(pDest->PublicKey.pbData, pSrc->PublicKey.pbData, pSrc->PublicKey.cbData);
	}
	else
		FreePubKeyInfo(pDest);

	return bOK;
}

void FreePubKeyInfo(CERT_PUBLIC_KEY_INFO *pPubKeyInfo)
{
	if (pPubKeyInfo->Algorithm.Parameters.pbData)
		delete pPubKeyInfo->Algorithm.Parameters.pbData;
	pPubKeyInfo->Algorithm.Parameters.pbData = NULL;
	if (pPubKeyInfo->PublicKey.pbData)
		delete pPubKeyInfo->PublicKey.pbData;
	pPubKeyInfo->PublicKey.pbData = NULL;
}

}
