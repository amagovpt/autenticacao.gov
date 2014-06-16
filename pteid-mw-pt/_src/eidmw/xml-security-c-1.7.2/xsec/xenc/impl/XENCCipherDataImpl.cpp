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
 * XENCCipherDataImpl := Implementation of CipherData elements 
 *
 * $Id: XENCCipherDataImpl.cpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#include <xsec/framework/XSECDefs.hpp>

#include "XENCCipherDataImpl.hpp"
#include "XENCCipherValueImpl.hpp"
#include "XENCCipherReferenceImpl.hpp"

#include <xsec/framework/XSECError.hpp>
#include <xsec/utils/XSECDOMUtils.hpp>
#include <xsec/framework/XSECEnv.hpp>

#include <xercesc/util/XMLUniDefs.hpp>

XERCES_CPP_NAMESPACE_USE

// --------------------------------------------------------------------------------
//			String Constants
// --------------------------------------------------------------------------------

static XMLCh s_CipherData[] = {

	chLatin_C,
	chLatin_i,
	chLatin_p,
	chLatin_h,
	chLatin_e,
	chLatin_r,
	chLatin_D,
	chLatin_a,
	chLatin_t,
	chLatin_a,
	chNull,
};

static XMLCh s_CipherValue[] = {

	chLatin_C,
	chLatin_i,
	chLatin_p,
	chLatin_h,
	chLatin_e,
	chLatin_r,
	chLatin_V,
	chLatin_a,
	chLatin_l,
	chLatin_u,
	chLatin_e,
	chNull,
};

static XMLCh s_CipherReference[] = {

	chLatin_C,
	chLatin_i,
	chLatin_p,
	chLatin_h,
	chLatin_e,
	chLatin_r,
	chLatin_R,
	chLatin_e,
	chLatin_f,
	chLatin_e,
	chLatin_r,
	chLatin_e,
	chLatin_n,
	chLatin_c,
	chLatin_e,
	chNull,
};

// --------------------------------------------------------------------------------
//			Constructors and Destructors
// --------------------------------------------------------------------------------

XENCCipherDataImpl::XENCCipherDataImpl(const XSECEnv * env) :
mp_env(env),
mp_cipherDataElement(NULL),
mp_cipherValue(NULL),
mp_cipherReference(NULL) {

}


XENCCipherDataImpl::XENCCipherDataImpl(const XSECEnv * env, DOMElement * node) :
mp_env(env),
mp_cipherDataElement(node),
mp_cipherValue(NULL),
mp_cipherReference(NULL) {

}

XENCCipherDataImpl::~XENCCipherDataImpl() {

	if (mp_cipherValue != NULL)
		delete mp_cipherValue;
	if (mp_cipherReference != NULL)
		delete mp_cipherReference;

}

// --------------------------------------------------------------------------------
//			Load DOM
// --------------------------------------------------------------------------------

void XENCCipherDataImpl::load() {

	if (mp_cipherDataElement == NULL) {

		// Attempt to load an empty encryptedType element
		throw XSECException(XSECException::CipherDataError,
			"XENCCipherData::load - called on empty DOM");

	}

	if (!strEquals(getXENCLocalName(mp_cipherDataElement), s_CipherData)) {
	
		throw XSECException(XSECException::CipherDataError,
			"XENCCipherData::load - called incorrect node");
	
	}

	// Find out whether this is a CipherValue or CipherReference and load
	// appropriately

	DOMElement *tmpElt = findFirstElementChild(mp_cipherDataElement);

	if (tmpElt != NULL && strEquals(getXENCLocalName(tmpElt), s_CipherValue)) {

		m_cipherDataType = VALUE_TYPE;
		XSECnew(mp_cipherValue, XENCCipherValueImpl(mp_env, tmpElt));
		mp_cipherValue->load();

	}

	else if (tmpElt != NULL && strEquals(getXENCLocalName(tmpElt), s_CipherReference)) {

		m_cipherDataType = REFERENCE_TYPE;
		XSECnew(mp_cipherReference, XENCCipherReferenceImpl(mp_env, tmpElt));
		mp_cipherReference->load();

	}

	else {

		throw XSECException(XSECException::ExpectedXENCChildNotFound,
			"XENCCipherData::load - expected <CipherValue> or <CipherReference>");

	}

}

// --------------------------------------------------------------------------------
//			Create a blank structure
// --------------------------------------------------------------------------------

DOMElement * XENCCipherDataImpl::createBlankCipherData(
						XENCCipherData::XENCCipherDataType type, 
						const XMLCh * value) {

	// Reset
	if (mp_cipherValue != NULL) {
		delete mp_cipherValue;
		mp_cipherValue = NULL;
	}

	m_cipherDataType = NO_TYPE;

	// Get some setup values
	safeBuffer str;
	DOMDocument *doc = mp_env->getParentDocument();
	const XMLCh * prefix = mp_env->getXENCNSPrefix();

	makeQName(str, prefix, s_CipherData);

	DOMElement *ret = doc->createElementNS(DSIGConstants::s_unicodeStrURIXENC, str.rawXMLChBuffer());
	mp_cipherDataElement = ret;

	mp_env->doPrettyPrint(ret);

	// Set the type
	if (type == VALUE_TYPE) {
		
		// Should also set in the DOM
		m_cipherDataType = VALUE_TYPE;

		// Create the Cipher Value
		XSECnew(mp_cipherValue, XENCCipherValueImpl(mp_env));
		DOMNode * cipherValueNode = mp_cipherValue->createBlankCipherValue(value);

		ret->appendChild(cipherValueNode);

		mp_env->doPrettyPrint(ret);

	}

	else if (type == REFERENCE_TYPE) {

		m_cipherDataType = REFERENCE_TYPE;

		// Create the Cipher Reference
		XSECnew(mp_cipherReference, XENCCipherReferenceImpl(mp_env));
		DOMNode * cipherReferenceNode = mp_cipherReference->createBlankCipherReference(value);

		ret->appendChild(cipherReferenceNode);
		mp_env->doPrettyPrint(ret);

	}

	return ret;

}

// --------------------------------------------------------------------------------
//			Constructors and Destructors
// --------------------------------------------------------------------------------

	// Interface methods
XENCCipherDataImpl::XENCCipherDataType XENCCipherDataImpl::getCipherDataType(void) {

	return m_cipherDataType;

}

XENCCipherValue * XENCCipherDataImpl::getCipherValue(void) {

	return mp_cipherValue;

}

XENCCipherReference * XENCCipherDataImpl::getCipherReference(void) {

	return mp_cipherReference;

}

