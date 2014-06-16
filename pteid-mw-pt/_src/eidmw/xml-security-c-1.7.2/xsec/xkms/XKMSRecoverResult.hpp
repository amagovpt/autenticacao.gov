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
 * XKMSRecoverResult := Interface for RecoverResult Messages
 *
 * $Id$
 *
 */

#ifndef XKMSRECOVERRESULT_INCLUDE
#define XKMSRECOVERRESULT_INCLUDE

// XSEC Includes

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/xkms/XKMSResultType.hpp>
#include <xsec/xkms/XKMSStatus.hpp>

class XKMSKeyBinding;
class XKMSUnverifiedKeyBinding;
class XKMSRSAKeyPair;
class XENCCipherData;

/**
 * @ingroup xkms
 */

/**
 * @brief Interface definition for the RecoverResult elements
 *
 * The \<RecoverResult\> is used by the X-KRSS service to response to
 * a user request to Recover a new key with the service.
 *
 * The schema definition for RecoverResult is as follows :
 *
 * \verbatim
   <!-- RecoverResult -->
   <element name="RecoverResult" type="xkms:RecoverResultType"/>
   <complexType name="RecoverResultType">
      <complexContent>
         <extension base="xkms:ResultType">
            <sequence>
               <element ref="xkms:KeyBinding" minOccurs="0" 
                     maxOccurs="unbounded"/>
               <element ref="xkms:PrivateKey" minOccurs="0"/>
            </sequence>
         </extension>
      </complexContent>
   </complexType>
   <!-- /RecoverResult -->
\endverbatim
 */

class XKMSRecoverResult : public XKMSResultType {

	/** @name Constructors and Destructors */
	//@{

protected:

	XKMSRecoverResult() {};

public:

	virtual ~XKMSRecoverResult() {};

	/** @name Getter Interface Methods */
	//@{

	/**
	 * \brief Return the element at the base of the message
	 */

	virtual XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * getElement(void) const = 0;

	//@}

	/** @name KeyBinding handling  */
	//@{

	/**
	 * \brief Find number of KeyBinding elements
	 *
	 * @returns the number of KeyBinding items within the RecoverResult
	 */

	virtual int getKeyBindingSize(void) const = 0;

	/*
	 * \brief Obtain an KeyBinding item
	 *
	 * @param item Index of the item to return (0 = first element)
	 * @returns the KeyBinding referenced by "item"
	 */

	virtual XKMSKeyBinding * getKeyBindingItem(int item) const = 0;

	/*
	 * \brief Append a new KeyBinding element
	 *
	 * Allows callers to append a new KeyBinding item.
	 * The item is initially empty of KeyInfo elements - these must be added
	 * by the caller.
	 *
	 * @param status The status (Valid, Invalid or Indeterminate) of this
	 * key
	 * @returns the newly created KeyBinding object (already inserted
	 * in the RecoverResult
	 */

	virtual XKMSKeyBinding * appendKeyBindingItem(XKMSStatus::StatusValue status) = 0;

	//@}

	/** @name PrivateKey handling  */
	//@{

	/**
	 * \brief Return the RSAKeyPair
	 *
	 * This call requires the passphrase used to encrypt the private key.
	 * The implementation decrypts the PrivateKey element in the
	 * RecoverResult and returns the resulting RSAKeyPair.
	 *
	 * @note The decryption is performed *inside* the RecoverResult, so
	 * this actually modified the contents of the XML.  It should never
	 * be called twice!
	 *
	 * @param passPhrase The local code page version of the pass phrase
	 * @returns the number of KeyBinding items within the RecoverResult
	 */

	virtual XKMSRSAKeyPair * getRSAKeyPair(const char * passPhrase) = 0;

	/**
	 * \brief Add the RSAKeyPair in an encrypted PrivateKey
	 *
	 * This call requires the passphrase to encrypt the private key.
	 * The implementation encrypts the RSAKeyPair and adds the result
	 * to the resulting RSAKey Pair.  It returns the CipherData element
	 * to the caller (not a structure for the PrivateKey)
	 *
	 * @note The encryption is performed *inside* the RecoverResult, so
	 * this actually modified the contents of the XML.  It should never
	 * be called twice!
	 *
	 * @param passPhrase The local code page version of the pass phrase
	 * @param Modulus Base64 encoded string with the modulus
	 * @param Exponent Base64 encoded string with the exponent
	 * @param P Base64 encoded string with p
	 * @param Q Base64 encoded string with q
	 * @param DP Base64 encoded string with dp
	 * @param DQ Base64 encoded string with dq
	 * @param InverseQ Base64 encoded string with inverseq
	 * @param D Base64 encoded string with d
	 * @param em The encryptionMethod to use for this encryption.  Use
	 * ENCRYPT_NONE if a user defined type is required.
	 * @param algorithmURI If ENCRYPT_NONE is passed in, this will be
	 * used to set the algorithm URI.  If this is also NULL - no
	 * EncryptionMethod will be set.  <b>NULL Value Unsupported if em not
	 * set!  It's use could cause problems!</b>
	 * @returns The encrypted result of adding the info
	 */

	virtual XENCEncryptedData * setRSAKeyPair(const char * passPhrase,
		XMLCh * Modulus,
		XMLCh * Exponent,
		XMLCh * P,
		XMLCh * Q,
		XMLCh * DP,
		XMLCh * DQ,
		XMLCh * InverseQ,
		XMLCh * D,
		encryptionMethod em,
		const XMLCh * algorithmURI = NULL) = 0;

	//@}


private:

	// Unimplemented
	XKMSRecoverResult(const XKMSRecoverResult &);
	XKMSRecoverResult & operator = (const XKMSRecoverResult &);

};

#endif /* XKMSRECOVERRESULT_INCLUDE */
