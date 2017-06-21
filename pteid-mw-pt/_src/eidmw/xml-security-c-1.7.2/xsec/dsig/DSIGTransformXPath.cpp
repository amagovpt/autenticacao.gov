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
 * DSIGTransformXPath := Class that holds XPath transforms
 *
 * $Id: DSIGTransformXPath.cpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */


#include <xsec/dsig/DSIGTransformXPath.hpp>
#include <xsec/transformers/TXFMXPath.hpp>
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

DSIGTransformXPath::DSIGTransformXPath(const XSECEnv * env, DOMNode * node) :
DSIGTransform(env, node) {

	mp_exprTextNode = NULL;
	mp_xpathNode = NULL;
	mp_NSMap = NULL;
	m_expr = "";

}
	

DSIGTransformXPath::DSIGTransformXPath(const XSECEnv * env) :
DSIGTransform(env) {

	mp_exprTextNode = NULL;
	mp_xpathNode = NULL;
	mp_NSMap = NULL;
	m_expr = "";
}
		  
DSIGTransformXPath::~DSIGTransformXPath() {};

// --------------------------------------------------------------------------------
//           Interface Methods
// --------------------------------------------------------------------------------
	
transformType DSIGTransformXPath::getTransformType() {

	return TRANSFORM_XPATH;

}

void DSIGTransformXPath::appendTransformer(TXFMChain * input) {

#ifdef XSEC_NO_XPATH

	throw XSECException(XSECException::UnsupportedFunction,
		"XPath transforms are not supported in this compilation of the XSEC library");

#else

	TXFMXPath *x;
	// XPath transform
	XSECnew(x, TXFMXPath(mp_txfmNode->getOwnerDocument()));
	input->appendTxfm(x);

	// These can throw, but the TXFMXPath is now owned by the chain, so will
	// be cleaned up down the calling stack.

	x->setNameSpace(mp_NSMap);
	x->evaluateExpr(mp_txfmNode, m_expr);
	
#endif /* NO_XPATH */

}

DOMElement * DSIGTransformXPath::createBlankTransform(DOMDocument * parentDoc) {

	safeBuffer str;
	const XMLCh * prefix;
	DOMElement *ret;
	DOMDocument *doc = mp_env->getParentDocument();

	prefix = mp_env->getDSIGNSPrefix();
	
	// Create the transform node
	makeQName(str, prefix, "Transform");
	ret = doc->createElementNS(DSIGConstants::s_unicodeStrURIDSIG, str.rawXMLChBuffer());
	ret->setAttributeNS(NULL,DSIGConstants::s_unicodeStrAlgorithm, DSIGConstants::s_unicodeStrURIXPATH);
	
	// Create the XPath element
	
	makeQName(str, prefix, "XPath");
	mp_xpathNode = doc->createElementNS(DSIGConstants::s_unicodeStrURIDSIG, str.rawXMLChBuffer());
	mp_exprTextNode = doc->createTextNode(MAKE_UNICODE_STRING(""));
	ret->appendChild(mp_xpathNode);
	mp_xpathNode->appendChild(mp_exprTextNode);

	mp_txfmNode = ret;

	return ret;

}

void DSIGTransformXPath::load(void) {

	// Find the XPath expression
	
	mp_xpathNode = mp_txfmNode->getFirstChild();

	while (mp_xpathNode != 0 && 
		mp_xpathNode->getNodeType() != DOMNode::ELEMENT_NODE && !strEquals(mp_xpathNode->getNodeName(), "XPath"))
		mp_xpathNode = mp_xpathNode->getNextSibling();
	
	if (mp_xpathNode == 0) {

		throw XSECException(XSECException::ExpectedDSIGChildNotFound,
			"Expected <XPath> Node in DSIGTransformXPath::load");
	}
	
	else {
		
		// Check for attributes - in particular any namespaces

		mp_NSMap = mp_xpathNode->getAttributes();
		
		// Find the text node
		mp_exprTextNode = findFirstChildOfType(mp_xpathNode, DOMNode::TEXT_NODE);

		if (mp_exprTextNode == NULL) {
			throw XSECException(XSECException::ExpectedDSIGChildNotFound,
				"Expected Text Node in beneath <XPath> in DSIGTransformXPath::load");
		}

		// Gather the text
		safeBuffer exprSB;

		gatherChildrenText(mp_xpathNode, exprSB);

		m_expr << (*(mp_env->getSBFormatter()) << exprSB.rawXMLChBuffer());

		//m_expr << (*(mp_env->getSBFormatter()) << mp_exprTextNode->getNodeValue());
				
	}

}


// --------------------------------------------------------------------------------
//           XPath Transform Specific Methods
// --------------------------------------------------------------------------------

void DSIGTransformXPath::setExpression(const char * expr) {

	mp_exprTextNode->setNodeValue(MAKE_UNICODE_STRING(expr));

	m_expr.sbStrcpyIn((char *) expr);

}



const char * DSIGTransformXPath::getExpression(void) {

	return m_expr.rawCharBuffer();

}


void DSIGTransformXPath::setNamespace(const char * prefix, const char * value) {

	safeBuffer str;

	str.sbStrcpyIn("xmlns:");
	str.sbStrcatIn((char *) prefix);

	DOMElement *x;

	x = static_cast <DOMElement *> (mp_xpathNode);

//	if (x == NULL) {
//
//		throw XSECException(XSECException::TransformError,
//			"Found a non ELEMENT node as the XPath node in DSIGTransformXPath");
//	}

	x->setAttributeNS(DSIGConstants::s_unicodeStrURIXMLNS,
		str.sbStrToXMLCh(),
		MAKE_UNICODE_STRING(value));

	mp_NSMap = mp_xpathNode->getAttributes();


}

void DSIGTransformXPath::deleteNamespace(const char * prefix) {

	DOMElement *x;

	x = static_cast <DOMElement *> (mp_xpathNode);

//	if (x == NULL) {
//
//		throw XSECException(XSECException::TransformError,
//			"Found a non ELEMENT node as the XPath node in DSIGTransformXPath");
//	}

	x->removeAttributeNS(DSIGConstants::s_unicodeStrURIXMLNS,
		MAKE_UNICODE_STRING(prefix));

}

