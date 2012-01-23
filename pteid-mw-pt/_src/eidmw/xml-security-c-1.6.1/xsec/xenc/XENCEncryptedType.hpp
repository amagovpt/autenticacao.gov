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
 * XENCEncryptedType := Definition for holder object for EncryptedType 
 * element
 *
 * Author(s): Berin Lautenbach
 *
 * $Id: XENCEncryptedType.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef XENCENCRYPTEDTYPE_INCLUDE
#define XENCENCRYPTEDTYPE_INCLUDE

// XSEC Includes

#include <xsec/framework/XSECDefs.hpp>

class XENCCipherData;
class DSIGKeyInfoList;
class DSIGKeyInfoName;
class DSIGKeyInfoValue;
class DSIGKeyInfoX509;
class XENCEncryptionMethod;
class XENCEncryptedKey;

/**
 * @ingroup xenc
 */

/**
 * @brief Interface definition for the EncryptedType object
 *
 * The \<EncryptedType\> element is an abstract type on which
 * EncryptedData and EncryptedKey objects are built.
 *
 * This is the base class on which most of the XML Encryption
 * standard is built.  Using classes derived from this, 
 * calling programs can decrypt the content, determine KeyInfo
 * references etc.
 *
 * In general derived objects should not be used directly.
 * The XENCCipher class should be used to operate on them.
 *
 * The schema definition for EncryptedType is as follows :
 *
 * \verbatim
  <complexType name='EncryptedType' abstract='true'>
    <sequence>
      <element name='EncryptionMethod' type='xenc:EncryptionMethodType' 
               minOccurs='0'/>
      <element ref='ds:KeyInfo' minOccurs='0'/>
      <element ref='xenc:CipherData'/>
      <element ref='xenc:EncryptionProperties' minOccurs='0'/>
    </sequence>
    <attribute name='Id' type='ID' use='optional'/>
    <attribute name='Type' type='anyURI' use='optional'/>
    <attribute name='MimeType' type='string' use='optional'/>
    <attribute name='Encoding' type='anyURI' use='optional'/> 
   </complexType>
  \endverbatim
 */


class XENCEncryptedType {

	/** @name Constructors and Destructors */
	//@{

protected:

	XENCEncryptedType() {};

public:

	virtual ~XENCEncryptedType() {};

	/** @name Basic Interface Methods */
	//@{

	/**
	 * \brief Retrieve the CipherData element
	 *
	 * CipherData elements are the sub part of the EncryptedData
	 * that hold the actual enciphered information.
	 *
	 * @returns The CipherData object
	 */

	virtual XENCCipherData * getCipherData(void) const = 0;

	/**
	 * \brief Retrieve the EncryptionMethod element
	 *
	 * The EncryptionMethod element holds information about the 
	 * encryption algorithm to be used to encrypt/decrypt the data
	 *
	 * This method provides a means to extract the EncryptionMethod
	 * element from the EncryptedType
	 *
	 * @returns The EncryptionMethod element
	 */

	virtual XENCEncryptionMethod * getEncryptionMethod(void) const = 0;

	/**
	 * \brief Get the DOM Element Node of this structure
	 *
	 * @returns the DOM Element Node representing the \<EncryptionType\> element
	 */

	virtual XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * getElement(void) const = 0;

	//@}

	/** @name Getter interface Methods */
	//@{

	/**
	 * \brief Get the Type URI for the EncryptedType
	 *
	 * If this object is an EncryptedData, it <em>may</em> have a 
	 * Type attribute that defines whether it is an encrypted
	 * Element or Element Content.  This method allows the caller
	 * to see this type URI.
	 *
	 * @returns a pointer to the URI string (owned by the library)
	 * or NULL if no type is set
	 */

	virtual const XMLCh * getType(void) const = 0;

	/**
	 * \brief Get the MimeType of the EncryptedType
	 *
	 * If this object is an EncryptedData, it <em>may</em> have a 
	 * MimeType attribute that "describes the media type of the 
	 * data which has been encrypted" (from the XML Encryption spec).
	 *
	 * The XML-Security-C library makes no use of this attribute, but
	 * it provides these functions to allow applications to set and get.
	 *
	 * @returns a pointer to the MimeType string (owned by the library)
	 * or NULL if no MimeType is set
	 */

	virtual const XMLCh * getMimeType(void) const = 0;

	/**
	 * \brief Get the Encoding of the EncryptedType
	 *
	 * If this object is an EncryptedData, it <em>may</em> have an
	 * encoding attribute that describes how the data has been encoded
	 * prior to encryption.  (E.g. http://www.w3.org/2000/09/xmldsig#base64)
	 *
	 * The XML-Security-C library makes no use of this attribute, but
	 * it provides these functions to allow applications to set and get.
	 *
	 * @returns A string (owned by the library) providing the encoding URI
	 */

	virtual const XMLCh * getEncoding(void) const = 0;

	//@}

	/** @name Setter interface methods */
	//@{

	/**
	 * \brief Set the Type URI for the EncryptedType
	 *
	 * Allows a calling application to set a particular Type URI for
	 * the EncryptedType.
	 * 
	 * @note calls to encryptElement and encryptElementContent
	 * automatically set the appropriate Type URI.
	 *
	 * @param uri The URI to set
	 */

	virtual void setType(const XMLCh * uri) = 0;

	/**
	 * \brief Set the MimeType of the EncryptedType
	 *
	 * If this object is an EncryptedData, it <em>may</em> have a 
	 * MimeType attribute that "describes the media type of the 
	 * data which has been encrypted" (from the XML Encryption spec).
	 *
	 * The XML-Security-C library makes no use of this attribute, but
	 * it provides these functions to allow applications to set and get.
	 *
	 * @param mimeType String to set in the MimeType attribute.
	 * @note no checking of this string is done by the library - it
	 * simply sets the value of the MimeType attribute to this value.
	 */

	virtual void setMimeType(const XMLCh * mimeType) = 0;

	/**
	 * \brief Set the Encoding of the EncryptedType
	 *
	 * If this object is an EncryptedData, it <em>may</em> have an
	 * encoding attribute that describes how the data has been encoded
	 * prior to encryption.  (E.g. http://www.w3.org/2000/09/xmldsig#base64)
	 *
	 * The XML-Security-C library makes no use of this attribute, but
	 * it provides these functions to allow applications to set and get.
	 *
	 * @param uri String (URI) to set in the Encoding attribute.
	 * @note no checking of this string is done by the library - it
	 * simply sets the value of the Encoding attribute to this value.
	 */

	virtual void setEncoding(const XMLCh * uri) = 0;

	//@}

	/** @name KeyInfo Element Manipulation */
	
	//@{

	/**
	 * \brief Get the list of \<KeyInfo\> elements.
	 *
	 * <p>This function recovers list that contains the KeyInfo elements
	 * read in from the DOM document.</p>
	 *
	 * <p>This list should be used by calling applications to determine what key
	 * is appropriate for decrypting the document.</p>
	 *
	 * @note The list should never be modified directly.  If you need to
	 * add keyInfo elements, call the appropriate functions in EncryptedType
	 *
	 * @returns A pointer to the DSIGKeyInfoList object held by the XENCCipher
	 */
	
	virtual DSIGKeyInfoList * getKeyInfoList(void) = 0;

	/**
	 * \brief Clear out all KeyInfo elements in the signature.
	 *
	 * This function will delete all KeyInfo elements from both the EncryptedType
	 * object <em>and the associated DOM</em>.
	 *
	 */

	virtual void clearKeyInfo(void) = 0;

	/**
	 * \brief Append a DSA KeyValue element 
	 *
	 * Add a new KeyInfo element for a DSA Value
	 *
	 * @param P Base64 encoded value of P
	 * @param Q Base64 encoded value of Q
	 * @param G Base64 encoded value of G
	 * @param Y Base64 encoded value of Y
	 * @returns A pointer to the created object.
	 */

	virtual DSIGKeyInfoValue * appendDSAKeyValue(const XMLCh * P, 
						   const XMLCh * Q, 
						   const XMLCh * G, 
						   const XMLCh * Y) = 0;

	/**
	 * \brief Append a RSA KeyValue element 
	 *
	 * Add a new KeyInfo element for a RSA Value
	 *
	 * @param modulus Base64 encoded value of the modulus
	 * @param exponent Base64 encoded value of exponent
	 * @returns A pointer to the created object.
	 */

	virtual DSIGKeyInfoValue * appendRSAKeyValue(const XMLCh * modulus, 
						   const XMLCh * exponent) = 0;

	/**
	 * \brief Append a X509Data element.
	 *
	 * Add a new KeyInfo element for X509 data.
	 *
	 * @note The added element is empty.  The caller must make use of the
	 * returned object to set the required values.
	 *
	 * @returns A pointer to the created object.
	 */

	virtual DSIGKeyInfoX509 * appendX509Data(void) = 0;

	/**
	 * \brief Append a KeyName element.
	 *
	 * Add a new KeyInfo element for a key name.
	 *
	 * @param name The name of the key to set in the XML
	 * @param isDName Treat the name as a Distinguished name and encode accordingly
	 * @returns A pointer to the created object
	 */

	virtual DSIGKeyInfoName * appendKeyName(const XMLCh * name, bool isDName = false) = 0;

	/**
	 * \brief Append an already created EncryptedKey.
	 *
	 * Add an already created EncryptedKey.
	 *
	 * @note The encryptedKey becomes the property of the owning EncryptedType
	 * object and will be deleted upon its destruction.
	 *
	 * @param encryptedKey A pointer to the encrypted Key
	 */

	virtual void appendEncryptedKey(XENCEncryptedKey * encryptedKey) = 0;
	//@}

private:

	// Unimplemented
	XENCEncryptedType(const XENCEncryptedType &);
	XENCEncryptedType & operator = (const XENCEncryptedType &);

};

#endif /* XENCENCRYPTEDTYPE_INCLUDE */
