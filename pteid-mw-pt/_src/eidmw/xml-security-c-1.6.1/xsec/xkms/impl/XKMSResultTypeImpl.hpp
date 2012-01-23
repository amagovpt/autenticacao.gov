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
 * XKMSResultTypeImpl := Implementation of base schema of XKMS Request messages
 *
 * $Id: XKMSResultTypeImpl.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef XKMSRESULTTYPEIMPL_INCLUDE
#define XKMSRESULTTYPEIMPL_INCLUDE

// XSEC Includes

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/xkms/XKMSResultType.hpp>

#include "XKMSMessageAbstractTypeImpl.hpp"

XSEC_DECLARE_XERCES_CLASS(DOMElement);
XSEC_DECLARE_XERCES_CLASS(DOMAttr);

class XKMSResultTypeImpl : public XKMSResultType {

public:
	XKMSMessageAbstractTypeImpl m_msg;
public:

	XKMSResultTypeImpl(const XSECEnv * env);
	XKMSResultTypeImpl(
		const XSECEnv * env, 
		XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * node);

	virtual ~XKMSResultTypeImpl();

	// load
	void load(void);

	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * 
		createBlankResultType(
		const XMLCh * tag,
		const XMLCh * service,
		const XMLCh * id,
		ResultMajor rmaj,
		ResultMinor rmin);


	/* Note yet implemented from MessageAbstractType */
	virtual messageType getMessageType(void) {return XKMSMessageAbstractType::None;}
	
	// Getter interface

	virtual ResultMajor getResultMajor(void) const;
	virtual ResultMinor getResultMinor(void) const;
	virtual const XMLCh * getRequestId(void) const;
	virtual const XMLCh * getRequestSignatureValue(void) const;

	// Setter interface
	virtual void setResultMajor(ResultMajor r);
	virtual void setResultMinor(ResultMinor r);
	virtual void setRequestId(const XMLCh * id);
	virtual void setRequestSignatureValue(const XMLCh * value);

	/* Forced inheritance from XKMSMessageAbstractTypeImpl */
	XKMS_MESSAGEABSTRACTYPE_IMPL_METHODS

private:

	XERCES_CPP_NAMESPACE_QUALIFIER  DOMAttr
						* mp_resultMajorAttr;
	XERCES_CPP_NAMESPACE_QUALIFIER  DOMAttr
						* mp_resultMinorAttr;
	XERCES_CPP_NAMESPACE_QUALIFIER  DOMAttr
						* mp_requestIdAttr;
	XERCES_CPP_NAMESPACE_QUALIFIER  DOMElement
						* mp_requestSignatureValueElement;

	XKMSResultType::ResultMajor m_resultMajor;
	XKMSResultType::ResultMinor m_resultMinor;

	// Unimplemented
	XKMSResultTypeImpl();
	XKMSResultTypeImpl(const XKMSResultTypeImpl &);
	XKMSResultTypeImpl & operator = (const XKMSResultTypeImpl &);

};

#define XKMS_RESULTTYPE_IMPL_METHODS \
virtual ResultMajor getResultMajor(void) const \
	{return m_result.getResultMajor();} \
virtual ResultMinor getResultMinor(void) const \
	{return m_result.getResultMinor();} \
virtual const XMLCh * getRequestId(void) const \
	{return m_result.getRequestId();} \
virtual const XMLCh * getRequestSignatureValue(void) const \
	{return m_result.getRequestSignatureValue();} \
virtual void setResultMajor(ResultMajor r) \
	{m_result.setResultMajor(r);} \
virtual void setResultMinor(ResultMinor r) \
	{m_result.setResultMinor(r);} \
virtual void setRequestId(const XMLCh * id) \
	{m_result.setRequestId(id);} \
virtual void setRequestSignatureValue(const XMLCh * value) \
	{m_result.setRequestSignatureValue(value);}



#endif /* XKMSRESULTTYPEIMPL_INCLUDE */
