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
 * XSECCryptoKey := Container class to hold cryptographic keys
 *					Should be re-implemented by all providers as the base class
 *                  for all keys
 *
 * Author(s): Berin Lautenbach
 *
 * $Id: XSECCryptoKey.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */



#ifndef XSECCRYPTOKEY_INCLUDE
#define XSECCRYPTOKEY_INCLUDE

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/dsig/DSIGConstants.hpp>

/**
 * \ingroup crypto
 */

/**
 * \brief Base interface class for key material.
 *
 * All keys used for signing and encrypting are derived from this
 * base interface class.  There are no methods for performing
 * cryptographic functions, as this is a base class used to allow
 * the library to pass key material to various objects without
 * knowing how to directly use it.
 */

class DSIG_EXPORT XSECCryptoKey {

public :

	/**
	 * \brief Key types understood by the library
	 *
	 * This type defines the list of key types that the library
	 * understands.
	 */

	enum KeyType {

		KEY_NONE,
		KEY_DSA_PUBLIC,
		KEY_DSA_PRIVATE,
		KEY_DSA_PAIR,
		KEY_RSA_PUBLIC,
		KEY_RSA_PRIVATE,
		KEY_RSA_PAIR,
		KEY_HMAC,
		KEY_SYMMETRIC,
		KEY_EC_PUBLIC,
		KEY_EC_PRIVATE,
		KEY_EC_PAIR
	
	};


	/** @name Constructors and Destructors */
	//@{
	
	/**
	 * \brief Constructor
	 **/

	XSECCryptoKey() {};

	/**
	 * \brief Destructor 
	 *
	 * Implementations must ensure that the held key is properly destroyed
	 * (overwritten) when key objects are deleted.
	 */

	virtual ~XSECCryptoKey() {};

	//@}

	/** @name Interface classes */
	//@{

	/**
	 * \brief Returns the type of this key.
	 */

	virtual KeyType getKeyType() const {return KEY_NONE;}

	/**
	 * \brief Returns a string that identifies the crypto owner of this library.
	 */

	virtual const XMLCh * getProviderName() const = 0;

	/**
	 * \brief Clone the key
	 *
	 * All keys need to be able to copy themselves and return
	 * a pointer to the copy.  This allows the library to 
	 * duplicate keys.
	 */

	virtual XSECCryptoKey * clone() const = 0;

  //@}

};


#endif /* XSECCRYPTOKEY_INCLUDE */
