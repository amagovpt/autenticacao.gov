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
 * Author(s): Berin Lautenbach
 *
 * $Id: OpenSSLCryptoSymmetricKey.cpp 1353308 2012-06-24 18:43:47Z scantor $
 *
 */

#include <xsec/framework/XSECDefs.hpp>
#include <iostream>
#include <xsec/enc/OpenSSL/OpenSSLCryptoSymmetricKey.hpp>
#include <xsec/framework/XSECError.hpp>
#include <xsec/enc/XSECCryptoException.hpp>

#include <xercesc/util/Janitor.hpp>
XERCES_CPP_NAMESPACE_USE;

#if defined (XSEC_HAVE_OPENSSL)

#include <string.h>

#include <openssl/rand.h>

// --------------------------------------------------------------------------------
//           Constructors and Destructors
// --------------------------------------------------------------------------------

OpenSSLCryptoSymmetricKey::OpenSSLCryptoSymmetricKey(XSECCryptoSymmetricKey::SymmetricKeyType type) :
m_keyType(type),
m_keyMode(MODE_NONE),
m_keyBuf(""),
m_tagBuf(""),
m_keyLen(0),
m_initialised(false) {

	EVP_CIPHER_CTX_init(&m_ctx);
	m_keyBuf.isSensitive();

}

OpenSSLCryptoSymmetricKey::~OpenSSLCryptoSymmetricKey() {

	// Clean up the context

	EVP_CIPHER_CTX_cleanup(&m_ctx);
}

// --------------------------------------------------------------------------------
//           Basic Key interface methods
// --------------------------------------------------------------------------------

XSECCryptoSymmetricKey::SymmetricKeyType OpenSSLCryptoSymmetricKey::getSymmetricKeyType() const {

	return m_keyType;

}

const XMLCh * OpenSSLCryptoSymmetricKey::getProviderName() const {

	return DSIGConstants::s_unicodeStrPROVOpenSSL;

}

XSECCryptoKey * OpenSSLCryptoSymmetricKey::clone() const {

	OpenSSLCryptoSymmetricKey * ret;

	XSECnew(ret, OpenSSLCryptoSymmetricKey(m_keyType));
	ret->m_keyMode = m_keyMode;
	ret->m_keyLen = m_keyLen;
	ret->m_keyBuf = m_keyBuf;

	return ret;

}

// --------------------------------------------------------------------------------
//           Store the key value
// --------------------------------------------------------------------------------

void OpenSSLCryptoSymmetricKey::setKey(const unsigned char * key, unsigned int keyLen) {

	m_keyBuf.sbMemcpyIn(key, keyLen);
	m_keyLen = keyLen;

}

// --------------------------------------------------------------------------------
//           Decrypt
// --------------------------------------------------------------------------------

int OpenSSLCryptoSymmetricKey::decryptCtxInit(const unsigned char* iv, const unsigned char* tag, unsigned taglen) {

	// Returns amount of IV data used (in bytes)
	// Sets m_initialised iff the key is OK and the IV is OK.

    // GCM modes will leave this unset until the second call with the iv

	if (m_initialised)
		return 0;

	if (m_keyLen == 0) {
		throw XSECCryptoException(XSECCryptoException::SymmetricError,
			"OpenSSL:SymmetricKey - Cannot initialise without key"); 
	}
    else if (m_keyMode == MODE_NONE) {
		throw XSECCryptoException(XSECCryptoException::SymmetricError,
			"OpenSSL:SymmetricKey - Cannot initialise without mode"); 
    }

	// Set up the context according to the required cipher type

	switch (m_keyType) {

	case (KEY_3DES_192) :

		// A 3DES key

		if (m_keyMode == MODE_CBC) {

			if (iv == NULL) {
				return 0;	// Cannot initialise without an IV
			}

			/* Do not use "_ex" calls yet - as want backwards compatibility
			   with 0.9.6 */

#if defined(XSEC_OPENSSL_CONST_BUFFERS)
			EVP_DecryptInit(&m_ctx, EVP_des_ede3_cbc(),m_keyBuf.rawBuffer(), iv);
#else
			EVP_DecryptInit(&m_ctx, EVP_des_ede3_cbc(),(unsigned char *) m_keyBuf.rawBuffer(), (unsigned char *) iv);
#endif
			m_ivSize = 8;
		}
		else if (m_keyMode == MODE_ECB) {
#if defined(XSEC_OPENSSL_CONST_BUFFERS)
			EVP_DecryptInit(&m_ctx, EVP_des_ecb(), m_keyBuf.rawBuffer(), NULL);
#else
			EVP_DecryptInit(&m_ctx, EVP_des_ecb(), (unsigned char *) m_keyBuf.rawBuffer(), NULL);
#endif
			m_ivSize = 0;
		}
        else {
		    throw XSECCryptoException(XSECCryptoException::SymmetricError,
			    "OpenSSL:SymmetricKey - Unsupported DES3 cipher mode"); 
        }


		m_blockSize = 8;
		break;

#if defined (XSEC_OPENSSL_HAVE_AES)

	case (KEY_AES_128) :

		// An AES key

		if (m_keyMode == MODE_CBC) {

			if (iv == NULL) {
				return 0;	// Cannot initialise without an IV
			}

			EVP_DecryptInit_ex(&m_ctx, EVP_aes_128_cbc(), NULL, m_keyBuf.rawBuffer(), iv);

		}
#if defined (XSEC_OPENSSL_HAVE_GCM)
        else if (m_keyMode == MODE_GCM) {

            if (tag != NULL && taglen != 16) {
		        throw XSECCryptoException(XSECCryptoException::SymmetricError,
			        "OpenSSL:SymmetricKey - Invalid authentication tag"); 
            }

            if (iv == NULL) {
                // Just save off tag for later.
                m_tagBuf.sbMemcpyIn(tag, taglen);
                return 0;
            }

            if (m_tagBuf.sbRawBufferSize() == 0) {
		        throw XSECCryptoException(XSECCryptoException::SymmetricError,
			        "OpenSSL:SymmetricKey - Authentication tag not set"); 
            }

            // We have everything, so we can fully init.
            EVP_CipherInit(&m_ctx, EVP_aes_128_gcm(), NULL, NULL, 0);
            EVP_CIPHER_CTX_ctrl(&m_ctx, EVP_CTRL_GCM_SET_IVLEN, 12, NULL);
            EVP_CIPHER_CTX_ctrl(&m_ctx, EVP_CTRL_GCM_SET_TAG, 16, (void*)m_tagBuf.rawBuffer());
            EVP_CipherInit(&m_ctx, NULL, m_keyBuf.rawBuffer(), iv, 0);
		}
#endif
		else if (m_keyMode == MODE_ECB) {

			EVP_DecryptInit_ex(&m_ctx, EVP_aes_128_ecb(), NULL, m_keyBuf.rawBuffer(), NULL);

		}
        else {
		    throw XSECCryptoException(XSECCryptoException::SymmetricError,
			    "OpenSSL:SymmetricKey - Unsupported AES cipher mode"); 
        }

		m_blockSize = 16;
		break;
	
	case (KEY_AES_192) :

		// An AES key

		if (m_keyMode == MODE_CBC) {

			if (iv == NULL) {
				return 0;	// Cannot initialise without an IV
			}

			EVP_DecryptInit_ex(&m_ctx, EVP_aes_192_cbc(), NULL, m_keyBuf.rawBuffer(), iv);

		}
#if defined (XSEC_OPENSSL_HAVE_GCM)
        else if (m_keyMode == MODE_GCM) {

            if (tag != NULL && taglen != 16) {
		        throw XSECCryptoException(XSECCryptoException::SymmetricError,
			        "OpenSSL:SymmetricKey - Invalid authentication tag"); 
            }

            if (iv == NULL) {
                // Just save off tag for later.
                m_tagBuf.sbMemcpyIn(tag, taglen);
                return 0;
            }

            if (m_tagBuf.sbRawBufferSize() == 0) {
		        throw XSECCryptoException(XSECCryptoException::SymmetricError,
			        "OpenSSL:SymmetricKey - Authentication tag not set"); 
            }

            // We have everything, so we can fully init.
            EVP_CipherInit(&m_ctx, EVP_aes_192_gcm(), NULL, NULL, 0);
            EVP_CIPHER_CTX_ctrl(&m_ctx, EVP_CTRL_GCM_SET_IVLEN, 12, NULL);
            EVP_CIPHER_CTX_ctrl(&m_ctx, EVP_CTRL_GCM_SET_TAG, 16, (void*)m_tagBuf.rawBuffer());
            EVP_CipherInit(&m_ctx, NULL, m_keyBuf.rawBuffer(), iv, 0);

		}
#endif
		else if (m_keyMode == MODE_ECB) {

			EVP_DecryptInit_ex(&m_ctx, EVP_aes_192_ecb(), NULL, m_keyBuf.rawBuffer(), NULL);

		}
        else {
		    throw XSECCryptoException(XSECCryptoException::SymmetricError,
			    "OpenSSL:SymmetricKey - Unsupported AES cipher mode"); 
        }

		m_blockSize = 16;
		break;

	case (KEY_AES_256) :

		// An AES key

		if (m_keyMode == MODE_CBC) {

			if (iv == NULL) {
				return 0;	// Cannot initialise without an IV
			}

			EVP_DecryptInit_ex(&m_ctx, EVP_aes_256_cbc(), NULL, m_keyBuf.rawBuffer(), iv);

		}
#if defined (XSEC_OPENSSL_HAVE_GCM)
        else if (m_keyMode == MODE_GCM) {

            if (tag != NULL && taglen != 16) {
		        throw XSECCryptoException(XSECCryptoException::SymmetricError,
			        "OpenSSL:SymmetricKey - Invalid authentication tag"); 
            }

            if (iv == NULL) {
                // Just save off tag for later.
                m_tagBuf.sbMemcpyIn(tag, taglen);
                return 0;
            }

            if (m_tagBuf.sbRawBufferSize() == 0) {
		        throw XSECCryptoException(XSECCryptoException::SymmetricError,
			        "OpenSSL:SymmetricKey - Authentication tag not set"); 
            }

            // We have everything, so we can fully init.
            EVP_CipherInit(&m_ctx, EVP_aes_256_gcm(), NULL, NULL, 0);
            EVP_CIPHER_CTX_ctrl(&m_ctx, EVP_CTRL_GCM_SET_IVLEN, 12, NULL);
            EVP_CIPHER_CTX_ctrl(&m_ctx, EVP_CTRL_GCM_SET_TAG, 16, (void*)m_tagBuf.rawBuffer());
            EVP_CipherInit(&m_ctx, NULL, m_keyBuf.rawBuffer(), iv, 0);

		}
#endif
		else if (m_keyMode == MODE_ECB) {

			EVP_DecryptInit_ex(&m_ctx, EVP_aes_256_ecb(), NULL, m_keyBuf.rawBuffer(), NULL);

		}
        else {
		    throw XSECCryptoException(XSECCryptoException::SymmetricError,
			    "OpenSSL:SymmetricKey - Unsupported AES cipher mode"); 
        }

		m_blockSize = 16;
		break;
#else 

	case (KEY_AES_128) :
	case (KEY_AES_192) :
	case (KEY_AES_256) :

		throw XSECCryptoException(XSECCryptoException::UnsupportedAlgorithm,
			 "OpenSSL:SymmetricKey - AES not supported in this version of OpenSSL");

#endif /* XSEC_OPENSSL_HAVE_AES */
	
	default :

		throw XSECCryptoException(XSECCryptoException::SymmetricError,
			"OpenSSL:SymmetricKey - Unknown key type"); 

	}


	// Setup ivSize
    switch (m_keyMode) {
        case MODE_CBC:
            m_ivSize = m_blockSize;
            break;

        case MODE_GCM:
            m_ivSize = 12;
            break;

        default:
            m_ivSize = 0;
    }

	// Reset some parameters
	m_initialised = true;
	m_bytesInLastBlock = 0;

	// Disable OpenSSL padding - The interop samples have broken PKCS padding - AARGHH

#if defined (XSEC_OPENSSL_CANSET_PADDING)
	EVP_CIPHER_CTX_set_padding(&m_ctx, 0);
#endif

	// Return number of bytes chewed up by IV
	return m_ivSize;
}


bool OpenSSLCryptoSymmetricKey::decryptInit(bool doPad, 
											SymmetricKeyMode mode,
											const unsigned char* iv,
                                            const unsigned char* tag,
                                            unsigned int taglen) {

	m_doPad = doPad;
	m_keyMode = mode;
	m_initialised = false;
	decryptCtxInit(iv, tag, taglen);
	return true;

}

unsigned int OpenSSLCryptoSymmetricKey::decrypt(const unsigned char * inBuf, 
								 unsigned char * plainBuf, 
								 unsigned int inLength,
								 unsigned int maxOutLength) {



	// NOTE: This won't actually stop OpenSSL blowing the buffer, so the onus is
	// on the caller.

#if 0

	/* normally commented out code to determine endian problems */
	unsigned int i;
	unsigned char e[2048];
	unsigned char * inBuf1 = (unsigned char *) inBuf;
	if (inLength < 2048) {
		memcpy(e, inBuf, inLength);
		for (i = 0; i < inLength;++i) {
			inBuf1[i] = e[inLength - 1 - i];
		}
	}

#endif

    // If cipher isn't initialized yet, we call back in with the ciphertext to supply the IV
    // For GCM, the tag has to have been supplied already or this will fail.

	unsigned int offset = 0;
	if (!m_initialised) {
        offset = decryptCtxInit(inBuf, NULL, 0);
		
        if (offset > inLength) {
			throw XSECCryptoException(XSECCryptoException::SymmetricError,
    			"OpenSSL:SymmetricKey - Not enough data passed in to get IV");
		}
	}

	int outl = maxOutLength;

	if (inLength - offset + m_bytesInLastBlock > maxOutLength) {
		throw XSECCryptoException(XSECCryptoException::SymmetricError,
			"OpenSSLSymmetricKey::decrypt - Not enough space in output buffer");
	}

#if defined (XSEC_OPENSSL_CONST_BUFFERS)
	if (EVP_DecryptUpdate(&m_ctx, &plainBuf[m_bytesInLastBlock], &outl, &inBuf[offset], inLength - offset) == 0) {
#else
	if (EVP_DecryptUpdate(&m_ctx, &plainBuf[m_bytesInLastBlock], &outl, (unsigned char *) &inBuf[offset], inLength - offset) == 0) {
#endif
		throw XSECCryptoException(XSECCryptoException::SymmetricError,
			"OpenSSL:SymmetricKey - Error during OpenSSL decrypt"); 

	}

	// Store the last block if we are padding
	if (m_doPad && m_blockSize > 0 && outl >= m_blockSize) {

		// Output will always be *at least* the blocksize

		// Copy the previous last block into the start of the output buffer
		memcpy(plainBuf, m_lastBlock, m_bytesInLastBlock);

		// Copy the tail into the buffer
		memcpy(m_lastBlock, &plainBuf[outl + m_bytesInLastBlock - m_blockSize], m_blockSize);

		outl = outl + m_bytesInLastBlock - m_blockSize;
		m_bytesInLastBlock = m_blockSize;

	}

	return outl;

}

unsigned int OpenSSLCryptoSymmetricKey::decryptFinish(unsigned char * plainBuf,
													  unsigned int maxOutLength) {

	int outl = maxOutLength;
	m_initialised = false;

#if defined (XSEC_OPENSSL_CANSET_PADDING)

	if (EVP_DecryptFinal(&m_ctx, plainBuf, &outl) == 0) {

		throw XSECCryptoException(XSECCryptoException::SymmetricError,
			"OpenSSL:SymmetricKey - Error during OpenSSL decrypt finalisation"); 

	}
	
	if (outl > 0) {
	
		// Should never see any bytes output, as we are not padding

		throw XSECCryptoException(XSECCryptoException::SymmetricError,
			"OpenSSL:SymmetricKey - Unexpectedly recieved bytes from EVP_DecryptFinal_ex"); 

	}

	// Calculate any padding issues
	if (m_doPad && m_bytesInLastBlock == m_blockSize) {

		outl = m_blockSize - m_lastBlock[m_blockSize - 1];

		if (outl > m_blockSize || outl < 0) {
			
			throw XSECCryptoException(XSECCryptoException::SymmetricError,
				"OpenSSL:SymmetricKey::decryptFinish - Out of range padding value in final block"); 
	
		}

		memcpy(plainBuf, m_lastBlock, outl);

	}

	if ((unsigned int) outl > maxOutLength) {
		throw XSECCryptoException(XSECCryptoException::SymmetricError,
			"OpenSSLSymmetricKey::decryptFinish - **WARNING** - Plaintext output > maxOutLength!"); 
	}

	return outl;

#else

	/* Working with a version of OpenSSL that *always* performs padding
	   so we need to work around it */
	unsigned char *scrPlainBuf;
	unsigned char *cipherBuf;
	scrPlainBuf = new unsigned char[3 * m_blockSize];
	ArrayJanitor<unsigned char> j_scrPlainBuf(scrPlainBuf);
	cipherBuf = new unsigned char[m_blockSize];
	ArrayJanitor<unsigned char> j_cipherBuf(cipherBuf);

	/* Zeroise the cipher buffer */
	memset(cipherBuf, 0, m_blockSize);

	unsigned int offset = 0;

	/* Get any previous bytes from the m_lastBlock */
	if (m_bytesInLastBlock > 0 && m_bytesInLastBlock <= m_blockSize) {
		memcpy(scrPlainBuf, m_lastBlock, m_bytesInLastBlock);
		offset = m_bytesInLastBlock;
	}

	outl = m_blockSize;

	/* This is really ugly - but we have to trick OpenSSL into thinking there
       is more, so that it sends us the lasts block with the padding in it.
       We can then clean that up ourselves
	*/

	if (EVP_DecryptUpdate(&m_ctx, &scrPlainBuf[offset], &outl, cipherBuf, m_blockSize) == 0) {
		throw XSECCryptoException(XSECCryptoException::SymmetricError,
			"OpenSSL:SymmetricKey - Error cecrypting final block during OpenSSL");
	} 

	outl += offset;

	if (m_doPad && outl > 0) {

		/* Strip any padding */
		outl -= scrPlainBuf[outl - 1];

		if (outl > (2 * m_blockSize) || outl < 0) {
			
			throw XSECCryptoException(XSECCryptoException::SymmetricError,
				"OpenSSL:SymmetricKey::decryptFinish - Out of range padding value in final block"); 
	
		}

	}

	if (outl > (int) maxOutLength) {
		throw XSECCryptoException(XSECCryptoException::SymmetricError,
			"OpenSSLSymmetricKey::decryptFinish - **WARNING** - Plaintext output > maxOutLength!"); 
	}

	if (outl > 0) {

		memcpy(plainBuf, scrPlainBuf, outl);

	}

	return outl;
#endif

}

// --------------------------------------------------------------------------------
//           Encrypt
// --------------------------------------------------------------------------------

bool OpenSSLCryptoSymmetricKey::encryptInit(bool doPad, 
											SymmetricKeyMode mode,
											const unsigned char * iv) {

	if (m_initialised == true)
		return true;

	m_doPad = doPad;
	m_keyMode = mode;
	
	if (m_keyLen == 0) {
		throw XSECCryptoException(XSECCryptoException::SymmetricError,
			"OpenSSL:SymmetricKey - Cannot initialise without key"); 
	}
    else if (m_keyMode == MODE_NONE) {
		throw XSECCryptoException(XSECCryptoException::SymmetricError,
			"OpenSSL:SymmetricKey - Cannot initialise without mode"); 
    }

	// Do some parameter initialisation
	m_initialised = true;
	m_bytesInLastBlock = 0;

	// Set up the context according to the required cipher type

	const unsigned char * usedIV = NULL;
	unsigned char genIV[256];

	// Tell the library that the IV still has to be sent

	m_ivSent = false;

	switch (m_keyType) {

	case (XSECCryptoSymmetricKey::KEY_3DES_192) :

		// A 3DES key

		if (m_keyMode == MODE_CBC) {

			if (iv == NULL) {
				
				bool res = ((RAND_status() == 1) && (RAND_bytes(genIV, 8) == 1));
				if (res == false) {
					throw XSECCryptoException(XSECCryptoException::SymmetricError,
						"OpenSSL:SymmetricKey - Error generating random IV");
				}

				usedIV = genIV;

			}
			else {
				usedIV = iv;
            }

#if defined (XSEC_OPENSSL_CONST_BUFFERS)
			EVP_EncryptInit(&m_ctx, EVP_des_ede3_cbc(), m_keyBuf.rawBuffer(), usedIV);
#else
			EVP_EncryptInit(&m_ctx, EVP_des_ede3_cbc(), (unsigned char *) m_keyBuf.rawBuffer(), (unsigned char *) usedIV);
#endif
		}
		else if (m_keyMode == MODE_ECB) {
#if defined (XSEC_OPENSSL_CONST_BUFFERS)
			EVP_EncryptInit(&m_ctx, EVP_des_ede3_ecb(), m_keyBuf.rawBuffer(), NULL);
#else
			EVP_EncryptInit(&m_ctx, EVP_des_ede3(), (unsigned char *) m_keyBuf.rawBuffer(), NULL);
#endif
		}
        else {
		    throw XSECCryptoException(XSECCryptoException::SymmetricError,
			    "OpenSSL:SymmetricKey - Unsupported DES3 cipher mode"); 
        }


		m_blockSize = 8;
		break;

#if defined (XSEC_OPENSSL_HAVE_AES)

	case (XSECCryptoSymmetricKey::KEY_AES_128) :

		// An AES key

		if (m_keyMode == MODE_CBC) {

			if (iv == NULL) {
				
				bool res = ((RAND_status() == 1) && (RAND_bytes(genIV, 16) == 1));
				if (res == false) {
					throw XSECCryptoException(XSECCryptoException::SymmetricError,
						"OpenSSL:SymmetricKey - Error generating random IV");
				}

				usedIV = genIV;

			}
			else
				usedIV = iv;

			EVP_EncryptInit_ex(&m_ctx, EVP_aes_128_cbc(), NULL, m_keyBuf.rawBuffer(), usedIV);
		}
		else if (m_keyMode == MODE_ECB) {

			EVP_EncryptInit_ex(&m_ctx, EVP_aes_128_ecb(), NULL, m_keyBuf.rawBuffer(), NULL);

		}
#ifdef XSEC_OPENSSL_HAVE_GCM
		else if (m_keyMode == MODE_GCM) {

			if (iv == NULL) {
				
				bool res = ((RAND_status() == 1) && (RAND_bytes(genIV, 12) == 1));
				if (res == false) {
					throw XSECCryptoException(XSECCryptoException::SymmetricError,
						"OpenSSL:SymmetricKey - Error generating random IV");
				}

				usedIV = genIV;

			}
			else
				usedIV = iv;

			EVP_EncryptInit_ex(&m_ctx, EVP_aes_128_gcm(), NULL, m_keyBuf.rawBuffer(), usedIV);
		}
#endif
        else {
		    throw XSECCryptoException(XSECCryptoException::SymmetricError,
			    "OpenSSL:SymmetricKey - Unsupported AES cipher mode");
        }

		m_blockSize = 16;
		break;

	case (XSECCryptoSymmetricKey::KEY_AES_192) :

		// An AES key

		if (m_keyMode == MODE_CBC) {

			if (iv == NULL) {
				
				bool res = ((RAND_status() == 1) && (RAND_bytes(genIV, 16) == 1));
				if (res == false) {
					throw XSECCryptoException(XSECCryptoException::SymmetricError,
						"OpenSSL:SymmetricKey - Error generating random IV");
				}

				usedIV = genIV;

			}
			else
				usedIV = iv;

			EVP_EncryptInit_ex(&m_ctx, EVP_aes_192_cbc(), NULL, m_keyBuf.rawBuffer(), usedIV);

		}
#ifdef XSEC_OPENSSL_HAVE_GCM
		else if (m_keyMode == MODE_GCM) {

			if (iv == NULL) {
				
				bool res = ((RAND_status() == 1) && (RAND_bytes(genIV, 12) == 1));
				if (res == false) {
					throw XSECCryptoException(XSECCryptoException::SymmetricError,
						"OpenSSL:SymmetricKey - Error generating random IV");
				}

				usedIV = genIV;

			}
			else
				usedIV = iv;

			EVP_EncryptInit_ex(&m_ctx, EVP_aes_192_gcm(), NULL, m_keyBuf.rawBuffer(), usedIV);
		}
#endif
		else if (m_keyMode == MODE_ECB) {

			EVP_EncryptInit_ex(&m_ctx, EVP_aes_192_ecb(), NULL, m_keyBuf.rawBuffer(), NULL);
		}
        else {
		    throw XSECCryptoException(XSECCryptoException::SymmetricError,
			    "OpenSSL:SymmetricKey - Unsupported AES cipher mode");
        }

		m_blockSize = 16;
		break;

	case (XSECCryptoSymmetricKey::KEY_AES_256) :

		// An AES key

		if (m_keyMode == MODE_CBC) {
			if (iv == NULL) {
				
				bool res = ((RAND_status() == 1) && (RAND_bytes(genIV, 16) == 1));
				if (res == false) {
					throw XSECCryptoException(XSECCryptoException::SymmetricError,
						"OpenSSL:SymmetricKey - Error generating random IV");
				}

				usedIV = genIV;

			}
			else
				usedIV = iv;

			EVP_EncryptInit_ex(&m_ctx, EVP_aes_256_cbc(), NULL, m_keyBuf.rawBuffer(), usedIV);

		}
#ifdef XSEC_OPENSSL_HAVE_GCM
		else if (m_keyMode == MODE_GCM) {

			if (iv == NULL) {
				
				bool res = ((RAND_status() == 1) && (RAND_bytes(genIV, 12) == 1));
				if (res == false) {
					throw XSECCryptoException(XSECCryptoException::SymmetricError,
						"OpenSSL:SymmetricKey - Error generating random IV");
				}

				usedIV = genIV;

			}
			else
				usedIV = iv;

			EVP_EncryptInit_ex(&m_ctx, EVP_aes_256_gcm(), NULL, m_keyBuf.rawBuffer(), usedIV);
		}
#endif
		else if (m_keyMode == MODE_ECB) {

			EVP_EncryptInit_ex(&m_ctx, EVP_aes_256_ecb(), NULL, m_keyBuf.rawBuffer(), NULL);

		}
        else {
		    throw XSECCryptoException(XSECCryptoException::SymmetricError,
			    "OpenSSL:SymmetricKey - Unsupported AES cipher mode");
        }

		m_blockSize = 16;
		break;

#else 

	case (XSECCryptoSymmetricKey::KEY_AES_128) :
	case (XSECCryptoSymmetricKey::KEY_AES_192) :
	case (XSECCryptoSymmetricKey::KEY_AES_256) :

		throw XSECCryptoException(XSECCryptoException::UnsupportedAlgorithm,
			 "OpenSSL:SymmetricKey - AES not supported in this version of OpenSSL");

#endif /* XSEC_OPENSSL_HAVE_AES */

	default :

		throw XSECCryptoException(XSECCryptoException::SymmetricError,
			"OpenSSL:SymmetricKey - Unknown key type"); 

	}

	// Clear up any read padding

	if (m_keyMode == MODE_CBC) {
		m_ivSize = m_blockSize;
		memcpy(m_lastBlock, usedIV, m_ivSize);
	}
    else if (m_keyMode == MODE_GCM) {
        m_ivSize = 12;
        memcpy(m_lastBlock, usedIV, m_ivSize);
    }
	else {
		m_ivSize = 0;
    }

#if defined (XSEC_OPENSSL_CANSET_PADDING)
	// Setup padding
	if (m_doPad) {
		EVP_CIPHER_CTX_set_padding(&m_ctx, 1);
	}
	else {
		EVP_CIPHER_CTX_set_padding(&m_ctx, 0);
	}
#endif

	return true;

}

unsigned int OpenSSLCryptoSymmetricKey::encrypt(const unsigned char * inBuf, 
								 unsigned char * cipherBuf, 
								 unsigned int inLength,
								 unsigned int maxOutLength) {

	if (m_initialised == false) {

		encryptInit();

	}

	// NOTE: This won't actually stop OpenSSL blowing the buffer, so the onus is
	// on the caller.

	unsigned int offset = 0;
	if (m_ivSent == false && m_ivSize > 0) {

		memcpy(cipherBuf, m_lastBlock, m_ivSize);
		m_ivSent = true;

		offset = m_ivSize;

	}

	int outl = maxOutLength - offset;

	if (inLength + offset > maxOutLength) {

		throw XSECCryptoException(XSECCryptoException::SymmetricError,
			"OpenSSL:SymmetricKey - Not enough space in output buffer for encrypt"); 

	}
#if defined (XSEC_OPENSSL_CONST_BUFFERS)
	if (EVP_EncryptUpdate(&m_ctx, &cipherBuf[offset], &outl, inBuf, inLength) == 0) {
#else
	if (EVP_EncryptUpdate(&m_ctx, &cipherBuf[offset], &outl, (unsigned char *) inBuf, inLength) == 0) {
#endif

		throw XSECCryptoException(XSECCryptoException::SymmetricError,
			"OpenSSL:SymmetricKey - Error during OpenSSL encrypt"); 

	}

	return outl + offset;

}

unsigned int OpenSSLCryptoSymmetricKey::encryptFinish(unsigned char * cipherBuf,
													  unsigned int maxOutLength,
                                                      unsigned int taglen) {

	int outl = maxOutLength;
	m_initialised = false;

	if (EVP_EncryptFinal(&m_ctx, cipherBuf, &outl) == 0) {

		throw XSECCryptoException(XSECCryptoException::SymmetricError,
		  "OpenSSLSymmetricKey::encryptFinish - Error during OpenSSL decrypt finalisation"); 

	}

	if ((unsigned int) outl > maxOutLength) {
		throw XSECCryptoException(XSECCryptoException::SymmetricError,
			"OpenSSLSymmetricKey::encryptFinish - **WARNING** - Cipheroutput > maxOutLength!"); 
	}
	
#if !defined (XSEC_OPENSSL_CANSET_PADDING)
	if (!m_doPad) {

		if (outl < m_blockSize) {
			throw XSECCryptoException(XSECCryptoException::SymmetricError,
			   "OpenSSLSymmetricKey::encryptFinish - cannot remove padding!"); 
		}

		outl -= m_blockSize;

	}
#endif

    if (taglen > 0) {
        // Extract authentication tag and add to ciphertext.
        if (maxOutLength - outl < taglen) {
		    throw XSECCryptoException(XSECCryptoException::SymmetricError,
			    "OpenSSLSymmetricKey::encryptFinish - **WARNING** - no room in ciphertext buffer for authentication tag"); 
        }
        if (m_keyMode == MODE_GCM) {
#ifdef XSEC_OPENSSL_HAVE_GCM
            EVP_CIPHER_CTX_ctrl(&m_ctx, EVP_CTRL_GCM_GET_TAG, taglen, cipherBuf + outl);
            outl += taglen;
#else
		    throw XSECCryptoException(XSECCryptoException::SymmetricError,
			    "OpenSSLSymmetricKey::encryptFinish - AES-GCM not supported in this version of OpenSSL"); 
#endif
        }
        else {
		    throw XSECCryptoException(XSECCryptoException::SymmetricError,
			    "OpenSSLSymmetricKey::encryptFinish - cipher mode does not support authentication tag"); 
        }
    }

	return outl;

}

#endif /* XSEC_HAVE_OPENSSL */
