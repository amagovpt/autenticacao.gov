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
 * AnonymousResolver := Class to resolve the IAIK anonymous sample reference
 *
 * Author(s): Berin Lautenbach
 *
 * $Id: AnonymousResolver.cpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#include "AnonymousResolver.hpp"

// XSEC

#include <xsec/framework/XSECException.hpp>
#include <xsec/utils/XSECDOMUtils.hpp>

#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/Janitor.hpp>
#include <xercesc/framework/URLInputSource.hpp>

XERCES_CPP_NAMESPACE_USE


// ----------------------------------------------------------------------------
//           AnonymousResolver
// ----------------------------------------------------------------------------

AnonymousResolver::~AnonymousResolver() {

	if (mp_baseURI != NULL)
		XSEC_RELEASE_XMLCH(mp_baseURI);

}


XSECURIResolver * AnonymousResolver::clone(void) {

   	AnonymousResolver * ret;

	ret = new AnonymousResolver();

	if (this->mp_baseURI != 0)
		ret->mp_baseURI = XMLString::replicate(this->mp_baseURI);
	else
		ret->mp_baseURI = 0;

	return ret;

}

void AnonymousResolver::setBaseURI(const XMLCh * uri) {

	if (mp_baseURI != NULL)
		XSEC_RELEASE_XMLCH(mp_baseURI);

	mp_baseURI = XMLString::replicate(uri);

}

BinInputStream * AnonymousResolver::resolveURI(const XMLCh * uri) {

	XSEC_USING_XERCES(URLInputSource);
	XSEC_USING_XERCES(XMLURL);
	XSEC_USING_XERCES(BinInputStream);

	URLInputSource			* URLS;		// Use Xerces URL Input source
	BinInputStream			* is;		// To handle the actual input

	if (uri != NULL) {
		throw XSECException(XSECException::ErrorOpeningURI,
			"AnonymousResolver - only anonymous references supported");
	}

	if (mp_baseURI == 0) {
		URLS = new URLInputSource(XMLURL(MAKE_UNICODE_STRING(anonURI)));
	}
	else {
		URLS = new URLInputSource(XMLURL(XMLURL(mp_baseURI), MAKE_UNICODE_STRING(anonURI)));
	}

	// makeStream can (and is quite likely to) throw an exception
	Janitor<URLInputSource> j_URLS(URLS);

	is = URLS->makeStream();

	if (is == NULL) {

		throw XSECException(XSECException::ErrorOpeningURI,
			"An error occurred in AnonymousResolver when opening an URLInputStream");

	}

	return is;
}

