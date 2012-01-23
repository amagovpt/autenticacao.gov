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
 * XKMSAuthenticationImpl := Implementation of Authentication elements
 *
 * $Id: XKMSAuthenticationImpl.cpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/framework/XSECEnv.hpp>
#include <xsec/framework/XSECError.hpp>
#include <xsec/xkms/XKMSConstants.hpp>

#include <xsec/dsig/DSIGReference.hpp>

#include "XKMSAuthenticationImpl.hpp"
#include "XKMSNotBoundAuthenticationImpl.hpp"

#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/XMLUniDefs.hpp>

XERCES_CPP_NAMESPACE_USE

// --------------------------------------------------------------------------------
//           Constructor/Destructor
// --------------------------------------------------------------------------------

XKMSAuthenticationImpl::XKMSAuthenticationImpl(const XSECEnv * env) :
mp_env(env),
mp_authenticationElement(NULL),
mp_keyBindingAuthenticationSignatureElement(NULL),
mp_keyBindingAuthenticationSignature(NULL),
mp_notBoundAuthentication(NULL)
{
}

XKMSAuthenticationImpl::XKMSAuthenticationImpl(const XSECEnv * env, DOMElement * node) :
mp_env(env),
mp_authenticationElement(node),
mp_keyBindingAuthenticationSignatureElement(NULL),
mp_keyBindingAuthenticationSignature(NULL),
mp_notBoundAuthentication(NULL)
{
}

XKMSAuthenticationImpl::~XKMSAuthenticationImpl() {

	if (mp_notBoundAuthentication != NULL)
		delete mp_notBoundAuthentication;
}

// --------------------------------------------------------------------------------
//           Load
// --------------------------------------------------------------------------------

void XKMSAuthenticationImpl::load(const XMLCh * id) {

	if (mp_authenticationElement == NULL) {
		throw XSECException(XSECException::ExpectedXKMSChildNotFound,
			"XKMSAuthenticationImpl::load - called on empty DOM");
	}

	// Store for later use
	mp_keyBindingId = id;

	DOMElement * tmpElt = findFirstElementChild(mp_authenticationElement);

	if (tmpElt != NULL && strEquals(getXKMSLocalName(tmpElt), 
									XKMSConstants::s_tagKeyBindingAuthentication)) {

		// Find the signature
		mp_keyBindingAuthenticationSignatureElement = 
			(DOMElement *) findFirstElementChild(tmpElt);

		while (mp_keyBindingAuthenticationSignatureElement != NULL && 
			!strEquals(getDSIGLocalName(mp_keyBindingAuthenticationSignatureElement), 
						XKMSConstants::s_tagSignature)) {

			mp_keyBindingAuthenticationSignatureElement= 
				findNextElementChild(mp_keyBindingAuthenticationSignatureElement);

		}

		// The provider will take care of cleaning this up later.

		if (mp_keyBindingAuthenticationSignatureElement != NULL) {

			mp_keyBindingAuthenticationSignature = m_prov.newSignatureFromDOM(
													  mp_keyBindingAuthenticationSignatureElement->getOwnerDocument(), 
													  mp_keyBindingAuthenticationSignatureElement);
			mp_keyBindingAuthenticationSignature->load();

			// Check the signature is across the correct input
			
			DSIGReferenceList * rl = 
				mp_keyBindingAuthenticationSignature->getReferenceList();

			if (rl->getSize() != 1) {
				throw XSECException(XSECException::XKMSError,
					"XKMSAuthenticationImpl::load - KeyBindingAuthentication Signature with incorrect number of references found (should be 1)");
			}

			safeBuffer sb;
			sb.sbXMLChIn(DSIGConstants::s_unicodeStrEmpty);
			sb.sbXMLChAppendCh(chPound);
			sb.sbXMLChCat(mp_keyBindingId);

			if (!strEquals(rl->item(0)->getURI(), sb.rawXMLChBuffer())) {
				throw XSECException(XSECException::XKMSError,
					"XKMSAuthenticationImpl::load - KeyBindingAuthentication Signature refers to incorrect Id (should be for KeyBinding)");
			}

			// We don't actually check the signature as we have no key material to do so!
		}

		tmpElt = findNextElementChild(tmpElt);

	}

	if (tmpElt != NULL && strEquals(getXKMSLocalName(tmpElt), 
									XKMSConstants::s_tagNotBoundAuthentication)) {

		XSECnew(mp_notBoundAuthentication, XKMSNotBoundAuthenticationImpl(mp_env, tmpElt));
		mp_notBoundAuthentication->load();

	}

}

// --------------------------------------------------------------------------------
//           Create
// --------------------------------------------------------------------------------

DOMElement * XKMSAuthenticationImpl::createBlankAuthentication(const XMLCh * id) {

	// Get some setup values
	safeBuffer str;
	DOMDocument *doc = mp_env->getParentDocument();
	const XMLCh * prefix = mp_env->getXKMSNSPrefix();

	makeQName(str, prefix, XKMSConstants::s_tagAuthentication);

	mp_authenticationElement = doc->createElementNS(XKMSConstants::s_unicodeStrURIXKMS, 
												str.rawXMLChBuffer());

	mp_env->doPrettyPrint(mp_authenticationElement);

	// This isn't used immediately - it is simply held for when we create a signature
	mp_keyBindingId = id;

	return mp_authenticationElement;

}


// --------------------------------------------------------------------------------
//           Interfacet - Get
// --------------------------------------------------------------------------------

DSIGSignature * XKMSAuthenticationImpl::getKeyBindingAuthenticationSignature(void) const {

	return mp_keyBindingAuthenticationSignature;

}

XKMSNotBoundAuthentication * XKMSAuthenticationImpl::getNotBoundAuthentication(void) const {

	return mp_notBoundAuthentication;

}

// --------------------------------------------------------------------------------
//           Interface - Set
// --------------------------------------------------------------------------------

DSIGSignature * XKMSAuthenticationImpl::addKeyBindingAuthenticationSignature(
		canonicalizationMethod cm,
		signatureMethod	sm,
		hashMethod hm) {

	if (mp_keyBindingId == NULL) {
		throw XSECException(XSECException::XKMSError,
			"XKMSAuthenticationImpl::addKeyBindingAuthenticationSignature - called prior to key infos being added");
	}

	DSIGSignature * ret = m_prov.newSignature();
	DOMElement * elt = ret->createBlankSignature(mp_env->getParentDocument(), cm, sm, hm);

	/* Create the enveloping reference */
	safeBuffer sb;
	sb.sbXMLChIn(DSIGConstants::s_unicodeStrEmpty);
	sb.sbXMLChAppendCh(chPound);
	sb.sbXMLChCat(mp_keyBindingId);

	DSIGReference *ref = ret->createReference(sb.rawXMLChBuffer());
	ref->appendCanonicalizationTransform(CANON_C14NE_COM);

	/* Embed the signature in the document inside a KeyBindingAuthentication element */
	safeBuffer str;
	DOMDocument *doc = mp_env->getParentDocument();
	const XMLCh * prefix = mp_env->getXKMSNSPrefix();

	makeQName(str, prefix, XKMSConstants::s_tagKeyBindingAuthentication);

	DOMElement * t = doc->createElementNS(XKMSConstants::s_unicodeStrURIXKMS, 
												str.rawXMLChBuffer());

	mp_env->doPrettyPrint(t);
	t->appendChild(elt);
	mp_env->doPrettyPrint(t);

	mp_authenticationElement->appendChild(t);
	mp_env->doPrettyPrint(mp_authenticationElement);

	return ret;
}

void XKMSAuthenticationImpl::setNotBoundAuthentication(
		const XMLCh * uri, 
		const XMLCh * value) {

	XSECnew(mp_notBoundAuthentication, XKMSNotBoundAuthenticationImpl(mp_env));
	mp_authenticationElement->appendChild(mp_notBoundAuthentication->createBlankNotBoundAuthentication(uri, value));
	mp_env->doPrettyPrint(mp_authenticationElement);

}

