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
#include "../CSP/ProviderContainers.h"

using namespace eIDMW;

TEST(CProviderContainers)
{
	CProviderContainers oProvContainers;
	char csContainer1[400];
	char csContainer2[400];
	DWORD dwContainer1Len = sizeof(csContainer1);
	DWORD dwContainer2Len = sizeof(csContainer2);

	BOOL bFound = oProvContainers.EnumContainers(
		(LPBYTE) csContainer1, &dwContainer1Len, CRYPT_FIRST);

	// It's possible that no containers are present on this PC
	// and for this user. In that case, you should register some
	// certs first using the GUI or the tray applet (at least if
	// you want to these tests).
	if (!bFound)
		printf("Note: no certs have been registered, skipping the CProviderContainers test\n");
	else
	{
		bFound = oProvContainers.EnumContainers(
			(LPBYTE) csContainer2, &dwContainer2Len, CRYPT_FIRST);
		CHECK_EQUAL(TRUE, bFound);
		CHECK_EQUAL(dwContainer1Len, dwContainer2Len);
		CHECK_EQUAL(0, memcmp(csContainer1, csContainer2, dwContainer1Len));

		dwContainer1Len = 0;
		bFound = oProvContainers.EnumContainers(NULL, &dwContainer1Len, CRYPT_FIRST);
		CHECK_EQUAL(TRUE, bFound);
		CHECK_EQUAL(true, dwContainer2Len <= dwContainer1Len);

		dwContainer1Len = 2;
		bFound = oProvContainers.EnumContainers(
			(LPBYTE) csContainer1, &dwContainer1Len, CRYPT_FIRST);
		CHECK_EQUAL(FALSE, bFound);
		CHECK_EQUAL(ERROR_MORE_DATA, GetLastError());

		// Test GetPublicKeyInfo()
		CERT_PUBLIC_KEY_INFO xPubKeyInfo;
		memset(&xPubKeyInfo, 0, sizeof(xPubKeyInfo));
		bFound = oProvContainers.GetPublicKeyInfo(csContainer1, &xPubKeyInfo);
		CHECK_EQUAL(TRUE, bFound);
		CHECK_EQUAL(true, xPubKeyInfo.PublicKey.cbData != 0);

		// Count the amount of containers
		int iContainerCount = 0;
		dwContainer1Len = sizeof(csContainer1);
		bFound = oProvContainers.EnumContainers(
			(LPBYTE) csContainer1, &dwContainer1Len, CRYPT_FIRST);
		while (bFound)
		{
			iContainerCount++;
			dwContainer1Len = sizeof(csContainer1);
			bFound = oProvContainers.EnumContainers(
				(LPBYTE) csContainer1, &dwContainer1Len, 0);
		}
		CHECK_EQUAL(ERROR_NO_MORE_ITEMS, GetLastError());

		// Count the amount of containers again, but now ask the length each time
		int iNewContainerCount = 0;
		dwContainer1Len = sizeof(csContainer1);
		bFound = oProvContainers.EnumContainers(
			(LPBYTE) csContainer1, &dwContainer1Len, CRYPT_FIRST);
		while (bFound)
		{
			iNewContainerCount++;
			bFound = oProvContainers.EnumContainers(
				NULL, &dwContainer1Len, 0);
			bFound = oProvContainers.EnumContainers(
				(LPBYTE) csContainer1, &dwContainer1Len, 0);
		}
		CHECK_EQUAL(ERROR_NO_MORE_ITEMS, GetLastError());

		CHECK_EQUAL(iContainerCount, iNewContainerCount);
	}
}
