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
 * XKMSRecoverRequestImpl := Implementation for RecoverRequest Messages
 *
 * $Id$
 *
 */

#ifndef XKMSRECOVERREQUESTIMPL_INCLUDE
#define XKMSRECOVERREQUESTIMPL_INCLUDE

// XSEC Includes

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/xkms/XKMSRecoverRequest.hpp>
#include "XKMSRequestAbstractTypeImpl.hpp"

class XKMSAuthenticationImpl;
class DSIGSignature;
class XKMSRecoverKeyBindingImpl;

class XKMSRecoverRequestImpl : public XKMSRecoverRequest {

public: 
	XKMSRequestAbstractTypeImpl m_request;
	XKMSMessageAbstractTypeImpl &m_msg;
public:

	XKMSRecoverRequestImpl(
		const XSECEnv * env
	);

	XKMSRecoverRequestImpl(
		const XSECEnv * env, 
		XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * node
	);

	virtual ~XKMSRecoverRequestImpl();

	// Load elements
	void load();

	// Creation
	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * 
		createBlankRecoverRequest(
		const XMLCh * service,
		const XMLCh * id = NULL);


	/* Getter Interface Methods */
	virtual XKMSRecoverKeyBinding * getRecoverKeyBinding(void) const;
	virtual XKMSAuthentication * getAuthentication (void) const;

	/* Setter Interface Methods */

	virtual XKMSRecoverKeyBinding * addRecoverKeyBinding(XKMSStatus::StatusValue status);
	virtual XKMSAuthentication * addAuthentication(void);

	/* Implemented from MessageAbstractType */
	virtual messageType getMessageType(void);

	/* Forced inheritance from XKMSMessageAbstractTypeImpl */
	XKMS_MESSAGEABSTRACTYPE_IMPL_METHODS

	/* Forced inheritance from RequestAbstractType */
	XKMS_REQUESTABSTRACTYPE_IMPL_METHODS


private:

	XKMSAuthenticationImpl		* mp_authentication;
	XKMSRecoverKeyBindingImpl	* mp_recoverKeyBinding;

	XSECProvider				m_prov;		// For creating the signature

	// Unimplemented
	XKMSRecoverRequestImpl(void);
	XKMSRecoverRequestImpl(const XKMSRecoverRequestImpl &);
	XKMSRecoverRequestImpl & operator = (const XKMSRecoverRequestImpl &);

};

#endif /* XKMSRECOVERREQUESTIMPL_INCLUDE */

