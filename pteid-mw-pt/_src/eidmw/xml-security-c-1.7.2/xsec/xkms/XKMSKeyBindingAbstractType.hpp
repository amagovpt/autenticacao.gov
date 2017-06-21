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
 * XKMSKeyBindingAbstractType := Interface for base schema of XKMS messages
 *
 * $Id: XKMSKeyBindingAbstractType.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef XKMSKEYBINDINGABSTRACTTYPE_INCLUDE
#define XKMSKEYBINDINGABSTRACTTYPE_INCLUDE

// XSEC Includes

#include <xsec/framework/XSECDefs.hpp>

class DSIGKeyInfoList;
class DSIGKeyInfoMgmtData;
class DSIGKeyInfoName;
class DSIGKeyInfoPGPData;
class DSIGKeyInfoSPKIData;
class DSIGKeyInfoValue;
class DSIGKeyInfoX509;
class XKMSUseKeyWith;

XSEC_DECLARE_XERCES_CLASS(DOMElement);

/**
 * @ingroup xkms
 */

/**
 * @brief Interface definition for the KeyBindingAbstractType
 *
 * The \<KeyBindingAbstractType\> is an abstract type on which all
 * KeyBinding components are build.
 *
 * The schema definition for KeyBindingAbstractType is as follows :
 *
 * \verbatim
   <!-- KeyBindingAbstractType-->
   <complexType name="KeyBindingAbstractType" abstract="true">
      <sequence>
         <element ref="ds:KeyInfo" minOccurs="0"/>
         <element ref="xkms:KeyUsage" minOccurs="0" maxOccurs="3"/>
         <element ref="xkms:UseKeyWith" minOccurs="0" 
               maxOccurs="unbounded"/>
      </sequence>
      <attribute name="Id" type="ID" use="optional"/>
   </complexType>
   <!-- /KeyBindingAbstractType-->
\endverbatim
 */


class XKMSKeyBindingAbstractType {

	/** @name Constructors and Destructors */
	//@{

protected:

	XKMSKeyBindingAbstractType() {};

public:

	virtual ~XKMSKeyBindingAbstractType() {};

	/** @name Getter Interface Methods */
	//@{

	/*
	 * \brief Obtain the base Element for this structure
	 *
	 * @returns the Element node at the head of the DOM structure
	 */

	virtual XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * getElement(void) const = 0;

	/**
	 * \brief Get the Id for the KeyBinding
	 *
	 * All KeyBinding elements MAY have an Id attribute that 
	 * identifies the binding.  This method is used to retrieve a pointer
	 * to the Id string.
	 *
	 * @returns a pointer to the Id string (owned by the library)
	 */

	virtual const XMLCh * getId(void) const = 0;

	/**
	 * \brief Obtain the KeyInfo elements for this KeyBinding
	 *
	 * KeyBinding elements may provide information about the keys that are being
	 * bound.
	 *
	 * @returns A KeyInfoList object containing all the KeyInfo elements
	 */

	virtual DSIGKeyInfoList * getKeyInfoList(void) const = 0;

	/**
	 * \brief Determine if an Encryption key usage is set
	 *
	 * KeyBinding elements may define Encryption, Signature of Exchange as being
	 * permitted for a particular key.
	 *
	 * @returns whether the Encryption KeyUsage element is set
	 */

	virtual bool getEncryptionKeyUsage(void) const = 0;

	/**
	 * \brief Determine if an Signature key usage is set
	 *
	 * KeyBinding elements may define Encryption, Signature of Exchange as being
	 * permitted for a particular key.
	 *
	 * @returns whether the Signature KeyUsage element is set
	 */

	virtual bool getSignatureKeyUsage(void) const = 0;

	/**
	 * \brief Determine if an Exchange key usage is set
	 *
	 * KeyBinding elements may define Encryption, Signature of Exchange as being
	 * permitted for a particular key.
	 *
	 * @returns whether the Exchange KeyUsage element is set
	 */

	virtual bool getExchangeKeyUsage(void) const = 0;

	//@}

	/** @name Setter interface methods */
	//@{

	/**
	 * \brief Set the Id URI for the Message
	 *
	 * Allows a calling application to set a new Id for the
	 * KeyBinding
	 *
	 * @param id The Id to set - if NULL, the library will generate a random Id
	 */

	virtual void setId(const XMLCh * id = NULL) = 0;

	/**
	 * \brief Set Encryption key usage on
	 *
	 * KeyBinding elements may define Encryption, Signature of Exchange as being
	 * permitted for a particular key.  When first created, all these elements
	 * are off (which indicates that all are permitted).
	 *
	 * This call activates the Encryption key usage for this KeyBinding
	 */

	virtual void setEncryptionKeyUsage(void) = 0;

	/**
	 * \brief Set Signature key usage on
	 *
	 * KeyBinding elements may define Encryption, Signature of Exchange as being
	 * permitted for a particular key.  When first created, all these elements
	 * are off (which indicates that all are permitted).
	 *
	 * This call activates the Signature key usage for this KeyBinding
	 */

	virtual void setSignatureKeyUsage(void) = 0;

	/**
	 * \brief Set Exchange key usage on
	 *
	 * KeyBinding elements may define Encryption, Signature of Exchange as being
	 * permitted for a particular key.  When first created, all these elements
	 * are off (which indicates that all are permitted).
	 *
	 * This call activates the Exchange key usage for this KeyBinding
	 */

	virtual void setExchangeKeyUsage(void) = 0;

	//@}

	/** @name KeyInfo Element Manipulation */
	
	//@{

	/**
	 * \brief Get the list of \<KeyInfo\> elements.
	 *
	 * <p>This function recovers list that contains the KeyInfo elements
	 * read in from the DOM document.</p>
	 *
	 * @returns A pointer to the (library owned) KeyInfo list, or NULL
	 * if none exists in the KeyBinding.
	 */
	
	virtual DSIGKeyInfoList * getKeyInfoList(void) = 0;

	/**
	 * \brief Clear out all KeyInfo elements in the KeyBinding.
	 *
	 * This function will delete all KeyInfo elements from both the KeyBinding
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
	 * \brief Append a PGPData element.
	 *
	 * Add a new KeyInfo element for a PGP key.
	 *
	 * @param id The ID of the key to set in the XML (base64 encoded - NULL if none)
	 * @param packet The Packet information to set in the XML (base64 encoded -
	 * NULL if none)
	 * @returns A pointer to the created object
	 */

	virtual DSIGKeyInfoPGPData * appendPGPData(const XMLCh * id, const XMLCh * packet) = 0;

	/**
	 * \brief Append a SPKIData element
	 *
	 * Add a new KeyInfo element for a set of SPKI S-expressions
	 *
	 * @param sexp The initial S-expression to set in the SPKIData element
	 * @returns A pointer to the created object
	 */

	virtual DSIGKeyInfoSPKIData * appendSPKIData(const XMLCh * sexp) = 0;

	/**
	 * \brief Append a MgmtData element
	 *
	 * Add a new KeyInfo element for Management Data
	 *
	 * @param data The string to set in the MgmtData element
	 * @returns A pointer to the created object
	 */

	virtual DSIGKeyInfoMgmtData * appendMgmtData(const XMLCh * data) = 0;

	//@}

	/** @name UseKeyWith handling  */
	//@{

	/**
	 * \brief Find number of UseKeyWith elements
	 *
	 * @returns the number of UseKeyWith items within the KeyBinding
	 */

	virtual int getUseKeyWithSize(void) const = 0;

	/*
	 * \brief Obtain an UseKeyWith item
	 *
	 * @param item Index of the item to return (0 = first element)
	 * @returns the UseKeyWith referenced by "item"
	 */

	virtual XKMSUseKeyWith * getUseKeyWithItem(int item) const = 0;

	/*
	 * \brief Append a new UnverifiedKeyBinding element
	 *
	 * Allows callers to append a new UnverifiedKeyBinding item.
	 * The item is initially empty of KeyInfo elements - these must be added
	 * by the caller.
	 *
	 * @returns the newly created UnverifiedKeyBinding object (already inserted
	 * in the LocateResult
	 */

	virtual XKMSUseKeyWith * appendUseKeyWithItem(
			const XMLCh * application,  
			const XMLCh * identifier) = 0;

	//@}

private:

	// Unimplemented
	XKMSKeyBindingAbstractType(const XKMSKeyBindingAbstractType &);
	XKMSKeyBindingAbstractType & operator = (const XKMSKeyBindingAbstractType &);

};

#endif /* XKMSKEYBINDINGABSTRACTTYPE_INCLUDE */
