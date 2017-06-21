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
 * XKMSLocateResult := Interface for LocateResult Messages
 *
 * $Id: XKMSLocateResult.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef XKMSLOCATERESULT_INCLUDE
#define XKMSLOCATERESULT_INCLUDE

// XSEC Includes

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/xkms/XKMSResultType.hpp>

class XKMSQueryKeyBinding;
class XKMSUnverifiedKeyBinding;

/**
 * @ingroup xkms
 */

/**
 * @brief Interface definition for the LocateResult elements
 *
 * The \<LocateResult\> is one of the fundamental message types of
 * the XKISS service.  It is used by the Locate service to return a key
 * and the bindings for that key.
 *
 * The schema definition for LocateResponse is as follows :
 *
 * \verbatim
   <!-- LocateResult -->
   <element name="LocateResult" type="xkms:LocateResultType"/>
   <complexType name="LocateResultType">
      <complexContent>
         <extension base="xkms:ResultType">
            <sequence>
               <element ref="xkms:UnverifiedKeyBinding" minOccurs="0" 
                     maxOccurs="unbounded"/>
            </sequence>
         </extension>
      </complexContent>
   </complexType>
   <!-- /LocateResult -->
\endverbatim
 */

class XKMSLocateResult : public XKMSResultType {

	/** @name Constructors and Destructors */
	//@{

protected:

	XKMSLocateResult() {};

public:

	virtual ~XKMSLocateResult() {};

	/** @name Getter Interface Methods */
	//@{

	/**
	 * \brief Return the element at the base of the message
	 */

	virtual XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * getElement(void) const = 0;

	//@}

	/** @name UnverifiedKeyBinding handling  */
	//@{

	/**
	 * \brief Find number of UnverifiedKeyBinding elements
	 *
	 * @returns the number of UnverifiedKeyBinding items within the LocateResult
	 */

	virtual int getUnverifiedKeyBindingSize(void) const = 0;

	/*
	 * \brief Obtain an UnverifiedKeyBinding item
	 *
	 * @param item Index of the item to return (0 = first element)
	 * @returns the UnverifiedKeyBinding referenced by "item"
	 */

	virtual XKMSUnverifiedKeyBinding * getUnverifiedKeyBindingItem(int item) const = 0;

	/*
	 * \brief Append a new UnverifiedKeyBinding element
	 *
	 * Allows callers to append a new UnverifiedKeyBinding item.
	 * The item is initially empty of KeyInfo elements - these must be added
	 * by the caller.
	 *
	 * @returns the newly created UnverifiedKeyBinding object (already inserted
	 * in the LocateResult
	 */

	virtual XKMSUnverifiedKeyBinding * appendUnverifiedKeyBindingItem(void ) = 0;

	//@}

private:

	// Unimplemented
	XKMSLocateResult(const XKMSLocateResult &);
	XKMSLocateResult & operator = (const XKMSLocateResult &);

};

#endif /* XKMSLOCATERESULT_INCLUDE */
