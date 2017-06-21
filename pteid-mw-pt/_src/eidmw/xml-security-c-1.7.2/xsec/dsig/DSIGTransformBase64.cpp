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
 * DSIGTransformBase64 := Class that holds a Base64 transform structure
 *
 * $Id: DSIGTransformBase64.cpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

// XSEC

#include <xsec/dsig/DSIGTransformBase64.hpp>
#include <xsec/dsig/DSIGSignature.hpp>
#include <xsec/transformers/TXFMBase64.hpp>
#include <xsec/transformers/TXFMC14n.hpp>
#include <xsec/transformers/TXFMChain.hpp>
#include <xsec/transformers/TXFMXPath.hpp>
#include <xsec/framework/XSECException.hpp>
#include <xsec/framework/XSECEnv.hpp>
#include <xsec/utils/XSECDOMUtils.hpp>
#include <xsec/framework/XSECError.hpp>

XERCES_CPP_NAMESPACE_USE

// --------------------------------------------------------------------------------
//           Constructors and Destructors
// --------------------------------------------------------------------------------

DSIGTransformBase64::DSIGTransformBase64(const XSECEnv * env, DOMNode * node) :
DSIGTransform(env, node) {};


DSIGTransformBase64::DSIGTransformBase64(const XSECEnv * env) :
DSIGTransform(env) {};
		  

DSIGTransformBase64::~DSIGTransformBase64() {};

// --------------------------------------------------------------------------------
//           Interface Methods
// --------------------------------------------------------------------------------


transformType DSIGTransformBase64::getTransformType() {

	return TRANSFORM_BASE64;

}


void DSIGTransformBase64::appendTransformer(TXFMChain * input) {

	// If the input is a Nodeset then we need to find the text from the input

	if (input->getLastTxfm()->getOutputType() == TXFMBase::DOM_NODES) {

		if (input->getLastTxfm()->getNodeType() != TXFMBase::DOM_NODE_XPATH_NODESET) {

#ifdef XSEC_NO_XPATH

			throw XSECException(XSECException::UnsupportedFunction,
				"Unable to extract Base64 text from Nodes without XPath support");

		}

#else
		
			// Use an XPath transform to get "Self::text()" from the nodeset
		
			TXFMXPath *x;
		
			XSECnew(x, TXFMXPath(mp_txfmNode->getOwnerDocument()));
			input->appendTxfm(x);
			((TXFMXPath *) x)->evaluateExpr(mp_txfmNode, "self::text()");

		}
		
		TXFMC14n *c;
		
		// Now use c14n to translate to BYTES
		
		XSECnew(c, TXFMC14n(mp_txfmNode->getOwnerDocument()));
		input->appendTxfm(c);
#endif

	}

	// Now the actual Base64

	TXFMBase64 *b;
	XSECnew(b, TXFMBase64(mp_txfmNode->getOwnerDocument()));
	input->appendTxfm(b);

}

DOMElement * DSIGTransformBase64::createBlankTransform(DOMDocument * parentDoc) {

	safeBuffer str;
	const XMLCh * prefix;
	DOMElement *ret;
	DOMDocument *doc = mp_env->getParentDocument();

	prefix = mp_env->getDSIGNSPrefix();
	
	// Create the transform node
	makeQName(str, prefix, "Transform");
	ret = doc->createElementNS(DSIGConstants::s_unicodeStrURIDSIG, str.rawXMLChBuffer());
	ret->setAttributeNS(NULL,DSIGConstants::s_unicodeStrAlgorithm, DSIGConstants::s_unicodeStrURIBASE64);

	mp_txfmNode = ret;

	return ret;

}

void DSIGTransformBase64::load(void) {

	// Do nothing for a Base64 transform

}
