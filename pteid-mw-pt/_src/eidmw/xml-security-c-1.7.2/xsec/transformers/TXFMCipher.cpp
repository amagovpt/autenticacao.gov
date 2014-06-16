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
 * TXFMCipher := Class that performs encryption and decryption transforms
 *
 * $Id: TXFMCipher.cpp 1352681 2012-06-21 20:58:40Z scantor $
 *
 */

// XSEC

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/transformers/TXFMCipher.hpp>
#include <xsec/utils/XSECPlatformUtils.hpp>
#include <xsec/framework/XSECException.hpp>

XERCES_CPP_NAMESPACE_USE

TXFMCipher::TXFMCipher(DOMDocument *doc, 
					   XSECCryptoKey * key, 
					   bool encrypt,
                       XSECCryptoSymmetricKey::SymmetricKeyMode mode,
                       unsigned int taglen) : 
TXFMBase(doc),
m_doEncrypt(encrypt),
m_taglen(taglen),
mp_cipher(NULL),
m_remaining(0) {

    if (key && key->getKeyType() == XSECCryptoKey::KEY_SYMMETRIC)
	    mp_cipher = key->clone();
	
	if (!mp_cipher) {
		throw XSECException(XSECException::CryptoProviderError, 
				"Error cloning key, or not a symmetric key");
	}

	m_complete = false;

	try {
		if (m_doEncrypt)
			((XSECCryptoSymmetricKey *) (mp_cipher))->encryptInit((mode != XSECCryptoSymmetricKey::MODE_GCM), mode);
		else
			((XSECCryptoSymmetricKey *) (mp_cipher))->decryptInit((mode != XSECCryptoSymmetricKey::MODE_GCM), mode);
	}
	catch (...) {
		delete mp_cipher;
		mp_cipher = NULL;
		throw;
	}

};

TXFMCipher::~TXFMCipher() {

		delete mp_cipher;

};

	// Methods to set the inputs

void TXFMCipher::setInput(TXFMBase *newInput) {

	input = newInput;

	// Set up for comments
	keepComments = input->getCommentsStatus();

}

	// Methods to get tranform output type and input requirement

TXFMBase::ioType TXFMCipher::getInputType(void) {

	return TXFMBase::BYTE_STREAM;

}
TXFMBase::ioType TXFMCipher::getOutputType(void) {

	return TXFMBase::BYTE_STREAM;

}


TXFMBase::nodeType TXFMCipher::getNodeType(void) {

	return TXFMBase::DOM_NODE_NONE;

}

// Methods to get output data

unsigned int TXFMCipher::readBytes(XMLByte * const toFill, unsigned int maxToFill) {
	
	unsigned int ret, fill, leftToFill;

	ret = 0;					// How much have we copied?
	leftToFill = maxToFill;		// Still have to copy in entire thing

	while (ret != maxToFill && (m_complete == false || m_remaining > 0)) {
	
		if (m_remaining != 0) {

			// Copy anything remaining in the buffer to the output

			fill = (leftToFill > m_remaining ? m_remaining : leftToFill);
			memcpy(&toFill[ret], m_outputBuffer, fill);

			if (fill < m_remaining)
				memmove(m_outputBuffer, m_outputBuffer + fill, (m_remaining - fill));

			m_remaining -= fill;
			leftToFill -= fill;
			ret += fill;
		}

		// Now do some crypting

		if (m_complete == false && m_remaining == 0) {

			unsigned int sz = input->readBytes(m_inputBuffer, 2048);
		
			XSECCryptoSymmetricKey * symCipher = 
				(XSECCryptoSymmetricKey*) mp_cipher;
			if (m_doEncrypt) {
					
				if (sz == 0) {
					m_complete = true;
					m_remaining = symCipher->encryptFinish(m_outputBuffer, 3072, m_taglen);
				}
				else
					m_remaining = symCipher->encrypt(m_inputBuffer, m_outputBuffer, sz, 3072);
			}
			else {

				if (sz == 0) {
					m_complete = true;
					m_remaining = symCipher->decryptFinish(m_outputBuffer, 3072);
				}
				else
					m_remaining = symCipher->decrypt(m_inputBuffer, m_outputBuffer, sz, 3072);
			}
		}

	}

	return ret;

}

DOMDocument *TXFMCipher::getDocument() {

	return NULL;

}

DOMNode * TXFMCipher::getFragmentNode() {

	return NULL;		// Return a null node

}

const XMLCh * TXFMCipher::getFragmentId() {

	return NULL;	// Empty string

}
