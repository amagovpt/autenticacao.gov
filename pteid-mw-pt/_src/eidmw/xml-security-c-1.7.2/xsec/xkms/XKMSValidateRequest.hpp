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
 * XKMSValidateRequest := Interface for ValidateRequest Messages
 *
 * $Id: XKMSValidateRequest.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef XKMSVALIDATEREQUETS_INCLUDE
#define XKMSVALIDATEREQUETS_INCLUDE

// XSEC Includes

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/xkms/XKMSRequestAbstractType.hpp>

class XKMSQueryKeyBinding;

/**
 * @ingroup xkms
 */

/**
 * @brief Interface definition for the ValidateRequest elements
 *
 * The \<ValidateRequest\> is one of the fundamental message types of
 * the XKISS service.  It is used by the Validate service to find a key
 * and return a set of key bindings for that key.
 *
 * The schema definition for ValidateRequest is as follows :
 *
 * \verbatim
   <!-- ValidateRequest -->
   <element name="ValidateRequest" type="xkms:ValidateRequestType"/>
   <complexType name="ValidateRequestType">
      <complexContent>
         <extension base="xkms:RequestAbstractType">
            <sequence>
               <element ref="xkms:QueryKeyBinding"/>
            </sequence>
         </extension>
      </complexContent>
   </complexType>
   <!-- /ValidateRequest -->
\endverbatim
 */

class XKMSValidateRequest : public XKMSRequestAbstractType {

	/** @name Constructors and Destructors */
	//@{

protected:

	XKMSValidateRequest() {};

public:

	virtual ~XKMSValidateRequest() {};

	/** @name Getter Interface Methods */
	//@{

	/**
	 * \brief Return the element at the base of the message
	 */

	virtual XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * getElement(void) const = 0;

	/**
	 * \brief Obtain the QueryKeyBinding element
	 *
	 * The QueryKeyBinding element is the core of the ValidateRequest message, and
	 * defines the public key information that will be sent to the server.
	 *
	 * @returns A pointer to the XKMSQueryKeyBinding element
	 */

	virtual XKMSQueryKeyBinding * getQueryKeyBinding(void) = 0;

	//@}

	/** @name Setter Interface Methods */
	//@{

	/** \brief Add a QueryKeyBinding element
	 *
	 * Set a QueryKeyBinding element in the ValidateRequest message.  The returned
	 * object can be manipulated to add KeyInfo elements to the ValidateRequest.
	 *
	 * @returns A pointer to the newly created QueryKeyBinding object, or
	 * the pointer to extant object if one already existed.
	 */

	virtual XKMSQueryKeyBinding * addQueryKeyBinding(void) = 0;

	//@}

private:

	// Unimplemented
	XKMSValidateRequest(const XKMSValidateRequest &);
	XKMSValidateRequest & operator = (const XKMSValidateRequest &);

};

#endif /* XKMSVALIDATEREQUETS_INCLUDE */
