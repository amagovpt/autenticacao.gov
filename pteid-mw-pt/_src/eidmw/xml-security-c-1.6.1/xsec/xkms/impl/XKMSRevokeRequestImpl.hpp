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
 * XKMSRevokeRequestImpl := Implementation for RevokeRequest Messages
 *
 * $Id:$
 *
 */

#ifndef XKMSREVOKEREQUESTIMPL_INCLUDE
#define XKMSREVOKEREQUESTIMPL_INCLUDE

// XSEC Includes

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/xkms/XKMSRevokeRequest.hpp>
#include "XKMSRequestAbstractTypeImpl.hpp"

class XKMSAuthenticationImpl;
class DSIGSignature;
class XKMSRevokeKeyBindingImpl;

class XKMSRevokeRequestImpl : public XKMSRevokeRequest {

public: 
	XKMSRequestAbstractTypeImpl m_request;
	XKMSMessageAbstractTypeImpl &m_msg;
public:

	XKMSRevokeRequestImpl(
		const XSECEnv * env
	);

	XKMSRevokeRequestImpl(
		const XSECEnv * env, 
		XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * node
	);

	virtual ~XKMSRevokeRequestImpl();

	// Load elements
	void load();

	// Creation
	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * 
		createBlankRevokeRequest(
		const XMLCh * service,
		const XMLCh * id = NULL);


	/* Getter Interface Methods */
	virtual XKMSRevokeKeyBinding * getRevokeKeyBinding(void) const;
	virtual XKMSAuthentication * getAuthentication (void) const;
	virtual const XMLCh * getRevocationCode(void) const;

	/* Setter Interface Methods */

	virtual XKMSRevokeKeyBinding * addRevokeKeyBinding(XKMSStatus::StatusValue status);
	virtual XKMSAuthentication * addAuthentication(void);
	virtual void addRevocationCode(const XMLCh * code);


	/* Implemented from MessageAbstractType */
	virtual messageType getMessageType(void);

	/* Forced inheritance from XKMSMessageAbstractTypeImpl */
	XKMS_MESSAGEABSTRACTYPE_IMPL_METHODS

	/* Forced inheritance from RequestAbstractType */
	XKMS_REQUESTABSTRACTYPE_IMPL_METHODS


private:

	XKMSAuthenticationImpl		* mp_authentication;
	XKMSRevokeKeyBindingImpl	* mp_revokeKeyBinding;
	XERCES_CPP_NAMESPACE_QUALIFIER
		DOMElement				* mp_revocationCodeElement;

	XSECProvider				m_prov;		// For creating the signature

	// Unimplemented
	XKMSRevokeRequestImpl(void);
	XKMSRevokeRequestImpl(const XKMSRevokeRequestImpl &);
	XKMSRevokeRequestImpl & operator = (const XKMSRevokeRequestImpl &);

};

#endif /* XKMSREVOKEREQUESTIMPL_INCLUDE */

