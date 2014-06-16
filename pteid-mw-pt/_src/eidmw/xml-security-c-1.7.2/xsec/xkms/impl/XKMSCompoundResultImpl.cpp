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
 * XKMSCompoundResultImpl := Implementation of CompoundResult Messages
 *
 * $Id: XKMSCompoundResultImpl.cpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/framework/XSECError.hpp>
#include <xsec/framework/XSECEnv.hpp>
#include <xsec/utils/XSECDOMUtils.hpp>
#include <xsec/xkms/XKMSConstants.hpp>

#include "XKMSCompoundResultImpl.hpp"
#include "XKMSLocateResultImpl.hpp"
#include "XKMSStatusResultImpl.hpp"
#include "XKMSValidateResultImpl.hpp"
#include "XKMSRegisterResultImpl.hpp"
#include "XKMSRevokeResultImpl.hpp"
#include "XKMSReissueResultImpl.hpp"
#include "XKMSRecoverResultImpl.hpp"
#include "XKMSResultImpl.hpp"

#include <xercesc/dom/DOM.hpp>

XERCES_CPP_NAMESPACE_USE

// --------------------------------------------------------------------------------
//           Construct/Destruct
// --------------------------------------------------------------------------------

XKMSCompoundResultImpl::XKMSCompoundResultImpl(
		const XSECEnv * env) :
m_result(env),
m_msg(m_result.m_msg) {

}

XKMSCompoundResultImpl::XKMSCompoundResultImpl(
		const XSECEnv * env, 
		XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * node) :
m_result(env, node),
m_msg(m_result.m_msg){

}

XKMSCompoundResultImpl::~XKMSCompoundResultImpl() {

	ResultListVectorType::iterator i;

	for (i = m_resultList.begin() ; i != m_resultList.end(); ++i) {

		delete (*i);

	}


}


// --------------------------------------------------------------------------------
//           Load from DOM
// --------------------------------------------------------------------------------

// Load elements
void XKMSCompoundResultImpl::load() {

	if (m_msg.mp_messageAbstractTypeElement == NULL) {

		// Attempt to load an empty element
		throw XSECException(XSECException::XKMSError,
			"XKMSCompoundResult::load - called on empty DOM");

	}

	if (!strEquals(getXKMSLocalName(m_msg.mp_messageAbstractTypeElement), 
									XKMSConstants::s_tagCompoundResult)) {
	
		throw XSECException(XSECException::XKMSError,
			"XKMSCompoundResult::load - called incorrect node");
	
	}

	// Load the base message
	m_result.load();

	// Now find all Result elements
	DOMElement * e = findFirstElementChild(m_msg.mp_messageAbstractTypeElement);

	while (e != NULL) {

		if (strEquals(getXKMSLocalName(e), XKMSConstants::s_tagLocateResult) ||
			strEquals(getXKMSLocalName(e), XKMSConstants::s_tagValidateResult)) {

			// Have a legitimate Result to load
			XKMSMessageAbstractTypeImpl * m = 
				(XKMSMessageAbstractTypeImpl *) m_factory.newMessageFromDOM(e);
			m_resultList.push_back((XKMSResultTypeImpl *) m);

		}

		e = findNextElementChild(e);

	}
	

}

// --------------------------------------------------------------------------------
//           Create a blank one
// --------------------------------------------------------------------------------
DOMElement * XKMSCompoundResultImpl::createBlankCompoundResult(
		const XMLCh * service,
		const XMLCh * id,
		ResultMajor rmaj,
		ResultMinor rmin) {

	return m_result.createBlankResultType(
		XKMSConstants::s_tagCompoundResult, service, id, rmaj, rmin);

}

// --------------------------------------------------------------------------------
//           Get interface methods
// --------------------------------------------------------------------------------

XKMSMessageAbstractType::messageType XKMSCompoundResultImpl::getMessageType(void) {

	return XKMSMessageAbstractTypeImpl::CompoundResult;

}

int XKMSCompoundResultImpl::getResultListSize(void) {

	return (int) m_resultList.size();

}

XKMSResultType * XKMSCompoundResultImpl::getResultListItem(int item) {

	if (item < 0 || item >= (int) m_resultList.size()) {

		throw XSECException(XSECException::XKMSError,
			"XKMSCompoundResult::getResultListItem - item out of range");
	}

	return m_resultList[item];


}

// --------------------------------------------------------------------------------
//           Setter methods
// --------------------------------------------------------------------------------

XKMSLocateResult * XKMSCompoundResultImpl::createLocateResult(
		XKMSLocateRequest * request,
		ResultMajor rmaj,
		ResultMinor rmin,
		const XMLCh * id) {

	XKMSLocateResult * r = m_factory.createLocateResult(request, m_msg.mp_env->getParentDocument(), rmaj, rmin, id);
	m_resultList.push_back((XKMSResultTypeImpl*) r);

	m_msg.mp_messageAbstractTypeElement->appendChild(r->getElement());
	m_msg.mp_env->doPrettyPrint(m_msg.mp_messageAbstractTypeElement);

	return r;

}

XKMSValidateResult * XKMSCompoundResultImpl::createValidateResult(
		XKMSValidateRequest * request,
		ResultMajor rmaj,
		ResultMinor rmin,
		const XMLCh * id) {

	XKMSValidateResult * r = m_factory.createValidateResult(request, m_msg.mp_env->getParentDocument(), rmaj, rmin, id);
	m_resultList.push_back((XKMSResultTypeImpl*) r);

	m_msg.mp_messageAbstractTypeElement->appendChild(r->getElement());
	m_msg.mp_env->doPrettyPrint(m_msg.mp_messageAbstractTypeElement);

	return r;

}

XKMSStatusResult * XKMSCompoundResultImpl::createStatusResult(
		XKMSStatusRequest * request,
		ResultMajor rmaj,
		ResultMinor rmin,
		const XMLCh * id) {

	XKMSStatusResult * r = m_factory.createStatusResult(request, m_msg.mp_env->getParentDocument(), rmaj, rmin, id);
	m_resultList.push_back((XKMSResultTypeImpl*) r);

	m_msg.mp_messageAbstractTypeElement->appendChild(r->getElement());
	m_msg.mp_env->doPrettyPrint(m_msg.mp_messageAbstractTypeElement);

	return r;

}

XKMSRegisterResult * XKMSCompoundResultImpl::createRegisterResult(
		XKMSRegisterRequest * request,
		ResultMajor rmaj,
		ResultMinor rmin,
		const XMLCh * id) {

	XKMSRegisterResult * r = m_factory.createRegisterResult(request, m_msg.mp_env->getParentDocument(), rmaj, rmin, id);
	m_resultList.push_back((XKMSResultTypeImpl*) r);

	m_msg.mp_messageAbstractTypeElement->appendChild(r->getElement());
	m_msg.mp_env->doPrettyPrint(m_msg.mp_messageAbstractTypeElement);

	return r;

}

XKMSRevokeResult * XKMSCompoundResultImpl::createRevokeResult(
		XKMSRevokeRequest * request,
		ResultMajor rmaj,
		ResultMinor rmin,
		const XMLCh * id) {

	XKMSRevokeResult * r = m_factory.createRevokeResult(request, m_msg.mp_env->getParentDocument(), rmaj, rmin, id);
	m_resultList.push_back((XKMSResultTypeImpl*) r);

	m_msg.mp_messageAbstractTypeElement->appendChild(r->getElement());
	m_msg.mp_env->doPrettyPrint(m_msg.mp_messageAbstractTypeElement);

	return r;

}

XKMSRecoverResult * XKMSCompoundResultImpl::createRecoverResult(
		XKMSRecoverRequest * request,
		ResultMajor rmaj,
		ResultMinor rmin,
		const XMLCh * id) {

	XKMSRecoverResult * r = m_factory.createRecoverResult(request, m_msg.mp_env->getParentDocument(), rmaj, rmin, id);
	m_resultList.push_back((XKMSResultTypeImpl*) r);

	m_msg.mp_messageAbstractTypeElement->appendChild(r->getElement());
	m_msg.mp_env->doPrettyPrint(m_msg.mp_messageAbstractTypeElement);

	return r;

}

XKMSReissueResult * XKMSCompoundResultImpl::createReissueResult(
		XKMSReissueRequest * request,
		ResultMajor rmaj,
		ResultMinor rmin,
		const XMLCh * id) {

	XKMSReissueResult * r = m_factory.createReissueResult(request, m_msg.mp_env->getParentDocument(), rmaj, rmin, id);
	m_resultList.push_back((XKMSResultTypeImpl*) r);

	m_msg.mp_messageAbstractTypeElement->appendChild(r->getElement());
	m_msg.mp_env->doPrettyPrint(m_msg.mp_messageAbstractTypeElement);

	return r;

}


XKMSResult * XKMSCompoundResultImpl::createResult(
		XKMSRequestAbstractType * request,
		ResultMajor rmaj,
		ResultMinor rmin,
		const XMLCh * id) {

	XKMSResult * r = m_factory.createResult(request, m_msg.mp_env->getParentDocument(), rmaj, rmin, id);
	m_resultList.push_back((XKMSResultTypeImpl*) r);

	m_msg.mp_messageAbstractTypeElement->appendChild(r->getElement());
	m_msg.mp_env->doPrettyPrint(m_msg.mp_messageAbstractTypeElement);

	return r;

}

