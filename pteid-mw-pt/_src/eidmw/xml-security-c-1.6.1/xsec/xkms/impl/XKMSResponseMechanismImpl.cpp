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
 * XKMSResponseMechanismImpl := Implementation of XKMSResponseMechanism
 *
 * $Id: XKMSResponseMechanismImpl.cpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

// XSEC Includes

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/framework/XSECEnv.hpp>
#include <xsec/framework/XSECError.hpp>
#include <xsec/utils/XSECDOMUtils.hpp>
#include <xsec/xkms/XKMSConstants.hpp>

#include <xercesc/util/XMLUniDefs.hpp>

#include "XKMSResponseMechanismImpl.hpp"

#include <xercesc/dom/DOM.hpp>

XERCES_CPP_NAMESPACE_USE


// --------------------------------------------------------------------------------
//           Construct/Destruct
// --------------------------------------------------------------------------------


XKMSResponseMechanismImpl::XKMSResponseMechanismImpl(const XSECEnv * env) :
mp_env(env),
mp_responseMechanismTextNode(NULL)
{}

XKMSResponseMechanismImpl::XKMSResponseMechanismImpl(		
		const XSECEnv * env, 
		XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * node) :
mp_env(env),
mp_responseMechanismElement(node),
mp_responseMechanismTextNode(NULL) {

}

XKMSResponseMechanismImpl::~XKMSResponseMechanismImpl() {}

// --------------------------------------------------------------------------------
//           Load
// --------------------------------------------------------------------------------

void XKMSResponseMechanismImpl::load(void) {

	if (mp_responseMechanismElement == NULL) {

		// Attempt to load an empty element
		throw XSECException(XSECException::XKMSError,
			"XKMSResponseMechanism::load - called on empty DOM");

	}

	mp_responseMechanismTextNode = findFirstChildOfType(mp_responseMechanismElement, DOMNode::TEXT_NODE);

	if (mp_responseMechanismTextNode == NULL) {

		throw XSECException(XSECException::ExpectedXKMSChildNotFound,
			"XKMSResponseMechanism::load - Expected TEXT node beneath <ResponseMechanism> element");

	}

}
// --------------------------------------------------------------------------------
//           Create
// --------------------------------------------------------------------------------

DOMElement * XKMSResponseMechanismImpl::createBlankResponseMechanism(const XMLCh * item) {

	// Get some setup values
	safeBuffer str;
	DOMDocument *doc = mp_env->getParentDocument();
	const XMLCh * prefix = mp_env->getXKMSNSPrefix();

	makeQName(str, prefix, XKMSConstants::s_tagResponseMechanism);

	mp_responseMechanismElement = doc->createElementNS(XKMSConstants::s_unicodeStrURIXKMS, 
												str.rawXMLChBuffer());

	// Create the ResponseMechanism item
	str.sbXMLChIn(XKMSConstants::s_unicodeStrURIXKMS);
	str.sbXMLChCat(item);
	mp_responseMechanismTextNode = doc->createTextNode(str.rawXMLChBuffer());
	mp_responseMechanismElement->appendChild(mp_responseMechanismTextNode);

	return mp_responseMechanismElement;
}

// --------------------------------------------------------------------------------
//           Get interface
// --------------------------------------------------------------------------------

const XMLCh * XKMSResponseMechanismImpl::getResponseMechanismString(void) const {

	if (mp_responseMechanismTextNode == NULL) {

		throw XSECException(XSECException::XKMSError,
			"XKMSResponseMechanism::getResponseMechanismString - Attempt to get prior to initialisation");
	}
	const XMLCh * r = mp_responseMechanismTextNode->getNodeValue();

	int index = XMLString::indexOf(r, chPound);
	if (index == -1 || XMLString::compareNString(r, XKMSConstants::s_unicodeStrURIXKMS, index)) {
			throw XSECException(XSECException::XKMSError,
				"XKMSResponseMechanism::getResponseMechanismString - Item not in XKMS Name Space");
	}

	return &r[index+1];

}

// --------------------------------------------------------------------------------
//           Set interface
// --------------------------------------------------------------------------------

void XKMSResponseMechanismImpl::setResponseMechanismString(const XMLCh * str) {

	if (mp_responseMechanismTextNode == NULL) {

		throw XSECException(XSECException::XKMSError,
			"XKMSResponseMechanism::setResponseMechanismString - Attempt to set prior to initialisation");
	}

	safeBuffer sb;
	sb.sbXMLChIn(XKMSConstants::s_unicodeStrURIXKMS);
	sb.sbXMLChCat(str);

	mp_responseMechanismTextNode->setNodeValue(sb.rawXMLChBuffer());

}

