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
 * XKMSResponseMechanism:= Interface for ResponseMechanism elements
 *
 * $Id: XKMSResponseMechanism.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef XKMSRESPONSEMECHANISM_INCLUDE
#define XKMSRESPONSEMECHANISM_INCLUDE

// XSEC Includes

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/xkms/XKMSMessageAbstractType.hpp>

/**
 * @ingroup xkms
 */

/**
 * @brief Interface definition for the ResponseMechanism element
 *
 * The \<ResponseMechanism\> is a very simple element structure that
 * allows a requestor to pass extended protocol information
 *
 * The schema definition for ResponseMechanism is as follows :
 *
 * \verbatim
   <!-- ResponseMechanism -->
   <element name="ResponseMechanism" type="QName"/>
   <!-- /ResponseMechanism -->
\endverbatim
 */


class XKMSResponseMechanism {

	/** @name Constructors and Destructors */
	//@{

protected:

	XKMSResponseMechanism() {};

public:

	virtual ~XKMSResponseMechanism() {};

	/** @name Getter Interface Methods */
	//@{

	/*
	 * \brief Obtain the element at the head of this structure
	 *
	 * @returns the owning Element for this structure
	 */

	virtual XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * getElement(void) const = 0;

	/**
	 * \brief Get the ResponseMechanism string for this element
	 *
	 * Each ResponseMechanism element has a text node that defines the Key type to
	 * be returned.
	 *
	 * @returns a pointer to the ResponseMechanism string for this element - note that the
	 * xkms namespace part of the QName (if any) is stripped away.
	 */

	virtual const XMLCh * getResponseMechanismString(void) const = 0;

	//@}

	/** @name Setter interface methods */
	//@{

	/**
	 * \brief Set the String within the ResponseMechanism element
	 *
	 * Allows a calling application to set a new ResponseMechanism string
	 * 
	 * @param str The string to set (overwriting any previous value).  Do not add
	 * any local name part - this will be added by the library.
	 * @note The library does not check that the string is a valid ResponseMechanism string.
	 */

	virtual void setResponseMechanismString(const XMLCh * str) = 0;

	//@}


private:

	// Unimplemented
	XKMSResponseMechanism(const XKMSResponseMechanism &);
	XKMSResponseMechanism & operator = (const XKMSResponseMechanism &);

};

#endif /* XKMSRESPONSEMECHANISM_INCLUDE */
