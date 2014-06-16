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
 * OpenSSLCryptoHashSHA1 := OpenSSL Implementation of SHA1
 *
 * Author(s): Berin Lautenbach
 *
 * $Id: OpenSSLCryptoHash.cpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#include <xsec/framework/XSECDefs.hpp>
#if defined (XSEC_HAVE_OPENSSL)

#include <xsec/enc/OpenSSL/OpenSSLCryptoHash.hpp>
#include <xsec/enc/XSECCryptoException.hpp>

#include <memory.h>

// Constructors/Destructors

OpenSSLCryptoHash::OpenSSLCryptoHash(HashType alg) {

	switch (alg) {

	case (XSECCryptoHash::HASH_SHA1) :
	
		mp_md = EVP_get_digestbyname("SHA1");
		break;

	case (XSECCryptoHash::HASH_MD5) :
	
		mp_md = EVP_get_digestbyname("MD5");
		break;

	case (XSECCryptoHash::HASH_SHA224) :
	
		mp_md = EVP_get_digestbyname("SHA224");
		if (mp_md == NULL) {
			throw XSECCryptoException(XSECCryptoException::MDError,
			"OpenSSL:Hash - SHA224 not supported by this version of OpenSSL"); 
		}

		break;

	case (XSECCryptoHash::HASH_SHA256) :
	
		mp_md = EVP_get_digestbyname("SHA256");
		if (mp_md == NULL) {
			throw XSECCryptoException(XSECCryptoException::MDError,
			"OpenSSL:Hash - SHA256 not supported by this version of OpenSSL"); 
		}

		break;

	case (XSECCryptoHash::HASH_SHA384) :
	
		mp_md = EVP_get_digestbyname("SHA384");
		if (mp_md == NULL) {
			throw XSECCryptoException(XSECCryptoException::MDError,
			"OpenSSL:Hash - SHA384 not supported by this version of OpenSSL"); 
		}

		break;

	case (XSECCryptoHash::HASH_SHA512) :
	
		mp_md = EVP_get_digestbyname("SHA512");
		if (mp_md == NULL) {
			throw XSECCryptoException(XSECCryptoException::MDError,
			"OpenSSL:Hash - SHA512 not supported by this version of OpenSSL"); 
		}

		break;

	default :

		mp_md = NULL;

	}

	if(!mp_md) {

		throw XSECCryptoException(XSECCryptoException::MDError,
			"OpenSSL:Hash - Error loading Message Digest"); 
	}

	EVP_DigestInit(&m_mdctx, mp_md);
	m_hashType = alg;

}


OpenSSLCryptoHash::~OpenSSLCryptoHash() {

	EVP_MD_CTX_cleanup(&m_mdctx);

}



// Hashing Activities
void OpenSSLCryptoHash::reset(void) {

	EVP_MD_CTX_cleanup(&m_mdctx);

	EVP_DigestInit(&m_mdctx, mp_md);

}

void OpenSSLCryptoHash::hash(unsigned char * data, 
								 unsigned int length) {

	EVP_DigestUpdate(&m_mdctx, data, length);

}
unsigned int OpenSSLCryptoHash::finish(unsigned char * hash,
									   unsigned int maxLength) {

	unsigned int retLen;

	// Finish up and copy out hash, returning the length

	EVP_DigestFinal(&m_mdctx, m_mdValue, &m_mdLen);

	// Copy to output buffer
	
	retLen = (maxLength > m_mdLen ? m_mdLen : maxLength);
	memcpy(hash, m_mdValue, retLen);

	return retLen;

}

// Get information

XSECCryptoHash::HashType OpenSSLCryptoHash::getHashType(void) const {

	return m_hashType;			// This could be any kind of hash

}

#endif /* XSEC_HAVE_OPENSSL */
