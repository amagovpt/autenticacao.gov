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
 * OpenSSLCryptoKeyEC := EC Keys
 *
 * Author(s): Scott Cantor
 *
 * $Id:$
 *
 */
#include <xsec/framework/XSECDefs.hpp>
#if defined (XSEC_HAVE_OPENSSL) && defined (XSEC_OPENSSL_HAVE_EC)

#include <xsec/enc/OpenSSL/OpenSSLCryptoKeyEC.hpp>
#include <xsec/enc/OpenSSL/OpenSSLCryptoBase64.hpp>
#include <xsec/enc/OpenSSL/OpenSSLCryptoProvider.hpp>
#include <xsec/enc/XSECCryptoException.hpp>
#include <xsec/enc/XSECCryptoUtils.hpp>
#include <xsec/enc/XSCrypt/XSCryptCryptoBase64.hpp>
#include <xsec/framework/XSECError.hpp>
#include <xsec/utils/XSECPlatformUtils.hpp>

#include <xercesc/util/Janitor.hpp>

XSEC_USING_XERCES(Janitor);
XSEC_USING_XERCES(ArrayJanitor);


#include <openssl/ecdsa.h>

OpenSSLCryptoKeyEC::OpenSSLCryptoKeyEC() : mp_ecKey(NULL) {
};

OpenSSLCryptoKeyEC::~OpenSSLCryptoKeyEC() {


	// If we have a EC_KEY, delete it
	// OpenSSL will ensure the memory holding any private key is freed.

	if (mp_ecKey)
		EC_KEY_free(mp_ecKey);

};

// Generic key functions

XSECCryptoKey::KeyType OpenSSLCryptoKeyEC::getKeyType() const {

	// Find out what we have
	if (mp_ecKey == NULL)
		return KEY_NONE;

	if (EC_KEY_get0_private_key(mp_ecKey) && EC_KEY_get0_public_key(mp_ecKey))
		return KEY_EC_PAIR;

	if (EC_KEY_get0_private_key(mp_ecKey))
		return KEY_EC_PRIVATE;

	if (EC_KEY_get0_public_key(mp_ecKey))
		return KEY_EC_PUBLIC;

	return KEY_NONE;

}

void OpenSSLCryptoKeyEC::loadPublicKeyBase64(const char* curveName, const char * buf, unsigned int len) {

    if (mp_ecKey) {
        EC_KEY_free(mp_ecKey);
        mp_ecKey = NULL;
    }

    EC_KEY* key = EC_KEY_new_by_curve_name(static_cast<OpenSSLCryptoProvider*>(XSECPlatformUtils::g_cryptoProvider)->curveNameToNID(curveName));

	int bufLen = len;
	unsigned char * outBuf;
	XSECnew(outBuf, unsigned char[len + 1]);
	ArrayJanitor<unsigned char> j_outBuf(outBuf);

	XSCryptCryptoBase64 *b64;
	XSECnew(b64, XSCryptCryptoBase64);
	Janitor<XSCryptCryptoBase64> j_b64(b64);

	b64->decodeInit();
	bufLen = b64->decode((unsigned char *) buf, len, outBuf, len);
	bufLen += b64->decodeFinish(&outBuf[bufLen], len-bufLen);

    if (bufLen > 0) {
        if (o2i_ECPublicKey(&key, (const unsigned char **) &outBuf, bufLen) == NULL) {
            EC_KEY_free(key);
            key = NULL;
        }
    }

	if (key == NULL) {

		throw XSECCryptoException(XSECCryptoException::ECError,
		"OpenSSL:EC - Error translating Base64 octets into OpenSSL EC_KEY structure");

	}

    mp_ecKey = key;
}


// "Hidden" OpenSSL functions

OpenSSLCryptoKeyEC::OpenSSLCryptoKeyEC(EVP_PKEY *k) {

	// Create a new key to be loaded as we go

	if (k == NULL || k->type != EVP_PKEY_EC)
		return;	// Nothing to do with us

    mp_ecKey = EC_KEY_dup(k->pkey.ec);
}

// --------------------------------------------------------------------------------
//           Verify a signature encoded as a Base64 string
// --------------------------------------------------------------------------------

bool OpenSSLCryptoKeyEC::verifyBase64SignatureDSA(unsigned char * hashBuf,
								 unsigned int hashLen,
								 char * base64Signature,
								 unsigned int sigLen) {

	// Use the currently loaded key to validate the Base64 encoded signature

	if (mp_ecKey == NULL) {

		throw XSECCryptoException(XSECCryptoException::ECError,
			"OpenSSL:EC - Attempt to validate signature with empty key");
	}

	char * cleanedBase64Signature;
	unsigned int cleanedBase64SignatureLen = 0;

	cleanedBase64Signature =
		XSECCryptoBase64::cleanBuffer(base64Signature, sigLen, cleanedBase64SignatureLen);
	ArrayJanitor<char> j_cleanedBase64Signature(cleanedBase64Signature);

	int sigValLen;
	unsigned char* sigVal = new unsigned char[sigLen + 1];
    ArrayJanitor<unsigned char> j_sigVal(sigVal);

	EVP_ENCODE_CTX m_dctx;
	EVP_DecodeInit(&m_dctx);
	int rc = EVP_DecodeUpdate(&m_dctx,
						  sigVal,
						  &sigValLen,
						  (unsigned char *) cleanedBase64Signature,
						  cleanedBase64SignatureLen);

	if (rc < 0) {

		throw XSECCryptoException(XSECCryptoException::ECError,
			"OpenSSL:EC - Error during Base64 Decode");
	}
	int t = 0;

	EVP_DecodeFinal(&m_dctx, &sigVal[sigValLen], &t);

	sigValLen += t;

    if (sigValLen <= 0 || sigValLen % 2 != 0) {
		throw XSECCryptoException(XSECCryptoException::ECError,
			"OpenSSL:EC - Signature length was odd");
    }

	// Translate to BNs by splitting in half, and thence to ECDSA_SIG

	ECDSA_SIG * dsa_sig = ECDSA_SIG_new();
	dsa_sig->r = BN_bin2bn(sigVal, sigValLen / 2, NULL);
	dsa_sig->s = BN_bin2bn(&sigVal[sigValLen / 2], sigValLen / 2, NULL);

	// Now we have a signature and a key - lets check

	int err = ECDSA_do_verify(hashBuf, hashLen, dsa_sig, mp_ecKey);

	ECDSA_SIG_free(dsa_sig);

	if (err < 0) {

		throw XSECCryptoException(XSECCryptoException::ECError,
			"OpenSSL:EC - Error validating signature");
	}

	return (err == 1);

}

// --------------------------------------------------------------------------------
//           Sign and encode result as a Base64 string
// --------------------------------------------------------------------------------


unsigned int OpenSSLCryptoKeyEC::signBase64SignatureDSA(unsigned char * hashBuf,
		unsigned int hashLen,
		char * base64SignatureBuf,
		unsigned int base64SignatureBufLen) {

	// Sign a pre-calculated hash using this key

	if (mp_ecKey == NULL) {

		throw XSECCryptoException(XSECCryptoException::ECError,
			"OpenSSL:EC - Attempt to sign data with empty key");
	}

	ECDSA_SIG * dsa_sig;

	dsa_sig = ECDSA_do_sign(hashBuf, hashLen, mp_ecKey);

	if (dsa_sig == NULL) {

		throw XSECCryptoException(XSECCryptoException::ECError,
			"OpenSSL:EC - Error signing data");

	}

	// Now turn the signature into a base64 string
	unsigned char* rawSigBuf = new unsigned char[(BN_num_bits(dsa_sig->r) + BN_num_bits(dsa_sig->s)) / 8];
    ArrayJanitor<unsigned char> j_sigbuf(rawSigBuf);

	unsigned int rawLen = BN_bn2bin(dsa_sig->r, rawSigBuf);

    if (rawLen <= 0) {

		throw XSECCryptoException(XSECCryptoException::ECError,
			"OpenSSL:EC - Error converting signature to raw buffer");

	}

	unsigned int rawLenS = BN_bn2bin(dsa_sig->s, (unsigned char *) &rawSigBuf[rawLen]);

    if (rawLenS <= 0) {

		throw XSECCryptoException(XSECCryptoException::ECError,
			"OpenSSL:EC - Error converting signature to raw buffer");

	}

	rawLen += rawLenS;

	// Now convert to Base 64

	BIO * b64 = BIO_new(BIO_f_base64());
	BIO * bmem = BIO_new(BIO_s_mem());

	BIO_set_mem_eof_return(bmem, 0);
	b64 = BIO_push(b64, bmem);

	// Translate signature to Base64

	BIO_write(b64, rawSigBuf, rawLen);
	BIO_flush(b64);

	unsigned int sigValLen = BIO_read(bmem, base64SignatureBuf, base64SignatureBufLen);

	BIO_free_all(b64);

	if (sigValLen <= 0) {

		throw XSECCryptoException(XSECCryptoException::ECError,
			"OpenSSL:EC - Error base64 encoding signature");
	}

	return sigValLen;

}



XSECCryptoKey * OpenSSLCryptoKeyEC::clone() const {

	OpenSSLCryptoKeyEC * ret;

	XSECnew(ret, OpenSSLCryptoKeyEC);

	ret->m_keyType = m_keyType;
    if (mp_ecKey)
        ret->mp_ecKey = EC_KEY_dup(mp_ecKey);

	return ret;

}

#endif /* XSEC_HAVE_OPENSSL */
