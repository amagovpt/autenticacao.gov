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
 * XKMSNotBoundAuthenticationImpl := Implementation for NotBoundAuthentication elements
 *
 * $Id: XKMSNotBoundAuthentication.cpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/framework/XSECEnv.hpp>
#include <xsec/framework/XSECError.hpp>
#include <xsec/utils/XSECDOMUtils.hpp>
#include <xsec/xkms/XKMSConstants.hpp>

#include "XKMSNotBoundAuthenticationImpl.hpp"

#include <xercesc/dom/DOM.hpp>

XERCES_CPP_NAMESPACE_USE

// --------------------------------------------------------------------------------
//           Construct/Destruct
// --------------------------------------------------------------------------------

XKMSNotBoundAuthenticationImpl::XKMSNotBoundAuthenticationImpl(const XSECEnv * env) :
mp_env(env),
mp_notBoundAuthenticationElement(NULL),
mp_protocolAttr(NULL),
mp_valueAttr(NULL)
{

}

XKMSNotBoundAuthenticationImpl::XKMSNotBoundAuthenticationImpl(const XSECEnv * env, 
															   DOMElement * node) :
mp_env(env),
mp_notBoundAuthenticationElement(node),
mp_protocolAttr(NULL),
mp_valueAttr(NULL)
{

}

XKMSNotBoundAuthenticationImpl::~XKMSNotBoundAuthenticationImpl() {

}

// --------------------------------------------------------------------------------
//           Load
// --------------------------------------------------------------------------------

void XKMSNotBoundAuthenticationImpl::load(void) {

	if (mp_notBoundAuthenticationElement == NULL) {
		throw XSECException(XSECException::ExpectedXKMSChildNotFound,
			"XKMSNotBoundAuthenticationImpl::load - called on empty DOM");
	}

	mp_protocolAttr = 
		mp_notBoundAuthenticationElement->getAttributeNodeNS(NULL, XKMSConstants::s_tagProtocol);
	mp_valueAttr = 
		mp_notBoundAuthenticationElement->getAttributeNodeNS(NULL, XKMSConstants::s_tagValue);

	// Protocol and Value *must* be set

	if (mp_protocolAttr == NULL) {
		throw XSECException(XSECException::ExpectedXKMSChildNotFound,
			"XKMSNotBoundAuthentication::load - Protocol attribute not found");
	}
	if (mp_valueAttr == NULL) {
		throw XSECException(XSECException::ExpectedXKMSChildNotFound,
			"XKMSNotBoundAuthentication::load - Value attribute not found");
	}

}

// --------------------------------------------------------------------------------
//           Create
// --------------------------------------------------------------------------------

DOMElement * XKMSNotBoundAuthenticationImpl::createBlankNotBoundAuthentication(
		const XMLCh * protocol, const XMLCh * value) {

	// Get some setup values
	safeBuffer str;
	DOMDocument *doc = mp_env->getParentDocument();
	const XMLCh * prefix = mp_env->getXKMSNSPrefix();

	makeQName(str, prefix, XKMSConstants::s_tagNotBoundAuthentication);

	mp_notBoundAuthenticationElement = 
		doc->createElementNS(XKMSConstants::s_unicodeStrURIXKMS, 
												str.rawXMLChBuffer());


	// Set URI and value
	mp_notBoundAuthenticationElement->setAttributeNS(NULL, 
		XKMSConstants::s_tagProtocol, 
		protocol);
	mp_notBoundAuthenticationElement->setAttributeNS(NULL, 
		XKMSConstants::s_tagValue, 
		value);

	mp_env->doPrettyPrint(mp_notBoundAuthenticationElement);

	mp_protocolAttr = 
		mp_notBoundAuthenticationElement->getAttributeNodeNS(NULL, XKMSConstants::s_tagProtocol);
	mp_valueAttr = 
		mp_notBoundAuthenticationElement->getAttributeNodeNS(NULL, XKMSConstants::s_tagValue);

	return mp_notBoundAuthenticationElement;
}
	
// --------------------------------------------------------------------------------
//           Get interfaces
// --------------------------------------------------------------------------------

const XMLCh * XKMSNotBoundAuthenticationImpl::getProtocol(void) const {

	if (mp_protocolAttr == NULL) {
		throw XSECException(XSECException::ExpectedXKMSChildNotFound,
			"XKMSNotBoundAuthenticationImpl::getProtocol- called on empty DOM");
	}

	return mp_protocolAttr->getNodeValue();

}


const XMLCh * XKMSNotBoundAuthenticationImpl::getValue(void) const {

	if (mp_valueAttr == NULL) {
		throw XSECException(XSECException::ExpectedXKMSChildNotFound,
			"XKMSNotBoundAuthenticationImpl::getValue- called on empty DOM");
	}

	return mp_valueAttr->getNodeValue();
}
// --------------------------------------------------------------------------------
//           Set Interfaces
// --------------------------------------------------------------------------------

void XKMSNotBoundAuthenticationImpl::setProtocol(const XMLCh * uri) {

	mp_notBoundAuthenticationElement->setAttributeNS(NULL, 
		XKMSConstants::s_tagProtocol, 
		uri);
	mp_protocolAttr = 
		mp_notBoundAuthenticationElement->getAttributeNodeNS(NULL, XKMSConstants::s_tagProtocol);

}

void XKMSNotBoundAuthenticationImpl::setValue(const XMLCh * value) {

	mp_notBoundAuthenticationElement->setAttributeNS(NULL, 
		XKMSConstants::s_tagValue, 
		value);
	mp_valueAttr = 
		mp_notBoundAuthenticationElement->getAttributeNodeNS(NULL, XKMSConstants::s_tagValue);
}

