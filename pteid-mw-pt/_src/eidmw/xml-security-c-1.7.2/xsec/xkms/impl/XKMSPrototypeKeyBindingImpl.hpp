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
 * XKMSPrototypeKeyBindingImpl := Implementation of PrototypeKeyBinding elements
 *
 * $Id: XKMSPrototypeKeyBindingImpl.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef XKMSPROTOTYPEKEYBINDINGIMPL_INCLUDE
#define XKMSPROTOTYPEKEYBINDINGIMPL_INCLUDE

// XSEC Includes

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/xkms/XKMSPrototypeKeyBinding.hpp>

#include "XKMSKeyBindingAbstractTypeImpl.hpp"

class XKMSValidityIntervalImpl;

class XKMSPrototypeKeyBindingImpl : public XKMSPrototypeKeyBinding, public XKMSKeyBindingAbstractTypeImpl {

public:

	/* Constructors and Destructors */

	XKMSPrototypeKeyBindingImpl(
		const XSECEnv * env
	);

	XKMSPrototypeKeyBindingImpl(
		const XSECEnv * env, 
		XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * node
	);

	virtual ~XKMSPrototypeKeyBindingImpl();

	// Load
	void load(void);

	// Create
	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *
		createBlankPrototypeKeyBinding(void);

	// Get methods

	virtual XKMSValidityInterval * getValidityInterval(void) const;
	virtual const XMLCh * getRevocationCodeIdentifier(void) const;

	// set methods

	virtual void setValidityInterval(const XMLCh * notBefore, const XMLCh * notOnOrAfter);
	virtual void setRevocationCodeIdentifier(const XMLCh * identifier);

	// Import methods from XKMSKeyBindingAbstractType
	XKMS_KEYBINDINGABSTRACTYPE_IMPL_METHODS

private:

	XKMSValidityIntervalImpl
				* mp_validityInterval;
	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement 
				* mp_revocationCodeIdentifierElement;

	// Unimplemented
	XKMSPrototypeKeyBindingImpl(void);
	XKMSPrototypeKeyBindingImpl(const XKMSPrototypeKeyBindingImpl &);
	XKMSPrototypeKeyBindingImpl & operator = (const XKMSPrototypeKeyBindingImpl &);

};

#endif /* XKMSPROTOTYPEKEYBINDINGIMPL_INCLUDE */
