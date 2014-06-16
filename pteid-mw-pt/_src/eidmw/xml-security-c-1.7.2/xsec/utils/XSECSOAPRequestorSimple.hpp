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
 * XSECSOAPRequestorSimple := (Very) Basic implementation of a SOAP
 *                         HTTP wrapper for testing the client code.
 *
 *
 * $Id: XSECSOAPRequestorSimple.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef XSECSOAPREQUESTORSIMPLE_INCLUDE
#define XSECSOAPREQUESTORSIMPLE_INCLUDE

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/utils/XSECSOAPRequestor.hpp>

#include <xercesc/util/XMLUri.hpp>

XSEC_DECLARE_XERCES_CLASS(DOMDocument);

/**
 * @ingroup xkms
 */
/*\@{*/

/**
 * @brief Basic HTTP implementation for SOAP Requests
 *
 * The XKMS client code needs to be able to call on a SOAP requestor
 * implementation that will handle wrapping the request in a SOAP msg
 * and transporting it to the SOAP server.  This class provides a very
 * naieve implementation that wraps the message and does a basic
 * HTTP POST to get the message to the end server.
 *
 */


class DSIG_EXPORT XSECSOAPRequestorSimple : public XSECSOAPRequestor {

public :

	/**
	 * Envelope Enumeration
	 */

	enum envelopeType {

		ENVELOPE_NONE,		/** No envelope - straight HTTP request */
		ENVELOPE_SOAP11,	/** SOAP 1.1 envelope (default) */
		ENVELOPE_SOAP12		/** SOAP 1.2 envelope */

	};

	/** @name Constructors and Destructors */
	//@{

	/**
	 * \brief Constructor
	 *
	 * Create a SOAP requestor that can be used to access a specific
	 * server
	 *
	 * @param uri The URI of the server that will be accessed.
	 * @note The URI must be http://...
	 */

	XSECSOAPRequestorSimple(const XMLCh * uri);
	virtual ~XSECSOAPRequestorSimple();

	//@}

	/** @name Interface methods */
	//@{

	/**
	 * \brief Do a SOAP request
	 *
	 * Performs a request based on the passed in DOM document and
	 * the indicated URI.  The function is returns a pointer
	 * to the parsed result message (with the SOAP envelope removed)
	 *
	 * @param request The DOM document containing the message to be 
	 * wrapped and sent.
	 * @returns The DOM document representing the result, with all
	 * SOAP headers removed
	 */

	virtual XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *
		doRequest(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument * request);

	//@}

	/** @name Configuration methods */
	//@{

	/**
	 * \brief Set the envelope type
	 *
	 * The requestor can do a SOAP 1.1, SOAP 1.2 or no envelope around the
	 * message.  This call allows the apllication to set the type.
	 *
	 * By default, the requestor uses a SOAP 1.1 envelope
	 *
	 * @param et - the type of envelope to use
	 */

	void setEnvelopeType(envelopeType et);


private:

	char * wrapAndSerialise(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument * request);
	XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *
		parseAndUnwrap(const char * buf, unsigned int len);

	XERCES_CPP_NAMESPACE_QUALIFIER XMLUri			
						m_uri;

	envelopeType		m_envelopeType;

};


#endif /* XSECSOAPREQUESTORSIMPLE_INCLUDE */

