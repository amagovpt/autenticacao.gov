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
 * XKMSValidityInterval := Implementation for ValidityInterval elements
 *
 * $Id: XKMSValidityIntervalImpl.cpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/framework/XSECEnv.hpp>
#include <xsec/framework/XSECError.hpp>
#include <xsec/utils/XSECDOMUtils.hpp>
#include <xsec/xkms/XKMSConstants.hpp>

#include "XKMSValidityIntervalImpl.hpp"

#include <xercesc/dom/DOM.hpp>

XERCES_CPP_NAMESPACE_USE

// --------------------------------------------------------------------------------
//           Construct/Destruct
// --------------------------------------------------------------------------------

XKMSValidityIntervalImpl::XKMSValidityIntervalImpl(const XSECEnv * env) :
mp_env(env),
mp_validityIntervalElement(NULL),
mp_notBeforeElement(NULL),
mp_notOnOrAfterElement(NULL) {

}


XKMSValidityIntervalImpl::XKMSValidityIntervalImpl(const XSECEnv * env, DOMElement * node) :
mp_env(env),
mp_validityIntervalElement(node),
mp_notBeforeElement(NULL),
mp_notOnOrAfterElement(NULL) {

}

XKMSValidityIntervalImpl::~XKMSValidityIntervalImpl() {

}

// --------------------------------------------------------------------------------
//           Load
// --------------------------------------------------------------------------------

void XKMSValidityIntervalImpl::load(void) {

	if (mp_validityIntervalElement == NULL) {
		throw XSECException(XSECException::ExpectedXKMSChildNotFound,
			"XKMSValidityIntervalImpl::load - called on empty DOM");
	}


	DOMElement * tmpElt = findFirstElementChild(mp_validityIntervalElement);

	if (tmpElt != NULL && strEquals(getXKMSLocalName(tmpElt), 
									XKMSConstants::s_tagNotBefore)) {
		mp_notBeforeElement = tmpElt;
		tmpElt = findNextElementChild(tmpElt);
	}

	if (tmpElt != NULL && strEquals(getXKMSLocalName(tmpElt), 
									XKMSConstants::s_tagNotOnOrAfter)) {
		mp_notOnOrAfterElement = tmpElt;
	}

}

// --------------------------------------------------------------------------------
//           Create
// --------------------------------------------------------------------------------

DOMElement * XKMSValidityIntervalImpl::createBlankValidityInterval(const XMLCh * notBefore, 
																   const XMLCh * notOnOrAfter) {

	// Get some setup values
	safeBuffer str;
	DOMDocument *doc = mp_env->getParentDocument();
	const XMLCh * prefix = mp_env->getXKMSNSPrefix();

	makeQName(str, prefix, XKMSConstants::s_tagValidityInterval);

	mp_validityIntervalElement = 
		doc->createElementNS(XKMSConstants::s_unicodeStrURIXKMS, 
												str.rawXMLChBuffer());

	mp_env->doPrettyPrint(mp_validityIntervalElement);

	if (notBefore != NULL) {
		makeQName(str, prefix, XKMSConstants::s_tagNotBefore);
		DOMElement * e = 
			doc->createElementNS(XKMSConstants::s_unicodeStrURIXKMS, str.rawXMLChBuffer());
		mp_validityIntervalElement->appendChild(e);
		e->appendChild(doc->createTextNode(notBefore));
		mp_env->doPrettyPrint(mp_validityIntervalElement);
	}
	if (notOnOrAfter != NULL) {
		makeQName(str, prefix, XKMSConstants::s_tagNotOnOrAfter);
		DOMElement * e = 
			doc->createElementNS(XKMSConstants::s_unicodeStrURIXKMS, str.rawXMLChBuffer());
		mp_validityIntervalElement->appendChild(e);
		e->appendChild(doc->createTextNode(notOnOrAfter));
		mp_env->doPrettyPrint(mp_validityIntervalElement);
	}

	return mp_validityIntervalElement;
}

// --------------------------------------------------------------------------------
//           Get Methods
// --------------------------------------------------------------------------------

const XMLCh * XKMSValidityIntervalImpl::getNotBefore(void) const {

	if (mp_notBeforeElement == NULL)
		return NULL;

	DOMNode * t = findFirstChildOfType(mp_notBeforeElement, DOMNode::TEXT_NODE);
	if (t == NULL) {
		throw XSECException(XSECException::ExpectedXKMSChildNotFound,
			"XKMSValidityIntervalImpl::getNotBefore - expected TEXT node");
	}
	
	return t->getNodeValue();

}

const XMLCh * XKMSValidityIntervalImpl::getNotOnOrAfter(void) const {

	if (mp_notOnOrAfterElement == NULL)
		return NULL;

	DOMNode * t = findFirstChildOfType(mp_notOnOrAfterElement, DOMNode::TEXT_NODE);
	if (t == NULL) {
		throw XSECException(XSECException::ExpectedXKMSChildNotFound,
			"XKMSValidityIntervalImpl::getNotOnOrAfter - expected TEXT node");
	}
	
	return t->getNodeValue();

}

// --------------------------------------------------------------------------------
//           Set Methods
// --------------------------------------------------------------------------------


void XKMSValidityIntervalImpl::setNotBefore(const XMLCh * val) {

	if (mp_notBeforeElement == NULL) {
		safeBuffer str;
		DOMDocument *doc = mp_env->getParentDocument();
		const XMLCh * prefix = mp_env->getXKMSNSPrefix();

		makeQName(str, prefix, XKMSConstants::s_tagNotBefore);

		mp_notBeforeElement = doc->createElementNS(XKMSConstants::s_unicodeStrURIXKMS, 
												str.rawXMLChBuffer());

		mp_notBeforeElement->appendChild(doc->createTextNode(val));

		// Now insert appropriately

		if (mp_notOnOrAfterElement != NULL) {
			mp_validityIntervalElement->insertBefore(mp_notBeforeElement, mp_notOnOrAfterElement);
			if (mp_env->getPrettyPrintFlag() == true) {
				mp_validityIntervalElement->insertBefore(
					doc->createTextNode(DSIGConstants::s_unicodeStrNL),
					mp_notOnOrAfterElement);

			}
		}
		else {
			mp_validityIntervalElement->appendChild(mp_notBeforeElement);
			mp_env->doPrettyPrint(mp_validityIntervalElement);
		}

		return;
	}

	DOMNode * t = findFirstChildOfType(mp_notBeforeElement, DOMNode::TEXT_NODE);
	if (t == NULL) {
		throw XSECException(XSECException::ExpectedXKMSChildNotFound,
			"XKMSValidityIntervalImpl::getNotBefore - expected TEXT node");
	}

	t->setNodeValue(val);

}

void XKMSValidityIntervalImpl::setNotOnOrAfter(const XMLCh * val) {

	if (mp_notOnOrAfterElement == NULL) {
		safeBuffer str;
		DOMDocument *doc = mp_env->getParentDocument();
		const XMLCh * prefix = mp_env->getXKMSNSPrefix();

		makeQName(str, prefix, XKMSConstants::s_tagNotOnOrAfter);

		mp_notOnOrAfterElement = doc->createElementNS(XKMSConstants::s_unicodeStrURIXKMS, 
												str.rawXMLChBuffer());

		mp_notOnOrAfterElement->appendChild(doc->createTextNode(val));

		// Now insert appropriately

		mp_validityIntervalElement->appendChild(mp_notOnOrAfterElement);
		mp_env->doPrettyPrint(mp_validityIntervalElement);

		return;
	}

	DOMNode * t = findFirstChildOfType(mp_notOnOrAfterElement, DOMNode::TEXT_NODE);
	if (t == NULL) {
		throw XSECException(XSECException::ExpectedXKMSChildNotFound,
			"XKMSValidityIntervalImpl::getNotOnOrAfter - expected TEXT node");
	}

	t->setNodeValue(val);

}


