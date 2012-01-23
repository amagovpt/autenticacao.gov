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
 * XKMSValidityInterval := Interface for ValidityInterval elements
 *
 * $Id: XKMSValidityInterval.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef XKMSVALIDITYINTERVAL_INCLUDE
#define XKMSVALIDITYINTERVAL_INCLUDE

// XSEC Includes

#include <xsec/framework/XSECDefs.hpp>

/**
 * @ingroup xkms
 */

/**
 * @brief Interface definition for the ValidityInterval elements
 *
 * The \<ValidityInterval\> Element is used to define the dates within which
 * a key is valid.
 *
 * The schema definition for ValidityInterval is as follows :
 *
 * \verbatim
   <!-- ValidityInterval -->
   <element name="ValidityInterval" type="xkms:ValidityIntervalType"/>
   <complexType name="ValidityIntervalType">
      <attribute name="NotBefore" type="dateTime" use="optional"/>
      <attribute name="NotOnOrAfter" type="dateTime" use="optional"/>
   </complexType>
   <!-- /ValidityInterval -->
\endverbatim
 */

class XKMSValidityInterval {

	/** @name Constructors and Destructors */
	//@{

protected:

	XKMSValidityInterval() {};

public:

	virtual ~XKMSValidityInterval() {};

	//@}

	/** @name Get Methods */
	//@{

	/**
	 * \brief Get the start date for the key
	 *
	 * Defines the first instance in time from which the key is valid.  At any time
	 * before this, the key is invalid.
	 *
	 * @return The NotBefore value (or NULL if none was defined)
	 */

	virtual const XMLCh * getNotBefore(void) const = 0;

	/**
	 * \brief Get the end date for the key
	 *
	 * Defines the first instance in time from which the key is invalid.  At this time
	 * or after, the key is invalid.
	 *
	 * @return The NotOnOrAfter value (or NULL if none was defined)
	 */

	virtual const XMLCh * getNotOnOrAfter(void) const = 0;

	//@}

	/** @name Set Methods */
	//@{

	/**
	 * \brief Set the start date for the key
	 *
	 * Defines the first instance in time from which the key is valid.  At any time
	 * before this, the key is invalid.
	 *
	 * @param val value to set
	 */

	virtual void setNotBefore(const XMLCh * val) = 0;

	/**
	 * \brief Set the end date for the key
	 *
	 * Defines the first instance in time from which the key is invalid.  At this time
	 * or after, the key is invalid.
	 *
	 * @param val value to set
	 */

	virtual void setNotOnOrAfter(const XMLCh * val) = 0;

	//@}

private:

	// Unimplemented
	XKMSValidityInterval(const XKMSValidityInterval &);
	XKMSValidityInterval & operator = (const XKMSValidityInterval &);

};

#endif /* XKMSVALIDITYINTERVAL_INCLUDE */
