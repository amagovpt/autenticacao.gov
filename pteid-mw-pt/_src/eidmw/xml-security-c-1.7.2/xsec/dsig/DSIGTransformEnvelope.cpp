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
 * DSIGTransformEnvelope := Class that calculates an Envelope with an XPath evaluator
 *
 * $Id: DSIGTransformEnvelope.cpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

// XSEC

#include <xsec/dsig/DSIGTransformEnvelope.hpp>
#include <xsec/dsig/DSIGSignature.hpp>
#include <xsec/transformers/TXFMEnvelope.hpp>
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

DSIGTransformEnvelope::DSIGTransformEnvelope(const XSECEnv * env, DOMNode * node) :
DSIGTransform(env, node) {};


DSIGTransformEnvelope::DSIGTransformEnvelope(const XSECEnv * env) :
DSIGTransform(env) {};
		  

DSIGTransformEnvelope::~DSIGTransformEnvelope() {};

// --------------------------------------------------------------------------------
//           Interface Methods
// --------------------------------------------------------------------------------


transformType DSIGTransformEnvelope::getTransformType() {

	return TRANSFORM_ENVELOPED_SIGNATURE;

}


void DSIGTransformEnvelope::appendTransformer(TXFMChain * input) {

#ifdef XSEC_USE_XPATH_ENVELOPE

	TXFMXPath *x;
	
	// Special XPath transform
	XSECnew(x, TXFMXPath(mp_txfmNode->getOwnerDocument()));
	input->appendTxfm(x);
	
	// Execute the envelope expression
	x->evaluateEnvelope(mp_txfmNode);
#else

	TXFMEnvelope *x;

	// Use the Envelope transform
	
	XSECnew(x, TXFMEnvelope(mp_txfmNode->getOwnerDocument()));
	input->appendTxfm(x);

	// Execute envelope
	x->evaluateEnvelope(mp_txfmNode);

#endif

}

DOMElement * DSIGTransformEnvelope::createBlankTransform(DOMDocument * parentDoc) {

	safeBuffer str;
	const XMLCh * prefix;
	DOMElement *ret;
	DOMDocument *doc = mp_env->getParentDocument();

	prefix = mp_env->getDSIGNSPrefix();
	
	// Create the transform node
	makeQName(str, prefix, "Transform");
	ret = doc->createElementNS(DSIGConstants::s_unicodeStrURIDSIG, str.rawXMLChBuffer());
	ret->setAttributeNS(NULL,DSIGConstants::s_unicodeStrAlgorithm, DSIGConstants::s_unicodeStrURIENVELOPE);

	mp_txfmNode = ret;

	return ret;

}

void DSIGTransformEnvelope::load(void) {

	// Do nothing for an Envelope transform

}
