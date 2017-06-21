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
 * XKMSMessageAbstractTypeImpl := Implementation class for base XKMS messages
 *
 * $Id: XKMSMessageAbstractTypeImpl.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef XKMSMESSAGEABSTRACTTYPEIMPL_INCLUDE
#define XKMSMESSAGEABSTRACTTYPEIMPL_INCLUDE

// XSEC Includes

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/xkms/XKMSMessageAbstractType.hpp>
#include <xsec/framework/XSECProvider.hpp>

class XSECEnv;
class DSIGSignature;

XSEC_DECLARE_XERCES_CLASS(DOMElement);
XSEC_DECLARE_XERCES_CLASS(DOMAttr);

class XKMSMessageAbstractTypeImpl : public XKMSMessageAbstractType {

public: 

	/* Constructors and Destructors */

	XKMSMessageAbstractTypeImpl(
		const XSECEnv * env
	);

	XKMSMessageAbstractTypeImpl(
		const XSECEnv * env, 
		XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * node
	);

	virtual ~XKMSMessageAbstractTypeImpl() ;

	// load
	void load(void);

	// Create from scratch - tag is the element name to create
	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * 
		createBlankMessageAbstractType(
		const XMLCh * tag,
		const XMLCh * service,
		const XMLCh * id = NULL);

	/* Message Manipulation Methods */

	virtual XKMSMessageAbstractType::messageType getMessageType(void) {
		return XKMSMessageAbstractType::None;
	}

	/* Getter Interface Methods */

	virtual bool isSigned(void) const;
	virtual DSIGSignature * getSignature(void) const;
	virtual XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * getElement(void) const;
	virtual const XMLCh * getId(void) const;
	virtual const XMLCh * getService(void) const;
	virtual const XMLCh * getNonce(void) const;

	/* Setter interface methods */

	virtual void setId(const XMLCh * id);
	virtual void setService(const XMLCh * service);
	virtual void setNonce(const XMLCh * uri);
	virtual DSIGSignature * addSignature(
		canonicalizationMethod cm = CANON_C14N_NOC,
		signatureMethod	sm = SIGNATURE_DSA,
		hashMethod hm = HASH_SHA1);

	/* Opaque Client Data interface */
	virtual int getOpaqueClientDataSize(void);
	virtual const XMLCh * getOpaqueClientDataItemStr(int item);
	virtual void appendOpaqueClientDataItem(const XMLCh * item);

public:

	const XSECEnv		* mp_env;		// NOTE: Owned by the base message class

	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement
						* mp_messageAbstractTypeElement;

private:

	XERCES_CPP_NAMESPACE_QUALIFIER  DOMAttr
						* mp_idAttr;
	XERCES_CPP_NAMESPACE_QUALIFIER  DOMAttr
						* mp_serviceAttr;
	XERCES_CPP_NAMESPACE_QUALIFIER  DOMAttr
						* mp_nonceAttr;
	XERCES_CPP_NAMESPACE_QUALIFIER  DOMElement
						* mp_signatureElement;
	XERCES_CPP_NAMESPACE_QUALIFIER  DOMElement
						* mp_opaqueClientDataElement;

	XSECProvider		m_prov;
	DSIGSignature		* mp_signature;

	int					m_opaqueClientDataSize;

	// Unimplemented
	XKMSMessageAbstractTypeImpl(void);
	XKMSMessageAbstractTypeImpl(const XKMSMessageAbstractTypeImpl &);
	XKMSMessageAbstractTypeImpl & operator = (const XKMSMessageAbstractTypeImpl &);

};

#define XKMS_MESSAGEABSTRACTYPE_IMPL_METHODS \
	virtual XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * \
		getElement(void) const \
		{return m_msg.getElement();} \
	virtual bool isSigned(void) const \
		{return m_msg.isSigned();} \
	virtual DSIGSignature * getSignature(void) const \
		{return m_msg.getSignature();} \
	virtual const XMLCh * getId(void) const \
		{return m_msg.getId();} \
	virtual const XMLCh * getService(void) const \
		{return m_msg.getService();} \
	virtual const XMLCh * getNonce(void) const \
		{return m_msg.getNonce();} \
	virtual void setId(const XMLCh * id) \
		{m_msg.setId(id);} \
	virtual void setService(const XMLCh * service) \
		{m_msg.setService(service);} \
	virtual void setNonce(const XMLCh * uri) \
		{m_msg.setNonce(uri);} \
	virtual DSIGSignature * addSignature( \
		canonicalizationMethod cm = CANON_C14N_NOC, \
		signatureMethod	sm = SIGNATURE_DSA, \
		hashMethod hm = HASH_SHA1) \
		{return m_msg.addSignature(cm,sm,hm);} \
	virtual int getOpaqueClientDataSize(void) \
		{return m_msg.getOpaqueClientDataSize();} \
	virtual const XMLCh * getOpaqueClientDataItemStr(int item) \
		{return m_msg.getOpaqueClientDataItemStr(item);} \
	virtual void appendOpaqueClientDataItem(const XMLCh * item) \
		{m_msg.appendOpaqueClientDataItem(item);}

#endif /* XKMSMESSAGEABSTRACTTYPEIMPL_INCLUDE */
