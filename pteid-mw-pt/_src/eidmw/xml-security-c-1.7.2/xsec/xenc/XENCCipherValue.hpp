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
 * XENCCipherValue := Interface definition for CipherValue element
 *
 * $Id: XENCCipherValue.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef XENCCIPHERVALUE_INCLUDE
#define XENCCIPHERVALUE_INCLUDE

// XSEC Includes

#include <xsec/framework/XSECDefs.hpp>

XSEC_DECLARE_XERCES_CLASS(DOMElement);

/**
 * @ingroup xenc
 */

/**
 * @brief Interface definition for the CipherValue object
 *
 * The \<CipherValue\> element holds the base64 encoded, encrypted data.
 * This is a very simple class that acts purely as a holder of data.
 *
 */


class XENCCipherValue {

	/** @name Constructors and Destructors */
	//@{

protected:

	XENCCipherValue() {};

public:

	virtual ~XENCCipherValue() {};

	/** @name Get Interface Methods */
	//@{

	/**
	 * \brief Get the encrypted information
	 *
	 * CipherValue nodes contain a text child that holds the base64 encoded
	 * cipher text that needs to be decrypted.  This call will return the
	 * base64 encoded string.
	 *
	 * @returns The Encrypted information in a base64 encoded string
	 */

	virtual const XMLCh * getCipherString(void) const = 0;

	/**
	 * \brief Get the DOM Node of this structure
	 *
	 * @returns the DOM Node representing the \<CipherValue\> element
	 */

	virtual XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * getElement(void) const = 0;

	//@}

	/** @name Set Interface Methods */
	//@{

	/**
	 * \brief Set the encrypted value
	 *
	 * Sets the base64 encoded string held in the CipherValue.
	 *
	 * @note This should not normally be called directly - use the XENCCipher
	 * interface for normal operation
	 *
	 * @param value The string to set
	 */

	virtual void setCipherString(const XMLCh * value) = 0;

	//@}

private:

	// Unimplemented
	XENCCipherValue(const XENCCipherValue &);
	XENCCipherValue & operator = (const XENCCipherValue &);

};

#endif /* XENCCIPHERVALUE_INCLUDE */

