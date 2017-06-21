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
 * XKMSUseKeyWith := Interface for UseKeyWith Messages
 *
 * $Id: XKMSUseKeyWith.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef XKMSUSEKEYWITH_INCLUDE
#define XKMSUSEKEYWITH_INCLUDE

// XSEC Includes

#include <xsec/framework/XSECDefs.hpp>

/**
 * @ingroup xkms
 */

/**
 * @brief Interface definition UseKeyWith elements
 *
 * \<UseKeyWith\> elements are played insied KeyBinding elements and identify
 * applications and application identifiers that correspond with the keys being
 * found.
 *
 * The schema definition for UseKeyWith is as follows :
 *
 * \verbatim
   <!-- UseKeyWith -->
   <element name="UseKeyWith" type="xkms:UseKeyWithType"/>
   <complexType name="UseKeyWithType">
      <attribute name="Application" type="anyURI" use="required"/>
      <attribute name="Identifier" type="string" use="required"/>
   </complexType>
   <!-- /UseKeyWith -->

\endverbatim
 */

class XKMSUseKeyWith {

	/** @name Constructors and Destructors */
	//@{

protected:

	XKMSUseKeyWith() {};

public:

	virtual ~XKMSUseKeyWith() {};

	/** @name Getter Interface Methods */
	//@{

	/**
	 * \brief Return the element at the base of the message
	 */

	virtual XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * getElement(void) const = 0;

	/**
	 * \brief Obtain the URI associated with the application for which this
	 * identifier applies
	 *
	 * @returns a library owned XMLCh string containing the URI
	 */

	virtual const XMLCh * getApplication(void) const = 0;

	/**
	 * \brief Obtain the identifier string that should be used to locate a key
	 *
	 * @returns a library owned XMLCh string containing the URI
	 */

	virtual const XMLCh * getIdentifier(void) const = 0;

	//@}

	/** @name Setter Interface Methods */
	//@{

	/**
	 * \brief Set the Application string
	 *
	 * param uri URI String identifying the application
	 */

	virtual void setApplication(const XMLCh * uri) = 0;

	/**
	 * \brief Set the Identifier string
	 *
	 * param identifier String identifying the key
	 */

	virtual void setIdentifier(const XMLCh * identifier) = 0;

	//@}

	

private:

	// Unimplemented
	XKMSUseKeyWith(const XKMSUseKeyWith &);
	XKMSUseKeyWith & operator = (const XKMSUseKeyWith &);

};

#endif /* XKMSUSEKEYWITH_INCLUDE */
