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
 * NSSCryptoX509:= NSS based class for handling X509 (V3) certificates
 *
 * Author(s): Milan Tomic
 *
 */

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/framework/XSECError.hpp>
#include <xsec/enc/NSS/NSSCryptoProvider.hpp>
#include <xsec/enc/NSS/NSSCryptoX509.hpp>
#include <xsec/enc/NSS/NSSCryptoKeyDSA.hpp>
#include <xsec/enc/NSS/NSSCryptoKeyRSA.hpp>
#include <xsec/enc/XSECCryptoException.hpp>
#include <xsec/enc/XSCrypt/XSCryptCryptoBase64.hpp>

#if defined (XSEC_HAVE_NSS)

extern "C" {

extern CERTCertificate *
__CERT_DecodeDERCertificate (SECItem *derSignedCert, PRBool copyDER, char *nickname);

}

#include <xercesc/util/Janitor.hpp>

XSEC_USING_XERCES(ArrayJanitor);

// --------------------------------------------------------------------------------
//           Constructor
// --------------------------------------------------------------------------------

NSSCryptoX509::NSSCryptoX509() : m_DERX509(""), mp_cert(NULL) { 


}

// --------------------------------------------------------------------------------
//           Constructor
// --------------------------------------------------------------------------------

NSSCryptoX509::NSSCryptoX509(CERTCertificate * pCert) {

	// Build this from an existing CERTCertificate structure

	mp_cert = pCert;

	unsigned char * encCert;
	unsigned long len = mp_cert->derCert.len * 2;
	XSECnew(encCert, unsigned char [len]);
	ArrayJanitor<unsigned char> j_encCert(encCert);

	// Base64 Encode
	XSCryptCryptoBase64 b64;

	b64.encodeInit();
	unsigned long encCertLen = b64.encode(mp_cert->derCert.data, mp_cert->derCert.len, encCert, len);
	encCertLen += b64.encodeFinish(&encCert[encCertLen], len - encCertLen);

	// Check the result
	if (encCert == NULL) {

		throw XSECCryptoException(XSECCryptoException::X509Error,
			"NSSX509:NSSX509 - Error encoding certificate");

	}

	m_DERX509.sbMemcpyIn(encCert, encCertLen);
	m_DERX509[encCertLen] = '\0';

}

// --------------------------------------------------------------------------------
//           Destructor
// --------------------------------------------------------------------------------

NSSCryptoX509::~NSSCryptoX509() {

  if (mp_cert != 0)
	  CERT_DestroyCertificate(mp_cert);

}

// --------------------------------------------------------------------------------
//           Load X509
// --------------------------------------------------------------------------------

void NSSCryptoX509::loadX509Base64Bin(const char * buf, unsigned int len) {

	unsigned char * rawCert;
	XSECnew(rawCert, unsigned char [len]);
	ArrayJanitor<unsigned char> j_rawCert(rawCert);

	// Base64 Decode
	XSCryptCryptoBase64 b64;

	b64.decodeInit();
	unsigned int rawCertLen = b64.decode((unsigned char *) buf, len, rawCert, len);
	rawCertLen += b64.decodeFinish(&rawCert[rawCertLen], len - rawCertLen);

	// Now load certificate

	SECItem i;
	i.type = siBuffer;
	i.data = rawCert;
	i.len = rawCertLen;

	SECItem *certs[1];
	certs[0] = &i;

	// For returning
	CERTCertificate **certArray = NULL;

	/* mp_cert = __CERT_DecodeDERCertificate(&i, PR_TRUE, NULL); */
	CERT_ImportCerts(CERT_GetDefaultCertDB(), certUsageUserCertImport, 1, certs, 
		&certArray, PR_FALSE, PR_FALSE, NULL);
	// 1. If you got an compiler error here add into "nss/cert.h" delacarion for
	// CERT_DecodeDERCertificate() (the same parameters as for __CERT_DecodeDERCertificate())
	// 2. Since __CERT_DecodeDERCertificate is a private function we might consider using
	// __CERT_NewTempCertificate() or CERT_ImportCerts() instead.

	// Now map to our cert
	if (certArray == NULL) {
		throw XSECCryptoException(XSECCryptoException::X509Error,
			"NSSX509:loadX509Base64Bin - Error decoding certificate");
	}

	mp_cert = certArray[0];

	if (mp_cert == 0) {

		throw XSECCryptoException(XSECCryptoException::X509Error,
			"NSSX509:loadX509Base64Bin - Error decoding certificate");

	}

	m_DERX509.sbMemcpyIn(buf, len);
	m_DERX509[len] = '\0';

}

// --------------------------------------------------------------------------------
//           Get NSS provider name
// --------------------------------------------------------------------------------

const XMLCh * NSSCryptoX509::getProviderName() const {

	return DSIGConstants::s_unicodeStrPROVNSS;

}

// --------------------------------------------------------------------------------
//           Get publickey type: RSA or DSA
// --------------------------------------------------------------------------------

XSECCryptoKey::KeyType NSSCryptoX509::getPublicKeyType() const {

  if (mp_cert == NULL) {

		throw XSECCryptoException(XSECCryptoException::X509Error,
			"NSS:X509 - getPublicKeyType called before X509 loaded");

	}

  XSECCryptoKey::KeyType kt = XSECCryptoKey::KEY_NONE;

  SECKEYPublicKey * pubkey = CERT_ExtractPublicKey(mp_cert);

  if (pubkey == 0) {

    throw XSECCryptoException(XSECCryptoException::X509Error,
			"NSS:X509 - Error extracting public key from X509 certificate");

  }

	if (pubkey->keyType == dsaKey)
		kt = XSECCryptoKey::KEY_DSA_PUBLIC;

	if (pubkey->keyType == rsaKey)
		kt = XSECCryptoKey::KEY_RSA_PUBLIC;

  SECKEY_DestroyPublicKey(pubkey);

	return kt;

}

// --------------------------------------------------------------------------------
//           Replicate public key
// --------------------------------------------------------------------------------

XSECCryptoKey * NSSCryptoX509::clonePublicKey() const {

	if (mp_cert == NULL) {

		throw XSECCryptoException(XSECCryptoException::X509Error,
			"NSS:X509 - clonePublicKey called before X509 loaded");

	}

	// Import the key into the provider to get a pointer to the key

	if (getPublicKeyType() == XSECCryptoKey::KEY_DSA_PUBLIC) {

    SECKEYPublicKey * pubkey = CERT_ExtractPublicKey(mp_cert);

		// Now that we have a handle for the DSA key, create a DSA Key object to
		// wrap it in

		NSSCryptoKeyDSA * ret;
		XSECnew(ret, NSSCryptoKeyDSA(pubkey));

		return ret;

	}

	if (getPublicKeyType() == XSECCryptoKey::KEY_RSA_PUBLIC) {

    SECKEYPublicKey * pubkey = CERT_ExtractPublicKey(mp_cert);

		// Now that we have a handle for the DSA key, create a DSA Key object to
		// wrap it in

		NSSCryptoKeyRSA * ret;
		XSECnew(ret, NSSCryptoKeyRSA(pubkey));

		return ret;

	}

	return NULL;		// Unknown key type, but not necessarily an error

}

#endif /* XSEC_HAVE_NSS */
