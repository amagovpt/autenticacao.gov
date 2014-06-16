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
 * XKMSReissueRequestImpl := Implementation for ReissueRequest Messages
 *
 * $Id: XKMSReissueRequestImpl.cpp 351366 2005-06-04 11:44:55Z blautenb $
 *
 */

// XSEC Includes

#include <xsec/framework/XSECDefs.hpp>

#include <xsec/dsig/DSIGReference.hpp>
#include <xsec/framework/XSECEnv.hpp>
#include <xsec/framework/XSECError.hpp>
#include <xsec/utils/XSECDOMUtils.hpp>
#include <xsec/xkms/XKMSConstants.hpp>

#include "XKMSReissueRequestImpl.hpp"
#include "XKMSAuthenticationImpl.hpp"
#include "XKMSReissueKeyBindingImpl.hpp"

#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/XMLUniDefs.hpp>

XERCES_CPP_NAMESPACE_USE

// --------------------------------------------------------------------------------
//           Construct/Destruct
// --------------------------------------------------------------------------------

XKMSReissueRequestImpl::XKMSReissueRequestImpl(const XSECEnv * env) :
m_request(env),
m_msg(m_request.m_msg),
mp_authentication(NULL),
mp_reissueKeyBinding(NULL),
mp_proofOfPossessionSignature(NULL) {
}

XKMSReissueRequestImpl::XKMSReissueRequestImpl(const XSECEnv * env, DOMElement * node) :
m_request(env, node),
m_msg(m_request.m_msg),
mp_authentication(NULL),
mp_reissueKeyBinding(NULL),
mp_proofOfPossessionSignature(NULL) {
}

XKMSReissueRequestImpl::~XKMSReissueRequestImpl() {

	if (mp_authentication != NULL)
		delete mp_authentication;
	if (mp_reissueKeyBinding != NULL)
		delete mp_reissueKeyBinding;
	// Provider will take care of the proofOfPossession signature

}

// --------------------------------------------------------------------------------
//           Load
// --------------------------------------------------------------------------------

void XKMSReissueRequestImpl::load(void) {

	if (m_msg.mp_messageAbstractTypeElement == NULL) {

		// Attempt to load an empty element
		throw XSECException(XSECException::XKMSError,
			"XKMSReissueRequest::load - called on empty DOM");

	}

	if (!strEquals(getXKMSLocalName(m_msg.mp_messageAbstractTypeElement), 
									XKMSConstants::s_tagReissueRequest)) {
	
		throw XSECException(XSECException::XKMSError,
			"XKMSReissueRequest::load - called on incorrect node");
	
	}

	// Load the base message
	m_request.load();

	// Now check for any ReissueKeyBinding elements
	DOMElement * tmpElt = findFirstElementChild(m_msg.mp_messageAbstractTypeElement);
	while (tmpElt != NULL && !strEquals(getXKMSLocalName(tmpElt), XKMSConstants::s_tagReissueKeyBinding)) {
		tmpElt = findNextElementChild(tmpElt);
	}

	if (tmpElt != NULL) {

		XSECnew(mp_reissueKeyBinding, XKMSReissueKeyBindingImpl(m_msg.mp_env, tmpElt));
		mp_reissueKeyBinding->load();

		tmpElt = findNextElementChild(tmpElt);

	}
	else {

		throw XSECException(XSECException::ExpectedXKMSChildNotFound,
			"XKMSReissueRequest::load - Expected ReissueKeyBinding node");
	
	}

	// Authentication Element

	if (tmpElt != NULL && strEquals(getXKMSLocalName(tmpElt), XKMSConstants::s_tagAuthentication)) {

		XSECnew(mp_authentication, XKMSAuthenticationImpl(m_msg.mp_env, tmpElt));
		mp_authentication->load(mp_reissueKeyBinding->getId());

		tmpElt = findNextElementChild(tmpElt);

	}
	else {

		throw XSECException(XSECException::ExpectedXKMSChildNotFound,
			"XKMSReissueRequest::load - Expected Authentication node");
	
	}

	if (tmpElt != NULL && strEquals(getXKMSLocalName(tmpElt), XKMSConstants::s_tagProofOfPossession)) {

		// Find the signature

		DOMElement * sigElt = (DOMElement *) findFirstElementChild(tmpElt);

		if (sigElt == NULL || !strEquals(getDSIGLocalName(sigElt), 
			XKMSConstants::s_tagSignature)) {
			throw XSECException(XSECException::ExpectedXKMSChildNotFound,
				"XKMSReissueRequest::load - Expected Signature child of ProofOfPossession");

		}

		// The provider will take care of cleaning this up later.

		mp_proofOfPossessionSignature = m_prov.newSignatureFromDOM(m_msg.mp_env->getParentDocument(), 
													  sigElt);
		mp_proofOfPossessionSignature->load();

		// Check the signature is across the correct input
			
		DSIGReferenceList * rl = 
			mp_proofOfPossessionSignature->getReferenceList();

		if (rl->getSize() != 1) {
			throw XSECException(XSECException::XKMSError,
				"XKMSReissueRequestImpl::load - ProofOfPossession Signature with incorrect number of references found (should be 1)");
		}

		safeBuffer sb;
		sb.sbXMLChIn(DSIGConstants::s_unicodeStrEmpty);
		sb.sbXMLChAppendCh(chPound);
		sb.sbXMLChCat(mp_reissueKeyBinding->getId());

		if (!strEquals(rl->item(0)->getURI(), sb.rawXMLChBuffer())) {
			throw XSECException(XSECException::XKMSError,
				"XKMSReissueRequestImpl::load - ProofOfPossession Signature refers to incorrect Id (should be for ReissueKeyBinding)");
		}

		// We don't actually check the signature as we have no key material to do so!
	}
}
	
	
// --------------------------------------------------------------------------------
//           Create
// --------------------------------------------------------------------------------

DOMElement * XKMSReissueRequestImpl::
	createBlankReissueRequest(const XMLCh * service, const XMLCh * id) {

	return m_request.createBlankRequestAbstractType(
		XKMSConstants::s_tagReissueRequest, service, id);

}

// --------------------------------------------------------------------------------
//           MessageType 
// --------------------------------------------------------------------------------

XKMSMessageAbstractType::messageType XKMSReissueRequestImpl::getMessageType(void) {

	return XKMSMessageAbstractTypeImpl::ReissueRequest;

}

// --------------------------------------------------------------------------------
//           Get Methods
// --------------------------------------------------------------------------------

XKMSReissueKeyBinding * XKMSReissueRequestImpl::getReissueKeyBinding(void) const {

	return mp_reissueKeyBinding;

}

XKMSAuthentication * XKMSReissueRequestImpl::getAuthentication (void) const {

	return mp_authentication;

}

DSIGSignature * XKMSReissueRequestImpl::getProofOfPossessionSignature(void) const {

	return mp_proofOfPossessionSignature;

}

// --------------------------------------------------------------------------------
//           Set Methods
// --------------------------------------------------------------------------------

XKMSReissueKeyBinding * XKMSReissueRequestImpl::addReissueKeyBinding(XKMSStatus::StatusValue status) {

	if (mp_reissueKeyBinding != NULL)
		return mp_reissueKeyBinding;


	// OK - Nothing exists, so we need to create from scratch

	XSECnew(mp_reissueKeyBinding, XKMSReissueKeyBindingImpl(m_msg.mp_env));
	DOMElement * elt = mp_reissueKeyBinding->createBlankReissueKeyBinding(status);

	// Insert

	DOMElement * be = findFirstElementChild(m_msg.mp_messageAbstractTypeElement);

	while (be != NULL && 
		!strEquals(getXKMSLocalName(be), XKMSConstants::s_tagAuthentication) &&
		!strEquals(getXKMSLocalName(be), XKMSConstants::s_tagProofOfPossession)) {
		be = findNextElementChild(be);
	}

	if (be == NULL) {
		m_msg.mp_env->doPrettyPrint(m_msg.mp_messageAbstractTypeElement);
		m_msg.mp_messageAbstractTypeElement->appendChild(elt);
		m_msg.mp_env->doPrettyPrint(m_msg.mp_messageAbstractTypeElement);
		return mp_reissueKeyBinding;
	}

	m_msg.mp_messageAbstractTypeElement->insertBefore(elt, be);
	if (m_msg.mp_env->getPrettyPrintFlag() == true) {
		m_msg.mp_messageAbstractTypeElement->insertBefore(
			m_msg.mp_env->getParentDocument()->createTextNode(DSIGConstants::s_unicodeStrNL),
			be);
	}

	return mp_reissueKeyBinding;

}

XKMSAuthentication * XKMSReissueRequestImpl::addAuthentication(void) {

	if (mp_authentication != NULL)
		return mp_authentication;

	if (mp_reissueKeyBinding == NULL) {
		throw XSECException(XSECException::XKMSError,
			"XKMSReissueRequestImpl::addAuthentication - called prior to key infos being added");
	}

	XSECnew(mp_authentication, XKMSAuthenticationImpl(m_msg.mp_env));
	DOMElement * e = 
		mp_authentication->createBlankAuthentication(mp_reissueKeyBinding->getId());

	DOMElement * be = findFirstElementChild(m_msg.mp_messageAbstractTypeElement);

	while (be != NULL && !strEquals(getXKMSLocalName(be), XKMSConstants::s_tagProofOfPossession))
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

DSIGSignature * XKMSReissueRequestImpl::addProofOfPossessionSignature(
		canonicalizationMethod cm,
		signatureMethod	sm,
		hashMethod hm) {

	DSIGSignature * ret = m_prov.newSignature();
	DOMElement * elt = ret->createBlankSignature(m_msg.mp_env->getParentDocument(), cm, sm, hm);

	/* Create the enveloping reference */
	safeBuffer sb;
	sb.sbXMLChIn(DSIGConstants::s_unicodeStrEmpty);
	sb.sbXMLChAppendCh(chPound);
	sb.sbXMLChCat(mp_reissueKeyBinding->getId());

	DSIGReference *ref = ret->createReference(sb.rawXMLChBuffer());
	ref->appendCanonicalizationTransform(CANON_C14NE_COM);

	/* Embed the signature in the document inside a KeyBindingAuthentication element */
	safeBuffer str;
	DOMDocument *doc = m_msg.mp_env->getParentDocument();
	const XMLCh * prefix = m_msg.mp_env->getXKMSNSPrefix();

	makeQName(str, prefix, XKMSConstants::s_tagProofOfPossession);

	DOMElement * t = doc->createElementNS(XKMSConstants::s_unicodeStrURIXKMS, 
												str.rawXMLChBuffer());

	m_msg.mp_env->doPrettyPrint(t);
	t->appendChild(elt);
	m_msg.mp_env->doPrettyPrint(t);

	// Now append into the ReissueRequest
	m_msg.mp_messageAbstractTypeElement->appendChild(t);
	m_msg.mp_env->doPrettyPrint(m_msg.mp_messageAbstractTypeElement);

	return ret;
}



