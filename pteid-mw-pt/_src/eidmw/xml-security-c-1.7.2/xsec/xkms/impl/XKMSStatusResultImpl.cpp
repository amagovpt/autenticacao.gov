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
 * XKMSStatusResultImpl := Implementation of StatusResult Messages
 *
 * $Id: XKMSStatusResultImpl.cpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/framework/XSECError.hpp>
#include <xsec/framework/XSECEnv.hpp>
#include <xsec/utils/XSECDOMUtils.hpp>
#include <xsec/xkms/XKMSConstants.hpp>

#include "XKMSStatusResultImpl.hpp"
#include "XKMSUnverifiedKeyBindingImpl.hpp"

#include <xercesc/dom/DOM.hpp>

XERCES_CPP_NAMESPACE_USE

// --------------------------------------------------------------------------------
//           Construct/Destruct
// --------------------------------------------------------------------------------

XKMSStatusResultImpl::XKMSStatusResultImpl(
		const XSECEnv * env) :
m_result(env),
m_msg(m_result.m_msg) {

	mp_successAttr = NULL;
	mp_failureAttr = NULL;
	mp_pendingAttr = NULL;

}

XKMSStatusResultImpl::XKMSStatusResultImpl(
		const XSECEnv * env, 
		XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * node) :
m_result(env, node),
m_msg(m_result.m_msg) {

	mp_successAttr = NULL;
	mp_failureAttr = NULL;
	mp_pendingAttr = NULL;

}

XKMSStatusResultImpl::~XKMSStatusResultImpl() {

}


// --------------------------------------------------------------------------------
//           Load from DOM
// --------------------------------------------------------------------------------

// Load elements
void XKMSStatusResultImpl::load() {

	if (m_msg.mp_messageAbstractTypeElement == NULL) {

		// Attempt to load an empty element
		throw XSECException(XSECException::XKMSError,
			"XKMSStatusResult::load - called on empty DOM");

	}

	if (!strEquals(getXKMSLocalName(m_msg.mp_messageAbstractTypeElement), 
									XKMSConstants::s_tagStatusResult)) {
	
		throw XSECException(XSECException::XKMSError,
			"XKMSStatusResult::load - called incorrect node");
	
	}

	// Load the base message
	m_result.load();

}

// --------------------------------------------------------------------------------
//           Create a blank one
// --------------------------------------------------------------------------------
DOMElement * XKMSStatusResultImpl::createBlankStatusResult(
		const XMLCh * service,
		const XMLCh * id,
		ResultMajor rmaj,
		ResultMinor rmin) {

	return m_result.createBlankResultType(
		XKMSConstants::s_tagStatusResult, service, id, rmaj, rmin);

}

// --------------------------------------------------------------------------------
//           Get interface methods
// --------------------------------------------------------------------------------

XKMSMessageAbstractType::messageType XKMSStatusResultImpl::getMessageType(void) {

	return XKMSMessageAbstractTypeImpl::StatusResult;

}

// --------------------------------------------------------------------------------
//           Result count handling
// --------------------------------------------------------------------------------

int XKMSStatusResultImpl::getSuccessCount(void) const {

	if (mp_successAttr == NULL)
		return 0;

	unsigned int val;
	XMLString::textToBin(mp_successAttr->getNodeValue(), val);

	return (int) val;

}

int XKMSStatusResultImpl::getFailureCount(void) const {

	if (mp_failureAttr == NULL)
		return 0;

	unsigned int val;
	XMLString::textToBin(mp_failureAttr->getNodeValue(), val);

	return (int) val;

}

int XKMSStatusResultImpl::getPendingCount(void) const {

	if (mp_pendingAttr == NULL)
		return 0;

	unsigned int val;
	XMLString::textToBin(mp_pendingAttr->getNodeValue(), val);

	return (int) val;

}

void XKMSStatusResultImpl::setSuccessCount(int count) {

	if (m_msg.mp_messageAbstractTypeElement == NULL) {

		// Attempt update when not initialised
		throw XSECException(XSECException::MessageAbstractTypeError,
			"XKMSStatusResult::setSuccessCount - called on non-initialised structure");

	}

	XMLCh val[16];
	XMLString::binToText(count, val, 16, 10);

	m_msg.mp_messageAbstractTypeElement->setAttributeNS(NULL, XKMSConstants::s_tagSuccess, val);
	mp_successAttr = 
		m_msg.mp_messageAbstractTypeElement->getAttributeNodeNS(NULL, XKMSConstants::s_tagSuccess);

}

void XKMSStatusResultImpl::setFailureCount(int count) {

	if (m_msg.mp_messageAbstractTypeElement == NULL) {

		// Attempt update when not initialised
		throw XSECException(XSECException::MessageAbstractTypeError,
			"XKMSStatusResult::setFailureCount - called on non-initialised structure");

	}

	XMLCh val[16];
	XMLString::binToText(count, val, 16, 10);

	m_msg.mp_messageAbstractTypeElement->setAttributeNS(NULL, XKMSConstants::s_tagFailure, val);
	mp_failureAttr = 
		m_msg.mp_messageAbstractTypeElement->getAttributeNodeNS(NULL, XKMSConstants::s_tagFailure);

}

void XKMSStatusResultImpl::setPendingCount(int count) {

	if (m_msg.mp_messageAbstractTypeElement == NULL) {

		// Attempt update when not initialised
		throw XSECException(XSECException::MessageAbstractTypeError,
			"XKMSStatusResult::setPendingCount - called on non-initialised structure");

	}

	XMLCh val[16];
	XMLString::binToText(count, val, 16, 10);

	m_msg.mp_messageAbstractTypeElement->setAttributeNS(NULL, XKMSConstants::s_tagPending, val);
	mp_pendingAttr = 
		m_msg.mp_messageAbstractTypeElement->getAttributeNodeNS(NULL, XKMSConstants::s_tagPending);

}


