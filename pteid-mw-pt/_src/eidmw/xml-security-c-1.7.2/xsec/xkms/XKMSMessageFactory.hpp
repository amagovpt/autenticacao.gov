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
 * XKMSMessageFactory := Interface for the factory used to produce XKMS msgs
 *
 * $Id: XKMSMessageFactory.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef XKMSMESSAGEFACTORY_INCLUDE
#define XKMSMESSAGEFACTORY_INCLUDE

// XSEC Includes

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/xkms/XKMSMessageAbstractType.hpp>
#include <xsec/xkms/XKMSResultType.hpp>

class DSIGSignature;
class XKMSCompoundRequest;
class XKMSCompoundResult;
class XKMSLocateRequest;
class XKMSLocateResult;
class XKMSRequestAbstractType;
class XKMSResult;
class XKMSValidateRequest;
class XKMSValidateResult;
class XKMSPendingRequest;
class XKMSStatusRequest;
class XKMSStatusResult;
class XKMSRegisterRequest;
class XKMSRegisterResult;
class XKMSRevokeResult;
class XKMSRevokeRequest;
class XKMSRecoverResult;
class XKMSRecoverRequest;
class XKMSReissueResult;
class XKMSReissueRequest;

XSEC_DECLARE_XERCES_CLASS(DOMElement);

/**
 * @defgroup xkms XML Key Management System Implementation
 *
 * <p>The classes in this group implement the W3C XKMS 2.0
 * specification.  Users should interact with these classes
 * via the XKMSMessageFactory class (for consuming and producing
 * XKMS messages) and the XKMSClient class (for actually
 * performing XKMS calls to a server)</p>
 *
 */
/*@{*/

/**
 * @brief Interface definition for the XKMSMessageFactory class
 *
 * The XKMSMessageFactory classes are used to provide an interface
 * to applicataions to produce and consume XKMS messages.  No logic
 * is defined within the class, other than that necessary to (for
 * example) generate a response message using a request message as
 * a base.
 *
 * Client apps that wish to make use of XKMS should generally use
 * the XKMSClient class.
 */


class XKMSMessageFactory {

	/** @name Constructors and Destructors */
	//@{

protected:

	XKMSMessageFactory() {};

public:

	virtual ~XKMSMessageFactory() {};

	/** @name Methods to build XKMS message objects from existing XML  */
	//@{

	/**
	 * \brief Load a message from an existing XML document
	 *
	 * Reads in the XML document and produces the corresponding XKMS
	 * message object.  Callers should make use of the ::getMessageType
	 * method to determine what type of message they are actually
	 * working with.
	 *
	 * @param elt The element at the head of the XKMS structure
	 */

	virtual XKMSMessageAbstractType * newMessageFromDOM(
		XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * elt) = 0;


	//@}

	/** @name Methods for building new X-KISS messages from scratch */
	//@{

	/**
	 * \brief Create a new \<CompoundRequest\> message.
	 * 
	 * Generates a new CompoundRequest message from scratch, building the DOM
	 * as it goes.
	 *
	 * @param service URI
	 * @param doc Document to create the DOM structure within.  The caller
	 * will need to embed the DOM structure at the appropriate place in the
	 * document (using a call to getElement to find the top level element)
	 * @param id Value to set in the Id field.  If NULL, the library will
	 * generate a new Unique Id value.
	 * @returns the new XKMSCompoundRequest structure
	 */

	virtual XKMSCompoundRequest * createCompoundRequest(
		const XMLCh * service,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument * doc,
		const XMLCh * id = NULL) = 0;
		
	/**
	 * \brief Create a new \<CompoundRequest\> message and surrounding document
	 * 
	 * Generates a new CompoundRequest message from scratch, building the DOM
	 * as it goes.
	 *
	 * @param service URI
	 * @param doc Will be used to return the newly created document element in.
	 * @param id Value to set in the Id field.  If NULL, the library will
	 * generate a new Unique Id value.
	 * @returns the new XKMSCompoundRequest structure
	 * @note Like all the xsec library functions, the document is owned by
	 * the calling application.  Deleteing the CompoundRequest object will not
	 * delete the DOM document as well.
	 */

	virtual XKMSCompoundRequest * createCompoundRequest(
		const XMLCh * service,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument **doc,
		const XMLCh * id = NULL) = 0;

	/**
	 * \brief Create a new \<LocateRequest\> message.
	 * 
	 * Generates a new LocateRequest message from scratch, building the DOM
	 * as it goes.
	 *
	 * @param service URI
	 * @param doc Document to create the DOM structure within.  The caller
	 * will need to embed the DOM structure at the appropriate place in the
	 * document (using a call to getElement to find the top level element)
	 * @param id Value to set in the Id field.  If NULL, the library will
	 * generate a new Unique Id value.
	 * @returns the new XKMSLocateRequest structure
	 */

	virtual XKMSLocateRequest * createLocateRequest(
		const XMLCh * service,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument * doc,
		const XMLCh * id = NULL) = 0;
		
	/**
	 * \brief Create a new \<LocateRequest\> message and surrounding document
	 * 
	 * Generates a new LocateRequest message from scratch, building the DOM
	 * as it goes.
	 *
	 * @param service URI
	 * @param doc Will be used to return the newly created document element in.
	 * @param id Value to set in the Id field.  If NULL, the library will
	 * generate a new Unique Id value.
	 * @returns the new XKMSLocateRequest structure
	 * @note Like all the xsec library functions, the document is owned by
	 * the calling application.  Deleteing the LocateRequest object will not
	 * delete the DOM document as well.
	 */

	virtual XKMSLocateRequest * createLocateRequest(
		const XMLCh * service,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument **doc,
		const XMLCh * id = NULL) = 0;

	/**
	 * \brief Create a new \<ValidateRequest\> message.
	 * 
	 * Generates a new ValidateRequest message from scratch, building the DOM
	 * as it goes.
	 *
	 * @param service URI
	 * @param doc Document to create the DOM structure within.  The caller
	 * will need to embed the DOM structure at the appropriate place in the
	 * document (using a call to getElement to find the top level element)
	 * @param id Value to set in the Id field.  If NULL, the library will
	 * generate a new Unique Id value.
	 * @returns the new XKMSValidateRequest structure
	 */

	virtual XKMSValidateRequest * createValidateRequest(
		const XMLCh * service,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument * doc,
		const XMLCh * id = NULL) = 0;
		
	/**
	 * \brief Create a new \<ValidateRequest\> message and surrounding document
	 * 
	 * Generates a new ValidateRequest message from scratch, building the DOM
	 * as it goes.
	 *
	 * @param service URI
	 * @param doc Will be used to return the newly created document element in.
	 * @param id Value to set in the Id field.  If NULL, the library will
	 * generate a new Unique Id value.
	 * @returns the new XKMSValidateRequest structure
	 * @note Like all the xsec library functions, the document is owned by
	 * the calling application.  Deleting the LocateRequest object will not
	 * delete the DOM document as well.
	 */

	virtual XKMSValidateRequest * createValidateRequest(
		const XMLCh * service,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument **doc,
		const XMLCh * id = NULL) = 0;

	/**
	 * \brief Create a new \<PendingRequest\> message.
	 * 
	 * Generates a new PendingRequest message from scratch, building the DOM
	 * as it goes.
	 *
	 * @param service URI
	 * @param doc Document to create the DOM structure within.  The caller
	 * will need to embed the DOM structure at the appropriate place in the
	 * document (using a call to getElement to find the top level element)
	 * @param id Value to set in the Id field.  If NULL, the library will
	 * generate a new Unique Id value.
	 * @returns the new XKMSPendingRequest structure
	 */

	virtual XKMSPendingRequest * createPendingRequest(
		const XMLCh * service,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument * doc,
		const XMLCh * id = NULL) = 0;
		
	/**
	 * \brief Create a new \<PendingRequest\> message and surrounding document
	 * 
	 * Generates a new PendingRequest message from scratch, building the DOM
	 * as it goes.
	 *
	 * @param service URI
	 * @param doc Will be used to return the newly created document element in.
	 * @param id Value to set in the Id field.  If NULL, the library will
	 * generate a new Unique Id value.
	 * @returns the new XKMSPendingRequest structure
	 * @note Like all the xsec library functions, the document is owned by
	 * the calling application.  Deleting the PendingRequest object will not
	 * delete the DOM document as well.
	 */

	virtual XKMSPendingRequest * createPendingRequest(
		const XMLCh * service,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument **doc,
		const XMLCh * id = NULL) = 0;

	/**
	 * \brief Create a new \<StatusRequest\> message.
	 * 
	 * Generates a new StatusRequest message from scratch, building the DOM
	 * as it goes.
	 *
	 * @param service URI
	 * @param doc Document to create the DOM structure within.  The caller
	 * will need to embed the DOM structure at the appropriate place in the
	 * document (using a call to getElement to find the top level element)
	 * @param id Value to set in the Id field.  If NULL, the library will
	 * generate a new Unique Id value.
	 * @returns the new XKMSStatusRequest structure
	 */

	virtual XKMSStatusRequest * createStatusRequest(
		const XMLCh * service,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument * doc,
		const XMLCh * id = NULL) = 0;
		
	/**
	 * \brief Create a new \<StatusRequest\> message and surrounding document
	 * 
	 * Generates a new StatusRequest message from scratch, building the DOM
	 * as it goes.
	 *
	 * @param service URI
	 * @param doc Will be used to return the newly created document element in.
	 * @param id Value to set in the Id field.  If NULL, the library will
	 * generate a new Unique Id value.
	 * @returns the new XKMSStatusRequest structure
	 * @note Like all the xsec library functions, the document is owned by
	 * the calling application.  Deleting the StatusRequest object will not
	 * delete the DOM document as well.
	 */

	virtual XKMSStatusRequest * createStatusRequest(
		const XMLCh * service,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument **doc,
		const XMLCh * id = NULL) = 0;

	/**
	 * \brief Create a new \<LocateResult\> message.
	 * 
	 * Generates a new LocateResult message from scratch, building the DOM
	 * as it goes.  The response will be based on a input LocateRequest message
	 * which is used to provide Id etc.
	 *
	 * @param request Request to base response on
	 * @param doc Will be used to return the newly created document element in.
	 * @param id Value to set in the Id field.  If NULL, the library will
	 * generate a new Unique Id value.
     * @param rmaj Major result code
     * @param rmin Minor result code
	 * @returns the new XKMSLocateResponse structure
	 */

	virtual XKMSLocateResult * createLocateResult(
		XKMSLocateRequest * request,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument * doc,
		XKMSResultType::ResultMajor rmaj,
		XKMSResultType::ResultMinor rmin = XKMSResultType::NoneMinor,
		const XMLCh * id = NULL) = 0;

	/**
	 * \brief Create a new \<LocateResult\> message and surrounding document
	 * 
	 * Generates a new LocateResult message from scratch, building the DOM
	 * as it goes.  The response will be based on a input LocateRequest message
	 * which is used to provide Id etc.
	 *
	 * @param request Request to base response on
	 * @param doc Will be used to return the newly created document element in.
	 * @param id Value to set in the Id field.  If NULL, the library will
	 * generate a new Unique Id value.
     * @param rmaj Major result code
     * @param rmin Minor result code
	 * @returns the new XKMSLocateResponse structure
	 * @note Like all the xsec library functions, the document is owned by
	 * the calling application.  Deleteing the LocateRequest object will not
	 * delete the DOM document as well.
	 */

	virtual XKMSLocateResult * createLocateResult(
		XKMSLocateRequest * request,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument **doc,
		XKMSResultType::ResultMajor rmaj,
		XKMSResultType::ResultMinor rmin = XKMSResultType::NoneMinor,
		const XMLCh * id = NULL) = 0;

	/**
	 * \brief Create a new \<ValidateResult\> message.
	 * 
	 * Generates a new ValidateResult message from scratch, building the DOM
	 * as it goes.  The response will be based on a input ValidateRequest message
	 * which is used to provide Id etc.
	 *
	 * @param request Request to base response on
	 * @param doc Will be used to return the newly created document element in.
	 * @param id Value to set in the Id field.  If NULL, the library will
	 * generate a new Unique Id value.
     * @param rmaj Major result code
     * @param rmin Minor result code
	 * @returns the new XKMSLocateResponse structure
	 */

	virtual XKMSValidateResult * createValidateResult(
		XKMSValidateRequest * request,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument * doc,
		XKMSResultType::ResultMajor rmaj,
		XKMSResultType::ResultMinor rmin = XKMSResultType::NoneMinor,
		const XMLCh * id = NULL) = 0;

	/**
	 * \brief Create a new \<ValidateResult\> message and surrounding document
	 * 
	 * Generates a new ValidateResult message from scratch, building the DOM
	 * as it goes.  The response will be based on a input ValidateRequest message
	 * which is used to provide Id etc.
	 *
	 * @param request Request to base response on
	 * @param doc Will be used to return the newly created document element in.
	 * @param id Value to set in the Id field.  If NULL, the library will
	 * generate a new Unique Id value.
     * @param rmaj Major result code
     * @param rmin Minor result code
	 * @returns the new XKMSValidateResponse structure
	 * @note Like all the xsec library functions, the document is owned by
	 * the calling application.  Deleteing the ValidateRequest object will not
	 * delete the DOM document as well.
	 */

	virtual XKMSValidateResult * createValidateResult(
		XKMSValidateRequest * request,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument **doc,
		XKMSResultType::ResultMajor rmaj,
		XKMSResultType::ResultMinor rmin = XKMSResultType::NoneMinor,
		const XMLCh * id = NULL) = 0;

	/**
	 * \brief Create a new \<CompoundResult\> message.
	 * 
	 * Generates a new CompoundResult message from scratch, building the DOM
	 * as it goes.  The response will be based on a input CompoundRequest message
	 * which is used to provide Id etc.
	 *
	 * @param request Request to base response on
	 * @param doc Will be used to return the newly created document element in.
	 * @param id Value to set in the Id field.  If NULL, the library will
	 * generate a new Unique Id value.
     * @param rmaj Major result code
     * @param rmin Minor result code
	 * @returns the new XKMSLocateResponse structure
	 */

	virtual XKMSCompoundResult * createCompoundResult(
		XKMSCompoundRequest * request,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument * doc,
		XKMSResultType::ResultMajor rmaj,
		XKMSResultType::ResultMinor rmin = XKMSResultType::NoneMinor,
		const XMLCh * id = NULL) = 0;

	/**
	 * \brief Create a new \<CompoundResult\> message and surrounding document
	 * 
	 * Generates a new CompoundResult message from scratch, building the DOM
	 * as it goes.  The response will be based on a input ValidateRequest message
	 * which is used to provide Id etc.
	 *
	 * @param request Request to base response on
	 * @param doc Will be used to return the newly created document element in.
	 * @param id Value to set in the Id field.  If NULL, the library will
	 * generate a new Unique Id value.
     * @param rmaj Major result code
     * @param rmin Minor result code
	 * @returns the new XKMSCompoundResponse structure
	 * @note Like all the xsec library functions, the document is owned by
	 * the calling application.  Deleteing the ValidateRequest object will not
	 * delete the DOM document as well.
	 */

	virtual XKMSCompoundResult * createCompoundResult(
		XKMSCompoundRequest * request,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument **doc,
		XKMSResultType::ResultMajor rmaj,
		XKMSResultType::ResultMinor rmin = XKMSResultType::NoneMinor,
		const XMLCh * id = NULL) = 0;

	/**
	 * \brief Create a new \<Result\> message.
	 * 
	 * Generates a new Result message from scratch, building the DOM
	 * as it goes.  The response will be based on an input Request message
	 * which is used to provide Id etc.
	 *
	 * @param request Request to base response on
	 * @param doc Will be used to return the newly created document element in.
	 * @param id Value to set in the Id field.  If NULL, the library will
	 * generate a new Unique Id value.
     * @param rmaj Major result code
     * @param rmin Minor result code
	 * @returns the new XKMSResult structure
	 */

	virtual XKMSResult * createResult(
		XKMSRequestAbstractType * request,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument * doc,
		XKMSResultType::ResultMajor rmaj,
		XKMSResultType::ResultMinor rmin = XKMSResultType::NoneMinor,
		const XMLCh * id = NULL) = 0;

	/**
	 * \brief Create a new \<Result\> message and surrounding document
	 * 
	 * Generates a new Result message from scratch, building the DOM
	 * as it goes.  The response will be based on a input Request message
	 * which is used to provide Id etc.
	 *
	 * @param request Request to base response on
	 * @param doc Will be used to return the newly created document element in.
	 * @param id Value to set in the Id field.  If NULL, the library will
	 * generate a new Unique Id value.
     * @param rmaj Major result code
     * @param rmin Minor result code
	 * @returns the new Response structure
	 * @note Like all the xsec library functions, the document is owned by
	 * the calling application.  Deleting the Request object will not
	 * delete the DOM document as well.
	 */

	virtual XKMSResult * createResult(
		XKMSRequestAbstractType * request,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument **doc,
		XKMSResultType::ResultMajor rmaj,
		XKMSResultType::ResultMinor rmin = XKMSResultType::NoneMinor,
		const XMLCh * id = NULL) = 0;

	/**
	 * \brief Create a new \<StatusResult\> message.
	 * 
	 * Generates a new StatusResult message from scratch, building the DOM
	 * as it goes.  The response will be based on a input StatusRequest message
	 * which is used to provide Id etc.
	 *
	 * @param request Request to base response on
	 * @param doc Will be used to return the newly created document element in.
	 * @param id Value to set in the Id field.  If NULL, the library will
	 * generate a new Unique Id value.
     * @param rmaj Major result code
     * @param rmin Minor result code
	 * @returns the new XKMSStatusRessult structure
	 */

	virtual XKMSStatusResult * createStatusResult(
		XKMSStatusRequest * request,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument * doc,
		XKMSResultType::ResultMajor rmaj,
		XKMSResultType::ResultMinor rmin = XKMSResultType::NoneMinor,
		const XMLCh * id = NULL) = 0;

	/**
	 * \brief Create a new \<StatusResult\> message and surrounding document
	 * 
	 * Generates a new StatusResult message from scratch, building the DOM
	 * as it goes.  The response will be based on a input StatusRequest message
	 * which is used to provide Id etc.
	 *
	 * @param request Request to base response on
	 * @param doc Will be used to return the newly created document element in.
	 * @param id Value to set in the Id field.  If NULL, the library will
	 * generate a new Unique Id value.
     * @param rmaj Major result code
     * @param rmin Minor result code
	 * @returns the new XKMSStatusResult structure
	 * @note Like all the xsec library functions, the document is owned by
	 * the calling application.  Deleting the StatusResult object will not
	 * delete the DOM document as well.
	 */

	virtual XKMSStatusResult * createStatusResult(
		XKMSStatusRequest * request,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument **doc,
		XKMSResultType::ResultMajor rmaj,
		XKMSResultType::ResultMinor rmin = XKMSResultType::NoneMinor,
		const XMLCh * id = NULL) = 0;

	//@}

	/** @name Methods for building new X-KRSS messages from scratch */
	//@{

	/**
	 * \brief Create a new \<RegisterRequest\> message.
	 * 
	 * Generates a new RegisterRequest message from scratch, building the DOM
	 * as it goes.
	 *
	 * @param service URI
	 * @param doc Document to create the DOM structure within.  The caller
	 * will need to embed the DOM structure at the appropriate place in the
	 * document (using a call to getElement to find the top level element)
	 * @param id Value to set in the Id field.  If NULL, the library will
	 * generate a new Unique Id value.
	 * @returns the new XKMSRegisterRequest structure
	 */

	virtual XKMSRegisterRequest * createRegisterRequest(
		const XMLCh * service,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument * doc,
		const XMLCh * id = NULL) = 0;
		
	/**
	 * \brief Create a new \<RegisterRequest\> message and surrounding document
	 * 
	 * Generates a new RegisterRequest message from scratch, building the DOM
	 * as it goes.
	 *
	 * @param service URI
	 * @param doc Will be used to return the newly created document element in.
	 * @param id Value to set in the Id field.  If NULL, the library will
	 * generate a new Unique Id value.
	 * @returns the new XKMSRegisterRequest structure
	 * @note Like all the xsec library functions, the document is owned by
	 * the calling application.  Deleting the RegisterRequest object will not
	 * delete the DOM document as well.
	 */

	virtual XKMSRegisterRequest * createRegisterRequest(
		const XMLCh * service,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument **doc,
		const XMLCh * id = NULL) = 0;

	/**
	 * \brief Create a new \<RegisterResult\> message.
	 * 
	 * Generates a new RegisterResult message from scratch, building the DOM
	 * as it goes.  The response will be based on a input ValidateRequest message
	 * which is used to provide Id etc.
	 *
	 * @param request Request to base response on
	 * @param doc Will be used to return the newly created document element in.
	 * @param id Value to set in the Id field.  If NULL, the library will
	 * generate a new Unique Id value.
     * @param rmaj Major result code
     * @param rmin Minor result code
	 * @returns the new XKMSRegisterResponse structure
	 */

	virtual XKMSRegisterResult * createRegisterResult(
		XKMSRegisterRequest * request,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument * doc,
		XKMSResultType::ResultMajor rmaj,
		XKMSResultType::ResultMinor rmin = XKMSResultType::NoneMinor,
		const XMLCh * id = NULL) = 0;

	/**
	 * \brief Create a new \<RegisterResult\> message and surrounding document
	 * 
	 * Generates a new RegisterResult message from scratch, building the DOM
	 * as it goes.  The response will be based on a input ValidateRequest message
	 * which is used to provide Id etc.
	 *
	 * @param request Request to base response on
	 * @param doc Will be used to return the newly created document element in.
	 * @param id Value to set in the Id field.  If NULL, the library will
	 * generate a new Unique Id value.
     * @param rmaj Major result code
     * @param rmin Minor result code
	 * @returns the new XKMSRegisterResponse structure
	 * @note Like all the xsec library functions, the document is owned by
	 * the calling application.  Deleting the RegisterRequest object will not
	 * delete the DOM document as well.
	 */

	virtual XKMSRegisterResult * createRegisterResult(
		XKMSRegisterRequest * request,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument **doc,
		XKMSResultType::ResultMajor rmaj,
		XKMSResultType::ResultMinor rmin = XKMSResultType::NoneMinor,
		const XMLCh * id = NULL) = 0;

	/**
	 * \brief Create a new \<RevokeRequest\> message.
	 * 
	 * Generates a new RevokeRequest message from scratch, building the DOM
	 * as it goes.
	 *
	 * @param service URI
	 * @param doc Document to create the DOM structure within.  The caller
	 * will need to embed the DOM structure at the appropriate place in the
	 * document (using a call to getElement to find the top level element)
	 * @param id Value to set in the Id field.  If NULL, the library will
	 * generate a new Unique Id value.
	 * @returns the new XKMSRevokeRequest structure
	 */

	virtual XKMSRevokeRequest * createRevokeRequest(
		const XMLCh * service,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument * doc,
		const XMLCh * id = NULL) = 0;
		
	/**
	 * \brief Create a new \<RevokeRequest\> message and surrounding document
	 * 
	 * Generates a new RevokeRequest message from scratch, building the DOM
	 * as it goes.
	 *
	 * @param service URI
	 * @param doc Will be used to return the newly created document element in.
	 * @param id Value to set in the Id field.  If NULL, the library will
	 * generate a new Unique Id value.
	 * @returns the new XKMSRevokeRequest structure
	 * @note Like all the xsec library functions, the document is owned by
	 * the calling application.  Deleting the RevokeRequest object will not
	 * delete the DOM document as well.
	 */

	virtual XKMSRevokeRequest * createRevokeRequest(
		const XMLCh * service,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument **doc,
		const XMLCh * id = NULL) = 0;

	/**
	 * \brief Create a new \<RevokeResult\> message.
	 * 
	 * Generates a new RevokeResult message from scratch, building the DOM
	 * as it goes.  The response will be based on a input RevokeRequest message
	 * which is used to provide Id etc.
	 *
	 * @param request Request to base response on
	 * @param doc Will be used to return the newly created document element in.
	 * @param id Value to set in the Id field.  If NULL, the library will
	 * generate a new Unique Id value.
     * @param rmaj Major result code
     * @param rmin Minor result code
	 * @returns the new XKMSRevokeResponse structure
	 */

	virtual XKMSRevokeResult * createRevokeResult(
		XKMSRevokeRequest * request,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument * doc,
		XKMSResultType::ResultMajor rmaj,
		XKMSResultType::ResultMinor rmin = XKMSResultType::NoneMinor,
		const XMLCh * id = NULL) = 0;

	/**
	 * \brief Create a new \<RevokeResult\> message and surrounding document
	 * 
	 * Generates a new RevokeResult message from scratch, building the DOM
	 * as it goes.  The response will be based on a input RevokeRequest message
	 * which is used to provide Id etc.
	 *
	 * @param request Request to base response on
	 * @param doc Will be used to return the newly created document element in.
	 * @param id Value to set in the Id field.  If NULL, the library will
	 * generate a new Unique Id value.
     * @param rmaj Major result code
     * @param rmin Minor result code
	 * @returns the new XKMSRevokeResponse structure
	 * @note Like all the xsec library functions, the document is owned by
	 * the calling application.  Deleting the RevokeRequest object will not
	 * delete the DOM document as well.
	 */

	virtual XKMSRevokeResult * createRevokeResult(
		XKMSRevokeRequest * request,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument **doc,
		XKMSResultType::ResultMajor rmaj,
		XKMSResultType::ResultMinor rmin = XKMSResultType::NoneMinor,
		const XMLCh * id = NULL) = 0;

	/**
	 * \brief Create a new \<RecoverRequest\> message.
	 * 
	 * Generates a new RecoverRequest message from scratch, building the DOM
	 * as it goes.
	 *
	 * @param service URI
	 * @param doc Document to create the DOM structure within.  The caller
	 * will need to embed the DOM structure at the appropriate place in the
	 * document (using a call to getElement to find the top level element)
	 * @param id Value to set in the Id field.  If NULL, the library will
	 * generate a new Unique Id value.
	 * @returns the new XKMSRecoverRequest structure
	 */

	virtual XKMSRecoverRequest * createRecoverRequest(
		const XMLCh * service,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument * doc,
		const XMLCh * id = NULL) = 0;
		
	/**
	 * \brief Create a new \<RecoverRequest\> message and surrounding document
	 * 
	 * Generates a new RecoverRequest message from scratch, building the DOM
	 * as it goes.
	 *
	 * @param service URI
	 * @param doc Will be used to return the newly created document element in.
	 * @param id Value to set in the Id field.  If NULL, the library will
	 * generate a new Unique Id value.
	 * @returns the new XKMSRecoverRequest structure
	 * @note Like all the xsec library functions, the document is owned by
	 * the calling application.  Deleting the RecoverRequest object will not
	 * delete the DOM document as well.
	 */

	virtual XKMSRecoverRequest * createRecoverRequest(
		const XMLCh * service,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument **doc,
		const XMLCh * id = NULL) = 0;

	/**
	 * \brief Create a new \<RecoverResult\> message.
	 * 
	 * Generates a new RecoverResult message from scratch, building the DOM
	 * as it goes.  The response will be based on a input RecoverRequest message
	 * which is used to provide Id etc.
	 *
	 * @param request Request to base response on
	 * @param doc Will be used to return the newly created document element in.
	 * @param id Value to set in the Id field.  If NULL, the library will
	 * generate a new Unique Id value.
     * @param rmaj Major result code
     * @param rmin Minor result code
	 * @returns the new XKMSRecoverResponse structure
	 */

	virtual XKMSRecoverResult * createRecoverResult(
		XKMSRecoverRequest * request,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument * doc,
		XKMSResultType::ResultMajor rmaj,
		XKMSResultType::ResultMinor rmin = XKMSResultType::NoneMinor,
		const XMLCh * id = NULL) = 0;

	/**
	 * \brief Create a new \<RecoverResult\> message and surrounding document
	 * 
	 * Generates a new RecoverResult message from scratch, building the DOM
	 * as it goes.  The response will be based on a input RecoverRequest message
	 * which is used to provide Id etc.
	 *
	 * @param request Request to base response on
	 * @param doc Will be used to return the newly created document element in.
	 * @param id Value to set in the Id field.  If NULL, the library will
	 * generate a new Unique Id value.
     * @param rmaj Major result code
     * @param rmin Minor result code
	 * @returns the new XKMSRecoverResponse structure
	 * @note Like all the xsec library functions, the document is owned by
	 * the calling application.  Deleting the RecoverRequest object will not
	 * delete the DOM document as well.
	 */

	virtual XKMSRecoverResult * createRecoverResult(
		XKMSRecoverRequest * request,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument **doc,
		XKMSResultType::ResultMajor rmaj,
		XKMSResultType::ResultMinor rmin = XKMSResultType::NoneMinor,
		const XMLCh * id = NULL) = 0;

	/**
	 * \brief Create a new \<ReissueRequest\> message.
	 * 
	 * Generates a new ReissueRequest message from scratch, building the DOM
	 * as it goes.
	 *
	 * @param service URI
	 * @param doc Document to create the DOM structure within.  The caller
	 * will need to embed the DOM structure at the appropriate place in the
	 * document (using a call to getElement to find the top level element)
	 * @param id Value to set in the Id field.  If NULL, the library will
	 * generate a new Unique Id value.
	 * @returns the new XKMSReissueRequest structure
	 */

	virtual XKMSReissueRequest * createReissueRequest(
		const XMLCh * service,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument * doc,
		const XMLCh * id = NULL) = 0;
		
	/**
	 * \brief Create a new \<ReissueRequest\> message and surrounding document
	 * 
	 * Generates a new ReissueRequest message from scratch, building the DOM
	 * as it goes.
	 *
	 * @param service URI
	 * @param doc Will be used to return the newly created document element in.
	 * @param id Value to set in the Id field.  If NULL, the library will
	 * generate a new Unique Id value.
	 * @returns the new XKMSReissueRequest structure
	 * @note Like all the xsec library functions, the document is owned by
	 * the calling application.  Deleting the ReissueRequest object will not
	 * delete the DOM document as well.
	 */

	virtual XKMSReissueRequest * createReissueRequest(
		const XMLCh * service,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument **doc,
		const XMLCh * id = NULL) = 0;

	/**
	 * \brief Create a new \<ReissueResult\> message.
	 * 
	 * Generates a new ReissueResult message from scratch, building the DOM
	 * as it goes.  The response will be based on a input ReissueRequest message
	 * which is used to provide Id etc.
	 *
	 * @param request Request to base response on
	 * @param doc Will be used to return the newly created document element in.
	 * @param id Value to set in the Id field.  If NULL, the library will
	 * generate a new Unique Id value.
     * @param rmaj Major result code
     * @param rmin Minor result code
	 * @returns the new XKMSReissueResponse structure
	 */

	virtual XKMSReissueResult * createReissueResult(
		XKMSReissueRequest * request,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument * doc,
		XKMSResultType::ResultMajor rmaj,
		XKMSResultType::ResultMinor rmin = XKMSResultType::NoneMinor,
		const XMLCh * id = NULL) = 0;

	/**
	 * \brief Create a new \<ReissueResult\> message and surrounding document
	 * 
	 * Generates a new ReissueResult message from scratch, building the DOM
	 * as it goes.  The response will be based on a input ReissueRequest message
	 * which is used to provide Id etc.
	 *
	 * @param request Request to base response on
	 * @param doc Will be used to return the newly created document element in.
	 * @param id Value to set in the Id field.  If NULL, the library will
	 * generate a new Unique Id value.
     * @param rmaj Major result code
     * @param rmin Minor result code
	 * @returns the new XKMSReissueResponse structure
	 * @note Like all the xsec library functions, the document is owned by
	 * the calling application.  Deleting the ReissueRequest object will not
	 * delete the DOM document as well.
	 */

	virtual XKMSReissueResult * createReissueResult(
		XKMSReissueRequest * request,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument **doc,
		XKMSResultType::ResultMajor rmaj,
		XKMSResultType::ResultMinor rmin = XKMSResultType::NoneMinor,
		const XMLCh * id = NULL) = 0;

	//@}

	/** @name Environment Manipulation Functions */
	//@{

	/**
	  * \brief Set the prefix be used for the DSIG namespace.
	  *
	  * @param prefix The UTF-16 encoided NS prefix to use for the XML 
	  * Digital Signature nodes
	  */

	virtual void setDSIGNSPrefix(const XMLCh * prefix) = 0;

	/**
	  * \brief Set the prefix be used for the Exclusive Canonicalisation namespace.
	  *
	  * The Exclusive Canonicalisation specification defines a new namespace for the
	  * InclusiveNamespaces node.  This function can be used to set the prefix
	  * that the library will use when creating nodes within this namespace.
	  *
	  * <p>xmlns:ds="http://www.w3.org/2001/10/xml-exc-c14n#"</p>
	  *
	  * If no prefix is set, the default namespace will be used
	  *
	  * @param prefix The UTF-16 encoided NS prefix to use for the XML 
	  * Exclusive Canonicalisation nodes
	  */

	virtual void setECNSPrefix(const XMLCh * prefix) = 0;

	/**
	  * \brief Set the prefix be used for the XPath-Filter2 namespace.
	  *
	  * The XPathFilter definition uses its own namespace.  This
	  * method can be used to set the prefix that the library will use
	  * when creating elements in this namespace
	  *
	  * <p>xmlns:ds="http://www.w3.org/2002/06/xmldsig-filter2"</p>
	  *
	  * If no prefix is set, the default namespace will be used
	  *
	  * @param prefix The UTF-16 encoided NS prefix to use for the XPath
	  * filter nodes
	  */

	virtual void setXPFNSPrefix(const XMLCh * prefix) = 0;

	/**
	  * \brief Set the prefix be used for the XML Encryption namespace.
	  *
	  * @param prefix The UTF-16 encoided NS prefix to use for the XML
	  * Encryption nodes
	  */

	virtual void setXENCNSPrefix(const XMLCh * prefix) = 0;

	/**
	  * \brief Set the prefix be used for the XKMS Namespace
	  *
	  * @param prefix The UTF-16 encoided NS prefix to use for the XKMS
	  * nodes
	  */

	virtual void setXKMSNSPrefix(const XMLCh * prefix) = 0;

	//@}

	/** @name Message Conversions */
	//@{

	/**
	 * \brief Convert to a Request type
	 *
	 * Used to convert a MessageAbstractType to a RequestAbstractType
	 *
	 * @param msg The message to convert
	 * @return the same message typecast to a RequestAbstractType (if NULL otherwise)
	 * otherwise
	 */

	virtual XKMSRequestAbstractType * toRequestAbstractType(XKMSMessageAbstractType *msg) = 0;

	/**
	 * \brief Convert to a Result type
	 *
	 * Used to convert a MessageAbstractType to a ResultType
	 *
	 * @param msg The message to convert
	 * @return the same message typecast to a ResultType (if OK - NULL otherwise)
	 * otherwise
	 */

	virtual XKMSResultType * toResultType(XKMSMessageAbstractType *msg) = 0;

	//@}
private:

	// Unimplemented
	XKMSMessageFactory(const XKMSMessageFactory &);
	XKMSMessageFactory & operator = (const XKMSMessageFactory &);

};

#endif /* XKMSMESSAGEFACTORY_INCLUDE */
