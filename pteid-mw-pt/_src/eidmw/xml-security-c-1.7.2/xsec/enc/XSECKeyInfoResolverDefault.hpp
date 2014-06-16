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
 * XSECKeyInfoResolverDefault := Default (very basic) class for applications
 *						 to map KeyInfo to keys
 *
 * Author(s): Berin Lautenbach
 *
 * $ID$
 *
 * $LOG$
 *
 */

#ifndef XSECKEYINFORESOLVERDEFAULT_INCLUDE
#define XSECKEYINFORESOLVERDEFAULT_INCLUDE

#include <xsec/enc/XSECKeyInfoResolver.hpp>

/**
 * @ingroup interfaces
 */
/*\@{*/

/**
 * @brief A basic class for mapping KeyInfo elements to Keys.
 *
 * This class is provides a method for applications to extract
 * keys from KeyInfo elements that have a public key embedded.
 *
 * The class makes use of the makeKey() method available in all
 * keyInfo elements.  (Which in turn relies on the clone*Key
 * methods available from the Encryption provider - these may
 * not be available, so this class may not work well in these
 * cases.)
 *
 * @note This class is <B>very</B> basic.  It does not validate
 * the trustworthyness of the key in any way.  It simply reads
 * the key from the first KeyInfo element that actually holds it
 * and returns the result (or NULL) if none is found.  It is mainly
 * provided to allow for interoperability testing.
 *
 */

class DSIG_EXPORT XSECKeyInfoResolverDefault : public XSECKeyInfoResolver {

public :

	/** @name Constructors and Destructors */
	//@{

	XSECKeyInfoResolverDefault();
	virtual ~XSECKeyInfoResolverDefault();

	//@}

	/** @name Interface Functions */
	//@{

	/**
	 * \brief Provide a key to the library
	 *
	 * Reads through the KeyInfo elements until it finds an element
	 * with an embedded Key.  If none is found, returns NULL.
	 *
	 * @param lst The list of KeyInfo elements from the signature
	 * @returns Either the appropriate key or NULL if none can be found
	 * @see XSECKeyInfoResolver::resolveKey()
	 */

	virtual XSECCryptoKey * resolveKey(DSIGKeyInfoList * lst);

	/**
	 * \brief Clone the resolver to be installed in a new object.
	 *
	 * @see XSECKeyInfoResolver::clone()
	 */

	virtual XSECKeyInfoResolver * clone(void) const;

	//@}

private:

	XSECSafeBufferFormatter		* mp_formatter;

	/*\@}*/
};


#endif /* XSECKEYINFORESOLVERDEFAULT_INCLUDE */
