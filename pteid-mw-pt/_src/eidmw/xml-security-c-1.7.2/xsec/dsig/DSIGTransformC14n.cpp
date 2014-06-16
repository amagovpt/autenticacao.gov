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
 * DSIGTransformC14n := Class that performs C14n canonicalisation
 *
 * $Id: DSIGTransformC14n.cpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#include <xsec/dsig/DSIGTransformC14n.hpp>
#include <xsec/framework/XSECException.hpp>
#include <xsec/transformers/TXFMC14n.hpp>
#include <xsec/transformers/TXFMChain.hpp>
#include <xsec/framework/XSECError.hpp>
#include <xsec/framework/XSECEnv.hpp>
#include <xsec/dsig/DSIGSignature.hpp>

#include <xercesc/util/Janitor.hpp>

XERCES_CPP_NAMESPACE_USE

// --------------------------------------------------------------------------------
//           Constructors and Destructors
// --------------------------------------------------------------------------------

DSIGTransformC14n::DSIGTransformC14n(const XSECEnv * env, DOMNode * node) :
DSIGTransform(env, node) {

	m_cMethod = CANON_NONE;
	mp_inclNSNode = NULL;
	mp_inclNSStr = NULL;
}


DSIGTransformC14n::DSIGTransformC14n(const XSECEnv * env) :
DSIGTransform(env) {

	m_cMethod = CANON_NONE;
	mp_inclNSNode = NULL;
	mp_inclNSStr = NULL;

}

DSIGTransformC14n::~DSIGTransformC14n() {};

// --------------------------------------------------------------------------------
//           Interface Methods
// --------------------------------------------------------------------------------

transformType DSIGTransformC14n::getTransformType() {

	if ((m_cMethod == CANON_C14NE_NOC) || (m_cMethod == CANON_C14NE_COM))
		return TRANSFORM_EXC_C14N;
	else if ((m_cMethod == CANON_C14N11_NOC) || (m_cMethod == CANON_C14N11_COM))
        return TRANSFORM_C14N11;
	return TRANSFORM_C14N;

}

void DSIGTransformC14n::appendTransformer(TXFMChain * input) {

	TXFMC14n * c;

	XSECnew(c, TXFMC14n(mp_txfmNode->getOwnerDocument()));
	input->appendTxfm(c);

	switch (m_cMethod) {

	case (CANON_C14N_NOC) :
	case (CANON_C14N11_NOC) :
	case (CANON_C14NE_NOC) :
		c->stripComments();
		break;
	case (CANON_C14N_COM) :
    case (CANON_C14N11_COM) :
	case (CANON_C14NE_COM) :
		c->activateComments();
		break;
	default:
		break;
	}

	// Check for exclusive and 1.1
	if (m_cMethod == CANON_C14NE_COM || m_cMethod == CANON_C14NE_NOC) {

		if (mp_inclNSStr == NULL) {

			c->setExclusive();

		}
		else {

			safeBuffer incl;
			incl << (*(mp_env->getSBFormatter()) << mp_inclNSStr);
			c->setExclusive(incl);

		}

	}
	else if (m_cMethod == CANON_C14N11_COM || m_cMethod == CANON_C14N11_NOC) {
	    c->setInclusive11();
	}

}

DOMElement * DSIGTransformC14n::createBlankTransform(DOMDocument * parentDoc) {

	safeBuffer str;
	const XMLCh * prefix;
	DOMElement *ret;
	DOMDocument *doc = mp_env->getParentDocument();

	prefix = mp_env->getDSIGNSPrefix();

	// Create the transform node
	makeQName(str, prefix, "Transform");
	ret = doc->createElementNS(DSIGConstants::s_unicodeStrURIDSIG, str.rawXMLChBuffer());
	ret->setAttributeNS(NULL,DSIGConstants::s_unicodeStrAlgorithm, DSIGConstants::s_unicodeStrURIC14N_NOC);

	mp_txfmNode = ret;
	mp_inclNSStr = NULL;
	mp_inclNSNode = NULL;

	return ret;

}

void DSIGTransformC14n::load(void) {

	const XMLCh * uri;
	DOMNamedNodeMap * atts;
	DOMNode *att;

	// Read the URI for the type
	if (mp_txfmNode == NULL) {

		throw XSECException(XSECException::ExpectedDSIGChildNotFound,
			"Expected <Transform> Node in DSIGTrasnformC14n::load");

	}

	atts = mp_txfmNode->getAttributes();

	if (atts == NULL ||
		((att = atts->getNamedItem(DSIGConstants::s_unicodeStrAlgorithm)) == NULL)) {

		throw XSECException(XSECException::ExpectedDSIGChildNotFound,
			"Expected to find Algorithm attribute in <Transform> node");

	}

	uri = att->getNodeValue();

	if (strEquals(uri, DSIGConstants::s_unicodeStrURIC14N_COM)) {
		m_cMethod = CANON_C14N_COM;
	}
	else if (strEquals(uri, DSIGConstants::s_unicodeStrURIC14N_NOC)) {
		m_cMethod = CANON_C14N_NOC;
	}
	else if (strEquals(uri, DSIGConstants::s_unicodeStrURIC14N11_COM)) {
        m_cMethod = CANON_C14N11_COM;
    }
    else if (strEquals(uri, DSIGConstants::s_unicodeStrURIC14N11_NOC)) {
        m_cMethod = CANON_C14N11_NOC;
    }
	else if (strEquals(uri, DSIGConstants::s_unicodeStrURIEXC_C14N_COM)) {
		m_cMethod = CANON_C14NE_COM;
	}
	else if (strEquals(uri, DSIGConstants::s_unicodeStrURIEXC_C14N_NOC)) {
		m_cMethod = CANON_C14NE_NOC;
	}
	else {
		throw XSECException(XSECException::ExpectedDSIGChildNotFound,
			"Unexpected URI found in canonicalisation <Transform>");
	}

	// Determine whether there is an InclusiveNamespaces list

	if (m_cMethod == CANON_C14NE_NOC || m_cMethod == CANON_C14NE_COM) {

		// Exclusive, so there may be an InclusiveNamespaces node

		DOMNode * inclNSNode = mp_txfmNode->getFirstChild();

		while (inclNSNode != NULL && (inclNSNode->getNodeType() != DOMNode::ELEMENT_NODE ||
			!strEquals(getECLocalName(inclNSNode), "InclusiveNamespaces")))
				inclNSNode = inclNSNode->getNextSibling();

		if (inclNSNode != 0) {

			mp_inclNSNode = static_cast<DOMElement *>(inclNSNode);

			// Have a prefix list
			atts = mp_inclNSNode->getAttributes();
			safeBuffer inSB;

			if (atts == 0 || ((att = atts->getNamedItem(MAKE_UNICODE_STRING("PrefixList"))) == NULL)) {
				throw XSECException(XSECException::ExpectedDSIGChildNotFound,
					"Expected PrefixList in InclusiveNamespaces");
			}

			mp_inclNSStr = att->getNodeValue();

		}
	}

}

// --------------------------------------------------------------------------------
//           Canonicalization Specific Methods
// --------------------------------------------------------------------------------


void DSIGTransformC14n::setCanonicalizationMethod(canonicalizationMethod method) {

	const XMLCh * m = canonicalizationMethod2UNICODEURI(method);

	if (strEquals(m, DSIGConstants::s_unicodeStrEmpty) || mp_txfmNode == NULL) {

		throw XSECException(XSECException::TransformError,
			"Either method unknown or Node not set in setCanonicalizationMethod");

	}

	if (method == CANON_C14N_NOC || method == CANON_C14N_COM || method == CANON_C14N11_NOC || method == CANON_C14N11_COM) {

		if (m_cMethod == CANON_C14NE_NOC || m_cMethod == CANON_C14NE_COM) {

			if (mp_inclNSNode != 0) {

				mp_txfmNode->removeChild(mp_inclNSNode);
				mp_inclNSNode->release();		// No longer required

				mp_inclNSNode = NULL;
				mp_inclNSStr = NULL;

			}
		}

	}

	// Now do the set.

	((DOMElement *) mp_txfmNode)->setAttributeNS(NULL,MAKE_UNICODE_STRING("Algorithm"), m);
	m_cMethod = method;

}

canonicalizationMethod DSIGTransformC14n::getCanonicalizationMethod(void) {

	return m_cMethod;

}

void DSIGTransformC14n::createInclusiveNamespaceNode(void) {

	// Creates an empty inclusiveNamespace node.  Does _not_ set the prefixlist attribute

	if (mp_inclNSNode != NULL)
		return;		// Already exists

	safeBuffer str;
	const XMLCh * prefix;
	DOMDocument *doc = mp_env->getParentDocument();

	// Use the Exclusive Canonicalisation prefix
	prefix = mp_env->getECNSPrefix();

	// Create the transform node
	makeQName(str, prefix, "InclusiveNamespaces");
	mp_inclNSNode = doc->createElementNS(DSIGConstants::s_unicodeStrURIEC, str.rawXMLChBuffer());

	// Add the node to the owner element
	mp_env->doPrettyPrint(mp_txfmNode);
	mp_txfmNode->appendChild(mp_inclNSNode);
	mp_env->doPrettyPrint(mp_txfmNode);

	// Set the namespace attribute
	if (prefix[0] == '\0') {
		str.sbTranscodeIn("xmlns");
	}
	else {
		str.sbTranscodeIn("xmlns:");
		str.sbXMLChCat(prefix);
	}

	mp_inclNSNode->setAttributeNS(DSIGConstants::s_unicodeStrURIXMLNS,
							str.rawXMLChBuffer(),
							DSIGConstants::s_unicodeStrURIEC);
}

void DSIGTransformC14n::setInclusiveNamespaces(const XMLCh * ns) {

	// Set all the namespaces at once

	if (m_cMethod != CANON_C14NE_COM && m_cMethod != CANON_C14NE_NOC) {

		throw XSECException(XSECException::TransformError,
			"Cannot set inclusive namespaces on non Exclusive Canonicalisation");

	}

	if (mp_inclNSNode == NULL) {

		// Create the transform node
		createInclusiveNamespaceNode();


	}

	// Now create the prefix list

	mp_inclNSNode->setAttributeNS(NULL,MAKE_UNICODE_STRING("PrefixList"), ns);
	mp_inclNSStr = mp_inclNSNode->getAttributes()->getNamedItem(MAKE_UNICODE_STRING("PrefixList"))->getNodeValue();

}


void DSIGTransformC14n::addInclusiveNamespace(const char * ns) {

	if (m_cMethod != CANON_C14NE_COM && m_cMethod != CANON_C14NE_NOC) {

		throw XSECException(XSECException::TransformError,
			"Cannot set inclusive namespaces on non Exclusive Canonicalisation");

	}

	if (mp_inclNSNode == NULL) {

		// Create the transform node
		createInclusiveNamespaceNode();

		// Now create the prefix list

		mp_inclNSNode->setAttributeNS(NULL,MAKE_UNICODE_STRING("PrefixList"), MAKE_UNICODE_STRING(ns));
		mp_inclNSStr = mp_inclNSNode->getAttributes()->getNamedItem(MAKE_UNICODE_STRING("PrefixList"))->getNodeValue();

	}

	else {

		// More tricky
		safeBuffer str;

		str << (*(mp_env->getSBFormatter()) << mp_inclNSStr);
		str.sbStrcatIn(" ");
		str.sbStrcatIn((char *) ns);
		mp_inclNSNode->setAttributeNS(NULL,MAKE_UNICODE_STRING("PrefixList"), str.sbStrToXMLCh());
		mp_inclNSStr = mp_inclNSNode->getAttributes()->getNamedItem(MAKE_UNICODE_STRING("PrefixList"))->getNodeValue();

	}

}

const XMLCh * DSIGTransformC14n::getPrefixList(void) {

	return mp_inclNSStr;

}

void DSIGTransformC14n::clearInclusiveNamespaces(void) {

	if (mp_inclNSNode != 0) {

		mp_txfmNode->removeChild(mp_inclNSNode);
		mp_inclNSNode->release();		// No longer required

		mp_inclNSNode = NULL;
		mp_inclNSStr = NULL;

	}
}
