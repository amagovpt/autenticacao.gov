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
 * XKMSKeyBinding := Interface for KeyBinding elements
 *
 * $Id: XKMSKeyBinding.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef XKMSKEYBINDING_INCLUDE
#define XKMSKEYBINDING_INCLUDE

// XSEC Includes

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/xkms/XKMSKeyBindingAbstractType.hpp>

/**
 * @ingroup xkms
 */

/**
 * @brief Interface definition for the KeyBinding elements
 *
 * The \<KeyBinding\> Element is used in a result message to a client
 * to provide information on a particular key.
 *
 * The schema definition for KeyBinding is as follows :
 *
 * \verbatim
   <!-- KeyBinding -->
   <element name="KeyBinding" type="xkms:KeyBindingType"/>
   <complexType name="KeyBindingType">
      <complexContent>
         <extension base="xkms:UnverifiedKeyBindingType">
            <sequence>
               <element ref="xkms:Status"/>
            </sequence>
         </extension>
      </complexContent>
   </complexType>
   <!-- /KeyBinding -->\endverbatim
 */

class XKMSStatus;

class XKMSKeyBinding : public XKMSKeyBindingAbstractType {

	/** @name Constructors and Destructors */
	//@{

protected:

	XKMSKeyBinding() {};

public:

	virtual ~XKMSKeyBinding() {};

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
	XKMSKeyBinding(const XKMSKeyBinding &);
	XKMSKeyBinding & operator = (const XKMSKeyBinding &);

};

#endif /* XKMSKEYBINDING_INCLUDE */
