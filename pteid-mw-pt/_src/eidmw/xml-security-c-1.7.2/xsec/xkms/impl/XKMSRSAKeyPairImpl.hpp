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
 * XKMSRSAKeyPairImpl := Implementation class for RSAKeyPair elements
 *
 * $Id:$
 *
 */

#ifndef XKMSRSAKEYPAIRIMPL_INCLUDE
#define XKMSRSAKEYPAIRIMPL_INCLUDE

// XSEC Includes

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/xkms/XKMSRSAKeyPair.hpp>

class XSECEnv;

class XKMSRSAKeyPairImpl : public XKMSRSAKeyPair {

public:

	/* Constructors and Destructors */

	XKMSRSAKeyPairImpl(
		const XSECEnv * env
	);

	XKMSRSAKeyPairImpl(
		const XSECEnv * env, 
		XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * node
	);

	virtual ~XKMSRSAKeyPairImpl() ;

	// load
	void load(void);

	// Create from scratch - tag is the element name to create
	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * 
		createBlankXKMSRSAKeyPairImpl(
			const XMLCh * Modulus, 
			const XMLCh * Exponent, 
			const XMLCh * P, 
			const XMLCh * Q, 
			const XMLCh * DO, 
			const XMLCh * DQ, 
			const XMLCh * InverseQ, 
			const XMLCh * D);

	// Interface methods

	virtual const XMLCh * getModulus(void) const;
	virtual const XMLCh * getExponent(void) const;
	virtual const XMLCh * getP(void) const;
	virtual const XMLCh * getQ(void) const;
	virtual const XMLCh * getDP(void) const;
	virtual const XMLCh * getDQ(void) const;
	virtual const XMLCh * getInverseQ(void) const;
	virtual const XMLCh * getD(void) const;

private:

	const XSECEnv		* mp_env;
	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement 
							* mp_RSAKeyPairElement;

	const XMLCh * mp_Modulus;
	const XMLCh * mp_Exponent;
	const XMLCh * mp_P;
	const XMLCh * mp_Q;
	const XMLCh * mp_DP;
	const XMLCh * mp_DQ;
	const XMLCh * mp_InverseQ;
	const XMLCh * mp_D;

	// Unimplemented
	XKMSRSAKeyPairImpl();
	XKMSRSAKeyPairImpl(const XKMSRSAKeyPairImpl &);
	XKMSRSAKeyPairImpl & operator = (const XKMSRSAKeyPairImpl &);

};

#endif /* XKMSRSAKEYPAIRIMPL_INCLUDE */
