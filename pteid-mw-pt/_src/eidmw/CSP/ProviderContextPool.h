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
 * For each CPAcquireContext() call, a CProviderContext
 * object is created. These CProviderContext objects are
 * maintained here.
 */

#ifndef __PROVIDERCONTEXTPOOL_H__
#define __PROVIDERCONTEXTPOOL_H__

#include "ProviderContext.h"
#include "../common/Mutex.h"
#include <map>

namespace eIDMW
{

typedef std::map <HCRYPTPROV, CProviderContext *> tProvCtxPool;

class CProviderContextPool
{
public:
	CProviderContextPool();
	~CProviderContextPool();

	HCRYPTPROV AddProviderContext(LPCSTR szContainer,
		DWORD dwFlags, PVTableProvStruc pVTable);

	CProviderContext * GetProviderContext(HCRYPTPROV hProv, bool bThrowException = true);

	BOOL DeleteProviderContext(HCRYPTPROV hProv);

private:
	CMutex m_oMutex;
	tProvCtxPool m_pool;
	HCRYPTPROV m_ProvCounter;
};

}

#endif
