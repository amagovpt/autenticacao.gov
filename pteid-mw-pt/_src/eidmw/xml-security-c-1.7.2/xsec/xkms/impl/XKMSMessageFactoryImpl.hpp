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
 * XKMSMessageFactoryImpl := Implementation of the XKMSMessageFactory class
 *
 * $Id: XKMSMessageFactoryImpl.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef XKMSMESSAGEFACTORYIMPL_INCLUDE
#define XKMSMESSAGEFACTORYIMPL_INCLUDE

// XSEC Includes

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/xkms/XKMSMessageFactory.hpp>

class XSECProvider;
class XKMSCompoundRequest;
class XKMSCompoundResult;
class XSECEnv;

class XKMSMessageFactoryImpl : public XKMSMessageFactory {

protected:

	XKMSMessageFactoryImpl(void);

public:

	virtual ~XKMSMessageFactoryImpl();

	/* DOM based construction methods */

	virtual XKMSMessageAbstractType * newMessageFromDOM(
		XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * elt);

	/* Construction from scratch */
	virtual XKMSLocateRequest * createLocateRequest(
		const XMLCh * service,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument * doc,
		const XMLCh * id = NULL);		
	virtual XKMSLocateRequest * createLocateRequest(
		const XMLCh * service,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument **doc,
		const XMLCh * id = NULL);
	virtual XKMSValidateRequest * createValidateRequest(
		const XMLCh * service,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument * doc,
		const XMLCh * id = NULL);		
	virtual XKMSValidateRequest * createValidateRequest(
		const XMLCh * service,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument **doc,
		const XMLCh * id = NULL);
	virtual XKMSCompoundRequest * createCompoundRequest(
		const XMLCh * service,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument * doc,
		const XMLCh * id = NULL);
	virtual XKMSCompoundRequest * createCompoundRequest(
		const XMLCh * service,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument **doc,
		const XMLCh * id = NULL);
	virtual XKMSPendingRequest * createPendingRequest(
		const XMLCh * service,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc,
		const XMLCh * id = NULL);
	virtual XKMSPendingRequest * createPendingRequest(
		const XMLCh * service,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument **doc,
		const XMLCh * id = NULL);
	virtual XKMSStatusRequest * createStatusRequest(
		const XMLCh * service,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument * doc,
		const XMLCh * id = NULL);
	virtual XKMSStatusRequest * createStatusRequest(
		const XMLCh * service,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument **doc,
		const XMLCh * id = NULL);


	virtual XKMSLocateResult * createLocateResult(
		XKMSLocateRequest * request,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument * doc,
		XKMSResultType::ResultMajor rmaj,
		XKMSResultType::ResultMinor rmin = XKMSResultType::NoneMinor,
		const XMLCh * id = NULL);
	virtual XKMSLocateResult * createLocateResult(
		XKMSLocateRequest * request,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument **doc,
		XKMSResultType::ResultMajor rmaj,
		XKMSResultType::ResultMinor rmin = XKMSResultType::NoneMinor,
		const XMLCh * id = NULL);
	virtual XKMSResult * createResult(
		XKMSRequestAbstractType * request,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument * doc,
		XKMSResultType::ResultMajor rmaj,
		XKMSResultType::ResultMinor rmin = XKMSResultType::NoneMinor,
		const XMLCh * id = NULL);
	virtual XKMSResult * createResult(
		XKMSRequestAbstractType * request,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument **doc,
		XKMSResultType::ResultMajor rmaj,
		XKMSResultType::ResultMinor rmin = XKMSResultType::NoneMinor,
		const XMLCh * id = NULL);
	virtual XKMSValidateResult * createValidateResult(
		XKMSValidateRequest * request,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument * doc,
		XKMSResultType::ResultMajor rmaj,
		XKMSResultType::ResultMinor rmin = XKMSResultType::NoneMinor,
		const XMLCh * id = NULL);
	virtual XKMSValidateResult * createValidateResult(
		XKMSValidateRequest * request,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument **doc,
		XKMSResultType::ResultMajor rmaj,
		XKMSResultType::ResultMinor rmin = XKMSResultType::NoneMinor,
		const XMLCh * id = NULL);
	virtual XKMSCompoundResult * createCompoundResult(
		XKMSCompoundRequest * request,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument * doc,
		XKMSResultType::ResultMajor rmaj,
		XKMSResultType::ResultMinor rmin = XKMSResultType::NoneMinor,
		const XMLCh * id = NULL);
	virtual XKMSCompoundResult * createCompoundResult(
		XKMSCompoundRequest * request,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument **doc,
		XKMSResultType::ResultMajor rmaj,
		XKMSResultType::ResultMinor rmin = XKMSResultType::NoneMinor,
		const XMLCh * id = NULL);
	virtual XKMSStatusResult * createStatusResult(
		XKMSStatusRequest * request,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument * doc,
		XKMSResultType::ResultMajor rmaj,
		XKMSResultType::ResultMinor rmin = XKMSResultType::NoneMinor,
		const XMLCh * id = NULL);
	virtual XKMSStatusResult * createStatusResult(
		XKMSStatusRequest * request,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument **doc,
		XKMSResultType::ResultMajor rmaj,
		XKMSResultType::ResultMinor rmin = XKMSResultType::NoneMinor,
		const XMLCh * id = NULL);

	/* X-KRSS Construction from Scratch */

	virtual XKMSRegisterRequest * createRegisterRequest(
		const XMLCh * service,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument * doc,
		const XMLCh * id = NULL);
	virtual XKMSRegisterRequest * createRegisterRequest(
		const XMLCh * service,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument **doc,
		const XMLCh * id = NULL);

	virtual XKMSRegisterResult * createRegisterResult(
		XKMSRegisterRequest * request,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument * doc,
		XKMSResultType::ResultMajor rmaj,
		XKMSResultType::ResultMinor rmin = XKMSResultType::NoneMinor,
		const XMLCh * id = NULL);
	virtual XKMSRegisterResult * createRegisterResult(
		XKMSRegisterRequest * request,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument **doc,
		XKMSResultType::ResultMajor rmaj,
		XKMSResultType::ResultMinor rmin = XKMSResultType::NoneMinor,
		const XMLCh * id = NULL);


	virtual XKMSRevokeRequest * createRevokeRequest(
		const XMLCh * service,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument * doc,
		const XMLCh * id = NULL);
	virtual XKMSRevokeRequest * createRevokeRequest(
		const XMLCh * service,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument **doc,
		const XMLCh * id = NULL);

	virtual XKMSRevokeResult * createRevokeResult(
		XKMSRevokeRequest * request,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument * doc,
		XKMSResultType::ResultMajor rmaj,
		XKMSResultType::ResultMinor rmin = XKMSResultType::NoneMinor,
		const XMLCh * id = NULL);
	virtual XKMSRevokeResult * createRevokeResult(
		XKMSRevokeRequest * request,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument **doc,
		XKMSResultType::ResultMajor rmaj,
		XKMSResultType::ResultMinor rmin = XKMSResultType::NoneMinor,
		const XMLCh * id = NULL);

	virtual XKMSRecoverRequest * createRecoverRequest(
		const XMLCh * service,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument * doc,
		const XMLCh * id = NULL);
	virtual XKMSRecoverRequest * createRecoverRequest(
		const XMLCh * service,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument **doc,
		const XMLCh * id = NULL);

	virtual XKMSRecoverResult * createRecoverResult(
		XKMSRecoverRequest * request,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument * doc,
		XKMSResultType::ResultMajor rmaj,
		XKMSResultType::ResultMinor rmin = XKMSResultType::NoneMinor,
		const XMLCh * id = NULL);
	virtual XKMSRecoverResult * createRecoverResult(
		XKMSRecoverRequest * request,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument **doc,
		XKMSResultType::ResultMajor rmaj,
		XKMSResultType::ResultMinor rmin = XKMSResultType::NoneMinor,
		const XMLCh * id = NULL);

	virtual XKMSReissueRequest * createReissueRequest(
		const XMLCh * service,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument * doc,
		const XMLCh * id = NULL);
	virtual XKMSReissueRequest * createReissueRequest(
		const XMLCh * service,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument **doc,
		const XMLCh * id = NULL);

	virtual XKMSReissueResult * createReissueResult(
		XKMSReissueRequest * request,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument * doc,
		XKMSResultType::ResultMajor rmaj,
		XKMSResultType::ResultMinor rmin = XKMSResultType::NoneMinor,
		const XMLCh * id = NULL);
	virtual XKMSReissueResult * createReissueResult(
		XKMSReissueRequest * request,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument **doc,
		XKMSResultType::ResultMajor rmaj,
		XKMSResultType::ResultMinor rmin = XKMSResultType::NoneMinor,
		const XMLCh * id = NULL);

	/* Conversions */
	virtual XKMSRequestAbstractType * toRequestAbstractType(XKMSMessageAbstractType *msg);
	virtual XKMSResultType * toResultType(XKMSMessageAbstractType *msg);



	/* Environment Manipulation */
	virtual void setDSIGNSPrefix(const XMLCh * prefix);
	virtual void setECNSPrefix(const XMLCh * prefix);
	virtual void setXPFNSPrefix(const XMLCh * prefix);
	virtual void setXENCNSPrefix(const XMLCh * prefix);
	virtual void setXKMSNSPrefix(const XMLCh * prefix);


	friend class XSECProvider;
	friend class XKMSCompoundRequestImpl;
	friend class XKMSCompoundResultImpl;

private:

	// Internal methods
	void copyRequestToResult(XKMSRequestAbstractType * req, XKMSResultType * res);
	// Environment
	XSECEnv					* mp_env;

	// Unimplemented
	XKMSMessageFactoryImpl(const XKMSMessageFactoryImpl &);
	XKMSMessageFactoryImpl & operator = (const XKMSMessageFactoryImpl &);

};

#endif /* XKMSMESSAGEFACTORYIMPL_INCLUDE */
