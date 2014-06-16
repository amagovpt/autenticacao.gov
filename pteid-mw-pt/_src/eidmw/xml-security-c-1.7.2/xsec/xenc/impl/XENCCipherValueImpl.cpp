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
 * XENCCipherValueImpl := Implementation for CipherValue elements
 *
 * $Id: XENCCipherValueImpl.cpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

// XSEC Includes

#include <xsec/framework/XSECDefs.hpp>

#include "XENCCipherValueImpl.hpp"

#include <xsec/framework/XSECError.hpp>
#include <xsec/utils/XSECDOMUtils.hpp>
#include <xsec/framework/XSECEnv.hpp>

#include <xercesc/util/XMLUniDefs.hpp>

XERCES_CPP_NAMESPACE_USE

// --------------------------------------------------------------------------------
//			String Constants
// --------------------------------------------------------------------------------

static XMLCh s_CipherValue[] = {

	chLatin_C,
	chLatin_i,
	chLatin_p,
	chLatin_h,
	chLatin_e,
	chLatin_r,
	chLatin_V,
	chLatin_a,
	chLatin_l,
	chLatin_u,
	chLatin_e,
	chNull,
};

// --------------------------------------------------------------------------------
//			Constructors/Destructors
// --------------------------------------------------------------------------------

XENCCipherValueImpl::XENCCipherValueImpl(const XSECEnv * env) :
mp_env(env),
mp_cipherValueElement(NULL),
mp_cipherString(NULL) {

}

XENCCipherValueImpl::XENCCipherValueImpl(const XSECEnv * env, DOMElement * node) :
mp_env(env),
mp_cipherValueElement(node),
mp_cipherString(NULL) {

}


XENCCipherValueImpl::~XENCCipherValueImpl() {

	if (mp_cipherString != NULL)
		XSEC_RELEASE_XMLCH(mp_cipherString);

}

// --------------------------------------------------------------------------------
//			Load
// --------------------------------------------------------------------------------

void XENCCipherValueImpl::load(void) {

	if (mp_cipherValueElement == NULL) {

		// Attempt to load an empty encryptedType element
		throw XSECException(XSECException::CipherValueError,
			"XENCCipherData::load - called on empty DOM");

	}

	if (!strEquals(getXENCLocalName(mp_cipherValueElement), s_CipherValue)) {
	
		throw XSECException(XSECException::CipherValueError,
			"XENCCipherData::load - called incorrect node");
	
	}

	// Just gather the text children and continue
	safeBuffer txt;

	gatherChildrenText(mp_cipherValueElement, txt);

	// Get a copy
	mp_cipherString = XMLString::replicate(txt.rawXMLChBuffer());

}

// --------------------------------------------------------------------------------
//			Create a blank structure
// --------------------------------------------------------------------------------

DOMElement * XENCCipherValueImpl::createBlankCipherValue(
						const XMLCh * value) {

	// Rest
	if (mp_cipherString != NULL) {
		XSEC_RELEASE_XMLCH(mp_cipherString);
		mp_cipherString = NULL;
	}

	// Get some setup values
	safeBuffer str;
	DOMDocument *doc = mp_env->getParentDocument();
	const XMLCh * prefix = mp_env->getXENCNSPrefix();

	makeQName(str, prefix, s_CipherValue);

	DOMElement *ret = doc->createElementNS(DSIGConstants::s_unicodeStrURIXENC, str.rawXMLChBuffer());
	mp_cipherValueElement = ret;

	// Append the value
	ret->appendChild(doc->createTextNode(value));
	
	mp_cipherString = XMLString::replicate(value);;

	return ret;

}

// --------------------------------------------------------------------------------
//			Interface Methods
// --------------------------------------------------------------------------------

const XMLCh * XENCCipherValueImpl::getCipherString(void) const {

	return mp_cipherString;

}

void XENCCipherValueImpl::setCipherString(const XMLCh * value) {

	if (mp_cipherValueElement == NULL) {

		throw XSECException(XSECException::CipherValueError,
			"XENCCipherData::setCipherString - called on empty DOM");

	}

	// Find first text child
	DOMNode * txt = findFirstChildOfType(mp_cipherValueElement, DOMNode::TEXT_NODE);
	
	if (txt == NULL) {
		throw XSECException(XSECException::CipherValueError,
			"XENCCipherData::setCipherString - Error finding text node");
	}

	txt->setNodeValue(value);

	if (mp_cipherString != NULL)
		XSEC_RELEASE_XMLCH(mp_cipherString);

	mp_cipherString = XMLString::replicate(value);

}
