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
 * XKMSUseKeyWithImpl := Implementation of UseKeyWith Messages
 *
 * $Id: XKMSUseKeyWithImpl.cpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

// XSEC Includes

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/framework/XSECError.hpp>
#include <xsec/framework/XSECEnv.hpp>
#include <xsec/xkms/XKMSConstants.hpp>
#include <xsec/utils/XSECDOMUtils.hpp>

#include <xercesc/dom/DOM.hpp>

#include "XKMSUseKeyWithImpl.hpp"

XERCES_CPP_NAMESPACE_USE

// --------------------------------------------------------------------------------
//           Construct/Destruct
// --------------------------------------------------------------------------------

XKMSUseKeyWithImpl::XKMSUseKeyWithImpl(const XSECEnv * env) {

	mp_env = env;
	mp_useKeyWithElement = NULL;
	mp_identifierAttr = NULL;
	mp_applicationAttr = NULL;

}

XKMSUseKeyWithImpl::XKMSUseKeyWithImpl(const XSECEnv * env, DOMElement * node) {

	mp_env = env;
	mp_useKeyWithElement = node;
	mp_identifierAttr = NULL;
	mp_applicationAttr = NULL;

}

XKMSUseKeyWithImpl::~XKMSUseKeyWithImpl(){

}

// --------------------------------------------------------------------------------
//           Load
// --------------------------------------------------------------------------------


void XKMSUseKeyWithImpl::load(void) {

	if (mp_useKeyWithElement == NULL) {

		// Attempt to load an empty element
		throw XSECException(XSECException::XKMSError,
			"XKMSUseKeyWith::load - called on empty DOM");

	}

	if (!strEquals(getXKMSLocalName(mp_useKeyWithElement), 
		XKMSConstants::s_tagUseKeyWith)) {
	
		throw XSECException(XSECException::XKMSError,
			"XKMSUseKeyWith::load - called incorrect node");
	
	}

	// Identifier
	mp_identifierAttr = 
		mp_useKeyWithElement->getAttributeNodeNS(NULL, XKMSConstants::s_tagIdentifier);
	// Application
	mp_applicationAttr = 
		mp_useKeyWithElement->getAttributeNodeNS(NULL, XKMSConstants::s_tagApplication);

	// Identifier and Application MUST be set for a message to be OK

	if (mp_identifierAttr == NULL) {
		throw XSECException(XSECException::ExpectedXKMSChildNotFound,
			"XKMSUseKeyWith::load - Identifier attribute not found");
	}

	if (mp_applicationAttr == NULL) {
		throw XSECException(XSECException::ExpectedXKMSChildNotFound,
			"XKMSUseKeyWith::load - Application attribute not found");
	}

}

// --------------------------------------------------------------------------------
//           Create
// --------------------------------------------------------------------------------

DOMElement * XKMSUseKeyWithImpl::createBlankUseKeyWith(const XMLCh * application, 
													   const XMLCh * identifier) {

	safeBuffer str;
	DOMDocument *doc = mp_env->getParentDocument();
	const XMLCh * prefix = mp_env->getXKMSNSPrefix();

	makeQName(str, prefix, XKMSConstants::s_tagUseKeyWith);

	mp_useKeyWithElement = doc->createElementNS(XKMSConstants::s_unicodeStrURIXKMS, 
												str.rawXMLChBuffer());

	// Setup the Application URI
	mp_useKeyWithElement->setAttributeNS(NULL, 
		XKMSConstants::s_tagApplication,
							application);
	mp_applicationAttr = 
		mp_useKeyWithElement->getAttributeNodeNS(NULL, XKMSConstants::s_tagApplication);

	// Identifier
	mp_useKeyWithElement->setAttributeNS(NULL, 
		XKMSConstants::s_tagIdentifier,
							identifier);
	mp_identifierAttr = 
		mp_useKeyWithElement->getAttributeNodeNS(NULL, XKMSConstants::s_tagIdentifier);

	return mp_useKeyWithElement;

}

// --------------------------------------------------------------------------------
//           Get methods
// --------------------------------------------------------------------------------


DOMElement * XKMSUseKeyWithImpl::getElement(void) const {

	return mp_useKeyWithElement;

}

const XMLCh * XKMSUseKeyWithImpl::getApplication(void) const {

	if (mp_applicationAttr == NULL) {
		throw XSECException(XSECException::XKMSError,
			"XKMSUseKeyWith::getApplication - called on non-loaded construct");
	}

	return mp_applicationAttr->getNodeValue();

}

const XMLCh * XKMSUseKeyWithImpl::getIdentifier(void) const {

	if (mp_identifierAttr == NULL) {
		throw XSECException(XSECException::XKMSError,
			"XKMSUseKeyWith::getIdentifier - called on non-loaded construct");
	}

	return mp_identifierAttr->getNodeValue();

}

// --------------------------------------------------------------------------------
//           Set methods
// --------------------------------------------------------------------------------

void XKMSUseKeyWithImpl::setApplication(const XMLCh * uri) {
	if (mp_applicationAttr == NULL) {
		throw XSECException(XSECException::XKMSError,
			"XKMSUseKeyWith::setApplication - called on non-loaded construct");
	}

	mp_applicationAttr->setNodeValue(uri);

}
void XKMSUseKeyWithImpl::setIdentifier(const XMLCh * identifier) {

	if (mp_identifierAttr == NULL) {
		throw XSECException(XSECException::XKMSError,
			"XKMSUseKeyWith::setIdentifier - called on non-loaded construct");
	}

	mp_identifierAttr->setNodeValue(identifier);

}
