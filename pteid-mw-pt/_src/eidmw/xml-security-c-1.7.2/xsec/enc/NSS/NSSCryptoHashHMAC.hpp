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
 * NSSCryptoHashHMAC := NSS Implementation of HMAC
 *
 * Author(s): Milan Tomic
 *
 */

#ifndef NSSCRYPTOHASHHMAC_INCLUDE
#define NSSCRYPTOHASHHMAC_INCLUDE

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/enc/XSECCryptoHash.hpp>

#if defined (XSEC_HAVE_NSS)

#include <pk11func.h>
#include <nss.h>

/**
 * @ingroup nsscrypto
 * @{
 */

/**
 * \brief Implementation of HMAC Hash functions in the NSS provider
 *
 * Uses the NSS functions to implement an HMAC.
 *
 */

class DSIG_EXPORT NSSCryptoHashHMAC : public XSECCryptoHash {


public :

	/** @name Constructors and Destructors */
	//@{

	/**
	 * \brief Constructor
	 *
	 * Create the object, with the indicated algorithm
	 * (Currently supports MD5 and SHA1)
	 *
	 * @param alg Digest algorithm to use
	 */
	
	NSSCryptoHashHMAC(XSECCryptoHash::HashType alg);
	virtual ~NSSCryptoHashHMAC();

	//@}

	/** @name HMAC Functions */
	//@{
	
	/**
	 *\brief Set the HMAC key
	 *
	 * Sets the key - which needs to have a base class of 
	 * NSSCryptoKeyHMAC.
	 *
	 * @param key The key the HMAC function should use.
	 */

	virtual void setKey(XSECCryptoKey * key);

	//@}

	/** @name Hash functions */
	//@{

	/**
	 * \brief Reset the hash function
	 *
	 * Re-initialises the digest structure.
	 */

	virtual void reset(void);

	/**
	 * \brief Hash some data.
	 *
	 * Take length bytes of data from the data buffer and update the hash
	 * that already exists.  This function may (and normally will) be called
	 * many times for large blocks of data.
	 *
	 * @param data The buffer containing the data to be hashed.
	 * @param length The number of bytes to be read from data
	 */

	virtual void		hash(unsigned char * data, 
							 unsigned int length);

  /**
	 * \brief Finish up a Digest operation and read the result.
	 *
	 * This call tells the CryptoHash object that the input is complete and
	 * to finalise the Digest.  The output of the digest is read into the 
	 * hash buffer (at most maxLength bytes).  This is effectively the
	 * signature for the data that has been run through the HMAC function.
	 *
	 * @param hash The buffer the hash should be read into.
	 * @param maxLength The maximum number of bytes to be read into hash
	 * @returns The number of bytes copied into the hash buffer
	 */

	virtual unsigned int finish(unsigned char * hash,
								unsigned int maxLength);

	//@}

	/** @name Information functions */
	//@{

	/**
	 *\brief
	 *
	 * Determine the hash type of this object
	 *
	 * @returns The hash type
	 */

	virtual HashType getHashType(void) const;

	//@}

private:

	// Not implemented constructors
	NSSCryptoHashHMAC();

	unsigned char				m_mdValue[XSEC_MAX_HASH_SIZE];		// Final output
	unsigned int				m_mdLen;

	HashType	          m_hashType;

  PK11Context *       mp_md;

	unsigned char				m_ipadKeyed[XSEC_MAX_HASH_SIZE];
	unsigned char				m_opadKeyed[XSEC_MAX_HASH_SIZE];

	unsigned int				m_blockSize;		// Block size (bytes) for used alg.

  safeBuffer			    m_keyBuf;						// The loaded key
	unsigned int		    m_keyLen;						// The loaded key length

};

#endif /* XSEC_HAVE_NSS */
#endif /* NSSCRYPTOHASHHMAC_INCLUDE */
