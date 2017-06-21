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
 * XSECCryptoProvider := Base virtual class to define a hash algorithm
 *
 * Author(s): Berin Lautenbach
 *
 * $Id: XSECCryptoHash.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef XSECCRYPTOHASH_INCLUDE
#define XSECCRYPTOHASH_INCLUDE

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/enc/XSECCryptoKey.hpp>

#define XSEC_MAX_HASH_SIZE			256		/* Max size of any expected hash algorithms (oversized) */
#define XSEC_MAX_HASH_BLOCK_SIZE	64		/* Max size of blocks used - MD5 and SHA1 are both 64 bytes*/

/**
 * @ingroup crypto
 */

/**
 * \brief Interface definition for Hash functions.
 *
 * The XSECCryptoHash function is used by the library to manipulate
 * Hashing (Digest) functions.
 *
 * It is also used as the base class for HMAC functions, and thus has
 * a #setKey() method.
 *
 * @todo bring the interface here in-line with that provided for
 * XSECCryptoBase64 - really should have only one way of calling these
 * kinds of objects.
 *
 */

class DSIG_EXPORT XSECCryptoHash {


public :

	/**
	 * \brief Enumeration of Hash (Digest) types
	 *
	 * The hash types known to XSEC
	 */

	enum HashType {

		HASH_NONE			= 0,
		HASH_SHA1			= 1,
		HASH_MD5			= 2,
		HASH_SHA224			= 3,
		HASH_SHA256			= 4,
		HASH_SHA384			= 5,
		HASH_SHA512			= 6

	};

	// Constructors/Destructors
	
	XSECCryptoHash() {};
	virtual ~XSECCryptoHash() {};

	/** @name Digest/Hash functions */
	//@{

	/**
	 * \brief Rest the hash function
	 *
	 * XSEC will call the #reset() function prior to re-using a CryptoHash
	 * object.
	 */

	virtual void		reset(void) = 0;					// Reset the hash

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
							 unsigned int length) = 0;

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
								unsigned int maxLength) = 0;// Finish and get hash

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

	virtual HashType getHashType(void) const = 0;

	//@}

	/** @name HMAC Functions */
	//@{
	
	/**
	 *\brief
	 *
	 * The HMAC classes are treated in the library as standard hash
	 * objects that just happen to take a key.  Thus all hash functions
	 * implement this function (potentially just to throw an exception)
	 *
	 * Sets the key - which needs to have a base class of 
	 * XSECCryptoKeyHMAC.
	 *
	 * @param key The key the HMAC function should use.
	 */

	virtual void		setKey(XSECCryptoKey * key) = 0;

	//@}

};

#endif /* XSECCRYPTOHASH_INCLUDE */
