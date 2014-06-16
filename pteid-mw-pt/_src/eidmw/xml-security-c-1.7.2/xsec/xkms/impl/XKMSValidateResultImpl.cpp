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
 * XKMSValidateResultImpl := Implementation of ValidateResult Messages
 *
 * $Id: XKMSValidateResultImpl.cpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/framework/XSECError.hpp>
#include <xsec/framework/XSECEnv.hpp>
#include <xsec/utils/XSECDOMUtils.hpp>
#include <xsec/xkms/XKMSConstants.hpp>

#include "XKMSValidateResultImpl.hpp"
#include "XKMSKeyBindingImpl.hpp"

#include <xercesc/dom/DOM.hpp>

XERCES_CPP_NAMESPACE_USE

// --------------------------------------------------------------------------------
//           Construct/Destruct
// --------------------------------------------------------------------------------

XKMSValidateResultImpl::XKMSValidateResultImpl(
		const XSECEnv * env) :
m_result(env),
m_msg(m_result.m_msg) {

}

XKMSValidateResultImpl::XKMSValidateResultImpl(
		const XSECEnv * env, 
		XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * node) :
m_result(env, node),
m_msg(m_result.m_msg) {

}

XKMSValidateResultImpl::~XKMSValidateResultImpl() {

	XKMSValidateResultImpl::KeyBindingVectorType::iterator i;

	for (i = m_keyBindingList.begin() ; i != m_keyBindingList.end(); ++i) {

		delete (*i);

	}

}


// --------------------------------------------------------------------------------
//           Load from DOM
// --------------------------------------------------------------------------------

// Load elements
void XKMSValidateResultImpl::load() {

	if (m_msg.mp_messageAbstractTypeElement == NULL) {

		// Attempt to load an empty element
		throw XSECException(XSECException::XKMSError,
			"XKMSValidateResult::load - called on empty DOM");

	}

	if (!strEquals(getXKMSLocalName(m_msg.mp_messageAbstractTypeElement), 
									XKMSConstants::s_tagValidateResult)) {
	
		throw XSECException(XSECException::XKMSError,
			"XKMSValidateResult::load - called incorrect node");
	
	}

	// Get any UnverifiedKeyBinding elements
	DOMNodeList * nl = m_msg.mp_messageAbstractTypeElement->getElementsByTagNameNS(
		XKMSConstants::s_unicodeStrURIXKMS,
		XKMSConstants::s_tagKeyBinding);

	if (nl != NULL) {

		XKMSKeyBindingImpl * kb;
		for (unsigned int i = 0; i < nl->getLength() ; ++ i) {

			XSECnew(kb, XKMSKeyBindingImpl(m_msg.mp_env, (DOMElement *) nl->item(i)));
			m_keyBindingList.push_back(kb);
			kb->load();

		}

	}


	// Load the base message
	m_result.load();

}

// --------------------------------------------------------------------------------
//           Create a blank one
// --------------------------------------------------------------------------------
DOMElement * XKMSValidateResultImpl::createBlankValidateResult(
		const XMLCh * service,
		const XMLCh * id,
		ResultMajor rmaj,
		ResultMinor rmin) {

	return m_result.createBlankResultType(
		XKMSConstants::s_tagValidateResult, service, id, rmaj, rmin);

}

// --------------------------------------------------------------------------------
//           Get interface methods
// --------------------------------------------------------------------------------

XKMSMessageAbstractType::messageType XKMSValidateResultImpl::getMessageType(void) {

	return XKMSMessageAbstractTypeImpl::ValidateResult;

}

// --------------------------------------------------------------------------------
//           UnverifiedKeyBinding handling
// --------------------------------------------------------------------------------


int XKMSValidateResultImpl::getKeyBindingSize(void) const {

	return (int) m_keyBindingList.size();

}

XKMSKeyBinding * XKMSValidateResultImpl::getKeyBindingItem(int item) const {

	if (item < 0 || item >= (int) m_keyBindingList.size()) {
		throw XSECException(XSECException::XKMSError,
			"XKMSValidateResult::getKeyBindingItem - item out of range");
	}

	return m_keyBindingList[item];

}

XKMSKeyBinding * XKMSValidateResultImpl::appendKeyBindingItem(XKMSStatus::StatusValue status) {

	XKMSKeyBindingImpl * u;

	XSECnew(u, XKMSKeyBindingImpl(m_msg.mp_env));

	m_keyBindingList.push_back(u);

	DOMElement * e = u->createBlankKeyBinding(status);

	// Append the element

	m_msg.mp_messageAbstractTypeElement->appendChild(e);
	m_msg.mp_env->doPrettyPrint(m_msg.mp_messageAbstractTypeElement);

	return u;

}
