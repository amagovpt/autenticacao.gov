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
 * XKMSValidateResult := Interface for ValidateResult Messages
 *
 * $Id: XKMSValidateResult.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef XKMSVALIDATERESULT_INCLUDE
#define XKMSVALIDATERESULT_INCLUDE

// XSEC Includes

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/xkms/XKMSResultType.hpp>
#include <xsec/xkms/XKMSStatus.hpp>

class XKMSKeyBinding;
class XKMSUnverifiedKeyBinding;

/**
 * @ingroup xkms
 */

/**
 * @brief Interface definition for the ValidateResult elements
 *
 * The \<ValidateResult\> is one of the fundamental message types of
 * the XKISS service.  It is used by the Validate service to return a key
 * and the bindings for that key.
 *
 * The schema definition for ValidateResult is as follows :
 *
 * \verbatim
   <!-- ValidateResult -->
   <element name="ValidateResult" type="xkms:ValidateResultType"/>
   <complexType name="ValidateResultType">
      <complexContent>
         <extension base="xkms:ResultType">
            <sequence>
               <element ref="xkms:KeyBinding" minOccurs="0" 
                     maxOccurs="unbounded"/>
            </sequence>
         </extension>
      </complexContent>
   </complexType>
   <!-- /ValidateResult -->
\endverbatim
 */

class XKMSValidateResult : public XKMSResultType {

	/** @name Constructors and Destructors */
	//@{

protected:

	XKMSValidateResult() {};

public:

	virtual ~XKMSValidateResult() {};

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
	 * @returns the number of KeyBinding items within the ValidateResult
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
	 * in the ValidateResult
	 */

	virtual XKMSKeyBinding * appendKeyBindingItem(XKMSStatus::StatusValue status) = 0;

	//@}

private:

	// Unimplemented
	XKMSValidateResult(const XKMSValidateResult &);
	XKMSValidateResult & operator = (const XKMSValidateResult &);

};

#endif /* XKMSVALIDATERESULT_INCLUDE */
