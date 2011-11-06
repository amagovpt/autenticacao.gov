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
#include "../CSP/csputil.h"

using namespace eIDMW;

TEST(FillProvParam)
{
	unsigned char tucDataIn[5] = {0x11, 0x22, 0x33, 0x44, 0x55};
	unsigned char tucDataOut[10];
	DWORD dwOutLen;

	BOOL bRet = FillProvParam(NULL, &dwOutLen, tucDataIn, 100);
	CHECK_EQUAL(TRUE, bRet);
	CHECK_EQUAL(100, dwOutLen);

	dwOutLen = sizeof(tucDataOut);
	bRet = FillProvParam(tucDataOut, &dwOutLen, tucDataIn, sizeof(tucDataIn));
	CHECK_EQUAL(TRUE, bRet);
	CHECK_EQUAL(sizeof(tucDataIn), dwOutLen);
	CHECK_EQUAL(0, memcmp(tucDataIn, tucDataOut, dwOutLen));

	dwOutLen = sizeof(tucDataIn) - 1;
	bRet = FillProvParam(tucDataOut, &dwOutLen, tucDataIn, sizeof(tucDataIn));
	CHECK_EQUAL(FALSE, bRet);
	CHECK_EQUAL(ERROR_MORE_DATA, GetLastError());
}

TEST(PubKeyInfo)
{
	bool bTestDone = false;

	HCERTSTORE hCertStore = CertOpenStore(CERT_STORE_PROV_SYSTEM, 0, NULL,
		CERT_SYSTEM_STORE_CURRENT_USER, L"ROOT");
	if (hCertStore != NULL)
	{
		PCCERT_CONTEXT pCertContext = CertEnumCertificatesInStore(hCertStore, NULL);
		if (pCertContext != NULL)
		{
			CERT_PUBLIC_KEY_INFO xPubKey;
			CERT_PUBLIC_KEY_INFO *pxPubKeyIn = &pCertContext->pCertInfo->SubjectPublicKeyInfo;

			BOOL bRet = DuplicatePubKeyInfo(&xPubKey, pxPubKeyIn);
			CHECK_EQUAL(TRUE, bRet);

			CHECK_EQUAL(pxPubKeyIn->Algorithm.Parameters.cbData, xPubKey.Algorithm.Parameters.cbData);
			CHECK_EQUAL(0, memcmp(pxPubKeyIn->Algorithm.Parameters.pbData,
				xPubKey.Algorithm.Parameters.pbData, xPubKey.Algorithm.Parameters.cbData));
			CHECK_EQUAL(pxPubKeyIn->Algorithm.pszObjId, xPubKey.Algorithm.pszObjId);

			CHECK_EQUAL(pxPubKeyIn->PublicKey.cbData, xPubKey.PublicKey.cbData); 
			CHECK_EQUAL(0, memcmp(pxPubKeyIn->PublicKey.pbData,
				xPubKey.PublicKey.pbData, xPubKey.PublicKey.cbData));

			FreePubKeyInfo(&xPubKey);

			bTestDone = true;
		}

		CertCloseStore(hCertStore, 0);
	}

	CHECK_EQUAL(true, bTestDone);
}
