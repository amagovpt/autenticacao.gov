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
 * WinCAPICryptoKeyDSA := DSA Keys
 *
 * Author(s): Berin Lautenbach
 *
 * $Id: WinCAPICryptoKeyDSA.cpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#include <xsec/enc/WinCAPI/WinCAPICryptoKeyDSA.hpp>
#include <xsec/enc/WinCAPI/WinCAPICryptoProvider.hpp>
#include <xsec/enc/XSCrypt/XSCryptCryptoBase64.hpp>
#include <xsec/enc/XSECCryptoException.hpp>
#include <xsec/enc/XSECCryptoUtils.hpp>
#include <xsec/framework/XSECError.hpp>

#if defined (XSEC_HAVE_WINCAPI)

#include <xercesc/util/Janitor.hpp>

XSEC_USING_XERCES(ArrayJanitor);

WinCAPICryptoKeyDSA::WinCAPICryptoKeyDSA(HCRYPTPROV prov) {

	// Create a new key to be loaded as we go

	m_key = 0;
	m_p = prov;
	m_keySpec = 0;

	mp_P = NULL;
	mp_Q = NULL;
	mp_G = NULL;
	mp_Y = NULL;

};

// "Hidden" WinCAPI constructor

WinCAPICryptoKeyDSA::WinCAPICryptoKeyDSA(HCRYPTPROV prov, 
										 HCRYPTKEY k) :
m_p(prov) {

	m_key = k;		// NOTE - We OWN this handle
	m_keySpec = 0;

	mp_P = mp_Q = mp_G = mp_Y = NULL;
	m_PLen = m_QLen = m_GLen = m_YLen = 0;

}

WinCAPICryptoKeyDSA::WinCAPICryptoKeyDSA(HCRYPTPROV prov, 
										 DWORD keySpec,
										 bool isPrivate) :
m_p(prov) {

	if (isPrivate == false) {

		throw XSECCryptoException(XSECCryptoException::DSAError,
			"Public keys defined via keySpec ctor not yet supported");

		
	}
	
	m_key = 0;
	m_keySpec = keySpec;

	mp_P = mp_Q = mp_G = mp_Y = NULL;
	m_PLen = m_QLen = m_GLen = m_YLen = 0;

}

WinCAPICryptoKeyDSA::~WinCAPICryptoKeyDSA() {


	// If we have a DSA, delete it

	if (m_key != 0)
		CryptDestroyKey(m_key);

	if (mp_P != NULL)
		delete[] mp_P;
	if (mp_Q != NULL)
		delete[] mp_Q;
	if (mp_G != NULL)
		delete[] mp_G;
	if (mp_Y != NULL)
		delete[] mp_Y;

};

// Generic key functions

XSECCryptoKey::KeyType WinCAPICryptoKeyDSA::getKeyType() const {

	// Find out what we have
	if (m_key == NULL) {

		// For now we don't really understand Private Windows keys
		if (m_keySpec != 0)
			return KEY_DSA_PRIVATE;
		
		// Check if we have parameters loaded
		if (mp_P == NULL ||
			mp_Q == NULL ||
			mp_G == NULL ||
			mp_Y == NULL)
			return KEY_NONE;
		else
			return KEY_DSA_PUBLIC;
	}

	if (m_keySpec != 0)
		return KEY_DSA_PAIR;

	// If we have m_key - it must be public

	return KEY_DSA_PUBLIC;

}

void WinCAPICryptoKeyDSA::loadPBase64BigNums(const char * b64, unsigned int len) {

	if (mp_P != NULL) {
		delete[] mp_P;
		mp_P = NULL;		// In case we get an exception
	}

	mp_P = WinCAPICryptoProvider::b642WinBN(b64, len, m_PLen);

}

void WinCAPICryptoKeyDSA::loadQBase64BigNums(const char * b64, unsigned int len) {

	if (mp_Q != NULL) {
		delete[] mp_Q;
		mp_Q = NULL;		// In case we get an exception
	}

	mp_Q = WinCAPICryptoProvider::b642WinBN(b64, len, m_QLen);
}

void WinCAPICryptoKeyDSA::loadGBase64BigNums(const char * b64, unsigned int len) {

	if (mp_G != NULL) {
		delete[] mp_G;
		mp_G = NULL;		// In case we get an exception
	}

	mp_G = WinCAPICryptoProvider::b642WinBN(b64, len, m_GLen);
}

void WinCAPICryptoKeyDSA::loadYBase64BigNums(const char * b64, unsigned int len) {

	if (mp_Y != NULL) {
		delete[] mp_Y;
		mp_Y = NULL;		// In case we get an exception
	}

	mp_Y = WinCAPICryptoProvider::b642WinBN(b64, len, m_YLen);
}

void WinCAPICryptoKeyDSA::loadJBase64BigNums(const char * b64, unsigned int len) {
/*
	Do nothing
*/
}


// --------------------------------------------------------------------------------
//           Verify a signature encoded as a Base64 string
// --------------------------------------------------------------------------------

void WinCAPICryptoKeyDSA::importKey(void) {
	
	if (m_key != 0 ||
		mp_P == NULL ||
		mp_Q == NULL ||
		mp_G == NULL || 
		mp_Y == NULL)
		return;

	// Do some basic checks
	if ((m_QLen > 20) |
		(m_GLen > m_PLen) |
		(m_YLen > m_PLen)) {

			throw XSECCryptoException(XSECCryptoException::DSAError,
				"WinCAPI:DSA - Parameter lengths inconsistent");
	}

	// Create a DSS Public-Key blob

	// First build a buffer to hold everything
	BYTE * blobBuffer;
	unsigned int blobBufferLen = WINCAPI_BLOBHEADERLEN + WINCAPI_DSSPUBKEYLEN + (3 * m_PLen) + 0x14 + WINCAPI_DSSSEEDLEN;
	XSECnew(blobBuffer, BYTE[blobBufferLen]);
	ArrayJanitor<BYTE> j_blobBuffer(blobBuffer);

	// Blob header
	BLOBHEADER * header = (BLOBHEADER *) blobBuffer;

	header->bType = PUBLICKEYBLOB;
	header->bVersion = 0x02;			// We are using a version 2 blob
	header->reserved = 0;
	header->aiKeyAlg = CALG_DSS_SIGN;

	// Now the public key header
	DSSPUBKEY * pubkey = (DSSPUBKEY *) (blobBuffer + WINCAPI_BLOBHEADERLEN);

	pubkey->magic = 0x31535344;		// ASCII encoding of DSS1
	pubkey->bitlen = m_PLen * 8;		// Number of bits in prime modulus

	// Now copy in each of the keys
	BYTE * i = (BYTE *) (pubkey);
	i += WINCAPI_DSSPUBKEYLEN;

	memcpy(i, mp_P, m_PLen);
	i+= m_PLen;

	// Q
	memcpy(i, mp_Q, m_QLen);
	i += m_QLen;

	// Pad with 0s
	unsigned int j;
	for (j = m_QLen; j < 20 ; ++j)
		*i++ = 0;

	// Generator
	memcpy(i, mp_G, m_GLen);
	i += m_GLen;
	// Pad
	for (j = m_GLen; j < m_PLen ; ++j)
		*i++ = 0;

	// Public key
	memcpy(i, mp_Y, m_YLen);
	i += m_YLen;
	// Pad
	for (j = m_YLen; j < m_PLen ; ++j)
		*i++ = 0;

	// Set seed to 0
	for (j = 0; j < WINCAPI_DSSSEEDLEN; ++j)
		*i++ = 0xFF;	// SEED Counter set to 0xFFFFFFFF will cause seed to be ignored

	// Now that we have the blob, import
	BOOL fResult = CryptImportKey(
					m_p,
					blobBuffer,
					blobBufferLen,
					0,				// Not signed
					0,				// No flags
					&m_key);

	if (fResult == 0) {
		
		throw XSECCryptoException(XSECCryptoException::DSAError,
			"WinCAPI:DSA Error attempting to import key parameters");

	}

}

bool WinCAPICryptoKeyDSA::verifyBase64Signature(unsigned char * hashBuf, 
								 unsigned int hashLen,
								 char * base64Signature,
								 unsigned int sigLen) {

	// Use the currently loaded key to validate the Base64 encoded signature

	if (m_key == 0) {

		// Try to import from the parameters
		importKey();

		if (m_key == 0) {
			throw XSECCryptoException(XSECCryptoException::DSAError,
				"WinCAPI:DSA - Attempt to validate signature with empty key");
		}
	}

	// Decode the signature
	unsigned char * rawSig;
	DWORD rawSigLen;
	XSECnew(rawSig, BYTE [sigLen]);
	ArrayJanitor<BYTE> j_rawSig(rawSig);

	// Decode the signature
	XSCryptCryptoBase64 b64;

	b64.decodeInit();
	rawSigLen = b64.decode((unsigned char *) base64Signature, sigLen, rawSig, sigLen);
	rawSigLen += b64.decodeFinish(&rawSig[rawSigLen], sigLen - rawSigLen);

	// Reverse the sig - Windows stores integers as octet streams in little endian
	// order.  The I2OSP algorithm used by XMLDSig to store integers is big endian

	BYTE rawSigFinal[40];
	BYTE * j, *k, *l, *m;

	unsigned char rb[20];
	unsigned char sb[20];

	if (rawSigLen == 40) {

		j = rawSig;
		k = rawSig + 20;

	} else if (rawSigLen == 46 && ASN2DSASig(rawSig, rb, sb) == true) {

		j = rb;
		k = sb;

	} else {

		throw XSECCryptoException(XSECCryptoException::DSAError,
			"WinCAPI:DSA::VerifyBase64Signature - Expect 40 bytes in a DSA signature");
	}

	l = rawSigFinal + 19;
	m = rawSigFinal + 39;
	
	while (l >= rawSigFinal) {
		*l-- = *j++;
		*m-- = *k++;
	}
	
	// Have to create a Windows hash object and feed in the hash
	BOOL fResult;
	HCRYPTHASH h;
	fResult = CryptCreateHash(m_p, 
					CALG_SHA1, 
					0, 
					0,
					&h);

	if (!fResult) {
		throw XSECCryptoException(XSECCryptoException::DSAError,
			"WinCAPI:DSA - Error creating Windows Hash Object");
	}

	// Feed the hash value into the newly created hash object
	fResult = CryptSetHashParam(
					h, 
					HP_HASHVAL, 
					hashBuf, 
					0);

	if (!fResult) {
		if (h)
			CryptDestroyHash(h);
		throw XSECCryptoException(XSECCryptoException::DSAError,
			"WinCAPI:DSA - Error Setting Hash Value in Windows Hash object");
	}

	// Now validate
	fResult = CryptVerifySignature(
				h,
				rawSigFinal,
				40,
				m_key,
				NULL,
				0);

	if (!fResult) {

		DWORD error = GetLastError();

		/* For some reason, the default Microsoft DSS provider generally returns
		 * NTE_FAIL (which denotes an internal failure in the provider) for a 
		 * failed signature rather than NTE_BAD_SIGNATURE
		 */

		if (error != NTE_BAD_SIGNATURE && error != NTE_FAIL) {
			if (h)
				CryptDestroyHash(h);
			throw XSECCryptoException(XSECCryptoException::DSAError,
			"WinCAPI:DSA - Error occurred in DSA validation");
		}

		if (h)
			CryptDestroyHash(h);
		return false;
	}

	if (h)
		CryptDestroyHash(h);

	return true;

}

// --------------------------------------------------------------------------------
//           Sign and encode result as a Base64 string
// --------------------------------------------------------------------------------


unsigned int WinCAPICryptoKeyDSA::signBase64Signature(unsigned char * hashBuf,
		unsigned int hashLen,
		char * base64SignatureBuf,
		unsigned int base64SignatureBufLen) {

	// Sign a pre-calculated hash using this key

	if (m_keySpec == 0) {

		throw XSECCryptoException(XSECCryptoException::DSAError,
			"WinCAPI:DSA - Attempt to sign data a public or non-existent key");
	}

	// Have to create a Windows hash object and feed in the hash
	BOOL fResult;
	HCRYPTHASH h;
	fResult = CryptCreateHash(m_p, 
					CALG_SHA1, 
					0, 
					0,
					&h);

	if (!fResult) {
		throw XSECCryptoException(XSECCryptoException::DSAError,
			"WinCAPI:DSA - Error creating Windows Hash Object");
	}

	// Feed the hash value into the newly created hash object
	fResult = CryptSetHashParam(
					h, 
					HP_HASHVAL, 
					hashBuf, 
					0);

	if (!fResult) {
		if (h)
			CryptDestroyHash(h);
		throw XSECCryptoException(XSECCryptoException::DSAError,
			"WinCAPI:DSA - Error Setting Hash Value in Windows Hash object");
	}

	// Now sign
	BYTE rawSig[50];
	DWORD rawSigLen = 50;
	fResult = CryptSignHash(
				h,
				m_keySpec,
				NULL,
				0,
				rawSig,
				&rawSigLen);

	if (!fResult || rawSigLen != 40) {

		if (h)
			CryptDestroyHash(h);
		throw XSECCryptoException(XSECCryptoException::DSAError,
		"WinCAPI:DSA - Error occurred in DSA signing");
	}

	if (h)
		CryptDestroyHash(h);
	// Now encode into a signature block
	BYTE rawSigFinal[40];

	BYTE * i, * j, * m, * n;

	i = rawSig;
	j = rawSig + 20;
	m = rawSigFinal + 19;
	n = rawSigFinal + 39;

	while (m >= rawSigFinal) {
		*m-- = *i++;
		*n-- = *j++;
	}

	// Now encode
	XSCryptCryptoBase64 b64;
	b64.encodeInit();
	unsigned int ret = b64.encode(rawSigFinal, 40, (unsigned char *) base64SignatureBuf, base64SignatureBufLen);
	ret += b64.encodeFinish((unsigned char *) &base64SignatureBuf[ret], base64SignatureBufLen - ret);

	return ret;

}

// --------------------------------------------------------------------------------
//           Clone key
// --------------------------------------------------------------------------------


XSECCryptoKey * WinCAPICryptoKeyDSA::clone() const {

	WinCAPICryptoKeyDSA * ret;

	XSECnew(ret, WinCAPICryptoKeyDSA(m_p));
	
	if (m_key != 0) {

		// CryptDuplicateKey is not supported in Windows NT, so we need to export and then
		// reimport the key to get a copy

		BYTE keyBuf[2048];
		DWORD keyBufLen = 2048;
		CryptExportKey(m_key, 0, PUBLICKEYBLOB, 0, keyBuf, &keyBufLen);

		// Now re-import
		CryptImportKey(m_p, keyBuf, keyBufLen, NULL, 0, &ret->m_key);
	}

	ret->m_PLen = m_PLen;
	if (mp_P != NULL) {
		XSECnew(ret->mp_P, BYTE[m_PLen]);
		memcpy(ret->mp_P, mp_P, m_PLen);
	}
	else
		ret->mp_P = NULL;

	ret->m_QLen = m_QLen;
	if (mp_Q != NULL) {
		XSECnew(ret->mp_Q, BYTE[m_QLen]);
		memcpy(ret->mp_Q, mp_Q, m_QLen);
	}
	else
		ret->mp_Q = NULL;

	ret->m_GLen = m_GLen;
	if (mp_G != NULL) {
		XSECnew(ret->mp_G, BYTE[m_GLen]);
		memcpy(ret->mp_G, mp_G, m_GLen);
	}
	else
		ret->mp_G = NULL;

	ret->m_YLen = m_YLen;
	if (mp_Y != NULL) {
		XSECnew(ret->mp_Y, BYTE[m_YLen]);
		memcpy(ret->mp_Y, mp_Y, m_YLen);
	}
	else
		ret->mp_Y = NULL;



	return ret;

}

// --------------------------------------------------------------------------------
//           Some utility functions
// --------------------------------------------------------------------------------

void WinCAPICryptoKeyDSA::loadParamsFromKey(void) {

	if (m_key == 0) {

		if (m_keySpec == 0)
			return;

		// See of we can get the user key
		if (!CryptGetUserKey(m_p, m_keySpec, &m_key))
			return;

	}

	// Export key into a keyblob
	BOOL fResult;
	DWORD blobLen;

	fResult = CryptExportKey(
		m_key,
		0,
		PUBLICKEYBLOB,
		0,
		NULL,
		&blobLen);
	
	if (fResult == 0 || blobLen < 1) {
		throw XSECCryptoException(XSECCryptoException::DSAError,
			"WinCAPI:DSA - Error exporting public key");
	}

	BYTE * blob;
	XSECnew(blob, BYTE[blobLen]);
	ArrayJanitor<BYTE> j_blob(blob);

	fResult = CryptExportKey(
		m_key,
		0,
		PUBLICKEYBLOB,
		0,
		blob,
		&blobLen);
	
	if (fResult == 0 || blobLen < 1) {
		throw XSECCryptoException(XSECCryptoException::DSAError,
			"WinCAPI:DSA - Error exporting public key");
	}

	DSSPUBKEY * pk = (DSSPUBKEY *) ( blob + WINCAPI_BLOBHEADERLEN );
	DWORD keyLen = pk->bitlen / 8;

	// Copy the keys
	
	BYTE * i = (BYTE *) ( pk );
	i += WINCAPI_DSSPUBKEYLEN;
	if (mp_P != NULL)
		delete[] mp_P;

	XSECnew(mp_P, BYTE[keyLen]);
	memcpy(mp_P, i, keyLen);
	m_PLen = keyLen;

	i+=keyLen;

	if (mp_Q != NULL)
		delete[] mp_Q;

	m_QLen = 20;
	while (i[m_QLen - 1] == 0 && m_QLen > 0)
		m_QLen--;
	XSECnew(mp_Q, BYTE[m_QLen]);
	memcpy(mp_Q, i, m_QLen);
	
	i+=20;

	if (mp_G != NULL)
		delete[] mp_G;
	
	m_GLen = keyLen;
	while(i[m_GLen - 1] == 0 && m_GLen > 0)
		m_GLen--;

	XSECnew(mp_G, BYTE[m_GLen]);
	memcpy(mp_G, i, m_GLen);

	i+=keyLen;

	if (mp_Y != NULL)
		delete[] mp_Y;

	m_YLen = keyLen;
	while (i[m_YLen] == 0 && m_YLen > 0)
		m_YLen--;

	XSECnew(mp_Y, BYTE[m_YLen]);
	memcpy(mp_Y, i, m_YLen);

}

unsigned int WinCAPICryptoKeyDSA::getPBase64BigNums(char * b64, unsigned int len) {

	if (m_key == 0 && m_keySpec == 0 && mp_P == NULL) {

		return 0;	// Nothing we can do

	}

	if (mp_P == NULL) {

		loadParamsFromKey();

	}

	unsigned int bLen;
	unsigned char * b =  WinCAPICryptoProvider::WinBN2b64(mp_P, m_PLen, bLen);
	if (bLen > len)
		bLen = len;
	memcpy(b64, b, bLen);
	delete[] b;

	return bLen;

}

unsigned int WinCAPICryptoKeyDSA::getQBase64BigNums(char * b64, unsigned int len) {

	if (m_key == 0 && m_keySpec == 0 && mp_Q == NULL) {

		return 0;	// Nothing we can do

	}

	if (mp_Q == NULL) {

		loadParamsFromKey();

	}

	unsigned int bLen;
	unsigned char * b =  WinCAPICryptoProvider::WinBN2b64(mp_Q, m_QLen, bLen);
	if (bLen > len)
		bLen = len;
	memcpy(b64, b, bLen);
	delete[] b;

	return bLen;

}

unsigned int WinCAPICryptoKeyDSA::getGBase64BigNums(char * b64, unsigned int len) {

	if (m_key == 0 && m_keySpec == 0 && mp_G == NULL) {

		return 0;	// Nothing we can do

	}

	if (mp_G == NULL) {

		loadParamsFromKey();

	}

	unsigned int bLen;
	unsigned char * b =  WinCAPICryptoProvider::WinBN2b64(mp_G, m_GLen, bLen);
	if (bLen > len)
		bLen = len;
	memcpy(b64, b, bLen);
	delete[] b;

	return bLen;

}

unsigned int WinCAPICryptoKeyDSA::getYBase64BigNums(char * b64, unsigned int len) {

	if (m_key == 0 && m_keySpec == 0 && mp_Y == NULL) {

		return 0;	// Nothing we can do

	}

	if (mp_Y == NULL) {

		loadParamsFromKey();

	}

	unsigned int bLen;
	unsigned char * b =  WinCAPICryptoProvider::WinBN2b64(mp_Y, m_YLen, bLen);
	if (bLen > len)
		bLen = len;
	memcpy(b64, b, bLen);
	delete[] b;

	return bLen;

}

#endif /* XSEC_HAVE_WINCAPI */

