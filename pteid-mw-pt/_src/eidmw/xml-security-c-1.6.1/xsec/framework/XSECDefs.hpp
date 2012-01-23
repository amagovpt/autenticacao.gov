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
 * XSECDefs := File for general XSEC definitions
 *
 * Author(s): Berin Lautenbach
 *
 * $Id: XSECDefs.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

// Use Xerces to do the "hard work in determining compilers etc." for us

#ifndef XSECDEFS_HEADER
#define XSECDEFS_HEADER

// General includes

#include <assert.h>
#include <stdlib.h>

// Include the generated include file

#if defined (_WIN32)
#	include <xsec/framework/XSECW32Config.hpp>
#	if defined (_DEBUG) && defined (_XSEC_DO_MEMDEBUG)
#		define _CRTDBG_MAP_ALLOC
#		include <crtdbg.h>
#	endif
#	define WIN32_LEAN_AND_MEAN
#	include <windows.h>
#elif defined (XSEC_LIBRARY_BUILD)
#       include "config.h"
#else
#	include <xsec/framework/XSECConfig.hpp>
#endif

// Xalan

//#include <Include/PlatformDefinitions.hpp>

// Xerces

#include <xercesc/util/XercesDefs.hpp>

// Define "sizes" of data to correspond to Xerces APIs
#ifdef XSEC_XERCES_64BITSAFE
    typedef XMLSize_t xsecsize_t;
#else
    typedef unsigned int xsecsize_t;
#endif


// --------------------------------------------------------------------------------
//           Namespace Handling
// --------------------------------------------------------------------------------

// Use an approach similar to that used in Xalan to process Xerces namespaces

#if defined(XERCES_HAS_CPP_NAMESPACE)
#	define XSEC_USING_XERCES(NAME) using XERCES_CPP_NAMESPACE :: NAME
#	define XSEC_DECLARE_XERCES_CLASS(NAME) namespace XERCES_CPP_NAMESPACE { class NAME; }
#	define XSEC_DECLARE_XERCES_STRUCT(NAME) namespace XERCES_CPP_NAMESPACE { struct NAME; }
#else
#	define XERCES_CPP_NAMESPACE_QUALIFIER
#	define XERCES_CPP_NAMESPACE_BEGIN
#	define XERCES_CPP_NAMESPACE_END
#	define XERCES_CPP_NAMESPACE_USE
#	define XSEC_USING_XERCES(NAME)
#	define XSEC_DECLARE_XERCES_CLASS(NAME) class NAME;
#	define XSEC_DECLARE_XERCES_STRUCT(NAME) struct NAME;
#endif

#if defined (XSEC_XERCES_XMLSTRING_HAS_RELEASE)
#    define XSEC_RELEASE_XMLCH(x) XMLString::release(&(x))
#else
#    define XSEC_RELEASE_XMLCH(x) delete[] (x);
#endif

// --------------------------------------------------------------------------------
//           Project Library Handling
// --------------------------------------------------------------------------------

#if defined (PLATFORM_EXPORT)
#  define XSEC_PLATFORM_EXPORT PLATFORM_EXPORT
#  define XSEC_PLATFORM_IMPORT PLATFORM_IMPORT
#else
#  define XSEC_PLATFORM_EXPORT XERCES_PLATFORM_EXPORT
#  define XSEC_PLATFORM_IMPORT XERCES_PLATFORM_IMPORT
#endif

#if defined (PROJ_CANON)

#define CANON_EXPORT XSEC_PLATFORM_EXPORT
#else
#define CANON_EXPORT XSEC_PLATFORM_IMPORT
#endif

#if defined (PROJ_DSIG)

#define DSIG_EXPORT XSEC_PLATFORM_EXPORT
#else
#define DSIG_EXPORT XSEC_PLATFORM_IMPORT
#endif

// Platform includes.  Much of this is taken from Xalan

#if defined(_MSC_VER)

// Microsoft VC++

#	pragma warning(disable: 4127 4251 4511 4503 4512 4514 4702 4710 4711 4786 4097; error: 4150 4172 4238 4239 4715)
#	define XSEC_NO_COVARIANT_RETURN_TYPE
/*
 * Removed to allow any compiler to compile - might not work, but ....
 * #elif defined(__GNUC__)
 * #elif defined(__INTEL_COMPILER)
 * #else
 * #error Unknown compiler.
 */
#endif


// Configuration includes

// We want to use XPath calculated transforms

//#define XSEC_USE_XPATH_ENVELOPE


// Given the configuration - what should we set?

#ifdef XSEC_NO_XALAN

// Xalan is not available!

#	define XSEC_NO_XPATH
#	define XSEC_NO_XSLT

#endif	/* XSEC_NO_XALAN */

#ifdef XSEC_NO_XPATH

#	ifdef XSEC_USE_XPATH_ENVELOPE
#		undef XSEC_USE_XPATH_ENVELOPE
#	endif

#endif

#ifdef XSEC_LIBRARY_BUILD
#   ifdef XSEC_HAVE_STRCASECMP
#       define _stricmp(x,y) strcasecmp(x,y)
#   else
#       define _stricmp(x,y) stricmp(x,y)
#   endif
#endif

#endif /* XSECDEFS_HEADER */
