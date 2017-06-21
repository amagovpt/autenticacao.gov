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
 * XKMSStatusResult := Interface for StatusResult Messages
 *
 * $Id: XKMSStatusResult.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef XKMSSTATUSRESULT_INCLUDE
#define XKMSSTATUSRESULT_INCLUDE

// XSEC Includes

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/xkms/XKMSResultType.hpp>

class XKMSQueryKeyBinding;
class XKMSUnverifiedKeyBinding;

/**
 * @ingroup xkms
 */

/**
 * @brief Interface definition for the StatusResult elements
 *
 * The \<StatusResult\> is used for Asynchronous processing and
 * allows a client to determing whether a Pending response is ready
 * for return.
 *
 * The schema definition for StatusResponse is as follows :
 *
 * \verbatim
   <!-- StatusResult -->
   <element name="StatusResult" type="xkms:StatusResultType"/>
   <complexType name="StatusResultType">
      <complexContent>
         <extension base="xkms:ResultType">
            <attribute name="Success" type="integer" use="optional"/>
            <attribute name="Failure" type="integer" use="optional"/>
            <attribute name="Pending" type="integer" use="optional"/>
         </extension>
      </complexContent>
   </complexType>
   <!-- /StatusResult -->\endverbatim
 */

class XKMSStatusResult : public XKMSResultType {

	/** @name Constructors and Destructors */
	//@{

protected:

	XKMSStatusResult() {};

public:

	virtual ~XKMSStatusResult() {};

	/** @name Getter Interface Methods */
	//@{

	/**
	 * \brief Return the element at the base of the message
	 */

	virtual XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * getElement(void) const = 0;

	//@}

	/** @name Count handling  */
	//@{

	/**
	 * \brief Find number of Success messages
	 *
	 * A StatusResult can have a count of messages within a pending CompoundResponse
	 * with a Success result code.
	 *
	 * @returns the number of Success messages in a pending CompoundResponse
	 */

	virtual int getSuccessCount(void) const = 0;

	/**
	 * \brief Find number of Failed messages
	 *
	 * A StatusResult can have a count of messages within a pending CompoundResponse
	 * with a Failure result type.
	 *
	 * @returns the number of Failed messages in a pending CompoundResponse
	 */

	virtual int getFailureCount(void) const = 0;

	/**
	 * \brief Find number of Pending messages
	 *
	 * A StatusResult can have a count of messages within a pending CompoundResponse
	 * with a Pending result code.
	 *
	 * @returns the number of Pending messages in a pending CompoundResponse
	 */

	virtual int getPendingCount(void) const = 0;

	/**
	 * \brief Set the number of Success messages
	 *
	 * A StatusResult can have a count of messages within a pending CompoundResponse
	 * with a Success result code.
	 *
	 * @param count the number of Success messages in a pending CompoundResponse
	 */

	virtual void setSuccessCount(int count) = 0;

	/**
	 * \brief Set the number of Failed messages
	 *
	 * A StatusResult can have a count of messages within a pending CompoundResponse
	 * with a Failed result code.
	 *
	 * @param count the number of Failed messages in a pending CompoundResponse
	 */

	virtual void setFailureCount(int count) = 0;

	/**
	 * \brief Set the number of Pending messages
	 *
	 * A StatusResult can have a count of messages within a pending CompoundResponse
	 * with a Pending result code.
	 *
	 * @param count the number of Pending messages in a pending CompoundResponse
	 */

	virtual void setPendingCount(int count) = 0;

	//@}

private:

	// Unimplemented
	XKMSStatusResult(const XKMSStatusResult &);
	XKMSStatusResult & operator = (const XKMSStatusResult &);

};

#endif /* XKMSSTATUSRESULT_INCLUDE */
