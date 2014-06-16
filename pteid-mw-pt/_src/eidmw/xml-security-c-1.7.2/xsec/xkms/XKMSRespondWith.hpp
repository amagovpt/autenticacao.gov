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
 * XKMSRespondWith:= Interface for RespondWith elements
 *
 * $Id: XKMSRespondWith.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef XKMSRESPONDWITH_INCLUDE
#define XKMSRESPONDWITH_INCLUDE

// XSEC Includes

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/xkms/XKMSMessageAbstractType.hpp>

/**
 * @ingroup xkms
 */

/**
 * @brief Interface definition for the RespondWith element
 *
 * The \<RespondWith\> is a very simple element structure that
 * allows a requestor to instruct the service on what kind of information
 * is to be presented back to the caller.
 *
 * The schema definition for RespondWith is as follows :
 *
 * \verbatim
   <!-- RespondWith -->
   <element name="RespondWith" type="QName"/>
   <!-- /RespondWith -->
\endverbatim
 */


class XKMSRespondWith {

	/** @name Constructors and Destructors */
	//@{

protected:

	XKMSRespondWith() {};

public:

	virtual ~XKMSRespondWith() {};

	/** @name Getter Interface Methods */
	//@{

	/*
	 * \brief Obtain the element at the head of this structure
	 *
	 * @returns the owning Element for this structure
	 */

	virtual XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * getElement(void) const = 0;

	/**
	 * \brief Get the RespondWith string for this element
	 *
	 * Each RespondWith element has a text node that defines the Key type to
	 * be returned.
	 *
	 * @returns a pointer to the RespondWith string for this element - note that the
	 * xkms namespace part of the QName (if any) is stripped away.
	 */

	virtual const XMLCh * getRespondWithString(void) const = 0;

	//@}

	/** @name Setter interface methods */
	//@{

	/**
	 * \brief Set the String within the RespondWith element
	 *
	 * Allows a calling application to set a new RespondWith string
	 * 
	 * @param str The string to set (overwriting any previous value).  Do not add
	 * any local name part - this will be added by the implementation.
	 * @note The library does not check that the string is a value RespondWith string.
	 */

	virtual void setRespondWithString(const XMLCh * str) = 0;

	//@}


private:

	// Unimplemented
	XKMSRespondWith(const XKMSRespondWith &);
	XKMSRespondWith & operator = (const XKMSRespondWith &);

};

#endif /* XKMSRESPONDWITH_INCLUDE */
