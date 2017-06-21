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
 * XKMSNotBoundAuthentication := Interface for ValidityInterval elements
 *
 * $Id: XKMSNotBoundAuthentication.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef XKMSNOTBOUNDAUTHENTICATION_INCLUDE
#define XKMSNOTBOUNDAUTHENTICATION_INCLUDE

// XSEC Includes

#include <xsec/framework/XSECDefs.hpp>


/**
 * @ingroup xkms
 */

/**
 * @brief Interface definition for the NotBoundAuthentication elements
 *
 * The \<NotBoundAuthentication\> Element used within an \<Authentication\>
 * element to pass a pass phrase of some kind that authenticates the X-KRSS
 * request.
 *
 * The schema definition for NotBoundAuthentication is as follows :
 *
 * \verbatim
   <!-- NotBoundAuthentication -->
   <element name="NotBoundAuthentication" 
         type="xkms:NotBoundAuthenticationType"/>
   <complexType name="NotBoundAuthenticationType">
      <attribute name="Protocol" type="anyURI" use="required"/>
      <attribute name="Value" type="base64Binary" use="required"/>
   </complexType>
   <!-- /NotBoundAuthentication -->
\endverbatim
 */

class XKMSNotBoundAuthentication {

	/** @name Constructors and Destructors */
	//@{

protected:

	XKMSNotBoundAuthentication() {};

public:

	virtual ~XKMSNotBoundAuthentication() {};

	//@}

	/** @name Get Methods */
	//@{

	/**
	 * \brief Get the protocol URI
	 *
	 * A \<XKMSNotBoundAuthentication\> has a protocol attribute defining the
	 * protocol being used to authenticate the request.
	 *
	 * @return The string holding the URI of the protocol
	 */

	virtual const XMLCh * getProtocol(void) const = 0;

	/**
	 * \brief Get the value
	 *
	 * Returns the authentication value.
	 *
	 * @return A string holding the base64 encoded authentication value
	 */

	virtual const XMLCh * getValue(void) const = 0;

	//@}

	/** @name Set Methods */
	//@{

	/**
	 * \brief Set the protocol
	 *
	 * @param uri String containing the URI to set as the protocol
	 */

	virtual void setProtocol(const XMLCh * uri) = 0;

	/**
	 * \brief Set the authentication value
	 *
	 * @param value Base64 encoded value to set as the authentication value
	 */

	virtual void setValue(const XMLCh * value) = 0;

	//@}

private:

	// Unimplemented
	XKMSNotBoundAuthentication(const XKMSNotBoundAuthentication &);
	XKMSNotBoundAuthentication & operator = (const XKMSNotBoundAuthentication &);

};

#endif /* XKMSNOTBOUNDAUTHENTICATION_INCLUDE */
