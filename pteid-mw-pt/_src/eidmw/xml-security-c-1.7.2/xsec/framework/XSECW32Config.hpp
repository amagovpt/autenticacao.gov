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
 * Configuration file for Windows platform
 *
 * Needs to be modified by hand
 *
 * Author(s): Berin Lautenbach
 *
 * $Id: XSECW32Config.hpp 1350042 2012-06-13 22:29:17Z scantor $
 *
 */

#include <xercesc/util/XercesVersion.hpp>

/*
 * Because we don't have a configure script, we need to rely on version
 * numbers to understand library idiosycracies
 */

#if (XERCES_VERSION_MAJOR >= 3)

/* Is it possible to setIdAttributes? - DOM level 3 call.  V3.x
   API Version */

#	define XSEC_XERCES_HAS_BOOLSETIDATTRIBUTE 1

/* 3.0 no longer supports DOMWriter, must use DOMLSSerializer instead
*/

#	define XSEC_XERCES_DOMLSSERIALIZER 1

/* 3.0 now uses getInputEncoding rather than getEncoding to determine
   encoding that was found in input document */

#	define XSEC_XERCES_DOMENTITYINPUTENCODING 1

/* 3.0 now 64-bit safe */

#   define XSEC_XERCES_64BITSAFE 1

/* 3.0 InputStream must expose content type */

#   define XSEC_XERCES_INPUTSTREAM_HAS_CONTENTTYPE 1
#endif

#if (XERCES_VERSION_MAJOR == 3) || ((XERCES_VERSION_MAJOR == 2) && (XERCES_VERSION_MINOR >= 3))
/*
 * As of version 2.3, xerces requires a version parameter in XMLFormatter
 * constructors
 */
#	define XSEC_XERCES_FORMATTER_REQUIRES_VERSION 1

 /* 2.3 and above use a user defined Memory Manager.  In some cases, this
   actually needs to be passed to functions
*/

#	define XSEC_XERCES_REQUIRES_MEMMGR 1

/* Does XMLString::release() exist */

#	define XSEC_XERCES_XMLSTRING_HAS_RELEASE 1

#	if (XERCES_VERSION_MAJOR < 3)
		/* Is it possible to setIdAttributes? - DOM level 3 call.  V2.x API */

#		define XSEC_XERCES_HAS_SETIDATTRIBUTE 1

#	endif

#else
/*
 * In version 2.2, the XMLUri class was broken for relative URI de-referencing
 */
#	define XSEC_XERCES_BROKEN_XMLURI 1
#endif



/*
 * The following defines whether Xalan integration is required.
 *
 * Xalan is used for XSLT and complex XPath processing.
 * Activate this #define if Xalan is not required (or desired)
 */

#define XSEC_NO_XALAN

#if !defined (XSEC_NO_XALAN)

#	include <xalanc/Include/XalanVersion.hpp>

#	if (_XALAN_VERSION <= 10800)
#		define XSEC_XSLEXCEPTION_RETURNS_DOMSTRING	1
#	endif
#	if (_XALAN_VERSION >= 10900)

		/* 1.9 and above have XSLException::getType() returns XalanDOMChar *, not
			XalanDOMString
		*/

#		undef XSEC_XSLEXCEPTION_RETURNS_DOMSTRING

		/* 1.9 and above do not take a XercesDOMSupport as input to the ctor */

#		undef XSEC_XERCESPARSERLIAISON_REQS_DOMSUPPORT

		/* 1.9 and above require a NodeRefList as input to XPathEvaluator::
		   selectNodeList
		*/

#		define XSEC_SELECTNODELIST_REQS_NODEREFLIST

		/* 1.9 and above use MemoryManager for the XPath Function classes
		*/

#		define XSEC_XALAN_REQS_MEMORYMANAGER
#	else
		/* 1.9 and above have XSLException::getType() returns XalanDOMChar *, not
			XalanDOMString
		*/

#		define XSEC_XSLEXCEPTION_RETURNS_DOMSTRING 1

		/* 1.9 and above do not take a XercesDOMSupport as input to the ctor */

#		define XSEC_XERCESPARSERLIAISON_REQS_DOMSUPPORT

		/* 1.9 and above require a NodeRefList as input to XPathEvaluator::
		   selectNodeList
		*/

#		undef XSEC_SELECTNODELIST_REQS_NODEREFLIST

		/* 1.9 and above use MemoryManager for the XPath Function classes
		*/

#		undef XSEC_XALAN_REQS_MEMORYMANAGER

#	endif

#endif

/*
 * Define presence of cryptographic providers
 */

#define XSEC_HAVE_OPENSSL 1

#define XSEC_HAVE_WINCAPI 1

// NSS Code is currently alpha.  It should work, but you will also
// need to include the NSS libraries during the link.

/* #define XSEC_HAVE_NSS 1 */


/*
 * Some settings for OpenSSL if we have it
 *
 */

#if defined (XSEC_HAVE_OPENSSL)

#	include <openssl/opensslv.h>
#	if (OPENSSL_VERSION_NUMBER >= 0x00907000)
#		define XSEC_OPENSSL_CONST_BUFFERS
#		define XSEC_OPENSSL_HAVE_AES
#       define XSEC_OPENSSL_HAVE_EC
#		define XSEC_OPENSSL_CANSET_PADDING
#		define XSEC_OPENSSL_HAVE_CRYPTO_CLEANUP_ALL_EX_DATA
#	endif
#	if (OPENSSL_VERSION_NUMBER >= 0x00908000)
#		define XSEC_OPENSSL_D2IX509_CONST_BUFFER
#       define XSEC_OPENSSL_HAVE_SHA2
#       define XSEC_OPENSSL_HAVE_MGF1
#	endif
#	if (OPENSSL_VERSION_NUMBER >= 0x10001000)
#		define XSEC_OPENSSL_HAVE_GCM
#	endif

#endif

/*
 * Macros used to determine what header files exist on this
 * system
 */

/* Posix unistd.h */
/* #define HAVE_UNISTD_H */

/* Windows direct.h */
#define HAVE_DIRECT_H 1

#include <xsec/framework/XSECVersion.hpp>
