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
 * XKMSStatus := Interface for Status elements (in KeyBinding)
 *
 * $Id: XKMSStatus.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef XKMSSTATUS_INCLUDE
#define XKMSSTATUS_INCLUDE

// XSEC Includes

#include <xsec/framework/XSECDefs.hpp>

/**
 * @ingroup xkms
 */

/**
 * @brief Interface definition for the Status elements
 *
 * The \<Status\> Element is used in a KeyBinding message to a client
 * to provide status information for keys that are returned in a
 * ValidateResult message
 *
 * The schema definition for KeyBinding is as follows :
 *
 * \verbatim
   <!-- Status -->
   <element name="Status" type="xkms:StatusType"/>
   <complexType name="StatusType">
      <sequence>
         <element ref="xkms:ValidReason" minOccurs="0" 
               maxOccurs="unbounded"/>
         <element ref="xkms:IndeterminateReason" minOccurs="0" 
               maxOccurs="unbounded"/>
         <element ref="xkms:InvalidReason" minOccurs="0" 
               maxOccurs="unbounded"/>
      </sequence>
      <attribute name="StatusValue" type="xkms:KeyBindingStatus" 
            use="required"/>
   </complexType>
   <simpleType name="KeyBindingStatus">
      <restriction base="QName">
         <enumeration value="xkms:Valid"/>
         <enumeration value="xkms:Invalid"/>
         <enumeration value="xkms:Indeterminate"/>
      </restriction>
   </simpleType>
   <!-- /Status -->
 \endverbatim
 */


class XKMSStatus {

	/** @name Constructors and Destructors */
	//@{

protected:

	XKMSStatus() {};

public:

	virtual ~XKMSStatus() {};

	//@}

	enum StatusValue {

		StatusUndefined = 0,	/** Error - the status value is undefined */
		Valid = 1,				/** Good key! */
		Invalid = 2,			/** Bad key! */
		Indeterminate = 3		/** Smarmy key! */

	};

	enum StatusReason {

		ReasonUndefined = 0,    /** The reason is undefined or unknown */
		IssuerTrust = 1,		/** The issuer of the info is considered good */
		RevocationStatus = 2,	/** Key is/is not revoked */
		ValidityInterval = 3,	/** Key not expired */
		Signature = 4			/** Signature on the provided data is good */

	};

	/** @name Getter methods */
	//@{

	/**
	 * \brief Get the KeyBinding status assertion
	 *
	 * The Status element has a StatusValue attribute that defines
	 * the validity of the key in question
	 *
	 * @return the status assertion
	 */

	virtual StatusValue getStatusValue(void) const = 0;

	/**
	 * \brief Find the Reason for a particular facet of status codes
	 *
	 * For each status reason, a Status can define a number of reasons
	 * to return to a client.  There are some controls on this (e.g. you cannot
	 * have an InvalidReason if the actual KeyBindingStatus is Valid), but
	 * you can have things in ValidReason and IndeterminateReason for an Invalid
	 * result.  This call allows you to find out about any facet of a reason
	 *
	 * @param status The particular status value you want to find a reason for
	 * @param reason The particular reason that you want to determine is or is not
	 * set
	 * @return true if the particular reason is set for the given status, false
	 * otherwise
	 */

	virtual bool getStatusReason(StatusValue status, StatusReason reason) const = 0;

	//@}

	/** @name Setter Methods */
	//@{

	/**
	 * \brief Activate or de-activate a particular reason in the status result
	 *
	 * For each status reason, a Status can define a number of reasons
	 * to return to a client.  There are some controls on this (e.g. you cannot
	 * have an InvalidReason if the actual KeyBindingStatus is Valid), but
	 * you can have things in ValidReason and IndeterminateReason for an Invalid
	 * result.  This call allows you to set any facet of a reason
	 *
	 * @param status The particular status value you want to set a reason for
	 * @param reason The particular reason that you want to set
	 * @param value true to turn on the reason, false to turn it off
	 */

	virtual void setStatusReason(StatusValue status, StatusReason reason, bool value) = 0;

	//@}

private:

	// Unimplemented
	XKMSStatus(const XKMSStatus &);
	XKMSStatus & operator = (const XKMSStatus &);

};

#endif /* XKMSSTATUS_INCLUDE */
