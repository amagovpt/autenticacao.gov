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
 * XKMSLocateResultImpl := Implementation of LocateResult Messages
 *
 * $Id: XKMSLocateResultImpl.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef XKMSLOCATERESULTIMPL_INCLUDE
#define XKMSLOCATERESULTIMPL_INCLUDE

// XSEC Includes

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/xkms/XKMSLocateResult.hpp>

#include "XKMSResultTypeImpl.hpp"

#include <vector>

class XKMSUnverifiedKeyBindingImpl;


class XKMSLocateResultImpl : public XKMSLocateResult {

public:
	XKMSResultTypeImpl m_result;
	XKMSMessageAbstractTypeImpl &m_msg;
public:

	XKMSLocateResultImpl(
		const XSECEnv * env
	);

	XKMSLocateResultImpl(
		const XSECEnv * env, 
		XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * node
	);

	virtual ~XKMSLocateResultImpl();

	// Load elements
	void load();

	// Creation
	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * 
		createBlankLocateResult(
		const XMLCh * service,
		const XMLCh * id,
		ResultMajor rmaj,
		ResultMinor rmin);

	// Interface methods
	virtual int getUnverifiedKeyBindingSize(void) const;
	virtual XKMSUnverifiedKeyBinding * getUnverifiedKeyBindingItem(int item) const;
	virtual XKMSUnverifiedKeyBinding * appendUnverifiedKeyBindingItem(void);


	/* Implemented from MessageAbstractType */
	virtual messageType getMessageType(void);

	/* Forced inheritance from XKMSMessageAbstractTypeImpl */
	XKMS_MESSAGEABSTRACTYPE_IMPL_METHODS

	/* Forced inheritance from XKMSResultTypeImpl */
	XKMS_RESULTTYPE_IMPL_METHODS

private:

#if defined(XSEC_NO_NAMESPACES)
	typedef vector<XKMSUnverifiedKeyBindingImpl *>		UnverifiedKeyBindingVectorType;
#else
	typedef std::vector<XKMSUnverifiedKeyBindingImpl *>	UnverifiedKeyBindingVectorType;
#endif

	UnverifiedKeyBindingVectorType	m_unverifiedKeyBindingList;

	// Unimplemented
	XKMSLocateResultImpl(const XKMSLocateResultImpl &);
	XKMSLocateResultImpl & operator = (const XKMSLocateResultImpl &);

};

#endif /* XKMSLOCATERESULTIMPL_INCLUDE */
