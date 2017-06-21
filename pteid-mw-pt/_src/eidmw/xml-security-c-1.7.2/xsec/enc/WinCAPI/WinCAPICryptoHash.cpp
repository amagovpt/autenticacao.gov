/**
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements. See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership. The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

/*
 * XSEC
 *
 * WinCAPICryptoHash := Windows CAPI Implementation of Message digests
 *
 * Author(s): Berin Lautenbach
 *
 * $Id: WinCAPICryptoHash.cpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#include <xsec/enc/WinCAPI/WinCAPICryptoHash.hpp>
#include <xsec/enc/XSECCryptoException.hpp>
#include <xsec/enc/WinCAPI/WinCAPICryptoProvider.hpp>

#if defined (XSEC_HAVE_WINCAPI)

#include <memory.h>

// Constructors/Destructors

WinCAPICryptoHash::WinCAPICryptoHash(HCRYPTPROV prov, HashType alg) {

	m_p = prov;
	m_hashType = alg;
	m_h = 0;

	reset();
}


WinCAPICryptoHash::~WinCAPICryptoHash() {

	if (m_h != 0)
		CryptDestroyHash(m_h);

}



// Hashing Activities
void WinCAPICryptoHash::reset(void) {

	if (m_h != 0)
		CryptDestroyHash(m_h);

	m_h = 0;
	
	BOOL fResult;
	ALG_ID alg_id;

	switch (m_hashType) {

	case (XSECCryptoHash::HASH_SHA1) :
	
		alg_id = CALG_SHA;
		break;

	case (XSECCryptoHash::HASH_MD5) :
	
		alg_id = CALG_MD5;
		break;

	default :

		alg_id = 0;

	}

	if(alg_id == 0) {

		throw XSECCryptoException(XSECCryptoException::MDError,
			"WinCAPI:Hash - Unknown algorithm (NOTE SHA512 and SHA256 not supported"); 
	}

	fResult = CryptCreateHash(
		m_p,
		alg_id,
		0,
		0,
		&m_h);

	if (fResult == 0 || m_h == 0) {
		throw XSECCryptoException(XSECCryptoException::MDError,
			"WinCAPI:Hash - Error creating hash object"); 
	}

}

void WinCAPICryptoHash::hash(unsigned char * data, 
								 unsigned int length) {

	BOOL fResult = CryptHashData(
		m_h,
		data,
		length,
		0);

	if (fResult == 0) {
		throw XSECCryptoException(XSECCryptoException::MDError,
			"WinCAPI:Hash - Error Hashing Data"); 
	}

}

unsigned int WinCAPICryptoHash::finish(unsigned char * hash,
									   unsigned int maxLength) {

	DWORD retLen;
	BOOL fResult;

	retLen = WINCAPI_MAX_HASH_SIZE;

	fResult = CryptGetHashParam(
		m_h,
		HP_HASHVAL,
		m_mdValue,
		&retLen,
		0);

	if (fResult == 0) {
		throw XSECCryptoException(XSECCryptoException::MDError,
			"WinCAPI:Hash - Error getting hash value"); 
	}

	m_mdLen = retLen;
	retLen = (maxLength > m_mdLen ? m_mdLen : maxLength);
	memcpy(hash, m_mdValue, retLen);

	return (unsigned int) retLen;

}

// Get information

XSECCryptoHash::HashType WinCAPICryptoHash::getHashType(void) const {

	return m_hashType;			// This could be any kind of hash

}

#endif /* XSEC_HAVE_WINCAPI */
