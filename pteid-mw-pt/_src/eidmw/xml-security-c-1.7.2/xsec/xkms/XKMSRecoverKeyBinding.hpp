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
 * XKMSRecoverKeyBinding := Interface for KeyBinding elements
 *
 * $Id$
 *
 */

#ifndef XKMSRECOVERKEYBINDING_INCLUDE
#define XKMSRECOVERKEYBINDING_INCLUDE

// XSEC Includes

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/xkms/XKMSKeyBindingAbstractType.hpp>

/**
 * @ingroup xkms
 */

/**
 * @brief Interface definition for the RecoverKeyBinding elements
 *
 * The \<RecoverKeyBinding\> Element is used in a request to Recover
 * a particular key from an XKRS service
 *
 * The schema definition for RecoverKeyBinding is as follows :
 *
 * \verbatim
   <!-- RecoverKeyBinding -->
	<element name="RecoverKeyBinding" type="xkms:KeyBindingType"/>
   <!-- /RecoverKeyBinding -->
 \endverbatim
 */

class XKMSStatus;

class XKMSRecoverKeyBinding : public XKMSKeyBindingAbstractType {

	/** @name Constructors and Destructors */
	//@{

protected:

	XKMSRecoverKeyBinding() {};

public:

	virtual ~XKMSRecoverKeyBinding() {};

	//@}

	/** @name Status handling */
	//@{

	/**
	 * \brief Obtain the status element for this KeyBinding
	 *
	 * The \<Status\> element is used to describe to the caller the
	 * validity of they key being described.  This call is used to
	 * obtain the status element
	 *
	 * @return A pointer to the XKMSStatus element
	 */

	virtual XKMSStatus * getStatus(void) const = 0;

	//@}

private:

	// Unimplemented
	XKMSRecoverKeyBinding(const XKMSRecoverKeyBinding &);
	XKMSRecoverKeyBinding & operator = (const XKMSRecoverKeyBinding &);

};

#endif /* XKMSRECOVERKEYBINDING_INCLUDE */
