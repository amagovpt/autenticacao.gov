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
#ifndef __BASEPROVIDER_H__
#define __BASEPROVIDER_H__

#include <windows.h>
#include <wincrypt.h>
#include "ProviderHash.h"
#include "../common/Mutex.h"

namespace eIDMW
{

class CBaseProvider
{
public:
	CBaseProvider();

	~CBaseProvider();

	BOOL ImportKey(CERT_PUBLIC_KEY_INFO *pPubKeyInfo,
		DWORD dwKeySpec, OUT HCRYPTKEY *phUserKey);

	BOOL DestroyKey(HCRYPTKEY hKey);

	BOOL GetKeyParam(HCRYPTKEY hKey, DWORD dwParam,
		LPBYTE pbData, LPDWORD pcbDataLen, DWORD dwFlags);

	BOOL ExportKey(HCRYPTKEY hKey, HCRYPTKEY hPubKey, DWORD dwBlobType,
		DWORD dwFlags, LPBYTE pbData, LPDWORD pcbDataLen);

	BOOL VerifySignature(CProviderHash & oHash,
		CONST BYTE *pbSignature, DWORD cbSigLen,
		HCRYPTKEY hPubKey, LPCWSTR szDescription, DWORD dwFlags);

private:
	void GetBaseProviderHandle();

	BOOL m_bBaseProvHandleOK;
	HCRYPTPROV m_hBaseProv;
	CMutex m_oBaseProvMutex;
};

}

#endif
