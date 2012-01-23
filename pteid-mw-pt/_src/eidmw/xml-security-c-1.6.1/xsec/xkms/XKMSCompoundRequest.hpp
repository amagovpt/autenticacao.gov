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
 * XKMSCompoundRequest := Interface for CompoundRequest Messages
 *
 * $Id: XKMSCompoundRequest.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef XKMSCOMPOUNDREQUEST_INCLUDE
#define XKMSCOMPOUNDREQUEST_INCLUDE

// XSEC Includes

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/xkms/XKMSRequestAbstractType.hpp>

class XKMSLocateRequest;
class XKMSValidateRequest;
class XKMSRegisterRequest;
class XKMSRevokeRequest;
class XKMSReissueRequest;
class XKMSRecoverRequest;

/**
 * @ingroup xkms
 */

/**
 * @brief Interface definition for the CompoundRequest elements
 *
 * The \<CompoundRequest\> is used to merge a number of separate requests
 * into a single message.  It is a very simple type - simply a holding
 * Request that has all the sub-requests held within it.
 *
 * The schema definition for CompoundRequest is as follows :
 *
 * \verbatim
   <!-- CompoundRequest -->
   <element name="CompoundRequest" type="xkms:CompoundRequestType"/>
   <complexType name="CompoundRequestType">
      <complexContent>
         <extension base="xkms:RequestAbstractType">
            <choice maxOccurs="unbounded">
               <element ref="xkms:LocateRequest"/>
               <element ref="xkms:ValidateRequest"/>
               <element ref="xkms:RegisterRequest"/>
               <element ref="xkms:ReissueRequest"/>
               <element ref="xkms:RecoverRequest"/>
               <element ref="xkms:RevokeRequest"/>
            </choice>
         </extension>
      </complexContent>
   </complexType>
   <!-- /CompoundRequest -->
\endverbatim
 */

class XKMSCompoundRequest : public XKMSRequestAbstractType {

	/** @name Constructors and Destructors */
	//@{

protected:

	XKMSCompoundRequest() {};

public:

	virtual ~XKMSCompoundRequest() {};

	/** @name Getter Interface Methods */
	//@{

	/**
	 * \brief Return the element at the base of the message
	 */

	virtual XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * getElement(void) const = 0;

	/**
	 * \brief Obtain the number of requests within the compound request
	 *
	 * This class holds a list of requests that can be accessed using the
	 * getRequestListItem method.  This method allows an application to determine
	 * how many request items there are.
	 *
	 * @returns The size of the request list
	 */

	virtual int getRequestListSize(void) = 0;

	/**
	 * \brief Obtain a request item
	 * 
	 * Obtain a particular request from the list of requests held in this
	 * compound object
	 *
	 * @returns The nominated item
	 */
	 
	 virtual XKMSRequestAbstractType * getRequestListItem(int item) = 0;

	//@}

	/** @name Setter Interface Methods */
	//@{

	/** \brief Add a LocateRequest item
	 *
	 * Appends a LocateRequest to the CompoundRequest object and returns the newly
	 * created object
	 *
	 * @param service URI
	 * @param id Value to set in the Id field.  If NULL, the library will
	 * generate a new Unique Id value.
	 * @returns the new XKMSLocateRequest structure
	 */

	virtual XKMSLocateRequest * createLocateRequest(
		const XMLCh * service,
		const XMLCh * id = NULL) = 0;

	/** \brief Add a ValidateRequest item
	 *
	 * Appends a ValidateRequest to the CompoundRequest object and returns the newly
	 * created object
	 *
	 * @param service URI
	 * @param id Value to set in the Id field.  If NULL, the library will
	 * generate a new Unique Id value.
	 * @returns the new XKMSValidateRequest structure
	 */

	virtual XKMSValidateRequest * createValidateRequest(
		const XMLCh * service,
		const XMLCh * id = NULL) = 0;
	 
	/** \brief Add a RegisterRequest item
	 *
	 * Appends a RegisterRequest to the CompoundRequest object and returns the newly
	 * created object
	 *
	 * @param service URI
	 * @param id Value to set in the Id field.  If NULL, the library will
	 * generate a new Unique Id value.
	 * @returns the new XKMSValidateRequest structure
	 */

	virtual XKMSRegisterRequest * createRegisterRequest(
		const XMLCh * service,
		const XMLCh * id = NULL) = 0;

	/** \brief Add a RevokeRequest item
	 *
	 * Appends a RevokeRequest to the CompoundRequest object and returns the newly
	 * created object
	 *
	 * @param service URI
	 * @param id Value to set in the Id field.  If NULL, the library will
	 * generate a new Unique Id value.
	 * @returns the new XKMSRevokeRequest structure
	 */

	virtual XKMSRevokeRequest * createRevokeRequest(
		const XMLCh * service,
		const XMLCh * id = NULL) = 0;

	/** \brief Add a RecoverRequest item
	 *
	 * Appends a RecoverRequest to the CompoundRequest object and returns the newly
	 * created object
	 *
	 * @param service URI
	 * @param id Value to set in the Id field.  If NULL, the library will
	 * generate a new Unique Id value.
	 * @returns the new XKMSRecoverRequest structure
	 */

	virtual XKMSRecoverRequest * createRecoverRequest(
		const XMLCh * service,
		const XMLCh * id = NULL) = 0;

	/** \brief Add a ReissueRequest item
	 *
	 * Appends a ReissueRequest to the CompoundRequest object and returns the newly
	 * created object
	 *
	 * @param service URI
	 * @param id Value to set in the Id field.  If NULL, the library will
	 * generate a new Unique Id value.
	 * @returns the new XKMSReissueRequest structure
	 */

	virtual XKMSReissueRequest * createReissueRequest(
		const XMLCh * service,
		const XMLCh * id = NULL) = 0;

	//@}

private:

	// Unimplemented
	XKMSCompoundRequest(const XKMSCompoundRequest &);
	XKMSCompoundRequest & operator = (const XKMSCompoundRequest &);

};

#endif /* XKMSCOMPOUNDREQUEST_INCLUDE */
