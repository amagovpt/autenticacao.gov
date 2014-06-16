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
 * DSIGKeyExt := An extension type for anything unknown to the library
 *
 * Author(s): Scott Cantor
 *
 * $Id:$
 *
 */

#include <xsec/dsig/DSIGKeyInfoExt.hpp>
#include <xsec/framework/XSECError.hpp>
#include <xsec/framework/XSECEnv.hpp>

XERCES_CPP_NAMESPACE_USE

// --------------------------------------------------------------------------------
//           Constructors and Destructors
// --------------------------------------------------------------------------------


DSIGKeyInfoExt::DSIGKeyInfoExt(const XSECEnv * env, XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *extNode)
: DSIGKeyInfo(env) {

    mp_keyInfoDOMNode = extNode;
}

DSIGKeyInfoExt::~DSIGKeyInfoExt() {}

// --------------------------------------------------------------------------------
//           Load and Get functions
// --------------------------------------------------------------------------------


void DSIGKeyInfoExt::load(void) {

	// Assuming we have a valid DOM_Node to start with, load the extension so that it can
	// be used later on

	if (mp_keyInfoDOMNode == NULL) {

		// Attempt to load an empty signature element
		throw XSECException(XSECException::LoadEmptyInfoName);

	}

}
