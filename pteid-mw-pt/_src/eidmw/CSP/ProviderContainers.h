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
 * Helper class to list the key containers of this CSP,
 * to check if a certain key container exists and to
 * extract the public key info for a certain container.
 * This is done by looking in the "MY" certificate strore
 * for certificates that were registered for this provider.
 */

#ifndef __PROVIDERCONTAINERS_H__
#define __PROVIDERCONTAINERS_H__

#include <windows.h>
#include "Cspdk.h"
#include <vector>
#include "../common/Mutex.h"

namespace eIDMW
{

class CProviderContainers
{
public:
	CProviderContainers();

	BOOL EnumContainers(LPBYTE pbData, LPDWORD pcbDataLen, DWORD dwFlags);

	BOOL ContainerExists(LPCSTR szContainer);

	BOOL GetPublicKeyInfo(const std::string &csContainerName,
		CERT_PUBLIC_KEY_INFO *pPubKeyInfo);

private:
	void ListContainers();
	BOOL ListContainersGetPublicKeyInfo(
		const wchar_t *wsName, CERT_PUBLIC_KEY_INFO *pPubKeyInfo);

	size_t m_index;
	BOOL m_bContainersListed;
	std::vector <std::string> m_containinerNames;
	CMutex m_oProvContMutex;
};

}

#endif
