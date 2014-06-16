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
 * XKMSReissueResultImpl := Implementation of RegisterResult Messages
 *
 * $Id$
 *
 */

#ifndef XKMSREISSUERESULTIMPL_INCLUDE
#define XKMSREISSUERESULTIMPL_INCLUDE

// XSEC Includes

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/xkms/XKMSReissueResult.hpp>

#include "XKMSResultTypeImpl.hpp"

#include <vector>

class XKMSKeyBindingImpl;

class XKMSReissueResultImpl : public XKMSReissueResult {

public:
	XKMSResultTypeImpl m_result;
	XKMSMessageAbstractTypeImpl &m_msg;
public:

	XKMSReissueResultImpl(
		const XSECEnv * env
	);

	XKMSReissueResultImpl(
		const XSECEnv * env, 
		XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * node
	);

	virtual ~XKMSReissueResultImpl();

	// Load elements
	void load();

	// Creation
	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * 
		createBlankReissueResult(
		const XMLCh * service,
		const XMLCh * id,
		ResultMajor rmaj,
		ResultMinor rmin);

	// Interface methods
	virtual int getKeyBindingSize(void) const;
	virtual XKMSKeyBinding * getKeyBindingItem(int item) const;
	virtual XKMSKeyBinding * appendKeyBindingItem(XKMSStatus::StatusValue status);


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

	// Unimplemented
	XKMSReissueResultImpl(void);
	XKMSReissueResultImpl(const XKMSReissueResultImpl &);
	XKMSReissueResultImpl & operator = (const XKMSReissueResultImpl &);

};

#endif /* XKMSREISSUERESULTIMPL_INCLUDE */
