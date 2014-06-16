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
 * XSECProvider.hpp := The main interface for users wishing to gain access
 *                     to signature objects
 *
 * $Id: XSECProvider.cpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#include <xsec/framework/XSECProvider.hpp>
#include <xsec/framework/XSECError.hpp>
#include <xsec/utils/XSECDOMUtils.hpp>
#include <xsec/framework/XSECURIResolverXerces.hpp>

#include "../xenc/impl/XENCCipherImpl.hpp"
#include "../xkms/impl/XKMSMessageFactoryImpl.hpp"

XERCES_CPP_NAMESPACE_USE

// --------------------------------------------------------------------------------
//           Constructors/Destructors
// --------------------------------------------------------------------------------


XSECProvider::XSECProvider() {

	mp_URIResolver = new XSECURIResolverXerces();
	XSECnew(mp_xkmsMessageFactory, XKMSMessageFactoryImpl());

}

XSECProvider::~XSECProvider() {

	// First delete signatures
	
	SignatureListVectorType::iterator i;
	
	for (i = m_activeSignatures.begin(); i != m_activeSignatures.end(); ++i)
		delete *i;

	m_activeSignatures.clear();

	if (mp_URIResolver != NULL)
		delete mp_URIResolver;

	// Now delete ciphers

	CipherListVectorType::iterator j;
	
	for (j = m_activeCiphers.begin(); j != m_activeCiphers.end(); ++j)
		delete *j;

	m_activeCiphers.clear();

	// Clean up XKMS stuff
	delete mp_xkmsMessageFactory;

}

// --------------------------------------------------------------------------------
//           Signature Creation/Deletion
// --------------------------------------------------------------------------------


DSIGSignature * XSECProvider::newSignatureFromDOM(DOMDocument *doc, DOMNode *sigNode) {

	DSIGSignature * ret;

	XSECnew(ret, DSIGSignature(doc, sigNode));

	setup(ret);

	return ret;

}

DSIGSignature * XSECProvider::newSignatureFromDOM(DOMDocument *doc) {

	DSIGSignature * ret;

	DOMNode *sigNode = findDSIGNode(doc, "Signature");

	if (sigNode == NULL) {
		
		throw XSECException(XSECException::SignatureCreationError,
			"Could not find a signature node in passed in DOM document");

	}

	XSECnew(ret, DSIGSignature(doc, sigNode));

	setup(ret);

	return ret;

}

DSIGSignature * XSECProvider::newSignature(void) {

	DSIGSignature * ret;

	XSECnew(ret, DSIGSignature());

	setup(ret);

	return ret;

}

void XSECProvider::releaseSignature(DSIGSignature * toRelease) {

	// Find in the active list

	SignatureListVectorType::iterator i;

	m_providerMutex.lock();
	i = m_activeSignatures.begin();
	while (i != m_activeSignatures.end() && *i != toRelease)
		++i;

	if (i == m_activeSignatures.end()) {

		m_providerMutex.unlock();

		throw XSECException(XSECException::ProviderError,
			"Attempt to release a signature that was not created by this provider");

	}
	
	// For now - remove from list.  Would be better to recycle
	m_activeSignatures.erase(i);
	m_providerMutex.unlock();
	delete toRelease;

}

// --------------------------------------------------------------------------------
//           Cipher Creation/Deletion
// --------------------------------------------------------------------------------

XENCCipher * XSECProvider::newCipher(DOMDocument * doc) {

	XENCCipherImpl * ret;

	XSECnew(ret, XENCCipherImpl(doc));

	setup(ret);

	return ret;

}

void XSECProvider::releaseCipher(XENCCipher * toRelease) {

	// Find in the active list

	CipherListVectorType::iterator i;

	m_providerMutex.lock();
	i = m_activeCiphers.begin();
	while (i != m_activeCiphers.end() && *i != toRelease)
		++i;

	if (i == m_activeCiphers.end()) {

		m_providerMutex.unlock();

		throw XSECException(XSECException::ProviderError,
			"Attempt to release a cipher that was not created by this provider");

	}
	
	// For now - remove from list.  Would be better to recycle
	m_activeCiphers.erase(i);
	m_providerMutex.unlock();
	delete toRelease;

}


// --------------------------------------------------------------------------------
//           XKMS Methods
// --------------------------------------------------------------------------------

XKMSMessageFactory * XSECProvider::getXKMSMessageFactory(void) {

	return mp_xkmsMessageFactory;

}

// --------------------------------------------------------------------------------
//           Environmental methods
// --------------------------------------------------------------------------------


void XSECProvider::setDefaultURIResolver(XSECURIResolver * resolver) {

	if (mp_URIResolver != 0)
		delete mp_URIResolver;

	mp_URIResolver = resolver->clone();

}

// --------------------------------------------------------------------------------
//           Internal functions
// --------------------------------------------------------------------------------

void XSECProvider::setup(DSIGSignature *sig) {

	// Called by all Signature creation methods to set up the sig

	// Add to the active list
	m_providerMutex.lock();
	m_activeSignatures.push_back(sig);
	m_providerMutex.unlock();

	sig->setURIResolver(mp_URIResolver);

}

void XSECProvider::setup(XENCCipher * cipher) {

	// Called by all Signature creation methods to set up the sig

	// Add to the active list
	m_providerMutex.lock();
	m_activeCiphers.push_back(cipher);
	m_providerMutex.unlock();

}
