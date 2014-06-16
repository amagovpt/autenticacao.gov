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
 * XENCCipherData := Interface for CipherData elements 
 *
 * $Id: XENCCipherData.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef XSECCIPHERDATA_INCLUDE
#define XSECCIPHERDATA_INCLUDE

// XSEC Includes

#include <xsec/framework/XSECDefs.hpp>

#include <xsec/xenc/XENCCipherValue.hpp>
#include <xsec/xenc/XENCCipherReference.hpp>

/**
 * @ingroup xenc
 */

/**
 * @brief Interface definition for the CipherData object
 *
 * The \<CipherData\> element either holds the encrypted data (via
 * a CipherValue element) or a reference to the encrypted data.
 *
 * Within the library, the CipherData element can only be used to
 * hold and read the encrypted data.  To actually decrypt/encrypt
 * data, an XENCEncryptedType derivative object of XENCCipher object 
 * should be used.
 *
 * The schema for CipherData is as follows:
 *
 * \verbatim
<element name='CipherData' type='xenc:CipherDataType'/>
  <complexType name='CipherDataType'>
     <choice>
       <element name='CipherValue' type='base64Binary'/>
       <element ref='xenc:CipherReference'/>
     </choice>
   </complexType>
\endverbatim
 */


class XENCCipherData {

public:

	/**
	 * CipherDataType
	 */

	enum XENCCipherDataType {

		NO_TYPE			= 0,    /** Not Set */
		VALUE_TYPE		= 1,
		REFERENCE_TYPE	= 2

	};

	/** @name Constructors and Destructors */
	//@{

protected:

	XENCCipherData() {};

public:

	virtual ~XENCCipherData() {};

	/** @name Get Interface Methods */
	//@{

	/**
	 * \brief Find the type of Cipher Data held
	 *
	 * CipherData elements can hold either a CipherValue element (whose
	 * text is the encrypted data) or a CipherReference element, where the
	 * contents tell the library how to retrieve the encrypted data from
	 * elsewhere.
	 *
	 * @returns The type of CipherData
	 */

	virtual XENCCipherDataType getCipherDataType(void) = 0;

	/**
	 * \brief Get the CipherValue element
	 *
	 * @returns the CipherValue element, or NULL if one is not held
	 */

	virtual XENCCipherValue * getCipherValue(void) = 0;

	/**
	 * \brief Get the CipherReference element
	 *
	 * @returns the CipherReference element, or NULL if one is not held
	 */

	virtual XENCCipherReference * getCipherReference(void) = 0;

	/**
	 * \brief Get the DOM Node of this structure
	 *
	 * @returns the DOM Node representing the \<CipherData\> element
	 */

	virtual XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * getElement(void) = 0;

	//@}

private:

	// Unimplemented
	XENCCipherData(const XENCCipherData &);
	XENCCipherData & operator = (const XENCCipherData &);

};

#endif /* XENCCIPHERDATA_INCLUDE */

