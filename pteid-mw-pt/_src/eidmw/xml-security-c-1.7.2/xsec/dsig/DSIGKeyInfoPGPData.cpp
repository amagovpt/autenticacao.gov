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
 * $Id: DSIGKeyInfoPGPData.cpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#include <xsec/dsig/DSIGKeyInfoPGPData.hpp>
#include <xsec/framework/XSECError.hpp>
#include <xsec/utils/XSECPlatformUtils.hpp>
#include <xsec/utils/XSECDOMUtils.hpp>
#include <xsec/dsig/DSIGSignature.hpp>
#include <xsec/framework/XSECEnv.hpp>

#include <xercesc/util/Janitor.hpp>

XERCES_CPP_NAMESPACE_USE

// --------------------------------------------------------------------------------
//           Constructors and Destructors
// --------------------------------------------------------------------------------


DSIGKeyInfoPGPData::DSIGKeyInfoPGPData(const XSECEnv * env, DOMNode *pgpDataNode) : 
DSIGKeyInfo(env),
mp_keyID(NULL),
mp_keyPacket(NULL),
mp_keyIDTextNode(NULL),
mp_keyPacketTextNode(NULL) {

	mp_keyInfoDOMNode = pgpDataNode;

}


DSIGKeyInfoPGPData::DSIGKeyInfoPGPData(const XSECEnv * env) : 
DSIGKeyInfo(env),
mp_keyID(NULL),
mp_keyPacket(NULL),
mp_keyIDTextNode(NULL),
mp_keyPacketTextNode(NULL) {

	mp_keyInfoDOMNode = 0;

}


DSIGKeyInfoPGPData::~DSIGKeyInfoPGPData() {


};

// --------------------------------------------------------------------------------
//           Load and Get functions
// --------------------------------------------------------------------------------


void DSIGKeyInfoPGPData::load(void) {

	// Assuming we have a valid DOM_Node to start with, load the signing key so that it can
	// be used later on

	if (mp_keyInfoDOMNode == NULL) {

		// Attempt to load an empty signature element
		throw XSECException(XSECException::LoadEmptyInfoName);

	}

	if (!strEquals(getDSIGLocalName(mp_keyInfoDOMNode), "PGPData")) {

		throw XSECException(XSECException::ExpectedDSIGChildNotFound,
			"Expected a PGPData node");

	}

	// Now find the first element node containing either KeyID or KeyPacket

	DOMNode * tmpElt = findFirstChildOfType(mp_keyInfoDOMNode, DOMNode::ELEMENT_NODE);

	if (tmpElt == 0) {

		throw XSECException(XSECException::ExpectedDSIGChildNotFound,
			"Expected <PGPKeyID> or <PGPKeyPacket> children of PGPData node");

	}

	if (strEquals(getDSIGLocalName(tmpElt), "PGPKeyID")) {

		// Find the text node
		mp_keyIDTextNode = findFirstChildOfType(tmpElt, DOMNode::TEXT_NODE);

		if (mp_keyIDTextNode == NULL) {

			throw XSECException(XSECException::ExpectedDSIGChildNotFound,
				"Expected a text node beneath PGPKeyID");

		}

		mp_keyID = mp_keyIDTextNode->getNodeValue();

		do {

			tmpElt = tmpElt->getNextSibling();

		} while (tmpElt != NULL && tmpElt->getNodeType() != DOMNode::ELEMENT_NODE);

	}

	if (tmpElt != NULL && strEquals(getDSIGLocalName(tmpElt), "PGPKeyPacket")) {

		// Find the text node
		mp_keyPacketTextNode = findFirstChildOfType(tmpElt, DOMNode::TEXT_NODE);

		if (mp_keyPacketTextNode == NULL) {

			throw XSECException(XSECException::ExpectedDSIGChildNotFound,
				"Expected a text node beneath PGPKeyPacket");

		}

		mp_keyPacket = mp_keyPacketTextNode->getNodeValue();

	}

	if (mp_keyPacketTextNode == NULL && mp_keyIDTextNode == NULL) {

		throw XSECException(XSECException::ExpectedDSIGChildNotFound,
			"Expected <PGPKeyID> or <PGPKeyPacket> children of PGPData node");
	
	}
}

// --------------------------------------------------------------------------------
//           Create and Set functions
// --------------------------------------------------------------------------------

DOMElement * DSIGKeyInfoPGPData::createBlankPGPData(const XMLCh * id, const XMLCh * packet) {

	// Create the DOM Structure

	safeBuffer str;
	DOMDocument *doc = mp_env->getParentDocument();
	const XMLCh * prefix = mp_env->getDSIGNSPrefix();

	makeQName(str, prefix, "PGPData");

	DOMElement *ret = doc->createElementNS(DSIGConstants::s_unicodeStrURIDSIG, str.rawXMLChBuffer());
	mp_keyInfoDOMNode = ret;
	mp_env->doPrettyPrint(ret);

	if (id != NULL) {

		makeQName(str, prefix, "PGPKeyID");
		DOMElement * t = doc->createElementNS(DSIGConstants::s_unicodeStrURIDSIG, str.rawXMLChBuffer());
		ret->appendChild(t);
		mp_env->doPrettyPrint(ret);
		mp_keyIDTextNode = doc->createTextNode(id);
		t->appendChild(mp_keyIDTextNode);
		mp_keyID = mp_keyIDTextNode->getNodeValue();

	}

	if (packet != NULL) {

		makeQName(str, prefix, "PGPKeyPacket");
		DOMElement * t = doc->createElementNS(DSIGConstants::s_unicodeStrURIDSIG, str.rawXMLChBuffer());
		ret->appendChild(t);
		mp_env->doPrettyPrint(ret);
		mp_keyPacketTextNode = doc->createTextNode(packet);
		t->appendChild(mp_keyPacketTextNode);
		mp_keyPacket = mp_keyPacketTextNode->getNodeValue();

	}
	
	return ret;

}

void DSIGKeyInfoPGPData::setKeyID(const XMLCh * id) {

	if (mp_keyInfoDOMNode == NULL) {

		throw XSECException(XSECException::KeyInfoError,
			"DSIGKeyInfoPGPData::setKeyID() called prior to load or createBlank");
	}

	if (mp_keyIDTextNode == 0) {

		// Need to create
		safeBuffer str;
		DOMDocument *doc = mp_env->getParentDocument();
		const XMLCh * prefix = mp_env->getDSIGNSPrefix();

		makeQName(str, prefix, "PGPKeyID");
		DOMElement * t = doc->createElementNS(DSIGConstants::s_unicodeStrURIDSIG, str.rawXMLChBuffer());
		DOMNode * pkt = findFirstChildOfType(mp_keyInfoDOMNode, DOMNode::ELEMENT_NODE);
		if (pkt != NULL) {
			mp_keyInfoDOMNode->insertBefore(t, pkt);
			if (mp_env->getPrettyPrintFlag() == true)
				mp_keyInfoDOMNode->insertBefore(doc->createTextNode(DSIGConstants::s_unicodeStrNL), pkt);
		}
		else {
			mp_keyInfoDOMNode->appendChild(t);
			mp_env->doPrettyPrint(mp_keyInfoDOMNode);
		}
		mp_keyIDTextNode = doc->createTextNode(id);
		t->appendChild(mp_keyIDTextNode);

	}

	else 
		mp_keyIDTextNode->setNodeValue(id);

	mp_keyID = mp_keyIDTextNode->getNodeValue();

}

void DSIGKeyInfoPGPData::setKeyPacket(const XMLCh * packet) {

	if (mp_keyInfoDOMNode == NULL) {

		throw XSECException(XSECException::KeyInfoError,
			"DSIGKeyInfoPGPData::setKeyID() called prior to load or createBlank");
	}

	if (mp_keyPacketTextNode == 0) {

		// Need to create

		safeBuffer str;
		DOMDocument *doc = mp_env->getParentDocument();
		const XMLCh * prefix = mp_env->getDSIGNSPrefix();

		makeQName(str, prefix, "PGPKeyPacket");
		DOMElement * t = doc->createElementNS(DSIGConstants::s_unicodeStrURIDSIG, str.rawXMLChBuffer());
		mp_keyInfoDOMNode->appendChild(t);
		mp_env->doPrettyPrint(mp_keyInfoDOMNode);
		mp_keyPacketTextNode = doc->createTextNode(packet);
		t->appendChild(mp_keyPacketTextNode);

	}

	else 
		mp_keyPacketTextNode->setNodeValue(packet);

	mp_keyPacket = mp_keyPacketTextNode->getNodeValue();

}

