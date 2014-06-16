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
 * XKMSStatusImpl := Implementation for Status elements (in KeyBinding)
 *
 * $Id: XKMSStatusImpl.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef XKMSSTATUSIMPL_INCLUDE
#define XKMSSTATUSIMPL_INCLUDE

// XSEC Includes

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/xkms/XKMSStatus.hpp>



class XKMSStatusImpl : public XKMSStatus {

public:

	XKMSStatusImpl(
		const XSECEnv * env
	);

	XKMSStatusImpl(
		const XSECEnv * env, 
		XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * node
	);

	virtual ~XKMSStatusImpl();

	// Load elements
	void load();

	// Creation
	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * 
		createBlankStatus(StatusValue status);

	/* Getter Interface Methods */
	virtual StatusValue getStatusValue(void) const;
	virtual bool getStatusReason(StatusValue status, StatusReason reason) const;

	/* Setter Methods */
	virtual void setStatusReason(StatusValue status, StatusReason reason, bool value);

private:

	const XSECEnv		* mp_env;
	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement 
							* mp_statusElement;
	XERCES_CPP_NAMESPACE_QUALIFIER DOMNode 
							* mp_statusValueAttr;

	StatusValue			m_statusValue;

	/* Record values of each status */
	XERCES_CPP_NAMESPACE_QUALIFIER DOMNode
						* m_statusReasons[3][4];


	// Unimplemented
	XKMSStatusImpl(const XKMSStatusImpl &);
	XKMSStatusImpl & operator = (const XKMSStatusImpl &);

};

#endif /* XKMSSTATUSIMPL_INCLUDE */
