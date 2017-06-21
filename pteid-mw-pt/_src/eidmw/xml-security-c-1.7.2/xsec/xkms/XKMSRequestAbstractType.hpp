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
 * XKMSRequestAbstractType := Interface for base schema of XKMS Request messages
 *
 * $Id: XKMSRequestAbstractType.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef XKMSREQUESTABSTRACTTYPE_INCLUDE
#define XKMSREQUESTABSTRACTTYPE_INCLUDE

// XSEC Includes

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/xkms/XKMSMessageAbstractType.hpp>

class XKMSRespondWith;
class XKMSResponseMechanism;

/**
 * @ingroup xkms
 */

/**
 * @brief Interface definition for the RequestAbstractType
 *
 * The \<RequestAbstractType\> is an abstract type on which all
 * XKMS Request messages are built.  
 *
 * The schema definition for RequestAbstractType is as follows :
 *
 * \verbatim
   <!-- RequestAbstractType -->
   <complexType name="RequestAbstractType" abstract="true">
      <complexContent>
         <extension base="xkms:MessageAbstractType">
            <sequence>
               <element ref="xkms:ResponseMechanism" minOccurs="0" 
                     maxOccurs="unbounded"/>
               <element ref="xkms:RespondWith" minOccurs="0" 
                     maxOccurs="unbounded"/>
               <element ref="xkms:PendingNotification" minOccurs="0"/>
            </sequence>
            <attribute name="OriginalRequestId" type="NCName" 
                  use="optional"/>
            <attribute name="ResponseLimit" type="integer" use="optional"/>
         </extension>
      </complexContent>
   </complexType>
   <!-- /RequestAbstractType -->
\endverbatim
 */


class XKMSRequestAbstractType : public XKMSMessageAbstractType {

	/** @name Constructors and Destructors */
	//@{

protected:

	XKMSRequestAbstractType() {};

public:

	virtual ~XKMSRequestAbstractType() {};

	/** @name Getter Interface Methods */
	//@{

	virtual XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * getElement(void) const = 0;

	/**
	 * \brief Get the Original Request Id for the Message
	 *
	 * For transactions that use multipt request/response sessions (e.g.
	 * two stage commit or asyncronous processing), this attribute can be
	 * used to identify the id of the original message that was processed.
	 *
	 * @returns a pointer to the Original Request Id string (owned by the library)
	 */

	virtual const XMLCh * getOriginalRequestId(void) const = 0;

	/**
	 * \brief Get the ResponseLimit size
	 *
	 * A client can limit the number of keys to be returned by the service by
	 * setting this value to the required limit.
	 *
	 * @returns Value of the ResponseLimit within the message.  0 if none was set
	 */

	virtual unsigned int getResponseLimit(void) const = 0;

	//@}

	/** @name Setter interface methods */
	//@{

	/**
	 * \brief Set the Original Reqeust Id for the Message
	 *
	 * Allows a calling application to set a new original request Id for the
	 * message
	 * 
	 * @param id The Id to set as the OriginalRequestId
	 */

	virtual void setOriginalRequestId(const XMLCh * id) = 0;

	/**
	 * \brief Set the ResponseLimit size
	 *
	 * A client can limit the number of keys to be returned by the service by
	 * setting this value to the required limit.
	 *
	 * @param limit Value to set the ResponseLimit within the message
	 */

	virtual void setResponseLimit(unsigned int limit) = 0;

	//@}

	/** @name \<RespondWith\> handling */
	//@{

	/**
	 * \brief Number of RespondWith elements in this Request
	 *
	 * Returns the number of RespondWith elements held within this request
	 *
	 * @returns Number of RespondWith elements
	 */

	virtual int getRespondWithSize(void) = 0;

	/**
	 * \brief Get the nth item in the RespondWith list
	 *
	 * @param item The item to return
	 * @returns the nth item in the RespondWith list.
	 *
	 */

	virtual XKMSRespondWith * getRespondWithItem(int item) = 0;

	/**
	 * \brief Get the string of the nth item in the RespondWith list
	 *
	 * @param item The item whose string is to be returned
	 * @returns the string of the nth item in the RespondWith list.
	 *
	 */

	virtual const XMLCh * getRespondWithItemStr(int item) = 0;

	/**
	 * \brief Append an item to the RespondWith list
	 *
	 * @param item XKMSRespondWith object to append to the list.
	 * @note Once passed in, this item will be owned by the library.
	 */

	virtual void appendRespondWithItem(XKMSRespondWith * item) = 0;

	/**
	 * \brief Shortcut for appending an item to the RespondWith list
	 *
	 * @param item String to create a RespondWith element around and 
	 * append to the list.
	 */

	virtual void appendRespondWithItem(const XMLCh * item) = 0;

	//@}

	/** @name \<ResponseMechanism\> handling */
	//@{

	/**
	 * \brief Number of ResponseMechanism elements in this Request
	 *
	 * Returns the number of ResponseMechanism elements held within this request
	 *
	 * @returns Number of ResponseMechanism elements
	 */

	virtual int getResponseMechanismSize(void) = 0;

	/**
	 * \brief Get the nth item in the ResponseMechanism list
	 *
	 * @param item The item to return
	 * @returns the nth item in the ResponseMechanism list.
	 *
	 */

	virtual XKMSResponseMechanism * getResponseMechanismItem(int item) = 0;

	/**
	 * \brief Get the string of the nth item in the ResponseMechanism list
	 *
	 * @param item The item whose string is to be returned
	 * @returns the string of the nth item in the RespondWith list.
	 *
	 */

	virtual const XMLCh * getResponseMechanismItemStr(int item) = 0;

	/**
	 * \brief Append an item to the ResponseMechanism list
	 *
	 * @param item XKMSResponseMechanism object to append to the list.
	 * @note Once passed in, this item will be owned by the library.
	 */

	virtual void appendResponseMechanismItem(XKMSResponseMechanism * item) = 0;

	/**
	 * \brief Shortcut for appending an item to the ResponseMechanism list
	 *
	 * @param item String to create a ResponseMechanism element around and 
	 * append to the list.
	 */

	virtual void appendResponseMechanismItem(const XMLCh * item) = 0;

	/**
	 * \brief Remove a ResponseMechanism
	 *
	 * Removes the identified ResponseMechanism from the item
	 *
	 * @param item Item to remove
	 */

	virtual void removeResponseMechanismItem(int item) = 0;

	//@}

private:

	// Unimplemented
	XKMSRequestAbstractType(const XKMSRequestAbstractType &);
	XKMSRequestAbstractType & operator = (const XKMSRequestAbstractType &);

};

#endif /* XKMSREQUESTABSTRACTTYPE_INCLUDE */
