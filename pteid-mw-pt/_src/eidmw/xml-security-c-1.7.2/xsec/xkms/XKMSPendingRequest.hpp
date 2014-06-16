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
 * XKMSPendingRequest := Interface for PendingRequest Messages
 *
 * $Id: XKMSPendingRequest.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef XKMSPENDINGREQUEST_INCLUDE
#define XKMSPENDINGREQUEST_INCLUDE

// XSEC Includes

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/xkms/XKMSRequestAbstractType.hpp>

class XKMSQueryKeyBinding;

/**
 * @ingroup xkms
 */

/**
 * @brief Interface definition for the PendingRequest elements
 *
 * The \<PendingRequest\> is used in Asynchronous processing.  A client
 * uses the PendingRequest element to request a response to a request that
 * was placed in the past, and to which the service originally provided
 * a "Pending" response - i.e. work yet to be done.
 *
 * The schema definition for PendingRequest is as follows :
 *
 * \verbatim
   <!-- PendingRequest -->
   <element name="PendingRequest" type="xkms:PendingRequestType"/>
   <complexType name="PendingRequestType">
      <complexContent>
         <extension base="xkms:RequestAbstractType">
            <attribute name="ResponseId" type="NCName" use="required"/>
         </extension>
      </complexContent>
   </complexType>
   <!-- /PendingRequest -->
\endverbatim
 */

class XKMSPendingRequest : public XKMSRequestAbstractType {

	/** @name Constructors and Destructors */
	//@{

protected:

	XKMSPendingRequest() {};

public:

	virtual ~XKMSPendingRequest() {};

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
	 * identify the original request for which this PendingRequest refers.
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
	XKMSPendingRequest(const XKMSPendingRequest &);
	XKMSPendingRequest & operator = (const XKMSPendingRequest &);

};

#endif /* XKMSPENDINGREQUEST_INCLUDE */
