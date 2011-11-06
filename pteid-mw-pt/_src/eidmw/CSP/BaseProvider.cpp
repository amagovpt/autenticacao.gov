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
#include "BaseProvider.h"
#include "../common/Log.h"
#include "../common/eidErrors.h"

using namespace eIDMW;

CBaseProvider::CBaseProvider()
{
	m_bBaseProvHandleOK = false;
}

CBaseProvider::~CBaseProvider()
{
	if (m_bBaseProvHandleOK)
		CryptReleaseContext(m_hBaseProv, 0);
}

BOOL CBaseProvider::ImportKey(CERT_PUBLIC_KEY_INFO *pPubKeyInfo,
	DWORD dwKeySpec, OUT HCRYPTKEY *phUserKey)
{
	GetBaseProviderHandle();

	return CryptImportPublicKeyInfo(m_hBaseProv,
		AT_KEYEXCHANGE, pPubKeyInfo, phUserKey);
}

BOOL CBaseProvider::DestroyKey(HCRYPTKEY hKey)
{
	GetBaseProviderHandle();

	return CryptDestroyKey(hKey);
}

BOOL CBaseProvider::GetKeyParam(HCRYPTKEY hKey, DWORD dwParam,
	LPBYTE pbData, LPDWORD pcbDataLen, DWORD dwFlags)
{
	GetBaseProviderHandle();

	return CryptGetKeyParam(hKey, dwParam, pbData, pcbDataLen, dwFlags);
}

BOOL CBaseProvider::ExportKey(HCRYPTKEY hKey, HCRYPTKEY hPubKey, DWORD dwBlobType,
	DWORD dwFlags, LPBYTE pbData, LPDWORD pcbDataLen)
{
	GetBaseProviderHandle();

	return CryptExportKey(hKey, hPubKey, dwBlobType, dwFlags, pbData, pcbDataLen);
}

BOOL CBaseProvider::VerifySignature(CProviderHash & oHash,
	CONST BYTE *pbSignature, DWORD cbSigLen,
	HCRYPTKEY hPubKey, LPCWSTR szDescription, DWORD dwFlags)
{
	GetBaseProviderHandle();

	CByteArray oHashValue = oHash.GetHashValue();

	//printf("algid: 0x%0x, hash: %s\n", oHash.GetAlgid(), oHashValue.ToString().c_str());
	//printf("sig: %s\n", CByteArray(pbSignature, cbSigLen).ToString().c_str());

	HCRYPTHASH hHash;
	BOOL bRet = CryptCreateHash(m_hBaseProv, oHash.GetAlgid(), NULL, 0, &hHash);
	if (bRet)
	{
		bRet = CryptSetHashParam(hHash, HP_HASHVAL, oHashValue.GetBytes(), 0);
		if (bRet)
		{
			bRet = CryptVerifySignature(hHash,
				pbSignature, cbSigLen, hPubKey, szDescription, dwFlags);
		}

		CryptDestroyHash(hHash);
	}

	return bRet;
}

void CBaseProvider::GetBaseProviderHandle()
{
	CAutoMutex oAutoMutex(&m_oBaseProvMutex);

	if (!m_bBaseProvHandleOK)
	{
		// Get the Base CSP name

		const char *csKey = "SOFTWARE\\Microsoft\\Cryptography\\Defaults\\Provider Types\\Type 001";
		HKEY hKey;
		LONG lRet = RegOpenKeyExA(HKEY_LOCAL_MACHINE, csKey, 0, KEY_READ, &hKey);
		if (ERROR_SUCCESS != lRet)
		{
			MWLOG(LEV_ERROR, MOD_CSP, L" Couldn't RegOpenKeyEx(): err = 0x%0x", lRet);
			throw CMWEXCEPTION(EIDMW_ERR_CHECK);
		}

		char csBaseProvName[200];
		DWORD dwLen = sizeof(csBaseProvName);
		DWORD dwType;
		lRet = RegQueryValueExA(hKey, "Name", NULL, &dwType, (LPBYTE) csBaseProvName, &dwLen);

		RegCloseKey(hKey);

		if (ERROR_SUCCESS != lRet)
		{
			MWLOG(LEV_ERROR, MOD_CSP, L" Couldn't RegQueryValueEx(): err = 0x%0x", lRet);
			throw CMWEXCEPTION(EIDMW_ERR_CHECK);
		}

		// Call CryptAcquireContext(base CSP name)

		m_bBaseProvHandleOK = CryptAcquireContextA(&m_hBaseProv, NULL,
			csBaseProvName, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT);
		if (!m_bBaseProvHandleOK)
		{
			MWLOG(LEV_ERROR, MOD_CSP, L" Couldn't CryptAcquireContext(): last error = 0x%0x", GetLastError());
			throw CMWEXCEPTION(EIDMW_ERR_CHECK);
		}
	}
}