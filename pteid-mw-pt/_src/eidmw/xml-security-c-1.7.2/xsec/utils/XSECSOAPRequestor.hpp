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
 * XSECSOAPRequestor:= Interface class used by the XKMS client code to
 *                     perform a SOAP request and receive response.
 *
 *
 * $Id: XSECSOAPRequestor.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef XSECSOAPREQUESTOR_INCLUDE
#define XSECSOAPREQUESTOR_INCLUDE

#include <xsec/framework/XSECDefs.hpp>

XSEC_DECLARE_XERCES_CLASS(DOMDocument);

/**
 * @brief Interface for SOAP Requests
 * @ingroup interfaces
 *
 * The XKMS client code needs to be able to call on a SOAP requestor
 * implementation that will handle wrapping the request in a SOAP msg
 * and transporting it to the SOAP server.  This class defines the
 * interface used by the client code to make this happen.
 *
 * Implementors need to wrap the passed in DOM document in a SOAP 1.1
 * or SOAP 1.2 msg.  E.g. (SOAP 1.2)
 * \verbatim
   <?xml version='1.0' encoding="utf-8"?>
   <env:Envelope xmlns:env="http://www.w3.org/2002/06/soap-envelope"> 
    <env:Header>
     <env:Body>
       XKMS Request Message element 
     </env:Body>
    </env:Header>
   </env:Envelope>
\endverbatim
 * 
 * SOAP implentors may want/need to add other headers.
 *
 */


class DSIG_EXPORT XSECSOAPRequestor {

public :

	/** @name Constructors and Destructors */
	//@{

	XSECSOAPRequestor() {}
	virtual ~XSECSOAPRequestor() {}

	//@}

	/** @name Interface methods */

	/**
	 * \brief Do a SOAP request
	 *
	 * Performs a request based on the passed in DOM document and
	 * the indicated URI.  The function is expected to return a pointer
	 * to the parsed result message (with the SOAP envelope removed)
	 *
	 * The implementing object is expected to know how to get the
	 * wrapped message to the server that will process the request
	 *
	 * @param request The DOM document containing the message to be 
	 * wrapped and sent.
	 * @returns The DOM document representing the result, with all
	 * SOAP headers removed
	 */

	virtual XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *
		doRequest(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument * request) = 0;

};


#endif /* XSECSOAPREQUESTOR_INCLUDE */

