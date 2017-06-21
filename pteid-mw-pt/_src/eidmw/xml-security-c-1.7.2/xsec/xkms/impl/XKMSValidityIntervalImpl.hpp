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
 * XKMSValidityInterval := Implementation for ValidityInterval elements
 *
 * $Id: XKMSValidityIntervalImpl.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef XKMSVALIDITYINTERVALIMPL_HEADER
#define XKMSVALIDITYINTERVALIMPL_HEADER

#include <xsec/framework/XSECDefs.hpp>

#include <xsec/xkms/XKMSValidityInterval.hpp>

class XSECEnv;

class XKMSValidityIntervalImpl : public XKMSValidityInterval {

public:

	/* Constructors and Destructors */

	XKMSValidityIntervalImpl(
		const XSECEnv * env
	);

	XKMSValidityIntervalImpl(
		const XSECEnv * env, 
		XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * node
	);

	virtual ~XKMSValidityIntervalImpl() ;

	// Load
	void load();

	// Create
	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *
		createBlankValidityInterval(const XMLCh * notBefore, const XMLCh * notOnOrAfter);

	// Get methods

	virtual const XMLCh * getNotBefore(void) const;
	virtual const XMLCh * getNotOnOrAfter(void) const;

	// Set

	virtual void setNotBefore(const XMLCh * val);
	virtual void setNotOnOrAfter(const XMLCh * val);

private:

	const XSECEnv
				* mp_env;
	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement
				* mp_validityIntervalElement;
	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement
				* mp_notBeforeElement;
	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement
				* mp_notOnOrAfterElement;

	// Unimplemented
	XKMSValidityIntervalImpl(void);
	XKMSValidityIntervalImpl(const XKMSValidityIntervalImpl &);
	XKMSValidityIntervalImpl & operator = (const XKMSValidityIntervalImpl &);

};

#endif /* XKMSVALIDITYINTERVAL_INCLUDE */
