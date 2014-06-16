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
 * XSECURIResolver := Virtual Interface class that takes a URI and
 *                    creates a binary input stream from it.
 *
 * $Id: XSECURIResolver.hpp 1125514 2011-05-20 19:08:33Z scantor $
 */

#ifndef XSECURIRESOLVER_INCLUDE
#define XSECURIRESOLVER_INCLUDE

#include <xsec/framework/XSECDefs.hpp>

XSEC_DECLARE_XERCES_CLASS(BinInputStream);

/**
 * @ingroup pubsig
 */
/*\@{*/

/**
 * @brief Interface class for resolving URIs.
 *
 * The XML Digital Signature standard makes heavy use of URIs to
 * identify information to be referenced and signed.
 *
 * The library internally handles reference URIs within the
 * document that contains the signature, but uses URIResolver
 * classes to dereference a URI into a byte stream that can
 * then be processed.
 *
 * The interface class allows others to re-implement and install
 * their own resolves according to their needs.  The basic
 * implementations found in the library are just that - very basic.
 * Enough to do the job for interoperability testing, but not
 * enough to provide robustness in a major application.
 *
 */

class DSIG_EXPORT XSECURIResolver {

public:

	/** @name Constructors and Destructors */
	//@{

	XSECURIResolver() {};
	virtual ~XSECURIResolver() {};

	//@}

	/** @name Interface Methods */
	//@{

	/**
	 * \brief Create a BYTE_STREAM from a URI.
	 *
	 * The resolver is required to take the input URI and
	 * dereference it to an actual stream of octets.
	 *
	 * The octets are provided back to the library using
	 * the Xerces BinInputStream class.
	 *
	 * @note The returned stream is "owned" by the caller, which
	 * will delete it when processing is complete.
	 * @param uri The string containing the URI to be de-referenced.  NULL
	 * if this is an anonymous reference.
	 * @returns The octet stream corresponding to the URI.
	 */

	virtual XERCES_CPP_NAMESPACE_QUALIFIER BinInputStream * 
		resolveURI(const XMLCh * uri) = 0;

	/**
	 * \brief Clone the resolver to be installed in a new object.
	 *
	 * When URIResolvers are passed into signatures and other
	 * objects, they are cloned and control of the original object
	 * is left with the caller.
	 *
	 */

	virtual XSECURIResolver * clone(void) = 0;

	//@}

};


#endif /* XSECURIRESOLVER_INCLUDE */
