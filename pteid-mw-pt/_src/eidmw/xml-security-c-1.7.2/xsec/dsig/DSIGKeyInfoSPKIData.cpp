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
 * DSIGKeyInfoSPKIData := SPKI Information
 *
 * Author(s): Berin Lautenbach
 *
 * $Id: DSIGKeyInfoSPKIData.cpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#include <xsec/dsig/DSIGKeyInfoSPKIData.hpp>
#include <xsec/framework/XSECError.hpp>
#include <xsec/dsig/DSIGSignature.hpp>
#include <xsec/framework/XSECEnv.hpp>

#include <xercesc/dom/DOM.hpp>

XERCES_CPP_NAMESPACE_USE

// --------------------------------------------------------------------------------
//           Constructors and Destructors
// --------------------------------------------------------------------------------

DSIGKeyInfoSPKIData::DSIGKeyInfoSPKIData(const XSECEnv * env, DOMNode *nameNode) : 
DSIGKeyInfo(env) {

	mp_keyInfoDOMNode = nameNode;

}

DSIGKeyInfoSPKIData::DSIGKeyInfoSPKIData(const XSECEnv * env) :
DSIGKeyInfo(env) {

	mp_keyInfoDOMNode = NULL;

}

DSIGKeyInfoSPKIData::~DSIGKeyInfoSPKIData() {

	sexpVectorType::iterator i;

	for (i = m_sexpList.begin(); i != m_sexpList.end(); ++i) {

		delete *i;

	}
}

// --------------------------------------------------------------------------------
//           Load and get
// --------------------------------------------------------------------------------

void DSIGKeyInfoSPKIData::load(void) {

	// Assuming we have a valid DOM_Node to start with, load the signing key so that it can
	// be used later on

	if (mp_keyInfoDOMNode == NULL) {

		// Attempt to load an empty signature element
		throw XSECException(XSECException::ExpectedDSIGChildNotFound,
			"DSIGKeyInfoSPKIData::load called without node being set");

	}

	if (!strEquals(getDSIGLocalName(mp_keyInfoDOMNode), "SPKIData")) {

		throw XSECException(XSECException::ExpectedDSIGChildNotFound,
			"Expected an <SPKIData> node");

	}

	// Now find the SPKISexp nodes
	DOMNode * tmpElt = findFirstChildOfType(mp_keyInfoDOMNode, DOMNode::ELEMENT_NODE);

	while (tmpElt != NULL && strEquals(getDSIGLocalName(tmpElt), "SPKISexp")) {

		DOMNode * txt = findFirstChildOfType(tmpElt, DOMNode::TEXT_NODE);

		if (txt == NULL) {

			throw XSECException(XSECException::ExpectedDSIGChildNotFound,
				"Expected text node child of <SPKISexp>");

		}

		SexpNode * s;
		XSECnew(s, SexpNode);

		m_sexpList.push_back(s);

		s->mp_expr = txt->getNodeValue();
		s->mp_exprTextNode = txt;

		// Find next SPKISexp
		
		do {

			tmpElt = tmpElt->getNextSibling();

		} while (tmpElt != NULL && tmpElt->getNodeType() != DOMNode::ELEMENT_NODE);

	}

	/*
	 * Note that it is not necessarily an error if non SPKISexp nodes are found
	 * after the SPKISexp nodes.
	 */

}

unsigned int DSIGKeyInfoSPKIData::getSexpSize(void) const {

	return (unsigned int) m_sexpList.size();

}

const XMLCh * DSIGKeyInfoSPKIData::getSexp(unsigned int index) const {

	if (index >= m_sexpList.size()) {

		throw XSECException(XSECException::KeyInfoError,
			"DSIGKeyInfoSPKIData::getSexp - index out of range");

	}

	return m_sexpList[index]->mp_expr;

}

// --------------------------------------------------------------------------------
//           Create and set
// --------------------------------------------------------------------------------

DOMElement * DSIGKeyInfoSPKIData::createBlankSPKIData(const XMLCh * Sexp) {

	// Create the DOM Structure

	safeBuffer str;
	DOMDocument *doc = mp_env->getParentDocument();
	const XMLCh * prefix = mp_env->getDSIGNSPrefix();

	makeQName(str, prefix, "SPKIData");

	DOMElement * ret = doc->createElementNS(DSIGConstants::s_unicodeStrURIDSIG, str.rawXMLChBuffer());
	mp_keyInfoDOMNode = ret;
	mp_env->doPrettyPrint(ret);

	appendSexp(Sexp);

	return ret;

}

void DSIGKeyInfoSPKIData::appendSexp(const XMLCh * Sexp) {

	// First create the new element in the list
	SexpNode *s;
	XSECnew(s, SexpNode);

	m_sexpList.push_back(s);

	safeBuffer str;
	DOMDocument *doc = mp_env->getParentDocument();
	const XMLCh * prefix = mp_env->getDSIGNSPrefix();

	makeQName(str, prefix, "SPKISexp");

	DOMNode *tmpElt = doc->createElementNS(DSIGConstants::s_unicodeStrURIDSIG, str.rawXMLChBuffer());
	
	s->mp_exprTextNode = doc->createTextNode(Sexp);
	s->mp_expr = s->mp_exprTextNode->getNodeValue();

	tmpElt->appendChild(s->mp_exprTextNode);

	mp_keyInfoDOMNode->appendChild(tmpElt);
	mp_env->doPrettyPrint(mp_keyInfoDOMNode);

}

