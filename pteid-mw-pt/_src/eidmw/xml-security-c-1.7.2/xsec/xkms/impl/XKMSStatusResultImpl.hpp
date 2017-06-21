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
 * XKMSStatusResultImpl := Implementation of StatusResult Messages
 *
 * $Id: XKMSStatusResultImpl.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef XKMSSTATUSRESULTIMPL_INCLUDE
#define XKMSSTATUSRESULTIMPL_INCLUDE

// XSEC Includes

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/xkms/XKMSStatusResult.hpp>

#include "XKMSResultTypeImpl.hpp"

#include <vector>

class XKMSUnverifiedKeyBindingImpl;


class XKMSStatusResultImpl : public XKMSStatusResult {

public:
	XKMSResultTypeImpl m_result;
	XKMSMessageAbstractTypeImpl &m_msg;
public:

	XKMSStatusResultImpl(
		const XSECEnv * env
	);

	XKMSStatusResultImpl(
		const XSECEnv * env, 
		XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * node
	);

	virtual ~XKMSStatusResultImpl();

	// Load elements
	void load();

	// Creation
	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * 
		createBlankStatusResult(
		const XMLCh * service,
		const XMLCh * id,
		ResultMajor rmaj,
		ResultMinor rmin);

	// Interface methods
	virtual int getSuccessCount(void) const;
	virtual int getFailureCount(void) const;
	virtual int getPendingCount(void) const;
	virtual void setSuccessCount(int count);
	virtual void setFailureCount(int count);
	virtual void setPendingCount(int count);

	/* Implemented from MessageAbstractType */
	virtual messageType getMessageType(void);

	/* Forced inheritance from XKMSMessageAbstractTypeImpl */
	XKMS_MESSAGEABSTRACTYPE_IMPL_METHODS

	/* Forced inheritance from XKMSResultTypeImpl */
	XKMS_RESULTTYPE_IMPL_METHODS

private:

	XERCES_CPP_NAMESPACE_QUALIFIER  DOMAttr
						* mp_successAttr;
	XERCES_CPP_NAMESPACE_QUALIFIER  DOMAttr
						* mp_failureAttr;
	XERCES_CPP_NAMESPACE_QUALIFIER  DOMAttr
						* mp_pendingAttr;

	// Unimplemented
	XKMSStatusResultImpl(const XKMSStatusResultImpl &);
	XKMSStatusResultImpl & operator = (const XKMSStatusResultImpl &);

};

#endif /* XKMSSTATUSRESULTIMPL_INCLUDE */
