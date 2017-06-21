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

#ifndef OPENSSLCRYPTOKEYEC_INCLUDE
#define OPENSSLCRYPTOKEYEC_INCLUDE

#include <xsec/enc/XSECCryptoKeyEC.hpp>

#if defined (XSEC_HAVE_OPENSSL) && defined(XSEC_OPENSSL_HAVE_EC)
#include <openssl/ec.h>
#include <openssl/evp.h>

/**
 * \ingroup opensslcrypto
 */

/**
 * \brief OpenSSL implementation class for EC keys.
 *
 * The library uses classes derived from this to process EC keys.
 */


class DSIG_EXPORT OpenSSLCryptoKeyEC : public XSECCryptoKeyEC {

public :

	/** @name Constructors and Destructors */
	//@{
	
	OpenSSLCryptoKeyEC();

	/**
	 * \brief Destructor
	 *
	 * Will call the OpenSSL function to destroy the EC key - which will
	 * also overwrite any Private keys
	 */

	virtual ~OpenSSLCryptoKeyEC();

	//@}

	/** @name Required Key Interface methods */
	//@{

	/**
	 * \brief Return the type of this key.
	 *
	 * Allows the caller (and library) to determine whether this is a 
	 * public key, private key or a key pair
	 */

	virtual XSECCryptoKey::KeyType getKeyType() const;

	/**
	 *\brief Return the OpenSSL string identifier
	 */

	virtual const XMLCh * getProviderName() const {return DSIGConstants::s_unicodeStrPROVOpenSSL;}

	/**
	 * \brief Replicate key
	 */

	virtual XSECCryptoKey * clone() const;

	//@}

	/** @name Required EC methods */
	//@{

	/**
	 * \brief Create an EC-DSA signature
	 *
	 * Sign the buffer using the internal private key.  Will throw an ECError
	 * if the key is not set, or is not a private key.
	 *
	 * @param hashBuf Buffer containing the pre-calculated (binary) digest
	 * @param hashLen Number of bytes of hash in the hashBuf
	 * @param base64SignatureBuf Buffer to place the base64 encoded result
	 * in.
	 * @param base64SignatureBufLen Implementations need to ensure they do
	 * not write more bytes than this into the buffer
	 */

	virtual unsigned int signBase64SignatureDSA(unsigned char * hashBuf,
		unsigned int hashLen,
		char * base64SignatureBuf,
		unsigned int base64SignatureBufLen);

	/**
	 * \brief Verify an EC-DSA signature
	 *
	 * The library will call this function to validate a signature
	 *
	 * @param hashBuf Buffer containing the pre-calculated (binary) digest
	 * @param hashLen Length of the data in the digest buffer
	 * @param base64Signature Buffer containing the Base64 encoded signature
	 * @param sigLen Length of the data in the signature buffer
	 * @returns true if the signature was valid, false otherwise
	 */


	virtual bool verifyBase64SignatureDSA(unsigned char * hashBuf, 
							 unsigned int hashLen,
							 char * base64Signature,
							 unsigned int sigLen);
	
	//@}

	/** @name Optional Interface methods
	 * 
	 * Have been fully implemented in the OpenSSL interface to allow interop
	 * testing to occur.
	 *
	 */
	//@{

    virtual void loadPublicKeyBase64(const char* curveName,
        const char* b64,
        unsigned int len);

	//@}

	/** @name OpenSSL Specific functions */
	//@{

	/**
	 * \brief Library specific constructor
	 *
	 * Used to create a new key around an OpenSSL EVP_PKEY object
	 * holding an EC key
	 */

	OpenSSLCryptoKeyEC(EVP_PKEY *k);

	/**
	 * \brief Get OpenSSL EC_KEY structure
	 */

	EC_KEY * getOpenSSLEC(void) {return mp_ecKey;}

    /**
	 * \brief Get OpenSSL EC_KEY structure
	 */

	const EC_KEY * getOpenSSLEC(void) const {return mp_ecKey;}

	//@}
	//@}


private:

	XSECCryptoKey::KeyType			m_keyType;
	EC_KEY					        * mp_ecKey;
	
};

#endif /* XSEC_HAVE_OPENSSL */
#endif /* XSECCRYPTOKEYEC_INCLUDE */
