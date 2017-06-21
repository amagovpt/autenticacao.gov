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
 *                                the box" with Windows.  Re-implements
 *								  much Xerces code, but allows us to
 *								  handle HTTP redirects as is required by
 *								  the DSIG Standard
 *
 * Author(s): Berin Lautenbach
 *
 * $Id: XSECURIResolverGenericWin32.cpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 * $Log$
 * Revision 1.12  2005/05/29 00:06:55  blautenb
 * Update localhost check to include an empty hostname - reported by Vincent Finn <vincent.finn@automsoft.com> on security-dev@xml 27/5/2005
 *
 * Revision 1.11  2005/02/03 13:56:22  milan
 * Apache licence fix.
 *
 * Revision 1.10  2004/02/21 08:26:54  blautenb
 * Use XMLString::release rather than delete[] for all Xerces strings
 *
 * Revision 1.9  2004/02/08 10:25:40  blautenb
 * Convert to Apache 2.0 license
 *
 * Revision 1.8  2004/01/26 00:16:04  blautenb
 * Remove escapes from URI before retrieving a path on the file system
 *
 * Revision 1.7  2003/09/11 11:11:05  blautenb
 * Cleaned up usage of Xerces namespace - no longer inject into global namespace in headers
 *
 * Revision 1.6  2003/07/28 12:52:46  blautenb
 * Fixed a bug with DEBUG_NEW when compiling with Xalan 1.6
 *
 * Revision 1.5  2003/07/05 10:30:38  blautenb
 * Copyright update
 *
 * Revision 1.4  2003/05/22 11:42:06  blautenb
 * Updates so Windows version will compile with Xerces 2.3
 *
 * Revision 1.3  2003/05/10 07:23:36  blautenb
 * Updates to support anonymous references
 *
 * Revision 1.2  2003/02/17 11:21:45  blautenb
 * Work around for Xerces XMLUri bug
 *
 * Revision 1.1  2003/02/12 09:45:29  blautenb
 * Win32 Re-implementation of Xerces URIResolver to support re-directs
 *
 *
 */

#include <xsec/utils/winutils/XSECURIResolverGenericWin32.hpp>

#include <xercesc/util/XMLUniDefs.hpp>
#include <xercesc/util/XMLUri.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <xercesc/util/Janitor.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/BinFileInputStream.hpp>

XERCES_CPP_NAMESPACE_USE

#include <xsec/framework/XSECError.hpp>
#include <xsec/utils/winutils/XSECBinHTTPURIInputStream.hpp>
#include <xsec/utils/XSECDOMUtils.hpp>

#include "../../utils/XSECAutoPtr.hpp"

static const XMLCh gFileScheme[] = {

	chLatin_f,
	chLatin_i,
	chLatin_l,
	chLatin_e,
	chNull

};

static const XMLCh gHttpScheme[] = {

	chLatin_h,
	chLatin_t,
	chLatin_t,
	chLatin_p,
	chNull

};

#if XERCES_VERSION_MAJOR == 2 && XERCES_VERSION_MINOR < 3


static const XMLCh DOTDOT_SLASH[] = {

	chPeriod,
	chPeriod,
	chForwardSlash,
	chNull

};

#endif

XSECURIResolverGenericWin32::XSECURIResolverGenericWin32() :
mp_baseURI(NULL) {

};


XSECURIResolverGenericWin32::~XSECURIResolverGenericWin32() {

	if (mp_baseURI != NULL)
	    XSEC_RELEASE_XMLCH(mp_baseURI);

}

// -----------------------------------------------------------------------
//  Resolve a URI that is passed in
// -----------------------------------------------------------------------

BinInputStream * XSECURIResolverGenericWin32::resolveURI(const XMLCh * uri) {

	XSEC_USING_XERCES(BinInputStream);
	XSEC_USING_XERCES(XMLUri);
	XSEC_USING_XERCES(XMLUni);
	XSEC_USING_XERCES(Janitor);
	XSEC_USING_XERCES(BinFileInputStream);

	XMLUri					* xmluri;

	if (uri == NULL) {
		throw XSECException(XSECException::ErrorOpeningURI,
			"XSECURIResolverGenericWin32 - anonymous references not supported in default URI Resolvers");
	}

	// Create the appropriate XMLUri objects
	if (mp_baseURI != NULL) {
		XMLUri	* turi;

#if defined(XSEC_XERCES_BROKEN_XMLURI)

		// XMLUri relative paths are broken, so we need to strip out ".."
		// Doesn't fix the whole problem, but gets us somewhere

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
		turi = new XMLUri(mp_baseURI);
		Janitor<XMLUri> j_turi(turi);

		xmluri = new XMLUri(turi, uri);
#endif

	}
	else {
		xmluri = new XMLUri(uri);
	}
	
	Janitor<XMLUri> j_xmluri(xmluri);

	// Determine what kind of URI this is and how to handle it.
	
	if (!XMLString::compareIString(xmluri->getScheme(), gFileScheme)) {

		// This is a file.  We only really understand if this is localhost
        
		if (xmluri->getHost() == NULL || xmluri->getHost()[0] == chNull ||
			!XMLString::compareIString(xmluri->getHost(), XMLUni::fgLocalHostString) ||
			!XMLString::compareIString(xmluri->getHost(), XMLUni::fgEmptyString)) {

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
				"XSECURIResolverGenericWin32 - unable to open non-localhost file");
		
		}

	}

	// Is the scheme a HTTP?
	if (!XMLString::compareIString(xmluri->getScheme(), gHttpScheme)) {

		// Pass straight to our local XSECBinHTTPUriInputStream
		XSECBinHTTPURIInputStream *ret;

		ret = new XSECBinHTTPURIInputStream(*xmluri);

		return ret;
		
	}

	throw XSECException(XSECException::ErrorOpeningURI,
		"XSECURIResolverGenericWin32 - unknown URI scheme");
	
}

// -----------------------------------------------------------------------
//  Clone me
// -----------------------------------------------------------------------


XSECURIResolver * XSECURIResolverGenericWin32::clone(void) {

	XSECURIResolverGenericWin32 * ret;

	ret = new XSECURIResolverGenericWin32();

	if (this->mp_baseURI != NULL)
		ret->mp_baseURI = XMLString::replicate(this->mp_baseURI);
	else
		ret->mp_baseURI = NULL;

	return ret;

}

// -----------------------------------------------------------------------
//  Set a base URI to map any incoming files against
// -----------------------------------------------------------------------

void XSECURIResolverGenericWin32::setBaseURI(const XMLCh * uri) {

	if (mp_baseURI != NULL)
	    XSEC_RELEASE_XMLCH(mp_baseURI);

	mp_baseURI = XMLString::replicate(uri);

}
