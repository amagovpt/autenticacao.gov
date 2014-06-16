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
 * XKMSNotBoundAuthenticationImpl := Interface for NotBoundAuthentication elements
 *
 * $Id: XKMSNotBoundAuthenticationImpl.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef XKMSNOTBOUNDAUTHENTICATIONIMPL_INCLUDE
#define XKMSNOTBOUNDAUTHENTICATIONIMPL_INCLUDE

#include <xsec/framework/XSECDefs.hpp>

#include <xsec/xkms/XKMSNotBoundAuthentication.hpp>

class XKMSNotBoundAuthenticationImpl : public XKMSNotBoundAuthentication {

public:

	/* Constructors and Destructors */

	XKMSNotBoundAuthenticationImpl(
		const XSECEnv * env
	);

	XKMSNotBoundAuthenticationImpl(
		const XSECEnv * env, 
		XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * node
	);

	virtual ~XKMSNotBoundAuthenticationImpl();

	// Load
	void load(void);

	// Create
	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *
		createBlankNotBoundAuthentication(const XMLCh * protocol, const XMLCh * value);
	
	// Get interfaces

	virtual const XMLCh * getProtocol(void) const;
	virtual const XMLCh * getValue(void) const;

	// Set Methods

	virtual void setProtocol(const XMLCh * uri);
	virtual void setValue(const XMLCh * value);

	//@}

private:

	const XSECEnv * mp_env;

	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement 
						* mp_notBoundAuthenticationElement;

	XERCES_CPP_NAMESPACE_QUALIFIER DOMNode
						* mp_protocolAttr;
	XERCES_CPP_NAMESPACE_QUALIFIER DOMNode
						* mp_valueAttr;

	// Unimplemented
	XKMSNotBoundAuthenticationImpl(void);
	XKMSNotBoundAuthenticationImpl(const XKMSNotBoundAuthenticationImpl &);
	XKMSNotBoundAuthenticationImpl & operator = (const XKMSNotBoundAuthenticationImpl &);

};

#endif /* XKMSNOTBOUNDAUTHENTICATIONIMPL_INCLUDE */
