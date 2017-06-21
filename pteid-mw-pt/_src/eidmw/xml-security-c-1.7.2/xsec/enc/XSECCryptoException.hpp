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
 * XSECCryptoException:= How we throw exceptions in XSEC
 *
 * Author(s): Berin Lautenbach
 *
 * $Id: XSECCryptoException.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */
#ifndef XSECCRYPTOEXCEPTION_INCLUDE
#define XSECCRYPTOEXCEPTION_INCLUDE

#include <xsec/utils/XSECSafeBuffer.hpp>

#include <stdlib.h>

/**
 * @ingroup crypto
 */

/**
 *\brief Exception strings
 *
 * Global array of strings that can be looked up using the #type element
 * of an XSECCryptoException to determine a default string for the erro
 */


extern const char * XSECCryptoExceptionStrings[];

/**
 * @ingroup crypto
 */

/**
 * \brief Exception class used by the cryptographic modules
 *
 * This exception class should be used by the Cryptographic providers
 * for reporting errors. It can be used directly or by classes which
 * inherit from it.
 *
 * It's a fairly standard exception class, with a type parameter
 * (and associated enumerated type) and a string that can be used to
 * provide more information to the caller.
 */

class DSIG_EXPORT XSECCryptoException {

public:

	/**
	 * \brief Defines the error type
	 *
	 * Enumerated type that can be looked up by the receiver of the
	 * exception to determine what the error was.
	 *
	 * Can also be used as a lookup into the XSECCryptoExceptionStrings
	 * global array for a default error description
	 */

	enum XSECCryptoExceptionType {

		None						= 0,
		GeneralError				= 1,
		MDError						= 2,		// Error in Message Digest
		Base64Error					= 3,		// Error in a Base64 operation
		MemoryError					= 4,		// Memory allocation error
		X509Error					= 5,		// X509 problem
		DSAError					= 6,		// DSA Error
		RSAError					= 7,		// RSA Error
		SymmetricError				= 8,
		UnsupportedError			= 9,		// Called function is not supported
		UnsupportedAlgorithm        = 10,
        ECError                     = 11,
		UnknownError				= 12		// Must be last!

	};

	
public:

	/** @name Contructors and Destructors */
	//@{

	/**
	 * \brief Common constructur
	 *
	 * Construct an exception with the given type and (possibly blank)
	 * message.
	 *
	 * @param eNum The error type
	 * @param inMsg The message to be passed to the caller.  If NULL
	 * this will be set to the associated value in 
	 * XSECCryptoExceptionStrings
	 */

	XSECCryptoException(XSECCryptoExceptionType eNum, const char * inMsg = NULL);

	/**
	 * \brief safeBuffer constructor
	 *
	 * As for the common constructor, but passes in a safeBuffer message
	 *
	 * @param eNum The error type
	 * @param inMsg The message string to be passed to the caller.
	 */

	XSECCryptoException(XSECCryptoExceptionType eNum, safeBuffer &inMsg);

	/**
	 * \brief Copy Constructor.
	 *
	 * @param toCopy The exception to be copied into the new exception
	 */

	XSECCryptoException(const XSECCryptoException &toCopy);
	~XSECCryptoException();

	//@}

	/** @name Get Methods */
	//@{

	/**
	 * \brief Get the message
	 *
	 * Allows the receiver of the exception to get the error message.
	 *
	 * @returns A pointer to the char buffer holding the error string
	 */

	const char * getMsg(void);

	/**
	 * \brief Get the error type
	 *
	 * @returns The error type of the error that caused the exception
	 */

	//@} 

	XSECCryptoExceptionType getType(void);

private:

	char				* msg;				// Message to the caller
	XSECCryptoExceptionType	type;				// Type of exception
	XSECCryptoException();

};


#endif /* XSECCryptoEXCEPTION_HEADER */
