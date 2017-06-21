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
 * XENCEncryptedKey := Definition for holder object for EncryptedKey 
 *
 * $Id: XENCEncryptedKey.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef XENCENCRYPTEDKEY_INCLUDE
#define XENCENCRYPTEDKEY_INCLUDE

// XSEC Includes

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/dsig/DSIGKeyInfo.hpp>
#include <xsec/xenc/XENCEncryptedType.hpp>
#include <xsec/xenc/XENCCipherData.hpp>

/**
 * @ingroup xenc
 */

/**
 * @brief Interface definition for the EncryptedKey object
 *
 * The \<EncryptedKey\> element is an abstract type which builds
 * on the EncryptedType element for encrypted data (as opposed to
 * encrypted data).
 *
 * In general, this class should not be used directly.  For most
 * applications, callers will want to use the XENCCipher class
 * instead.
 *
 * The schema definition for EncryptedKey is as follows:
 *
 * \verbatim
  <element name='EncryptedKey' type='xenc:EncryptedKeyType'/>
  <complexType name='EncryptedKeyType'>
    <complexContent>
      <extension base='xenc:EncryptedType'>
        <sequence>
          <element ref='xenc:ReferenceList' minOccurs='0'/>
          <element name='CarriedKeyName' type='string' minOccurs='0'/>
        </sequence>
        <attribute name='Recipient' type='string' use='optional'/>
      </extension>
    </complexContent>   
  </complexType>
\endverbatim
 */


class XENCEncryptedKey : public XENCEncryptedType, public DSIGKeyInfo {

	/** @name Constructors and Destructors */
	//@{

protected:

	// Because we inherit from KeyInfo, we need to implement a slightly different 
	// constructor.

	XENCEncryptedKey(const XSECEnv * env) : DSIGKeyInfo(env) {};

public:

	virtual ~XENCEncryptedKey() {};

	/** @name EncryptedKey Specific Getter Methods */
	//@{

	/**
	 * \brief Get the CarriedKeyName
	 *
	 * EncryptedKey elements MAY have a CarriedKeyName element that links
	 * the EncryptedKey to a KeyName KeyInfo element in another EncryptedKey
	 * or EncryptedData element.
	 * 
	 * This method allows applications to retrieve the Carried Key Name for
	 * the particular EncryptedKey
	 *
	 * @returns A pointer (owned by the library) to the CarriedKeyName string 
	 * (or NULL if none)
	 */

	virtual const XMLCh * getCarriedKeyName(void) const = 0;

	/**
	 * \brief Get the Recipient name
	 *
	 * EncryptedKey elements MAY have a Recipient Attribute on the main
	 * EncryptedKey element that provide a hint to the application as to who
	 * the recipient of the key is.
	 *
	 * This method returns this string in cases where it has been provided
	 *
	 * @returns A pointer (owned by the library) to the Recipient string
	 * (or NULL if none provided).
	 */

	virtual const XMLCh * getRecipient(void) const = 0;

	//@}

	/** @name EncryptedKey Specific Setter Methods */
	//@{

	/**
	 * \brief Set the CarriedKeyName
	 *
	 * EncryptedKey elements MAY have a CarriedKeyName element that links
	 * the EncryptedKey to a KeyName KeyInfo element in another EncryptedKey
	 * or EncryptedData element.
	 * 
	 * This method allows applications to set the Carried Key Name for
	 * the particular EncryptedKey
	 *
	 * @param name String to set in the CarriedKeyName element
	 */

	virtual void setCarriedKeyName(const XMLCh * name) = 0;

	/**
	 * \brief Set the Recipient name
	 *
	 * EncryptedKey elements MAY have a Recipient Attribute on the main
	 * EncryptedKey element that provide a hint to the application as to who
	 * the recipient of the key is.
	 *
	 * This method sets the Recipient string
	 *
	 * @param recipient String to set in the Recipient attribute
	 */

	virtual void setRecipient(const XMLCh * recipient) = 0;

	//@}

private:

	// Unimplemented
	XENCEncryptedKey(const XENCEncryptedKey &);
	XENCEncryptedKey & operator = (const XENCEncryptedKey &);


};

#endif /* XENCENCRYPTEDKEY_INCLUDE */
