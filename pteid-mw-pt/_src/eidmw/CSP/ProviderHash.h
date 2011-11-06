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
#ifndef __PROVIDERHASH_H__
#define __PROVIDERHASH_H__

#include <windows.h>
#include "Cspdk.h"
#include "../common/Hash.h"
#include "../common/ByteArray.h"

namespace eIDMW
{

class CProviderHash
{
public:
	CProviderHash(ALG_ID Algid, tHashAlgo algo);
	CProviderHash(const CProviderHash & oProvHash);

	ALG_ID GetAlgid() const;
	unsigned long GetLengthBytes() const;
	void Update(const CByteArray &oData);
	void SetHashValue(const CByteArray &oHashValue);
	CByteArray & GetHashValue();
	CHash & GetHashObject();

private:
	ALG_ID m_Algid;
	CHash m_oHash;
	CByteArray m_oValue;

};

}

#endif
