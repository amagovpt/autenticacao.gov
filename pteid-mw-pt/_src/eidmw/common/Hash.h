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
 * This class takes care of hashing (SHA1, MD5, ..)
 *
 * Instead of providing the result of GetHash() to the
 * CCard::Sign() function, it is better to provide the
 * hash object itself to CCard::Sign(); this way it's
 * possible for the CardLayer to do 'partial signatures'
 * in which the last part of the hash is calculated on
 * the card.
 * Example:
 *   CByteArray sign(CCard &oCard, const tPrivKey & key, const tPin & Pin)
 *   {
 *     CHash oHash;
 *     oHash.Init(ALGO_MD5_SHA1);
 *     oHash.Update(...);
 *     oHash.Update(...);
 *     return oCard.Sign(key, Pin, SIGN_ALGO_RSA_PKCS, oHash);
 *   }
 *
 * TODO: add support for partial signing (when needed).
 */

#pragma once

#ifndef HASH_H
#define HASH_H

#include "Export.h"
#include "ByteArray.h"
#include "libtomcrypt/tomcrypt_hash.h"

namespace eIDMW
{

enum tHashAlgo {
	ALGO_MD5,       // 16-byte hash
	ALGO_SHA1,      // 20-byte hash
	ALGO_MD5_SHA1,  // 36-byte hash, useful for SSL/TLS
	ALGO_SHA256,    // 32-byte hash
	ALGO_SHA384,    // 48-byte hash
	ALGO_SHA512,    // 64-byte hash
	ALGO_RIPEMD160, // 64-byte hash
};

class EIDMW_CMN_API CHash 
{
public:
	CHash();

	static unsigned long GetHashLength(tHashAlgo algo);

	/* All at once */
	CByteArray Hash(tHashAlgo algo, const CByteArray & data);
	CByteArray Hash(tHashAlgo algo, const CByteArray & data,
		unsigned long ulOffset, unsigned long ulLen);

	void Init(tHashAlgo algo);
	void Update(const CByteArray & data);
	void Update(const CByteArray & data, unsigned long ulOffset, unsigned long ulLen);
	CByteArray GetHash();

private:
	hash_state m_md1;
	hash_state m_md2;
	tHashAlgo m_Algo;
	bool m_bInitialized;
};

}
#endif
