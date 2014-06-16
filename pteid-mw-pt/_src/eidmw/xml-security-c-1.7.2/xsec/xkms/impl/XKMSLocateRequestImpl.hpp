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
 * XKMSLocateRequestImpl := Implementation of LocateRequest Messages
 *
 * $Id: XKMSLocateRequestImpl.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef XKMSLOCATEREQUESTIMPL_INCLUDE
#define XKMSLOCATEREQUESTIMPL_INCLUDE

// XSEC Includes

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/xkms/XKMSLocateRequest.hpp>

#include "XKMSRequestAbstractTypeImpl.hpp"
#include "XKMSQueryKeyBindingImpl.hpp"

class XKMSQueryKeyRequestImpl;

class XKMSLocateRequestImpl : public XKMSLocateRequest {

public: 
	XKMSRequestAbstractTypeImpl m_request;
	XKMSMessageAbstractTypeImpl &m_msg;
public:

	XKMSLocateRequestImpl(
		const XSECEnv * env
	);

	XKMSLocateRequestImpl(
		const XSECEnv * env, 
		XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * node
	);

	virtual ~XKMSLocateRequestImpl();

	// Load elements
	void load();

	// Creation
	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * 
		createBlankLocateRequest(
		const XMLCh * service,
		const XMLCh * id = NULL);


	/* Getter Interface Methods */

	virtual XKMSQueryKeyBinding * getQueryKeyBinding(void);

	/* Setter interface methods */
	virtual XKMSQueryKeyBinding * addQueryKeyBinding(void);

	/* Implemented from MessageAbstractType */
	virtual messageType getMessageType(void);

	/* Forced inheritance from XKMSMessageAbstractTypeImpl */
	XKMS_MESSAGEABSTRACTYPE_IMPL_METHODS

	/* Forced inheritance from RequestAbstractType */
	XKMS_REQUESTABSTRACTYPE_IMPL_METHODS

private:

	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement
				* mp_queryKeyBindingElement;
	XKMSQueryKeyBindingImpl * mp_queryKeyBinding;

	// Unimplemented
	XKMSLocateRequestImpl(const XKMSLocateRequestImpl &);
	XKMSLocateRequestImpl & operator = (const XKMSLocateRequestImpl &);

};

#endif /* XKMSLOCATEREQUESTIMPL_INCLUDE */
