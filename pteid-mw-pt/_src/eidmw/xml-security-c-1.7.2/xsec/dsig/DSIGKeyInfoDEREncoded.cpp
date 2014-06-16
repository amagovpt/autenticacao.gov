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
 * DSIGKeyInfoDEREncoded := DER-encoded key information
 *
 * Author(s): Scott Cantor
 *
 * $Id:$
 *
 */

#include <xsec/dsig/DSIGKeyInfoDEREncoded.hpp>
#include <xsec/framework/XSECError.hpp>
#include <xsec/utils/XSECDOMUtils.hpp>
#include <xsec/dsig/DSIGSignature.hpp>
#include <xsec/framework/XSECEnv.hpp>

#include <xercesc/util/Janitor.hpp>

XERCES_CPP_NAMESPACE_USE

// --------------------------------------------------------------------------------
//           Constructors and Destructors
// --------------------------------------------------------------------------------


DSIGKeyInfoDEREncoded::DSIGKeyInfoDEREncoded(const XSECEnv * env, DOMNode *nameNode) : 
DSIGKeyInfo(env),
mp_data(NULL),
mp_dataTextNode(0) {

	mp_keyInfoDOMNode = nameNode;

}


DSIGKeyInfoDEREncoded::DSIGKeyInfoDEREncoded(const XSECEnv * env) : 
DSIGKeyInfo(env),
mp_data(NULL),
mp_dataTextNode(0) {

	mp_keyInfoDOMNode = 0;

}


DSIGKeyInfoDEREncoded::~DSIGKeyInfoDEREncoded() {


};

// --------------------------------------------------------------------------------
//           Load and Get functions
// --------------------------------------------------------------------------------


void DSIGKeyInfoDEREncoded::load(void) {

	// Assuming we have a valid DOM_Node to start with, load the signing key so that it can
	// be used later on

	if (mp_keyInfoDOMNode == NULL) {

		// Attempt to load an empty element
		throw XSECException(XSECException::KeyInfoError,
			"DSIGKeyInfoDEREncoded::load - called on empty DOM");

	}

	if (!strEquals(getDSIG11LocalName(mp_keyInfoDOMNode), "DEREncodedKeyValue")) {

		throw XSECException(XSECException::KeyInfoError,
			"DSIGKeyInfoDEREncoded::load - called on non <ds11::DEREncodedKeyValue> node");

	}

	// Now find the text node containing the name

	DOMNode *tmpElt = findFirstChildOfType(mp_keyInfoDOMNode, DOMNode::TEXT_NODE);

	if (tmpElt != 0) {

		mp_dataTextNode = tmpElt;
		mp_data = tmpElt->getNodeValue();

	}

	else {

		throw XSECException(XSECException::ExpectedDSIGChildNotFound,
			"DSIGKeyInfoDEREncoded::load - Expected TEXT node as child to <ds11::DEREncodedKeyValue> element");

	}

}

// --------------------------------------------------------------------------------
//           Create and Set functions
// --------------------------------------------------------------------------------

DOMElement * DSIGKeyInfoDEREncoded::createBlankDEREncoded(const XMLCh * data) {

	// Create the DOM Structure

	safeBuffer str;
	DOMDocument *doc = mp_env->getParentDocument();
	const XMLCh * prefix = mp_env->getDSIG11NSPrefix();

	makeQName(str, prefix, "DEREncodedKeyValue");

	DOMElement *ret = doc->createElementNS(DSIGConstants::s_unicodeStrURIDSIG11, str.rawXMLChBuffer());
	mp_keyInfoDOMNode = ret;

	// Check whether to encode prior to adding
	mp_dataTextNode = doc->createTextNode(data);

	ret->appendChild(mp_dataTextNode);

	mp_data = mp_dataTextNode->getNodeValue();

	return ret;

}

void DSIGKeyInfoDEREncoded::setData(const XMLCh * data) {

	if (mp_dataTextNode == 0) {

		// Attempt to set an empty element
		throw XSECException(XSECException::KeyInfoError,
			"DSIGKeyInfoDEREncoded::setData() called prior to load() or createBlank()");

	}

	mp_dataTextNode->setNodeValue(data);
	mp_data = mp_dataTextNode->getNodeValue();

}

