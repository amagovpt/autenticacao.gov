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
 * XSECKeyInfoResolver := Virtual interface class for applications
 *						 to map KeyInfo to keys
 *
 * Author(s): Berin Lautenbach
 *
 * $Id: XSECKeyInfoResolver.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef XSECKEYINFORESOLVER_INCLUDE
#define XSECKEYINFORESOLVER_INCLUDE

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/dsig/DSIGKeyInfoList.hpp>
#include <xsec/enc/XSECCryptoKey.hpp>

/**
 * @ingroup interfaces
 */
/*\@{*/

/**
 * @brief Interface class for providing keys to the library.
 *
 * The XSEC library does not perform actual mapping of KeyInfo
 * elements to encryption keys.  (In some cases a KeyInfo might
 * not even be provided).
 *
 * This interface class allows the application to perform this
 * key mapping and provision function.  Applications can perform
 * whatever steps are necessary to determine the appropriate key
 * and what trust level it might have.
 *
 */

class DSIG_EXPORT XSECKeyInfoResolver {

public :

	/** @name Constructors and Destructors */
	//@{

	XSECKeyInfoResolver() {};
	virtual ~XSECKeyInfoResolver() {};

	//@}

	/** @name Mandatory Interface Functions */
	//@{

	/**
	 * \brief Provide a key to the library
	 *
	 * The library will pass the KeyInfoList to the resolver
	 * which then needs to provide a key back to the library.
	 * The key may have absolutely no relationship to the KeyInfoList
	 * (which is only supposed to provide a hint).
	 *
	 * If no KeyInfo elements were provided in the Signature,
	 * NULL will be passed in.
	 *
	 * @param lst The list of KeyInfo elements from the signature
	 * @returns Either the appropriate key or NULL if none can be found
	 */

	virtual XSECCryptoKey * resolveKey(DSIGKeyInfoList * lst) = 0;

	/**
	 * \brief Clone the resolver to be installed in a new object.
	 *
	 * When KeyInfoResolvers are passed into signatures and other
	 * objects, they are cloned and control of the original object
	 * is left with the caller.
	 *
	 */

	virtual XSECKeyInfoResolver * clone(void) const = 0;

	//@}

	/*\@}*/
};


#endif /* XSECKEYINFORESOLVER_INCLUDE */
