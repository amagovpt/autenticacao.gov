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
 * XKMSRevokeRequestImpl := Implementation for RegisterRequest Messages
 *
 * $Id:$
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

#include "XKMSRevokeRequestImpl.hpp"
#include "XKMSRevokeKeyBindingImpl.hpp"
#include "XKMSAuthenticationImpl.hpp"

#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/XMLUniDefs.hpp>

XERCES_CPP_NAMESPACE_USE

// --------------------------------------------------------------------------------
//           Construct/Destruct
// --------------------------------------------------------------------------------

XKMSRevokeRequestImpl::XKMSRevokeRequestImpl(const XSECEnv * env) :
m_request(env),
m_msg(m_request.m_msg),
mp_authentication(NULL),
mp_revokeKeyBinding(NULL),
mp_revocationCodeElement(NULL) {
}

XKMSRevokeRequestImpl::XKMSRevokeRequestImpl(const XSECEnv * env, DOMElement * node) :
m_request(env, node),
m_msg(m_request.m_msg),
mp_authentication(NULL),
mp_revokeKeyBinding(NULL),
mp_revocationCodeElement(NULL) {
}

XKMSRevokeRequestImpl::~XKMSRevokeRequestImpl() {

	if (mp_authentication != NULL)
		delete mp_authentication;
	if (mp_revokeKeyBinding != NULL)
		delete mp_revokeKeyBinding;

}

// --------------------------------------------------------------------------------
//           Load
// --------------------------------------------------------------------------------

void XKMSRevokeRequestImpl::load(void) {

	if (m_msg.mp_messageAbstractTypeElement == NULL) {

		// Attempt to load an empty element
		throw XSECException(XSECException::XKMSError,
			"XKMSRevokeRequest::load - called on empty DOM");

	}

	if (!strEquals(getXKMSLocalName(m_msg.mp_messageAbstractTypeElement), 
									XKMSConstants::s_tagRevokeRequest)) {
	
		throw XSECException(XSECException::XKMSError,
			"XKMSRevokeRequest::load - called on incorrect node");
	
	}

	// Load the base message
	m_request.load();

	// Now check for any RevokeKeyBinding elements
	DOMElement * tmpElt = findFirstElementChild(m_msg.mp_messageAbstractTypeElement);
	while (tmpElt != NULL && !strEquals(getXKMSLocalName(tmpElt), XKMSConstants::s_tagRevokeKeyBinding)) {
		tmpElt = findNextElementChild(tmpElt);
	}

	if (tmpElt != NULL) {

		XSECnew(mp_revokeKeyBinding, XKMSRevokeKeyBindingImpl(m_msg.mp_env, tmpElt));
		mp_revokeKeyBinding->load();

		tmpElt = findNextElementChild(tmpElt);

	}
	else {

		throw XSECException(XSECException::ExpectedXKMSChildNotFound,
			"XKMSRevokeRequest::load - Expected RevokeKeyBinding node");
	
	}

	// Authentication Element | RevocationCode Element

	if (tmpElt != NULL && strEquals(getXKMSLocalName(tmpElt), XKMSConstants::s_tagAuthentication)) {

		XSECnew(mp_authentication, XKMSAuthenticationImpl(m_msg.mp_env, tmpElt));
		mp_authentication->load(mp_revokeKeyBinding->getId());

	}
	else if (tmpElt != NULL && strEquals(getXKMSLocalName(tmpElt), XKMSConstants::s_tagRevocationCode)) {

		mp_revocationCodeElement = tmpElt;

	}
	else {

		throw XSECException(XSECException::ExpectedXKMSChildNotFound,
			"XKMSRevokeRequest::load - Expected Authentication or RevocationCode nodes");
	
	}

}
	
	
// --------------------------------------------------------------------------------
//           Create
// --------------------------------------------------------------------------------

DOMElement * XKMSRevokeRequestImpl::
	createBlankRevokeRequest(const XMLCh * service, const XMLCh * id) {

	return m_request.createBlankRequestAbstractType(
		XKMSConstants::s_tagRevokeRequest, service, id);

}

// --------------------------------------------------------------------------------
//           MessageType 
// --------------------------------------------------------------------------------

XKMSMessageAbstractType::messageType XKMSRevokeRequestImpl::getMessageType(void) {

	return XKMSMessageAbstractTypeImpl::RevokeRequest;

}

// --------------------------------------------------------------------------------
//           Get Methods
// --------------------------------------------------------------------------------

XKMSRevokeKeyBinding * XKMSRevokeRequestImpl::getRevokeKeyBinding(void) const {

	return mp_revokeKeyBinding;

}

XKMSAuthentication * XKMSRevokeRequestImpl::getAuthentication (void) const {

	return mp_authentication;

}

const XMLCh * XKMSRevokeRequestImpl::getRevocationCode(void) const {

	if (mp_revocationCodeElement == NULL)
		return NULL;

	DOMNode * t = findFirstChildOfType(mp_revocationCodeElement, DOMNode::TEXT_NODE);

	if (t == NULL) {
		throw XSECException(XSECException::ExpectedXKMSChildNotFound,
			"XKMSRevokeRequestImpl::getRevocationCode - expected TEXT node");
	}
	
	return t->getNodeValue();
}

// --------------------------------------------------------------------------------
//           Set Methods
// --------------------------------------------------------------------------------

XKMSRevokeKeyBinding * XKMSRevokeRequestImpl::addRevokeKeyBinding(XKMSStatus::StatusValue status) {

	if (mp_revokeKeyBinding != NULL)
		return mp_revokeKeyBinding;


	// OK - Nothing exists, so we need to create from scratch

	XSECnew(mp_revokeKeyBinding, XKMSRevokeKeyBindingImpl(m_msg.mp_env));
	DOMElement * elt = mp_revokeKeyBinding->createBlankRevokeKeyBinding(status);

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
		return mp_revokeKeyBinding;
	}

	m_msg.mp_messageAbstractTypeElement->insertBefore(elt, be);
	if (m_msg.mp_env->getPrettyPrintFlag() == true) {
		m_msg.mp_messageAbstractTypeElement->insertBefore(
			m_msg.mp_env->getParentDocument()->createTextNode(DSIGConstants::s_unicodeStrNL),
			be);
	}

	return mp_revokeKeyBinding;

}

XKMSAuthentication * XKMSRevokeRequestImpl::addAuthentication(void) {

	if (mp_authentication != NULL)
		return mp_authentication;

	if (mp_revokeKeyBinding == NULL) {
		throw XSECException(XSECException::XKMSError,
			"XKMSRevokeRequestImpl::addAuthentication - called prior to key infos being added");
	}

	XSECnew(mp_authentication, XKMSAuthenticationImpl(m_msg.mp_env));
	DOMElement * e = 
		mp_authentication->createBlankAuthentication(mp_revokeKeyBinding->getId());

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

void XKMSRevokeRequestImpl::addRevocationCode(const XMLCh * code) {

	safeBuffer str;
	DOMDocument *doc = m_msg.mp_env->getParentDocument();
	const XMLCh * prefix = m_msg.mp_env->getXKMSNSPrefix();

	makeQName(str, prefix, XKMSConstants::s_tagRevocationCode);

	mp_revocationCodeElement = doc->createElementNS(XKMSConstants::s_unicodeStrURIXKMS, 
												str.rawXMLChBuffer());

	m_msg.mp_messageAbstractTypeElement->appendChild(mp_revocationCodeElement);
	m_msg.mp_env->doPrettyPrint(m_msg.mp_messageAbstractTypeElement);

	mp_revocationCodeElement->appendChild(
		m_msg.mp_env->getParentDocument()->createTextNode(code));

}


