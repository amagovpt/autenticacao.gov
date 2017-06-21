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
 * XKMSRevokeRequest := Interface for RegisterRequest Messages
 *
 * $Id$
 *
 */

#ifndef XKMSREVOKEREQUEST_INCLUDE
#define XKMSREVOKEREQUEST_INCLUDE

// XSEC Includes

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/xkms/XKMSRequestAbstractType.hpp>
#include <xsec/xkms/XKMSStatus.hpp>

class DSIGSignature;
class XKMSAuthentication;
class XKMSRevokeKeyBinding;

/**
 * @ingroup xkms
 */

/**
 * @brief Interface definition for the RevokeRequest elements
 *
 * The \<RevokeRequest\> is one of the message types of
 * the X-KRMS service.  It is used when a client wishes to revoke
 * a key known to the service.
 *
 * The schema definition for RevokeRequest is as follows :
 *
 * \verbatim
   <!-- RevokeRequest -->
   <element name="RevokeRequest" type="xkms:RevokeRequestType"/>
   <complexType name="RevokeRequestType">
      <complexContent>
         <extension base="xkms:RequestAbstractType">
            <sequence>
               <element ref="xkms:RevokeKeyBinding"/>
               <choice>
                  <element ref="xkms:Authentication"/>
                  <element ref="xkms:RevocationCode"/>
               </choice>
            </sequence>
         </extension>
      </complexContent>
   </complexType>
   <element name="RevokeKeyBinding" type="xkms:KeyBindingType"/>
   <!-- /RevokeRequest -->
\endverbatim
 */

class XKMSRevokeRequest : public XKMSRequestAbstractType {

	/** @name Constructors and Destructors */
	//@{

protected:

	XKMSRevokeRequest() {};

public:

	virtual ~XKMSRevokeRequest() {};

	/** @name Getter Interface Methods */
	//@{

	/**
	 * \brief Return the element at the base of the message
	 */

	virtual XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * getElement(void) const = 0;

	/**
	 * \brief Obtain the PrototypKeyBinding element
	 *
	 * The PrototypeKeyBinding element is the core of the RegisterRequest message, and
	 * defines the key information that will be sent to the server for registration.
	 *
	 * @returns A pointer to the XKMSPrototypeKeyBinding element
	 */

	virtual XKMSRevokeKeyBinding * getRevokeKeyBinding(void) const = 0;

	/**
	 * \brief Get the Authentication element
	 *
	 * The Authentication element of the RegisterRequest is used by the client to
	 * authenticate the request to the server.
	 *
	 * @return A pointer to the Authentication structure 
	 */

	virtual XKMSAuthentication * getAuthentication (void) const = 0;

	/**
	 * \brief Get the revocation code for the request
	 *
	 * This element is used by the client to provide a MAC of a pass phrase that
	 * proves to the service tha the client has the authority to revoke this
	 * key.
	 *
	 * @return A pointer to string representing the base64 encoded
	 * revocation code, or NULL if none was defined
	 */

	virtual const XMLCh * getRevocationCode(void) const = 0;

	//@}

	/** @name Setter Interface Methods */
	//@{

	/** \brief Add a RevokeKeyBinding element
	 *
	 * Set a RevokeKeyBinding element in the Request message.  The returned
	 * object can be manipulated to add KeyInfo elements to the Request.
	 *
	 * @param status The status value to add into the RevokeKeyBinding structure
	 * @returns A pointer to the newly created RevokeKeyBinding object, or
	 * the pointer to extant object if one already existed.
	 */

	virtual XKMSRevokeKeyBinding * addRevokeKeyBinding(XKMSStatus::StatusValue status) = 0;

	/** \brief Add an Authentication element
	 *
	 * Set a Authentication element in the Request message.  The returned
	 * object can be manipulated to add Authentication information to the request.
	 *
	 * @returns A pointer to the newly created Authenticaton object, or
	 * the pointer to extant object if one already existed.
	 */

	virtual XKMSAuthentication * addAuthentication(void) = 0;

	/**
	 * \brief Add a RevocationCode to the message
	 *
	 * Allows the client to add a new Revocation Code string
	 * RevokeRequest element
	 *
	 * @param code the base64 representation of the revocation code
	 */

	virtual void addRevocationCode(const XMLCh * code) = 0;
	
	//@}

private:

	// Unimplemented
	XKMSRevokeRequest(const XKMSRevokeRequest &);
	XKMSRevokeRequest & operator = (const XKMSRevokeRequest &);

};

#endif /* XKMSREVOKEREQUEST_INCLUDE */
