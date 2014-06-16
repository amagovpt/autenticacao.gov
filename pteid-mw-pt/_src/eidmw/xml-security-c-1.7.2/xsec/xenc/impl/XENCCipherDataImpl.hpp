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
 * $Id: XENCCipherDataImpl.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef XSECCIPHERDATAIMPL_INCLUDE
#define XSECCIPHERDATAIMPL_INCLUDE

// XSEC Includes

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/xenc/XENCCipherData.hpp>

#include "XENCCipherImpl.hpp"

class XENCCipherValueImpl;
class XENCCipherReferenceImpl;

XSEC_DECLARE_XERCES_CLASS(DOMNode);

class XENCCipherDataImpl : public XENCCipherData {

public:

	XENCCipherDataImpl(const XSECEnv * env);
	XENCCipherDataImpl(
		const XSECEnv * env, 
		XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * node
	);

	virtual ~XENCCipherDataImpl();

	// Load elements
	void load();
	// Create
	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * 
		createBlankCipherData(XENCCipherData::XENCCipherDataType type, const XMLCh * value);


	// Interface methods
	virtual XENCCipherDataType getCipherDataType(void);
	virtual XENCCipherValue * getCipherValue(void);
	virtual XENCCipherReference * getCipherReference(void);
	virtual XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * getElement(void)
		{return mp_cipherDataElement;}


private:

	// Unimplemented constructor
	XENCCipherDataImpl();

	const XSECEnv			* mp_env;
	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement					
							* mp_cipherDataElement;		// Node at head of structure

	XENCCipherDataType		m_cipherDataType;			// Is this a value or a reference?
	XENCCipherValueImpl		* mp_cipherValue;			// Cipher value node
	XENCCipherReferenceImpl		* mp_cipherReference;			// Cipher Reference node
};

#endif /* XENCCIPHERDATAIMPL_INCLUDE */

