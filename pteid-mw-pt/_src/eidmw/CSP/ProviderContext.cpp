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
#include "ProviderContext.h"
#include "csputil.h"

using namespace eIDMW;

CProviderContext::CProviderContext(HCRYPTPROV hProv, LPCSTR szContainer,
	DWORD dwFlags, PVTableProvStruc pVTable)
{
	m_hProv = hProv;
	m_szContainer = szContainer == NULL ? "" : szContainer;
	m_dwFlags = dwFlags;

	m_csSerialNr = szContainer == NULL ? "" : ExtractSerialNr(szContainer);

	m_HashCounter = 0;

	m_poReader = NULL;
}

CProviderContext::~CProviderContext()
{
	CAutoMutex oAutoMutex(&m_oHashMutex);

	while (m_hashPool.size() > 0)
	{
		tHashPool::iterator it = m_hashPool.begin();
		delete it->second;
		m_hashPool.erase(it);
	}
}

std::string CProviderContext::GetContainerName()
{
	return m_szContainer;
}

std::string CProviderContext::GetSerialNr()
{
	return m_csSerialNr;
}

HCRYPTHASH CProviderContext::AddHash(ALG_ID Algid)
{
	tHashAlgo algo = GetHashAlgo(Algid);

	CAutoMutex oAutoMutex(&m_oHashMutex);

	HCRYPTHASH hHash = ++m_HashCounter;

	CProviderHash *poProvHash = new CProviderHash(Algid, algo);
	if (poProvHash == NULL)
		throw CMWEXCEPTION(EIDMW_ERR_MEMORY);

	m_hashPool[hHash] = poProvHash;

	return hHash;
}

HCRYPTHASH CProviderContext::DuplicateHash(HCRYPTHASH hOldHash)
{
	CProviderHash *poOldHash = GetHash(hOldHash);

	CAutoMutex oAutoMutex(&m_oHashMutex);

	HCRYPTHASH hNewHash = ++m_HashCounter;

	CProviderHash * poNewHash = new CProviderHash(*poOldHash);
	if (poNewHash == NULL)
		throw CMWEXCEPTION(EIDMW_ERR_MEMORY);

	m_hashPool[hNewHash] = poNewHash;

	return hNewHash;
}

CProviderHash * CProviderContext::GetHash(HCRYPTHASH hHash, bool bThrowException)
{
	CProviderHash *poProvHash = NULL;

	CAutoMutex oAutoMutex(&m_oHashMutex);

	tHashPool::iterator it = m_hashPool.begin();
	for( ; it != m_hashPool.end(); it++)
	{
		if (it->first == hHash)
			poProvHash = it->second;
	}

	if (poProvHash == NULL && bThrowException)
		throw CMWEXCEPTION(EIDMW_ERR_PARAM_BAD);

	return poProvHash;
}

BOOL CProviderContext::DeleteHash(HCRYPTHASH hHash)
{
	bool bRet = FALSE;

	CAutoMutex oAutoMutex(&m_oHashMutex);

	tHashPool::iterator it = m_hashPool.begin();
	for( ; it != m_hashPool.end(); it++)
	{
		if (it->first == hHash)
		{
			delete it->second;
			m_hashPool.erase(it);
			bRet = TRUE;
			break;
		}
	}

	return bRet;
}

/**
 * Old container name format:
 *   Signature(534C494E336600296CFF2623660B0826)
 *   Authentication(534C494E336600296CFF2491AA090425)
 * New container name format:
 *   534C494E336600296CFF2623660B0826_2
 *   534C494E336600296CFF2491AA090425_3
 *   => the number after the _ is the key/cert ID
 */
std::string CProviderContext::ExtractSerialNr(LPCSTR szContainer)
{
	char csSerial[129];
	csSerial[0] = '\0';

	const char *csStart = strstr(szContainer, "(");
	if (csStart != NULL)
	{
		csStart++;
		const char *csEnd = strstr(csStart, ")");
		if (csEnd != NULL)
		{
			size_t len = csEnd - csStart;
			if (len < sizeof(csSerial) - 1)
			{
				memcpy(csSerial, csStart, len);
				csSerial[len] = '\0';
			}
		}
	}
	else
	{
		csStart = szContainer;
		while (*csStart != '\0' && (*csStart > '9' || *csStart < '0'))
			csStart++;
		const char *csEnd = strstr(csStart, "_");
		if (csEnd != NULL)
		{
			size_t len = csEnd - csStart;
			if (len < sizeof(csSerial) - 1)
			{
				memcpy(csSerial, csStart, len);
				csSerial[len] = '\0';
			}
		}
	}

	return csSerial;
}

CBaseProvider & CProviderContext::GetBaseProvider()
{
	return m_BaseProvider;
}

CReader *CProviderContext::GetReader()
{
	return m_poReader;
}

void CProviderContext:: SetReader(CReader *poReader)
{
	m_poReader = poReader;
}
