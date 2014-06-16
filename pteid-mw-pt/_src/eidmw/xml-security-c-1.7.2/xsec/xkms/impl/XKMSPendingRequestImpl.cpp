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
 * XKMSPendingRequestImpl := Implementation of PendingRequest Messages
 *
 * $Id: XKMSPendingRequestImpl.cpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/framework/XSECError.hpp>
#include <xsec/framework/XSECEnv.hpp>
#include <xsec/utils/XSECDOMUtils.hpp>
#include <xsec/xkms/XKMSConstants.hpp>

#include "XKMSPendingRequestImpl.hpp"

#include <xercesc/dom/DOM.hpp>

XERCES_CPP_NAMESPACE_USE

// --------------------------------------------------------------------------------
//           Construct/Destruct
// --------------------------------------------------------------------------------

XKMSPendingRequestImpl::XKMSPendingRequestImpl(
		const XSECEnv * env) :
m_request(env),
m_msg(m_request.m_msg),
mp_responseIdAttr(NULL) {
}

XKMSPendingRequestImpl::XKMSPendingRequestImpl(
		const XSECEnv * env, 
		XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * node) :
m_request(env, node),
m_msg(m_request.m_msg),
mp_responseIdAttr(NULL) {

}

XKMSPendingRequestImpl::~XKMSPendingRequestImpl() {

}


// --------------------------------------------------------------------------------
//           Load from DOM
// --------------------------------------------------------------------------------

// Load elements
void XKMSPendingRequestImpl::load() {

	if (m_msg.mp_messageAbstractTypeElement == NULL) {

		// Attempt to load an empty element
		throw XSECException(XSECException::XKMSError,
			"XKMSPendingRequest::load - called on empty DOM");

	}

	if (!strEquals(getXKMSLocalName(m_msg.mp_messageAbstractTypeElement), 
									XKMSConstants::s_tagPendingRequest)) {
	
		throw XSECException(XSECException::XKMSError,
			"XKMSPendingRequest::load - called incorrect node");
	
	}

	// Load the base message
	m_request.load();

	// Now check for ResponseId attribute
	mp_responseIdAttr = 
		m_msg.mp_messageAbstractTypeElement->getAttributeNodeNS(NULL, XKMSConstants::s_tagResponseId);

	if (mp_responseIdAttr == NULL) {
		// Attempt to load an empty element
		throw XSECException(XSECException::ExpectedXKMSChildNotFound,
			"XKMSPendingRequest::load - No ResponseId attribute found");
	}
}

// --------------------------------------------------------------------------------
//           Create a blank one
// --------------------------------------------------------------------------------
DOMElement * XKMSPendingRequestImpl::createBlankPendingRequest(
		const XMLCh * service,
		const XMLCh * id) {

	return m_request.createBlankRequestAbstractType(
		XKMSConstants::s_tagPendingRequest, service, id);
//	return XKMSRequestAbstractTypeImpl::createBlankMessageAbstractType(
//		MAKE_UNICODE_STRING("ValidateRequest"), service, id);

}

// --------------------------------------------------------------------------------
//           Get interface methods
// --------------------------------------------------------------------------------

XKMSMessageAbstractType::messageType XKMSPendingRequestImpl::getMessageType(void) {

	return XKMSMessageAbstractTypeImpl::PendingRequest;

}


const XMLCh * XKMSPendingRequestImpl::getResponseId(void) const {

	if (mp_responseIdAttr == NULL) {

		// Attempt read when not initialised
		throw XSECException(XSECException::MessageAbstractTypeError,
			"XKMSPendingRequest::getResponseId - called on non-initialised structure");

	}

	return 	mp_responseIdAttr->getNodeValue();
}

// --------------------------------------------------------------------------------
//           Setter methods
// --------------------------------------------------------------------------------

void XKMSPendingRequestImpl::setResponseId(const XMLCh * responseId) {

	if (m_msg.mp_messageAbstractTypeElement == NULL) {

		// Attempt update when not initialised
		throw XSECException(XSECException::MessageAbstractTypeError,
			"XKMSPendingRequest::setResponseId - called on non-initialised structure");

	}

	m_msg.mp_messageAbstractTypeElement->setAttributeNS(NULL, XKMSConstants::s_tagResponseId, responseId);
	mp_responseIdAttr = 
		m_msg.mp_messageAbstractTypeElement->getAttributeNodeNS(NULL, XKMSConstants::s_tagResponseId);

}


