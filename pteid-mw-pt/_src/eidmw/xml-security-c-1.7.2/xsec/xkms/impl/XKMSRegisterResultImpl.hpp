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
 * XKMSRegisterResultImpl := Implementation of RegisterResult Messages
 *
 * $Id: XKMSRegisterResultImpl.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef XKMSREGISTERRESULTIMPL_INCLUDE
#define XKMSREGISTERRESULTIMPL_INCLUDE

// XSEC Includes

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/xkms/XKMSRegisterResult.hpp>
#include <xsec/framework/XSECProvider.hpp>

#include "XKMSResultTypeImpl.hpp"

#include <vector>

class XKMSKeyBindingImpl;
class XKMSRSAKeyPairImpl;
class XENCCipherImpl;

class XKMSRegisterResultImpl : public XKMSRegisterResult {

public:
	XKMSResultTypeImpl m_result;
	XKMSMessageAbstractTypeImpl &m_msg;
public:

	XKMSRegisterResultImpl(
		const XSECEnv * env
	);

	XKMSRegisterResultImpl(
		const XSECEnv * env, 
		XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * node
	);

	virtual ~XKMSRegisterResultImpl();

	// Load elements
	void load();

	// Creation
	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * 
		createBlankRegisterResult(
		const XMLCh * service,
		const XMLCh * id,
		ResultMajor rmaj,
		ResultMinor rmin);

	// Interface methods
	virtual int getKeyBindingSize(void) const;
	virtual XKMSKeyBinding * getKeyBindingItem(int item) const;
	virtual XKMSKeyBinding * appendKeyBindingItem(XKMSStatus::StatusValue status);
	virtual XKMSRSAKeyPair * getRSAKeyPair(const char * passPhrase);
	virtual XENCEncryptedData * setRSAKeyPair(const char * passPhrase,
		XMLCh * Modulus,
		XMLCh * Exponent,
		XMLCh * P,
		XMLCh * Q,
		XMLCh * DP,
		XMLCh * DQ,
		XMLCh * InverseQ,
		XMLCh * D,		
		encryptionMethod em,
		const XMLCh * algorithmURI = NULL);



	/* Implemented from MessageAbstractType */
	virtual messageType getMessageType(void);

	/* Forced inheritance from XKMSMessageAbstractTypeImpl */
	XKMS_MESSAGEABSTRACTYPE_IMPL_METHODS

	/* Forced inheritance from XKMSResultTypeImpl */
	XKMS_RESULTTYPE_IMPL_METHODS

private:

#if defined(XSEC_NO_NAMESPACES)
	typedef vector<XKMSKeyBindingImpl *>		KeyBindingVectorType;
#else
	typedef std::vector<XKMSKeyBindingImpl *>	KeyBindingVectorType;
#endif

	KeyBindingVectorType	m_keyBindingList;
	XKMSRSAKeyPairImpl		* mp_RSAKeyPair;

	XERCES_CPP_NAMESPACE_QUALIFIER  DOMElement
						* mp_privateKeyElement;

	// To handle the cipher
	XSECProvider	m_prov;

	// Unimplemented
	XKMSRegisterResultImpl(void);
	XKMSRegisterResultImpl(const XKMSRegisterResultImpl &);
	XKMSRegisterResultImpl & operator = (const XKMSRegisterResultImpl &);

};

#endif /* XKMSREGISTERRESULTIMPL_INCLUDE */
