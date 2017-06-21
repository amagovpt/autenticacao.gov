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
 * XSECURIResolverGenericUnix := A URI Resolver that will work "out of
 *                                the box" with UNIX.  Re-implements
 *								  much Xerces code, but allows us to
 *								  handle HTTP redirects as is required by
 *								  the DSIG Standard
 *
 * Author(s): Berin Lautenbach
 *
 * $Id: XSECURIResolverGenericUnix.cpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 * $Log$
 * Revision 1.11  2005/02/03 13:55:08  milan
 * Apache licence fix.
 *
 * Revision 1.10  2004/10/10 07:00:34  blautenb
 * Compile with Xerces 2.1
 *
 * Revision 1.9  2004/02/21 08:26:54  blautenb
 * Use XMLString::release rather than delete[] for all Xerces strings
 *
 * Revision 1.8  2004/02/08 10:50:22  blautenb
 * Update to Apache 2.0 license
 *
 * Revision 1.7  2004/02/03 11:00:03  blautenb
 * Add Space handling to UNIX URL handling
 *
 * Revision 1.6  2004/01/26 00:29:48  blautenb
 * Check for Xerces new way of handling NULL hostnames in URIs
 *
 * Revision 1.5  2003/09/11 11:29:12  blautenb
 * Fix Xerces namespace usage in *NIX build
 *
 * Revision 1.4  2003/07/05 10:30:38  blautenb
 * Copyright update
 *
 * Revision 1.3  2003/05/10 07:23:36  blautenb
 * Updates to support anonymous references
 *
 * Revision 1.2  2003/02/20 10:35:10  blautenb
 * Fix for broken Xerces XMLUri
 *
 * Revision 1.1  2003/02/12 11:21:03  blautenb
 * UNIX generic URI resolver
 *
 *
 */

#include <xsec/utils/unixutils/XSECURIResolverGenericUnix.hpp>

#include <xercesc/util/XMLUniDefs.hpp>
#include <xercesc/util/XMLUri.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <xercesc/util/Janitor.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/BinFileInputStream.hpp>

XERCES_CPP_NAMESPACE_USE

#include <xsec/framework/XSECError.hpp>
#include <xsec/utils/XSECDOMUtils.hpp>
#include <xsec/utils/unixutils/XSECBinHTTPURIInputStream.hpp>

#include "../../utils/XSECAutoPtr.hpp"

static const XMLCh gFileScheme[] = {

	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_f,
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i,
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_l,
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e,
	XERCES_CPP_NAMESPACE_QUALIFIER chNull

};

static const XMLCh gHttpScheme[] = {

	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_h,
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t,
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t,
	XERCES_CPP_NAMESPACE_QUALIFIER chLatin_p,
	XERCES_CPP_NAMESPACE_QUALIFIER chNull

};


#if XERCES_VERSION_MAJOR == 2 && XERCES_VERSION_MINOR < 3


static const XMLCh DOTDOT_SLASH[] = {

	XERCES_CPP_NAMESPACE_QUALIFIER chPeriod,
	XERCES_CPP_NAMESPACE_QUALIFIER chPeriod,
	XERCES_CPP_NAMESPACE_QUALIFIER chForwardSlash,
	XERCES_CPP_NAMESPACE_QUALIFIER chNull

};

#endif



XSECURIResolverGenericUnix::XSECURIResolverGenericUnix() :
mp_baseURI(NULL) {

};

XSECURIResolverGenericUnix::~XSECURIResolverGenericUnix() {

	if (mp_baseURI != NULL)
		XSEC_RELEASE_XMLCH(mp_baseURI);

}

// -----------------------------------------------------------------------
//  Resolve a URI that is passed in
// -----------------------------------------------------------------------

BinInputStream * XSECURIResolverGenericUnix::resolveURI(const XMLCh * uri) {

	XSEC_USING_XERCES(BinInputStream);
	XSEC_USING_XERCES(XMLUri);
	XSEC_USING_XERCES(XMLUni);
	XSEC_USING_XERCES(Janitor);
	XSEC_USING_XERCES(BinFileInputStream);

	XMLUri					* xmluri;

	if (uri == NULL) {
		throw XSECException(XSECException::ErrorOpeningURI,
			"XSECURIResolverGenericUnix - anonymous references not supported in default URI Resolvers");
	}

	// Create the appropriate XMLUri objects

	if (mp_baseURI != NULL) {
		XMLUri	* turi;

#if XERCES_VERSION_MAJOR == 2 && XERCES_VERSION_MINOR < 3

		// XMLUri relative paths are broken, so we need to strip out ".."
        XSECAutoPtrXMLCh b(mp_baseURI);
        XSECAutoPtrXMLCh r(uri);

		int index = 0;
		while (XMLString::startsWith(&(r.get()[index]), DOTDOT_SLASH)) {

			// Strip the last segment of the base

			int lastIndex = XMLString::lastIndexOf(b.get(), XERCES_CPP_NAMESPACE_QUALIFIER chForwardSlash);
			if (lastIndex > 0)
			    const_cast<XMLCh*>(b.get())[lastIndex] = 0;

			index += 3;

		}

		XSECnew(turi, XMLUri(b.get()));
		Janitor<XMLUri> j_turi(turi);
		XSECnew(xmluri, XMLUri(turi, &(r.get()[index])));

#else
		XSECnew(turi, XMLUri(mp_baseURI));
		Janitor<XMLUri> j_turi(turi);

		XSECnew(xmluri, XMLUri(turi, uri));
#endif

	}
	else {
		XSECnew(xmluri, XMLUri(uri));
	}
	
	Janitor<XMLUri> j_xmluri(xmluri);

	// Determine what kind of URI this is and how to handle it.
	
	if (!XMLString::compareIString(xmluri->getScheme(), gFileScheme)) {

		// This is a file.  We only really understand if this is localhost
		// XMLUri has already cleaned of escape characters (%xx)
        
		if (xmluri->getHost() == NULL || xmluri->getHost()[0] == chNull ||
			!XMLString::compareIString(xmluri->getHost(), XMLUni::fgLocalHostString)) {

			// Clean hex escapes
			XMLCh * realPath = cleanURIEscapes(xmluri->getPath());

			// Localhost

            BinFileInputStream* retStrm = new BinFileInputStream(realPath);
            XSEC_RELEASE_XMLCH(realPath);

            if (!retStrm->getIsOpen())
            {
                delete retStrm;
                return 0;
            }
            return retStrm;

		}

		else {

			throw XSECException(XSECException::ErrorOpeningURI,
				"XSECURIResolverGenericUnix - unable to open non-localhost file");
		
		}

	}

	// Is the scheme a HTTP?
	if (!XMLString::compareIString(xmluri->getScheme(), gHttpScheme)) {

		// Pass straight to our local XSECBinHTTPUriInputStream
		XSECBinHTTPURIInputStream *ret;

		XSECnew(ret, XSECBinHTTPURIInputStream(*xmluri));

		return ret;
		
	}

	throw XSECException(XSECException::ErrorOpeningURI,
		"XSECURIResolverGenericUnix - unknown URI scheme");
	
}

// -----------------------------------------------------------------------
//  Clone me
// -----------------------------------------------------------------------


XSECURIResolver * XSECURIResolverGenericUnix::clone(void) {

	XSECURIResolverGenericUnix * ret;

	ret = new XSECURIResolverGenericUnix();

	if (this->mp_baseURI != NULL)
		ret->mp_baseURI = XMLString::replicate(this->mp_baseURI);
	else
		ret->mp_baseURI = NULL;

	return ret;

}

// -----------------------------------------------------------------------
//  Set a base URI to map any incoming files against
// -----------------------------------------------------------------------

void XSECURIResolverGenericUnix::setBaseURI(const XMLCh * uri) {

	if (mp_baseURI != NULL)
		XSEC_RELEASE_XMLCH(mp_baseURI);

	mp_baseURI = XMLString::replicate(uri);

}
