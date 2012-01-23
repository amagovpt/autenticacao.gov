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
 * DSIGKeyInfoMgmtData := Inband key information
 *
 * Author(s): Berin Lautenbach
 *
 * $Id: DSIGKeyInfoMgmtData.cpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#include <xsec/dsig/DSIGKeyInfoMgmtData.hpp>
#include <xsec/framework/XSECError.hpp>
#include <xsec/utils/XSECDOMUtils.hpp>
#include <xsec/dsig/DSIGSignature.hpp>
#include <xsec/framework/XSECEnv.hpp>

#include <xercesc/util/Janitor.hpp>

XERCES_CPP_NAMESPACE_USE

// --------------------------------------------------------------------------------
//           Constructors and Destructors
// --------------------------------------------------------------------------------


DSIGKeyInfoMgmtData::DSIGKeyInfoMgmtData(const XSECEnv * env, DOMNode *nameNode) : 
DSIGKeyInfo(env),
mp_data(NULL),
mp_dataTextNode(0) {

	mp_keyInfoDOMNode = nameNode;

}


DSIGKeyInfoMgmtData::DSIGKeyInfoMgmtData(const XSECEnv * env) : 
DSIGKeyInfo(env),
mp_data(NULL),
mp_dataTextNode(0) {

	mp_keyInfoDOMNode = 0;

}


DSIGKeyInfoMgmtData::~DSIGKeyInfoMgmtData() {


};

// --------------------------------------------------------------------------------
//           Load and Get functions
// --------------------------------------------------------------------------------


void DSIGKeyInfoMgmtData::load(void) {

	// Assuming we have a valid DOM_Node to start with, load the signing key so that it can
	// be used later on

	if (mp_keyInfoDOMNode == NULL) {

		// Attempt to load an empty signature element
		throw XSECException(XSECException::KeyInfoError,
			"DSIGKeyInfoMgmtData::load - called on empty DOM");

	}

	if (!strEquals(getDSIGLocalName(mp_keyInfoDOMNode), "MgmtData")) {

		throw XSECException(XSECException::KeyInfoError,
			"DSIGKeyInfoMgmtData::load - called on non <MgmtData> node");

	}

	// Now find the text node containing the name

	DOMNode *tmpElt = findFirstChildOfType(mp_keyInfoDOMNode, DOMNode::TEXT_NODE);

	if (tmpElt != 0) {

		mp_dataTextNode = tmpElt;
		mp_data = tmpElt->getNodeValue();

	}

	else {

		throw XSECException(XSECException::ExpectedDSIGChildNotFound,
			"DSIGKeyInfoMgmtData::load - Expected TEXT node as child to <MgmtData> element");

	}

}

// --------------------------------------------------------------------------------
//           Create and Set functions
// --------------------------------------------------------------------------------

DOMElement * DSIGKeyInfoMgmtData::createBlankMgmtData(const XMLCh * data) {

	// Create the DOM Structure

	safeBuffer str;
	DOMDocument *doc = mp_env->getParentDocument();
	const XMLCh * prefix = mp_env->getDSIGNSPrefix();

	makeQName(str, prefix, "MgmtData");

	DOMElement *ret = doc->createElementNS(DSIGConstants::s_unicodeStrURIDSIG, str.rawXMLChBuffer());
	mp_keyInfoDOMNode = ret;

	// Check whether to encode prior to adding
	mp_dataTextNode = doc->createTextNode(data);

	ret->appendChild(mp_dataTextNode);

	mp_data = mp_dataTextNode->getNodeValue();

	return ret;

}

void DSIGKeyInfoMgmtData::setData(const XMLCh * data) {

	if (mp_dataTextNode == 0) {

		// Attempt to set an empty element
		throw XSECException(XSECException::KeyInfoError,
			"KeyInfoMgmtData::setData() called prior to load() or createBlank()");

	}

	mp_dataTextNode->setNodeValue(data);
	mp_data = mp_dataTextNode->getNodeValue();

}

