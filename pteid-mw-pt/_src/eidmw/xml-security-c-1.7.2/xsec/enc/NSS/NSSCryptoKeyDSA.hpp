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
 * NSSCryptoKeyDSA := DSA Keys
 *
 * Author(s): Milan Tomic
 *
 */

#ifndef NSSCRYPTOKEYDSA_INCLUDE
#define NSSCRYPTOKEYDSA_INCLUDE

#include <xsec/enc/XSECCryptoKeyDSA.hpp>

#if defined (XSEC_HAVE_NSS)

#include <pk11func.h>
#include <keyhi.h>
#include <nss.h>

class NSSCryptoProvider;

/**
 * \ingroup nsscrypto
 * @{
 */

/**
 * \brief NSS implementation class for DSA keys.
 *
 * The library uses classes derived from this to process DSA keys.
 */


class DSIG_EXPORT NSSCryptoKeyDSA : public XSECCryptoKeyDSA {

public :

	/** @name Constructors and Destructors */
	//@{
	
	/**
	 * \brief Ctor for DSA keys
	 *
	 * You can set either public, private or both keys
	 *
	 * @param pubkey A handle to the public key (optional)
	 * @param privkey A handle to the private key (optional)
	 */

	NSSCryptoKeyDSA(SECKEYPublicKey * pubkey = NULL, SECKEYPrivateKey * privkey = NULL);
	
	virtual ~NSSCryptoKeyDSA();

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
	 *\brief Return the NSS string identifier
	 */

	virtual const XMLCh * getProviderName() const {return DSIGConstants::s_unicodeStrPROVNSS;}
	
	/**
	 * \brief Replicate key
	 */

	virtual XSECCryptoKey * clone() const;

	//@}

	/** @name Required DSA methods */
	//@{

	/**
	 * \brief Create a signature
	 *
	 * Sign the buffer using the internal private key.  Will throw a DSAError
	 * if the key is not set, or is not a private key.
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
						unsigned int base64SignatureBufLen);

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
						 unsigned int sigLen);

	//@}

	/** @name Optional Interface methods
	 * 
	 * Have been fully implemented in the NSS interface to allow interop
	 * testing to occur.
	 *
	 */
	//@{

	/**
	 * \brief Load P
	 *
	 * @param b64 Base64 encoded parameter - read from XML document
	 * @param len Length of the encoded string
	 */

	virtual void loadPBase64BigNums(const char * b64, unsigned int len);

	/**
	 * \brief Load Q
	 *
	 * @param b64 Base64 encoded parameter - read from XML document
	 * @param len Length of the encoded string
	 */
	
	virtual void loadQBase64BigNums(const char * b64, unsigned int len);

	/**
	 * \brief Load G
	 *
	 * @param b64 Base64 encoded parameter - read from XML document
	 * @param len Length of the encoded string
	 */

	virtual void loadGBase64BigNums(const char * b64, unsigned int len);
	
	/**
	 * \brief Load Y
	 *
	 * @param b64 Base64 encoded parameter - read from XML document
	 * @param len Length of the encoded string
	 */

	virtual void loadYBase64BigNums(const char * b64, unsigned int len);
	
	/**
	 * \brief Load J
	 *
	 * @param b64 Base64 encoded parameter - read from XML document
	 * @param len Length of the encoded string
	 */

	virtual void loadJBase64BigNums(const char * b64, unsigned int len);

	//@}

	/** @name NSS Specific functions */
	//@{

	/**
	 * \brief Retrieve P
	 *
	 * @param b64 Buffer to place ds:CryptoBinary encoded P parameter in
	 * @param len Max amount of data to be placed in buffer
	 * @returns The number of bytes placed in the buffer
	 */

	unsigned int getPBase64BigNums(char * b64, unsigned int len);

	/**
	 * \brief Retrieve Q
	 *
	 * @param b64 Buffer to place ds:CryptoBinary encoded Q parameter in
	 * @param len Max amount of data to be placed in buffer
	 * @returns The number of bytes placed in the buffer
	 */
	
	unsigned int getQBase64BigNums(char * b64, unsigned int len);

	/**
	 * \brief Retrieve G
	 *
	 * @param b64 Buffer to place ds:CryptoBinary encoded G parameter in
	 * @param len Max amount of data to be placed in buffer
	 * @returns The number of bytes placed in the buffer
	 */
	
	unsigned int getGBase64BigNums(char * b64, unsigned int len);

	/**
	 * \brief Retrieve Y
	 *
	 * @param b64 Buffer to place ds:CryptoBinary encoded Y parameter in
	 * @param len Max amount of data to be placed in buffer
	 * @returns The number of bytes placed in the buffer
	 */
	
	unsigned int getYBase64BigNums(char * b64, unsigned int len);

	//@}

private:

	SECKEYPublicKey  *	mp_pubkey;
	SECKEYPrivateKey *	mp_privkey;

  SECItem * mp_P;
  SECItem * mp_Q;
  SECItem * mp_G;
  SECItem * mp_Y;

	// Instruct to import from parameters

	void importKey(void);
	void loadParamsFromKey(void);

};

#endif /* XSEC_HAVE_NSS */
#endif /* NSSCRYPTOKEYDSA_INCLUDE */
