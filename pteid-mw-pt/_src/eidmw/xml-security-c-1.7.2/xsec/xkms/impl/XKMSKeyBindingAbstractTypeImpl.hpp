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
 * XKMSKeyBindingAbstractTypeImpl := Implementation of base for KeyBinding elements
 *
 * $Id: XKMSKeyBindingAbstractTypeImpl.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef XKMSKEYBINDINGABSTRACTTYPEIMPL_INCLUDE
#define XKMSKEYBINDINGABSTRACTTYPEIMPL_INCLUDE

// XSEC Includes

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/xkms/XKMSKeyBindingAbstractType.hpp>

#include <vector>

XSEC_DECLARE_XERCES_CLASS(DOMNode);

class XSECEnv;
class DSIGKeyInfoList;
class XKMSUseKeyWithImpl;

class XKMSKeyBindingAbstractTypeImpl : public XKMSKeyBindingAbstractType {

public:

	/* Constructors and Destructors */

	XKMSKeyBindingAbstractTypeImpl(
		const XSECEnv * env
	);

	XKMSKeyBindingAbstractTypeImpl(
		const XSECEnv * env, 
		XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * node
	);

	virtual ~XKMSKeyBindingAbstractTypeImpl() ;

	// Load
	void load(void);

	// Create
	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *
		createBlankKeyBindingAbstractType(const XMLCh * tag);

	/* Getter Interface Methods */

	virtual XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * getElement(void) const;

	virtual const XMLCh * getId(void) const;
	virtual DSIGKeyInfoList * getKeyInfoList(void) const;
	virtual bool getEncryptionKeyUsage(void) const;
	virtual bool getSignatureKeyUsage(void) const;
	virtual bool getExchangeKeyUsage(void) const;

	/* Setter interface methods */

	virtual void setId(const XMLCh * id = NULL);
	virtual void setEncryptionKeyUsage(void);
	virtual void setSignatureKeyUsage(void);
	virtual void setExchangeKeyUsage(void);

	/* Key Info methods */
	virtual DSIGKeyInfoList * getKeyInfoList();
	virtual void clearKeyInfo(void);
	virtual DSIGKeyInfoValue * appendDSAKeyValue(const XMLCh * P, 
						   const XMLCh * Q, 
						   const XMLCh * G, 
						   const XMLCh * Y);
	virtual DSIGKeyInfoValue * appendRSAKeyValue(const XMLCh * modulus, 
						   const XMLCh * exponent);
	virtual DSIGKeyInfoX509 * appendX509Data(void);
	virtual DSIGKeyInfoName * appendKeyName(const XMLCh * name, bool isDName = false);
	virtual DSIGKeyInfoPGPData * appendPGPData(const XMLCh * id, const XMLCh * packet);
	virtual DSIGKeyInfoSPKIData * appendSPKIData(const XMLCh * sexp);
	virtual DSIGKeyInfoMgmtData * appendMgmtData(const XMLCh * data);

	/* UseKeyWith handling */

	virtual int getUseKeyWithSize(void) const;
	virtual XKMSUseKeyWith * getUseKeyWithItem(int item) const;
	virtual XKMSUseKeyWith * appendUseKeyWithItem(
			const XMLCh * application,  
			const XMLCh * identifier);

protected:

	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement
					* mp_keyBindingAbstractTypeElement;
	const XSECEnv	* mp_env;

private:

	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *
		setKeyUsage(const XMLCh * usage);

#if defined(XSEC_NO_NAMESPACES)
	typedef vector<XKMSUseKeyWithImpl *>		UseKeyWithVectorType;
#else
	typedef std::vector<XKMSUseKeyWithImpl *>	UseKeyWithVectorType;
#endif

	UseKeyWithVectorType	m_useKeyWithList;
	
	XERCES_CPP_NAMESPACE_QUALIFIER DOMNode
					* mp_idAttr;
	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement
					* mp_keyUsageSignatureElement;
	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement
					* mp_keyUsageEncryptionElement;
	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement
					* mp_keyUsageExchangeElement;
	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement
					* mp_keyInfoElement;
	DSIGKeyInfoList * mp_keyInfoList;

	void createKeyInfoElement(void);

	// Unimplemented
	XKMSKeyBindingAbstractTypeImpl(void);
	XKMSKeyBindingAbstractTypeImpl(const XKMSKeyBindingAbstractTypeImpl &);
	XKMSKeyBindingAbstractTypeImpl & operator = (const XKMSKeyBindingAbstractTypeImpl &);

};

// Define imported methods for classes that inherit them (makes things easier)

#define XKMS_KEYBINDINGABSTRACTYPE_IMPL_METHODS \
	virtual XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * getElement(void) const \
		{return XKMSKeyBindingAbstractTypeImpl::getElement();} \
	virtual const XMLCh * getId(void) const \
		{return XKMSKeyBindingAbstractTypeImpl::getId();} \
	virtual DSIGKeyInfoList * getKeyInfoList(void) const \
		{return XKMSKeyBindingAbstractTypeImpl::getKeyInfoList();} \
	virtual bool getEncryptionKeyUsage(void) const \
		{return XKMSKeyBindingAbstractTypeImpl::getEncryptionKeyUsage();} \
	virtual bool getSignatureKeyUsage(void) const \
		{return XKMSKeyBindingAbstractTypeImpl::getSignatureKeyUsage();} \
	virtual bool getExchangeKeyUsage(void) const \
		{return XKMSKeyBindingAbstractTypeImpl::getExchangeKeyUsage();} \
	virtual void setId(const XMLCh * id) \
		{XKMSKeyBindingAbstractTypeImpl::setId(id);} \
	virtual void setEncryptionKeyUsage(void) \
		{XKMSKeyBindingAbstractTypeImpl::setEncryptionKeyUsage();} \
	virtual void setSignatureKeyUsage(void) \
		{XKMSKeyBindingAbstractTypeImpl::setSignatureKeyUsage();} \
	virtual void setExchangeKeyUsage(void) \
		{XKMSKeyBindingAbstractTypeImpl::setExchangeKeyUsage();} \
	virtual DSIGKeyInfoList * getKeyInfoList() \
		{return XKMSKeyBindingAbstractTypeImpl::getKeyInfoList();} \
	virtual void clearKeyInfo(void) \
		{XKMSKeyBindingAbstractTypeImpl::clearKeyInfo();} \
	virtual DSIGKeyInfoValue * appendDSAKeyValue(const XMLCh * P,  \
						   const XMLCh * Q,  \
						   const XMLCh * G,  \
						   const XMLCh * Y) \
		{return XKMSKeyBindingAbstractTypeImpl::appendDSAKeyValue(P,Q,G,Y);} \
	virtual DSIGKeyInfoValue * appendRSAKeyValue(const XMLCh * modulus,  \
						   const XMLCh * exponent) \
		{return XKMSKeyBindingAbstractTypeImpl::appendRSAKeyValue(modulus,exponent);} \
	virtual DSIGKeyInfoX509 * appendX509Data(void) \
		{return XKMSKeyBindingAbstractTypeImpl::appendX509Data();} \
	virtual DSIGKeyInfoName * appendKeyName(const XMLCh * name, bool isDName = false) \
		{return XKMSKeyBindingAbstractTypeImpl::appendKeyName(name,isDName);} \
	virtual DSIGKeyInfoPGPData * appendPGPData(const XMLCh * id, const XMLCh * packet) \
		{return XKMSKeyBindingAbstractTypeImpl::appendPGPData(id,packet);} \
	virtual DSIGKeyInfoSPKIData * appendSPKIData(const XMLCh * sexp) \
		{return XKMSKeyBindingAbstractTypeImpl::appendSPKIData(sexp);} \
	virtual DSIGKeyInfoMgmtData * appendMgmtData(const XMLCh * data) \
		{return XKMSKeyBindingAbstractTypeImpl::appendMgmtData(data);} \
	virtual int getUseKeyWithSize(void) const \
		{return XKMSKeyBindingAbstractTypeImpl::getUseKeyWithSize();} \
	virtual XKMSUseKeyWith * getUseKeyWithItem(int item) const \
		{return XKMSKeyBindingAbstractTypeImpl::getUseKeyWithItem(item);} \
	virtual XKMSUseKeyWith * appendUseKeyWithItem( \
			const XMLCh * application,  \
			const XMLCh * identifier) \
		{return XKMSKeyBindingAbstractTypeImpl::appendUseKeyWithItem(application, identifier);}



#endif /* XKMSKEYBINDINGABSTRACTTYPEIMPL_INCLUDE */
