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
 * XKMSAuthentication := Interface for ValidityInterval elements
 *
 * $Id: XKMSAuthentication.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef XKMSAUTHENTICATION_INCLUDE
#define XKMSAUTHENTICATION_INCLUDE

// XSEC Includes

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/dsig/DSIGConstants.hpp>

class DSIGSignature;
class XKMSNotBoundAuthentication;

/**
 * @ingroup xkms
 */

/**
 * @brief Interface definition for the Authentication elements
 *
 * The \<Authentication\> Element used within a number of X-KRSS requests
 * as the mechanism for authentication a request from a client to the server
 *
 * It can contain either a standard XML-Signature element (via a 
 * KeyBindingAuthentication element) or a straight password (not otherwise
 * protected).
 *
 * The schema definition for ValidityInterval is as follows :
 *
 * \verbatim
   <!-- Authentication -->
   <element name="Authentication" type="xkms:AuthenticationType"/>
   <complexType name="AuthenticationType">
      <sequence>
         <element ref="xkms:KeyBindingAuthentication" minOccurs="0"/>
         <element ref="xkms:NotBoundAuthentication" minOccurs="0"/>
      </sequence>
   </complexType>
   <!-- /Authentication -->
\endverbatim
 */

class XKMSAuthentication {

	/** @name Constructors and Destructors */
	//@{

protected:

	XKMSAuthentication() {};

public:

	virtual ~XKMSAuthentication() {};

	//@}

	/** @name Get Methods */
	//@{

	/**
	 * \brief Get KeyBindingAuthentication signature construct
	 *
	 * A \<XKMSAuthentication\> can have a \<KeyBindingAuthentication\>
	 * element as an immediate child.  This element is used to hold a 
	 * XML-Signature over the PrototypeKeyBinding.
	 *
	 * @return The signature held in the KeyBindingAuthentication - or NULL
	 * if none is set.
	 */

	virtual DSIGSignature * getKeyBindingAuthenticationSignature(void) const = 0;

	/**
	 * \brief Get the NotBoundAuthentication structure
	 *
	 * The NotBoundAuthentication element is used to pass a plaintext key
	 * along with the request message as the authentication mechanism.
	 *
	 * @note The XKMS Spec indicates that this element MUST NOT be used unless
	 * there is another form of security being used to wrap the requests -
	 * e.g. authenticated SSL.
	 *
	 * @return The NotBoundAuthentication structure (or NULL if none is defined)
	 */

	virtual XKMSNotBoundAuthentication * getNotBoundAuthentication(void) const = 0;

	//@}

	/** @name Set Methods */
	//@{

	/**
	 * \brief Add a KeyBindingAuthentication signature to the message
	 *
	 * Allows the application to add a new signature into a 
	 * KeyBindingAuthentication element
	 *
	 * @returns the new Signature structure
	 */

	virtual DSIGSignature * addKeyBindingAuthenticationSignature(
		canonicalizationMethod cm = CANON_C14NE_NOC,
		signatureMethod	sm = SIGNATURE_HMAC,
		hashMethod hm = HASH_SHA1) = 0;

	/**
	 * \brief Set the NotBoundAuthentication value
	 *
	 * Allows applications to set a authentication value
	 *
	 * @param uri URI defining the authentication protocol
	 * @param value Base64 encoded value to set as the authentication value
	 */

	virtual void setNotBoundAuthentication(const XMLCh * uri, const XMLCh * value) = 0;

	//@}

private:

	// Unimplemented
	XKMSAuthentication(const XKMSAuthentication &);
	XKMSAuthentication & operator = (const XKMSAuthentication &);

};

#endif /* XKMSAUTHENTICATION_INCLUDE */
