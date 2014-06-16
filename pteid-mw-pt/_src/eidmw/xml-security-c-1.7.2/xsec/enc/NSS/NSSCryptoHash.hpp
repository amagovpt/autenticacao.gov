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
 * NSSCryptoHash := NSS Implementation of Message digests
 *
 * Author(s): Milan Tomic
 *
 */

#ifndef NSSCRYPTOHASH_INCLUDE
#define NSSCRYPTOHASH_INCLUDE

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
 * \brief NSS Implementation of Hash functions.
 *
 * Uses the NSS functions to perform digest functions.
 *
 */

class DSIG_EXPORT NSSCryptoHash : public XSECCryptoHash {


public :

	/** @name Constructors/Destructors */
	//@{
	/**
	 * \brief Construct a Hash object
	 *
	 * Creates a NSS based hash object of the required type.
	 *
	 * @param alg The algorithm to use for digest operations
	 */

	NSSCryptoHash(XSECCryptoHash::HashType alg);
	virtual ~NSSCryptoHash();

	//@}

	/** @name HMAC Functions */
	//@{
	
	/**
	 *\brief
	 *
	 * Does nothing. If the required function is an HMAC function,
	 * then NSSCryptoHashHMAC should be used.
	 *
	 * @param key The key the HMAC function should use.
	 */

	virtual void setKey(XSECCryptoKey * key) {}

	//@}

	/** @name Digest/Hash functions */
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
	 * hash buffer (at most maxLength bytes)
	 *
	 * @param hash The buffer the hash should be read into.
	 * @param maxLength The maximum number of bytes to be read into hash
	 * @returns The number of bytes copied into the hash buffer
	 */

	virtual unsigned int finish(unsigned char * hash,
								unsigned int maxLength);// Finish and get hash

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
	NSSCryptoHash();

	unsigned char				m_mdValue[XSEC_MAX_HASH_SIZE];		// Final output
	unsigned int				m_mdLen;

	HashType	          m_hashType;

  PK11Context *       mp_md;

};

#endif /* XSEC_HAVE_NSS */
#endif /* NSSCRYPTOHASHSHA1_INCLUDE */
