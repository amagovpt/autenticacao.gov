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
 * OpenSSLCryptoX509:= OpenSSL based class for handling X509 (V3) certificates
 *
 * Author(s): Berin Lautenbach
 *
 * $Id: OpenSSLCryptoX509.cpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#include <xsec/framework/XSECDefs.hpp>
#if defined (XSEC_HAVE_OPENSSL)

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/framework/XSECError.hpp>
#include <xsec/enc/OpenSSL/OpenSSLCryptoX509.hpp>
#include <xsec/enc/OpenSSL/OpenSSLCryptoKeyDSA.hpp>
#include <xsec/enc/OpenSSL/OpenSSLCryptoKeyEC.hpp>
#include <xsec/enc/OpenSSL/OpenSSLCryptoKeyRSA.hpp>
#include <xsec/enc/XSECCryptoException.hpp>
#include <xsec/enc/XSCrypt/XSCryptCryptoBase64.hpp>

#include <xercesc/util/Janitor.hpp>

XSEC_USING_XERCES(ArrayJanitor);
XSEC_USING_XERCES(Janitor);

#include <openssl/evp.h>

OpenSSLCryptoX509::OpenSSLCryptoX509() :
m_DERX509("") { 

	mp_X509 = NULL;

}

OpenSSLCryptoX509::~OpenSSLCryptoX509() {

	if (mp_X509 != NULL)
		X509_free(mp_X509);

}

OpenSSLCryptoX509::OpenSSLCryptoX509(X509 * x) {

	// Build this from an existing X509 structure

	mp_X509 = X509_dup(x);
	
	// Now need to create the DER encoding

	BIO * b64 = BIO_new(BIO_f_base64());
	BIO * bmem = BIO_new(BIO_s_mem());

	BIO_set_mem_eof_return(bmem, 0);
	b64 = BIO_push(b64, bmem);

	// Translate X509 to Base64

	i2d_X509_bio(b64, x);

	BIO_flush(b64);

	char buf[1024];
	unsigned int l;
	
	m_DERX509.sbStrcpyIn("");

	while ((l = BIO_read(bmem, buf, 1023)) > 0) {
		buf[l] = '\0';
		m_DERX509.sbStrcatIn(buf);
	}

	BIO_free_all(b64);

}

// load functions

void OpenSSLCryptoX509::loadX509Base64Bin(const char * buf, unsigned int len) {

	// Free anything currently held.
	
	if (mp_X509 != NULL)
		X509_free(mp_X509);
	
	// Have to implement using EVP_Decode routines due to a bug in older
	// versions of OpenSSL BIO_f_base64

	int bufLen = len;
	unsigned char * outBuf;
	XSECnew(outBuf, unsigned char[len + 1]);
	ArrayJanitor<unsigned char> j_outBuf(outBuf);

	/* Had to move to our own Base64 decoder because it handles non-wrapped b64
	   better.  Grrr. */

	XSCryptCryptoBase64 *b64;
	XSECnew(b64, XSCryptCryptoBase64);
	Janitor<XSCryptCryptoBase64> j_b64(b64);

	b64->decodeInit();
	bufLen = b64->decode((unsigned char *) buf, len, outBuf, len);
	bufLen += b64->decodeFinish(&outBuf[bufLen], len-bufLen);

	/*

	EVP_ENCODE_CTX m_dctx;
	EVP_DecodeInit(&m_dctx);

	int rc = EVP_DecodeUpdate(&m_dctx, 
						  outBuf, 
						  &bufLen, 
						  (unsigned char *) buf, 
						  len);

	if (rc < 0) {

		throw XSECCryptoException(XSECCryptoException::Base64Error,
			"OpenSSL:Base64 - Error during Base64 Decode of X509 Certificate");
	}
	
	int finalLen;
	EVP_DecodeFinal(&m_dctx, &outBuf[bufLen], &finalLen); 

	bufLen += finalLen;
	*/
	if (bufLen > 0) {
#if defined(XSEC_OPENSSL_D2IX509_CONST_BUFFER)
		mp_X509=  d2i_X509(NULL, (const unsigned char **) (&outBuf), bufLen);
#else
		mp_X509=  d2i_X509(NULL, &outBuf, bufLen);
#endif
	}

	// Check to see if we have a certificate....
	if (mp_X509 == NULL) {

		throw XSECCryptoException(XSECCryptoException::X509Error,
		"OpenSSL:X509 - Error translating Base64 DER encoding into OpenSSL X509 structure");

	}

	m_DERX509.sbStrcpyIn(buf);

}

// Info functions

const XMLCh * OpenSSLCryptoX509::getProviderName() const {

	return DSIGConstants::s_unicodeStrPROVOpenSSL;

}

XSECCryptoKey::KeyType OpenSSLCryptoX509::getPublicKeyType() const {

	if (mp_X509 == NULL) {
		throw XSECCryptoException(XSECCryptoException::X509Error,
			"OpenSSL:X509 - getPublicKeyType called before X509 loaded");
	}

	EVP_PKEY *pkey;

	pkey = X509_get_pubkey(mp_X509);

	if (pkey == NULL) {
		throw XSECCryptoException(XSECCryptoException::X509Error,
			"OpenSSL:X509 - cannot retrieve public key from cert");
	}

	XSECCryptoKey::KeyType ret;

	switch (pkey->type) {

	case EVP_PKEY_DSA :

		ret = XSECCryptoKey::KEY_DSA_PUBLIC;
		break;

	case EVP_PKEY_RSA :

		ret = XSECCryptoKey::KEY_RSA_PUBLIC;
		break;

#if defined(XSEC_OPENSSL_HAVE_EC)
	case EVP_PKEY_EC :

		ret = XSECCryptoKey::KEY_EC_PUBLIC;
		break;
#endif

	default :

		ret = XSECCryptoKey::KEY_NONE;

	}

	EVP_PKEY_free (pkey);

	return ret;

}
		

// Get functions
XSECCryptoKey * OpenSSLCryptoX509::clonePublicKey() const {

	if (mp_X509 == NULL) {
		throw XSECCryptoException(XSECCryptoException::X509Error,
			"OpenSSL:X509 - clonePublicKey called before X509 loaded");
	}

	EVP_PKEY *pkey;
	XSECCryptoKey * ret;

	pkey = X509_get_pubkey(mp_X509);

	if (pkey == NULL) {
		throw XSECCryptoException(XSECCryptoException::X509Error,
			"OpenSSL:X509 - cannot retrieve public key from cert");
	}

	switch (pkey->type) {

	case EVP_PKEY_DSA :

		ret = new OpenSSLCryptoKeyDSA(pkey);
		break;

	case EVP_PKEY_RSA :

		ret = new OpenSSLCryptoKeyRSA(pkey);
		break;

#if defined(XSEC_OPENSSL_HAVE_EC)
	case EVP_PKEY_EC :

		ret = new OpenSSLCryptoKeyEC(pkey);
		break;
#endif

	default :

		ret = NULL;

	}

	EVP_PKEY_free (pkey);

	return ret;

}

#endif /* XSEC_HAVE_OPENSSL */
