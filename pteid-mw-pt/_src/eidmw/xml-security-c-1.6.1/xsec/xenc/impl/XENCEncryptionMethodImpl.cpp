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
 * XENCEncryptionMethod := Interface definition for EncryptionMethod element
 *
 * $Id: XENCEncryptionMethodImpl.cpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/framework/XSECError.hpp>
#include <xsec/utils/XSECDOMUtils.hpp>
#include <xsec/framework/XSECEnv.hpp>

#include "XENCEncryptionMethodImpl.hpp"

#include <xercesc/util/XMLUniDefs.hpp>
#include <xercesc/dom/DOM.hpp>

XERCES_CPP_NAMESPACE_USE

// --------------------------------------------------------------------------------
//			UNICODE Strings
// --------------------------------------------------------------------------------

static XMLCh s_EncryptionMethod[] = {

	chLatin_E,
	chLatin_n,
	chLatin_c,
	chLatin_r,
	chLatin_y,
	chLatin_p,
	chLatin_t,
	chLatin_i,
	chLatin_o,
	chLatin_n,
	chLatin_M,
	chLatin_e,
	chLatin_t,
	chLatin_h,
	chLatin_o,
	chLatin_d,
	chNull,
};

static XMLCh s_DigestMethod[] = {

	chLatin_D,
	chLatin_i,
	chLatin_g,
	chLatin_e,
	chLatin_s,
	chLatin_t,
	chLatin_M,
	chLatin_e,
	chLatin_t,
	chLatin_h,
	chLatin_o,
	chLatin_d,
	chNull
};

static XMLCh s_OAEPparams [] = {

	chLatin_O,
	chLatin_A,
	chLatin_E,
	chLatin_P,
	chLatin_p,
	chLatin_a,
	chLatin_r,
	chLatin_a,
	chLatin_m,
	chLatin_s,
	chNull
};

static XMLCh s_KeySize [] = {

	chLatin_K,
	chLatin_e,
	chLatin_y,
	chLatin_S,
	chLatin_i,
	chLatin_z,
	chLatin_e,
	chNull
};

// --------------------------------------------------------------------------------
//			Constructors and Destructors
// --------------------------------------------------------------------------------

XENCEncryptionMethodImpl::XENCEncryptionMethodImpl(const XSECEnv * env) :
mp_env(env),
mp_encryptionMethodElement(NULL),
mp_algorithmAttr(NULL),
mp_digestAlgorithmAttr(NULL),
mp_oaepParamsTextNode(NULL),
mp_keySizeTextNode(NULL) {

}

XENCEncryptionMethodImpl::XENCEncryptionMethodImpl(
		const XSECEnv * env, 
		DOMElement * element) :
mp_env(env),
mp_encryptionMethodElement(element),
mp_algorithmAttr(NULL),
mp_digestAlgorithmAttr(NULL),
mp_oaepParamsTextNode(NULL),
mp_keySizeTextNode(NULL) {

}

XENCEncryptionMethodImpl::~XENCEncryptionMethodImpl() {


}


// --------------------------------------------------------------------------------
//			Load from DOM
// --------------------------------------------------------------------------------


void XENCEncryptionMethodImpl::load() {

	if (mp_encryptionMethodElement == NULL) {

		// Attempt to load an empty encryptedType element
		throw XSECException(XSECException::EncryptionMethodError,
			"XENCEncryptionMethod::load - called on empty DOM");

	}

	if (!strEquals(getXENCLocalName(mp_encryptionMethodElement), s_EncryptionMethod)) {

		// Attempt to load an empty encryptedData element
		throw XSECException(XSECException::EncryptionMethodError,
			"XENCEncryptionMethod::load - called on non EncryptionMethod node");

	}

	// Find the type
	mp_algorithmAttr = 
		mp_encryptionMethodElement->getAttributeNodeNS(NULL, 
			DSIGConstants::s_unicodeStrAlgorithm);

	// Check for known children
	DOMElement * c = findFirstElementChild(mp_encryptionMethodElement);

	while (c != NULL) {

		if (strEquals(getDSIGLocalName(c), s_DigestMethod)) {

			mp_digestAlgorithmAttr = c->getAttributeNodeNS(NULL, 
				DSIGConstants::s_unicodeStrAlgorithm);

			if (mp_digestAlgorithmAttr == NULL) {
				throw XSECException(XSECException::EncryptionMethodError,
					"XENCEncryptionMethod::load - Cannot find Algorithm Attribute in DigestMethod element");
			}
		}

		else if (strEquals(getXENCLocalName(c), s_OAEPparams)) {

			mp_oaepParamsTextNode = NULL;
			mp_oaepParamsTextNode = findFirstChildOfType(c, DOMNode::TEXT_NODE);

			if (mp_oaepParamsTextNode == NULL) {
				throw XSECException(XSECException::EncryptionMethodError,
					"XENCEncryptionMethod::load - Cannot find text value of OAEPparams node");
			}

		}

		else if (strEquals(getXENCLocalName(c), s_KeySize)) {

			mp_keySizeTextNode = NULL;
			mp_keySizeTextNode = findFirstChildOfType(c, DOMNode::TEXT_NODE);

			if (mp_keySizeTextNode == NULL) {
				throw XSECException(XSECException::EncryptionMethodError,
					"XENCEncryptionMethod::load - Cannot find text value of KeySize node");
			}

		}
		c = findNextElementChild(c);
	}
}

// --------------------------------------------------------------------------------
//			Create from scratch
// --------------------------------------------------------------------------------

DOMElement * XENCEncryptionMethodImpl::createBlankEncryptedMethod(const XMLCh * algorithm) {

	// Get some setup values
	safeBuffer str;
	DOMDocument *doc = mp_env->getParentDocument();
	const XMLCh * prefix = mp_env->getXENCNSPrefix();

	makeQName(str, prefix, s_EncryptionMethod);

	mp_encryptionMethodElement = doc->createElementNS(DSIGConstants::s_unicodeStrURIXENC, str.rawXMLChBuffer());

	// Set the algorithm attribute

	if (algorithm != NULL) {
		mp_encryptionMethodElement->setAttributeNS(NULL,
							DSIGConstants::s_unicodeStrAlgorithm,
							algorithm);
		mp_algorithmAttr = 
			mp_encryptionMethodElement->getAttributeNodeNS(NULL, 
														   DSIGConstants::s_unicodeStrAlgorithm);

	}

	return mp_encryptionMethodElement;

}
// --------------------------------------------------------------------------------
//			Getter functions
// --------------------------------------------------------------------------------

const XMLCh * XENCEncryptionMethodImpl::getDigestMethod(void) const {

	if (mp_digestAlgorithmAttr != NULL)
		return mp_digestAlgorithmAttr->getNodeValue();

	return NULL;

}

const XMLCh * XENCEncryptionMethodImpl::getOAEPparams(void) const {

	if (mp_oaepParamsTextNode != NULL) {
		return mp_oaepParamsTextNode->getNodeValue();
	}

	return NULL;

}

const XMLCh * XENCEncryptionMethodImpl::getAlgorithm(void) const {

	if (mp_algorithmAttr != NULL) {
		return mp_algorithmAttr->getNodeValue();
	}

	return NULL;

}

int XENCEncryptionMethodImpl::getKeySize(void) const {

	if(mp_keySizeTextNode != NULL) {

		const XMLCh * keyVal = mp_keySizeTextNode->getNodeValue();
		unsigned int res = 0;
		if (!XMLString::textToBin(keyVal, res)) {
			throw XSECException(XSECException::EncryptionMethodError,
				"XENCEncryptionMethod::getKeySize - Cannot convert KeySize to integer");
		}

		return (int) res;

	}

	return 0;

}

// --------------------------------------------------------------------------------
//			Setter functions
// --------------------------------------------------------------------------------

void XENCEncryptionMethodImpl::setDigestMethod(const XMLCh * method) {

	if (mp_digestAlgorithmAttr == NULL) {

		// Need to create
		if (mp_oaepParamsTextNode == NULL) {
			mp_env->doPrettyPrint(mp_encryptionMethodElement);
		}

		// Get some setup values
		safeBuffer str;
		DOMDocument *doc = mp_env->getParentDocument();
		const XMLCh * prefix = mp_env->getDSIGNSPrefix();

		makeQName(str, prefix, s_DigestMethod);

		DOMElement *e = doc->createElementNS(DSIGConstants::s_unicodeStrURIDSIG, str.rawXMLChBuffer());
		if (mp_oaepParamsTextNode != NULL) {
			mp_encryptionMethodElement->insertBefore(e, mp_oaepParamsTextNode->getParentNode());
			if (mp_env->getPrettyPrintFlag())
				mp_encryptionMethodElement->insertBefore(doc->createTextNode(DSIGConstants::s_unicodeStrNL), mp_oaepParamsTextNode->getParentNode());
		}
		else {
			mp_encryptionMethodElement->appendChild(e);
			mp_env->doPrettyPrint(mp_encryptionMethodElement);
		}

		e->setAttributeNS(NULL,
					DSIGConstants::s_unicodeStrAlgorithm,
					method);

		// Set namespace

		if (prefix[0] == XERCES_CPP_NAMESPACE_QUALIFIER chNull) {
			str.sbTranscodeIn("xmlns");
		}
		else {
			str.sbTranscodeIn("xmlns:");
			str.sbXMLChCat(prefix);
		}

		e->setAttributeNS(DSIGConstants::s_unicodeStrURIXMLNS, 
								str.rawXMLChBuffer(), 
								DSIGConstants::s_unicodeStrURIDSIG);

		// Now retrieve for later use
		mp_digestAlgorithmAttr =
			e->getAttributeNodeNS(NULL, 
				DSIGConstants::s_unicodeStrAlgorithm);

		if (mp_digestAlgorithmAttr == NULL) {

			throw XSECException(XSECException::EncryptionMethodError,
				"XENCEncryptionMethod::setDigestMethod - Error creating Algorithm Attribute");
		}
	} 
	
	else {

		mp_digestAlgorithmAttr->setNodeValue(method);

	}

}

void XENCEncryptionMethodImpl::setOAEPparams(const XMLCh * params) {

	if (mp_oaepParamsTextNode == NULL) {

		// Need to create
		if (mp_digestAlgorithmAttr == NULL) {
			mp_env->doPrettyPrint(mp_encryptionMethodElement);
		}

		// Get some setup values
		safeBuffer str;
		DOMDocument *doc = mp_env->getParentDocument();
		const XMLCh * prefix = mp_env->getXENCNSPrefix();

		makeQName(str, prefix, s_OAEPparams);

		DOMElement *e = doc->createElementNS(DSIGConstants::s_unicodeStrURIXENC, str.rawXMLChBuffer());
		mp_encryptionMethodElement->appendChild(e);
		mp_env->doPrettyPrint(mp_encryptionMethodElement);

		mp_oaepParamsTextNode = doc->createTextNode(params);
		e->appendChild(mp_oaepParamsTextNode);

	} 
	
	else {

		mp_oaepParamsTextNode->setNodeValue(params);

	}

}


void XENCEncryptionMethodImpl::setKeySize(int size) {

	// First map the int to an XMLCh string
	XMLCh sizeXMLCh[10];
	XMLString::binToText((unsigned int) size, sizeXMLCh, 9, 10);

	if (mp_keySizeTextNode == NULL) {

		// Get some setup values
		safeBuffer str;
		DOMDocument *doc = mp_env->getParentDocument();
		const XMLCh * prefix = mp_env->getXENCNSPrefix();

		makeQName(str, prefix, s_KeySize);

		DOMElement *e = doc->createElementNS(DSIGConstants::s_unicodeStrURIXENC, str.rawXMLChBuffer());
		if (mp_oaepParamsTextNode != NULL) {
			// Need to insert before
			DOMNode * oaepNode = mp_oaepParamsTextNode->getParentNode();
			mp_encryptionMethodElement->insertBefore(e, oaepNode);
			if (mp_env->getPrettyPrintFlag()) {
				mp_encryptionMethodElement->insertBefore(doc->createTextNode(DSIGConstants::s_unicodeStrNL),
														 oaepNode);
			}
		}
		else {
			mp_env->doPrettyPrint(mp_encryptionMethodElement);
			mp_encryptionMethodElement->appendChild(e);
			mp_env->doPrettyPrint(mp_encryptionMethodElement);
		}

		e->appendChild(doc->createTextNode(sizeXMLCh));
	} 
	
	else {

		mp_keySizeTextNode->setNodeValue(sizeXMLCh);

	}

}
