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
 * $Id: XKMSCompoundResultImpl.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef XKMSCOMPOUNDRESULTIMPL_INCLUDE
#define XKMSCOMPOUNDRESULTIMPL_INCLUDE

// XSEC Includes

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/xkms/XKMSCompoundResult.hpp>

#include "XKMSResultTypeImpl.hpp"
#include "XKMSMessageFactoryImpl.hpp"

class XKMSCompoundResultImpl : public XKMSCompoundResult {

public:

	XKMSResultTypeImpl m_result;
	XKMSMessageAbstractTypeImpl &m_msg;

public:

	XKMSCompoundResultImpl(
		const XSECEnv * env
	);

	XKMSCompoundResultImpl(
		const XSECEnv * env, 
		XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * node
	);

	virtual ~XKMSCompoundResultImpl();

	// Load elements
	void load();

	// Creation
	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * 
		createBlankCompoundResult(
		const XMLCh * service,
		const XMLCh * id,
		ResultMajor rmaj,
		ResultMinor rmin);

	// Getter methods

	virtual int getResultListSize(void);
	virtual XKMSResultType * getResultListItem(int item);

	// Setter Methods

	virtual XKMSLocateResult * createLocateResult(
		XKMSLocateRequest * request,
		ResultMajor rmaj,
		ResultMinor rmin = XKMSResultTypeImpl::NoneMinor,
		const XMLCh * id = NULL);
	virtual XKMSValidateResult * createValidateResult(
		XKMSValidateRequest * request,
		ResultMajor rmaj,
		ResultMinor rmin = XKMSResultTypeImpl::NoneMinor,
		const XMLCh * id = NULL);
	virtual XKMSStatusResult * createStatusResult(
		XKMSStatusRequest * request,
		ResultMajor rmaj,
		ResultMinor rmin = XKMSResultTypeImpl::NoneMinor,
		const XMLCh * id = NULL);
	virtual XKMSResult * createResult(
		XKMSRequestAbstractType * request,
		ResultMajor rmaj,
		ResultMinor rmin = XKMSResultTypeImpl::NoneMinor,
		const XMLCh * id = NULL);
	virtual XKMSRegisterResult * createRegisterResult(
		XKMSRegisterRequest * request,
		ResultMajor rmaj,
		ResultMinor rmin = XKMSResultTypeImpl::NoneMinor,
		const XMLCh * id = NULL);
	virtual XKMSRevokeResult * createRevokeResult(
		XKMSRevokeRequest * request,
		ResultMajor rmaj,
		ResultMinor rmin = XKMSResultTypeImpl::NoneMinor,
		const XMLCh * id = NULL);
	virtual XKMSRecoverResult * createRecoverResult(
		XKMSRecoverRequest * request,
		ResultMajor rmaj,
		ResultMinor rmin = XKMSResultTypeImpl::NoneMinor,
		const XMLCh * id = NULL);
	virtual XKMSReissueResult * createReissueResult(
		XKMSReissueRequest * request,
		ResultMajor rmaj,
		ResultMinor rmin = XKMSResultTypeImpl::NoneMinor,
		const XMLCh * id = NULL);

	/* Implemented from MessageAbstractType */
	virtual messageType getMessageType(void);

	/* Forced inheritance from XKMSMessageAbstractTypeImpl */
	XKMS_MESSAGEABSTRACTYPE_IMPL_METHODS

	/* Forced inheritance from XKMSResultTypeImpl */
	XKMS_RESULTTYPE_IMPL_METHODS

private:

#if defined(XSEC_NO_NAMESPACES)
	typedef vector<XKMSResultTypeImpl *>		ResultListVectorType;
#else
	typedef std::vector<XKMSResultTypeImpl *>	ResultListVectorType;
#endif

	ResultListVectorType	m_resultList;

	/* Used to consume and produce messages in the list */
	XKMSMessageFactoryImpl	m_factory;

	// Unimplemented
	XKMSCompoundResultImpl(const XKMSCompoundResultImpl &);
	XKMSCompoundResultImpl & operator = (const XKMSCompoundResultImpl &);

};

#endif /* XKMSCOMPOUNDRESULTIMPL_INCLUDE */
