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
 * XKMSValidateRequestImpl := Implementation of ValidateRequest Messages
 *
 * $Id: XKMSValidateRequestImpl.cpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/framework/XSECError.hpp>
#include <xsec/framework/XSECEnv.hpp>
#include <xsec/utils/XSECDOMUtils.hpp>
#include <xsec/xkms/XKMSConstants.hpp>

#include "XKMSValidateRequestImpl.hpp"

#include <xercesc/dom/DOM.hpp>

XERCES_CPP_NAMESPACE_USE

// --------------------------------------------------------------------------------
//           Construct/Destruct
// --------------------------------------------------------------------------------

XKMSValidateRequestImpl::XKMSValidateRequestImpl(
		const XSECEnv * env) :
m_request(env),
m_msg(m_request.m_msg),
mp_queryKeyBindingElement(NULL),
mp_queryKeyBinding(NULL) {

}

XKMSValidateRequestImpl::XKMSValidateRequestImpl(
		const XSECEnv * env, 
		XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * node) :
m_request(env, node),
m_msg(m_request.m_msg),
mp_queryKeyBinding(NULL) {

}

XKMSValidateRequestImpl::~XKMSValidateRequestImpl() {

	if (mp_queryKeyBinding != NULL)
		delete mp_queryKeyBinding;

}


// --------------------------------------------------------------------------------
//           Load from DOM
// --------------------------------------------------------------------------------

// Load elements
void XKMSValidateRequestImpl::load() {

	if (m_msg.mp_messageAbstractTypeElement == NULL) {

		// Attempt to load an empty element
		throw XSECException(XSECException::XKMSError,
			"XKMSValidateRequest::load - called on empty DOM");

	}

	if (!strEquals(getXKMSLocalName(m_msg.mp_messageAbstractTypeElement), 
									XKMSConstants::s_tagValidateRequest)) {
	
		throw XSECException(XSECException::XKMSError,
			"XKMSValidateRequest::load - called incorrect node");
	
	}

	// Load the base message
	m_request.load();

	// Now check for any QueryKeyBinding elements
	DOMElement * tmpElt = findFirstElementChild(m_msg.mp_messageAbstractTypeElement);
	while (tmpElt != NULL && 
		!strEquals(getXKMSLocalName(tmpElt), XKMSConstants::s_tagQueryKeyBinding)) 
		tmpElt = findNextElementChild(tmpElt);

	if (tmpElt != NULL) {

		XSECnew(mp_queryKeyBinding, XKMSQueryKeyBindingImpl(m_msg.mp_env, tmpElt));
		mp_queryKeyBinding->load();
		mp_queryKeyBindingElement = tmpElt;

	}

}

// --------------------------------------------------------------------------------
//           Create a blank one
// --------------------------------------------------------------------------------
DOMElement * XKMSValidateRequestImpl::createBlankValidateRequest(
		const XMLCh * service,
		const XMLCh * id) {

	return m_request.createBlankRequestAbstractType(
		XKMSConstants::s_tagValidateRequest, service, id);
//	return XKMSRequestAbstractTypeImpl::createBlankMessageAbstractType(
//		MAKE_UNICODE_STRING("ValidateRequest"), service, id);

}

// --------------------------------------------------------------------------------
//           Get interface methods
// --------------------------------------------------------------------------------

XKMSMessageAbstractType::messageType XKMSValidateRequestImpl::getMessageType(void) {

	return XKMSMessageAbstractTypeImpl::ValidateRequest;

}


XKMSQueryKeyBinding * XKMSValidateRequestImpl::getQueryKeyBinding(void) {

	return mp_queryKeyBinding;

}

// --------------------------------------------------------------------------------
//           Setter methods
// --------------------------------------------------------------------------------

XKMSQueryKeyBinding * XKMSValidateRequestImpl::addQueryKeyBinding(void) {

	if (mp_queryKeyBinding != NULL)
		return mp_queryKeyBinding;


	// OK - Nothing exists, so we need to create from scratch

	XSECnew(mp_queryKeyBinding, XKMSQueryKeyBindingImpl(m_msg.mp_env));
	mp_queryKeyBindingElement = mp_queryKeyBinding->createBlankQueryKeyBinding();

	if (m_msg.mp_messageAbstractTypeElement->getFirstChild() == NULL) {
		m_msg.mp_env->doPrettyPrint(m_msg.mp_messageAbstractTypeElement);
	}
	m_msg.mp_messageAbstractTypeElement->appendChild(mp_queryKeyBindingElement);
	m_msg.mp_env->doPrettyPrint(m_msg.mp_messageAbstractTypeElement);

	return mp_queryKeyBinding;

}
