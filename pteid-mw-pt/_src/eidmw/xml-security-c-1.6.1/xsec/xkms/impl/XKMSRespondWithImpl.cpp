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
 * XKMSRespondWithImpl := Implementation of XKMSRespondWith
 *
 * $Id: XKMSRespondWithImpl.cpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

// XSEC Includes

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/framework/XSECEnv.hpp>
#include <xsec/framework/XSECError.hpp>
#include <xsec/utils/XSECDOMUtils.hpp>
#include <xsec/xkms/XKMSConstants.hpp>

#include <xercesc/util/XMLUniDefs.hpp>

#include "XKMSRespondWithImpl.hpp"

#include <xercesc/dom/DOM.hpp>

XERCES_CPP_NAMESPACE_USE


// --------------------------------------------------------------------------------
//           Construct/Destruct
// --------------------------------------------------------------------------------


XKMSRespondWithImpl::XKMSRespondWithImpl(const XSECEnv * env) :
mp_env(env),
mp_respondWithTextNode(NULL)
{}

XKMSRespondWithImpl::XKMSRespondWithImpl(		
		const XSECEnv * env, 
		XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * node) :
mp_env(env),
mp_respondWithElement(node),
mp_respondWithTextNode(NULL) {

}

XKMSRespondWithImpl::~XKMSRespondWithImpl() {}

// --------------------------------------------------------------------------------
//           Load
// --------------------------------------------------------------------------------

void XKMSRespondWithImpl::load(void) {

	if (mp_respondWithElement == NULL) {

		// Attempt to load an empty element
		throw XSECException(XSECException::XKMSError,
			"XKMSRespondWith::load - called on empty DOM");

	}

	mp_respondWithTextNode = findFirstChildOfType(mp_respondWithElement, DOMNode::TEXT_NODE);

	if (mp_respondWithTextNode == NULL) {

		throw XSECException(XSECException::ExpectedXKMSChildNotFound,
			"XKMSRespondWith::load - Expected TEXT node beneath <RespondWith> element");

	}

}
// --------------------------------------------------------------------------------
//           Create
// --------------------------------------------------------------------------------

DOMElement * XKMSRespondWithImpl::createBlankRespondWith(const XMLCh * item) {

	// Get some setup values
	safeBuffer str;
	DOMDocument *doc = mp_env->getParentDocument();
	const XMLCh * prefix = mp_env->getXKMSNSPrefix();

	makeQName(str, prefix, XKMSConstants::s_tagRespondWith);

	mp_respondWithElement = doc->createElementNS(XKMSConstants::s_unicodeStrURIXKMS, 
												str.rawXMLChBuffer());

	// Create the RespondWith item
	str.sbXMLChIn(XKMSConstants::s_unicodeStrURIXKMS);
	str.sbXMLChCat(item);

	mp_respondWithTextNode = doc->createTextNode(str.rawXMLChBuffer());
	mp_respondWithElement->appendChild(mp_respondWithTextNode);

	return mp_respondWithElement;
}

// --------------------------------------------------------------------------------
//           Get interface
// --------------------------------------------------------------------------------

const XMLCh * XKMSRespondWithImpl::getRespondWithString(void) const {

	if (mp_respondWithTextNode == NULL) {

		throw XSECException(XSECException::XKMSError,
			"XKMSRespondWith::getRespondWithString - Attempt to get prior to initialisation");
	}
	const XMLCh * r = mp_respondWithTextNode->getNodeValue();

	int index = XMLString::indexOf(r, chPound);

	if (index == -1 || XMLString::compareNString(r, XKMSConstants::s_unicodeStrURIXKMS, index)) {
			throw XSECException(XSECException::XKMSError,
				"XKMSRespondWith::getRespondWithString - Item not in XKMS Name Space");
	}

	return &r[index+1];

}

// --------------------------------------------------------------------------------
//           Set interface
// --------------------------------------------------------------------------------

void XKMSRespondWithImpl::setRespondWithString(const XMLCh * str) {

	if (mp_respondWithTextNode == NULL) {

		throw XSECException(XSECException::XKMSError,
			"XKMSRespondWith::setRespondWithString - Attempt to set prior to initialisation");
	}

	safeBuffer sb;
	sb.sbXMLChIn(XKMSConstants::s_unicodeStrURIXKMS);
	sb.sbXMLChCat(str);

	mp_respondWithTextNode->setNodeValue(sb.rawXMLChBuffer());

}

