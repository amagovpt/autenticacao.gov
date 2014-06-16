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
 * XKMSCompoundRequestImpl := Implementation of CompoundRequest Messages
 *
 * $Id: XKMSCompoundRequestImpl.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef XKMSCOMPOUNDREQUESTIMPL_INCLUDE
#define XKMSCOMPOUNDREQUESTIMPL_INCLUDE

// XSEC Includes

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/xkms/XKMSCompoundRequest.hpp>

#include "XKMSRequestAbstractTypeImpl.hpp"
#include "XKMSMessageFactoryImpl.hpp"

class XKMSQueryKeyRequestImpl;

class XKMSCompoundRequestImpl : public XKMSCompoundRequest {

public:

	XKMSRequestAbstractTypeImpl m_request;
	XKMSMessageAbstractTypeImpl &m_msg;

public:

	XKMSCompoundRequestImpl(
		const XSECEnv * env
	);

	XKMSCompoundRequestImpl(
		const XSECEnv * env, 
		XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * node
	);

	virtual ~XKMSCompoundRequestImpl();

	// Load elements
	void load();

	// Creation
	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * 
		createBlankCompoundRequest(
		const XMLCh * service,
		const XMLCh * id = NULL);

	// Getter methods

	virtual int getRequestListSize(void);
	virtual XKMSRequestAbstractType * getRequestListItem(int item);

	/* Implemented from MessageAbstractType */
	virtual messageType getMessageType(void);

	// Setter Methods

	virtual XKMSLocateRequest * createLocateRequest(
		const XMLCh * service,
		const XMLCh * id = NULL);

	virtual XKMSValidateRequest * createValidateRequest(
		const XMLCh * service,
		const XMLCh * id = NULL);
	virtual XKMSRegisterRequest * createRegisterRequest(
		const XMLCh * service,
		const XMLCh * id = NULL);
	virtual XKMSRevokeRequest * createRevokeRequest(
		const XMLCh * service,
		const XMLCh * id = NULL);
	virtual XKMSRecoverRequest * createRecoverRequest(
		const XMLCh * service,
		const XMLCh * id = NULL);
	virtual XKMSReissueRequest * createReissueRequest(
		const XMLCh * service,
		const XMLCh * id = NULL);

	/* Forced inheritance from XKMSMessageAbstractTypeImpl */
	XKMS_MESSAGEABSTRACTYPE_IMPL_METHODS 

	/* Forced inheritance from RequestAbstractType */
	XKMS_REQUESTABSTRACTYPE_IMPL_METHODS

private:

#if defined(XSEC_NO_NAMESPACES)
	typedef vector<XKMSRequestAbstractTypeImpl *>		RequestListVectorType;
#else
	typedef std::vector<XKMSRequestAbstractTypeImpl *>	RequestListVectorType;
#endif

	RequestListVectorType	m_requestList;

	/* Used to consume and produce messages in the list */
	XKMSMessageFactoryImpl	m_factory;

	// Unimplemented
	XKMSCompoundRequestImpl(const XKMSCompoundRequestImpl &);
	XKMSCompoundRequestImpl & operator = (const XKMSCompoundRequestImpl &);

};

#endif /* XKMSCOMPOUNDREQUESTIMPL_INCLUDE */
