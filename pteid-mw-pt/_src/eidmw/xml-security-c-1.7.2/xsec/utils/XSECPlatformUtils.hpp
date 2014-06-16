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
 * XSECPlatformUtils:= To support the platform we run in
 *
 * Author(s): Berin Lautenbach
 *
 * $Id: XSECPlatformUtils.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *					 
 */

#ifndef XSECPLATFORMUTILS_INCLUDE
#define XSECPLATFORMUTILS_INCLUDE

#include <xercesc/dom/DOM.hpp>

// XSEC

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/enc/XSECCryptoProvider.hpp>

class TXFMBase;
class XSECAlgorithmMapper;
class XSECAlgorithmHandler;

#include <stdio.h>

/**
 * \brief High level library interface class.
 * @ingroup internal
 *
 * This class is used primarily to initialise the library and
 * communicate high level parameters that will be common to all
 * objects from the class in any given session.
 *
 * It is primarily a static class.
 */

class DSIG_EXPORT XSECPlatformUtils {

public :

	/**
	 * \brief Number of times initialise has been called 
	 *
	 * initCount can be read by any class or function to determine how
	 * many times the library has been initialised.
	 */

	static int initCount;

	/**
	 * \brief The main cryptographic provider
	 *
	 * This pointer can be used to determine the primary crypto
	 * provider registered in the library.
	 *
	 * Individual signatures can over-ride this default.
	 *
	 */

	static XSECCryptoProvider * g_cryptoProvider;

	/**
	 * \brief The global Algorithm Mapper
	 *
	 * The algorithm mapper is used to map algorithm type URI strings
	 * to algorithm implementations.  Note that this is a level of
	 * indirection above actual cryptographic algorithms.  For example:
	 *
	 * URI = http://www.w3.org/2001/04/xmlenc#tripledes-cbc
	 *
	 * is the URI for 3DES in CBC mode.  The mapper will return an
	 * algorithm handler that understands what this means in terms of
	 * IVs and how to call the XSECCryptoKey interface.  It then uses the
	 * cryptographic provider to actually perform the encryption.
	 *
	 * This allows applications to provide new algorithm types.  The
	 * mapper is used to map the type string to the means of doing the
	 * encryption, and a new XSECCryptoKey derivative can be provided
	 * to perform the actual crypo work.
	 *
	 * @note The provider should only be added to via the
	 * XSECPlatformUtils::registerAlgorithmHandler() call.
	 *
	 * @see #addAlgorithmHandler()
	 */

	static const XSECAlgorithmMapper * g_algorithmMapper;

	/**
	 * \brief Initialise the library
	 *
	 * <b>Must</b> be called prior to using any functions in the library.
	 *
	 * Primarily sets up static variables used by all classes in the
	 * library.
	 *
	 * @param p A pointer to a XSECCryptoProvider object that the library 
	 * should use for cryptographic functions.  If p == NULL, the library
	 * will instantiate an OpenSSLCryptoProvider object.
	 */

	static void Initialise(XSECCryptoProvider * p = NULL);

	/**
	 * \brief Set a new crypto provider
	 * 
	 * Set the crypto provider to the value passed in.  Any current provider
	 * is deleted.
	 *
	 * @note This is not thread-safe.  It should be called prior to any real
	 * usage of the library.
	 *
	 * @param p A pointer to a XSECCryptoProvider object that the library 
	 * should use for cryptographic functions.  
	 * @note Ownership of the provider is passed to the library, which will
	 * delete it at Termination.
	 */

	static void SetCryptoProvider(XSECCryptoProvider * p);

	/**
	 * \brief Add a new algorithm Handler
	 *
	 * Application developers can extend the XSECAlgorithmHandler class to
	 * implement new cryptographic algorithms.  This will then allow the
	 * library to call the provided handler whenever trying to process a
	 * type it doesn't understand.
	 *
	 * Any handler previously registered for this URI will be overwritten,
	 * allowing callers to overwrite the handlers for default URIs.
	 *
	 * @see XSECAlgorithmHandler
	 * @note This is <b>not</b> thread safe.  Algorithm handlers should
	 * be added prior to any processing of signatures etc.
	 * @param uri Type URI that maps to this handler
	 * @param handler The handler to be used whenever this URI is seen by
	 * the library.
	 */

	static void registerAlgorithmHandler(const XMLCh * uri, const XSECAlgorithmHandler & handler);

    /**
     * \brief Indicate an algorithm is approved for use, implying others are not.
     *
     * @see XSECAlgorithmHandler
     * @note This is <b>not</b> thread safe.  Algorithms should
     * be whitelisted prior to any processing of signatures etc.
     * @param URI algorithm to whitelist
     */

	static void whitelistAlgorithm(const XMLCh* URI);

    /**
     * \brief Indicate an algorithm is not approved for use, implying others are.
     *
     * @see XSECAlgorithmHandler
     * @note This is <b>not</b> thread safe.  Algorithms should
     * be blacklisted prior to any processing of signatures etc.
     * @param URI algorithm to blacklist
     */

    static void blacklistAlgorithm(const XMLCh* URI);

    typedef TXFMBase* TransformFactory(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument*);

    /**
     * \brief Installs logging support during Reference processing
     *
     * The function provided will be called during Reference computation
     * to obtain a transform interface to place at the end of the
     * transform chain. It will be given the chance to log or preserve
     * the result of applying transforms to References during signing
     * and verification operations.
     */
    static void SetReferenceLoggingSink(TransformFactory* factory);

    /**
     * \brief Returns a transform for logging of Reference processing
     *
     * @param doc   the DOM document containing the data being processed
     * @return  a transform to install for logging of Reference data, or NULL
     */
    static TXFMBase* GetReferenceLoggingSink(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* doc);

	/**
	 * \brief Terminate
	 *
	 * Should be called prior to any program exist to allow the library
	 * to cleanly delete any memory associated with the library as a whole.
	 *
	 * @note Do not call this function while any xml-security-c object
	 * remain instantiated.  The results of doing so is undefined, and could
	 * cause bad results.
	 */

	static void Terminate(void);

private:
	static TransformFactory* g_loggingSink;
};


#endif /* XSECPLATFORMUTILS_INCLUDE */

