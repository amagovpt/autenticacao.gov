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
 * XKMSStatusRequest := Interface for StatusRequest Messages
 *
 * $Id: XKMSStatusRequest.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef XKMSSTATUSREQUEST_INCLUDE
#define XKMSSTATUSREQUEST_INCLUDE

// XSEC Includes

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/xkms/XKMSRequestAbstractType.hpp>

class XKMSQueryKeyBinding;

/**
 * @ingroup xkms
 */

/**
 * @brief Interface definition for the StatusRequest elements
 *
 * The \<StatusRequest\> is used in Asynchronous processing.  A client
 * uses the StatusRequest element to request a determine whether a
 * request for which the original response was "Pending" is now ready.
 *
 * The schema definition for StatusRequest is as follows :
 *
 * \verbatim
   <!-- StatusRequest -->
   <element name="StatusRequest" type="xkms:StatusRequestType"/>
   <complexType name="StatusRequestType">
      <complexContent>
         <extension base="xkms:PendingRequestType"/>
      </complexContent>
   </complexType>
   <!-- /StatusRequest -->\endverbatim
 */

class XKMSStatusRequest : public XKMSRequestAbstractType {

	/** @name Constructors and Destructors */
	//@{

protected:

	XKMSStatusRequest() {};

public:

	virtual ~XKMSStatusRequest() {};

	/** @name Getter Interface Methods */
	//@{

	/**
	 * \brief Return the element at the base of the message
	 */

	virtual XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * getElement(void) const = 0;

	/**
	 * \brief Return the ResponseId
	 *
	 * The ResponseId equates to the Id of the first response the service returned,
	 * which must (by definition) have had a response code of "Pending".  Together
	 * with the OriginalRequestId, this can be used by the service to uniquley
	 * identify the original request for which this StatusRequest refers.
	 *
	 * @return A pointer to the XMLCh string holding the ResponseId
	 */

	virtual const XMLCh * getResponseId(void) const = 0;

	//@}

	/** @name Setter Interface Methods */
	//@{

	/**
	 * \brief Set the ResponseId
	 *
	 * Allows a calling application to over-ride the configured responseId
	 *
	 * @param responseId The new responseId for this element
	 */

	virtual void setResponseId(const XMLCh * responseId) = 0;

	//@}

private:

	// Unimplemented
	XKMSStatusRequest(const XKMSStatusRequest &);
	XKMSStatusRequest & operator = (const XKMSStatusRequest &);

};

#endif /* XKMSSTATUSREQUEST_INCLUDE */
