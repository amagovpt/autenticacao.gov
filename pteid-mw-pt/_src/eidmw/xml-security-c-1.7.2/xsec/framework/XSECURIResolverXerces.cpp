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
 * $Id: XSECURIResolverXerces.cpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */


#include <xsec/framework/XSECDefs.hpp>
#include <xsec/framework/XSECURIResolverXerces.hpp>
#include <xsec/framework/XSECError.hpp>

#include <xercesc/framework/URLInputSource.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/Janitor.hpp>

XERCES_CPP_NAMESPACE_USE

#include <string.h>

// --------------------------------------------------------------------------------
//           Constructors and Destructors
// --------------------------------------------------------------------------------

XSECURIResolverXerces::XSECURIResolverXerces(const XMLCh * baseURI) {

	if (baseURI != 0) {

		mp_baseURI = XMLString::replicate(baseURI);

	}
	else
		mp_baseURI = NULL;

};

XSECURIResolverXerces::~XSECURIResolverXerces() {

	if (mp_baseURI != NULL)
		XSEC_RELEASE_XMLCH(mp_baseURI);
}

// --------------------------------------------------------------------------------
//           Interface Methods
// --------------------------------------------------------------------------------

XERCES_CPP_NAMESPACE_QUALIFIER BinInputStream * XSECURIResolverXerces::resolveURI(const XMLCh * uri) {

	XSEC_USING_XERCES(URLInputSource);
	XSEC_USING_XERCES(XMLURL);
	XSEC_USING_XERCES(BinInputStream);

	
	URLInputSource			* URLS;		// Use Xerces URL Input source
	BinInputStream			* is;		// To handle the actual input

	if (uri == NULL) {
		throw XSECException(XSECException::ErrorOpeningURI,
			"XSECURIResolverXerces - anonymous references not supported in default URI Resolvers");
	}

	if (mp_baseURI == 0) {
		URLS = new URLInputSource(XMLURL(uri));
	}
	else {
		URLS = new URLInputSource(XMLURL(XMLURL(mp_baseURI), uri));
	}

	// makeStream can (and is quite likely to) throw an exception
	Janitor<URLInputSource> j_URLS(URLS);

	is = URLS->makeStream();

	if (is == NULL) {

		throw XSECException(XSECException::ErrorOpeningURI,
			"An error occurred in XSECURIREsolverXerces when opening an URLInputStream");

	}

	return is;

}


XSECURIResolver * XSECURIResolverXerces::clone(void) {

	XSECURIResolverXerces * ret;

	ret = new XSECURIResolverXerces();

	if (this->mp_baseURI != 0)
		ret->mp_baseURI = XMLString::replicate(this->mp_baseURI);
	else
		ret->mp_baseURI = 0;

	return ret;

}

// --------------------------------------------------------------------------------
//           Specific Methods
// --------------------------------------------------------------------------------


void XSECURIResolverXerces::setBaseURI(const XMLCh * uri) {

	if (mp_baseURI != 0)
		XSEC_RELEASE_XMLCH(mp_baseURI);

	mp_baseURI = XMLString::replicate(uri);

};
