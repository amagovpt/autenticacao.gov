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
 * $Id: XKMSMessageFactoryImpl.cpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/framework/XSECEnv.hpp>
#include <xsec/framework/XSECError.hpp>
#include <xsec/utils/XSECDOMUtils.hpp>
#include <xsec/xkms/XKMSConstants.hpp>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/Janitor.hpp>

#include "XKMSCompoundRequestImpl.hpp"
#include "XKMSCompoundResultImpl.hpp"
#include "XKMSMessageFactoryImpl.hpp"
#include "XKMSLocateRequestImpl.hpp"
#include "XKMSLocateResultImpl.hpp"
#include "XKMSStatusRequestImpl.hpp"
#include "XKMSStatusResultImpl.hpp"
#include "XKMSResultImpl.hpp"
#include "XKMSValidateRequestImpl.hpp"
#include "XKMSValidateResultImpl.hpp"
#include "XKMSPendingRequestImpl.hpp"
#include "XKMSRegisterRequestImpl.hpp"
#include "XKMSRegisterResultImpl.hpp"
#include "XKMSRevokeResultImpl.hpp"
#include "XKMSRevokeRequestImpl.hpp"
#include "XKMSRecoverResultImpl.hpp"
#include "XKMSRecoverRequestImpl.hpp"
#include "XKMSReissueResultImpl.hpp"
#include "XKMSReissueRequestImpl.hpp"

XERCES_CPP_NAMESPACE_USE

// --------------------------------------------------------------------------------
//           Construct/Destruct
// --------------------------------------------------------------------------------

XKMSMessageFactoryImpl::XKMSMessageFactoryImpl(void) {

	// Factory isn't tied to a particular document

	XSECnew(mp_env, XSECEnv(NULL));
	mp_env->setDSIGNSPrefix(MAKE_UNICODE_STRING("ds"));

};

XKMSMessageFactoryImpl::~XKMSMessageFactoryImpl(void) {

	delete mp_env;

};

// --------------------------------------------------------------------------------
//			Internal methods
// --------------------------------------------------------------------------------

void XKMSMessageFactoryImpl::copyRequestToResult(XKMSRequestAbstractType * req, 
												 XKMSResultType * res) {


	/* Set the requestId */
	res->setRequestId(req->getId());

	/* Copy any Opaque Data */

	int i;
	int sz = req->getOpaqueClientDataSize();
	for (i = 0; i < sz; ++i) {
		res->appendOpaqueClientDataItem(req->getOpaqueClientDataItemStr(i));
	}

	/* Set RequestSignatureValue data if necessary */
	sz = req->getResponseMechanismSize();
	for (i = 0; i < sz; ++i) {
		if (strEquals(req->getResponseMechanismItemStr(i), XKMSConstants::s_tagRequestSignatureValue)) {
			DSIGSignature *s = req->getSignature();
			if (s != NULL && res->getResultMinor() != XKMSResultType::NoAuthentication) {

				res->setRequestSignatureValue(s->getSignatureValue());

			}

			break;
		}
	}

}

// --------------------------------------------------------------------------------
//			Set/get the namespace prefix to be used when creating nodes
// --------------------------------------------------------------------------------

void XKMSMessageFactoryImpl::setDSIGNSPrefix(const XMLCh * prefix) {

	mp_env->setDSIGNSPrefix(prefix);

}
void XKMSMessageFactoryImpl::setECNSPrefix(const XMLCh * prefix) {

	mp_env->setECNSPrefix(prefix);

}
void XKMSMessageFactoryImpl::setXPFNSPrefix(const XMLCh * prefix) {

	mp_env->setXPFNSPrefix(prefix);

}
void XKMSMessageFactoryImpl::setXENCNSPrefix(const XMLCh * prefix) {

	mp_env->setXENCNSPrefix(prefix);

}
void XKMSMessageFactoryImpl::setXKMSNSPrefix(const XMLCh * prefix) {

	mp_env->setXKMSNSPrefix(prefix);

}

// --------------------------------------------------------------------------------
//           DOM Based construction
// --------------------------------------------------------------------------------

XKMSMessageAbstractType * XKMSMessageFactoryImpl::newMessageFromDOM(
						XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * elt) {


	XSECEnv * env;
	XSECnew(env, XSECEnv(*mp_env));

	env->setParentDocument(elt->getOwnerDocument());

	if (elt == NULL) {

		throw XSECException(XSECException::XKMSError,
			"XKMSMessageFactory::newMessageFromDOM - called on empty DOM");

	}

	// See if this is a known element
	const XMLCh * name = getXKMSLocalName(elt);

	if (strEquals(name, XKMSConstants::s_tagCompoundRequest)) {

		// This is a <CompoundRequest> message
		XKMSCompoundRequestImpl * ret;
		XSECnew(ret, XKMSCompoundRequestImpl(env, elt));

		ret->load();

		return (XKMSCompoundRequest*) ret;

	}

	if (strEquals(name, XKMSConstants::s_tagCompoundResult)) {

		// This is a <CompoundResult> message
		XKMSCompoundResultImpl * ret;
		XSECnew(ret, XKMSCompoundResultImpl(env, elt));

		ret->load();

		return (XKMSCompoundRequest*) ret;

	}

	if (strEquals(name, XKMSConstants::s_tagLocateRequest)) {

		// This is a <LocateRequest> message
		XKMSLocateRequestImpl * ret;
		XSECnew(ret, XKMSLocateRequestImpl(env, elt));

		ret->load();

		return (XKMSLocateRequest *) ret;

	}

	if (strEquals(name, XKMSConstants::s_tagValidateRequest)) {

		// This is a <ValidateRequest> message
		XKMSValidateRequestImpl * ret;
		XSECnew(ret, XKMSValidateRequestImpl(env, elt));

		ret->load();

		return (XKMSValidateRequest *) ret;

	}

	else if (strEquals(name, XKMSConstants::s_tagLocateResult)) {

		// This is a <LocateRequest> message
		XKMSLocateResultImpl * ret;
		XSECnew(ret, XKMSLocateResultImpl(env, elt));
		Janitor<XKMSLocateResultImpl> j_ret(ret);

		ret->load();
		
		j_ret.release();
		return (XKMSLocateResult *) ret;

	}

	else if (strEquals(name, XKMSConstants::s_tagValidateResult)) {

		// This is a <LocateRequest> message
		XKMSValidateResultImpl * ret;
		XSECnew(ret, XKMSValidateResultImpl(env, elt));
		Janitor<XKMSValidateResultImpl> j_ret(ret);

		ret->load();
		
		j_ret.release();
		return (XKMSValidateResult *) ret;

	}

	else if (strEquals(name, XKMSConstants::s_tagResult)) {

		// This is a <LocateRequest> message
		XKMSResultImpl * ret;
		XSECnew(ret, XKMSResultImpl(env, elt));
		Janitor<XKMSResultImpl> j_ret(ret);

		ret->load();
		
		j_ret.release();
		return (XKMSResult *) ret;

	}

	else if (strEquals(name, XKMSConstants::s_tagPendingRequest)) {

		// This is a <PendingRequest> message
		XKMSPendingRequestImpl * ret;
		XSECnew(ret, XKMSPendingRequestImpl(env, elt));
		Janitor<XKMSPendingRequestImpl> j_ret(ret);

		ret->load();
		
		j_ret.release();
		return (XKMSPendingRequest *) ret;

	}

	else if (strEquals(name, XKMSConstants::s_tagStatusRequest)) {

		// This is a <StatusRequest> message
		XKMSStatusRequestImpl * ret;
		XSECnew(ret, XKMSStatusRequestImpl(env, elt));
		Janitor<XKMSStatusRequestImpl> j_ret(ret);

		ret->load();
		
		j_ret.release();
		return (XKMSStatusRequest *) ret;

	}

	else if (strEquals(name, XKMSConstants::s_tagStatusResult)) {

		// This is a <StatusRequest> message
		XKMSStatusResultImpl * ret;
		XSECnew(ret, XKMSStatusResultImpl(env, elt));
		Janitor<XKMSStatusResultImpl> j_ret(ret);

		ret->load();
		
		j_ret.release();
		return (XKMSStatusResult *) ret;

	}

	else if (strEquals(name, XKMSConstants::s_tagRegisterRequest)) {

		// This is a <PendingRequest> message
		XKMSRegisterRequestImpl * ret;
		XSECnew(ret, XKMSRegisterRequestImpl(env, elt));
		Janitor<XKMSRegisterRequestImpl> j_ret(ret);

		ret->load();
		
		j_ret.release();
		return (XKMSRegisterRequest *) ret;

	}

	else if (strEquals(name, XKMSConstants::s_tagRegisterResult)) {

		// This is a <RegisterResult> message
		XKMSRegisterResultImpl * ret;
		XSECnew(ret, XKMSRegisterResultImpl(env, elt));
		Janitor<XKMSRegisterResultImpl> j_ret(ret);

		ret->load();
		
		j_ret.release();
		return (XKMSRegisterResult *) ret;

	}
	else if (strEquals(name, XKMSConstants::s_tagRevokeRequest)) {

		// This is a <RevokeRequest> message
		XKMSRevokeRequestImpl * ret;
		XSECnew(ret, XKMSRevokeRequestImpl(env, elt));
		Janitor<XKMSRevokeRequestImpl> j_ret(ret);

		ret->load();
		
		j_ret.release();
		return (XKMSRevokeRequest *) ret;

	}

	else if (strEquals(name, XKMSConstants::s_tagRevokeResult)) {

		// This is a <RevokeResult> message
		XKMSRevokeResultImpl * ret;
		XSECnew(ret, XKMSRevokeResultImpl(env, elt));
		Janitor<XKMSRevokeResultImpl> j_ret(ret);

		ret->load();
		
		j_ret.release();
		return (XKMSRevokeResult *) ret;

	}

	else if (strEquals(name, XKMSConstants::s_tagRecoverRequest)) {

		// This is a <RevokeRequest> message
		XKMSRecoverRequestImpl * ret;
		XSECnew(ret, XKMSRecoverRequestImpl(env, elt));
		Janitor<XKMSRecoverRequestImpl> j_ret(ret);

		ret->load();
		
		j_ret.release();
		return (XKMSRecoverRequest *) ret;

	}

	else if (strEquals(name, XKMSConstants::s_tagRecoverResult)) {

		// This is a <RecoverResult> message
		XKMSRecoverResultImpl * ret;
		XSECnew(ret, XKMSRecoverResultImpl(env, elt));
		Janitor<XKMSRecoverResultImpl> j_ret(ret);

		ret->load();
		
		j_ret.release();
		return (XKMSRecoverResult *) ret;

	}

	else if (strEquals(name, XKMSConstants::s_tagReissueRequest)) {

		// This is a <ReissueRequest> message
		XKMSReissueRequestImpl * ret;
		XSECnew(ret, XKMSReissueRequestImpl(env, elt));
		Janitor<XKMSReissueRequestImpl> j_ret(ret);

		ret->load();
		
		j_ret.release();
		return (XKMSReissueRequest *) ret;

	}

	else if (strEquals(name, XKMSConstants::s_tagReissueResult)) {

		// This is a <RevokeResult> message
		XKMSReissueResultImpl * ret;
		XSECnew(ret, XKMSReissueResultImpl(env, elt));
		Janitor<XKMSReissueResultImpl> j_ret(ret);

		ret->load();
		
		j_ret.release();
		return (XKMSReissueResult *) ret;

	}

	delete env;
	return NULL;

}

// --------------------------------------------------------------------------------
//           Construction from scratch
// --------------------------------------------------------------------------------

XKMSCompoundRequest * XKMSMessageFactoryImpl::createCompoundRequest(
		const XMLCh * service,
		DOMDocument * doc,
		const XMLCh * id) {

	XKMSCompoundRequestImpl * cri;

	XSECEnv * tenv;
	XSECnew(tenv, XSECEnv(*mp_env));
	tenv->setParentDocument(doc);

	XSECnew(cri, XKMSCompoundRequestImpl(tenv));
	cri->createBlankCompoundRequest(service, id);

	return cri;

}

XKMSCompoundRequest * XKMSMessageFactoryImpl::createCompoundRequest(
		const XMLCh * service,
		DOMDocument **doc,
		const XMLCh * id) {

	// Create a document to put the element in

	XMLCh tempStr[100];
	XMLString::transcode("Core", tempStr, 99);    
	DOMImplementation *impl = DOMImplementationRegistry::getDOMImplementation(tempStr);

	*doc = impl->createDocument();

	// Embed the new structure in the document
	XKMSCompoundRequest * cri = createCompoundRequest(service, *doc, id);
	(*doc)->appendChild(cri->getElement());

	return cri;
}
	
XKMSLocateRequest * XKMSMessageFactoryImpl::createLocateRequest(
		const XMLCh * service,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument * doc,
		const XMLCh * id) {
	
	XKMSLocateRequestImpl * lri;

	XSECEnv * tenv;
	XSECnew(tenv, XSECEnv(*mp_env));
	tenv->setParentDocument(doc);

	XSECnew(lri, XKMSLocateRequestImpl(tenv));
	lri->createBlankLocateRequest(service, id);

	return lri;

}


XKMSLocateRequest * XKMSMessageFactoryImpl::createLocateRequest(
		const XMLCh * service,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument **doc,
		const XMLCh * id) {


	// Create a document to put the element in

	XMLCh tempStr[100];
	XMLString::transcode("Core", tempStr, 99);    
	DOMImplementation *impl = DOMImplementationRegistry::getDOMImplementation(tempStr);

	*doc = impl->createDocument();

	// Embed the new structure in the document
	XKMSLocateRequest * lri = createLocateRequest(service, *doc, id);
	(*doc)->appendChild(lri->getElement());

	return lri;
}

XKMSValidateRequest * XKMSMessageFactoryImpl::createValidateRequest(
		const XMLCh * service,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument * doc,
		const XMLCh * id) {
	
	XKMSValidateRequestImpl * vri;

	XSECEnv * tenv;
	XSECnew(tenv, XSECEnv(*mp_env));
	tenv->setParentDocument(doc);

	XSECnew(vri, XKMSValidateRequestImpl(tenv));
	vri->createBlankValidateRequest(service, id);

	return vri;

}


XKMSValidateRequest * XKMSMessageFactoryImpl::createValidateRequest(
		const XMLCh * service,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument **doc,
		const XMLCh * id) {


	// Create a document to put the element in

	XMLCh tempStr[100];
	XMLString::transcode("Core", tempStr, 99);    
	DOMImplementation *impl = DOMImplementationRegistry::getDOMImplementation(tempStr);

	*doc = impl->createDocument();

	// Embed the new structure in the document
	XKMSValidateRequest * vri = createValidateRequest(service, *doc, id);
	(*doc)->appendChild(vri->getElement());

	return vri;
}

XKMSPendingRequest * XKMSMessageFactoryImpl::createPendingRequest(
		const XMLCh * service,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument * doc,
		const XMLCh * id) {
	
	XKMSPendingRequestImpl * pri;

	XSECEnv * tenv;
	XSECnew(tenv, XSECEnv(*mp_env));
	tenv->setParentDocument(doc);

	XSECnew(pri, XKMSPendingRequestImpl(tenv));
	pri->createBlankPendingRequest(service, id);

	return pri;

}


XKMSPendingRequest * XKMSMessageFactoryImpl::createPendingRequest(
		const XMLCh * service,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument **doc,
		const XMLCh * id) {


	// Create a document to put the element in

	XMLCh tempStr[100];
	XMLString::transcode("Core", tempStr, 99);    
	DOMImplementation *impl = DOMImplementationRegistry::getDOMImplementation(tempStr);

	*doc = impl->createDocument();

	// Embed the new structure in the document
	XKMSPendingRequest * pri = createPendingRequest(service, *doc, id);
	(*doc)->appendChild(pri->getElement());

	return pri;
}

XKMSStatusRequest * XKMSMessageFactoryImpl::createStatusRequest(
		const XMLCh * service,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument * doc,
		const XMLCh * id) {
	
	XKMSStatusRequestImpl* sri;

	XSECEnv * tenv;
	XSECnew(tenv, XSECEnv(*mp_env));
	tenv->setParentDocument(doc);

	XSECnew(sri, XKMSStatusRequestImpl(tenv));
	sri->createBlankStatusRequest(service, id);

	return sri;

}

XKMSStatusRequest * XKMSMessageFactoryImpl::createStatusRequest(
		const XMLCh * service,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument **doc,
		const XMLCh * id) {


	// Create a document to put the element in

	XMLCh tempStr[100];
	XMLString::transcode("Core", tempStr, 99);    
	DOMImplementation *impl = DOMImplementationRegistry::getDOMImplementation(tempStr);

	*doc = impl->createDocument();

	// Embed the new structure in the document
	XKMSStatusRequest * sri = createStatusRequest(service, *doc, id);
	(*doc)->appendChild(sri->getElement());

	return sri;
}

XKMSRegisterRequest * XKMSMessageFactoryImpl::createRegisterRequest(
		const XMLCh * service,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument * doc,
		const XMLCh * id) {
	
	XKMSRegisterRequestImpl * rri;

	XSECEnv * tenv;
	XSECnew(tenv, XSECEnv(*mp_env));
	tenv->setParentDocument(doc);

	XSECnew(rri, XKMSRegisterRequestImpl(tenv));
	rri->createBlankRegisterRequest(service, id);

	return rri;

}


XKMSRegisterRequest * XKMSMessageFactoryImpl::createRegisterRequest(
		const XMLCh * service,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument **doc,
		const XMLCh * id) {


	// Create a document to put the element in

	XMLCh tempStr[100];
	XMLString::transcode("Core", tempStr, 99);    
	DOMImplementation *impl = DOMImplementationRegistry::getDOMImplementation(tempStr);

	*doc = impl->createDocument();

	// Embed the new structure in the document
	XKMSRegisterRequest * rri = createRegisterRequest(service, *doc, id);
	(*doc)->appendChild(rri->getElement());

	return rri;
}

XKMSRevokeRequest * XKMSMessageFactoryImpl::createRevokeRequest(
		const XMLCh * service,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument * doc,
		const XMLCh * id) {
	
	XKMSRevokeRequestImpl * rri;

	XSECEnv * tenv;
	XSECnew(tenv, XSECEnv(*mp_env));
	tenv->setParentDocument(doc);

	XSECnew(rri, XKMSRevokeRequestImpl(tenv));
	rri->createBlankRevokeRequest(service, id);

	return rri;

}


XKMSRevokeRequest * XKMSMessageFactoryImpl::createRevokeRequest(
		const XMLCh * service,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument **doc,
		const XMLCh * id) {


	// Create a document to put the element in

	XMLCh tempStr[100];
	XMLString::transcode("Core", tempStr, 99);    
	DOMImplementation *impl = DOMImplementationRegistry::getDOMImplementation(tempStr);

	*doc = impl->createDocument();

	// Embed the new structure in the document
	XKMSRevokeRequest * rri = createRevokeRequest(service, *doc, id);
	(*doc)->appendChild(rri->getElement());

	return rri;
}

XKMSRecoverRequest * XKMSMessageFactoryImpl::createRecoverRequest(
		const XMLCh * service,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument * doc,
		const XMLCh * id) {
	
	XKMSRecoverRequestImpl * rri;

	XSECEnv * tenv;
	XSECnew(tenv, XSECEnv(*mp_env));
	tenv->setParentDocument(doc);

	XSECnew(rri, XKMSRecoverRequestImpl(tenv));
	rri->createBlankRecoverRequest(service, id);

	return rri;

}


XKMSRecoverRequest * XKMSMessageFactoryImpl::createRecoverRequest(
		const XMLCh * service,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument **doc,
		const XMLCh * id) {


	// Create a document to put the element in

	XMLCh tempStr[100];
	XMLString::transcode("Core", tempStr, 99);    
	DOMImplementation *impl = DOMImplementationRegistry::getDOMImplementation(tempStr);

	*doc = impl->createDocument();

	// Embed the new structure in the document
	XKMSRecoverRequest * rri = createRecoverRequest(service, *doc, id);
	(*doc)->appendChild(rri->getElement());

	return rri;
}

XKMSReissueRequest * XKMSMessageFactoryImpl::createReissueRequest(
		const XMLCh * service,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument * doc,
		const XMLCh * id) {
	
	XKMSReissueRequestImpl * rri;

	XSECEnv * tenv;
	XSECnew(tenv, XSECEnv(*mp_env));
	tenv->setParentDocument(doc);

	XSECnew(rri, XKMSReissueRequestImpl(tenv));
	rri->createBlankReissueRequest(service, id);

	return rri;

}


XKMSReissueRequest * XKMSMessageFactoryImpl::createReissueRequest(
		const XMLCh * service,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument **doc,
		const XMLCh * id) {


	// Create a document to put the element in

	XMLCh tempStr[100];
	XMLString::transcode("Core", tempStr, 99);    
	DOMImplementation *impl = DOMImplementationRegistry::getDOMImplementation(tempStr);

	*doc = impl->createDocument();

	// Embed the new structure in the document
	XKMSReissueRequest * rri = createReissueRequest(service, *doc, id);
	(*doc)->appendChild(rri->getElement());

	return rri;
}

// --------------------------------------------------------------------------------
//           Create a result based on a request
// --------------------------------------------------------------------------------

XKMSLocateResult * XKMSMessageFactoryImpl::createLocateResult(
		XKMSLocateRequest * request,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc,
		XKMSResultType::ResultMajor rmaj,
		XKMSResultType::ResultMinor rmin,
		const XMLCh * id) {

	XKMSLocateResultImpl * lri;

	XSECEnv * tenv;
	XSECnew(tenv, XSECEnv(*mp_env));
	tenv->setParentDocument(doc);

	XSECnew(lri, XKMSLocateResultImpl(tenv));
	lri->createBlankLocateResult(request->getService(), id, rmaj, rmin);

	copyRequestToResult(request, (XKMSResultTypeImpl*) lri);

	return lri;

}

XKMSLocateResult * XKMSMessageFactoryImpl::createLocateResult(
		XKMSLocateRequest * request,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument **doc,
		XKMSResultType::ResultMajor rmaj,
		XKMSResultType::ResultMinor rmin,
		const XMLCh * id) {

	// Create a document to put the element in

	XMLCh tempStr[100];
	XMLString::transcode("Core", tempStr, 99);    
	DOMImplementation *impl = DOMImplementationRegistry::getDOMImplementation(tempStr);

	*doc = impl->createDocument();

	// Embed the new structure in the document
	XKMSLocateResult * lr = createLocateResult(request, *doc, rmaj, rmin, id);
	(*doc)->appendChild(lr->getElement());

	return lr;
}

XKMSStatusResult * XKMSMessageFactoryImpl::createStatusResult(
		XKMSStatusRequest * request,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc,
		XKMSResultType::ResultMajor rmaj,
		XKMSResultType::ResultMinor rmin,
		const XMLCh * id) {

	XKMSStatusResultImpl * sri;

	XSECEnv * tenv;
	XSECnew(tenv, XSECEnv(*mp_env));
	tenv->setParentDocument(doc);

	XSECnew(sri, XKMSStatusResultImpl(tenv));
	sri->createBlankStatusResult(request->getService(), id, rmaj, rmin);

	copyRequestToResult(request, (XKMSResultTypeImpl*) sri);

	return sri;

}

XKMSStatusResult * XKMSMessageFactoryImpl::createStatusResult(
		XKMSStatusRequest * request,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument **doc,
		XKMSResultType::ResultMajor rmaj,
		XKMSResultType::ResultMinor rmin,
		const XMLCh * id) {

	// Create a document to put the element in

	XMLCh tempStr[100];
	XMLString::transcode("Core", tempStr, 99);    
	DOMImplementation *impl = DOMImplementationRegistry::getDOMImplementation(tempStr);

	*doc = impl->createDocument();

	// Embed the new structure in the document
	XKMSStatusResult * sr = createStatusResult(request, *doc, rmaj, rmin, id);
	(*doc)->appendChild(sr->getElement());

	return sr;
}

XKMSResult * XKMSMessageFactoryImpl::createResult(
		XKMSRequestAbstractType * request,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc,
		XKMSResultType::ResultMajor rmaj,
		XKMSResultType::ResultMinor rmin,
		const XMLCh * id) {

	XKMSResultImpl * ri;

	XSECEnv * tenv;
	XSECnew(tenv, XSECEnv(*mp_env));
	tenv->setParentDocument(doc);

	XSECnew(ri, XKMSResultImpl(tenv));
	ri->createBlankResult(request->getService(), id, rmaj, rmin);

	copyRequestToResult(request, (XKMSResultTypeImpl*) ri);

	return ri;

}

XKMSResult * XKMSMessageFactoryImpl::createResult(
		XKMSRequestAbstractType * request,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument **doc,
		XKMSResultType::ResultMajor rmaj,
		XKMSResultType::ResultMinor rmin,
		const XMLCh * id) {

	// Create a document to put the element in

	XMLCh tempStr[100];
	XMLString::transcode("Core", tempStr, 99);    
	DOMImplementation *impl = DOMImplementationRegistry::getDOMImplementation(tempStr);

	*doc = impl->createDocument();

	// Embed the new structure in the document
	XKMSResult * r = createResult(request, *doc, rmaj, rmin, id);
	(*doc)->appendChild(r->getElement());

	return r;
}

XKMSValidateResult * XKMSMessageFactoryImpl::createValidateResult(
		XKMSValidateRequest * request,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc,
		XKMSResultType::ResultMajor rmaj,
		XKMSResultType::ResultMinor rmin,
		const XMLCh * id) {

	XKMSValidateResultImpl * vri;

	XSECEnv * tenv;
	XSECnew(tenv, XSECEnv(*mp_env));
	tenv->setParentDocument(doc);

	XSECnew(vri, XKMSValidateResultImpl(tenv));
	vri->createBlankValidateResult(request->getService(), id, rmaj, rmin);

	copyRequestToResult(request, (XKMSResultTypeImpl*) vri);

	return vri;

}

XKMSValidateResult * XKMSMessageFactoryImpl::createValidateResult(
		XKMSValidateRequest * request,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument **doc,
		XKMSResultType::ResultMajor rmaj,
		XKMSResultType::ResultMinor rmin,
		const XMLCh * id) {

	// Create a document to put the element in

	XMLCh tempStr[100];
	XMLString::transcode("Core", tempStr, 99);    
	DOMImplementation *impl = DOMImplementationRegistry::getDOMImplementation(tempStr);

	*doc = impl->createDocument();

	// Embed the new structure in the document
	XKMSValidateResult * vr = createValidateResult(request, *doc, rmaj, rmin, id);
	(*doc)->appendChild(vr->getElement());

	return vr;
}

XKMSCompoundResult * XKMSMessageFactoryImpl::createCompoundResult(
		XKMSCompoundRequest * request,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc,
		XKMSResultType::ResultMajor rmaj,
		XKMSResultType::ResultMinor rmin,
		const XMLCh * id) {

	XKMSCompoundResultImpl * cri;

	XSECEnv * tenv;
	XSECnew(tenv, XSECEnv(*mp_env));
	tenv->setParentDocument(doc);

	XSECnew(cri, XKMSCompoundResultImpl(tenv));
	cri->createBlankCompoundResult(request->getService(), id, rmaj, rmin);

	copyRequestToResult(request, (XKMSResultTypeImpl*) cri);

	return cri;

}

XKMSCompoundResult * XKMSMessageFactoryImpl::createCompoundResult(
		XKMSCompoundRequest * request,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument **doc,
		XKMSResultType::ResultMajor rmaj,
		XKMSResultType::ResultMinor rmin,
		const XMLCh * id) {

	// Create a document to put the element in

	XMLCh tempStr[100];
	XMLString::transcode("Core", tempStr, 99);    
	DOMImplementation *impl = DOMImplementationRegistry::getDOMImplementation(tempStr);

	*doc = impl->createDocument();

	// Embed the new structure in the document
	XKMSCompoundResult * cr = createCompoundResult(request, *doc, rmaj, rmin, id);
	(*doc)->appendChild(cr->getElement());

	return cr;
}

XKMSRegisterResult * XKMSMessageFactoryImpl::createRegisterResult(
		XKMSRegisterRequest * request,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc,
		XKMSResultType::ResultMajor rmaj,
		XKMSResultType::ResultMinor rmin,
		const XMLCh * id) {

	XKMSRegisterResultImpl * rri;

	XSECEnv * tenv;
	XSECnew(tenv, XSECEnv(*mp_env));
	tenv->setParentDocument(doc);

	XSECnew(rri, XKMSRegisterResultImpl(tenv));
	rri->createBlankRegisterResult(request->getService(), id, rmaj, rmin);

	copyRequestToResult(request, (XKMSResultTypeImpl*) rri);

	return rri;

}

XKMSRegisterResult * XKMSMessageFactoryImpl::createRegisterResult(
		XKMSRegisterRequest * request,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument **doc,
		XKMSResultType::ResultMajor rmaj,
		XKMSResultType::ResultMinor rmin,
		const XMLCh * id) {

	// Create a document to put the element in

	XMLCh tempStr[100];
	XMLString::transcode("Core", tempStr, 99);    
	DOMImplementation *impl = DOMImplementationRegistry::getDOMImplementation(tempStr);

	*doc = impl->createDocument();

	// Embed the new structure in the document
	XKMSRegisterResult * rr = createRegisterResult(request, *doc, rmaj, rmin, id);
	(*doc)->appendChild(rr->getElement());

	return rr;
}


XKMSRevokeResult * XKMSMessageFactoryImpl::createRevokeResult(
		XKMSRevokeRequest * request,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc,
		XKMSResultType::ResultMajor rmaj,
		XKMSResultType::ResultMinor rmin,
		const XMLCh * id) {

	XKMSRevokeResultImpl * rri;

	XSECEnv * tenv;
	XSECnew(tenv, XSECEnv(*mp_env));
	tenv->setParentDocument(doc);

	XSECnew(rri, XKMSRevokeResultImpl(tenv));
	rri->createBlankRevokeResult(request->getService(), id, rmaj, rmin);

	copyRequestToResult(request, (XKMSResultTypeImpl*) rri);

	return rri; 

}


XKMSRevokeResult * XKMSMessageFactoryImpl::createRevokeResult(
		XKMSRevokeRequest * request,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument **doc,
		XKMSResultType::ResultMajor rmaj,
		XKMSResultType::ResultMinor rmin,
		const XMLCh * id) {

	// Create a document to put the element in

	XMLCh tempStr[100];
	XMLString::transcode("Core", tempStr, 99);    
	DOMImplementation *impl = DOMImplementationRegistry::getDOMImplementation(tempStr);

	*doc = impl->createDocument();

	// Embed the new structure in the document
	XKMSRevokeResult * rr = createRevokeResult(request, *doc, rmaj, rmin, id);
	(*doc)->appendChild(rr->getElement());

	return rr;
}

XKMSReissueResult * XKMSMessageFactoryImpl::createReissueResult(
		XKMSReissueRequest * request,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc,
		XKMSResultType::ResultMajor rmaj,
		XKMSResultType::ResultMinor rmin,
		const XMLCh * id) {

	XKMSReissueResultImpl * rri;

	XSECEnv * tenv;
	XSECnew(tenv, XSECEnv(*mp_env));
	tenv->setParentDocument(doc);

	XSECnew(rri, XKMSReissueResultImpl(tenv));
	rri->createBlankReissueResult(request->getService(), id, rmaj, rmin);

	copyRequestToResult(request, (XKMSResultTypeImpl*) rri);

	return rri; 

}

XKMSReissueResult * XKMSMessageFactoryImpl::createReissueResult(
		XKMSReissueRequest * request,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument **doc,
		XKMSResultType::ResultMajor rmaj,
		XKMSResultType::ResultMinor rmin,
		const XMLCh * id) {

	// Create a document to put the element in

	XMLCh tempStr[100];
	XMLString::transcode("Core", tempStr, 99);    
	DOMImplementation *impl = DOMImplementationRegistry::getDOMImplementation(tempStr);

	*doc = impl->createDocument();

	// Embed the new structure in the document
	XKMSReissueResult * rr = createReissueResult(request, *doc, rmaj, rmin, id);
	(*doc)->appendChild(rr->getElement());

	return rr;
}

XKMSRecoverResult * XKMSMessageFactoryImpl::createRecoverResult(
		XKMSRecoverRequest * request,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc,
		XKMSResultType::ResultMajor rmaj,
		XKMSResultType::ResultMinor rmin,
		const XMLCh * id) {

	XKMSRecoverResultImpl * rri;

	XSECEnv * tenv;
	XSECnew(tenv, XSECEnv(*mp_env));
	tenv->setParentDocument(doc);

	XSECnew(rri, XKMSRecoverResultImpl(tenv));
	rri->createBlankRecoverResult(request->getService(), id, rmaj, rmin);

	copyRequestToResult(request, (XKMSResultTypeImpl*) rri);

	return rri; 

}

XKMSRecoverResult * XKMSMessageFactoryImpl::createRecoverResult(
		XKMSRecoverRequest * request,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument **doc,
		XKMSResultType::ResultMajor rmaj,
		XKMSResultType::ResultMinor rmin,
		const XMLCh * id) {

	// Create a document to put the element in

	XMLCh tempStr[100];
	XMLString::transcode("Core", tempStr, 99);    
	DOMImplementation *impl = DOMImplementationRegistry::getDOMImplementation(tempStr);

	*doc = impl->createDocument();

	// Embed the new structure in the document
	XKMSRecoverResult * rr = createRecoverResult(request, *doc, rmaj, rmin, id);
	(*doc)->appendChild(rr->getElement());

	return rr;
}

// --------------------------------------------------------------------------------
//           Message Conversions
// --------------------------------------------------------------------------------


XKMSRequestAbstractType * XKMSMessageFactoryImpl::toRequestAbstractType(XKMSMessageAbstractType *msg) {


	switch (msg->getMessageType()) {

	case XKMSMessageAbstractType::LocateRequest :
	case XKMSMessageAbstractType::ValidateRequest :
	case XKMSMessageAbstractType::CompoundRequest :
	case XKMSMessageAbstractType::PendingRequest :
	case XKMSMessageAbstractType::RegisterRequest :
	case XKMSMessageAbstractType::RevokeRequest :
	case XKMSMessageAbstractType::RecoverRequest :
	case XKMSMessageAbstractType::ReissueRequest :
	case XKMSMessageAbstractType::StatusRequest :

		return (XKMSRequestAbstractType *) msg;

	default:

		return NULL;
	}

	return NULL;

}

XKMSResultType * XKMSMessageFactoryImpl::toResultType(XKMSMessageAbstractType *msg) {

	switch (msg->getMessageType()) {

	case XKMSMessageAbstractType::LocateResult :
	case XKMSMessageAbstractType::ValidateResult :
	case XKMSMessageAbstractType::CompoundResult :
	case XKMSMessageAbstractType::RegisterResult :
	case XKMSMessageAbstractType::RevokeResult :
	case XKMSMessageAbstractType::RecoverResult :
	case XKMSMessageAbstractType::ReissueResult :
	case XKMSMessageAbstractType::StatusResult :
	case XKMSMessageAbstractType::Result :

		return (XKMSResultType *) msg;

	default:
		return NULL;
	}
	return NULL;
}
