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
 * XENCEncryptionMethod := Interface definition for EncryptionMethod element
 *
 * $Id: XENCEncryptionMethod.hpp 1350045 2012-06-13 22:33:10Z scantor $
 *
 */

#ifndef XENCENCRYPTIONMETHOD_INCLUDE
#define XENCENCRYPTIONMETHOD_INCLUDE

// XSEC Includes

#include <xsec/framework/XSECDefs.hpp>

/**
 * @ingroup xenc
 */

/**
 * @brief Interface definition for the EncryptionMethod object
 *
 * The \<EncryptionMethod\> element holds information about the 
 * encryption algorithm being used.
 *
 * This element is optional within an EncryptedType derivative,
 * but applications not making use of this need to know the 
 * this information, otherwise the library will not be able to
 * decrypt the data.
 *
 * It is defined as :
 * \verbatim
  <complexType name='EncryptionMethodType' mixed='true'>
    <sequence>
      <element name='KeySize' minOccurs='0' type='xenc:KeySizeType'/>
      <element name='OAEPparams' minOccurs='0' type='base64Binary'/>
      <any namespace='##other' minOccurs='0' maxOccurs='unbounded'/>
    </sequence>
    <attribute name='Algorithm' type='anyURI' use='required'/>
  </complexType>
  \endverbatim
 */


class XENCEncryptionMethod {

public:

	XENCEncryptionMethod() {};

	virtual ~XENCEncryptionMethod() {};

	/** @name Getter Methods */
	//@{

	/**
	 * \brief Get the algorithm
	 *
	 * Return the Algorithm URI representing the encryption type for this
	 * encrypted data
	 *
	 * @returns the URI representing the algorithm
	 */

	virtual const XMLCh * getAlgorithm(void) const = 0;

	/**
	 * \brief Get the digest method URI
	 *
	 * Return the Algorithm URI represtenting the Digest Method for those
	 * encryption algorithms that require it (such as RSA with OAEP padding)
	 *
	 * @returns the URI representing the digest method algorithm
	 */

	virtual const XMLCh * getDigestMethod(void) const = 0;

	/**
	 * \brief Get the value of the OAEPparams string
	 *
	 * The OAEP RSA padding method allows a user to set an optional
	 * params string (that will be used as input to the Digest algorithm).
	 *
	 * @returns The string (base64 encoded value) representing the OAEP params
	 */

	virtual const XMLCh * getOAEPparams(void) const = 0;

	/**
	 * \brief Get the MGF URI
	 *
	 * Return the Algorithm URI represtenting the Mask Generation Function for those
	 * encryption algorithms that require it (such as RSA with OAEP padding)
	 *
	 * @returns the URI representing the mask generation function
	 */

	virtual const XMLCh * getMGF(void) const = 0;

	/**
	 * \brief Get the KeySize that was set in this EncryptionMethod.
	 *
	 * This field would not normally be used for the encryption algorithms
	 * explicitly referenced in the XML Encryption standard.  It is provided
	 * mainly for stream ciphers that have a variable key length
	 */

	virtual int getKeySize(void) const = 0;

	/**
	 * \brief Get the DOM Element Node of this structure
	 *
	 * @returns the DOM Element Node representing the \<EncryptionMethod\> element
	 */

	virtual XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * getElement(void) const = 0;


	//@}

	/** @name Setter Methods */
	//@{

	/**
	 * \brief Set the value of the DigestMethod
	 *
	 * Sets the DigestMethod element's Algorithm attribute to the passed in
	 * value - should be a URI string
	 *
	 * @param method String to set in the Algorithm attribute.  Will create a
	 * \<DigestMethod\> element if one does not already exist
	 */

	virtual void setDigestMethod(const XMLCh * method) = 0;

	/**
	 * \brief Set the value of the OAEPparams string
	 *
	 * Sets the OAEPparams element's Text node child to the passed in
	 * value - should be a base64 encoded value
	 *
	 * @param params String to set in the OAEPparams text node.  Will create a
	 * \<OAEPparams\> element if one does not already exist
	 */

	virtual void setOAEPparams(const XMLCh * params) = 0;

	/**
	 * \brief Set the value of the MGF
	 *
	 * Sets the MGF element's Algorithm attribute to the passed in
	 * value - should be a URI string
	 *
	 * @param method String to set in the Algorithm attribute.  Will create a
	 * \<xenc11:MGF\> element if one does not already exist
	 */

	virtual void setMGF(const XMLCh * mgf) = 0;

	/**
	 * \brief Set the KeySize that in this EncryptionMethod.
	 *
	 * This field would not normally be used for the encryption algorithms
	 * explicitly referenced in the XML Encryption standard.  It is provided
	 * mainly for stream ciphers that have a variable key length
	 */

	virtual void setKeySize(int size) = 0;

	//@}

private:

	// Unimplemented
	XENCEncryptionMethod(const XENCEncryptionMethod &);
	XENCEncryptionMethod & operator = (const XENCEncryptionMethod &);

};

#endif /* XENCENCRYPTIONMETHOD_INCLUDE */
