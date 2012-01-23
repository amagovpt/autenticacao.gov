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
 * OpenSSLCryptoProvider := Base class to define an OpenSSL module
 *
 * Author(s): Berin Lautenbach
 *
 * $Id: OpenSSLCryptoProvider.cpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#include <xsec/framework/XSECDefs.hpp>
#if defined (XSEC_HAVE_OPENSSL)

#include <xsec/framework/XSECError.hpp>

#include <xsec/enc/OpenSSL/OpenSSLCryptoProvider.hpp>
#include <xsec/enc/OpenSSL/OpenSSLCryptoHash.hpp>
#include <xsec/enc/OpenSSL/OpenSSLCryptoHashHMAC.hpp>
#include <xsec/enc/OpenSSL/OpenSSLCryptoBase64.hpp>
#include <xsec/enc/OpenSSL/OpenSSLCryptoX509.hpp>
#include <xsec/enc/OpenSSL/OpenSSLCryptoKeyDSA.hpp>
#include <xsec/enc/OpenSSL/OpenSSLCryptoKeyEC.hpp>
#include <xsec/enc/OpenSSL/OpenSSLCryptoKeyHMAC.hpp>
#include <xsec/enc/OpenSSL/OpenSSLCryptoKeyRSA.hpp>
#include <xsec/enc/OpenSSL/OpenSSLCryptoSymmetricKey.hpp>

#include <xsec/enc/XSCrypt/XSCryptCryptoBase64.hpp>

#include <xsec/enc/XSECCryptoException.hpp>

#include <xercesc/util/Janitor.hpp>

XSEC_USING_XERCES(ArrayJanitor);
XSEC_USING_XERCES(Janitor);

#include <openssl/rand.h>
#include <openssl/err.h>
#include <openssl/obj_mac.h>

OpenSSLCryptoProvider::OpenSSLCryptoProvider() {

	OpenSSL_add_all_algorithms();		// Initialise Openssl
	ERR_load_crypto_strings();

	//SSLeay_add_all_algorithms();
#ifdef XSEC_OPENSSL_HAVE_EC
    // Populate curve names.
    m_namedCurveMap["urn:oid:1.3.132.0.6"] = NID_secp112r1;
    m_namedCurveMap["urn:oid:1.3.132.0.7"] = NID_secp112r2;
    m_namedCurveMap["urn:oid:1.3.132.0.28"] = NID_secp128r1;
    m_namedCurveMap["urn:oid:1.3.132.0.29"] = NID_secp128r2;
    m_namedCurveMap["urn:oid:1.3.132.0.9"] = NID_secp160k1;
    m_namedCurveMap["urn:oid:1.3.132.0.8"] = NID_secp160r1;
    m_namedCurveMap["urn:oid:1.3.132.0.30"] = NID_secp160r2;
    m_namedCurveMap["urn:oid:1.3.132.0.31"] = NID_secp192k1;
    m_namedCurveMap["urn:oid:1.3.132.0.32"] = NID_secp224k1;
    m_namedCurveMap["urn:oid:1.3.132.0.33"] = NID_secp224r1;
    m_namedCurveMap["urn:oid:1.3.132.0.10"] = NID_secp256k1;
    m_namedCurveMap["urn:oid:1.3.132.0.34"] = NID_secp384r1;
    m_namedCurveMap["urn:oid:1.3.132.0.35"] = NID_secp521r1;

    m_namedCurveMap["urn:oid:1.2.840.10045.3.1.1"] = NID_X9_62_prime192v1;
    m_namedCurveMap["urn:oid:1.2.840.10045.3.1.2"] = NID_X9_62_prime192v2;
    m_namedCurveMap["urn:oid:1.2.840.10045.3.1.3"] = NID_X9_62_prime192v3;
    m_namedCurveMap["urn:oid:1.2.840.10045.3.1.4"] = NID_X9_62_prime239v1;
    m_namedCurveMap["urn:oid:1.2.840.10045.3.1.5"] = NID_X9_62_prime239v2;
    m_namedCurveMap["urn:oid:1.2.840.10045.3.1.6"] = NID_X9_62_prime239v3;
    m_namedCurveMap["urn:oid:1.2.840.10045.3.1.7"] = NID_X9_62_prime256v1;

    m_namedCurveMap["urn:oid:1.3.132.0.4"] = NID_sect113r1;
    m_namedCurveMap["urn:oid:1.3.132.0.5"] = NID_sect113r2;
    m_namedCurveMap["urn:oid:1.3.132.0.22"] = NID_sect131r1;
    m_namedCurveMap["urn:oid:1.3.132.0.23"] = NID_sect131r2;
    m_namedCurveMap["urn:oid:1.3.132.0.1"] = NID_sect163k1;
    m_namedCurveMap["urn:oid:1.3.132.0.2"] = NID_sect163r1;
    m_namedCurveMap["urn:oid:1.3.132.0.15"] = NID_sect163r2;
    m_namedCurveMap["urn:oid:1.3.132.0.24"] = NID_sect193r1;
    m_namedCurveMap["urn:oid:1.3.132.0.25"] = NID_sect193r2;
    m_namedCurveMap["urn:oid:1.3.132.0.26"] = NID_sect233k1;
    m_namedCurveMap["urn:oid:1.3.132.0.27"] = NID_sect233r1;
    m_namedCurveMap["urn:oid:1.3.132.0.3"] = NID_sect239k1;
    m_namedCurveMap["urn:oid:1.3.132.0.16"] = NID_sect283k1;
    m_namedCurveMap["urn:oid:1.3.132.0.17"] = NID_sect283r1;
    m_namedCurveMap["urn:oid:1.3.132.0.36"] = NID_sect409k1;
    m_namedCurveMap["urn:oid:1.3.132.0.37"] = NID_sect409r1;
    m_namedCurveMap["urn:oid:1.3.132.0.38"] = NID_sect571k1;
    m_namedCurveMap["urn:oid:1.3.132.0.39"] = NID_sect571r1;

    m_namedCurveMap["urn:oid:1.2.840.10045.3.0.1"] = NID_X9_62_c2pnb163v1;
    m_namedCurveMap["urn:oid:1.2.840.10045.3.0.2"] = NID_X9_62_c2pnb163v2;
    m_namedCurveMap["urn:oid:1.2.840.10045.3.0.3"] = NID_X9_62_c2pnb163v3;
    m_namedCurveMap["urn:oid:1.2.840.10045.3.0.4"] = NID_X9_62_c2pnb176v1;
    m_namedCurveMap["urn:oid:1.2.840.10045.3.0.5"] = NID_X9_62_c2tnb191v1;
    m_namedCurveMap["urn:oid:1.2.840.10045.3.0.6"] = NID_X9_62_c2tnb191v2;
    m_namedCurveMap["urn:oid:1.2.840.10045.3.0.7"] = NID_X9_62_c2tnb191v3;
    m_namedCurveMap["urn:oid:1.2.840.10045.3.0.8"] = NID_X9_62_c2onb191v4;
    m_namedCurveMap["urn:oid:1.2.840.10045.3.0.9"] = NID_X9_62_c2onb191v5;
    m_namedCurveMap["urn:oid:1.2.840.10045.3.0.10"] = NID_X9_62_c2pnb208w1;
    m_namedCurveMap["urn:oid:1.2.840.10045.3.0.11"] = NID_X9_62_c2tnb239v1;
    m_namedCurveMap["urn:oid:1.2.840.10045.3.0.12"] = NID_X9_62_c2tnb239v2;
    m_namedCurveMap["urn:oid:1.2.840.10045.3.0.13"] = NID_X9_62_c2tnb239v3;
    m_namedCurveMap["urn:oid:1.2.840.10045.3.0.14"] = NID_X9_62_c2onb239v4;
    m_namedCurveMap["urn:oid:1.2.840.10045.3.0.15"] = NID_X9_62_c2onb239v5;
    m_namedCurveMap["urn:oid:1.2.840.10045.3.0.16"] = NID_X9_62_c2pnb272w1;
    m_namedCurveMap["urn:oid:1.2.840.10045.3.0.17"] = NID_X9_62_c2pnb304w1;
    m_namedCurveMap["urn:oid:1.2.840.10045.3.0.18"] = NID_X9_62_c2tnb359v1;
    m_namedCurveMap["urn:oid:1.2.840.10045.3.0.19"] = NID_X9_62_c2pnb368w1;
    m_namedCurveMap["urn:oid:1.2.840.10045.3.0.20"] = NID_X9_62_c2tnb431r1;

    m_namedCurveMap["urn:oid:2.23.43.1.4.1"] = NID_wap_wsg_idm_ecid_wtls1;
    m_namedCurveMap["urn:oid:2.23.43.1.4.3"] = NID_wap_wsg_idm_ecid_wtls3;
    m_namedCurveMap["urn:oid:2.23.43.1.4.4"] = NID_wap_wsg_idm_ecid_wtls4;
    m_namedCurveMap["urn:oid:2.23.43.1.4.5"] = NID_wap_wsg_idm_ecid_wtls5;
    m_namedCurveMap["urn:oid:2.23.43.1.4.6"] = NID_wap_wsg_idm_ecid_wtls6;
    m_namedCurveMap["urn:oid:2.23.43.1.4.7"] = NID_wap_wsg_idm_ecid_wtls7;
    m_namedCurveMap["urn:oid:2.23.43.1.4.8"] = NID_wap_wsg_idm_ecid_wtls8;
    m_namedCurveMap["urn:oid:2.23.43.1.4.9"] = NID_wap_wsg_idm_ecid_wtls9;
    m_namedCurveMap["urn:oid:2.23.43.1.4.10"] = NID_wap_wsg_idm_ecid_wtls10;
    m_namedCurveMap["urn:oid:2.23.43.1.4.11"] = NID_wap_wsg_idm_ecid_wtls11;
    m_namedCurveMap["urn:oid:2.23.43.1.4.12"] = NID_wap_wsg_idm_ecid_wtls12;
#endif
}


OpenSSLCryptoProvider::~OpenSSLCryptoProvider() {

	EVP_cleanup();
	ERR_free_strings();
	/* As suggested by Jesse Pelton */
#if defined(XSEC_OPENSSL_HAVE_CRYPTO_CLEANUP_ALL_EX_DATA)
	CRYPTO_cleanup_all_ex_data();
#endif
	RAND_cleanup();
	X509_TRUST_cleanup();
	ERR_remove_state(0);
}

#ifdef XSEC_OPENSSL_HAVE_EC
int OpenSSLCryptoProvider::curveNameToNID(const char* curveName) const {

    std::map<std::string,int>::const_iterator i = m_namedCurveMap.find(curveName);
    if (i == m_namedCurveMap.end())
   		throw XSECCryptoException(XSECCryptoException::UnsupportedError,
			"OpenSSLCryptoProvider::curveNameToNID - curve name not recognized");
    return i->second;

}
#endif

const XMLCh * OpenSSLCryptoProvider::getProviderName() const {

	return DSIGConstants::s_unicodeStrPROVOpenSSL;

}
	// Hashing classes

XSECCryptoHash	* OpenSSLCryptoProvider::hashSHA1() const {

	OpenSSLCryptoHash * ret;

	XSECnew(ret, OpenSSLCryptoHash(XSECCryptoHash::HASH_SHA1));

	return ret;

}

XSECCryptoHash * OpenSSLCryptoProvider::hashHMACSHA1() const {

	OpenSSLCryptoHashHMAC * ret;

	XSECnew(ret, OpenSSLCryptoHashHMAC(XSECCryptoHash::HASH_SHA1));

	return ret;

}

XSECCryptoHash	* OpenSSLCryptoProvider::hashSHA(int length) const {


	OpenSSLCryptoHash * ret;

	switch (length) {

	case 160: XSECnew(ret, OpenSSLCryptoHash(XSECCryptoHash::HASH_SHA1));
		break;
	case 224: XSECnew(ret, OpenSSLCryptoHash(XSECCryptoHash::HASH_SHA224));
		break;
	case 256: XSECnew(ret, OpenSSLCryptoHash(XSECCryptoHash::HASH_SHA256));
		break;
	case 384: XSECnew(ret, OpenSSLCryptoHash(XSECCryptoHash::HASH_SHA384));
		break;
	case 512: XSECnew(ret, OpenSSLCryptoHash(XSECCryptoHash::HASH_SHA512));
		break;
	default:
		ret = NULL;
	}

	return ret;

}

XSECCryptoHash * OpenSSLCryptoProvider::hashHMACSHA(int length) const {

	OpenSSLCryptoHashHMAC * ret;

	switch (length) {

	case 160: XSECnew(ret, OpenSSLCryptoHashHMAC(XSECCryptoHash::HASH_SHA1));
		break;
	case 224: XSECnew(ret, OpenSSLCryptoHashHMAC(XSECCryptoHash::HASH_SHA224));
		break;
	case 256: XSECnew(ret, OpenSSLCryptoHashHMAC(XSECCryptoHash::HASH_SHA256));
		break;
	case 384: XSECnew(ret, OpenSSLCryptoHashHMAC(XSECCryptoHash::HASH_SHA384));
		break;
	case 512: XSECnew(ret, OpenSSLCryptoHashHMAC(XSECCryptoHash::HASH_SHA512));
		break;
	default:
		ret = NULL;
	}

	return ret;

}

XSECCryptoHash	* OpenSSLCryptoProvider::hashMD5() const {

	OpenSSLCryptoHash * ret;

	XSECnew(ret, OpenSSLCryptoHash(XSECCryptoHash::HASH_MD5));

	return ret;

}

XSECCryptoHash * OpenSSLCryptoProvider::hashHMACMD5() const {

	OpenSSLCryptoHashHMAC * ret;

	XSECnew(ret, OpenSSLCryptoHashHMAC(XSECCryptoHash::HASH_MD5));

	return ret;

}

XSECCryptoKeyHMAC * OpenSSLCryptoProvider::keyHMAC(void) const {

	OpenSSLCryptoKeyHMAC * ret;
	XSECnew(ret, OpenSSLCryptoKeyHMAC);

	return ret;

}

XSECCryptoKeyDSA * OpenSSLCryptoProvider::keyDSA() const {
	
	OpenSSLCryptoKeyDSA * ret;

	XSECnew(ret, OpenSSLCryptoKeyDSA());

	return ret;

}

XSECCryptoKeyEC * OpenSSLCryptoProvider::keyEC() const {
	
#ifdef XSEC_OPENSSL_HAVE_EC
	OpenSSLCryptoKeyEC * ret;

	XSECnew(ret, OpenSSLCryptoKeyEC());

	return ret;
#else
    throw XSECCryptoException(XSECCryptoException::UnsupportedError,
        "OpenSSLCryptoProvider::keyEC - EC support not available");
#endif
}

XSECCryptoKeyRSA * OpenSSLCryptoProvider::keyRSA() const {
	
	OpenSSLCryptoKeyRSA * ret;

	XSECnew(ret, OpenSSLCryptoKeyRSA());

	return ret;

}

XSECCryptoKey* OpenSSLCryptoProvider::keyDER(const char* buf, unsigned long len, bool base64) const {

    EVP_PKEY* pkey = NULL;

    if (base64) {
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

        BIO* b = BIO_new_mem_buf((void*)outBuf, bufLen);
        pkey = d2i_PUBKEY_bio(b, NULL);
        BIO_free(b);

    }
    else {

        BIO* b = BIO_new_mem_buf((void*)buf, len);
        pkey = d2i_PUBKEY_bio(b, NULL);
        BIO_free(b);

    }

    if (pkey) {
        XSECCryptoKey* ret = NULL;
        try {
            switch (pkey->type) {
                case EVP_PKEY_RSA:
                    ret = new OpenSSLCryptoKeyRSA(pkey);
                    break;

                case EVP_PKEY_DSA:
                    ret = new OpenSSLCryptoKeyDSA(pkey);
                    break;

#if defined(XSEC_OPENSSL_HAVE_EC)
                case EVP_PKEY_EC:
                    ret = new OpenSSLCryptoKeyEC(pkey);
                    break;
#endif
            }
        }
        catch (XSECCryptoException&) {
            EVP_PKEY_free(pkey);
            throw;
        }

        EVP_PKEY_free(pkey);
        return ret;
    }

    throw XSECCryptoException(XSECCryptoException::UnsupportedError,
		"OpenSSLCryptoProvider::keyDER - Error decoding public key"); 
}


XSECCryptoX509 * OpenSSLCryptoProvider::X509() const {

	OpenSSLCryptoX509 * ret;

	XSECnew(ret, OpenSSLCryptoX509());

	return ret;

}

XSECCryptoBase64 * OpenSSLCryptoProvider::base64() const {

#if 0
	OpenSSLCryptoBase64 * ret;

	XSECnew(ret, OpenSSLCryptoBase64());
#else
	XSCryptCryptoBase64 *ret;
	XSECnew(ret, XSCryptCryptoBase64);

#endif
	return ret;

}

bool OpenSSLCryptoProvider::algorithmSupported(XSECCryptoSymmetricKey::SymmetricKeyType alg) const {

	switch (alg) {

	case (XSECCryptoSymmetricKey::KEY_AES_128) :
	case (XSECCryptoSymmetricKey::KEY_AES_192) :
	case (XSECCryptoSymmetricKey::KEY_AES_256) :

#if !defined (XSEC_OPENSSL_HAVE_AES)
		return false;
#endif
	case (XSECCryptoSymmetricKey::KEY_3DES_192) :

		return true;

	default:

		return false;

	}

	return false;

}

bool OpenSSLCryptoProvider::algorithmSupported(XSECCryptoHash::HashType alg) const {

	switch (alg) {

	case (XSECCryptoHash::HASH_SHA1) :
	case (XSECCryptoHash::HASH_MD5) :
		return true;

	case (XSECCryptoHash::HASH_SHA224) :
	case (XSECCryptoHash::HASH_SHA256) :
#if defined(XSEC_OPENSSL_HAVE_SHA2) && !defined(OPENSSL_NO_SHA256)
	    return true;
#else
        return false;
#endif

	case (XSECCryptoHash::HASH_SHA384) :
	case (XSECCryptoHash::HASH_SHA512) :
#if defined(XSEC_OPENSSL_HAVE_SHA2) && !defined(OPENSSL_NO_SHA512)
		return true;
#else
		return false;
#endif

	default:
		return false;
	}

	return false;

}


XSECCryptoSymmetricKey	* OpenSSLCryptoProvider::keySymmetric(XSECCryptoSymmetricKey::SymmetricKeyType alg) const {

	OpenSSLCryptoSymmetricKey * ret;

	XSECnew(ret, OpenSSLCryptoSymmetricKey(alg));

	return ret;

}

unsigned int OpenSSLCryptoProvider::getRandom(unsigned char * buffer, unsigned int numOctets) const {

	if (RAND_status() != 1) {

		throw XSECCryptoException(XSECCryptoException::GeneralError,
			"OpenSSLCryptoProvider::getRandom - OpenSSL random not properly initialised"); 
	}

	int res = RAND_bytes(buffer, numOctets);

	if (res == 0) {

		throw XSECCryptoException(XSECCryptoException::GeneralError,
			"OpenSSLCryptoProvider::getRandom - Error obtaining random octets"); 
	
	}

	return numOctets;

}


#endif /* XSEC_HAVE_OPENSSL */
