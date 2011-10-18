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
 * object is created that takes are of maintaing all
 * info, params, ... over all CPxx() calls for this
 * provider context (referenced by a HCRYPTPROV handle).
 */ 
#ifndef __PROVIDERCONTEXT_H__
#define __PROVIDERCONTEXT_H__

#include <string>
#include <map>

#include "ProviderHash.h"
#include "BaseProvider.h"
#include "../common/MWException.h"
#include "../common/eidErrors.h"
#include "../common/Mutex.h"
#include "../cardlayer/Reader.h"

namespace eIDMW
{

typedef std::map <HCRYPTHASH, CProviderHash *> tHashPool;

class CProviderContext
{
public:
	CProviderContext(HCRYPTPROV hProv, LPCSTR szContainer,
		DWORD dwFlags, PVTableProvStruc pVTable);
	~CProviderContext();

	std::string GetContainerName();
	std::string GetSerialNr();

	HCRYPTHASH AddHash(ALG_ID Algid);
	HCRYPTHASH DuplicateHash(HCRYPTHASH hOldHash);
	CProviderHash * GetHash(HCRYPTHASH hHash, bool bThrowException = true);
	BOOL DeleteHash(HCRYPTHASH hHash);

	CBaseProvider & GetBaseProvider();

	CReader *GetReader();
	void SetReader(CReader *poReader);

private:
	std::string ExtractSerialNr(LPCSTR szContainer);

	HCRYPTPROV m_hProv;
	std::string m_szContainer;
	DWORD m_dwFlags;

	std::string m_csSerialNr;

	CMutex m_oHashMutex;
	tHashPool m_hashPool;
	HCRYPTHASH m_HashCounter;

	CBaseProvider m_BaseProvider;

	CReader *m_poReader;
};

}

#endif
