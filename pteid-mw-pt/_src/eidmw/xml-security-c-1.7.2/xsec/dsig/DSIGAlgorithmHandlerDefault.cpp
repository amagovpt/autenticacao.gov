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
 * $Id: DSIGAlgorithmHandlerDefault.cpp 1493960 2013-06-17 22:27:28Z scantor $
 *
 */

// XSEC Includes

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/transformers/TXFMChain.hpp>
#include <xsec/transformers/TXFMBase64.hpp>
#include <xsec/transformers/TXFMSHA1.hpp>
#include <xsec/transformers/TXFMMD5.hpp>
#include <xsec/enc/XSECCryptoKey.hpp>
#include <xsec/framework/XSECError.hpp>
#include <xsec/utils/XSECDOMUtils.hpp>

#include <xsec/dsig/DSIGAlgorithmHandlerDefault.hpp>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/Janitor.hpp>

XERCES_CPP_NAMESPACE_USE

#define MAXB64BUFSIZE 2048

// --------------------------------------------------------------------------------
//           Some useful utility functions
// --------------------------------------------------------------------------------


bool compareBase64StringToRaw(const char * b64Str, 
							  unsigned char * raw, 
							  unsigned int rawLen, 
							  unsigned int maxCompare = 0) {
	// Decode a base64 buffer and then compare the result to a raw buffer
	// Compare at most maxCompare bits (if maxCompare > 0)
	// Note - whilst the other parameters are bytes, maxCompare is bits

	// The div function below takes signed int, so make sure the value
	// is safe to cast.
	if ((int) maxCompare < 0) {

		throw XSECException(XSECException::CryptoProviderError, 
				"Comparison length was unsafe");

	}

	unsigned char outputStr[MAXB64BUFSIZE];
	unsigned int outputLen = 0;
	
	XSECCryptoBase64 * b64 = XSECPlatformUtils::g_cryptoProvider->base64();
	
	if (!b64) {

		throw XSECException(XSECException::CryptoProviderError, 
				"Error requesting Base64 object from Crypto Provider");

	}

	Janitor<XSECCryptoBase64> j_b64(b64);

	b64->decodeInit();
	outputLen = b64->decode((unsigned char *) b64Str, (unsigned int) strlen((char *) b64Str), outputStr, MAXB64BUFSIZE);
	outputLen += b64->decodeFinish(&outputStr[outputLen], MAXB64BUFSIZE - outputLen);

	// Compare

	div_t d;
    d.rem = 0;
    d.quot = 0;

	unsigned int maxCompareBytes, maxCompareBits;
	maxCompareBits = 0;

	unsigned int size;

	if (maxCompare > 0) {
		d = div(maxCompare, 8);
		maxCompareBytes = d.quot;
		if (d.rem != 0)
			maxCompareBytes++;

		if (rawLen < maxCompareBytes && outputLen < maxCompareBytes) {
			if (rawLen != outputLen)
				return false;
			size = rawLen;
		}
		else if (rawLen < maxCompareBytes || outputLen < maxCompareBytes) {
			return false;
		}
		else
			size = maxCompareBytes;
	}
	else {

		if (rawLen != outputLen)
			return false;

		size = rawLen;

	}

	// Compare bytes
	unsigned int i, j;
	for (i = 0; i < size; ++ i) {
		if (raw[i] != outputStr[i])
			return false;
	}

	// Compare bits

	char mask = 0x01;
	if (maxCompare != 0) {
	    for (j = 0 ; j < (unsigned int) d.rem; ++j) {

		    if ((raw[i] & mask) != (outputStr[i] & mask))
			    return false;

			mask = mask << 1;
		}
	}

	return true;

}


void convertRawToBase64String(safeBuffer &b64SB, 
							  unsigned char * raw, 
							  unsigned int rawLen, 
							  unsigned int maxBits = 0) {

	// Translate the rawbuffer (at most maxBits or rawLen - whichever is smaller)
	// to a base64 string

	unsigned char b64Str[MAXB64BUFSIZE];
	unsigned int outputLen = 0;
	
	XSECCryptoBase64 * b64 = XSECPlatformUtils::g_cryptoProvider->base64();
	
	if (!b64) {

		throw XSECException(XSECException::CryptoProviderError, 
				"Error requesting Base64 object from Crypto Provider");

	}

	Janitor<XSECCryptoBase64> j_b64(b64);

	// Determine length to translate
	unsigned int size;

	if (maxBits > 0) {
		div_t d = div(maxBits, 8);
		size = d.quot;
		if (d.rem != 0)
			++size;
		
		if (size > rawLen)
			size = rawLen;
	}

	else
		size = rawLen;

	b64->encodeInit();
	outputLen = b64->encode((unsigned char *) raw, rawLen, b64Str, MAXB64BUFSIZE - 1);
	outputLen += b64->encodeFinish(&b64Str[outputLen], MAXB64BUFSIZE - outputLen - 1);
	b64Str[outputLen] = '\0';

	// Copy out

	b64SB.sbStrcpyIn((char *) b64Str);

}

// --------------------------------------------------------------------------------
//			Clone
// --------------------------------------------------------------------------------

XSECAlgorithmHandler * DSIGAlgorithmHandlerDefault::clone(void) const {

	DSIGAlgorithmHandlerDefault * ret;
	XSECnew(ret, DSIGAlgorithmHandlerDefault);

	return ret;

}
// --------------------------------------------------------------------------------
//			Add a hash txfm
// --------------------------------------------------------------------------------

TXFMBase * addHashTxfm(signatureMethod sm, hashMethod hm, XSECCryptoKey * key, 
					   DOMDocument * doc) {

	// Given a hash method and signature method, create an appropriate TXFM

	TXFMBase * txfm;
	
	switch (hm) {

	case HASH_MD5 :

		if (sm == SIGNATURE_HMAC){
			if (key->getKeyType() != XSECCryptoKey::KEY_HMAC) {
				throw XSECException(XSECException::AlgorithmMapperError,
					"DSIGAlgorithmHandlerDefault::addHashTxfm - non HMAC key passed in to HMAC signature");
			}
			XSECnew(txfm, TXFMMD5(doc, key));
		}
		else  {
			XSECnew(txfm, TXFMMD5(doc));
		}

		break;

	case HASH_SHA1 :

		if (sm == SIGNATURE_HMAC){
			if (key->getKeyType() != XSECCryptoKey::KEY_HMAC) {
				throw XSECException(XSECException::AlgorithmMapperError,
					"DSIGAlgorithmHandlerDefault::addHashTxfm - non HMAC key passed in to HMAC signature");
			}
			XSECnew(txfm, TXFMSHA1(doc, HASH_SHA1, key));
		}
		else  {
			XSECnew(txfm, TXFMSHA1(doc));
		}

		break;

	case HASH_SHA224 :

		if (sm == SIGNATURE_HMAC){
			if (key->getKeyType() != XSECCryptoKey::KEY_HMAC) {
				throw XSECException(XSECException::AlgorithmMapperError,
					"DSIGAlgorithmHandlerDefault::addHashTxfm - non HMAC key passed in to HMAC signature");
			}
			XSECnew(txfm, TXFMSHA1(doc, HASH_SHA224, key));
		}
		else  {
			XSECnew(txfm, TXFMSHA1(doc, HASH_SHA224));
		}

		break;

	case HASH_SHA256 :

		if (sm == SIGNATURE_HMAC){
			if (key->getKeyType() != XSECCryptoKey::KEY_HMAC) {
				throw XSECException(XSECException::AlgorithmMapperError,
					"DSIGAlgorithmHandlerDefault::addHashTxfm - non HMAC key passed in to HMAC signature");
			}
			XSECnew(txfm, TXFMSHA1(doc, HASH_SHA256, key));
		}
		else  {
			XSECnew(txfm, TXFMSHA1(doc, HASH_SHA256));
		}

		break;

	case HASH_SHA384 :

		if (sm == SIGNATURE_HMAC){
			if (key->getKeyType() != XSECCryptoKey::KEY_HMAC) {
				throw XSECException(XSECException::AlgorithmMapperError,
					"DSIGAlgorithmHandlerDefault::addHashTxfm - non HMAC key passed in to HMAC signature");
			}
			XSECnew(txfm, TXFMSHA1(doc, HASH_SHA384, key));
		}
		else  {
			XSECnew(txfm, TXFMSHA1(doc, HASH_SHA384));
		}

		break;

	case HASH_SHA512 :

		if (sm == SIGNATURE_HMAC){
			if (key->getKeyType() != XSECCryptoKey::KEY_HMAC) {
				throw XSECException(XSECException::AlgorithmMapperError,
					"DSIGAlgorithmHandlerDefault::addHashTxfm - non HMAC key passed in to HMAC signature");
			}
			XSECnew(txfm, TXFMSHA1(doc, HASH_SHA512, key));
		}
		else  {
			XSECnew(txfm, TXFMSHA1(doc, HASH_SHA512));
		}

		break;

	default :

			throw XSECException(XSECException::AlgorithmMapperError,
				"Hash method unknown in DSIGAlgorithmHandlerDefault::addHashTxfm");

	}

	return txfm;
}

// --------------------------------------------------------------------------------
//			Map a Signature hash
// --------------------------------------------------------------------------------

bool DSIGAlgorithmHandlerDefault::appendSignatureHashTxfm(TXFMChain * inputBytes,
														  const XMLCh * URI,
														  XSECCryptoKey * key) {

	signatureMethod sm;
	hashMethod hm;

	// Map to internal constants

	if (!XSECmapURIToSignatureMethods(URI, sm, hm)) {
		safeBuffer sb;
		sb.sbTranscodeIn("DSIGAlgorithmHandlerDefault - Unknown URI : ");
		sb.sbXMLChCat(URI);

		throw XSECException(XSECException::AlgorithmMapperError, 
			sb.rawXMLChBuffer());
	}

	// Now append the appropriate hash transform onto the end of the chain
	// If this is an HMAC of some kind - this function will add the appropriate key

	TXFMBase * htxfm = addHashTxfm(sm, hm, key, inputBytes->getLastTxfm()->getDocument());
	inputBytes->appendTxfm(htxfm);

	return true;

}


// --------------------------------------------------------------------------------
//			Sign
// --------------------------------------------------------------------------------

unsigned int DSIGAlgorithmHandlerDefault::signToSafeBuffer(
		TXFMChain * inputBytes,
		const XMLCh * URI,
		XSECCryptoKey * key,
		unsigned int outputLength,
		safeBuffer & result) {

	signatureMethod sm;
	hashMethod hm;

	// Map to internal constants

	if (!XSECmapURIToSignatureMethods(URI, sm, hm)) {
		safeBuffer sb;
		sb.sbTranscodeIn("DSIGAlgorithmHandlerDefault - Unknown URI : ");
		sb.sbXMLChCat(URI);

		throw XSECException(XSECException::AlgorithmMapperError, 
			sb.rawXMLChBuffer());
	}

	// Now append the appropriate hash transform onto the end of the chain
	// If this is an HMAC of some kind - this function will add the appropriate key

	TXFMBase * htxfm = addHashTxfm(sm, hm, key, inputBytes->getLastTxfm()->getDocument());
	inputBytes->appendTxfm(htxfm);

	unsigned char hash[4096];

	int hashLen = inputBytes->getLastTxfm()->readBytes((XMLByte *) hash, 4096);
	
	// Now check the calculated hash

	// For now, use a fixed length buffer, but expand it,
	// and detect if the signature size exceeds what we can
	// handle.
	char b64Buf[MAXB64BUFSIZE];
	unsigned int b64Len;
	safeBuffer b64SB;
	
	switch (key->getKeyType()) {

	case (XSECCryptoKey::KEY_DSA_PRIVATE) :
	case (XSECCryptoKey::KEY_DSA_PAIR) :

		if (sm != SIGNATURE_DSA) {

			throw XSECException(XSECException::AlgorithmMapperError,
				"Key type does not match <SignedInfo> signature type");

		}

		b64Len = ((XSECCryptoKeyDSA *) key)->signBase64Signature(
			hash, 
			hashLen,
			(char *) b64Buf, 
			MAXB64BUFSIZE);

		if (b64Len <= 0) {

			throw XSECException(XSECException::AlgorithmMapperError,
				"Unknown error occured during a DSA Signing operation");

		}
		else if (b64Len >= MAXB64BUFSIZE) {

            throw XSECException(XSECException::AlgorithmMapperError,
                "DSA Signing operation exceeded size of buffer");

		}

		if (b64Buf[b64Len-1] == '\n')
			b64Buf[b64Len-1] = '\0';
		else
			b64Buf[b64Len] = '\0';

		break;

	case (XSECCryptoKey::KEY_RSA_PRIVATE) :
	case (XSECCryptoKey::KEY_RSA_PAIR) :

		if (sm != SIGNATURE_RSA) {

			throw XSECException(XSECException::AlgorithmMapperError,
				"Key type does not match <SignedInfo> signature type");

		}

		b64Len = ((XSECCryptoKeyRSA *) key)->signSHA1PKCS1Base64Signature(
			hash, 
			hashLen,
			(char *) b64Buf, 
			MAXB64BUFSIZE,
			hm);

		if (b64Len <= 0) {

			throw XSECException(XSECException::AlgorithmMapperError,
				"Unknown error occured during a RSA Signing operation");

		}
        else if (b64Len >= MAXB64BUFSIZE) {

            throw XSECException(XSECException::AlgorithmMapperError,
                "RSA Signing operation exceeded size of buffer");

        }

		// Clean up some "funnies" and make sure the string is NULL terminated

		if (b64Buf[b64Len-1] == '\n')
			b64Buf[b64Len-1] = '\0';
		else
			b64Buf[b64Len] = '\0';

		break;

	case (XSECCryptoKey::KEY_EC_PRIVATE) :
	case (XSECCryptoKey::KEY_EC_PAIR) :

		if (sm != SIGNATURE_ECDSA) {

			throw XSECException(XSECException::AlgorithmMapperError,
				"Key type does not match <SignedInfo> signature type");

		}

		b64Len = ((XSECCryptoKeyEC *) key)->signBase64SignatureDSA(
			hash, 
			hashLen,
			(char *) b64Buf, 
			MAXB64BUFSIZE);

		if (b64Len <= 0) {

			throw XSECException(XSECException::AlgorithmMapperError,
				"Unknown error occured during an ECDSA Signing operation");

		}
        else if (b64Len >= MAXB64BUFSIZE) {

            throw XSECException(XSECException::AlgorithmMapperError,
                "ECDSA Signing operation exceeded size of buffer");

        }

		if (b64Buf[b64Len-1] == '\n')
			b64Buf[b64Len-1] = '\0';
		else
			b64Buf[b64Len] = '\0';

		break;

	case (XSECCryptoKey::KEY_HMAC) :

		if (sm != SIGNATURE_HMAC) {

			throw XSECException(XSECException::AlgorithmMapperError,
				"Key type does not match <SignedInfo> signature type");

		}

		// Signature already created, so just translate to base 64 and enter string

        // FIX: CVE-2009-0217
        if (outputLength > 0 && (outputLength > hashLen || outputLength < 80 || outputLength < hashLen / 2)) {
            throw XSECException(XSECException::AlgorithmMapperError,
                "HMACOutputLength set to unsafe value.");
        }
		
		convertRawToBase64String(b64SB, 
								hash, 
								hashLen, 
								outputLength);
		
		strncpy(b64Buf, (char *) b64SB.rawBuffer(), MAXB64BUFSIZE);
		break;

	default :

		throw XSECException(XSECException::AlgorithmMapperError,
			"Key found, but don't know how to sign the document using it");

	}

	result = b64Buf;

	return (unsigned int) strlen(b64Buf);

}

// --------------------------------------------------------------------------------
//			Verify
// --------------------------------------------------------------------------------
bool DSIGAlgorithmHandlerDefault::verifyBase64Signature(
		TXFMChain * inputBytes,
		const XMLCh * URI,
		const char * sig,
		unsigned int outputLength,
		XSECCryptoKey * key) {

	signatureMethod sm;
	hashMethod hm;

	// Map to internal constants

	if (!XSECmapURIToSignatureMethods(URI, sm, hm)) {
		safeBuffer sb;
		sb.sbTranscodeIn("DSIGAlgorithmHandlerDefault - Unknown URI : ");
		sb.sbXMLChCat(URI);

		throw XSECException(XSECException::AlgorithmMapperError, 
			sb.rawXMLChBuffer());
	}

	// Now append the appropriate hash transform onto the end of the chain
	// If this is an HMAC of some kind - this function will add the appropriate key

	TXFMBase * htxfm = addHashTxfm(sm, hm, key, inputBytes->getLastTxfm()->getDocument());
	inputBytes->appendTxfm(htxfm);

	unsigned char hash[4096];

	int hashLen = inputBytes->getLastTxfm()->readBytes((XMLByte *) hash, 4096);
	
	// Now check the calculated hash
	bool sigVfyRet = false;

	switch (key->getKeyType()) {

	case (XSECCryptoKey::KEY_DSA_PUBLIC) :
	case (XSECCryptoKey::KEY_DSA_PAIR) :

		if (sm != SIGNATURE_DSA) {

			throw XSECException(XSECException::AlgorithmMapperError,
				"Key type does not match <SignedInfo> signature type");

		}

		sigVfyRet = ((XSECCryptoKeyDSA *) key)->verifyBase64Signature(
			hash, 
			hashLen,
			(char *) sig, 
			(unsigned int) strlen(sig));

		break;

	case (XSECCryptoKey::KEY_RSA_PUBLIC) :
	case (XSECCryptoKey::KEY_RSA_PAIR) :

		if (sm != SIGNATURE_RSA) {
		
			throw XSECException(XSECException::AlgorithmMapperError,
				"Key type does not match <SignedInfo> signature type");

		}

		sigVfyRet = ((XSECCryptoKeyRSA *) key)->verifySHA1PKCS1Base64Signature(
			hash,
			hashLen,
			sig,
			(unsigned int) strlen(sig),
			hm);

		break;

	case (XSECCryptoKey::KEY_EC_PUBLIC) :
	case (XSECCryptoKey::KEY_EC_PAIR) :

		if (sm != SIGNATURE_ECDSA) {

			throw XSECException(XSECException::AlgorithmMapperError,
				"Key type does not match <SignedInfo> signature type");

		}

		sigVfyRet = ((XSECCryptoKeyEC *) key)->verifyBase64SignatureDSA(
			hash, 
			hashLen,
			(char *) sig, 
			(unsigned int) strlen(sig));

		break;

	case (XSECCryptoKey::KEY_HMAC) :

		// Already done - just compare calculated value with read value

        // FIX: CVE-2009-0217
        if (outputLength > 0 && (outputLength > hashLen || outputLength < 80 || outputLength < hashLen / 2)) {
            throw XSECException(XSECException::AlgorithmMapperError,
                "HMACOutputLength set to unsafe value.");
        }

	    sigVfyRet = compareBase64StringToRaw(sig,
			hash, 
			hashLen,
			outputLength);

		break;

	default :

		throw XSECException(XSECException::AlgorithmMapperError,
			"Key found, but don't know how to check the signature using it");

	}

	return sigVfyRet;

}

// --------------------------------------------------------------------------------
//			Hash TXFM appenders
// --------------------------------------------------------------------------------

bool DSIGAlgorithmHandlerDefault::appendHashTxfm(
		TXFMChain * inputBytes,
		const XMLCh * URI) {

	hashMethod hm;

	// Is this a URI we recognise?

	if (!XSECmapURIToHashMethod(URI, hm)) {
		safeBuffer sb;
		sb.sbTranscodeIn("DSIGAlgorithmHandlerDefault - Unknown Hash URI : ");
		sb.sbXMLChCat(URI);

		throw XSECException(XSECException::AlgorithmMapperError, 
			sb.rawXMLChBuffer());
	}

	TXFMBase * txfm;
	DOMDocument *d = inputBytes->getLastTxfm()->getDocument();
	switch (hm) {

	case HASH_SHA1 :
	case HASH_SHA224 :
	case HASH_SHA256 :
	case HASH_SHA384 :
	case HASH_SHA512 :

		XSECnew(txfm, TXFMSHA1(d, hm));
		break;
	
	case HASH_MD5 :

		XSECnew(txfm, TXFMMD5(d));
		break;

	default :

		safeBuffer sb;
		sb.sbTranscodeIn("DSIGAlgorithmHandlerDefault - Internal error unknown Hash, but URI known. URI : ");
		sb.sbXMLChCat(URI);

		throw XSECException(XSECException::AlgorithmMapperError, 
			sb.rawXMLChBuffer());

	}

	inputBytes->appendTxfm(txfm);

	return true;

}

// --------------------------------------------------------------------------------
//			SafeBuffer decryption
// --------------------------------------------------------------------------------

unsigned int DSIGAlgorithmHandlerDefault::decryptToSafeBuffer(
		TXFMChain * cipherText,
		XENCEncryptionMethod * encryptionMethod,
		XSECCryptoKey * key,
		DOMDocument * doc,
		safeBuffer & result
		) {

	throw XSECException(XSECException::AlgorithmMapperError, 
			"DSIGAlgorithmHandlerDefault - Encryption operations not supported");

}

bool DSIGAlgorithmHandlerDefault::appendDecryptCipherTXFM(
		TXFMChain * cipherText,
		XENCEncryptionMethod * encryptionMethod,
		XSECCryptoKey * key,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument * doc
		) {

	throw XSECException(XSECException::AlgorithmMapperError, 
			"DSIGAlgorithmHandlerDefault - Encryption operations not supported");
}


// --------------------------------------------------------------------------------
//			SafeBuffer encryption
// --------------------------------------------------------------------------------

bool DSIGAlgorithmHandlerDefault::encryptToSafeBuffer(
		TXFMChain * plainText,
		XENCEncryptionMethod * encryptionMethod,
		XSECCryptoKey * key,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument * doc,
		safeBuffer & result
		) {

	throw XSECException(XSECException::AlgorithmMapperError, 
			"DSIGAlgorithmHandlerDefault - Encryption operations not supported");

}

// --------------------------------------------------------------------------------
//			Key Creation
// --------------------------------------------------------------------------------

XSECCryptoKey * DSIGAlgorithmHandlerDefault::createKeyForURI(
		const XMLCh * uri,
		const unsigned char * keyBuffer,
		unsigned int keyLen
		) {

	throw XSECException(XSECException::AlgorithmMapperError, 
			"DSIGAlgorithmHandlerDefault - Key creation operations not supported");

}

