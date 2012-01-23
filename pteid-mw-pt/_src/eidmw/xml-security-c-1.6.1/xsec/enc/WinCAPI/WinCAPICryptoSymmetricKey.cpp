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
 * XSECCryptoSymmetricKey := Bulk encryption algorithms should all be
 *							implemented via this interface
 *
 * $Id: WinCAPICryptoSymmetricKey.cpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/utils/XSECPlatformUtils.hpp>
#include <xsec/utils/XSECDOMUtils.hpp>
#include <xsec/enc/WinCAPI/WinCAPICryptoProvider.hpp>
#include <xsec/enc/WinCAPI/WinCAPICryptoSymmetricKey.hpp>
#include <xsec/framework/XSECError.hpp>
#include <xsec/enc/XSECCryptoException.hpp>

#include <xercesc/util/Janitor.hpp>

XERCES_CPP_NAMESPACE_USE

#if defined (XSEC_HAVE_WINCAPI)

// --------------------------------------------------------------------------------
//           Constructors and Destructors
// --------------------------------------------------------------------------------

WinCAPICryptoSymmetricKey::WinCAPICryptoSymmetricKey(
						HCRYPTPROV prov,
						XSECCryptoSymmetricKey::SymmetricKeyType type) :
m_keyType(type),
m_keyMode(MODE_ECB),
m_initialised(false),
m_doPad(true),
m_p(prov),
m_k(0) {


}

WinCAPICryptoSymmetricKey::~WinCAPICryptoSymmetricKey() {

	if (m_k != 0)
		CryptDestroyKey(m_k);

}

// --------------------------------------------------------------------------------
//           Basic Key interface methods
// --------------------------------------------------------------------------------

XSECCryptoKey * WinCAPICryptoSymmetricKey::clone() const {

	WinCAPICryptoSymmetricKey * ret;

	XSECnew(ret, WinCAPICryptoSymmetricKey(m_p, m_keyType));
	ret->m_keyLen = m_keyLen;
	ret->m_keyBuf = m_keyBuf;

	if (m_k != 0) {

#if (_WIN32_WINNT > 0x0400)

		// Only supported in Win2K and above

		if (CryptDuplicateKey(m_k,
			 				  0,
							  0,
							  &(ret->m_k)) == 0 ) {

			throw XSECCryptoException(XSECCryptoException::SymmetricError,
				"WinCAPI:SymmetricKey Error attempting to clone key parameters");

		}
#else

		ret->setKey(m_keyBuf.rawBuffer(), m_keyLen);

#endif
	}
	else
		ret->m_k = 0;

	return ret;

}

// --------------------------------------------------------------------------------
//           Store the key value
// --------------------------------------------------------------------------------

void WinCAPICryptoSymmetricKey::setKey(const unsigned char * key, unsigned int keyLen) {

	m_keyBuf.sbMemcpyIn(key, keyLen);
	m_keyLen = keyLen;

	if (m_k != 0)
		CryptDestroyKey(m_k);
	
	m_p = 0;
	m_k = createWindowsKey(key, keyLen, m_keyType, &m_p);
}

// --------------------------------------------------------------------------------
//           Decrypt
// --------------------------------------------------------------------------------

int WinCAPICryptoSymmetricKey::decryptCtxInit(const unsigned char * iv) {

	// Returns amount of IV data used (in bytes)
	// Sets m_initialised iff the key is OK and the IV is OK.

	if (m_initialised)
		return 0;

	if (m_k == 0) {

		throw XSECCryptoException(XSECCryptoException::SymmetricError,
			"WinCAPI:SymmetricKey - Cannot initialise without key"); 

	}

	DWORD cryptMode;
	if (m_keyMode == MODE_CBC) {

		if (iv == NULL) {

			return 0;	// Cannot initialise without an IV

		}
		
		cryptMode = CRYPT_MODE_CBC;

		if (!CryptSetKeyParam(m_k, KP_MODE, (BYTE *) (&cryptMode), 0)) {

			throw XSECCryptoException(XSECCryptoException::SymmetricError,
				"WinCAPI:SymmetricKey - Error setting cipher mode"); 

		}
		
		if (!CryptSetKeyParam(m_k, KP_IV, (unsigned char *) iv, 0)) {

			throw XSECCryptoException(XSECCryptoException::SymmetricError,
			"WinCAPI:SymmetricKey - Error setting IV"); 

		}

	}
	else {
		cryptMode = CRYPT_MODE_ECB;

		if (!CryptSetKeyParam(m_k, KP_MODE, (BYTE *) (&cryptMode), 0)) {

			throw XSECCryptoException(XSECCryptoException::SymmetricError,
				"WinCAPI:SymmetricKey - Error setting cipher mode"); 

		}
	}

	// Set up the context according to the required cipher type
	switch (m_keyType) {

	case (XSECCryptoSymmetricKey::KEY_3DES_192) :

		// A 3DES CBC key

		m_blockSize = 8;
		m_bytesInLastBlock = 0;
		m_initialised = true;
		
		break;

	case (XSECCryptoSymmetricKey::KEY_AES_128) :
	case (XSECCryptoSymmetricKey::KEY_AES_192) :
	case (XSECCryptoSymmetricKey::KEY_AES_256) :

		// An AES key

		m_blockSize = 16;
		m_bytesInLastBlock = 0;
		m_initialised = true;
		
		break;

	default :

		// Cannot do this without an IV
		throw XSECCryptoException(XSECCryptoException::SymmetricError,
			"WinCAPI:SymmetricKey - Unknown key type"); 

	}

	return (m_keyMode == MODE_CBC ? m_blockSize : 0);
}


bool WinCAPICryptoSymmetricKey::decryptInit(bool doPad, 
											SymmetricKeyMode mode, 
											const unsigned char * iv) {

	m_initialised = false;
	m_doPad = doPad;
	m_keyMode = mode;
	decryptCtxInit(iv);
	return true;

}

unsigned int WinCAPICryptoSymmetricKey::decrypt(const unsigned char * inBuf, 
								 unsigned char * plainBuf, 
								 unsigned int inLength,
								 unsigned int maxOutLength) {



	// NOTE: This won't actually stop WinCAPI blowing the buffer, so the onus is
	// on the caller.

	unsigned int offset = 0;
	if (!m_initialised) {
		offset = decryptCtxInit(inBuf);
		if (offset > inLength) {
			throw XSECCryptoException(XSECCryptoException::SymmetricError,
			"WinCAPI:SymmetricKey - Not enough data passed in to get IV");
		}
	}

	/* As per bug #38365 - we need to ensure there are enough characters to decrypt.
	   Otherwise we get some nasty errors due to rounding calculations when inputs
	   are too small */

	else if (m_bytesInLastBlock + inLength < m_blockSize) {
		// Not enough input data
		memcpy(&m_lastBlock[m_bytesInLastBlock], inBuf, inLength);
		m_bytesInLastBlock += inLength;
		return 0;
	}

	DWORD outl = inLength - offset;

	// Copy in last block
	if (m_bytesInLastBlock > 0)
		memcpy(plainBuf, m_lastBlock, m_bytesInLastBlock);

	// Copy out the tail, as we _MUST_ know when we come to the end for decryptFinal
	unsigned int rounding = (outl % m_blockSize) + m_blockSize;
	memcpy(&plainBuf[m_bytesInLastBlock], &inBuf[offset], outl - rounding);

	// Copy the tail to m_lastBlock
	memcpy(m_lastBlock, &inBuf[offset + outl - rounding], rounding);
	outl = outl - rounding + m_bytesInLastBlock;
	m_bytesInLastBlock = rounding;
	
	if (!CryptDecrypt(m_k,
				 0,
				 FALSE,
				 0,
				 plainBuf,
				 &outl)) {

		throw XSECCryptoException(XSECCryptoException::SymmetricError,
			"WinCAPI:SymmetricKey - Error during WinCAPI decrypt"); 

	}

	return outl;

}

unsigned int WinCAPICryptoSymmetricKey::decryptFinish(unsigned char * plainBuf,
													  unsigned int maxOutLength) {

	DWORD outl = m_bytesInLastBlock;

	memcpy(plainBuf, m_lastBlock, outl);

	if (!CryptDecrypt(m_k, 
					  0, 
					  FALSE,
					  0,
					  plainBuf,
					  &outl)) {

		throw XSECCryptoException(XSECCryptoException::SymmetricError,
			"WinCAPI:SymmetricKey - Error during WinCAPI decrypt finalisation"); 

	}

	if (m_doPad) {

		// Need to do this ourselves, as WinCAPI appears broken
		if (plainBuf[outl - 1] > m_blockSize) {
			throw XSECCryptoException(XSECCryptoException::SymmetricError,
				"WinCAPI:SymmetricKey - Bad padding"); 
		}

		outl -= plainBuf[outl - 1];

	}

	// This is just to reset the key, and does nothing else useful.
	DWORD tmpout = maxOutLength - outl;
	CryptDecrypt(m_k, 0, TRUE, 0, &plainBuf[outl], &tmpout);

	return outl;
}

// --------------------------------------------------------------------------------
//           Encrypt
// --------------------------------------------------------------------------------

void WinCAPICryptoSymmetricKey::encryptCtxInit(const unsigned char * iv) {



	if (m_initialised == true)
		return;
	
	if (m_keyLen == 0) {

		throw XSECCryptoException(XSECCryptoException::SymmetricError,
			"WinCAPI:SymmetricKey - Cannot initialise without key"); 

	}

	m_initialised = true;

	// Set up the context according to the required cipher type

	const unsigned char * usedIV = NULL;
	unsigned char genIV[256];
	DWORD cryptMode;

	// Tell the library that the IV still has to be sent

	if (m_keyMode == MODE_CBC) {

		if (iv == NULL) {
			
			BOOL res = CryptGenRandom(m_p, 256, genIV);
			if (res == FALSE) {
				throw XSECCryptoException(XSECCryptoException::SymmetricError,
					"WinCAPI:SymmetricKey - Error generating random IV");
			}

			usedIV = genIV;
			//return 0;	// Cannot initialise without an IV

		}
		else
			usedIV = iv;

		cryptMode = CRYPT_MODE_CBC;

		if (!CryptSetKeyParam(m_k, KP_MODE, (BYTE *) (&cryptMode), 0)) {

			throw XSECCryptoException(XSECCryptoException::SymmetricError,
				"WinCAPI:SymmetricKey - Error setting cipher mode"); 

		}

		// Set the IV parameter
		if (!CryptSetKeyParam(m_k, KP_IV, (unsigned char *) usedIV, 0)) {

			throw XSECCryptoException(XSECCryptoException::SymmetricError,
			"WinCAPI:SymmetricKey - Error setting IV"); 

		}

	}
	else {

		cryptMode = CRYPT_MODE_ECB;

		if (!CryptSetKeyParam(m_k, KP_MODE, (BYTE *) (&cryptMode), 0)) {

			throw XSECCryptoException(XSECCryptoException::SymmetricError,
				"WinCAPI:SymmetricKey - Error setting cipher mode"); 

		}
	}

	switch (m_keyType) {

	case (XSECCryptoSymmetricKey::KEY_3DES_192) :

		// A 3DES key

		m_blockSize = 8;
		if (m_keyMode == MODE_CBC)
			m_ivSize = 8;
		else 
			m_ivSize = 0;
		m_bytesInLastBlock = 0;

		break;

	case (XSECCryptoSymmetricKey::KEY_AES_128) :
	case (XSECCryptoSymmetricKey::KEY_AES_192) :
	case (XSECCryptoSymmetricKey::KEY_AES_256) :

		// An AES key

		m_blockSize = 16;
		if (m_keyMode == MODE_CBC)
			m_ivSize = 16;
		else 
			m_ivSize = 0;
		m_bytesInLastBlock = 0;

		break;
	
	default :

		// Cannot do this without an IV
		throw XSECCryptoException(XSECCryptoException::SymmetricError,
			"WinCAPI:SymmetricKey - Unknown key type"); 

	}

	if (usedIV != NULL && m_ivSize > 0)
		memcpy(m_lastBlock, usedIV, m_ivSize);


}
bool WinCAPICryptoSymmetricKey::encryptInit(bool doPad, 
											SymmetricKeyMode mode, 
											const unsigned char * iv) {

	m_doPad = doPad;
	m_keyMode = mode;
	m_initialised = false;
	encryptCtxInit(iv);
	return true;

}

unsigned int WinCAPICryptoSymmetricKey::encrypt(const unsigned char * inBuf, 
								 unsigned char * cipherBuf, 
								 unsigned int inLength,
								 unsigned int maxOutLength) {

	if (m_initialised == false) {

		encryptInit();

	}

	// NOTE: This won't actually stop WinCAPI blowing the buffer, so the onus is
	// on the caller.

	unsigned int offset = 0;
	unsigned char * bufPtr;
	unsigned char * encPtr;	// Ptr to start of block to encrypt

	DWORD outl = 0;
	
	if (m_ivSize > 0) {

		DWORD len = 16;
		if (!CryptGetKeyParam(m_k, KP_IV, (unsigned char *) m_lastBlock, &len, 0)) {

			throw XSECCryptoException(XSECCryptoException::SymmetricError,
			"WinCAPI:SymmetricKey - Error setting IV"); 

		}
		memcpy(cipherBuf, m_lastBlock, m_ivSize);
		offset = m_ivSize;
		outl += m_ivSize;
		m_ivSize = 0;

	}

	/* As per bug #38365 - we need to ensure there are enough characters to encrypt.
	   Otherwise we get some nasty errors due to rounding calculations when inputs
	   are too small */

	if (m_bytesInLastBlock + inLength < m_blockSize) {
		// Not enough input data
		memcpy(&m_lastBlock[m_bytesInLastBlock], inBuf, inLength);
		m_bytesInLastBlock += inLength;

		// Just in case we have returned an IV
		return outl;
	}

	bufPtr = &cipherBuf[offset];
	encPtr = bufPtr;

	if (m_bytesInLastBlock > 0) {
		memcpy(bufPtr, m_lastBlock, m_bytesInLastBlock);
		bufPtr = &bufPtr[m_bytesInLastBlock];
		outl += m_bytesInLastBlock;
	}

	unsigned int rounding = (m_bytesInLastBlock + inLength) % m_blockSize;
	rounding += m_blockSize;

	outl += inLength - rounding;
	if (outl > maxOutLength) {
		throw XSECCryptoException(XSECCryptoException::SymmetricError,
			"WinCAPI:SymmetricKey - Not enough space in output buffer for encrypt"); 
	}

	outl -= offset;

	// Now copy back one block + rounding
	memcpy(m_lastBlock, &inBuf[inLength - rounding], rounding);
	m_bytesInLastBlock = rounding;

	// Finally, copy in last of buffer to encrypt
	memcpy(bufPtr, inBuf, inLength - rounding);

	// Do the enrypt
	if (!CryptEncrypt(m_k, 0, FALSE, 0, encPtr, &outl, maxOutLength)) {
		throw XSECCryptoException(XSECCryptoException::SymmetricError,
			"WinCAPI:SymmetricKey - Error during WinCAPI encrypt"); 
	}
	
	return outl + offset;

}

unsigned int WinCAPICryptoSymmetricKey::encryptFinish(unsigned char * cipherBuf,
													  unsigned int maxOutLength) {

	DWORD outl = m_bytesInLastBlock + m_blockSize;

	if (outl > maxOutLength) {
		throw XSECCryptoException(XSECCryptoException::SymmetricError,
			"WinCAPI:SymmetricKey - Not enough space in output buffer for encrypt - NOTE WinCAPI requires an extra block to complete encryption"); 
	}

	outl = 0;

	if (m_bytesInLastBlock != 0) {

		memcpy(cipherBuf, m_lastBlock, m_bytesInLastBlock);
		outl = m_bytesInLastBlock;

	}

	if (!CryptEncrypt(m_k, 0, TRUE, 0, cipherBuf, &outl, maxOutLength)) {
		throw XSECCryptoException(XSECCryptoException::SymmetricError,
			"WinCAPI:SymmetricKey - Error during WinCAPI encrypt"); 
	}

	if (!m_doPad) {

		// It is the responsibility of the caller to ensure they have 
		// passed in block size num bytes

		if (outl >= m_blockSize)
			outl -= m_blockSize;

	}

	return outl;

}


// --------------------------------------------------------------------------------
//           Create a windows key
// --------------------------------------------------------------------------------

HCRYPTKEY WinCAPICryptoSymmetricKey::createWindowsKey(
								const unsigned char * key, 
								unsigned int keyLen, 
								XSECCryptoSymmetricKey::SymmetricKeyType type,
								HCRYPTPROV * prov) {

	// First get the correct Provider handle to load the key into
	
	HCRYPTPROV p;
	
	if (prov == NULL || *prov == 0) {
		if (!strEquals(XSECPlatformUtils::g_cryptoProvider->getProviderName(), DSIGConstants::s_unicodeStrPROVWinCAPI)) {
			throw XSECCryptoException(XSECCryptoException::SymmetricError,
				"WinCAPI:SymmetricKey - Main provider is not Windows provider"); 
		}

		WinCAPICryptoProvider * cp = 
			(WinCAPICryptoProvider*) XSECPlatformUtils::g_cryptoProvider;

		p = cp->getApacheKeyStore();

		if (p == 0) {

			throw XSECCryptoException(XSECCryptoException::SymmetricError,
			"WinCAPI:SymmetricKey - Unable to retrieve internal key store"); 

		}

		if (prov != NULL)
			*prov = p;
	}

	else if (prov != NULL)
		p = *prov;

	// Get the key wrapping key
	HCRYPTKEY k;
	if (!CryptGetUserKey(p, AT_KEYEXCHANGE, &k)) {
		throw XSECCryptoException(XSECCryptoException::SymmetricError,
			"WinCAPI:SymmetricKey - Unable to retrieve internal key pair"); 
	}

	// Find out how long the output will be
	DWORD outl = 0;
	if (!CryptEncrypt(k, 0, TRUE, 0, 0, &outl, keyLen)) {
		DWORD error = GetLastError();
		if (error == NTE_BAD_KEY) {
			// We throw either way, but this is *likely* to be an unsupported OS issue
			throw XSECCryptoException(XSECCryptoException::SymmetricError,
				"WinCAPI:SymmetricKey::createWindowsKey - Error encrypting a key - is this >= Windows 2000?");
		}

		throw XSECCryptoException(XSECCryptoException::SymmetricError,
			"WinCAPI:SymmetricKey - Unable to determine space required to encrypt key");
	}

	// Create the necessary buffer
	unsigned char * encryptBuf;
	unsigned int encryptBufSize = outl;
	XSECnew(encryptBuf, unsigned char[outl]);
	ArrayJanitor<unsigned char> j_encryptBuf(encryptBuf);

	memcpy(encryptBuf, key, keyLen);
	outl = keyLen;

	// Do the encrypt

	if (!CryptEncrypt(k, 0, TRUE, 0, encryptBuf, &outl, encryptBufSize)) {
		throw XSECCryptoException(XSECCryptoException::SymmetricError,
			"WinCAPI:SymmetricKey - Unable to encrypt key");
	}

	// Now we have the encrypted buffer, create a SIMPLEBLOB structure

	unsigned char * simpleBlob;
	XSECnew(simpleBlob, unsigned char [sizeof (BLOBHEADER) + sizeof (DWORD) +  outl]);
	ArrayJanitor<unsigned char> j_simpleBlob(simpleBlob);

	BLOBHEADER * blobHeader = (BLOBHEADER *) simpleBlob;
	blobHeader->bType = SIMPLEBLOB;
	blobHeader->bVersion = CUR_BLOB_VERSION;
	blobHeader->reserved = 0;

	unsigned int expectedLength;

	switch (type) {

	case (XSECCryptoSymmetricKey::KEY_3DES_192) :
					blobHeader->aiKeyAlg = CALG_3DES;
					expectedLength = 24;
					break;
	case (XSECCryptoSymmetricKey::KEY_AES_128) :
					blobHeader->aiKeyAlg = CALG_AES_128;
					expectedLength = 16;
					break;
	case (XSECCryptoSymmetricKey::KEY_AES_192) :
					blobHeader->aiKeyAlg = CALG_AES_192;
					expectedLength = 24;
					break;
	case (XSECCryptoSymmetricKey::KEY_AES_256) :
					blobHeader->aiKeyAlg = CALG_AES_256;
					expectedLength = 32;
					break;
	default :

		throw XSECCryptoException(XSECCryptoException::SymmetricError,
			"WinCAPI:SymmetricKey::createWindowsKey - Unknown Symmetric key type");

	}

	// Check key length - otherwise the user could get some very cryptic error messages
	if (keyLen != expectedLength) {
		throw XSECCryptoException(XSECCryptoException::SymmetricError,
			"WinCAPI:SymmetricKey::createWindowsKey - Key length incorrect for algorithm");
	}

	DWORD * algId = (DWORD *) (simpleBlob + sizeof(BLOBHEADER));
	*algId = CALG_RSA_KEYX;

	// Copy in the encrypted data
	memcpy(&simpleBlob[sizeof(BLOBHEADER) + sizeof(DWORD)], encryptBuf, outl);

	// Now do the import

	HCRYPTKEY k2;

	if (!CryptImportKey(p, simpleBlob, sizeof(BLOBHEADER) + sizeof(DWORD) + outl, k, CRYPT_EXPORTABLE, &k2)) {
		throw XSECCryptoException(XSECCryptoException::SymmetricError,
			"WinCAPI:SymmetricKey - Unable to import key");
	}

	return k2;

}


#endif /* XSEC_HAVE_WINCAPI */
