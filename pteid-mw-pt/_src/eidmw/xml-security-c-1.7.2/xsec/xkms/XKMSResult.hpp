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
 * XKMSResult := Interface for Result Messages
 *
 * $Id: XKMSResult.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef XKMSRESULT_INCLUDE
#define XKMSRESULT_INCLUDE

// XSEC Includes

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/xkms/XKMSResultType.hpp>

/**
 * @ingroup xkms
 */

/**
 * @brief Interface definition for the Result elements
 *
 * The \<Result\> is used when an XKMS service cannot return a result
 * of a more specific type - e.g. when a pending request comes in for which
 * there is no result.  It is a straight instantiation of the ResultType
 */

class XKMSResult : public XKMSResultType {

	/** @name Constructors and Destructors */
	//@{

protected:

	XKMSResult() {};

public:

	virtual ~XKMSResult() {};

	/** @name Getter Interface Methods */
	//@{

	/**
	 * \brief Return the element at the base of the message
	 */

	virtual XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * getElement(void) const = 0;

	//@}

private:

	// Unimplemented
	XKMSResult(const XKMSResult &);
	XKMSResult & operator = (const XKMSResult &);

};

#endif /* XKMSRESULT_INCLUDE */
