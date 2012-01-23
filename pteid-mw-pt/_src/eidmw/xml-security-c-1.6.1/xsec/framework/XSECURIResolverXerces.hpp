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
 * XSECURIResolverXerces := Virtual Interface class that takes a URI and
 *                    creates a binary input stream from it.
 *
 * $Id: XSECURIResolverXerces.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef XSECURIRESOLVERXERCES_INCLUDE
#define XSECURIRESOLVERXERCES_INCLUDE

#include <xsec/framework/XSECURIResolver.hpp>

/**
 * @ingroup pubsig
 */
/*\@{*/

/**
 * @brief URIResolver implementation class based on Xerces functions.
 *
 * This class provides the default implementation of the XSECURIResolver
 * class.  If no other resolver is specified, the XSECProvider will
 * install this into the Signatures that are created.
 *
 * The class is very simple - it calls on the Xerces functions
 * to provide primitive URI resolving support.
 *
 */

class DSIG_EXPORT XSECURIResolverXerces : public XSECURIResolver {

public:

	/** @name Constructors and Destructors */
	//@{

	/**
	 * \brief Constructor
	 *
	 * @param baseURI Provide a URI that relative URIs can be
	 * matched to.
	 */

	XSECURIResolverXerces(const XMLCh * baseURI = 0);
	virtual ~XSECURIResolverXerces();

	//@}

	/** @name Interface Methods */
	//@{

	/**
	 * \brief Create a BYTE_STREAM from a URI.
	 *
	 * The resolver is required to take the input URI and
	 * dereference it to an actual stream of octets.
	 *
	 * @param uri The string containing the URI to be de-referenced.
	 * @returns The octet stream corresponding to the URI.
	 */

	virtual XERCES_CPP_NAMESPACE_QUALIFIER BinInputStream * resolveURI(const XMLCh * uri);

	/**
	 * \brief Clone the resolver to be installed in a new object.
	 *
	 * When URIResolvers are passed into signatures and other
	 * objects, they are cloned and control of the original object
	 * is left with the caller.
	 *
	 */

	virtual XSECURIResolver * clone(void);

	//@}

	/** @name XSECURIResolverXerces Specific Methods */
	//@{

	/**
	 * \brief Set the base URI for relative URIs.
	 *
	 */

	void setBaseURI(const XMLCh * uri);

	//@}

private:

	XMLCh			* mp_baseURI;

};

#endif /* XSECURIRESOLVERXERCES_INCLUDE */
