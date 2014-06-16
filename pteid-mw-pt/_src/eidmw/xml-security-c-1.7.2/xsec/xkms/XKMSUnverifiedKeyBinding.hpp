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
 * XKMSUnverifiedKeyBinding := Interface for UnverifiedKeyBinding elements
 *
 * $Id: XKMSUnverifiedKeyBinding.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef XKMSUNVERIFIEDKEYBINDING_INCLUDE
#define XKMSUNVERIFIEDKEYBINDING_INCLUDE

// XSEC Includes

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/xkms/XKMSKeyBindingAbstractType.hpp>

/**
 * @ingroup xkms
 */

/**
 * @brief Interface definition for the UnverifiedKeyBinding elements
 *
 * The \<UnverifiedKeyBinding\> Element is used in a LocateResult message to 
 * provide results to a client.  Such results should be treated by the client
 * without any trust - they are literally "unverified"
 *
 * The schema definition for UnverifiedKeyBinding is as follows :
 *
 * \verbatim
   <!-- UnverifiedKeyBinding -->
   <element name="UnverifiedKeyBinding" 
         type="xkms:UnverifiedKeyBindingType"/>
   <complexType name="UnverifiedKeyBindingType">
      <complexContent>
         <extension base="xkms:KeyBindingAbstractType">
            <sequence>
               <element ref="xkms:ValidityInterval" minOccurs="0"/>
            </sequence>
         </extension>
      </complexContent>
   </complexType>
   <!-- /UnverifiedKeyBinding -->\endverbatim
 */


class XKMSUnverifiedKeyBinding : public XKMSKeyBindingAbstractType {

	/** @name Constructors and Destructors */
	//@{

protected:

	XKMSUnverifiedKeyBinding() {};

public:

	virtual ~XKMSUnverifiedKeyBinding() {};

private:

	// Unimplemented
	XKMSUnverifiedKeyBinding(const XKMSUnverifiedKeyBinding &);
	XKMSUnverifiedKeyBinding & operator = (const XKMSUnverifiedKeyBinding &);

};

#endif /* XKMSUNVERIFIEDKEYBINDING_INCLUDE */
