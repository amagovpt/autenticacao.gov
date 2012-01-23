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
 * XSECException:= How we throw exceptions in XSEC
 *
 * Author(s): Berin Lautenbach
 *
 * $Id: XSECException.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef XSECEXCEPTION_INCLUDE
#define XSECEXCEPTION_INCLUDE

#include <xsec/framework/XSECDefs.hpp>

/**
 * @addtogroup pubsig
 * @{
 */

/**
 * @brief Exception Class.
 *
 * <p>This class is used for all Exceptions raised by the XSEC library.
 * It holds a "type" number that can be used to find the generic type
 * of the exception, as well as a XMLCh string that holds a description of
 * the error</p>
 *
 */

class DSIG_EXPORT XSECException {

public:

	/**
	 * \brief Type of Exception thrown.
	 *
	 * This enumerated type is used to inform the recipient of the 
	 * exception what generic error caused it to be raised.
	 */

	enum XSECExceptionType {

		None						= 0,
		MemoryAllocationFail		= 1,
		NoHashFoundInDigestValue	= 2,
		UnknownDSIGAttribute		= 3,
		ExpectedDSIGChildNotFound	= 4,
		UnknownTransform			= 5,
		TransformInputOutputFail	= 6,
		IDNotFoundInDOMDoc			= 7,
		UnsupportedXpointerExpr		= 8,
		XPathError					= 9,
		XSLError					= 10,
		Unsupported					= 11,
		LoadEmptySignature			= 12,
		LoadNonSignature			= 13,
		UnknownCanonicalization		= 14,
		UnknownSignatureAlgorithm	= 15,
		LoadEmptyX509				= 16,
		LoadNonX509					= 17,
		OpenSSLError				= 18,
		SigVfyError					= 19,
		LoadEmptySignedInfo			= 20,
		LoadNonSignedInfo			= 21,
		ExpectedReferenceURI		= 22,
		NotLoaded					= 23,
		CryptoProviderError			= 24,
		KeyInfoError				= 25,
		SigningError				= 26,
		LoadEmptyInfoName			= 27,
		LoadNonInfoName				= 28,
		UnknownKeyValue				= 29,
		SignatureCreationError		= 30,
		ErrorOpeningURI				= 31,
		ProviderError				= 32,
		InternalError				= 33,
		EnvelopeError				= 34,
		UnsupportedFunction			= 35,
		TransformError				= 36,
		SafeBufferError				= 37,
		HTTPURIInputStreamError     = 38,
		LoadEmptyXPathFilter		= 39,
		XPathFilterError			= 40,
		DNameDecodeError			= 41,

		// Encryption errors
		EncryptedTypeError			= 42,
		ExpectedXENCChildNotFound	= 43,
		CipherDataError				= 44,
		CipherValueError			= 45,
		CipherError					= 46,
		AlgorithmMapperError		= 47,
		EncryptionMethodError		= 48,
		CipherReferenceError		= 49,
		ObjectError					= 50,

		// XKMS Error
		XKMSError					= 51,
		ExpectedXKMSChildNotFound	= 52,
		MessageAbstractTypeError	= 53,
		RequestAbstractTypeError	= 54,
		ResultTypeError				= 55,
		StatusError 				= 56,
		UnknownError				= 58		// Must be last!

	};

	/** @name Constructors and Destructors */
	//@{

	/**
	 * \brief General Constructor
	 *
	 * Generic constructor used within the library.  Where inMsg == NULL,
	 * the general string for this exception type will be used.
	 *
	 * @param eNum Exception type
	 * @param inMsg Msg to be used or NULL for general system message
	 */

	XSECException(XSECExceptionType eNum, const XMLCh * inMsg = NULL);

	/**
	 * \brief Local code page constructor
	 *
	 * Shortcut constructor to allow local code page strings to be used
	 * for the message.  Strings are converted to UTF-16.
	 *
	 * @param eNum Exception type
	 * @param inMsg Msg to be used or NULL for general system message
	 */

	XSECException(XSECExceptionType eNum, const char * inMsg);

	// XSECException(XSECExceptionType eNum, safeBuffer &inMsg);
	
	/**
	 * \brief Copy Constructor
	 */

	XSECException(const XSECException &toCopy);
	/**
	 * \brief Destructor
	 */

	~XSECException();
	
	//@}

	/** @name Information functions */
	//@{

	/**
	 * \brief Get message
	 *
	 * Return a pointer to the XMLCh buffer holding the error message
	 *
	 * @returns A pointer to the buffer within the exception that holds the
	 * error message */

	const XMLCh * getMsg(void);

	/**
	 * \brief Get error type
	 *
	 * Returns an XSECExceptionType coding of the generic error that raised
	 * this exception
	 *
	 * @returns The excetpion type
	 */

	XSECExceptionType getType(void);

private:

	XMLCh				* msg;				// Message to the caller
	XSECExceptionType	type;				// Type of exception
	
	/* Unimplemented Constructor */
	XSECException();


};

/** @}*/

#endif /* XSECEXCEPTION_INCLUDE */
