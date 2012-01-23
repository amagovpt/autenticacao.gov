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
 * WinCAPICryptoX509:= Windows CAPI based class for handling X509 (V3) certificates
 *
 * Author(s): Berin Lautenbach
 *
 * $Id: WinCAPICryptoX509.cpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/framework/XSECError.hpp>
#include <xsec/enc/WinCAPI/WinCAPICryptoProvider.hpp>
#include <xsec/enc/WinCAPI/WinCAPICryptoX509.hpp>
#include <xsec/enc/WinCAPI/WinCAPICryptoKeyDSA.hpp>
#include <xsec/enc/WinCAPI/WinCAPICryptoKeyRSA.hpp>
#include <xsec/enc/XSECCryptoException.hpp>
#include <xsec/enc/XSCrypt/XSCryptCryptoBase64.hpp>

#if defined (XSEC_HAVE_WINCAPI)

#include <xercesc/util/Janitor.hpp>

XSEC_USING_XERCES(ArrayJanitor);

WinCAPICryptoX509::WinCAPICryptoX509(HCRYPTPROV provRSA, HCRYPTPROV provDSS) :
m_DERX509(""), mp_certContext(NULL), m_pRSA(provRSA), m_pDSS(provDSS) { 


}

WinCAPICryptoX509::WinCAPICryptoX509(PCCERT_CONTEXT pCertContext,
									 HCRYPTPROV provRSA, HCRYPTPROV provDSS) :
m_pRSA(provRSA), m_pDSS(provDSS) {

	// Build this from an existing PCCERT_CONTEXT structure

	mp_certContext = pCertContext;

	unsigned char * encCert;
	unsigned long len = mp_certContext->cbCertEncoded * 2;
	XSECnew(encCert, unsigned char [len]);
	ArrayJanitor<unsigned char> j_encCert(encCert);

	// Base64 Encode
	XSCryptCryptoBase64 b64;

	b64.encodeInit();
	unsigned long encCertLen = b64.encode(mp_certContext->pbCertEncoded, mp_certContext->cbCertEncoded, encCert, len);
	encCertLen += b64.encodeFinish(&encCert[encCertLen], len - encCertLen);

	// Check the result
	if (encCert == NULL) {
		throw XSECCryptoException(XSECCryptoException::X509Error,
			"WinCAPIX509:WinCAPIX509 - Error encoding certificate");
	}

	m_DERX509.sbMemcpyIn(encCert, encCertLen);
	m_DERX509[encCertLen] = '\0';

}

WinCAPICryptoX509::~WinCAPICryptoX509() {

	if (mp_certContext != NULL)
		CertFreeCertificateContext(mp_certContext);

}

// load functions

void WinCAPICryptoX509::loadX509Base64Bin(const char * buf, unsigned int len) {

	unsigned char * rawCert;
	XSECnew(rawCert, unsigned char [len]);
	ArrayJanitor<unsigned char> j_rawCert(rawCert);

	// Base64 Decode
	XSCryptCryptoBase64 b64;

	b64.decodeInit();
	unsigned int rawCertLen = b64.decode((unsigned char *) buf, len, rawCert, len);
	rawCertLen += b64.decodeFinish(&rawCert[rawCertLen], len - rawCertLen);

	// Now load certificate into Win32 CSP

	mp_certContext = CertCreateCertificateContext(
		X509_ASN_ENCODING,
		rawCert,
		rawCertLen);

	if (mp_certContext == 0) {
		throw XSECCryptoException(XSECCryptoException::X509Error,
			"WinCAPIX509:loadX509Base64Bin - Error decoding certificate");
	}


	m_DERX509.sbMemcpyIn(buf, len);
	m_DERX509[len] = '\0';

}

// Info functions

XSECCryptoKey::KeyType WinCAPICryptoX509::getPublicKeyType() const {

	if (mp_certContext == NULL) {
		throw XSECCryptoException(XSECCryptoException::X509Error,
			"WinCAPI:X509 - getPublicKeyType called before X509 loaded");
	}

	if (lstrcmp(mp_certContext->pCertInfo->SubjectPublicKeyInfo.Algorithm.pszObjId, CRYPTO_OID_DSA) == 0)
		return XSECCryptoKey::KEY_DSA_PUBLIC;

	if (lstrcmp(mp_certContext->pCertInfo->SubjectPublicKeyInfo.Algorithm.pszObjId, szOID_RSA_RSA) == 0)
		return XSECCryptoKey::KEY_RSA_PUBLIC;

	return XSECCryptoKey::KEY_NONE;

}
		

// Get functions
XSECCryptoKey * WinCAPICryptoX509::clonePublicKey() const {


	if (mp_certContext == NULL) {
		throw XSECCryptoException(XSECCryptoException::X509Error,
			"WinCAPI:X509 - clonePublicKey called before X509 loaded");
	}

	// Import the key into the provider to get a pointer to the key

	HCRYPTKEY key;
	BOOL fResult;
                 
	if (getPublicKeyType() == XSECCryptoKey::KEY_DSA_PUBLIC) {

		fResult= CryptImportPublicKeyInfo(
			   m_pDSS,
			   X509_ASN_ENCODING,
			   &(mp_certContext->pCertInfo->SubjectPublicKeyInfo),
			   &key);
                 

		if (fResult == FALSE) {
			throw XSECCryptoException(XSECCryptoException::X509Error,
				"WinCAPI:X509 - Error loading public key info from certificate");
		}

		// Now that we have a handle for the DSA key, create a DSA Key object to
		// wrap it in

		WinCAPICryptoKeyDSA * ret;
		XSECnew(ret, WinCAPICryptoKeyDSA(m_pDSS, key));

		return ret;

	}

	if (getPublicKeyType() == XSECCryptoKey::KEY_RSA_PUBLIC) {

		fResult= CryptImportPublicKeyInfo(
			   m_pRSA,
			   X509_ASN_ENCODING,
			   &(mp_certContext->pCertInfo->SubjectPublicKeyInfo),
			   &key);
                 

		if (fResult == FALSE) {
			throw XSECCryptoException(XSECCryptoException::X509Error,
				"WinCAPI:X509 - Error loading public key info from certificate");
		}

		// Now that we have a handle for the DSA key, create a DSA Key object to
		// wrap it in

		WinCAPICryptoKeyRSA * ret;
		XSECnew(ret, WinCAPICryptoKeyRSA(m_pRSA, key));

		return ret;

	}

	return NULL;		// Unknown key type, but not necessarily an error

}

#endif /* XSEC_HAVE_WINCAPI */


