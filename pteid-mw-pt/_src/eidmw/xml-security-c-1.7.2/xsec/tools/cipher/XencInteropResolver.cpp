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
 * XencInteropResolver := Class to resolve key elements into certificates for
 *						interop test
 *
 * $Id: XencInteropResolver.cpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

// XSEC

#include "XencInteropResolver.hpp"

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/framework/XSECProvider.hpp>
#include <xsec/enc/XSECKeyInfoResolver.hpp>
#include <xsec/dsig/DSIGKeyInfoName.hpp>
#include <xsec/dsig/DSIGKeyInfoX509.hpp>
#include <xsec/utils/XSECDOMUtils.hpp>
#include <xsec/xenc/XENCCipher.hpp>
#include <xsec/xenc/XENCEncryptedKey.hpp>
#include <xercesc/util/Janitor.hpp>
#include <xercesc/util/XMLUniDefs.hpp>

XERCES_CPP_NAMESPACE_USE

#include <iostream>

#if !defined (XSEC_HAVE_OPENSSL) && !defined (XSEC_HAVE_WINCAPI) && !defined (XSEC_HAVE_NSS)
#	error Require OpenSSL or Windows Crypto API for the Merlin Resolver
#endif

#if defined (XSEC_HAVE_OPENSSL)
#	include <openssl/x509.h>
#	include <openssl/pem.h>
#	include <xsec/enc/OpenSSL/OpenSSLCryptoX509.hpp>
#	include <xsec/enc/OpenSSL/OpenSSLCryptoKeyRSA.hpp>
#   include <xsec/enc/OpenSSL/OpenSSLCryptoSymmetricKey.hpp>
#endif

#if defined (XSEC_HAVE_WINCAPI)
#   include <xsec/enc/WinCAPI/WinCAPICryptoProvider.hpp>
#   include <xsec/enc/WinCAPI/WinCAPICryptoSymmetricKey.hpp>
#endif

#if defined (XSEC_HAVE_NSS)
#   include <xsec/enc/NSS/NSSCryptoProvider.hpp>
#   include <xsec/enc/NSS/NSSCryptoSymmetricKey.hpp>
#endif

// --------------------------------------------------------------------------------
//           Merlin Xenc-Five keys and Strings
// --------------------------------------------------------------------------------

static XMLCh s_bobName[] = {
	chLatin_b,
	chLatin_o,
	chLatin_b,
	chNull
};

static XMLCh s_jobName[] = {
	chLatin_j,
	chLatin_o,
	chLatin_b,
	chNull
};

static XMLCh s_jebName[] = {
	chLatin_j,
	chLatin_e,
	chLatin_b,
	chNull
};

static XMLCh s_jedName[] = {
	chLatin_j,
	chLatin_e,
	chLatin_d,
	chNull
};

static char s_bobKey[] = "abcdefghijklmnopqrstuvwx";
static char s_jobKey[] = "abcdefghijklmnop";
static char s_jebKey[] = "abcdefghijklmnopqrstuvwx";
static char s_jedKey[] = "abcdefghijklmnopqrstuvwxyz012345";

// --------------------------------------------------------------------------------
//           Phaos xenc-three strings and keys
// --------------------------------------------------------------------------------

static XMLCh s_phaosRSAName[] = {
	chLatin_m,
	chLatin_y,
	chDash,
	chLatin_r,
	chLatin_s,
	chLatin_a,
	chDash,
	chLatin_k,
	chLatin_e,
	chLatin_y,
	chNull
};

static XMLCh s_phaosTripleDESName[] = {
	chLatin_m,
	chLatin_y,
	chDash,
	chLatin_t,
	chLatin_r,
	chLatin_i,
	chLatin_p,
	chLatin_l,
	chLatin_e,
	chLatin_d,
	chLatin_e,
	chLatin_s,
	chDash,
	chLatin_k,
	chLatin_e,
	chLatin_y,
	chNull
};

static XMLCh s_phaos3DESName[] = {
	chLatin_m,
	chLatin_y,
	chDash,
	chDigit_3,
	chLatin_d,
	chLatin_e,
	chLatin_s,
	chDash,
	chLatin_k,
	chLatin_e,
	chLatin_y,
	chNull
};

static XMLCh s_phaosAES128Name[] = {
	chLatin_m,
	chLatin_y,
	chDash,
	chLatin_a,
	chLatin_e,
	chLatin_s,
	chDigit_1,
	chDigit_2,
	chDigit_8,
	chDash,
	chLatin_k,
	chLatin_e,
	chLatin_y,
	chNull
};

static XMLCh s_phaosAES192Name[] = {
	chLatin_m,
	chLatin_y,
	chDash,
	chLatin_a,
	chLatin_e,
	chLatin_s,
	chDigit_1,
	chDigit_9,
	chDigit_2,
	chDash,
	chLatin_k,
	chLatin_e,
	chLatin_y,
	chNull
};

static XMLCh s_phaosAES256Name[] = {
	chLatin_m,
	chLatin_y,
	chDash,
	chLatin_a,
	chLatin_e,
	chLatin_s,
	chDigit_2,
	chDigit_5,
	chDigit_6,
	chDash,
	chLatin_k,
	chLatin_e,
	chLatin_y,
	chNull
};

unsigned char s_phaos3DESKey[] = {

	0xc8, 0x8f, 0x89, 0xd5, 0xfd, 0xe9, 0xb9, 0x80, 
	0x04, 0x46, 0x32, 0x1c, 0x4f, 0xab, 0xdf, 0x83, 
	0xa4, 0x62, 0xb6, 0x62, 0x97, 0xf2, 0x70, 0xf4

};

unsigned char s_phaosAES128Key[] = {

	0xd3, 0x5f, 0xb2, 0xb9, 0x0d, 0xa1, 0xb8, 0xf4, 
	0xb5, 0xf9, 0x0b, 0xf4, 0x2c, 0x7f, 0xb3, 0x69

};

unsigned char s_phaosAES192Key[] = {
	
	0x22, 0x57, 0xee, 0x4b, 0x8d, 0x0b, 0xbd, 0x2b, 
	0x55, 0x53, 0x43, 0x23, 0xf1, 0xe3, 0xeb, 0xac, 
	0x61, 0xd5, 0x84, 0x06, 0xf8, 0xf3, 0x2f, 0xbe

};

unsigned char s_phaosAES256Key[] = {
	
	0x66, 0x16, 0x78, 0xbf, 0x74, 0x65, 0xc1, 0x39, 
	0x42, 0x10, 0xea, 0x48, 0xac, 0x77, 0xcb, 0x29, 
	0x5c, 0x89, 0x38, 0x10, 0xed, 0x10, 0x93, 0x8e, 
	0x40, 0x36, 0xad, 0xff, 0x8c, 0x51, 0xd5, 0xb0

};

// --------------------------------------------------------------------------------
//           Construct/Destruct
// --------------------------------------------------------------------------------


XencInteropResolver::XencInteropResolver(DOMDocument * doc, const XMLCh * baseURI) {

	if (baseURI != NULL)
		mp_baseURI = XMLString::replicate(baseURI);
	else
		mp_baseURI = NULL;

#if !defined(_WIN32)
	m_fcount = 0;
#endif

	mp_doc = doc;

}


XencInteropResolver::~XencInteropResolver() {

	if (mp_baseURI != NULL)
		XSEC_RELEASE_XMLCH(mp_baseURI);

}
// --------------------------------------------------------------------------------
//			Utility functions
// --------------------------------------------------------------------------------
#if defined(_WIN32)

void reverseSlash(safeBuffer &path) {

	for (int i = 0; i < (int) strlen(path.rawCharBuffer()); ++i) {

		if (path[i] == '/')
			path[i] = '\\';

	}

}

#endif
	
XSECCryptoSymmetricKey * XencInteropResolver::makeSymmetricKey(XSECCryptoSymmetricKey::SymmetricKeyType type) {

		XSECCryptoSymmetricKey * k = XSECPlatformUtils::g_cryptoProvider->keySymmetric(type);
		return k;
}

#if defined (XSEC_HAVE_OPENSSL)

BIO * createFileBIO(const XMLCh * baseURI, const char * name) {

	// Open file URI relative to the encrypted file

	BIO * bioFile;
	if ((bioFile = BIO_new(BIO_s_file())) == NULL) {
		
		return NULL;

	}

	safeBuffer fname;
	fname.sbTranscodeIn(baseURI);
	fname.sbStrcatIn("/");
	fname.sbStrcatIn(name);

#if defined(_WIN32)
	reverseSlash(fname);
#endif

	if (BIO_read_filename(bioFile, fname.rawCharBuffer()) <= 0) {
		
		return NULL;

	}

	return bioFile;
}
#endif
// --------------------------------------------------------------------------------
//           Resolver
// --------------------------------------------------------------------------------

XSECCryptoKey * XencInteropResolver::resolveKey(DSIGKeyInfoList * lst) {

	int lstSize = (int) lst->getSize();

	for (int i = 0; i < lstSize; ++i) {

		DSIGKeyInfo * ki = lst->item(i);

		if (ki->getKeyInfoType() == DSIGKeyInfo::KEYINFO_NAME) {

			DSIGKeyInfoName * kn = (DSIGKeyInfoName *) ki;

			const XMLCh * name = kn->getKeyName();

			// Check if this is a key we know

			if (strEquals(s_bobName, name)) {
				XSECCryptoSymmetricKey * k = 
					XSECPlatformUtils::g_cryptoProvider->keySymmetric(XSECCryptoSymmetricKey::KEY_3DES_192);
				try {
					k->setKey((unsigned char *) s_bobKey, (unsigned int) strlen(s_bobKey));
				} catch (...) {
					delete k;
					throw;
				}
				return k;
			}
			if (strEquals(s_jobName, name)) {
				XSECCryptoSymmetricKey * k = 
					XSECPlatformUtils::g_cryptoProvider->keySymmetric(XSECCryptoSymmetricKey::KEY_AES_128);
				try {
					k->setKey((unsigned char *) s_jobKey, (unsigned int) strlen(s_jobKey));
				} catch(...) {
					delete k;
					throw;
				}
				return k;
			}
			if (strEquals(s_jebName, name)) {
				XSECCryptoSymmetricKey * k = 
					XSECPlatformUtils::g_cryptoProvider->keySymmetric(XSECCryptoSymmetricKey::KEY_AES_192);
				try {
					k->setKey((unsigned char *) s_jebKey, (unsigned int) strlen(s_jebKey));
				} catch(...) {
					delete k;
					throw;
				}
				return k;
			}
			if (strEquals(s_jedName, name)) {
				XSECCryptoSymmetricKey * k = 
					XSECPlatformUtils::g_cryptoProvider->keySymmetric(XSECCryptoSymmetricKey::KEY_AES_256);
				try {
					k->setKey((unsigned char *) s_jedKey, (unsigned int) strlen(s_jedKey));
				} catch(...) {
					delete k;
					throw;
				}
				return k;
			}
			// PHAOS Keys
			if (strEquals(s_phaos3DESName, name) || strEquals(s_phaosTripleDESName, name)) {
				XSECCryptoSymmetricKey * k = 
					XSECPlatformUtils::g_cryptoProvider->keySymmetric(XSECCryptoSymmetricKey::KEY_3DES_192);
				try {
					k->setKey(s_phaos3DESKey, 24);
				} catch (...) {
					delete k;
					throw;
				}
				return k;
			}
			if (strEquals(s_phaosAES128Name, name)) {
				XSECCryptoSymmetricKey * k = 
					XSECPlatformUtils::g_cryptoProvider->keySymmetric(XSECCryptoSymmetricKey::KEY_AES_128);
				try {
					k->setKey(s_phaosAES128Key, 16);
				} catch(...) {
					delete k;
					throw;
				}
				return k;
			}
			if (strEquals(s_phaosAES192Name, name)) {
				XSECCryptoSymmetricKey * k = 
					XSECPlatformUtils::g_cryptoProvider->keySymmetric(XSECCryptoSymmetricKey::KEY_AES_192);
				try {
					k->setKey(s_phaosAES192Key, 24);
				} catch(...) {
					delete k;
					throw;
				}
				return k;
			}
			if (strEquals(s_phaosAES256Name, name)) {
				XSECCryptoSymmetricKey * k = 
					XSECPlatformUtils::g_cryptoProvider->keySymmetric(XSECCryptoSymmetricKey::KEY_AES_256);
				try {
					k->setKey(s_phaosAES256Key, 32);
				} catch(...) {
					delete k;
					throw;
				}
				return k;
			}
#if defined (XSEC_HAVE_OPENSSL)
			if (strEquals(s_phaosRSAName, name)) {

				// This is the Phaos RSA key
				BIO * rsaFile = createFileBIO(mp_baseURI, "rsa-priv-key.der");
				if (rsaFile == NULL)
					return NULL;

				PKCS8_PRIV_KEY_INFO * p8inf;
				p8inf = d2i_PKCS8_PRIV_KEY_INFO_bio(rsaFile, NULL);

				EVP_PKEY * pk = EVP_PKCS82PKEY(p8inf);
				OpenSSLCryptoKeyRSA * k = new OpenSSLCryptoKeyRSA(pk);
				PKCS8_PRIV_KEY_INFO_free(p8inf);
				BIO_free_all(rsaFile);
				return k;

			}
#endif

			// If we get this far, we don't know it.  So look for EncryptedKey elements
			// containing this name as a CarriedKeyName

			DOMNode * c = mp_doc->getDocumentElement()->getFirstChild();
			while (c != NULL) {

				if (c->getNodeType() == DOMNode::ELEMENT_NODE &&
					strEquals(getDSIGLocalName(c), MAKE_UNICODE_STRING("KeyInfo"))) {

					DOMNode * ek = c->getFirstChild();
					while (ek != NULL) {

						if (ek->getNodeType() == DOMNode::ELEMENT_NODE &&
							strEquals(getXENCLocalName(ek), MAKE_UNICODE_STRING("EncryptedKey"))) {

							// Load
							XSECProvider prov;
							XENCCipher * cipher = prov.newCipher(mp_doc);

							XENCEncryptedKey * xek = cipher->loadEncryptedKey(static_cast<DOMElement*>(ek));
							Janitor<XENCEncryptedKey> j_xek(xek);

							if (strEquals(xek->getCarriedKeyName(), name) &&
								strEquals(xek->getRecipient(), MAKE_UNICODE_STRING("you"))) {

								// This is it!
								cipher->setKeyInfoResolver(this);
								unsigned char keyBuf[1024];
								int sz = cipher->decryptKey(xek, keyBuf, 1024);

								if (sz > 0) {
									XSECCryptoSymmetricKey * k = 
										XSECPlatformUtils::g_cryptoProvider->keySymmetric(XSECCryptoSymmetricKey::KEY_AES_256);
									try {
										k->setKey(keyBuf, sz);
									} catch (...) {
										delete k;
										throw;
									}

									return k;
								}
							}
						}
						ek = ek->getNextSibling();
					}
				}
				
				c = c->getNextSibling();
			}

		}

		else if (ki->getKeyInfoType() == DSIGKeyInfo::KEYINFO_X509) {

			DSIGKeyInfoX509 * kix = (DSIGKeyInfoX509 *) ki;

			XSECCryptoX509 * XCX509 = kix->getCertificateCryptoItem(0);

			if (XCX509 != 0) {
#if defined (XSEC_HAVE_OPENSSL)


				if (strEquals(XCX509->getProviderName(),DSIGConstants::s_unicodeStrPROVOpenSSL)) {

					OpenSSLCryptoX509 * OSSLX509 = (OpenSSLCryptoX509 *) XCX509;
					X509 * x509 = OSSLX509->getOpenSSLX509();

					// Check the serial number
					BIGNUM * bnserial = ASN1_INTEGER_to_BN(x509->cert_info->serialNumber, NULL);
					BN_free(bnserial);

					BIO * rsaFile = createFileBIO(mp_baseURI, "rsa.p8");
					if (rsaFile == NULL)
						return NULL;

					PKCS8_PRIV_KEY_INFO * p8inf;
					p8inf = d2i_PKCS8_PRIV_KEY_INFO_bio(rsaFile, NULL);

					EVP_PKEY * pk = EVP_PKCS82PKEY(p8inf);
					OpenSSLCryptoKeyRSA * k = new OpenSSLCryptoKeyRSA(pk);
					PKCS8_PRIV_KEY_INFO_free(p8inf);
					BIO_free_all(rsaFile);
					return k;

				}
#if defined (XSEC_HAVE_WINCAPI)
				else {
#endif /* XSEC_HAVE_WINCAPI */
#endif /* XSEC_HAVE_OPENSSL */

#if defined (XSEC_HAVE_WINCAPI)
					std::cerr << "WARNING - Unable to load PKCS8 private key file into Windows CAPI" << std::endl;
#if defined (XSEC_HAVE_OPENSSL)
				}
#endif /* XSEC_HAVE_WINCAPI */
#endif /* XSEC_HAVE_OPENSSL */
			}
		}
	}

	return NULL;

}

XSECKeyInfoResolver * XencInteropResolver::clone(void) const {

	return new XencInteropResolver(mp_doc, mp_baseURI);

}



