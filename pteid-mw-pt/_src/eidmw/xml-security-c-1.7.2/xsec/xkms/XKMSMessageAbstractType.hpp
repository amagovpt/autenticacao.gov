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
 * XKMSMessageAbstractType := Interface for base schema of XKMS messages
 *
 * $Id: XKMSMessageAbstractType.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef XKMSMESSAGEABSTRACTTYPE_INCLUDE
#define XKMSMESSAGEABSTRACTTYPE_INCLUDE

// XSEC Includes

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/dsig/DSIGConstants.hpp>

class DSIGSignature;

XSEC_DECLARE_XERCES_CLASS(DOMElement);

/**
 * @ingroup xkms
 */

/**
 * @brief Interface definition for the MessageAbstractType
 *
 * The \<MessageAbstractType\> is an abstract type on which all
 * other XKMS messages are built.  All request/response messages
 * have :
 * <ul>
 *  <li><b>Id</b> The unique ID of this message</li>
 *  <li><b>Service</b> The URI of the service for which this message is
 * destined or comes from.</li>
 *  <li><b>Nonce</b> (Optional) Random data used to defend against replay attacks.
 * </ul>
 *
 * In addition, such messages may have :
 *
 * <ul>
 *  <li><b>Signature</b> Validating authenticity of this message</li>
 *  <li><b>MessageExtension</b> Used for extensibility</li>
 *  <li><b>OpaqueClientData</b> Used for application specific info</li>
 * </ul>
 *
 * The schema definition for MessageAbstractType is as follows :
 *
 * \verbatim
   <!-- MessageAbstractType -->
   <complexType name="MessageAbstractType" abstract="true">
      <sequence>
         <element ref="ds:Signature" minOccurs="0"/>
         <element ref="xkms:MessageExtension" minOccurs="0" 
               maxOccurs="unbounded"/>
         <element ref="xkms:OpaqueClientData" minOccurs="0"/>
      </sequence>
      <attribute name="Id" type="ID" use="required"/>
      <attribute name="Service" type="anyURI" use="required"/>
      <attribute name="Nonce" type="base64Binary" use="optional"/>
   </complexType>
   <!-- /MessageAbstractType -->

  \endverbatim
 */


class XKMSMessageAbstractType {

	/** @name Constructors and Destructors */
	//@{

protected:

	XKMSMessageAbstractType() {};

public:

	enum messageType {

		None,					/** A default value that indicates something hasn't been set */

		/* X-KISS messages */

		CompoundRequest,		/** A CompoundRequest message - holding multiple requests */
		CompoundResult,			/** A CompoundResult message - holding multiple results */
		LocateRequest,			/** A LocateRequest message */
		LocateResult,			/** A LocateResult message */
		ValidateRequest,		/** A ValidateRequest message */
		ValidateResult,			/** A ValidateResult message */
		PendingRequest,			/** A PendingRequest message (Asynchronous protocol) */
		StatusRequest,          /** A StatusRequest message (Asynchronous protocol) */
		StatusResult,           /** A StatusResult message (Asynchronous protocol) */
		Result,					/** A non-specific result message */

		/* X-KRSS messages */

		RegisterRequest,		/** A RegisterReqeust message */
		RegisterResult,			/** A RegisterResult message */
		RevokeRequest,			/** A RevokeRequest message */
		RevokeResult,			/** A RevokeResult message */
		RecoverRequest,			/** A RecoverRequest message */
		RecoverResult,			/** A RecoverResult message */
		ReissueRequest,			/** A ReissueRequest message */
		ReissueResult			/** A ReissueResult message */

	};

	virtual ~XKMSMessageAbstractType() {};

	/** @name Message Manipulation Methods */
	//@{

	virtual messageType getMessageType(void) = 0;

	/** @name Getter Interface Methods */
	//@{

	/**
	 * \brief Find out whether this is a signed Message
	 *
	 * @returns true if the message is signed
	 */

	virtual bool isSigned(void) const = 0;

	/**
	 * \brief Retrieve the Signature information (if any is attached)
	 *
	 * All XKMS messages MAY have a \<Signature\> node that is used
	 * to attest to the validity of the message.
	 *
	 * @returns The DSIGSignature object associated with this Message,
	 * or NULL if none is defined
	 */

	virtual DSIGSignature * getSignature(void) const = 0;

	/*
	 * \brief Obtain the base Element for this structure
	 *
	 * @returns the Element node at the head of the DOM structure
	 */

	virtual XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * getElement(void) const = 0;

	/**
	 * \brief Get the Id for the Message
	 *
	 * All messages are required to have an Id attribute that uniquely
	 * identifies the message.  This method is used to retrieve a pointer
	 * to the Id string.
	 *
	 * @returns a pointer to the Id string (owned by the library)
	 */

	virtual const XMLCh * getId(void) const = 0;

	/**
	 * \brief Get the Service identifier for this message
	 *
	 * All Messages are associated with a particular XKMS service.  This
	 * method can be used to retrieve the Service URI for a particular
	 * message.
	 *
	 * @returns a pointer to the Service URI string (owned by the library)
	 *
	 */

	virtual const XMLCh * getService(void) const = 0;

	/**
	 * \brief Get the Nonce of the message
	 *
	 * All messages MAY have a nonce, used to provide cryptographically
	 * random data that will help defend against a replay attack.
	 *
	 * @returns A string (owned by the library) providing the nonce data in
	 * base64 encoded form.  If no Nonce is set, NULL is returned.
	 */

	virtual const XMLCh * getNonce(void) const = 0;

	//@}

	/** @name Setter interface methods */
	//@{

	/**
	 * \brief Set the Id URI for the Message
	 *
	 * Allows a calling application to set a new Id for the
	 * message
	 * 
	 * Note: Great care should be taken on resetting Id attributes, as
	 * they are used to track a particular transaction through its
	 * liefcycle.
	 *
	 * @param id The Id to set
	 */

	virtual void setId(const XMLCh * id) = 0;

	/**
	 * \brief Set the Service for the Message
	 *
	 * Used to set a new service URI for a particular message.  In general,
	 * the service attribute would not be expected to change.
	 *
	 *
	 * @param service String to set in the Service attribute.
	 */

	virtual void setService(const XMLCh * service) = 0;

	/**
	 * \brief Set the Nonce value
	 *
	 * Allows the application to set a new Nonce value (overwriting any
	 * that is currently set).
	 *
	 * @param uri String (base64 encoded data) to set in the Nonce attribute.
	 * @note no checking of this string is done by the library - it
	 * is assumed that it is a valid base64 encoding.
	 */

	virtual void setNonce(const XMLCh * uri) = 0;

	/**
	 * \brief Add a signature to the message
	 *
	 * Allows an application to sign the message
	 *
	 * @returns the new Signature structure
	 */

	virtual DSIGSignature * addSignature(
		canonicalizationMethod cm = CANON_C14N_NOC,
		signatureMethod	sm = SIGNATURE_DSA,
		hashMethod hm = HASH_SHA1) = 0;

	//@}

	/** @name Opaque client data handling */
	//@{

	/**
	 * \brief Find the number of \<OpaqueData\> elements
	 *
	 * The \<OpaqueClientData\> element can have multiple
	 * \<OpaqueData\> children.  This method allows an application
	 * to determine how many such children exist
	 *
	 * @return Number of \<OpaqueData\> elements
	 */

	virtual int getOpaqueClientDataSize(void) = 0;

	/*
	 * \brief Get the nth item in the OpaqueClientData list
	 *
	 * @param item The item to return
	 * @returns the string of the nth item in the OpaqueClientData list.
	 *
	 */

	virtual const XMLCh * getOpaqueClientDataItemStr(int item) = 0;

	/*
	 * \brief Append an item to the OpaqueClientData list
	 *
	 * @param item Base64 string to append to the list.  Will be
	 * wrapped in a OpaqueData element.
	 */

	virtual void appendOpaqueClientDataItem(const XMLCh * item) = 0;

	//@}

private:

	// Unimplemented
	XKMSMessageAbstractType(const XKMSMessageAbstractType &);
	XKMSMessageAbstractType & operator = (const XKMSMessageAbstractType &);

};

#endif /* XKMSMESSAGEABSTRACTTYPE_INCLUDE */
