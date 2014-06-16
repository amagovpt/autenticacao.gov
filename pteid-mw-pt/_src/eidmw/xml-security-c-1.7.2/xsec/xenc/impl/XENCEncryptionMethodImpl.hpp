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
 * XENCEncryptionMethod := Interface definition for EncryptionMethod element
 *
 * $Id: XENCEncryptionMethodImpl.hpp 1350045 2012-06-13 22:33:10Z scantor $
 *
 */

#ifndef XENCENCRYPTIONMETHODIMPL_INCLUDE
#define XENCENCRYPTIONMETHODIMPL_INCLUDE

// XSEC Includes

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/xenc/XENCEncryptionMethod.hpp>

class XSECEnv;

XSEC_DECLARE_XERCES_CLASS(DOMNode);

class XENCEncryptionMethodImpl : public XENCEncryptionMethod {

public:

	XENCEncryptionMethodImpl(const XSECEnv * env);
	XENCEncryptionMethodImpl(
		const XSECEnv * env, 
		XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * element
	);
	virtual ~XENCEncryptionMethodImpl();

	// Load elements
	void load();

	// Create from scratch
	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * createBlankEncryptedMethod(
						const XMLCh * algorithm);

	// Interface
	const XMLCh * getAlgorithm(void) const;
	virtual XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * getElement(void) const
		{return mp_encryptionMethodElement;}
	virtual const XMLCh * getDigestMethod(void) const;
	virtual const XMLCh * getMGF(void) const;
	virtual const XMLCh * getOAEPparams(void) const;
	virtual int getKeySize(void) const;
	virtual void setDigestMethod(const XMLCh * method);
    virtual void setMGF(const XMLCh * mgf);
	virtual void setOAEPparams(const XMLCh * params);
	virtual void setKeySize(int size);



private:

	// Unimplemented
	XENCEncryptionMethodImpl();
	XENCEncryptionMethodImpl(const XENCEncryptionMethodImpl &);
	XENCEncryptionMethodImpl & operator = (const XENCEncryptionMethodImpl &);

	const XSECEnv				* mp_env;
	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement					
								* mp_encryptionMethodElement;	// Node at head of structure
	XERCES_CPP_NAMESPACE_QUALIFIER DOMNode
								* mp_algorithmAttr;
	XERCES_CPP_NAMESPACE_QUALIFIER DOMNode					
								* mp_digestAlgorithmAttr;
	XERCES_CPP_NAMESPACE_QUALIFIER DOMNode					
								* mp_mgfAlgorithmAttr;
	XERCES_CPP_NAMESPACE_QUALIFIER DOMNode
								* mp_oaepParamsTextNode;
	XERCES_CPP_NAMESPACE_QUALIFIER DOMNode
								* mp_keySizeTextNode;
};

#endif /* XENCENCRYPTIONMETHODIMPL_INCLUDE */
