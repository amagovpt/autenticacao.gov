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
 * XSECError := General include file for handling errors
 *
 * Author(s): Berin Lautenbach
 *
 * $Id: XSECError.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/framework/XSECException.hpp>
#include <xsec/enc/XSECCryptoException.hpp>
#include <new>

/**
 * @defgroup pubsig Main Signature API
 * This section describes the main classes and interfaces necessary for 
 * programming with the XML-Security-C library.
 * @{
 */

/**
 * \brief Error strings
 *
 * An array that can be used to obtain an error string associated with
 * an exception number.
 */

extern const char * XSECExceptionStrings [];

/** @} */

#if defined (_WIN32) && defined (_DEBUG) && defined (_XSEC_DO_MEMDEBUG_OLD)

#	define XSECnew( a, b ) \
	try {\
		if (( a = DEBUG_NEW b ) == NULL) { \
			throw XSECException (XSECException::MemoryAllocationFail); \
		}\
	} \
	catch (XSECCryptoException &e) \
	{\
		throw XSECException (XSECException::InternalError, e.getMsg()); \
	} \
	catch (std::bad_alloc&) { \
		throw XSECException (XSECException::MemoryAllocationFail); \
	}

#else 

#	define XSECnew(a, b) \
	try {\
		if ((a = new b) == NULL) { \
			throw XSECException (XSECException::MemoryAllocationFail); \
		} \
	} \
	catch (XSECCryptoException &e) \
	{\
		throw XSECException (XSECException::InternalError, e.getMsg()); \
	} \
	catch (std::bad_alloc&) { \
		throw XSECException (XSECException::MemoryAllocationFail); \
	}

#endif
