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
 * XSECAlgorithmHandlerDefault := Interface class to define handling of
 *								  default encryption algorithms
 *
 * $Id: XENCAlgorithmHandlerDefault.cpp 1482595 2013-05-14 21:24:14Z scantor $
 *
 */

// XSEC Includes

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/transformers/TXFMChain.hpp>
#include <xsec/transformers/TXFMCipher.hpp>
#include <xsec/transformers/TXFMBase64.hpp>
#include <xsec/transformers/TXFMSB.hpp>
#include <xsec/xenc/XENCEncryptionMethod.hpp>
#include <xsec/enc/XSECCryptoKey.hpp>
#include <xsec/enc/XSECCryptoSymmetricKey.hpp>
#include <xsec/framework/XSECError.hpp>
#include <xsec/utils/XSECDOMUtils.hpp>

#include "../../utils/XSECAutoPtr.hpp"

#include "XENCAlgorithmHandlerDefault.hpp"

#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/Janitor.hpp>

XERCES_CPP_NAMESPACE_USE

#define _MY_MAX_KEY_SIZE 2048

unsigned char s_3DES_CMS_IV [] = {
	0x4a,
	0xdd,
	0xa2,
	0x2c,
	0x79,
	0xe8,
	0x21,
	0x05
};

unsigned char s_AES_IV [] = {

	0xA6,
	0xA6,
	0xA6,
	0xA6,
	0xA6,
	0xA6,
	0xA6,
	0xA6

};

// --------------------------------------------------------------------------------
//			Compare URI to key type
// --------------------------------------------------------------------------------

void XENCAlgorithmHandlerDefault::mapURIToKey(const XMLCh * uri, 
											  XSECCryptoKey * key,
											  XSECCryptoKey::KeyType &kt,
											  XSECCryptoSymmetricKey::SymmetricKeyType &skt,
											  bool &isSymmetricKeyWrap,
                                              XSECCryptoSymmetricKey::SymmetricKeyMode &skm,
                                              unsigned int& taglen) {

	if (key == NULL) {
		throw XSECException(XSECException::CipherError, 
			"XENCAlgorithmHandlerDefault::mapURIToKey - trying to process a NULL key");
	}

	XSECCryptoSymmetricKey * keySymmetric;
	bool keyOK = false;

	kt = key->getKeyType();
	skt = XSECCryptoSymmetricKey::KEY_NONE;
	isSymmetricKeyWrap = false;
    skm = XSECCryptoSymmetricKey::MODE_NONE;
    taglen = 0;
	
	switch (kt) {

	case XSECCryptoKey::KEY_RSA_PUBLIC :
	case XSECCryptoKey::KEY_RSA_PAIR :
	case XSECCryptoKey::KEY_RSA_PRIVATE :
		keyOK = strEquals(uri, DSIGConstants::s_unicodeStrURIRSA_1_5) ||
			strEquals(uri, DSIGConstants::s_unicodeStrURIRSA_OAEP_MGFP1) ||
            strEquals(uri, DSIGConstants::s_unicodeStrURIRSA_OAEP);
		break;

	case XSECCryptoKey::KEY_SYMMETRIC :

		keySymmetric = (XSECCryptoSymmetricKey *) key;
		if (keySymmetric != NULL) {
			skt = keySymmetric->getSymmetricKeyType();

			switch (skt) {

			case XSECCryptoSymmetricKey::KEY_3DES_192 :
                if (strEquals(uri, DSIGConstants::s_unicodeStrURIKW_3DES)) {
                    keyOK = true;
                    isSymmetricKeyWrap = true;
                    skm = XSECCryptoSymmetricKey::MODE_CBC;
                }
                else if (strEquals(uri, DSIGConstants::s_unicodeStrURI3DES_CBC)) {
                    keyOK = true;
                    skm = XSECCryptoSymmetricKey::MODE_CBC;
                }
				break;

			case XSECCryptoSymmetricKey::KEY_AES_128 :
                if (strEquals(uri, DSIGConstants::s_unicodeStrURIKW_AES128) || strEquals(uri, DSIGConstants::s_unicodeStrURIKW_AES128_PAD)) {
                    keyOK = true;
                    isSymmetricKeyWrap = true;
                    skm = XSECCryptoSymmetricKey::MODE_ECB;
                }
                else if (strEquals(uri, DSIGConstants::s_unicodeStrURIAES128_CBC)) {
                    keyOK = true;
                    skm = XSECCryptoSymmetricKey::MODE_CBC;
                }
                else if (strEquals(uri, DSIGConstants::s_unicodeStrURIAES128_GCM)) {
                    keyOK = true;
                    skm = XSECCryptoSymmetricKey::MODE_GCM;
                    taglen = 16;
                }
				break;

			case XSECCryptoSymmetricKey::KEY_AES_192 :
                if (strEquals(uri, DSIGConstants::s_unicodeStrURIKW_AES192) || strEquals(uri, DSIGConstants::s_unicodeStrURIKW_AES192_PAD)) {
                    keyOK = true;
                    isSymmetricKeyWrap = true;
                    skm = XSECCryptoSymmetricKey::MODE_ECB;
                }
                else if (strEquals(uri, DSIGConstants::s_unicodeStrURIAES192_CBC)) {
                    keyOK = true;
                    skm = XSECCryptoSymmetricKey::MODE_CBC;
                }
                else if (strEquals(uri, DSIGConstants::s_unicodeStrURIAES192_GCM)) {
                    keyOK = true;
                    skm = XSECCryptoSymmetricKey::MODE_GCM;
                    taglen = 16;
                }
				break;

			case XSECCryptoSymmetricKey::KEY_AES_256 :
                if (strEquals(uri, DSIGConstants::s_unicodeStrURIKW_AES256) || strEquals(uri, DSIGConstants::s_unicodeStrURIKW_AES256_PAD)) {
                    keyOK = true;
                    isSymmetricKeyWrap = true;
                    skm = XSECCryptoSymmetricKey::MODE_ECB;
                }
                else if (strEquals(uri, DSIGConstants::s_unicodeStrURIAES256_CBC)) {
                    keyOK = true;
                    skm = XSECCryptoSymmetricKey::MODE_CBC;
                }
                else if (strEquals(uri, DSIGConstants::s_unicodeStrURIAES256_GCM)) {
                    keyOK = true;
                    skm = XSECCryptoSymmetricKey::MODE_GCM;
                    taglen = 16;
                }
				break;

			default:
				break;
			}
		}
		break;

	default:
		 break;
	}

	if (keyOK == false) {
		throw XSECException(XSECException::CipherError, 
			"XENCAlgorithmHandlerDefault::mapURIToKey - key inappropriate for URI");
	}

}

// --------------------------------------------------------------------------------
//			AES Key wrap/unwrap
// --------------------------------------------------------------------------------

unsigned int XENCAlgorithmHandlerDefault::unwrapKeyAES(
   		TXFMChain * cipherText,
		XSECCryptoKey * key,
		safeBuffer & result) {

	// Cat the encrypted key
	XMLByte buf[_MY_MAX_KEY_SIZE];
	XMLByte aesBuf[16];
	XMLByte aesOutBuf[16];
	TXFMBase * b = cipherText->getLastTxfm();
	unsigned int sz = (unsigned int) b->readBytes(buf, _MY_MAX_KEY_SIZE);

	if (sz <= 0) {
		throw XSECException(XSECException::CipherError, 
			"XENCAlgorithmHandlerDefault - AES Wrapped Key not found");
	}

	if (sz == _MY_MAX_KEY_SIZE) {
		throw XSECException(XSECException::CipherError, 
			"XENCAlgorithmHandlerDefault - Key to decrypt too big!");
	}

	// Find number of blocks, and ensure we are a multiple of 64 bits
	if (sz % 8 != 0) {
		throw XSECException(XSECException::CipherError, 
			"XENCAlgorithmHandlerDefault - AES wrapped key not a multiple of 64");
	}

	// Do the decrypt - this cast will throw if wrong, but we should
	// not have been able to get through algorithm checks otherwise
	XSECCryptoSymmetricKey * sk = (XSECCryptoSymmetricKey *) key;

	int blocks = sz / 8;
	int n = blocks - 1;
	for (int j = 5; j >= 0; j--) {
		for (int i = n ; i > 0 ; --i) {

			// Gather blocks to decrypt
			// A
			memcpy(aesBuf, buf, 8);
			// Ri
			memcpy(&aesBuf[8], &buf[8 * i], 8);
			// A mod t
			aesBuf[7] ^= ((n * j) + i);

			// do decrypt
			sk->decryptInit(false, XSECCryptoSymmetricKey::MODE_ECB);		// No padding
			int sz = sk->decrypt(aesBuf, aesOutBuf, 16, 16);
			sz += sk->decryptFinish(&aesOutBuf[sz], 16 - sz);

			if (sz != 16) {
				throw XSECException(XSECException::CipherError, 
					"XENCAlgorithmHandlerDefault - Error performing decrypt in AES Unwrap");
			}

			// Copy back to where we are
			// A
			memcpy(buf, aesOutBuf, 8);
			// Ri
			memcpy(&buf[8 * i], &aesOutBuf[8], 8);

		}
	}

	// Check is valid
	if (memcmp(buf, s_AES_IV, 8) != 0) {
		throw XSECException(XSECException::CipherError, 
			"XENCAlgorithmHandlerDefault - decrypt failed - AES IV is not correct");
	}

	// Copy to safebuffer
	result.sbMemcpyIn(&buf[8], n * 8);

	return n * 8;
}

bool XENCAlgorithmHandlerDefault::wrapKeyAES(
   		TXFMChain * cipherText,
		XSECCryptoKey * key,
		safeBuffer & result) {

	// get the raw key
	XMLByte buf[_MY_MAX_KEY_SIZE + 8];
	memcpy(buf, s_AES_IV, 8);
	XMLByte aesBuf[16];
	XMLByte aesOutBuf[32];  // Give this an extra block for WinCAPI
	TXFMBase * b = cipherText->getLastTxfm();
	unsigned int sz = (unsigned int) b->readBytes(&buf[8], _MY_MAX_KEY_SIZE);

	if (sz <= 0) {
		throw XSECException(XSECException::CipherError, 
			"XENCAlgorithmHandlerDefault - Key not found");
	}

	if (sz == _MY_MAX_KEY_SIZE) {
		throw XSECException(XSECException::CipherError, 
			"XENCAlgorithmHandlerDefault - Key to encrypt too big!");
	}

	// Find number of blocks, and ensure we are a multiple of 64 bits
	if (sz % 8 != 0) {
		throw XSECException(XSECException::CipherError, 
			"XENCAlgorithmHandlerDefault - AES wrapped key not a multiple of 64");
	}

	// Do the decrypt - this cast will throw if wrong, but we should
	// not have been able to get through algorithm checks otherwise
	XSECCryptoSymmetricKey * sk = (XSECCryptoSymmetricKey *) key;

	int n = sz / 8;

	for (int j = 0; j <= 5; ++j) {
		for (int i = 1 ; i <= n ; ++i) {

			// Gather blocks to decrypt
			// A
			memcpy(aesBuf, buf, 8);
			// Ri
			memcpy(&aesBuf[8], &buf[8 * i], 8);

			// do encrypt
			sk->encryptInit(false, XSECCryptoSymmetricKey::MODE_ECB);
			int sz = sk->encrypt(aesBuf, aesOutBuf, 16, 32);
			sz += sk->encryptFinish(&aesOutBuf[sz], 32 - sz);

			if (sz != 16) {
				throw XSECException(XSECException::CipherError, 
					"XENCAlgorithmHandlerDefault - Error performing encrypt in AES wrap");
			}

			// Copy back to where we are
			// A
			memcpy(buf, aesOutBuf, 8);
			// A mod t
			buf[7] ^= ((n * j) + i);
			// Ri
			memcpy(&buf[8 * i], &aesOutBuf[8], 8);

		}
	}

	// Now we have to base64 encode
	XSECCryptoBase64 * b64 = XSECPlatformUtils::g_cryptoProvider->base64();

	if (!b64) {

		throw XSECException(XSECException::CryptoProviderError, 
				"XENCAlgorithmHandlerDefault - Error getting base64 encoder in AES wrap");

	}

	Janitor<XSECCryptoBase64> j_b64(b64);
	unsigned char * b64Buffer;
	int bufLen = ((n + 1) * 8) * 3;
	XSECnew(b64Buffer, unsigned char[bufLen + 1]);// Overkill
	ArrayJanitor<unsigned char> j_b64Buffer(b64Buffer);

	b64->encodeInit();
	int outputLen = b64->encode (buf, (n+1) * 8, b64Buffer, bufLen);
	outputLen += b64->encodeFinish(&b64Buffer[outputLen], bufLen - outputLen);
	b64Buffer[outputLen] = '\0';

	// Copy to safebuffer
	result.sbStrcpyIn((const char *) b64Buffer);

	return true;
}

// --------------------------------------------------------------------------------
//			DES CMS Key wrap/unwrap
// --------------------------------------------------------------------------------

unsigned int XENCAlgorithmHandlerDefault::unwrapKey3DES(
   		TXFMChain * cipherText,
		XSECCryptoKey * key,
		safeBuffer & result) {

	// Perform an unwrap on the key
	safeBuffer cipherSB;

	// Cat the encrypted key
	XMLByte buf[_MY_MAX_KEY_SIZE];
	TXFMBase * b = cipherText->getLastTxfm();
	unsigned int offset = 0;
	unsigned int sz = (unsigned int) b->readBytes(buf, _MY_MAX_KEY_SIZE);

	while (sz > 0) {
		cipherSB.sbMemcpyIn(offset, buf, sz);
		offset += sz;
		sz = (unsigned int) b->readBytes(buf, _MY_MAX_KEY_SIZE);
	}

	if (offset > _MY_MAX_KEY_SIZE) {
		throw XSECException(XSECException::CipherError, 
			"XENCAlgorithmHandlerDefault - Key to decrypt too big!");
	}

	// Do the decrypt - this cast will throw if wrong, but we should
	// not have been able to get through algorithm checks otherwise
	XSECCryptoSymmetricKey * sk = (XSECCryptoSymmetricKey *) key;

	sk->decryptInit(false, XSECCryptoSymmetricKey::MODE_CBC, s_3DES_CMS_IV);
	// If key is bigger than this, then we have a problem
	sz = sk->decrypt(cipherSB.rawBuffer(), buf, offset, _MY_MAX_KEY_SIZE);

	sz += sk->decryptFinish(&buf[sz], _MY_MAX_KEY_SIZE - sz);

	if (sz <= 0) {
		throw XSECException(XSECException::CipherError, 
			"XENCAlgorithmHandlerDefault - Error decrypting key!");
	}

	// We now have the first cut, reverse the cipher text
	XMLByte buf2[_MY_MAX_KEY_SIZE];
	for (unsigned int i = 0; i < sz; ++ i) {
		buf2[sz - i - 1] = buf[i];
	}

	// decrypt again
	sk->decryptInit(false);
	offset = sk->decrypt(buf2, buf, sz, _MY_MAX_KEY_SIZE);
	offset += sk->decryptFinish(&buf[offset], _MY_MAX_KEY_SIZE - offset);

	// Calculate the CMS Key Checksum
	XSECCryptoHash * sha1 = XSECPlatformUtils::g_cryptoProvider->hashSHA1();
	if (!sha1) {

		throw XSECException(XSECException::CryptoProviderError, 
				"XENCAlgorithmHandlerDefault - Error getting SHA-1 object in 3DES unwrap");

	}
	Janitor<XSECCryptoHash> j_sha1(sha1);
	sha1->reset();
	sha1->hash(buf, offset - 8);
	sha1->finish(buf2, _MY_MAX_KEY_SIZE);

	// Compare
	for (unsigned int j = 0; j < 8; ++j) {

		if (buf[offset - 8 + j] != buf2[j]) {
			throw XSECException(XSECException::CipherError, 
				"XENCAlgorithmHandlerDefault::unwrapKey3DES - CMS Key Checksum does not match");
		}
	}

	result.sbMemcpyIn(buf, offset - 8);

	return offset - 8;

}

bool XENCAlgorithmHandlerDefault::wrapKey3DES(
   		TXFMChain * cipherText,
		XSECCryptoKey * key,
		safeBuffer & result) {

	// Cat the plaintext key
	XMLByte buf[_MY_MAX_KEY_SIZE + 16];
	TXFMBase * b = cipherText->getLastTxfm();
	int offset = 0;
	unsigned int sz = (unsigned int) b->readBytes(buf, _MY_MAX_KEY_SIZE);

	if (sz <= 0) {
		throw XSECException(XSECException::CipherError, 
			"XENCAlgorithmHandlerDefault::wrapKey3DES - Unable to read key");
	}

	if (sz >= _MY_MAX_KEY_SIZE) {
		throw XSECException(XSECException::CipherError, 
			"XENCAlgorithmHandlerDefault::wrapKey3DES - Key to decrypt too big!");
	}

	if (sz % 8 != 0) {
		throw XSECException(XSECException::CipherError, 
			"XENCAlgorithmHandlerDefault::wrapKey3DES - Key to encrypt not a multiple of 8 bytes");
	}

	// Calculate the CMS Key Checksum

	// Do the first encrypt
	XMLByte buf2[_MY_MAX_KEY_SIZE + 16];

	XSECCryptoHash * sha1 = XSECPlatformUtils::g_cryptoProvider->hashSHA1();
	if (!sha1) {

		throw XSECException(XSECException::CryptoProviderError, 
				"XENCAlgorithmHandlerDefault - Error getting SHA-1 object in 3DES wrap");

	}
	Janitor<XSECCryptoHash> j_sha1(sha1);
	sha1->reset();
	sha1->hash(buf, sz);
	sha1->finish(buf2, _MY_MAX_KEY_SIZE);

	for (int j = 0; j < 8 ; ++j)
		buf[sz++] = buf2[j];

	// Do the first encrypt - this cast will throw if wrong, but we should
	// not have been able to get through algorithm checks otherwise
	XSECCryptoSymmetricKey * sk = (XSECCryptoSymmetricKey *) key;

	sk->encryptInit(false);
	// If key is bigger than this, then we have a problem
	sz = sk->encrypt(buf, buf2, sz, _MY_MAX_KEY_SIZE);
	sz += sk->encryptFinish(&buf2[sz], _MY_MAX_KEY_SIZE - sz);

	if (sz <= 0) {
		throw XSECException(XSECException::CipherError, 
			"XENCAlgorithmHandlerDefault::wrapKey3DES - Error encrypting key!");
	}

	// We now have the first cut, reverse the cipher text
	for (unsigned int i = 0; i < sz; ++ i) {
		buf[sz - i - 1] = buf2[i];
	}

	// encrypt again
	sk->encryptInit(false, XSECCryptoSymmetricKey::MODE_CBC, s_3DES_CMS_IV);
	offset = sk->encrypt(buf, buf2, sz, _MY_MAX_KEY_SIZE);
	offset += sk->encryptFinish(&buf2[offset], _MY_MAX_KEY_SIZE - offset);

	// Base64 encode
	XSECCryptoBase64 * b64 = XSECPlatformUtils::g_cryptoProvider->base64();

	if (!b64) {

		throw XSECException(XSECException::CryptoProviderError, 
				"XENCAlgorithmHandlerDefault - Error getting base64 encoder in 3DES wrap");

	}

	Janitor<XSECCryptoBase64> j_b64(b64);
	unsigned char * b64Buffer;
	int bufLen = (offset + 9) * 3;
	XSECnew(b64Buffer, unsigned char[bufLen + 1]);// Overkill
	ArrayJanitor<unsigned char> j_b64Buffer(b64Buffer);

	b64->encodeInit();
	int outputLen = b64->encode (&buf2[8], offset-8, b64Buffer, bufLen);
	outputLen += b64->encodeFinish(&b64Buffer[outputLen], bufLen - outputLen);
	b64Buffer[outputLen] = '\0';

	// Copy to safebuffer
	result.sbStrcpyIn((const char *) b64Buffer);

	return true;

}

// --------------------------------------------------------------------------------
//			InputStream decryption
// --------------------------------------------------------------------------------
	
bool XENCAlgorithmHandlerDefault::appendDecryptCipherTXFM(
		TXFMChain * cipherText,
		XENCEncryptionMethod * encryptionMethod,
		XSECCryptoKey * key,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument * doc
		) {

	// We only support this for bulk Symmetric key algorithms

	XSECCryptoKey::KeyType kt;
	XSECCryptoSymmetricKey::SymmetricKeyType skt;
	bool isKeyWrap = false;
    XSECCryptoSymmetricKey::SymmetricKeyMode skm;
    unsigned int taglen;

	mapURIToKey(encryptionMethod->getAlgorithm(), key, kt, skt, isKeyWrap, skm, taglen);
    if (kt != XSECCryptoKey::KEY_SYMMETRIC || isKeyWrap == true) {
		throw XSECException(XSECException::CipherError, 
			"XENCAlgorithmHandlerDefault::appendDecryptCipherTXFM - only supports bulk symmetric algorithms");
	}

    if (skm == XSECCryptoSymmetricKey::MODE_GCM) {

        // GCM doesn't fit the pipelined model of the existing code, so we have a custom
        // routine that decrypts to a safeBuffer directly.
        safeBuffer result;
        unsigned int sz = doGCMDecryptToSafeBuffer(cipherText, key, taglen, result);

        // Now we hijack the original tansform chain with a safeBuffer-sourced link.
        TXFMSB* tsb;
        XSECnew(tsb, TXFMSB(doc));
        tsb->setInput(result, sz);
        cipherText->appendTxfm(tsb);
        result.cleanseBuffer();
        return true;
    }


	// Add the decryption TXFM
	TXFMCipher* tcipher;
	XSECnew(tcipher, TXFMCipher(doc, key, false));
	cipherText->appendTxfm(tcipher);

	return true;
}


// --------------------------------------------------------------------------------
//			GCM SafeBuffer decryption
// --------------------------------------------------------------------------------

unsigned int XENCAlgorithmHandlerDefault::doGCMDecryptToSafeBuffer(
		TXFMChain * cipherText,
		XSECCryptoKey * key,
        unsigned int taglen,
		safeBuffer & result) {

	// Only works with symmetric key
    if (key->getKeyType() != XSECCryptoKey::KEY_SYMMETRIC) {
		throw XSECException(XSECException::CipherError, 
			"XENCAlgorithmHandlerDefault - GCM Decrypt must use symmetric key");
	}

    // First read in all the data so we can get to the tag.
    safeBuffer cipherBuf("");
    XMLByte inbuf[3072];
    unsigned int szTotal = 0, sz = cipherText->getLastTxfm()->readBytes(inbuf, 3072);
    while (sz > 0) {
        cipherBuf.sbMemcpyIn(szTotal, inbuf, sz);
        szTotal += sz;
        sz = cipherText->getLastTxfm()->readBytes(inbuf, 3072);
    }

    if (szTotal <= taglen) {
		throw XSECException(XSECException::CipherError, 
			"XENCAlgorithmHandlerDefault - GCM ciphertext size not large enough to include authentication tag");
    }

    const unsigned char* cipherPtr = cipherBuf.rawBuffer();

    // Init the cipher with the tag at the end of the cipher text and omit it from later decryption.
    szTotal -= taglen;
    ((XSECCryptoSymmetricKey*) key)->decryptInit(false, XSECCryptoSymmetricKey::MODE_GCM, NULL, cipherPtr + szTotal, taglen);

    unsigned int plainOffset = 0;
    do {
        // Feed the data in at most 2048-byte chunks.
        sz = ((XSECCryptoSymmetricKey*) key)->decrypt(cipherPtr, inbuf, (szTotal > 2048 ? 2048 : szTotal), 3072);
        cipherPtr += (szTotal > 2048 ? 2048 : szTotal);
        szTotal -= (szTotal > 2048 ? 2048 : szTotal);
        if (sz > 0) {
            result.sbMemcpyIn(plainOffset, inbuf, sz);
            plainOffset += sz;
        }
    } while (szTotal > 0);

    // Wrap it up.
    sz = ((XSECCryptoSymmetricKey*) key)->decryptFinish(inbuf, 3072);
    if (sz > 0) {
        result.sbMemcpyIn(plainOffset, inbuf, sz);
        plainOffset += sz;
    }

    // In case any plaintext is left lying around.
    memset(inbuf, 0, 3072);
    return plainOffset;
}

// --------------------------------------------------------------------------------
//			RSA SafeBuffer decryption
// --------------------------------------------------------------------------------

unsigned int XENCAlgorithmHandlerDefault::doRSADecryptToSafeBuffer(
		TXFMChain * cipherText,
		XENCEncryptionMethod * encryptionMethod,
		XSECCryptoKey * key,
		DOMDocument * doc,
		safeBuffer & result) {

	// Only works with RSA_PRIVATE or PAIR
	if (key->getKeyType() == XSECCryptoKey::KEY_RSA_PUBLIC) {
		throw XSECException(XSECException::CipherError, 
			"XENCAlgorithmHandlerDefault - RSA Decrypt must use private key");
	}

	// Know this is an RSA key, so just cast

	XSECCryptoKeyRSA * rsa = (XSECCryptoKeyRSA *) key;

	// Allocate an output buffer
	unsigned char * decBuf;
	XSECnew(decBuf, unsigned char[rsa->getLength()]);
	ArrayJanitor<unsigned char> j_decBuf(decBuf);

	// Input
	TXFMBase * b = cipherText->getLastTxfm();
	safeBuffer cipherSB;
	XMLByte buf[1024];
	unsigned int offset = 0;

	unsigned int bytesRead = (unsigned int) b->readBytes(buf, 1024);
	while (bytesRead > 0) {
		cipherSB.sbMemcpyIn(offset, buf, bytesRead);
		offset += bytesRead;
		bytesRead = (unsigned int) b->readBytes(buf, 1024);
	}

	unsigned int decryptLen;

	// Now we find out what kind of padding
	if (strEquals(encryptionMethod->getAlgorithm(), DSIGConstants::s_unicodeStrURIRSA_1_5)) {

		// Do decrypt
		decryptLen = rsa->privateDecrypt(cipherSB.rawBuffer(), 
												  decBuf, 
												  offset, 
												  rsa->getLength(), 
												  XSECCryptoKeyRSA::PAD_PKCS_1_5, 
												  HASH_NONE);
	}
	else if (strEquals(encryptionMethod->getAlgorithm(), DSIGConstants::s_unicodeStrURIRSA_OAEP_MGFP1) ||
             strEquals(encryptionMethod->getAlgorithm(), DSIGConstants::s_unicodeStrURIRSA_OAEP)) {

        hashMethod hm;
	    const XMLCh* digmeth = encryptionMethod->getDigestMethod();

	    // Is this a URI we recognize?
	    if (!digmeth|| !*digmeth) {
	        hm = HASH_SHA1;
	    }
	    else if (!XSECmapURIToHashMethod(digmeth, hm)) {
	        safeBuffer sb;
	        sb.sbTranscodeIn("XENCAlgorithmHandlerDefault - Unknown Digest URI : ");
	        sb.sbXMLChCat(digmeth);

	        throw XSECException(XSECException::AlgorithmMapperError,
	            sb.rawXMLChBuffer());
	    }

        const XMLCh* mgfalg = encryptionMethod->getMGF();
        if (mgfalg && *mgfalg) {
            maskGenerationFunc mgf;
            if (!XSECmapURIToMaskGenerationFunc(mgfalg, mgf)) {
	            safeBuffer sb;
	            sb.sbTranscodeIn("XENCAlgorithmHandlerDefault - Unknown MGF URI : ");
	            sb.sbXMLChCat(mgfalg);

	            throw XSECException(XSECException::AlgorithmMapperError,
	                sb.rawXMLChBuffer());
            }
            rsa->setMGF(mgf);
        }

		// Read out any OAEP params
		unsigned char * oaepParamsBuf = NULL;
		const XMLCh * oaepParams = encryptionMethod->getOAEPparams();
		unsigned int sz = 0;
		if (oaepParams != NULL) {

			XSECAutoPtrChar oaepParamsStr(oaepParams);

			unsigned int bufLen = (unsigned int) strlen(oaepParamsStr.get());
			oaepParamsBuf = new unsigned char[bufLen];
			ArrayJanitor<unsigned char> j_oaepParamsBuf(oaepParamsBuf);

			XSECCryptoBase64 * b64 = 
				XSECPlatformUtils::g_cryptoProvider->base64();
			Janitor<XSECCryptoBase64> j_b64(b64);

			b64->decodeInit();
			sz = b64->decode((unsigned char *) oaepParamsStr.get(), bufLen, oaepParamsBuf, bufLen);
			sz += b64->decodeFinish(&oaepParamsBuf[sz], bufLen - sz);

			rsa->setOAEPparams(oaepParamsBuf, sz);

		}
		else
			rsa->setOAEPparams(NULL, 0);

		decryptLen = rsa->privateDecrypt(cipherSB.rawBuffer(), 
												  decBuf, 
												  offset, 
												  rsa->getLength(), 
												  XSECCryptoKeyRSA::PAD_OAEP_MGFP1, 
												  hm);

	}

	else {
		throw XSECException(XSECException::CipherError, 
			"XENCAlgorithmHandlerDefault::doRSADecryptToSafeBuffer - Unknown padding type");
	}
	// Copy to output
	result.sbMemcpyIn(decBuf, decryptLen);
	
	memset(decBuf, 0, decryptLen);

	return decryptLen;

}

// --------------------------------------------------------------------------------
//			SafeBuffer decryption
// --------------------------------------------------------------------------------

unsigned int XENCAlgorithmHandlerDefault::decryptToSafeBuffer(
		TXFMChain * cipherText,
		XENCEncryptionMethod * encryptionMethod,
		XSECCryptoKey * key,
		DOMDocument * doc,
		safeBuffer & result
		) {

	XSECCryptoKey::KeyType kt;
	XSECCryptoSymmetricKey::SymmetricKeyType skt;
	bool isKeyWrap = false;
    XSECCryptoSymmetricKey::SymmetricKeyMode skm;
    unsigned int taglen;

	if (encryptionMethod == NULL) {
		throw XSECException(XSECException::CipherError,
			"XENCAlgorithmHandlerDefault::decryptToSafeBuffer - Cannot operate with NULL encryption Method");
	}


	// Check the uri against the key type
	mapURIToKey(encryptionMethod->getAlgorithm(), key, kt, skt, isKeyWrap, skm, taglen);

	// RSA?
	if (kt == XSECCryptoKey::KEY_RSA_PAIR || 
		kt == XSECCryptoKey::KEY_RSA_PUBLIC || 
		kt == XSECCryptoKey::KEY_RSA_PRIVATE) {

		return doRSADecryptToSafeBuffer(cipherText, encryptionMethod, key, doc, result);

	}

	// Ensure is symmetric before we continue
	if (kt != XSECCryptoKey::KEY_SYMMETRIC) {
		throw XSECException(XSECException::CipherError,
			"XENCAlgorithmHandlerDefault::decryptToSafeBuffer - Not an RSA key, but not symmetric");
	}

	// Key wrap?

	if (isKeyWrap == true) {

		if (skt == XSECCryptoSymmetricKey::KEY_AES_128 ||
			skt == XSECCryptoSymmetricKey::KEY_AES_192 ||
			skt == XSECCryptoSymmetricKey::KEY_AES_256) {

			return unwrapKeyAES(cipherText, key, result);

		}

		else if (skt == XSECCryptoSymmetricKey::KEY_3DES_192) {
			return unwrapKey3DES(cipherText, key, result);
		}

		else {
			throw XSECException(XSECException::CipherError,
				"XENCAlgorithmHandlerDefault::decryptToSafeBuffer - don't know how to do key wrap for algorithm");
		}

	}

    if (skm == XSECCryptoSymmetricKey::MODE_GCM) {
        // GCM doesn't fit the pipelined model of the existing code, so we have a custom
        // routine that decrypts to a safeBuffer directly.
        return doGCMDecryptToSafeBuffer(cipherText, key, taglen, result);
    }

	// It's symmetric and it's not a key wrap or GCM, so just treat as a block algorithm.

	TXFMCipher * tcipher;
	XSECnew(tcipher, TXFMCipher(doc, key, false));

	cipherText->appendTxfm(tcipher);

	// Do the decrypt to the safeBuffer.

	result.sbStrcpyIn("");
	unsigned int offset = 0;
	XMLByte buf[1024];
	TXFMBase * b = cipherText->getLastTxfm();

	unsigned int bytesRead = (unsigned int) b->readBytes(buf, 1024);
	while (bytesRead > 0) {
		result.sbMemcpyIn(offset, buf, bytesRead);
		offset += bytesRead;
		bytesRead = (unsigned int) b->readBytes(buf, 1024);
	}

	result[offset] = '\0'; 

	return offset;

}

// --------------------------------------------------------------------------------
//			RSA SafeBuffer encryption
// --------------------------------------------------------------------------------

bool XENCAlgorithmHandlerDefault::doRSAEncryptToSafeBuffer(
		TXFMChain * plainText,
		XENCEncryptionMethod * encryptionMethod,
		XSECCryptoKey * key,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument * doc,
		safeBuffer & result
		) {

	// Only works with RSA_PRIVATE or PAIR
	if (key->getKeyType() == XSECCryptoKey::KEY_RSA_PRIVATE) {
		throw XSECException(XSECException::CipherError, 
			"XENCAlgorithmHandlerDefault - RSA Encrypt must use public key");
	}

	XSECCryptoKeyRSA * rsa = (XSECCryptoKeyRSA *) key;
	
	// Allocate an output buffer
	unsigned char * encBuf;
	XSECnew(encBuf, unsigned char[rsa->getLength()]);
	ArrayJanitor<unsigned char> j_encBuf(encBuf);

	// Input
	TXFMBase * b = plainText->getLastTxfm();
	safeBuffer plainSB;
	plainSB.isSensitive();

	XMLByte buf[1024];
	unsigned int offset = 0;

	unsigned int bytesRead = (unsigned int) b->readBytes(buf, 1024);
	while (bytesRead > 0) {
		plainSB.sbMemcpyIn(offset, buf, bytesRead);
		offset += bytesRead;
		bytesRead = (unsigned int) b->readBytes(buf, 1024);
	}

	unsigned int encryptLen;

	// Do decrypt
	if (strEquals(encryptionMethod->getAlgorithm(), DSIGConstants::s_unicodeStrURIRSA_1_5)) {
		encryptLen = rsa->publicEncrypt(plainSB.rawBuffer(), 
												  encBuf, 
												  offset, 
												  rsa->getLength(), 
												  XSECCryptoKeyRSA::PAD_PKCS_1_5, 
												  HASH_NONE);
	}

	else if (strEquals(encryptionMethod->getAlgorithm(), DSIGConstants::s_unicodeStrURIRSA_OAEP_MGFP1) ||
            strEquals(encryptionMethod->getAlgorithm(), DSIGConstants::s_unicodeStrURIRSA_OAEP)) {
        
        hashMethod hm;
        if (encryptionMethod->getDigestMethod() == NULL) {
            hm = HASH_SHA1;
		    encryptionMethod->setDigestMethod(DSIGConstants::s_unicodeStrURISHA1);
        }
        else if (!XSECmapURIToHashMethod(encryptionMethod->getDigestMethod(), hm)) {
	        safeBuffer sb;
	        sb.sbTranscodeIn("XENCAlgorithmHandlerDefault - Unknown Digest URI : ");
	        sb.sbXMLChCat(encryptionMethod->getDigestMethod());

	        throw XSECException(XSECException::AlgorithmMapperError,
	            sb.rawXMLChBuffer());
	    }

        const XMLCh* mgfalg = encryptionMethod->getMGF();
        if (mgfalg && *mgfalg) {
            maskGenerationFunc mgf;
            if (!XSECmapURIToMaskGenerationFunc(mgfalg, mgf)) {
	            safeBuffer sb;
	            sb.sbTranscodeIn("XENCAlgorithmHandlerDefault - Unknown MGF URI : ");
	            sb.sbXMLChCat(mgfalg);

	            throw XSECException(XSECException::AlgorithmMapperError,
	                sb.rawXMLChBuffer());
            }
            rsa->setMGF(mgf);
        }
        else if (rsa->getMGF() != MGF1_SHA1) {
            safeBuffer sb;
            if (maskGenerationFunc2URI(sb, rsa->getMGF()))
                encryptionMethod->setMGF(sb.rawXMLChBuffer());
        }

		// Check for OAEP params
		int oaepParamsLen = rsa->getOAEPparamsLen();
		if (oaepParamsLen > 0) {
			unsigned char * oaepParamsB64;
			XSECnew(oaepParamsB64, unsigned char[oaepParamsLen * 2]);
			ArrayJanitor<unsigned char> j_oaepParamsB64(oaepParamsB64);

			XSECCryptoBase64 * b64 = 
				XSECPlatformUtils::g_cryptoProvider->base64();
			Janitor<XSECCryptoBase64> j_b64(b64);

			b64->encodeInit();
			int sz = b64->encode(rsa->getOAEPparams(), oaepParamsLen, oaepParamsB64, oaepParamsLen *2);
			sz += b64->encodeFinish(&oaepParamsB64[sz], (oaepParamsLen * 2)  - sz);
			oaepParamsB64[sz] = '\0';

			XSECAutoPtrXMLCh xBuf((char *) oaepParamsB64);

			encryptionMethod->setOAEPparams(xBuf.get());

		}

		encryptLen = rsa->publicEncrypt(plainSB.rawBuffer(), 
										  encBuf, 
										  offset, 
										  rsa->getLength(), 
										  XSECCryptoKeyRSA::PAD_OAEP_MGFP1, 
										  hm);

	}
	else {
		throw XSECException(XSECException::CipherError, 
			"XENCAlgorithmHandlerDefault::doRSAEncryptToSafeBuffer - Unknown padding type");
	}

	// Now need to base64 encode
	XSECCryptoBase64 * b64 = 
		XSECPlatformUtils::g_cryptoProvider->base64();
	Janitor<XSECCryptoBase64> j_b64(b64);

	b64->encodeInit();
	encryptLen = b64->encode(encBuf, encryptLen, buf, 1024);
	result.sbMemcpyIn(buf, encryptLen);
	unsigned int finalLen = b64->encodeFinish(buf, 1024);
	result.sbMemcpyIn(encryptLen, buf, finalLen);
	result[encryptLen + finalLen] = '\0';

	// This is a string, so set the buffer correctly
	result.setBufferType(safeBuffer::BUFFER_CHAR);

	return true;

}


// --------------------------------------------------------------------------------
//			SafeBuffer encryption
// --------------------------------------------------------------------------------

bool XENCAlgorithmHandlerDefault::encryptToSafeBuffer(
		TXFMChain * plainText,
		XENCEncryptionMethod * encryptionMethod,
		XSECCryptoKey * key,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument * doc,
		safeBuffer & result
		) {

	XSECCryptoKey::KeyType kt;
	XSECCryptoSymmetricKey::SymmetricKeyType skt;
	bool isKeyWrap = false;
    XSECCryptoSymmetricKey::SymmetricKeyMode skm;
    unsigned int taglen;

	if (encryptionMethod == NULL) {
		throw XSECException(XSECException::CipherError,
			"XENCAlgorithmHandlerDefault::encryptToSafeBuffer - Cannot operate with NULL encryption Method");
	}


	// Check the uri against the key type
	mapURIToKey(encryptionMethod->getAlgorithm(), key, kt, skt, isKeyWrap, skm, taglen);

	// RSA?
	if (kt == XSECCryptoKey::KEY_RSA_PRIVATE || 
		kt == XSECCryptoKey::KEY_RSA_PUBLIC || 
		kt == XSECCryptoKey::KEY_RSA_PAIR) {

		return doRSAEncryptToSafeBuffer(plainText, encryptionMethod, key, doc, result);

	}

	// Ensure is symmetric before we continue
	if (kt != XSECCryptoKey::KEY_SYMMETRIC) {
		throw XSECException(XSECException::CipherError,
			"XENCAlgorithmHandlerDefault::encryptToSafeBuffer - Not an RSA key, but not symmetric");
	}

	if (isKeyWrap == true) {

		if (skt == XSECCryptoSymmetricKey::KEY_AES_128 ||
			skt == XSECCryptoSymmetricKey::KEY_AES_192 ||
			skt == XSECCryptoSymmetricKey::KEY_AES_256) {

			return wrapKeyAES(plainText, key, result);

		}

		if (skt == XSECCryptoSymmetricKey::KEY_3DES_192) {
			return wrapKey3DES(plainText, key, result);
		}

		else {
			throw XSECException(XSECException::CipherError,
				"XENCAlgorithmHandlerDefault::decryptToSafeBuffer - don't know how to do key wrap for algorithm");
		}

	}
	
	// Must be bulk symmetric - do the encryption

	TXFMCipher *tcipher;
	XSECnew(tcipher, TXFMCipher(doc, key, true, skm, taglen));
	plainText->appendTxfm(tcipher);

	// Transform to Base64
	TXFMBase64 * tb64;
	XSECnew(tb64, TXFMBase64(doc, false));
	plainText->appendTxfm(tb64);

	// Read into the safeBuffer
	result = "";

	result << plainText->getLastTxfm();

	return true;

}
// --------------------------------------------------------------------------------
//			Key Creation
// --------------------------------------------------------------------------------

XSECCryptoKey * XENCAlgorithmHandlerDefault::createKeyForURI(
		const XMLCh * uri,
		const unsigned char * keyBuffer,
		unsigned int keyLen
		) {

	XSECCryptoSymmetricKey * sk = NULL;

	if (strEquals(uri, DSIGConstants::s_unicodeStrURI3DES_CBC)) {
        if (keyLen < 192 / 8)
            throw XSECException(XSECException::CipherError, 
		        "XENCAlgorithmHandlerDefault - key size was invalid");
		sk = XSECPlatformUtils::g_cryptoProvider->keySymmetric(XSECCryptoSymmetricKey::KEY_3DES_192);
	}
	else if (strEquals(uri, DSIGConstants::s_unicodeStrURIAES128_CBC) || strEquals(uri, DSIGConstants::s_unicodeStrURIAES128_GCM)) {
        if (keyLen < 128 / 8)
            throw XSECException(XSECException::CipherError, 
		        "XENCAlgorithmHandlerDefault - key size was invalid");
		sk = XSECPlatformUtils::g_cryptoProvider->keySymmetric(XSECCryptoSymmetricKey::KEY_AES_128);
	}
	else if (strEquals(uri, DSIGConstants::s_unicodeStrURIAES192_CBC) || strEquals(uri, DSIGConstants::s_unicodeStrURIAES192_GCM)) {
        if (keyLen < 192 / 8)
            throw XSECException(XSECException::CipherError, 
		        "XENCAlgorithmHandlerDefault - key size was invalid");
		sk = XSECPlatformUtils::g_cryptoProvider->keySymmetric(XSECCryptoSymmetricKey::KEY_AES_192);
	}
	else if (strEquals(uri, DSIGConstants::s_unicodeStrURIAES256_CBC) || strEquals(uri, DSIGConstants::s_unicodeStrURIAES256_GCM)) {
        if (keyLen < 256 / 8)
            throw XSECException(XSECException::CipherError, 
		        "XENCAlgorithmHandlerDefault - key size was invalid");
		sk = XSECPlatformUtils::g_cryptoProvider->keySymmetric(XSECCryptoSymmetricKey::KEY_AES_256);
	}

	if (sk != NULL) {
		sk->setKey(keyBuffer, keyLen);
		return sk;
	}

	throw XSECException(XSECException::CipherError, 
		"XENCAlgorithmHandlerDefault - URI Provided, but cannot create associated key");

}


// --------------------------------------------------------------------------------
//			Clone
// --------------------------------------------------------------------------------

XSECAlgorithmHandler * XENCAlgorithmHandlerDefault::clone(void) const {

	XENCAlgorithmHandlerDefault * ret;
	XSECnew(ret, XENCAlgorithmHandlerDefault);

	return ret;

}

// --------------------------------------------------------------------------------
//			Unsupported operations
// --------------------------------------------------------------------------------

unsigned int XENCAlgorithmHandlerDefault::signToSafeBuffer(
		TXFMChain * inputBytes,
		const XMLCh * URI,
		XSECCryptoKey * key,
		unsigned int outputLength,
		safeBuffer & result) {

	throw XSECException(XSECException::AlgorithmMapperError, 
			"XENCAlgorithmHandlerDefault - Signature operations not supported");

}


bool XENCAlgorithmHandlerDefault::appendSignatureHashTxfm(
		TXFMChain * inputBytes,
		const XMLCh * URI,
		XSECCryptoKey * key) {

	throw XSECException(XSECException::AlgorithmMapperError, 
			"XENCAlgorithmHandlerDefault - Signature operations not supported");

}

bool XENCAlgorithmHandlerDefault::verifyBase64Signature(
		TXFMChain * inputBytes,
		const XMLCh * URI,
		const char * sig,
		unsigned int outputLength,
		XSECCryptoKey * key) {

	throw XSECException(XSECException::AlgorithmMapperError, 
			"XENCAlgorithmHandlerDefault - Signature operations not supported");

}

bool XENCAlgorithmHandlerDefault::appendHashTxfm(
		TXFMChain * inputBytes,
		const XMLCh * URI) {

	throw XSECException(XSECException::AlgorithmMapperError, 
			"XENCAlgorithmHandlerDefault - Hash operations not supported");

}

