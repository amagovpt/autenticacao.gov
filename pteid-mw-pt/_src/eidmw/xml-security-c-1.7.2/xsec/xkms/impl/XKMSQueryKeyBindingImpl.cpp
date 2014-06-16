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
 * XKMSQueryKeyBindingImpl := Implementation for QueryKeyBinding
 *
 * $Id: XKMSQueryKeyBindingImpl.cpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

// XSEC Includes

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/framework/XSECError.hpp>
#include <xsec/xkms/XKMSConstants.hpp>

#include <xercesc/dom/DOM.hpp>

#include "XKMSQueryKeyBindingImpl.hpp"

XERCES_CPP_NAMESPACE_USE

// --------------------------------------------------------------------------------
//           Construct/Destruct
// --------------------------------------------------------------------------------

XKMSQueryKeyBindingImpl::XKMSQueryKeyBindingImpl(
		const XSECEnv * env 
		) :
XKMSKeyBindingAbstractTypeImpl(env) {

}

XKMSQueryKeyBindingImpl::XKMSQueryKeyBindingImpl(
		const XSECEnv * env, 
		XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * node
		) :
XKMSKeyBindingAbstractTypeImpl(env, node) {

}

XKMSQueryKeyBindingImpl::~XKMSQueryKeyBindingImpl() {}

// --------------------------------------------------------------------------------
//           Load from DOM
// --------------------------------------------------------------------------------

void XKMSQueryKeyBindingImpl::load(void) {

	if (mp_keyBindingAbstractTypeElement == NULL) {
		throw XSECException(XSECException::ExpectedXKMSChildNotFound,
			"XKMSQueryKeyBindingImpl::load - called on empty DOM");
	}

	XKMSKeyBindingAbstractTypeImpl::load();

}

// --------------------------------------------------------------------------------
//           Create
// --------------------------------------------------------------------------------

DOMElement * XKMSQueryKeyBindingImpl::createBlankQueryKeyBinding(void) {

	return XKMSKeyBindingAbstractTypeImpl::
				createBlankKeyBindingAbstractType(XKMSConstants::s_tagQueryKeyBinding);

}

