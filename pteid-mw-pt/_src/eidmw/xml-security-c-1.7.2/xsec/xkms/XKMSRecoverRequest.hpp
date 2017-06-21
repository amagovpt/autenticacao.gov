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
 * XKMSRecoverRequest := Interface for RegisterRequest Messages
 *
 * $Id$
 *
 */

#ifndef XKMSRECOVERREQUEST_INCLUDE 
#define XKMSRECOVERREQUEST_INCLUDE 

// XSEC Includes

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/xkms/XKMSRequestAbstractType.hpp>
#include <xsec/xkms/XKMSStatus.hpp>

class DSIGSignature;
class XKMSAuthentication;
class XKMSRecoverKeyBinding;

/**
 * @ingroup xkms
 */

/**
 * @brief Interface definition for the RecoverRequest elements
 *
 * The \<RecoverRequest\> is one of the message types of
 * the X-KRMS service.  It is used when a client wishes to recover
 * a private key known to the service.
 *
 * The schema definition for RecoverRequest is as follows :
 *
 * \verbatim
    <!-- RecoverRequest -->
   <element name="RecoverRequest" type="xkms:RecoverRequestType"/>
   <complexType name="RecoverRequestType">
      <complexContent>
         <extension base="xkms:RequestAbstractType">
            <sequence>
               <element ref="xkms:RecoverKeyBinding"/>
               <element ref="xkms:Authentication"/>
            </sequence>
         </extension>
      </complexContent>
   </complexType>
   <element name="RecoverKeyBinding" type="xkms:KeyBindingType"/>
   <!-- /RecoverRequest -->
\endverbatim
 */

class XKMSRecoverRequest : public XKMSRequestAbstractType {

	/** @name Constructors and Destructors */
	//@{

protected:

	XKMSRecoverRequest() {};

public:

	virtual ~XKMSRecoverRequest() {};

	/** @name Getter Interface Methods */
	//@{

	/**
	 * \brief Return the element at the base of the message
	 */

	virtual XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * getElement(void) const = 0;

	/**
	 * \brief Obtain the RecoverKeyBinding element
	 *
	 * The RecoverKeyBinding element is the core of the RecoverRequest message, and
	 * defines the key information that a recover is required for.
	 *
	 * @returns A pointer to the XKMSPrototypeKeyBinding element
	 */

	virtual XKMSRecoverKeyBinding * getRecoverKeyBinding(void) const = 0;

	/**
	 * \brief Get the Authentication element
	 *
	 * The Authentication element of the RecoverRequest is used by the client to
	 * authenticate the request to the server.
	 *
	 * @return A pointer to the Authentication structure 
	 */

	virtual XKMSAuthentication * getAuthentication (void) const = 0;

	//@}

	/** @name Setter Interface Methods */
	//@{

	/** \brief Add a RecoverKeyBinding element
	 *
	 * Set a RecoverKeyBinding element in the Request message.  The returned
	 * object can be manipulated to add KeyInfo elements to the Request.
	 *
	 * @param status The status value to add into the RecoverKeyBinding structure
	 * @returns A pointer to the newly created RecoverKeyBinding object, or
	 * the pointer to extant object if one already existed.
	 */

	virtual XKMSRecoverKeyBinding * addRecoverKeyBinding(XKMSStatus::StatusValue status) = 0;

	/** \brief Add an Authentication element
	 *
	 * Set a Authentication element in the Request message.  The returned
	 * object can be manipulated to add Authentication information to the request.
	 *
	 * @returns A pointer to the newly created Authenticaton object, or
	 * the pointer to extant object if one already existed.
	 */

	virtual XKMSAuthentication * addAuthentication(void) = 0;

	//@}

private:

	// Unimplemented
	XKMSRecoverRequest(const XKMSRecoverRequest &);
	XKMSRecoverRequest & operator = (const XKMSRecoverRequest &);

};

#endif /* XKMSRECOVERREQUEST_INCLUDE */
