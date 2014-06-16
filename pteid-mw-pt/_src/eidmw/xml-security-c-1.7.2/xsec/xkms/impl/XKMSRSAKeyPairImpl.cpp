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
 * XKMSRSAKeyPairImpl := Implementation class for RSAKeyPair elements
 *
 * $Id:$
 *
 */

// XSEC Includes

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/utils/XSECDOMUtils.hpp>
#include <xsec/framework/XSECError.hpp>
#include <xsec/framework/XSECEnv.hpp>
#include <xsec/xkms/XKMSConstants.hpp>


#include "XKMSRSAKeyPairImpl.hpp"

#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/XMLUniDefs.hpp>

XERCES_CPP_NAMESPACE_USE

#define XKMS_GET_RSA_VAL(XVAL) e = (DOMElement *) findNextElementChild(e); \
	if (e == NULL || !strEquals(getXKMSLocalName(e), XKMSConstants::s_tag##XVAL) || \
		((t = findFirstChildOfType(e, DOMNode::TEXT_NODE)) == NULL)) { \
		throw XSECException(XSECException::ExpectedXKMSChildNotFound, \
			"XKMSRSAKeyPair::load - Expected <XVAL> node"); \
	} \
	mp_##XVAL = t->getNodeValue();

#define XKMS_SET_RSA_VAL(XVAL) makeQName(str, prefix, XKMSConstants::s_tag##XVAL); \
	e = doc->createElementNS(XKMSConstants::s_unicodeStrURIXKMS, \
												str.rawXMLChBuffer());\
	e->appendChild(doc->createTextNode(XVAL));\
	mp_RSAKeyPairElement->appendChild(e);\
	mp_env->doPrettyPrint(mp_RSAKeyPairElement);

#define XKMS_RET_RSA_VAL(XVAL) const XMLCh * XKMSRSAKeyPairImpl::get##XVAL (void) const {\
	return mp_##XVAL;}

// --------------------------------------------------------------------------------
//           Construct/Destruct
// --------------------------------------------------------------------------------

XKMSRSAKeyPairImpl::XKMSRSAKeyPairImpl(const XSECEnv * env) :
mp_env(env),
mp_RSAKeyPairElement(NULL),
mp_Modulus(NULL),
mp_Exponent(NULL),
mp_P(NULL),
mp_Q(NULL),
mp_DP(NULL),
mp_DQ(NULL),
mp_InverseQ(NULL),
mp_D(NULL) {}

XKMSRSAKeyPairImpl::XKMSRSAKeyPairImpl(const XSECEnv * env, DOMElement * node) :
mp_env(env),
mp_RSAKeyPairElement(node),
mp_Modulus(NULL),
mp_Exponent(NULL),
mp_P(NULL),
mp_Q(NULL),
mp_DP(NULL),
mp_DQ(NULL),
mp_InverseQ(NULL),
mp_D(NULL) {}


XKMSRSAKeyPairImpl::~XKMSRSAKeyPairImpl() {}

// --------------------------------------------------------------------------------
//           load
// --------------------------------------------------------------------------------

void XKMSRSAKeyPairImpl::load(void) {

	if (mp_RSAKeyPairElement == NULL) {

		// Attempt to load an empty element
		throw XSECException(XSECException::XKMSError,
			"XKMSRSAKeyPair::load - called on empty DOM");

	}

	DOMElement * e;
	DOMNode * t;
	// Check for Modulus node
	e = (DOMElement *) findFirstElementChild(mp_RSAKeyPairElement);
	if (e == NULL || !strEquals(getXKMSLocalName(e), XKMSConstants::s_tagModulus) ||
		((t = findFirstChildOfType(e, DOMNode::TEXT_NODE)) == NULL)) {

		throw XSECException(XSECException::ExpectedXKMSChildNotFound,
			"XKMSRSAKeyPair::load - Expected <Modulus> node");

	}
	mp_Modulus = t->getNodeValue();

	XKMS_GET_RSA_VAL(Exponent);
	XKMS_GET_RSA_VAL(P);
	XKMS_GET_RSA_VAL(Q);
	XKMS_GET_RSA_VAL(DP);
	XKMS_GET_RSA_VAL(DQ);
	XKMS_GET_RSA_VAL(InverseQ);
	XKMS_GET_RSA_VAL(D);

}

// --------------------------------------------------------------------------------
//           Build from Scratch
// --------------------------------------------------------------------------------

DOMElement * XKMSRSAKeyPairImpl::createBlankXKMSRSAKeyPairImpl(
			const XMLCh * Modulus, 
			const XMLCh * Exponent, 
			const XMLCh * P, 
			const XMLCh * Q, 
			const XMLCh * DP, 
			const XMLCh * DQ, 
			const XMLCh * InverseQ, 
			const XMLCh * D) {

	// Get some setup values
	safeBuffer str;
	DOMDocument *doc = mp_env->getParentDocument();
	const XMLCh * prefix = mp_env->getXKMSNSPrefix();

	makeQName(str, prefix, XKMSConstants::s_tagRSAKeyPair);

	mp_RSAKeyPairElement = doc->createElementNS(XKMSConstants::s_unicodeStrURIXKMS, 
												str.rawXMLChBuffer());

	mp_env->doPrettyPrint(mp_RSAKeyPairElement);

	/* Now add the Values element */
	DOMElement * e;
	XKMS_SET_RSA_VAL(Modulus)
	XKMS_SET_RSA_VAL(Exponent)
	XKMS_SET_RSA_VAL(P)
	XKMS_SET_RSA_VAL(Q)
	XKMS_SET_RSA_VAL(DP)
	XKMS_SET_RSA_VAL(DQ)
	XKMS_SET_RSA_VAL(InverseQ)
	XKMS_SET_RSA_VAL(D)

	return mp_RSAKeyPairElement;

}

XKMS_RET_RSA_VAL(Modulus)
XKMS_RET_RSA_VAL(Exponent)
XKMS_RET_RSA_VAL(P)
XKMS_RET_RSA_VAL(Q)
XKMS_RET_RSA_VAL(DP)
XKMS_RET_RSA_VAL(DQ)
XKMS_RET_RSA_VAL(InverseQ)
XKMS_RET_RSA_VAL(D)


