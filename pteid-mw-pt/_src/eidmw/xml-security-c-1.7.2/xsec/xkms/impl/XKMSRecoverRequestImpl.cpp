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
 * XKMSRecoverRequestImpl := Implementation for RegisterRequest Messages
 *
 * $Id$
 *
 */

// XSEC Includes

#include <xsec/framework/XSECDefs.hpp>

#include <xsec/dsig/DSIGReference.hpp>
#include <xsec/framework/XSECEnv.hpp>
#include <xsec/framework/XSECError.hpp>
#include <xsec/utils/XSECDOMUtils.hpp>
#include <xsec/xkms/XKMSConstants.hpp>
#include <xsec/xkms/XKMSStatus.hpp>

#include "XKMSRecoverRequestImpl.hpp"
#include "XKMSRecoverKeyBindingImpl.hpp"
#include "XKMSAuthenticationImpl.hpp"

#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/XMLUniDefs.hpp>

XERCES_CPP_NAMESPACE_USE

// --------------------------------------------------------------------------------
//           Construct/Destruct
// --------------------------------------------------------------------------------

XKMSRecoverRequestImpl::XKMSRecoverRequestImpl(const XSECEnv * env) :
m_request(env),
m_msg(m_request.m_msg),
mp_authentication(NULL),
mp_recoverKeyBinding(NULL) {
}

XKMSRecoverRequestImpl::XKMSRecoverRequestImpl(const XSECEnv * env, DOMElement * node) :
m_request(env, node),
m_msg(m_request.m_msg),
mp_authentication(NULL),
mp_recoverKeyBinding(NULL) {
}

XKMSRecoverRequestImpl::~XKMSRecoverRequestImpl() {

	if (mp_authentication != NULL)
		delete mp_authentication;
	if (mp_recoverKeyBinding != NULL)
		delete mp_recoverKeyBinding;

}

// --------------------------------------------------------------------------------
//           Load
// --------------------------------------------------------------------------------

void XKMSRecoverRequestImpl::load(void) {

	if (m_msg.mp_messageAbstractTypeElement == NULL) {

		// Attempt to load an empty element
		throw XSECException(XSECException::XKMSError,
			"XKMSRecoverRequest::load - called on empty DOM");

	}

	if (!strEquals(getXKMSLocalName(m_msg.mp_messageAbstractTypeElement), 
									XKMSConstants::s_tagRecoverRequest)) {
	
		throw XSECException(XSECException::XKMSError,
			"XKMSRecoverRequest::load - called on incorrect node");
	
	}

	// Load the base message
	m_request.load();

	// Now check for any RecoverKeyBinding elements
	DOMElement * tmpElt = findFirstElementChild(m_msg.mp_messageAbstractTypeElement);
	while (tmpElt != NULL && !strEquals(getXKMSLocalName(tmpElt), XKMSConstants::s_tagRecoverKeyBinding)) {
		tmpElt = findNextElementChild(tmpElt);
	}

	if (tmpElt != NULL) {

		XSECnew(mp_recoverKeyBinding, XKMSRecoverKeyBindingImpl(m_msg.mp_env, tmpElt));
		mp_recoverKeyBinding->load();

		tmpElt = findNextElementChild(tmpElt);

	}
	else {

		throw XSECException(XSECException::ExpectedXKMSChildNotFound,
			"XKMSRecoverRequest::load - Expected RecoverKeyBinding node");
	
	}

	// Authentication Element 

	if (tmpElt != NULL && strEquals(getXKMSLocalName(tmpElt), XKMSConstants::s_tagAuthentication)) {

		XSECnew(mp_authentication, XKMSAuthenticationImpl(m_msg.mp_env, tmpElt));
		mp_authentication->load(mp_recoverKeyBinding->getId());

	}
	else {

		throw XSECException(XSECException::ExpectedXKMSChildNotFound,
			"XKMSRecoverRequest::load - Expected Authentication node");
	
	}

}
	
	
// --------------------------------------------------------------------------------
//           Create
// --------------------------------------------------------------------------------

DOMElement * XKMSRecoverRequestImpl::
	createBlankRecoverRequest(const XMLCh * service, const XMLCh * id) {

	return m_request.createBlankRequestAbstractType(
		XKMSConstants::s_tagRecoverRequest, service, id);

}

// --------------------------------------------------------------------------------
//           MessageType 
// --------------------------------------------------------------------------------

XKMSMessageAbstractType::messageType XKMSRecoverRequestImpl::getMessageType(void) {

	return XKMSMessageAbstractTypeImpl::RecoverRequest;

}

// --------------------------------------------------------------------------------
//           Get Methods
// --------------------------------------------------------------------------------

XKMSRecoverKeyBinding * XKMSRecoverRequestImpl::getRecoverKeyBinding(void) const {

	return mp_recoverKeyBinding;

}

XKMSAuthentication * XKMSRecoverRequestImpl::getAuthentication (void) const {

	return mp_authentication;

}

// --------------------------------------------------------------------------------
//           Set Methods
// --------------------------------------------------------------------------------

XKMSRecoverKeyBinding * XKMSRecoverRequestImpl::addRecoverKeyBinding(XKMSStatus::StatusValue status) {

	if (mp_recoverKeyBinding != NULL)
		return mp_recoverKeyBinding;


	// OK - Nothing exists, so we need to create from scratch

	XSECnew(mp_recoverKeyBinding, XKMSRecoverKeyBindingImpl(m_msg.mp_env));
	DOMElement * elt = mp_recoverKeyBinding->createBlankRecoverKeyBinding(status);

	// Insert

	DOMElement * be = findFirstElementChild(m_msg.mp_messageAbstractTypeElement);

	while (be != NULL && 
		!strEquals(getXKMSLocalName(be), XKMSConstants::s_tagAuthentication) &&
		!strEquals(getXKMSLocalName(be), XKMSConstants::s_tagRevocationCode)) {
		be = findNextElementChild(be);
	}

	if (be == NULL) {
		m_msg.mp_env->doPrettyPrint(m_msg.mp_messageAbstractTypeElement);
		m_msg.mp_messageAbstractTypeElement->appendChild(elt);
		m_msg.mp_env->doPrettyPrint(m_msg.mp_messageAbstractTypeElement);
		return mp_recoverKeyBinding;
	}

	m_msg.mp_messageAbstractTypeElement->insertBefore(elt, be);
	if (m_msg.mp_env->getPrettyPrintFlag() == true) {
		m_msg.mp_messageAbstractTypeElement->insertBefore(
			m_msg.mp_env->getParentDocument()->createTextNode(DSIGConstants::s_unicodeStrNL),
			be);
	}

	return mp_recoverKeyBinding;

}

XKMSAuthentication * XKMSRecoverRequestImpl::addAuthentication(void) {

	if (mp_authentication != NULL)
		return mp_authentication;

	if (mp_recoverKeyBinding == NULL) {
		throw XSECException(XSECException::XKMSError,
			"XKMSRecoverRequestImpl::addAuthentication - called prior to key infos being added");
	}

	XSECnew(mp_authentication, XKMSAuthenticationImpl(m_msg.mp_env));
	DOMElement * e = 
		mp_authentication->createBlankAuthentication(mp_recoverKeyBinding->getId());

	DOMElement * be = findFirstElementChild(m_msg.mp_messageAbstractTypeElement);

	while (be != NULL && !strEquals(getXKMSLocalName(be), XKMSConstants::s_tagRevocationCode))
		be = findNextElementChild(be);

	if (be == NULL) {
		m_msg.mp_env->doPrettyPrint(m_msg.mp_messageAbstractTypeElement);
		m_msg.mp_messageAbstractTypeElement->appendChild(e);
		m_msg.mp_env->doPrettyPrint(m_msg.mp_messageAbstractTypeElement);
		return mp_authentication;
	}

	m_msg.mp_messageAbstractTypeElement->insertBefore(e, be);
	if (m_msg.mp_env->getPrettyPrintFlag() == true) {
		m_msg.mp_messageAbstractTypeElement->insertBefore(
			m_msg.mp_env->getParentDocument()->createTextNode(DSIGConstants::s_unicodeStrNL),
			be);
	}

	return mp_authentication;

}


