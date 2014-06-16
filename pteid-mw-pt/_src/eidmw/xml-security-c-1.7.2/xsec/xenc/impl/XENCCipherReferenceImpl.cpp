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
 * XENCCipherReference := Implementation for CipherReference element
 *
 * $Id: XENCCipherReferenceImpl.cpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/dsig/DSIGTransformList.hpp>
#include <xsec/dsig/DSIGReference.hpp>
#include <xsec/dsig/DSIGTransformList.hpp>
#include <xsec/dsig/DSIGTransformBase64.hpp>
#include <xsec/dsig/DSIGTransformXPath.hpp>
#include <xsec/dsig/DSIGTransformXPathFilter.hpp>
#include <xsec/dsig/DSIGTransformXSL.hpp>
#include <xsec/dsig/DSIGTransformC14n.hpp>
#include <xsec/framework/XSECError.hpp>
#include <xsec/utils/XSECDOMUtils.hpp>
#include <xsec/utils/XSECSafeBufferFormatter.hpp>
#include <xsec/framework/XSECEnv.hpp>

#include <xercesc/util/XMLUniDefs.hpp>
#include <xercesc/util/Janitor.hpp>

#include "XENCCipherReferenceImpl.hpp"

XERCES_CPP_NAMESPACE_USE

// --------------------------------------------------------------------------------
//			String Constants
// --------------------------------------------------------------------------------

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
	chNull
};

static XMLCh s_Transforms[] = {

	chLatin_T,
	chLatin_r,
	chLatin_a,
	chLatin_n,
	chLatin_s,
	chLatin_f,
	chLatin_o,
	chLatin_r,
	chLatin_m,
	chLatin_s,
	chNull
};

// --------------------------------------------------------------------------------
//			Constructors/Destructors
// --------------------------------------------------------------------------------

XENCCipherReferenceImpl::XENCCipherReferenceImpl(const XSECEnv * env) :
mp_env(env),
mp_cipherReferenceElement(NULL),
mp_uriAttr(NULL),
mp_transformsElement(NULL),
mp_transformList(NULL) {

}

XENCCipherReferenceImpl::XENCCipherReferenceImpl(const XSECEnv * env, DOMElement * node) :
mp_env(env),
mp_cipherReferenceElement(node),
mp_uriAttr(NULL),
mp_transformsElement(NULL),
mp_transformList(NULL) {

}


XENCCipherReferenceImpl::~XENCCipherReferenceImpl() {

	if (mp_transformList != NULL)
		delete mp_transformList;

}

// --------------------------------------------------------------------------------
//           Creation of Transforms
// --------------------------------------------------------------------------------

void XENCCipherReferenceImpl::createTransformList(void) {

	// Creates the transforms list
	safeBuffer str;
	const XMLCh * prefix;
	DOMDocument *doc = mp_env->getParentDocument();

	prefix = mp_env->getXENCNSPrefix();
	
	if (mp_transformsElement == NULL) {

		// Need to create a transforms node
		makeQName(str, prefix, "Transforms");
		mp_transformsElement = doc->createElementNS(DSIGConstants::s_unicodeStrURIXENC, str.rawXMLChBuffer());
		mp_env->doPrettyPrint(mp_cipherReferenceElement);
		mp_cipherReferenceElement->appendChild(mp_transformsElement);
		mp_env->doPrettyPrint(mp_transformsElement);
		mp_env->doPrettyPrint(mp_cipherReferenceElement);

		// Create the list
		XSECnew(mp_transformList, DSIGTransformList());
	}

}

void XENCCipherReferenceImpl::addTransform(DSIGTransform * txfm, DOMElement * txfmElt) {

	if (mp_transformList == NULL)
		createTransformList();

	mp_transformsElement->appendChild(txfmElt);
	mp_env->doPrettyPrint(mp_transformsElement);

	mp_transformList->addTransform(txfm);
}


DSIGTransformBase64 * XENCCipherReferenceImpl::appendBase64Transform() {

	DOMElement *txfmElt;
	DSIGTransformBase64 * txfm;

	XSECnew(txfm, DSIGTransformBase64(mp_env));
	txfmElt = txfm->createBlankTransform(mp_env->getParentDocument());

	addTransform(txfm, txfmElt);

	return txfm;

}

DSIGTransformXSL * XENCCipherReferenceImpl::appendXSLTransform(DOMNode * stylesheet) {

	DOMElement *txfmElt;
	DSIGTransformXSL * txfm;

	XSECnew(txfm, DSIGTransformXSL(mp_env));
	txfmElt = txfm->createBlankTransform(mp_env->getParentDocument());
	txfm->setStylesheet(stylesheet);

	addTransform(txfm, txfmElt);

	return txfm;

}

DSIGTransformC14n * XENCCipherReferenceImpl::appendCanonicalizationTransform(canonicalizationMethod cm) {

	DOMElement *txfmElt;
	DSIGTransformC14n * txfm;

	XSECnew(txfm, DSIGTransformC14n(mp_env));
	txfmElt = txfm->createBlankTransform(mp_env->getParentDocument());
	txfm->setCanonicalizationMethod(cm);

	addTransform(txfm, txfmElt);

	return txfm;

}	

DSIGTransformXPath * XENCCipherReferenceImpl::appendXPathTransform(const char * expr) {

	DOMElement *txfmElt;
	DSIGTransformXPath * txfm;

	XSECnew(txfm, DSIGTransformXPath(mp_env));
	txfmElt = txfm->createBlankTransform(mp_env->getParentDocument());
	txfm->setExpression(expr);

	addTransform(txfm, txfmElt);

	return txfm;
}

DSIGTransformXPathFilter * XENCCipherReferenceImpl::appendXPathFilterTransform(void) {

	DOMElement *txfmElt;
	DSIGTransformXPathFilter * txfm;

	XSECnew(txfm, DSIGTransformXPathFilter(mp_env));
	txfmElt = txfm->createBlankTransform(mp_env->getParentDocument());

	addTransform(txfm, txfmElt);
	mp_env->doPrettyPrint(txfmElt);

	return txfm;
}

// --------------------------------------------------------------------------------
//			Load
// --------------------------------------------------------------------------------

void XENCCipherReferenceImpl::load(void) {

	if (mp_cipherReferenceElement == NULL) {

		// Attempt to load an empty encryptedType element
		throw XSECException(XSECException::CipherReferenceError,
			"XENCCipherReference::load - called on empty DOM");

	}

	if (!strEquals(getXENCLocalName(mp_cipherReferenceElement), s_CipherReference)) {
	
		throw XSECException(XSECException::CipherReferenceError,
			"XENCCipherReference::load - called incorrect node");
	
	}

	// Find the URI attribute
	mp_uriAttr = mp_cipherReferenceElement->getAttributeNodeNS(NULL, DSIGConstants::s_unicodeStrURI);

	if (mp_uriAttr == NULL) {

		throw XSECException(XSECException::CipherReferenceError,
			"XENCCipherReference::load - URI attribute not found");
	
	}

	// See if there are any transforms
	DOMElement * c = findFirstElementChild(mp_cipherReferenceElement);
	if (c != NULL) {

		if (!strEquals(getXENCLocalName(c), s_Transforms)) {

			throw XSECException(XSECException::CipherReferenceError,
				"XENCCipherReference::load - Expected Transforms, found something else");

		}

		mp_transformsElement = c;

		XSECSafeBufferFormatter * formatter;
		XSECnew(formatter, XSECSafeBufferFormatter("UTF-8",XMLFormatter::NoEscapes, 
												XMLFormatter::UnRep_CharRef));
		Janitor<XSECSafeBufferFormatter> j_formatter(formatter);

		mp_transformList = DSIGReference::loadTransforms(c, formatter, mp_env);

	}


}

// --------------------------------------------------------------------------------
//			Create a blank structure
// --------------------------------------------------------------------------------

DOMElement * XENCCipherReferenceImpl::createBlankCipherReference(
						const XMLCh * URI) {

	// Clean up
	if (mp_transformList != NULL) {
		delete mp_transformList;
		mp_transformList = NULL;
	}

	mp_uriAttr = NULL;

	// Get some setup values
	safeBuffer str;
	DOMDocument *doc = mp_env->getParentDocument();
	const XMLCh * prefix = mp_env->getXENCNSPrefix();

	makeQName(str, prefix, s_CipherReference);

	mp_cipherReferenceElement = doc->createElementNS(DSIGConstants::s_unicodeStrURIXENC, str.rawXMLChBuffer());

	// Set the URI Attribute
	mp_cipherReferenceElement->setAttributeNS(NULL, DSIGConstants::s_unicodeStrURI, URI);

	// Find the URI
	mp_uriAttr = mp_cipherReferenceElement->getAttributeNodeNS(NULL, DSIGConstants::s_unicodeStrURI);

	if (mp_uriAttr == NULL) {

		throw XSECException(XSECException::CipherReferenceError,
			"XENCCipherReference::createBlankReference - URI attribute not found after creation");
	
	}

	return mp_cipherReferenceElement;

}

// --------------------------------------------------------------------------------
//			Get Interface methods
// --------------------------------------------------------------------------------

DSIGTransformList * XENCCipherReferenceImpl::getTransforms(void) const {

	return mp_transformList;

}

const XMLCh * XENCCipherReferenceImpl::getURI (void) const {

	if (mp_uriAttr != NULL)
		return mp_uriAttr->getNodeValue();

	return NULL;

}

DOMElement * XENCCipherReferenceImpl::getElement(void) const {

	return mp_cipherReferenceElement;

}
