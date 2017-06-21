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
 * XKMSQueryKeyBinding := Interface for QueryKeyBinding elements
 *
 * $Id: XKMSQueryKeyBinding.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef XKMSQUERYKEYBINDING_INCLUDE
#define XKMSQUERYKEYBINDING_INCLUDE

// XSEC Includes

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/xkms/XKMSKeyBindingAbstractType.hpp>

/**
 * @ingroup xkms
 */

/**
 * @brief Interface definition for the QueryKeyBinding elements
 *
 * The \<QueryKeyBinding\> Element is used in a request message to a server
 * to ask the server as to whether particular key bindings are permitted for
 * a given key.
 *
 * The schema definition for QueryKeyBinding is as follows :
 *
 * \verbatim
   <!-- QueryKeyBinding -->
   <element name="QueryKeyBinding" type="xkms:QueryKeyBindingType"/>
   <complexType name="QueryKeyBindingType">
      <complexContent>
         <extension base="xkms:KeyBindingAbstractType">
            <sequence>
               <element ref="xkms:TimeInstant" minOccurs="0"/>
            </sequence>
         </extension>
      </complexContent>
   </complexType>
   <!-- /QueryKeyBinding -->
\endverbatim
 */


class XKMSQueryKeyBinding : public XKMSKeyBindingAbstractType {

	/** @name Constructors and Destructors */
	//@{

protected:

	XKMSQueryKeyBinding() {};

public:

	virtual ~XKMSQueryKeyBinding() {};


private:

	// Unimplemented
	XKMSQueryKeyBinding(const XKMSQueryKeyBinding &);
	XKMSQueryKeyBinding & operator = (const XKMSQueryKeyBinding &);

};

#endif /* XKMSQUERYKEYBINDING_INCLUDE */
