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
 * XSECCryptoKeyEC := EC Keys
 *
 * Author(s): Scott Cantor
 *
 * $Id:$
 *
 */

#ifndef XSECCRYPTOKEYEC_INCLUDE
#define XSECCRYPTOKEYEC_INCLUDE

#include <xsec/enc/XSECCryptoKey.hpp>

/**
 * \ingroup crypto
 */

/**
 * \brief Interface class for RSA keys.
 *
 * The library uses classes derived from this to process RSA keys.
 */


class DSIG_EXPORT XSECCryptoKeyEC : public XSECCryptoKey {

public :

	/** @name Constructors and Destructors */
	//@{

	XSECCryptoKeyEC() {};
	virtual ~XSECCryptoKeyEC() {};

	//@}

	/** @name Key Interface methods */
	//@{

	/**
	 * \brief Return the type of this key.
	 *
	 * For EC keys, this allows people to determine whether this is a 
	 * public key, private key or a key pair
	 */

	virtual XSECCryptoKey::KeyType getKeyType() const {return KEY_NONE;}
	
	/**
	 * \brief Replicate key
	 */

	virtual XSECCryptoKey * clone() const = 0;

	//@}

	/** @name Mandatory EC interface methods 
	 *
	 * These methods are required by the library.
	 */
	//@{

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
								 unsigned int sigLen) = 0;

	/**
	 * \brief Create an EC-DSA signature
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

	virtual unsigned int signBase64SignatureDSA(unsigned char * hashBuf,
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
	 * \brief Load the key
	 *
	 * Load the key for a named curve from a Base64 encoded string
	 *
     * param curveName  a URI identifying the curve (typically an OID URN)
	 * param b64 A buffer containing the encoded string
	 * param len The length of the data in the buffer
	 */

	virtual void loadPublicKeyBase64(const char* curveName,
        const char* b64,
        unsigned int len) = 0;

	//@}

};



#endif /* XSECCRYPTOKEYEC_INCLUDE */
