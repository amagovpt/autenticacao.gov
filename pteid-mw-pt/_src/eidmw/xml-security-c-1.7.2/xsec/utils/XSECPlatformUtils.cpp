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
 * $Id: XSECPlatformUtils.cpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

// XSEC

#include <xsec/utils/XSECPlatformUtils.hpp>
#include <xsec/framework/XSECError.hpp>
#include <xsec/dsig/DSIGConstants.hpp>
#include <xsec/dsig/DSIGSignature.hpp>
#include <xsec/xkms/XKMSConstants.hpp>
#include <xsec/framework/XSECAlgorithmMapper.hpp>
#include <xsec/transformers/TXFMOutputFile.hpp>

#include "../xenc/impl/XENCCipherImpl.hpp"

XERCES_CPP_NAMESPACE_USE

#if defined(_WIN32)
#include <xsec/utils/winutils/XSECBinHTTPURIInputStream.hpp>
#endif

#if defined (XSEC_HAVE_OPENSSL)
#	include <xsec/enc/OpenSSL/OpenSSLCryptoProvider.hpp>
#endif

#if defined (XSEC_HAVE_WINCAPI)
#	include <xsec/enc/WinCAPI/WinCAPICryptoProvider.hpp>
#endif

#if defined (XSEC_HAVE_NSS)
#	include <xsec/enc/NSS/NSSCryptoProvider.hpp>
#endif

// Static data used by all of XSEC
int XSECPlatformUtils::initCount = 0;
XSECCryptoProvider * XSECPlatformUtils::g_cryptoProvider = NULL;

// Have a const copy for external usage
const XSECAlgorithmMapper * XSECPlatformUtils::g_algorithmMapper = NULL;

XSECAlgorithmMapper * internalMapper = NULL;

XSECPlatformUtils::TransformFactory* XSECPlatformUtils::g_loggingSink = NULL;

// Determine default crypto provider

#if defined (XSEC_HAVE_OPENSSL)
#	define XSEC_DEFAULT_PROVIDER	OpenSSLCryptoProvider()
#else
#	if defined (XSEC_HAVE_WINCAPI)
#		define XSEC_DEFAULT_PROVIDER	WinCAPICryptoProvider()
#	else 
#		if defined (XSEC_HAVE_NSS)
#			define XSEC_DEFAULT_PROVIDER	NSSCryptoProvider()
#		endif
#	endif
#endif

TXFMBase* TXFMOutputFileFactory(DOMDocument* doc) {

    TXFMOutputFile* sink = new TXFMOutputFile(doc);
    if (sink)
        sink->setFile(getenv("XSEC_DEBUG_FILE"));
    return sink;

}

void XSECPlatformUtils::Initialise(XSECCryptoProvider * p) {

	if (++initCount > 1)
		return;

	if (p != NULL)
		g_cryptoProvider = p;
	else
#if defined XSEC_DEFAULT_PROVIDER
		XSECnew(g_cryptoProvider, XSEC_DEFAULT_PROVIDER);
#else
	throw XSECException(XSECException::CryptoProviderError,
		"XSECPlatformUtils::Initialise() called with NULL provider, but no default defined");
#endif

	// Set up necessary constants
	DSIGConstants::create();
	XKMSConstants::create();

	// Initialise the safeBuffer system
	safeBuffer::init();

	// Initialise Algorithm Mapper
	XSECnew(internalMapper, XSECAlgorithmMapper);
	g_algorithmMapper = internalMapper;

	// Initialise the XENCCipherImpl class
	XENCCipherImpl::Initialise();

	// Initialise the DSIGSignature class
	DSIGSignature::Initialise();

	const char* sink = getenv("XSEC_DEBUG_FILE");
	if (sink && *sink)
	    g_loggingSink = TXFMOutputFileFactory;
};

void XSECPlatformUtils::SetCryptoProvider(XSECCryptoProvider * p) {

	if (g_cryptoProvider != NULL)
		delete g_cryptoProvider;

	g_cryptoProvider = p;

}

void XSECPlatformUtils::SetReferenceLoggingSink(TransformFactory* factory) {

    g_loggingSink = factory;

}

TXFMBase* XSECPlatformUtils::GetReferenceLoggingSink(DOMDocument* doc) {

    return (g_loggingSink ? g_loggingSink(doc) : NULL);
}

void XSECPlatformUtils::Terminate(void) {

	if (--initCount > 0)
		return;

	// Clean out the algorithm mapper
	delete internalMapper;

	if (g_cryptoProvider != NULL)
		delete g_cryptoProvider;

	DSIGConstants::destroy();
	XKMSConstants::destroy();

	// Destroy anything platform specific
#if defined(_WIN32)
	XSECBinHTTPURIInputStream::Cleanup();
#endif

}

void XSECPlatformUtils::registerAlgorithmHandler(
		const XMLCh * uri, 
		const XSECAlgorithmHandler & handler) {

	internalMapper->registerHandler(uri, handler);

}

void XSECPlatformUtils::whitelistAlgorithm(const XMLCh* uri) {

    internalMapper->whitelistAlgorithm(uri);

}

void XSECPlatformUtils::blacklistAlgorithm(const XMLCh* uri) {

    internalMapper->blacklistAlgorithm(uri);

}
