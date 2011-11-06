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
#include "ProviderContextPool.h"
#include "../common/eidErrors.h"
#include "../common/MWException.h"

using namespace eIDMW;

CProviderContextPool::CProviderContextPool()
{
	m_ProvCounter = 0;
}

CProviderContextPool::~CProviderContextPool()
{
	CAutoMutex oAutMutex(&m_oMutex);

	while (m_pool.size() > 0)
	{
		tProvCtxPool::iterator it = m_pool.begin();
		delete(it->second);
		m_pool.erase(it);
	}
}

HCRYPTPROV CProviderContextPool::AddProviderContext(LPCSTR szContainer,
	DWORD dwFlags, PVTableProvStruc pVTable)
{
	CAutoMutex oAutMutex(&m_oMutex);

	// Provider handle = 1 for the 1st context, 2 for the 2nd context, ...
	HCRYPTPROV hProv = ++m_ProvCounter;

	CProviderContext *poProvCtx = new CProviderContext(hProv,
		szContainer, dwFlags, pVTable);
	if (poProvCtx == NULL)
		throw CMWEXCEPTION(EIDMW_ERR_MEMORY);

	m_pool[hProv] = poProvCtx;

	return hProv;
}

CProviderContext * CProviderContextPool::GetProviderContext(
	HCRYPTPROV hProv, bool bThrowException)
{
	CProviderContext *poProvCtx = NULL;

	CAutoMutex oAutMutex(&m_oMutex);

	tProvCtxPool::iterator it = m_pool.begin();
	for ( ; it != m_pool.end(); it++)
	{
		if (it->first == hProv)
		{
			poProvCtx = it->second;
			break;
		}
	}

	if (poProvCtx == NULL && bThrowException)
		throw CMWEXCEPTION(EIDMW_ERR_PARAM_BAD);

	return poProvCtx;
}

BOOL CProviderContextPool::DeleteProviderContext(HCRYPTPROV hProv)
{
	BOOL bRet = FALSE;

	CAutoMutex oAutMutex(&m_oMutex);

	tProvCtxPool::iterator it = m_pool.begin();
	for ( ; it != m_pool.end(); it++)
	{
		if (it->first == hProv)
		{
			delete it->second;
			m_pool.erase(it);
			bRet = TRUE;
			break;
		}
	}

	return bRet;
}
