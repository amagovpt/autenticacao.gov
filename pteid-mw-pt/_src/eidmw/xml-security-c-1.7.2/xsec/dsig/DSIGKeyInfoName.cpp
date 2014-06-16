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
 * DSIGKeyName := Simply a string that names a key for an application to read
 *
 * Author(s): Berin Lautenbach
 *
 * $Id: DSIGKeyInfoName.cpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#include <xsec/dsig/DSIGKeyInfoName.hpp>
#include <xsec/framework/XSECError.hpp>
#include <xsec/utils/XSECPlatformUtils.hpp>
#include <xsec/utils/XSECDOMUtils.hpp>
#include <xsec/dsig/DSIGSignature.hpp>
#include <xsec/framework/XSECEnv.hpp>

XERCES_CPP_NAMESPACE_USE

// --------------------------------------------------------------------------------
//           Constructors and Destructors
// --------------------------------------------------------------------------------


DSIGKeyInfoName::DSIGKeyInfoName(const XSECEnv * env, DOMNode *nameNode) : 
DSIGKeyInfo(env),
mp_name(NULL),
mp_decodedDName(NULL),
mp_keyNameTextNode(0) {

	mp_keyInfoDOMNode = nameNode;

}


DSIGKeyInfoName::DSIGKeyInfoName(const XSECEnv * env) : 
DSIGKeyInfo(env),
mp_name(NULL),
mp_decodedDName(NULL),
mp_keyNameTextNode(0) {

	mp_keyInfoDOMNode = 0;

}


DSIGKeyInfoName::~DSIGKeyInfoName() {

	if (mp_decodedDName != NULL)
		XSEC_RELEASE_XMLCH(mp_decodedDName);

};

// --------------------------------------------------------------------------------
//           Load and Get functions
// --------------------------------------------------------------------------------


void DSIGKeyInfoName::load(void) {

	// Assuming we have a valid DOM_Node to start with, load the signing key so that it can
	// be used later on

	if (mp_keyInfoDOMNode == NULL) {

		// Attempt to load an empty signature element
		throw XSECException(XSECException::LoadEmptyInfoName);

	}

	if (!strEquals(getDSIGLocalName(mp_keyInfoDOMNode), "KeyName")) {

		throw XSECException(XSECException::LoadNonInfoName);

	}

	// Now find the text node containing the name

	DOMNode *tmpElt = mp_keyInfoDOMNode->getFirstChild();

	while (tmpElt != 0 && tmpElt->getNodeType() != DOMNode::TEXT_NODE)
		tmpElt = tmpElt->getNextSibling();

	if (tmpElt != 0) {

		mp_keyNameTextNode = tmpElt;
		mp_name = tmpElt->getNodeValue();

	}

	else {

		throw XSECException(XSECException::ExpectedDSIGChildNotFound,
			MAKE_UNICODE_STRING("Expected TEXT node as child to <KeyName> element"));

	}

}

const XMLCh * DSIGKeyInfoName::getDecodedKeyName(void) {

	if (mp_decodedDName == NULL) {

		mp_decodedDName = decodeDName(mp_name);

	}

	return mp_decodedDName;

}

// --------------------------------------------------------------------------------
//           Create and Set functions
// --------------------------------------------------------------------------------

DOMElement * DSIGKeyInfoName::createBlankKeyName(const XMLCh * name, bool isDName) {

	// Create the DOM Structure

	safeBuffer str;
	DOMDocument *doc = mp_env->getParentDocument();
	const XMLCh * prefix = mp_env->getDSIGNSPrefix();

	makeQName(str, prefix, "KeyName");

	DOMElement *ret = doc->createElementNS(DSIGConstants::s_unicodeStrURIDSIG, str.rawXMLChBuffer());
	mp_keyInfoDOMNode = ret;

	// Check whether to encode prior to adding
	if (isDName == true) {

		// Treat as a distinguished name

		mp_decodedDName = XMLString::replicate(name);
		XMLCh * encodedName = encodeDName(name);
		mp_keyNameTextNode = doc->createTextNode(encodedName);
		XSEC_RELEASE_XMLCH(encodedName);
	}

	else
		mp_keyNameTextNode = doc->createTextNode(name);

	ret->appendChild(mp_keyNameTextNode);

	mp_name = mp_keyNameTextNode->getNodeValue();

	return ret;

}

void DSIGKeyInfoName::setKeyName(const XMLCh * name, bool isDName) {

	if (mp_keyNameTextNode == 0) {

		// Attempt to load an empty element
		throw XSECException(XSECException::LoadEmptySignature,
			MAKE_UNICODE_STRING("KeyInfoName::set() called prior to load() or createBlank()"));

	}

	if (mp_decodedDName != NULL) {

		XSEC_RELEASE_XMLCH(mp_decodedDName);
		mp_decodedDName = NULL;

	}

	if (isDName == true) {

		// This name should be treated as a Distinguished Name - so do the
		// required encoding

		mp_decodedDName = XMLString::replicate(name);
		XMLCh * encodedName = encodeDName(name);
		mp_keyNameTextNode->setNodeValue(encodedName);
		XSEC_RELEASE_XMLCH(encodedName);
	}

	else {

		mp_keyNameTextNode->setNodeValue(name);

	}

	mp_name = mp_keyNameTextNode->getNodeValue();

}
