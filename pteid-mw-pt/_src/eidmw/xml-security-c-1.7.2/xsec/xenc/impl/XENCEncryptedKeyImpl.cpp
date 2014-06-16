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
 * XENCEncryptedKeyImpl := Implementation for holder object for EncryptedKeys
 *
 * $Id: XENCEncryptedKeyImpl.cpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#include <xsec/framework/XSECDefs.hpp>

#include "XENCCipherImpl.hpp"
#include "XENCEncryptedKeyImpl.hpp"
#include "XENCCipherDataImpl.hpp"

#include <xsec/framework/XSECError.hpp>
#include <xsec/framework/XSECEnv.hpp>
#include <xsec/utils/XSECDOMUtils.hpp>

#include <xercesc/util/XMLUniDefs.hpp>

XERCES_CPP_NAMESPACE_USE

// --------------------------------------------------------------------------------
//			UNICODE Strings
// --------------------------------------------------------------------------------

static XMLCh s_EncryptedKey[] = {

	chLatin_E,
	chLatin_n,
	chLatin_c,
	chLatin_r,
	chLatin_y,
	chLatin_p,
	chLatin_t,
	chLatin_e,
	chLatin_d,
	chLatin_K,
	chLatin_e,
	chLatin_y,
	chNull
};

static XMLCh s_CarriedKeyName[] = {

	chLatin_C,
	chLatin_a,
	chLatin_r,
	chLatin_r,
	chLatin_i,
	chLatin_e,
	chLatin_d,
	chLatin_K,
	chLatin_e,
	chLatin_y,
	chLatin_N,
	chLatin_a,
	chLatin_m,
	chLatin_e,
	chNull
};

static XMLCh s_Recipient[] = {

	chLatin_R,
	chLatin_e,
	chLatin_c,
	chLatin_i,
	chLatin_p,
	chLatin_i,
	chLatin_e,
	chLatin_n,
	chLatin_t,
	chNull
};

// --------------------------------------------------------------------------------
//			Construct/Destruct
// --------------------------------------------------------------------------------


XENCEncryptedKeyImpl::XENCEncryptedKeyImpl(const XSECEnv * env) :
XENCEncryptedKey(env),
XENCEncryptedTypeImpl(env),
mp_carriedKeyNameTextNode(NULL),
mp_recipientAttr(NULL) {
	
}

XENCEncryptedKeyImpl::XENCEncryptedKeyImpl(const XSECEnv * env, DOMElement * node) :
XENCEncryptedKey(env),
XENCEncryptedTypeImpl(env, node),
mp_carriedKeyNameTextNode(NULL),
mp_recipientAttr(NULL) {

}

XENCEncryptedKeyImpl::~XENCEncryptedKeyImpl() {

}

// --------------------------------------------------------------------------------
//			Load
// --------------------------------------------------------------------------------

void XENCEncryptedKeyImpl::load(void) {

	if (mp_encryptedTypeElement == NULL) {

		// Attempt to load an empty encryptedData element
		throw XSECException(XSECException::EncryptedTypeError,
			"XENCEncryptedKey::load - called on empty DOM");

	}

	if (!strEquals(getXENCLocalName(mp_encryptedTypeElement), s_EncryptedKey)) {

		// Attempt to load an empty encryptedKey element
		throw XSECException(XSECException::EncryptedTypeError,
			"XENCEncryptedKey::load - called on non EncryptedData node");

	}

	// Now call the virtual function we overloaded to get here.
	XENCEncryptedTypeImpl::load();

	// Set up the keyInfo node
	mp_keyInfoDOMNode = mp_encryptedTypeElement;

	// Find the Recipient Attribute
	mp_recipientAttr = mp_encryptedTypeElement->getAttributeNodeNS(NULL, s_Recipient);

	// Now load specific EncryptedKey elements
	DOMNode * c = findFirstChildOfType(mp_encryptedTypeElement, DOMNode::ELEMENT_NODE);

	while (c != NULL) {

		if (strEquals(getXENCLocalName(c), s_CarriedKeyName)) {

			// Have a CarriedKeyName node
			mp_carriedKeyNameTextNode = findFirstChildOfType(c, DOMNode::TEXT_NODE);

		}

		c = findNextElementChild(c);

	}

}
// --------------------------------------------------------------------------------
//			Create from scratch
// --------------------------------------------------------------------------------

DOMElement * XENCEncryptedKeyImpl::createBlankEncryptedKey(
									XENCCipherData::XENCCipherDataType type, 
									const XMLCh * algorithm,
									const XMLCh * value) {

	DOMElement * ret = createBlankEncryptedType(s_EncryptedKey, type, algorithm, value);

	// Set up the KeyInfo information
	mp_keyInfoDOMNode = mp_encryptedTypeElement;

	return ret;

}

// --------------------------------------------------------------------------------
//			Interface Methods
// --------------------------------------------------------------------------------


const XMLCh * XENCEncryptedKeyImpl::getCarriedKeyName(void) const {

	if (mp_carriedKeyNameTextNode != NULL)
		return mp_carriedKeyNameTextNode->getNodeValue();

	return NULL;

}

const XMLCh * XENCEncryptedKeyImpl::getRecipient(void) const {

	if (mp_recipientAttr != NULL)
		return mp_recipientAttr->getNodeValue();

	return NULL;

}

void XENCEncryptedKeyImpl::setCarriedKeyName(const XMLCh * name) {

	if (mp_carriedKeyNameTextNode == NULL) {

		// Get some setup values
		safeBuffer str;
		DOMDocument *doc = XENCEncryptedTypeImpl::mp_env->getParentDocument();
		const XMLCh * prefix = XENCEncryptedTypeImpl::mp_env->getXENCNSPrefix();

		makeQName(str, prefix, s_CarriedKeyName);

		DOMElement *e = doc->createElementNS(DSIGConstants::s_unicodeStrURIXENC, str.rawXMLChBuffer());

		mp_encryptedTypeElement->appendChild(e);
		XENCEncryptedTypeImpl::mp_env->doPrettyPrint(mp_encryptedTypeElement);

		mp_carriedKeyNameTextNode = doc->createTextNode(name);
		e->appendChild(mp_carriedKeyNameTextNode);

	} 
	
	else {

		mp_carriedKeyNameTextNode->setNodeValue(name);

	}
}

void XENCEncryptedKeyImpl::setRecipient(const XMLCh * recipient) {

	if (mp_recipientAttr == NULL) {

		if (mp_encryptedTypeElement->getNodeType() != DOMNode::ELEMENT_NODE) {
			throw XSECException(XSECException::EncryptedTypeError,
				"XENCEncryptedKeyImpl::setRecipient - encryptedTypeNode is not an Element");
		}

		mp_encryptedTypeElement->setAttributeNS(NULL, 
											 s_Recipient,
											 recipient);
		// Now retrieve for later use
		mp_recipientAttr = mp_encryptedTypeElement->getAttributeNodeNS(NULL, s_Recipient);

		if (mp_recipientAttr == NULL) {

			throw XSECException(XSECException::EncryptionMethodError,
				"XENCEncryptionKey::setRecipient - Error creating Recipient Attribute");
		}
	} 
	
	else {

		mp_recipientAttr->setNodeValue(recipient);

	}

}

