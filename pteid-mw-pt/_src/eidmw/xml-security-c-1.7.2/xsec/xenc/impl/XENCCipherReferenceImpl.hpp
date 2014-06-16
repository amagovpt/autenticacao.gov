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
 * $Id: XENCCipherReferenceImpl.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef XENCCIPHERREFERENCEIMPL_INCLUDE
#define XENCCIPHERREFERENCEIMPL_INCLUDE

// XSEC Includes

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/xenc/XENCCipherReference.hpp>

XSEC_DECLARE_XERCES_CLASS(DOMNode);

class DSIGTransformList;
class XSECEnv;
class DSIGTransform;

class XENCCipherReferenceImpl : public XENCCipherReference {

public: 

	XENCCipherReferenceImpl(const XSECEnv * env);
	XENCCipherReferenceImpl(
		const XSECEnv * env, 
		XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * node
	);

	virtual ~XENCCipherReferenceImpl();

	// Load
	void load(void);
	// Create
	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * 
		createBlankCipherReference(const XMLCh * URI);

	// Get methods
	virtual DSIGTransformList * getTransforms(void) const;
	virtual const XMLCh * getURI (void) const;
	virtual XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * getElement(void) const;

	// Set methods
	virtual DSIGTransformBase64 * appendBase64Transform();
	virtual DSIGTransformXPath * appendXPathTransform(const char * expr);
	virtual DSIGTransformXPathFilter * appendXPathFilterTransform(void);
	virtual DSIGTransformXSL * appendXSLTransform(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *stylesheet);
	virtual DSIGTransformC14n * appendCanonicalizationTransform(canonicalizationMethod cm);


private:

	// Unimplemented
	XENCCipherReferenceImpl(const XENCCipherReference &);
	XENCCipherReferenceImpl & operator = (const XENCCipherReference &);

	// Private functions
	void createTransformList(void);
	void addTransform(DSIGTransform * txfm, 
		XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * txfmElt);


	const XSECEnv			* mp_env;
	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement					
							* mp_cipherReferenceElement;
	XERCES_CPP_NAMESPACE_QUALIFIER DOMNode
							* mp_uriAttr;
	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement
							* mp_transformsElement;

	DSIGTransformList		* mp_transformList;


};

#endif /* XENCCIPHERREFERENCE_INCLUDE */

