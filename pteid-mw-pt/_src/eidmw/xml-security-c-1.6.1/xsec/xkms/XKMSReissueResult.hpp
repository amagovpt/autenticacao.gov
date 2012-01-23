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
 * XKMSReissueResult := Interface for RegisterResult Messages
 *
 * $Id$
 *
 */

#ifndef XKMSREISSUERESULT_INCLUDE
#define XKMSREISSUERESULT_INCLUDE

// XSEC Includes

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/xkms/XKMSResultType.hpp>
#include <xsec/xkms/XKMSStatus.hpp>

class XKMSKeyBinding;

/**
 * @ingroup xkms
 */

/**
 * @brief Interface definition for the ReissueResult elements
 *
 * The \<ReissueResult\> is used by the X-KRSS service to response to
 * a user request to reissue a key from the service - e.g. for a new
 * cert
 *
 * The schema definition for ReissueResult is as follows :
 *
 * \verbatim
   <!-- ReissueResult -->
   <element name="ReissueResult" type="xkms:ReissueResultType"/>
   <complexType name="ReissueResultType">
      <complexContent>
         <extension base="xkms:ResultType">
            <sequence>
               <element ref="xkms:KeyBinding" minOccurs="0" 
                     maxOccurs="unbounded"/>
            </sequence>
         </extension>
      </complexContent>
   </complexType>
   <!-- /ReissueResult -->
\endverbatim
 */

class XKMSReissueResult : public XKMSResultType {

	/** @name Constructors and Destructors */
	//@{

protected:

	XKMSReissueResult() {};

public:

	virtual ~XKMSReissueResult() {};

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
	 * @returns the number of KeyBinding items within the ReissueResult
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
	 * in the ReissueResult
	 */

	virtual XKMSKeyBinding * appendKeyBindingItem(XKMSStatus::StatusValue status) = 0;

	//@}

private:

	// Unimplemented
	XKMSReissueResult(const XKMSReissueResult &);
	XKMSReissueResult & operator = (const XKMSReissueResult &);

};

#endif /* XKMSREISSUERESULT_INCLUDE */
