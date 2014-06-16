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
 * XKMSRespondWithImpl := Implementation of XKMSRespondWith
 *
 * $Id: XKMSRespondWithImpl.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef XKMSRESPONDWITHIMPL_INCLUDE
#define XKMSRESPONDWITHIMPL_INCLUDE

// XSEC Includes

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/xkms/XKMSRespondWith.hpp>

class XSECEnv;

class XKMSRespondWithImpl : public XKMSRespondWith {

public:

	XKMSRespondWithImpl(const XSECEnv * env);
	XKMSRespondWithImpl(		
		const XSECEnv * env, 
		XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * node
	);

	virtual ~XKMSRespondWithImpl();

	// load
	void load(void);

	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * 
		createBlankRespondWith(
		const XMLCh * item);

	/* Getter Interface Methods */

	virtual XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * getElement(void) const {
		return mp_respondWithElement;
	}

	virtual const XMLCh * getRespondWithString(void) const;

	/* Setter interface methods */

	virtual void setRespondWithString(const XMLCh * str);

	//@}


private:

	const XSECEnv			* mp_env;
	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement 
							* mp_respondWithElement;
	XERCES_CPP_NAMESPACE_QUALIFIER DOMNode
							* mp_respondWithTextNode;

	// Unimplemented
	XKMSRespondWithImpl(const XKMSRespondWithImpl &);
	XKMSRespondWithImpl & operator = (const XKMSRespondWithImpl &);

};

#endif /* XKMSRESPONDWITHIMPL_INCLUDE */
