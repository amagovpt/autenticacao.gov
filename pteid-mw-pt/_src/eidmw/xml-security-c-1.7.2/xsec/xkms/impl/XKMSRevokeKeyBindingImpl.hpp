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
 * XKMSRevokeKeyBinding := Implementation for RevokeKeyBinding
 *
 * $Id:$
 *
 */

#ifndef XKMSREVOKEKEYBINDINGIMPL_INCLUDE 
#define XKMSREVOKEKEYBINDINGIMPL_INCLUDE 

// XSEC Includes

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/xkms/XKMSRevokeKeyBinding.hpp>
#include <xsec/xkms/XKMSStatus.hpp>

#include "XKMSKeyBindingAbstractTypeImpl.hpp"

class XKMSStatusImpl;

class XKMSRevokeKeyBindingImpl : public XKMSRevokeKeyBinding, public XKMSKeyBindingAbstractTypeImpl {

public:

	XKMSRevokeKeyBindingImpl(
		const XSECEnv * env
	);

	XKMSRevokeKeyBindingImpl(
		const XSECEnv * env, 
		XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * node
	);

	virtual ~XKMSRevokeKeyBindingImpl() ;

	// Load
	void load(void);

	// Create
	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *
		createBlankRevokeKeyBinding(XKMSStatus::StatusValue status);

	// Interface
	virtual XKMSStatus * getStatus(void) const;

	// Import methods from XKMSKeyBindingAbstractType
	XKMS_KEYBINDINGABSTRACTYPE_IMPL_METHODS

private:

	XKMSStatusImpl		* mp_status;

	// Unimplemented
	XKMSRevokeKeyBindingImpl(void);
	XKMSRevokeKeyBindingImpl(const XKMSRevokeKeyBindingImpl &);
	XKMSRevokeKeyBindingImpl & operator = (const XKMSRevokeKeyBindingImpl &);

};

#endif /* XKMSREVOKEKEYBINDINGIMPL_INCLUDE */
