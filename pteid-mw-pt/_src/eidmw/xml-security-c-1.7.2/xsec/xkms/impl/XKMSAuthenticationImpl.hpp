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
 * XKMSAuthenticationImpl := Implementation of Authentication elements
 *
 * $Id: XKMSAuthenticationImpl.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef XKMSAUTHENTICATIONIMPL_INCLUDE
#define XKMSAUTHENTICATIONIMPL_INCLUDE

// XSEC Includes

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/framework/XSECProvider.hpp>
#include <xsec/xkms/XKMSAuthentication.hpp>

class XKMSKeyBindingAuthenticationImpl;
class XKMSNotBoundAuthenticationImpl;
class XSECEnv;

XSEC_DECLARE_XERCES_CLASS(DOMElement);

class XKMSAuthenticationImpl : public XKMSAuthentication {

public:

	/* Constructors and Destructors */

	XKMSAuthenticationImpl(
		const XSECEnv * env
	);

	XKMSAuthenticationImpl(
		const XSECEnv * env, 
		XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * node
	);

	virtual ~XKMSAuthenticationImpl() ;

	// Load
	// The Authentication load is a bit different to others, as it needs
	// to know the Id of the KeyBinding that it is authentication so that it
	// can check it is valid in the signature

	void load(const XMLCh * id);

	// Create
	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *
		createBlankAuthentication(const XMLCh * id);

	// Interface GET methods

	virtual DSIGSignature * getKeyBindingAuthenticationSignature(void) const;
	virtual XKMSNotBoundAuthentication * getNotBoundAuthentication(void) const;

	// Interface Set methods

	virtual DSIGSignature * addKeyBindingAuthenticationSignature(
		canonicalizationMethod cm = CANON_C14NE_NOC,
		signatureMethod	sm = SIGNATURE_HMAC,
		hashMethod hm = HASH_SHA1);
	virtual void setNotBoundAuthentication(const XMLCh * uri, const XMLCh * value);

private:

	const XSECEnv		* mp_env;

	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement
					* mp_authenticationElement;

	XSECProvider	m_prov;

	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement
					* mp_keyBindingAuthenticationSignatureElement;
	DSIGSignature					* mp_keyBindingAuthenticationSignature;
	XKMSNotBoundAuthenticationImpl	* mp_notBoundAuthentication;

	const XMLCh		* mp_keyBindingId;

	// Unimplemented
	XKMSAuthenticationImpl(void);
	XKMSAuthenticationImpl(const XKMSAuthenticationImpl &);
	XKMSAuthenticationImpl & operator = (const XKMSAuthenticationImpl &);

};

#endif /* XKMSAUTHENTICATIONIMPL_INCLUDE */
