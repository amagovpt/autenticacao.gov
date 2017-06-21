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
 * XKMSPrototypeKeyBindingImpl := Implementation of PrototypeKeyBinding elements
 *
 * $Id: XKMSPrototypeKeyBindingImpl.cpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/framework/XSECEnv.hpp>
#include <xsec/framework/XSECError.hpp>
#include <xsec/utils/XSECDOMUtils.hpp>
#include <xsec/xkms/XKMSConstants.hpp>

#include "XKMSPrototypeKeyBindingImpl.hpp"
#include "XKMSValidityIntervalImpl.hpp"

#include <xercesc/dom/DOM.hpp>

XERCES_CPP_NAMESPACE_USE

// --------------------------------------------------------------------------------
//           Construct/Destruct
// --------------------------------------------------------------------------------

XKMSPrototypeKeyBindingImpl::XKMSPrototypeKeyBindingImpl(const XSECEnv * env) :
XKMSKeyBindingAbstractTypeImpl(env),
mp_validityInterval(NULL),
mp_revocationCodeIdentifierElement(NULL) {

}

XKMSPrototypeKeyBindingImpl::XKMSPrototypeKeyBindingImpl(const XSECEnv * env, DOMElement * node) :
XKMSKeyBindingAbstractTypeImpl(env, node),
mp_validityInterval(NULL),
mp_revocationCodeIdentifierElement(NULL) {

}

XKMSPrototypeKeyBindingImpl::~XKMSPrototypeKeyBindingImpl() {

	if (mp_validityInterval != NULL)
		delete mp_validityInterval;
}

// --------------------------------------------------------------------------------
//           Load
// --------------------------------------------------------------------------------

void XKMSPrototypeKeyBindingImpl::load(void) {

	if (mp_keyBindingAbstractTypeElement == NULL) {
		throw XSECException(XSECException::ExpectedXKMSChildNotFound,
			"XKMSPrototypeKeyBindingImpl::load - called on empty DOM");
	}


	DOMElement * tmpElt = findFirstElementChild(mp_keyBindingAbstractTypeElement);

	while (tmpElt != NULL && !strEquals(getXKMSLocalName(tmpElt), 
									XKMSConstants::s_tagValidityInterval) &&
						  !strEquals(getXKMSLocalName(tmpElt), 
									XKMSConstants::s_tagRevocationCodeIdentifier))
		tmpElt = findNextElementChild(tmpElt);
						  

	if (tmpElt != NULL && strEquals(getXKMSLocalName(tmpElt), 
									XKMSConstants::s_tagValidityInterval)) {

		XSECnew(mp_validityInterval, XKMSValidityIntervalImpl(mp_env, tmpElt));
		mp_validityInterval->load();

		tmpElt = findNextElementChild(tmpElt);

	}

	if (tmpElt != NULL && strEquals(getXKMSLocalName(tmpElt), 
									XKMSConstants::s_tagRevocationCodeIdentifier)) {

		mp_revocationCodeIdentifierElement = tmpElt;

	}

	XKMSKeyBindingAbstractTypeImpl::load();

}

// --------------------------------------------------------------------------------
//           Create
// --------------------------------------------------------------------------------

DOMElement * XKMSPrototypeKeyBindingImpl::createBlankPrototypeKeyBinding(void) {

	DOMElement * ret = XKMSKeyBindingAbstractTypeImpl::
				createBlankKeyBindingAbstractType(XKMSConstants::s_tagPrototypeKeyBinding);

	// Must have an Id

	XKMSKeyBindingAbstractTypeImpl::setId();

	return ret;

}

// --------------------------------------------------------------------------------
//           Get methods
// --------------------------------------------------------------------------------

XKMSValidityInterval * XKMSPrototypeKeyBindingImpl::getValidityInterval(void) const {

	return mp_validityInterval;

}

const XMLCh * XKMSPrototypeKeyBindingImpl::getRevocationCodeIdentifier(void) const {

	if (mp_revocationCodeIdentifierElement == NULL)
		return NULL;

	DOMNode * t = findFirstChildOfType(mp_revocationCodeIdentifierElement, DOMNode::TEXT_NODE);
	if (t == NULL) {
		throw XSECException(XSECException::ExpectedXKMSChildNotFound,
			"XKMSPrototypeKeyBindingImpl::getRevocationCodeIdentifier - expected TEXT node");
	}
	
	return t->getNodeValue();
}

// --------------------------------------------------------------------------------
//           Set Methods
// --------------------------------------------------------------------------------

void XKMSPrototypeKeyBindingImpl::setValidityInterval(const XMLCh * notBefore, const XMLCh * notOnOrAfter) {

	XSECnew(mp_validityInterval, XKMSValidityIntervalImpl(mp_env));
	DOMElement * e = mp_validityInterval->createBlankValidityInterval(notBefore, notOnOrAfter);

	// Now insert
	if (mp_revocationCodeIdentifierElement != NULL) {
		mp_keyBindingAbstractTypeElement->insertBefore(e, mp_revocationCodeIdentifierElement);
		if (mp_env->getPrettyPrintFlag() == true) {
			mp_keyBindingAbstractTypeElement->insertBefore(
				mp_env->getParentDocument()->createTextNode(DSIGConstants::s_unicodeStrNL),
				mp_revocationCodeIdentifierElement);
		}
	}
	else
		mp_keyBindingAbstractTypeElement->appendChild(e);

}

void XKMSPrototypeKeyBindingImpl::setRevocationCodeIdentifier(const XMLCh * identifier) {

	safeBuffer str;
	DOMDocument *doc = mp_env->getParentDocument();
	const XMLCh * prefix = mp_env->getXKMSNSPrefix();

	makeQName(str, prefix, XKMSConstants::s_tagRevocationCodeIdentifier);

	mp_revocationCodeIdentifierElement = doc->createElementNS(XKMSConstants::s_unicodeStrURIXKMS, 
												str.rawXMLChBuffer());

	mp_keyBindingAbstractTypeElement->appendChild(mp_revocationCodeIdentifierElement);
	mp_env->doPrettyPrint(mp_keyBindingAbstractTypeElement);

	mp_revocationCodeIdentifierElement->appendChild(
		mp_env->getParentDocument()->createTextNode(identifier));

}


