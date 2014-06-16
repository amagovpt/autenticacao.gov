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
 * XSECCryptoKeyDSA := DSA Keys
 *
 * Author(s): Berin Lautenbach
 *
 * $Id: XSECCryptoKeyDSA.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef XSECCRYPTOKEYDSA_INCLUDE
#define XSECCRYPTOKEYDSA_INCLUDE

#include <xsec/enc/XSECCryptoKey.hpp>

/**
 * \ingroup crypto
 */

/**
 * \brief Interface class for DSA keys.
 *
 * The library uses classes derived from this to process DSA keys.
 */

class DSIG_EXPORT XSECCryptoKeyDSA : public XSECCryptoKey {

public :

	/** @name Constructors and Destructors */
	//@{
	
	XSECCryptoKeyDSA() {};
	virtual ~XSECCryptoKeyDSA() {};

	//@}

	/** @name Key Interface methods */
	//@{

	/**
	 * \brief Return the type of this key.
	 *
	 * For DSA keys, this allows people to determine whether this is a 
	 * public key, private key or a key pair
	 */

	virtual XSECCryptoKey::KeyType getKeyType() const {return KEY_NONE;}

	/**
	 * \brief Replicate key
	 */

	virtual XSECCryptoKey * clone() const = 0;

	//@}

	/** @name Mandatory DSA interface methods 
	 *
	 * These methods are required by the library.
	 */
	//@{

	/**
	 * \brief Verify a signature
	 *
	 * The library will call this function to validate a signature
	 *
	 * @param hashBuf Buffer containing the pre-calculated (binary) digest
	 * @param hashLen Length of the data in the digest buffer
	 * @param base64Signature Buffer containing the Base64 encoded signature
	 * @param sigLen Length of the data in the signature buffer
	 * @returns true if the signature was valid, false otherwise
	 */

	virtual bool verifyBase64Signature(unsigned char * hashBuf, 
								 unsigned int hashLen,
								 char * base64Signature,
								 unsigned int sigLen) = 0;

	/**
	 * \brief Create a signature
	 *
	 * The library will call this function to create a signature from
	 * a pre-calculated digest.  The output signature is required to
	 * be Base64 encoded such that it can be placed directly into the
	 * XML document
	 *
	 * @param hashBuf Buffer containing the pre-calculated (binary) digest
	 * @param hashLen Number of bytes of hash in the hashBuf
	 * @param base64SignatureBuf Buffer to place the base64 encoded result
	 * in.
	 * @param base64SignatureBufLen Implementations need to ensure they do
	 * not write more bytes than this into the buffer
	 */

	virtual unsigned int signBase64Signature(unsigned char * hashBuf,
		unsigned int hashLen,
		char * base64SignatureBuf,
		unsigned int base64SignatureBufLen) = 0;

	//@}

	/** @name Optional Interface methods
	 * 
	 * These functions do not necessarily have to be implmented.  They
	 * are used by XSECKeyInfoResolverDefault to try to create a key from
	 * KeyInfo elements without knowing anything else.
	 *
	 * If an interface class does not implement these functions, a simple
	 * stub that does nothing should be used.
	 */
	//@{

	/**
	 * \brief Load P
	 *
	 * @param b64 Base64 encoded parameter - read from XML document
	 * @param len Length of the encoded string
	 */

	virtual void loadPBase64BigNums(const char * b64, unsigned int len) = 0;

	/**
	 * \brief Load Q
	 *
	 * @param b64 Base64 encoded parameter - read from XML document
	 * @param len Length of the encoded string
	 */

	virtual void loadQBase64BigNums(const char * b64, unsigned int len) = 0;

	/**
	 * \brief Load G
	 *
	 * @param b64 Base64 encoded parameter - read from XML document
	 * @param len Length of the encoded string
	 */

	virtual void loadGBase64BigNums(const char * b64, unsigned int len) = 0;

	/**
	 * \brief Load Y
	 *
	 * @param b64 Base64 encoded parameter - read from XML document
	 * @param len Length of the encoded string
	 */

	virtual void loadYBase64BigNums(const char * b64, unsigned int len) = 0;

	/**
	 * \brief Load J
	 *
	 * @param b64 Base64 encoded parameter - read from XML document
	 * @param len Length of the encoded string
	 */

	virtual void loadJBase64BigNums(const char * b64, unsigned int len) = 0;

	//@}
};


#endif /* XSECCRYPTOKEYDSA_INCLUDE */
