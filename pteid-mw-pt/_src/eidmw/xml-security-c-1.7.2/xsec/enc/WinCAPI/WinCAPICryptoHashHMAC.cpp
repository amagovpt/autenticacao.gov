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
 * WinCAPICryptoHashHMAC := Windows CAPI Implementation of Message digests
 *
 * Author(s): Berin Lautenbach
 *
 * $Id: WinCAPICryptoHashHMAC.cpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#include <xsec/enc/WinCAPI/WinCAPICryptoHashHMAC.hpp>
#include <xsec/enc/XSECCryptoException.hpp>
#include <xsec/enc/WinCAPI/WinCAPICryptoProvider.hpp>
#include <xsec/enc/WinCAPI/WinCAPICryptoKeyHMAC.hpp>
#include <xsec/utils/XSECDOMUtils.hpp>

#if defined (XSEC_HAVE_WINCAPI)

#include <memory.h>

// --------------------------------------------------------------------------------
//           IPAD/OPAD definitions
// --------------------------------------------------------------------------------

static unsigned char ipad[] = {

	0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36,
	0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36,
	0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36,
	0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36,
	0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36,
	0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36,
	0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36,
	0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36,
};

static unsigned char opad[] = {

	0x5C, 0x5C, 0x5C, 0x5C, 0x5C, 0x5C, 0x5C, 0x5C, 
	0x5C, 0x5C, 0x5C, 0x5C, 0x5C, 0x5C, 0x5C, 0x5C, 
	0x5C, 0x5C, 0x5C, 0x5C, 0x5C, 0x5C, 0x5C, 0x5C, 
	0x5C, 0x5C, 0x5C, 0x5C, 0x5C, 0x5C, 0x5C, 0x5C, 
	0x5C, 0x5C, 0x5C, 0x5C, 0x5C, 0x5C, 0x5C, 0x5C, 
	0x5C, 0x5C, 0x5C, 0x5C, 0x5C, 0x5C, 0x5C, 0x5C, 
	0x5C, 0x5C, 0x5C, 0x5C, 0x5C, 0x5C, 0x5C, 0x5C, 
	0x5C, 0x5C, 0x5C, 0x5C, 0x5C, 0x5C, 0x5C, 0x5C, 
};

// --------------------------------------------------------------------------------
//           Constructors/Destructors
// --------------------------------------------------------------------------------

WinCAPICryptoHashHMAC::WinCAPICryptoHashHMAC(HCRYPTPROV prov, HashType alg) {

	m_p = prov;
	m_h = 0;
	m_blockSize = 64;		// We only know SHA-1 and MD5 at this time - both are 64 bytes

	switch (alg) {

	case (XSECCryptoHash::HASH_SHA1) :
	
		m_algId = CALG_SHA;
		break;

	case (XSECCryptoHash::HASH_MD5) :
	
		m_algId = CALG_MD5;
		break;

	default :

		m_algId = 0;

	}

	if(m_algId == 0) {

		throw XSECCryptoException(XSECCryptoException::MDError,
			"WinCAPI:Hash - Unknown algorithm"); 
	}

	m_hashType = alg;


}

void WinCAPICryptoHashHMAC::reset() {

	if (m_h != 0)
		CryptDestroyHash(m_h);

}

WinCAPICryptoHashHMAC::~WinCAPICryptoHashHMAC() {

	if (m_h != 0)
		CryptDestroyHash(m_h);

}

// --------------------------------------------------------------------------------
//           Key manipulation
// --------------------------------------------------------------------------------

void WinCAPICryptoHashHMAC::eraseKeys(void) {

	// Overwrite the ipad/opad calculated key values
	unsigned char * i = m_ipadKeyed;
	unsigned char * j = m_opadKeyed;

	for (unsigned int k = 0; k < XSEC_MAX_HASH_BLOCK_SIZE; ++k) {
		*i++ = 0;
		*j++ = 0;
	}

}

void WinCAPICryptoHashHMAC::setKey(XSECCryptoKey *key) {

	
	BOOL fResult;

	// Use this to initialise the ipadKeyed/opadKeyed values

	if (key->getKeyType() != XSECCryptoKey::KEY_HMAC) {

		throw XSECCryptoException(XSECCryptoException::MDError,
			"WinCAPI:HashHMAC - Non HMAC Key passed to HashHMAC");

	}

	if (m_blockSize > XSEC_MAX_HASH_BLOCK_SIZE) {

		throw XSECCryptoException(XSECCryptoException::MDError,
			"WinCAPI:HashHMAC - Internal error - have got a blocksize bigger than I can handle");

	}

	// Check to see if this is an internal Windows Key
	if (strEquals(key->getProviderName(), DSIGConstants::s_unicodeStrPROVWinCAPI) &&
		((WinCAPICryptoKeyHMAC *) key)->getWinKey() != 0) {

		// Over-ride the local provider for this 

		HCRYPTPROV p = ((WinCAPICryptoKeyHMAC *) key)->getWinKeyProv();
		HCRYPTKEY k = ((WinCAPICryptoKeyHMAC *) key)->getWinKey();

		fResult = CryptCreateHash(
			p,
			CALG_HMAC,
			k,
			0,
			&m_h);

		if (fResult == 0 || m_h == 0) {
			DWORD error = GetLastError();
			throw XSECCryptoException(XSECCryptoException::MDError,
				"WinCAPI:Hash::setKey - Error creating internally keyed hash object"); 
		}

		// Set the HMAC algorithm
		HMAC_INFO hi;

		hi.HashAlgid = m_algId;
		hi.pbInnerString = NULL;		// Use default inner and outer strings
		hi.cbInnerString = 0;
		hi.pbOuterString = NULL;
		hi.cbOuterString = 0;

		fResult = CryptSetHashParam(
			m_h,
			HP_HMAC_INFO,
			(BYTE *) &hi,
			0);

		if (fResult == 0 || m_h == 0) {
			DWORD error = GetLastError();
			throw XSECCryptoException(XSECCryptoException::MDError,
				"WinCAPI:Hash::setKey - Error setting HASH_INFO object"); 
		}



		return;

	}

	// Need to load from raw bit string

	safeBuffer keyBuf;
	unsigned int keyLen = ((XSECCryptoKeyHMAC *) key)->getKey(keyBuf);
	
	if (keyLen > m_blockSize) {

		HCRYPTHASH h;

		fResult = CryptCreateHash(
			m_p,
			m_algId,
			0,
			0,
			&h);

		if (fResult == 0 || h == 0) {
			throw XSECCryptoException(XSECCryptoException::MDError,
				"WinCAPI:Hash::setKey - Error creating hash object"); 
		}

		fResult = CryptHashData(
			h,
			keyBuf.rawBuffer(),
			keyLen,
			0);

		if (fResult == 0 || h == 0) {
			if (h)
				CryptDestroyHash(h);
			throw XSECCryptoException(XSECCryptoException::MDError,
				"WinCAPI:Hash::setKey - Error hashing key data"); 
		}

		BYTE outData[XSEC_MAX_HASH_SIZE];
		DWORD outDataLen = XSEC_MAX_HASH_SIZE;

		CryptGetHashParam(
			h,
			HP_HASHVAL,
			outData,
			&outDataLen,
			0);

		if (fResult == 0 || h == 0) {
			if (h)
				CryptDestroyHash(h);
			throw XSECCryptoException(XSECCryptoException::MDError,
				"WinCAPI:Hash::setKey - Error getting hash result"); 
		}

		keyBuf.sbMemcpyIn(outData, outDataLen);
		keyLen = outDataLen;

		if (h)
			CryptDestroyHash(h);


	}

	// Now create the ipad and opad keyed values
	memcpy(m_ipadKeyed, ipad, m_blockSize);
	memcpy(m_opadKeyed, opad, m_blockSize);

	// XOR with the key
	for (unsigned int i = 0; i < keyLen; ++i) {
		m_ipadKeyed[i] = keyBuf[i] ^ m_ipadKeyed[i];
		m_opadKeyed[i] = keyBuf[i] ^ m_opadKeyed[i];
	}


	// Now create the hash object, and start with the ipad operation
	fResult = CryptCreateHash(
		m_p,
		m_algId,
		0,
		0,
		&m_h);

	if (fResult == 0 || m_h == 0) {
		throw XSECCryptoException(XSECCryptoException::MDError,
			"WinCAPI:HashHMAC - Error creating hash object"); 
	}

	fResult = CryptHashData(
		m_h,
		m_ipadKeyed,
		m_blockSize,
		0);

	if (fResult == 0 || m_h == 0) {
		throw XSECCryptoException(XSECCryptoException::MDError,
			"WinCAPI:HashHMAC - Error performing initial ipad digest"); 
	}

}

// --------------------------------------------------------------------------------
//           Hash operations
// --------------------------------------------------------------------------------

void WinCAPICryptoHashHMAC::hash(unsigned char * data, 
								 unsigned int length) {

	if (m_h == 0) {
		throw XSECCryptoException(XSECCryptoException::MDError,
			"WinCAPI:HashHMAC::hash() - Called prior to setting key"); 
	}

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

unsigned int WinCAPICryptoHashHMAC::finish(unsigned char * hash,
									   unsigned int maxLength) {

	DWORD retLen;
	BOOL fResult;

	retLen = XSEC_MAX_HASH_SIZE;

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

	// Perform the opad operation
	HCRYPTHASH h;
	fResult = CryptCreateHash(
		m_p,
		m_algId,
		0,
		0,
		&h);

	if (fResult == 0 || h == 0) {
		throw XSECCryptoException(XSECCryptoException::MDError,
			"WinCAPI:Hash::finish - Error creating hash object for opad operation"); 
	}

	fResult = CryptHashData(
		h,
		m_opadKeyed,
		m_blockSize,
		0);

	if (fResult == 0 || h == 0) {
		if (h)
			CryptDestroyHash(h);
		throw XSECCryptoException(XSECCryptoException::MDError,
			"WinCAPI:Hash::finish - Error hashing opad data"); 
	}

	fResult = CryptHashData(
		h,
		m_mdValue,
		retLen,
		0);

	if (fResult == 0 || h == 0) {
		if (h)
			CryptDestroyHash(h);
		throw XSECCryptoException(XSECCryptoException::MDError,
			"WinCAPI:Hash::finish - Error hashing ipad hash to opad"); 
	}

	// Read out the final hash
	retLen = XSEC_MAX_HASH_SIZE;

	fResult = CryptGetHashParam(
		h,
		HP_HASHVAL,
		m_mdValue,
		&retLen,
		0);

	CryptDestroyHash(h);

	m_mdLen = retLen;
	retLen = (maxLength > m_mdLen ? m_mdLen : maxLength);
	memcpy(hash, m_mdValue, retLen);

	return (unsigned int) retLen;

}

// Get information

XSECCryptoHash::HashType WinCAPICryptoHashHMAC::getHashType(void) const {

	return m_hashType;			// This could be any kind of hash

}

#endif /* XSEC_HAVE_WINCAPI */
