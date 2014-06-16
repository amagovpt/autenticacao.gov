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
 * XKMSRegisterRequestImpl := Implementation for RegisterRequest Messages
 *
 * $Id: XKMSRegisterRequestImpl.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef XKMSREGISTERREQUESTIMPL_INCLUDE
#define XKMSREGISTERREQUESTIMPL_INCLUDE

// XSEC Includes

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/xkms/XKMSRegisterRequest.hpp>
#include "XKMSRequestAbstractTypeImpl.hpp"

class XKMSAuthenticationImpl;
class XKMSPrototypeKeyBindingImpl;
class DSIGSignature;

class XKMSRegisterRequestImpl : public XKMSRegisterRequest {

public: 
	XKMSRequestAbstractTypeImpl m_request;
	XKMSMessageAbstractTypeImpl &m_msg;
public:

	XKMSRegisterRequestImpl(
		const XSECEnv * env
	);

	XKMSRegisterRequestImpl(
		const XSECEnv * env, 
		XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * node
	);

	virtual ~XKMSRegisterRequestImpl();

	// Load elements
	void load();

	// Creation
	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * 
		createBlankRegisterRequest(
		const XMLCh * service,
		const XMLCh * id = NULL);


	/* Getter Interface Methods */
	virtual XKMSPrototypeKeyBinding * getPrototypeKeyBinding(void) const;
	virtual XKMSAuthentication * getAuthentication (void) const;
	virtual DSIGSignature * getProofOfPossessionSignature(void) const;

	/* Setter Interface Methods */

	virtual XKMSPrototypeKeyBinding * addPrototypeKeyBinding(void);
	virtual XKMSAuthentication * addAuthentication(void);
	virtual DSIGSignature * addProofOfPossessionSignature(
		canonicalizationMethod cm = CANON_C14N_NOC,
		signatureMethod	sm = SIGNATURE_DSA,
		hashMethod hm = HASH_SHA1);

	/* Implemented from MessageAbstractType */
	virtual messageType getMessageType(void);

	/* Forced inheritance from XKMSMessageAbstractTypeImpl */
	XKMS_MESSAGEABSTRACTYPE_IMPL_METHODS

	/* Forced inheritance from RequestAbstractType */
	XKMS_REQUESTABSTRACTYPE_IMPL_METHODS


private:

	XKMSAuthenticationImpl		* mp_authentication;
	XKMSPrototypeKeyBindingImpl	* mp_prototypeKeyBinding;
	DSIGSignature				* mp_proofOfPossessionSignature;

	XSECProvider				m_prov;		// For creating the signature

	// Unimplemented
	XKMSRegisterRequestImpl(void);
	XKMSRegisterRequestImpl(const XKMSRegisterRequestImpl &);
	XKMSRegisterRequestImpl & operator = (const XKMSRegisterRequestImpl &);

};

#endif /* XKMSREGISTERREQUESTIMPL_INCLUDE */
