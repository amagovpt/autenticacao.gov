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
 * XKMSPrototypeKeyBinding := Interface for KeyBinding elements
 *
 * $Id: XKMSPrototypeKeyBinding.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef XKMSPROTOTYPEKEYBINDING_INCLUDE
#define XKMSPROTOTYPEKEYBINDING_INCLUDE

// XSEC Includes

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/xkms/XKMSKeyBindingAbstractType.hpp>

class XKMSValidityInterval;

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
   <!-- PrototypeKeyBinding -->
   <element name="PrototypeKeyBinding" type="xkms:PrototypeKeyBindingType"/>
   <complexType name="PrototypeKeyBindingType">
      <complexContent>
         <extension base="xkms:KeyBindingAbstractType">
            <sequence>
               <element ref="xkms:ValidityInterval" minOccurs="0"/>
               <element ref="xkms:RevocationCodeIdentifier" minOccurs="0"/>
            </sequence>
         </extension>
      </complexContent>
   </complexType>
   <!-- /PrototypeKeyBinding -->
\endverbatim
 */

class XKMSStatus;

class XKMSPrototypeKeyBinding : public XKMSKeyBindingAbstractType {

	/** @name Constructors and Destructors */
	//@{

protected:

	XKMSPrototypeKeyBinding() {};

public:

	virtual ~XKMSPrototypeKeyBinding() {};

	//@}

	/** @name Get Methods */
	//@{

	/**
	 * \brief Get the suggested ValidityInterval for this key
	 *
	 * Clients can suggest a Validity period for a key using this element.
	 * Such requests can (of course) be ignored by the server
	 *
	 * @return The ValditityInterval object or NULL if none was defined
	 */

	virtual XKMSValidityInterval * getValidityInterval(void) const = 0;

	/**
	 * \brief Obtain the RevocationCodeIdentifier to be used for this key
	 *
	 * The client can provide a revocation code when registering a new key.
	 * The code provided initially is a double MACd version of the client's 
	 * revocation pass phrase.  
	 *
	 * Should the client later wish to later revoke their key, they should provde
	 * the single MACd version which can then be converted to the double MAC by
	 * the server - thus validating the code without ever passing anything over
	 * the wire that will directly compromise the key until the final revoke request
	 *
	 * @return A pointer to the base64 encoded HMAC output
	 */

	virtual const XMLCh * getRevocationCodeIdentifier(void) const = 0;

	//@}

	/** @name Set Methods */
	//@{

	/**
	 * \brief Set the Validity Interval for this key
	 *
	 * @param notBefore - dateTime string to set the NotBefore item as (NULL to clear)
	 * @param notOnOrAfter - dateTime string to set the NotOnOrAfter item as (NULL to clear)
	 */

	virtual void setValidityInterval(const XMLCh * notBefore, const XMLCh * notOnOrAfter) = 0;

	/**
	 * \brief Set the RevocationCodeIdentifier
	 *
	 * Sets the revocation code identifer
	 *
	 * @param identifier string containing base64 encoded HMAC value
	 */

	virtual void setRevocationCodeIdentifier(const XMLCh * identifier) = 0;

	//@}

private:

	// Unimplemented
	XKMSPrototypeKeyBinding(const XKMSPrototypeKeyBinding &);
	XKMSPrototypeKeyBinding & operator = (const XKMSPrototypeKeyBinding &);

};

#endif /* XKMSPROTOTYPEKEYBINDING_INCLUDE */
