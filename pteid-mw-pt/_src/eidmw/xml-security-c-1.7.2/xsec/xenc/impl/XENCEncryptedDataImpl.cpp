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
 * XENCEncryptedDataImpl := Implementation for holder object for EncryptedData 
 *
 * $Id: XENCEncryptedDataImpl.cpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#include <xsec/framework/XSECDefs.hpp>

#include "XENCCipherImpl.hpp"
#include "XENCEncryptedDataImpl.hpp"
#include "XENCCipherDataImpl.hpp"

#include <xsec/framework/XSECError.hpp>
#include <xsec/utils/XSECDOMUtils.hpp>

#include <xercesc/util/XMLUniDefs.hpp>

XERCES_CPP_NAMESPACE_USE

// --------------------------------------------------------------------------------
//			UNICODE Strings
// --------------------------------------------------------------------------------

static XMLCh s_EncryptedData[] = {

	chLatin_E,
	chLatin_n,
	chLatin_c,
	chLatin_r,
	chLatin_y,
	chLatin_p,
	chLatin_t,
	chLatin_e,
	chLatin_d,
	chLatin_D,
	chLatin_a,
	chLatin_t,
	chLatin_a,
	chNull,
};

// --------------------------------------------------------------------------------
//			Construct/Destruct
// --------------------------------------------------------------------------------


XENCEncryptedDataImpl::XENCEncryptedDataImpl(const XSECEnv * env) :
XENCEncryptedTypeImpl(env) {
	
}

XENCEncryptedDataImpl::XENCEncryptedDataImpl(const XSECEnv * env, DOMElement * node) :
XENCEncryptedTypeImpl(env, node) {

}

XENCEncryptedDataImpl::~XENCEncryptedDataImpl() {

}

// --------------------------------------------------------------------------------
//			Load
// --------------------------------------------------------------------------------

void XENCEncryptedDataImpl::load(void) {

	if (mp_encryptedTypeElement == NULL) {

		// Attempt to load an empty encryptedData element
		throw XSECException(XSECException::EncryptedTypeError,
			"XENCEncryptedData::load - called on empty DOM");

	}

	if (!strEquals(getXENCLocalName(mp_encryptedTypeElement), s_EncryptedData)) {

		// Attempt to load an empty encryptedData element
		throw XSECException(XSECException::EncryptedTypeError,
			"XENCEncryptedData::load - called on non EncryptedData node");

	}

	// Now call the virtual function we overloaded to get here.
	XENCEncryptedTypeImpl::load();

}
// --------------------------------------------------------------------------------
//			Create from scratch
// --------------------------------------------------------------------------------

DOMElement * XENCEncryptedDataImpl::createBlankEncryptedData(
									XENCCipherData::XENCCipherDataType type, 
									const XMLCh * algorithm,
									const XMLCh * value) {

	return createBlankEncryptedType(s_EncryptedData, type, algorithm, value);

}

// --------------------------------------------------------------------------------
//			Interface Methods
// --------------------------------------------------------------------------------


