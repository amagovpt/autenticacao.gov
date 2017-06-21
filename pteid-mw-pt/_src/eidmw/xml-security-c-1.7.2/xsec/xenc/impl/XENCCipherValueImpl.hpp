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
 * XENCCipherValueImpl := Implementation for CipherValue elements
 *
 * $Id: XENCCipherValueImpl.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef XENCCIPHERVALUEIMPL_INCLUDE
#define XENCCIPHERVALUEIMPL_INCLUDE

// XSEC Includes

#include <xsec/framework/XSECDefs.hpp>

#include <xsec/xenc/XENCCipherValue.hpp>

#include "XENCCipherImpl.hpp"

XSEC_DECLARE_XERCES_CLASS(DOMNode);

class XSECEnv;

class XENCCipherValueImpl : public XENCCipherValue {

public:

	XENCCipherValueImpl(const XSECEnv * env);
	XENCCipherValueImpl(
		const XSECEnv * env, 
		XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * node
	);

	virtual ~XENCCipherValueImpl();

	// Load
	void load(void);
	// Create
	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * 
		createBlankCipherValue(const XMLCh * value);

	// Interface methods

	virtual const XMLCh * getCipherString(void) const;
	virtual void setCipherString(const XMLCh * value);
	virtual XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * getElement(void) const
		{return mp_cipherValueElement;}

private:

	// Unimplemented constructors
	XENCCipherValueImpl(const XENCCipherValueImpl &);
	XENCCipherValueImpl & operator = (const XENCCipherValueImpl &);

	const XSECEnv			* mp_env;
	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement					
							* mp_cipherValueElement;
	
	// This is held as a string, not a node as we might have multiple text
	// nodes making up the string

	XMLCh 					* mp_cipherString;
};

#endif /* XENCCIPHERVALUEIMPL_INCLUDE */

