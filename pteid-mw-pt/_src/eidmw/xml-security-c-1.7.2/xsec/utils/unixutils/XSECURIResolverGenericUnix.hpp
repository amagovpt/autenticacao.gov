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
 * XSECURIResolverGenericWin32 := A URI Resolver that will work "out of
 *                                the box" with UNIX.  Re-implements
 *								  much Xerces code, but allows us to
 *								  handle HTTP redirects as is required by
 *								  the DSIG Standard
 *
 * Author(s): Berin Lautenbach
 *
 * $Id: XSECURIResolverGenericUnix.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 * $Log$
 * Revision 1.5  2005/02/03 13:55:08  milan
 * Apache licence fix.
 *
 * Revision 1.4  2004/02/08 10:50:22  blautenb
 * Update to Apache 2.0 license
 *
 * Revision 1.3  2003/09/11 11:29:12  blautenb
 * Fix Xerces namespace usage in *NIX build
 *
 * Revision 1.2  2003/07/05 10:30:38  blautenb
 * Copyright update
 *
 * Revision 1.1  2003/02/12 11:21:04  blautenb
 * UNIX generic URI resolver
 *
 *
 */

#ifndef XSECURIRESOLVERGENERICUNIX_INCLUDE
#define XSECURIRESOLVERGENERICUNIX_INCLUDE

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/framework/XSECURIResolver.hpp>

#include <xercesc/util/XMLString.hpp>

/**
 * @ingroup pubsig
 */
/*\@{*/

/**
 * @brief Generic UNIX URI Resolver.
 *
 * The XML Digital Signature standard makes heavy use of URIs to
 * identify information to be referenced and signed.
 *
 * This class implements the XSECURIResolver for UNIX, re-using
 * much of the Xerces code.
 *
 * @todo Implement a "pluggable" URI resolver that allows developers
 * to plug in different classes for different schemes
 */

class DSIG_EXPORT XSECURIResolverGenericUnix : public XSECURIResolver {

public:

	/** @name Constructors and Destructors */
	//@{

	XSECURIResolverGenericUnix();
	virtual ~XSECURIResolverGenericUnix();

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
	 * @param uri The string containing the URI to be de-referenced.
	 * @returns The octet stream corresponding to the URI.
	 */

	virtual XERCES_CPP_NAMESPACE_QUALIFIER BinInputStream * 
	    resolveURI(const XMLCh * uri);

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

	/** @name Class specific functions */
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


#endif /* XSECURIRESOLVERGENERICUNIX_INCLUDE */
