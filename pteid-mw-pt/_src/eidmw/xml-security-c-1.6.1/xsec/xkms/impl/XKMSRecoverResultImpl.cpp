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
 * XKMSRecoverResultImpl := Implementation of RecoverResult Messages
 *
 * $Id$
 *
 */

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/framework/XSECError.hpp>
#include <xsec/framework/XSECEnv.hpp>
#include <xsec/framework/XSECAlgorithmMapper.hpp>
#include <xsec/framework/XSECAlgorithmHandler.hpp>
#include <xsec/utils/XSECDOMUtils.hpp>
#include <xsec/xkms/XKMSConstants.hpp>
#include <xsec/enc/XSECCryptoUtils.hpp>
#include <xsec/enc/XSECCryptoKey.hpp>
#include <xsec/xenc/XENCEncryptedData.hpp>
#include <xsec/xenc/XENCEncryptionMethod.hpp>
#include <xsec/xenc/XENCCipher.hpp>

#include "XKMSRecoverResultImpl.hpp"
#include "XKMSKeyBindingImpl.hpp"
#include "XKMSRSAKeyPairImpl.hpp"

#include <xercesc/dom/DOM.hpp>

XERCES_CPP_NAMESPACE_USE

// --------------------------------------------------------------------------------
//           Construct/Destruct
// --------------------------------------------------------------------------------

XKMSRecoverResultImpl::XKMSRecoverResultImpl(
		const XSECEnv * env) :
m_result(env),
m_msg(m_result.m_msg),
mp_RSAKeyPair(NULL),
mp_privateKeyElement(NULL) {

}

XKMSRecoverResultImpl::XKMSRecoverResultImpl(
		const XSECEnv * env, 
		XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * node) :
m_result(env, node),
m_msg(m_result.m_msg),
mp_RSAKeyPair(NULL),
mp_privateKeyElement(NULL) {

}

XKMSRecoverResultImpl::~XKMSRecoverResultImpl() {

	XKMSRecoverResultImpl::KeyBindingVectorType::iterator i;

	for (i = m_keyBindingList.begin() ; i != m_keyBindingList.end(); ++i) {

		delete (*i);

	}

	if (mp_RSAKeyPair != NULL)
		delete mp_RSAKeyPair;

}


// --------------------------------------------------------------------------------
//           Load from DOM
// --------------------------------------------------------------------------------

// Load elements
void XKMSRecoverResultImpl::load() {

	if (m_msg.mp_messageAbstractTypeElement == NULL) {

		// Attempt to load an empty element
		throw XSECException(XSECException::XKMSError,
			"XKMSRecoverResult::load - called on empty DOM");

	}

	if (!strEquals(getXKMSLocalName(m_msg.mp_messageAbstractTypeElement), 
									XKMSConstants::s_tagRecoverResult)) {
	
		throw XSECException(XSECException::XKMSError,
			"XKMSRecoverResult::load - called incorrect node");
	
	}

	// Get any UnverifiedKeyBinding elements
	DOMNodeList * nl = m_msg.mp_messageAbstractTypeElement->getElementsByTagNameNS(
		XKMSConstants::s_unicodeStrURIXKMS,
		XKMSConstants::s_tagKeyBinding);

	if (nl != NULL) {

		XKMSKeyBindingImpl * kb;
		for (unsigned int i = 0; i < nl->getLength() ; ++ i) {

			XSECnew(kb, XKMSKeyBindingImpl(m_msg.mp_env, (DOMElement *) nl->item(i)));
			m_keyBindingList.push_back(kb);
			kb->load();

		}

	}

	nl = m_msg.mp_messageAbstractTypeElement->getElementsByTagNameNS(
		XKMSConstants::s_unicodeStrURIXKMS,
		XKMSConstants::s_tagPrivateKey);

	if (nl != NULL)
		mp_privateKeyElement = (DOMElement *) nl->item(0);

	// Load the base message
	m_result.load();

}

// --------------------------------------------------------------------------------
//           Create a blank one
// --------------------------------------------------------------------------------
DOMElement * XKMSRecoverResultImpl::createBlankRecoverResult(
		const XMLCh * service,
		const XMLCh * id,
		ResultMajor rmaj,
		ResultMinor rmin) {

	return m_result.createBlankResultType(
		XKMSConstants::s_tagRecoverResult, service, id, rmaj, rmin);

}

// --------------------------------------------------------------------------------
//           Get interface methods
// --------------------------------------------------------------------------------

XKMSMessageAbstractType::messageType XKMSRecoverResultImpl::getMessageType(void) {

	return XKMSMessageAbstractTypeImpl::RecoverResult;

}

// --------------------------------------------------------------------------------
//           UnverifiedKeyBinding handling
// --------------------------------------------------------------------------------


int XKMSRecoverResultImpl::getKeyBindingSize(void) const {

	return (int) m_keyBindingList.size();

}

XKMSKeyBinding * XKMSRecoverResultImpl::getKeyBindingItem(int item) const {

	if (item < 0 || item >= (int) m_keyBindingList.size()) {
		throw XSECException(XSECException::XKMSError,
			"XKMSRecoverResult::getKeyBindingItem - item out of range");
	}

	return m_keyBindingList[item];

}

XKMSKeyBinding * XKMSRecoverResultImpl::appendKeyBindingItem(XKMSStatus::StatusValue status) {

	XKMSKeyBindingImpl * u;

	XSECnew(u, XKMSKeyBindingImpl(m_msg.mp_env));

	m_keyBindingList.push_back(u);

	DOMElement * e = u->createBlankKeyBinding(status);

	// Append the element
	DOMElement * c = findFirstElementChild(m_msg.mp_messageAbstractTypeElement);
	while (c != NULL) {

		if (strEquals(getXKMSLocalName(c), XKMSConstants::s_tagPrivateKey))
			break;

	}

	if (c != NULL) {
		m_msg.mp_messageAbstractTypeElement->insertBefore(e, c);
		if (m_msg.mp_env->getPrettyPrintFlag()) {
			m_msg.mp_messageAbstractTypeElement->insertBefore(
				m_msg.mp_env->getParentDocument()->createTextNode(DSIGConstants::s_unicodeStrNL), c);
		}
	}
	else {
		m_msg.mp_messageAbstractTypeElement->appendChild(e);
		m_msg.mp_env->doPrettyPrint(m_msg.mp_messageAbstractTypeElement);
	}

	return u;

}

// --------------------------------------------------------------------------------
//           RSAKeyPair handling
// --------------------------------------------------------------------------------

XKMSRSAKeyPair * XKMSRecoverResultImpl::getRSAKeyPair(const char * passPhrase) {

	// Already done?
	if (mp_RSAKeyPair != NULL)
		return mp_RSAKeyPair;

	// Nope - can we do it?
	if (mp_privateKeyElement == NULL)
		return NULL;

	// Yep!  Load the key
	unsigned char kbuf[XSEC_MAX_HASH_SIZE];
	unsigned int len = CalculateXKMSKEK((unsigned char *) passPhrase, (int) strlen(passPhrase), kbuf, XSEC_MAX_HASH_SIZE);

    if (len == 0) {
		throw XSECException(XSECException::XKMSError,
			"XKMSRecoverResult::getRSAKeyPair - error deriving KEK");
    }

	XSECProvider prov;
	XENCCipher * cipher = prov.newCipher(m_msg.mp_env->getParentDocument());

	// Find the encrypted info
	DOMNode * n = findXENCNode(mp_privateKeyElement, "EncryptedData");

	// Load into the Cipher class
	XENCEncryptedData * xed = cipher->loadEncryptedData((DOMElement *) n);
	if (xed == NULL) {
		throw XSECException(XSECException::XKMSError,
			"XKMSRecoverResult::getRSAKeyPair - error loading encrypted data");
	}

	// Setup the appropriate key
	if (xed->getEncryptionMethod() == NULL) {
		throw XSECException(XSECException::XKMSError,
			"XKMSRecoverResult::getRSAKeyPair - no <EncryptionMethod> in EncryptedData");
	}

	// Now find if we can get an algorithm for this URI
	XSECAlgorithmHandler *handler;

	handler = 
		XSECPlatformUtils::g_algorithmMapper->mapURIToHandler(
			xed->getEncryptionMethod()->getAlgorithm());

	if (handler == NULL) {
		throw XSECException(XSECException::XKMSError,
			"XKMSRecoverResult::getRSAKeyPair - unable to handle algorithm in EncryptedData");
	}

	XSECCryptoKey * sk = handler->createKeyForURI(
					xed->getEncryptionMethod()->getAlgorithm(),
					(XMLByte *) kbuf,
					XSEC_MAX_HASH_SIZE);

	memset(kbuf, 0, XSEC_MAX_HASH_SIZE);

	cipher->setKey(sk);
	cipher->decryptElement();

	// WooHoo - if we get this far things are looking good!
	DOMElement * kp = findFirstElementChild(mp_privateKeyElement);
	if (kp == NULL || !strEquals(getXKMSLocalName(kp), XKMSConstants::s_tagRSAKeyPair)) {
	
		throw XSECException(XSECException::XKMSError,
			"XKMSRecoverResult::getRSAKeyPair - private key did not decrypt to RSAKeyPair");
	
	}

	XSECnew(mp_RSAKeyPair, XKMSRSAKeyPairImpl(m_msg.mp_env, kp));
	mp_RSAKeyPair->load();

	return mp_RSAKeyPair;
}

XENCEncryptedData * XKMSRecoverResultImpl::setRSAKeyPair(const char * passPhrase,
		XMLCh * Modulus,
		XMLCh * Exponent,
		XMLCh * P,
		XMLCh * Q,
		XMLCh * DP,
		XMLCh * DQ,
		XMLCh * InverseQ,
		XMLCh * D,
		encryptionMethod em,
		const XMLCh * algorithmURI) {

	// Try to set up the key first - if this fails, don't want to have added the
	// XML

	const XMLCh * uri;
	safeBuffer algorithmSB;

	if (em != ENCRYPT_NONE) {
		if (encryptionMethod2URI(algorithmSB, em) != true) {
			throw XSECException(XSECException::XKMSError, 
				"XKMSRecoverResult::setRSAKeyPair - Unknown encryption method");
		}
		uri = algorithmSB.sbStrToXMLCh();
	}
    else
        uri = algorithmURI;

	// Find if we can get an algorithm for this URI
	XSECAlgorithmHandler *handler;

	handler = 
		XSECPlatformUtils::g_algorithmMapper->mapURIToHandler(
			uri);

	if (handler == NULL) {
		throw XSECException(XSECException::XKMSError,
			"XKMSRecoverResult::setRSAKeyPair - unable to handle algorithm");
	}

	unsigned char kbuf[XSEC_MAX_HASH_SIZE];
	unsigned int len = CalculateXKMSKEK((unsigned char *) passPhrase, (int) strlen(passPhrase), kbuf, XSEC_MAX_HASH_SIZE);

    if (len == 0) {
		throw XSECException(XSECException::XKMSError,
			"XKMSRecoverResult::setRSAKeyPair - error deriving KEK");
    }

	XSECCryptoKey * sk = handler->createKeyForURI(
					uri,
					(XMLByte *) kbuf,
					XSEC_MAX_HASH_SIZE);

	memset(kbuf, 0, XSEC_MAX_HASH_SIZE);

	// Get some setup values
	safeBuffer str;
	DOMDocument *doc = m_msg.mp_env->getParentDocument();
	const XMLCh * prefix = m_msg.mp_env->getXKMSNSPrefix();

	makeQName(str, prefix, XKMSConstants::s_tagPrivateKey);

	// Create a PrivateKey to add this to
	DOMElement * pk = doc->createElementNS(XKMSConstants::s_unicodeStrURIXKMS, 
												str.rawXMLChBuffer());

	m_msg.mp_env->doPrettyPrint(pk);

	// Add it to the request doc
	m_msg.mp_messageAbstractTypeElement->appendChild(pk);
	m_msg.mp_env->doPrettyPrint(m_msg.mp_messageAbstractTypeElement);

	// Now create the RSA structure
	XKMSRSAKeyPairImpl * rsa;
	XSECnew(rsa, XKMSRSAKeyPairImpl(m_msg.mp_env));

	DOMElement * e = 
		rsa->createBlankXKMSRSAKeyPairImpl(Modulus, Exponent, P, Q, DP, DQ, InverseQ, D);

	// Add it to the PrivateKey
	pk->appendChild(e);
	m_msg.mp_env->doPrettyPrint(pk);

	// Encrypt all of this for future use
	XENCCipher * cipher = m_prov.newCipher(m_msg.mp_env->getParentDocument());
	cipher->setKey(sk);
	cipher->encryptElementContent(pk, ENCRYPT_NONE, uri);

	// Now load the encrypted data back in
	return cipher->loadEncryptedData(findFirstElementChild(pk));

}	

