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
 * XKMSResultType := Interface for base schema of XKMS Request messages
 *
 * $Id: XKMSResultType.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef XKMSRESULTTYPE_INCLUDE
#define XKMSRESULTTYPE_INCLUDE

// XSEC Includes

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/xkms/XKMSMessageAbstractType.hpp>

/**
 * @ingroup xkms
 */

/**
 * @brief Interface definition for the ResultType
 *
 * The \<ResultType\> is the base type on which Result messages
 * are built.  (The \<Result\> element is a direct implementation)
 *
 * The schema definition for ResultType is as follows :
 *
 * \verbatim
   <!-- ResultType -->
   <element name="Result" type="xkms:ResultType"/>
   <complexType name="ResultType">
      <complexContent>
         <extension base="xkms:MessageAbstractType">
            <sequence>
               <element ref="xkms:RequestSignatureValue" minOccurs="0"/>
            </sequence>
            <attribute name="ResultMajor" type="QName" use="required"/>
            <attribute name="ResultMinor" type="QName" use="optional"/>
            <attribute name="RequestId" type="NCName" use="optional"/>
         </extension>
      </complexContent>
   </complexType>
   <!-- /ResultType -->
\endverbatim
 */


class XKMSResultType : public XKMSMessageAbstractType {

public:

	/**
	 * \brief Definition of Major Result codes
	 */

	enum ResultMajor {
		NoneMajor=0,		/** No code - error! */
		Success,   			/** Final - The operation succeeded. */
		VersionMismatch, 	/** Final - The service does not support the protocol version specified in the request.*/
		Sender,				/** Final - An error occurred that was due to the message sent by the sender. */
		Receiver, 			/** Final - An error occurred at the receiver. */
		Represent, 			/** Not Final - The service has not acted on the request. In order for the request to be acted upon the request MUST be represented with the specified nonce in accordance with the two phase protocol */
		Pending				/** The request has been accepted for processing and the service will return the result asynchronously */
	};

	enum ResultMinor {
		NoneMinor=0,		/** No code - no code was returned */
		NoMatch,   	   	    /** No match was found for the search prototype provided.
							    Success 	The result code Success.NoMatch indicates that the service is authoritative for the search prototype specified and that the service positively asserts that no matches exist.
								Receiver 	The result code Receiver.NoMatch indicates that the service is not authoritative for the search prototype provided.*/
		TooManyResponses, 	/** The request resulted in the number of responses that exceeded either  the ResponseLimit value specified in the request or some other limit determined by the service. The service MAY either return a subset of the possible responses or none at all.
								Success 	The service has returned one or more responses that represent a subset of the possible responses.
								Receiver 	The service has returned no responses. */
		Incomplete, 		/** Success 	Only part of the information requested could be provided. */
		Failure, 	  		/** The service attempted to perform the request but the operation failed for unspecified reasons.
								Sender 	The reason for failure is attributed to the sender (e.g. the request failed schema validation).
								Receiver 	The reason for failure is attributed to the receiver (e.g. a database lookup failed).*/
		Refused, 	  		/** The operation was refused. The service did not attempt to perform the request.
								Sender 	The sender failed to provide sufficient information to authenticate or authorize the request (e.g. payment not supplied)
								Receiver 	The receiver is currently refusing certain requests for unspecified reasons. */
		NoAuthentication, 	/** Sender 	The operation was refused because the necessary authentication information was incorrect or missing.*/
		MessageNotSupported,/** Sender 	The receiver does not implement the specified operation. */
		UnknownResponseId, 	/** Sender 	The ResponseId for which pending status was requested is unknown to the service.*/
		RepresentRequired, 	/** Sender 	The responder requires that the sender offer the represent protocol option in order to process the request.*/
		NotSynchronous 		/** Receiver 	The receiver does not support synchronous processing of this type of request */
	};

	/** @name Constructors and Destructors */
	//@{

protected:

	XKMSResultType() {};

public:

	virtual ~XKMSResultType() {};

	/** @name Getter Interface Methods */
	//@{

	/**
	 * \brief Get the ResultMajor code for the message
	 *
	 * All result messages contain a Major Result code defining the overall
	 * status of the request performed by the client
	 *
	 * @returns The ResultMajor code
	 */

	virtual ResultMajor getResultMajor(void) const = 0;

	/**
	 * \brief Get the ResultMinor code for the message
	 *
	 * Messages may contain a minor result code that better defines the result
	 * returned in the ResultMajor code
	 *
	 * @returns The ResultMinor code
	 */

	virtual ResultMinor getResultMinor(void) const = 0;

	/**
	 * \brief Obtain the RequestId attribute
	 *
	 * The RequestId attribute matches the Id of the request message to which
	 * this response is related.
	 *
	 * @returns the RequestId string
	 */

	virtual const XMLCh * getRequestId(void) const = 0;

	/**
	 * \brief Obtain the value of the RequestSignatureValue element
	 *
	 * A request can have a ResponseMechanism of RequestSignatureValue.  When this
	 * is set, the service can place the text within the SignatureValue element into
	 * the Result.  This call allows a consumer of the result to find the value
	 *
	 * @returns The original signature that was on the request message
	 */

	virtual const XMLCh * getRequestSignatureValue(void) const = 0;

	//@}

	/** @name Setter interface methods */
	//@{

	/**
	 * \brief Set a new value for the ResultMajor of this response
	 *
	 * @param rmaj New value for ResultMajor for this messag
	 */

	virtual void setResultMajor(ResultMajor rmaj) = 0;

	/**
	 * \brief Set a new value for the ResultMinor of this response
	 *
	 * @param rmin New value for ResultMajor for this message
	 */

	virtual void setResultMinor(ResultMinor rmin) = 0;

	/**
	 * \brief Set the Request Id for the Message
	 *
	 * Allows a calling application to set a new request Id for the
	 * message
	 * 
	 * @param id The Id to set as the RequestId
	 */

	virtual void setRequestId(const XMLCh * id) = 0;

	/**
	 * \brief Set the value of the RequestSignatureValue element
	 *
	 * A request can have a ResponseMechanism of RequestSignatureValue.  When this
	 * is set, the service can place the text within the SignatureValue element into
	 * the Result.  This call allows a service to set this value in a response
	 *
	 * @param value The base64 encoded signature value of the original request
	 */

	virtual void setRequestSignatureValue(const XMLCh * value) = 0;

	//@}


private:

	// Unimplemented
	XKMSResultType(const XKMSResultType &);
	XKMSResultType & operator = (const XKMSResultType &);

};

#endif /* XKMSRESULTTYPE_INCLUDE */
