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
 * TXFMBase64 := Class that performs a Base64 transform
 *
 * Author(s): Berin Lautenbach
 *
 * $Id: TXFMBase64.cpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

// XSEC

#include <xsec/transformers/TXFMBase64.hpp>
#include <xsec/utils/XSECPlatformUtils.hpp>
#include <xsec/framework/XSECException.hpp>

XERCES_CPP_NAMESPACE_USE

TXFMBase64::TXFMBase64(DOMDocument *doc, bool decode) : TXFMBase(doc) {

	m_complete = false;					// Nothing yet to output
	m_remaining = 0;
	m_doDecode = decode;

	mp_b64 = XSECPlatformUtils::g_cryptoProvider->base64();
	
	if (!mp_b64) {

		throw XSECException(XSECException::CryptoProviderError, 
				"Error requesting Base64 object from Crypto Provider");

	}
	if (decode)
		mp_b64->decodeInit();
	else
		mp_b64->encodeInit();

};

TXFMBase64::~TXFMBase64() {

	if (mp_b64 != NULL)
		delete mp_b64;

};

	// Methods to set the inputs

void TXFMBase64::setInput(TXFMBase *newInput) {

	input = newInput;

	// Set up for comments
	keepComments = input->getCommentsStatus();

}

	// Methods to get tranform output type and input requirement

TXFMBase::ioType TXFMBase64::getInputType(void) {

	return TXFMBase::BYTE_STREAM;

}
TXFMBase::ioType TXFMBase64::getOutputType(void) {

	return TXFMBase::BYTE_STREAM;

}


TXFMBase::nodeType TXFMBase64::getNodeType(void) {

	return TXFMBase::DOM_NODE_NONE;

}

	// Methods to get output data

unsigned int TXFMBase64::readBytes(XMLByte * const toFill, unsigned int maxToFill) {
	
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

			unsigned int sz = input->readBytes(m_inputBuffer, 1024);
		
			if (m_doDecode) {
				
				if (sz == 0) {
					m_complete = true;
					m_remaining = mp_b64->decodeFinish(m_outputBuffer, 2048);
				}
				else
					m_remaining = mp_b64->decode(m_inputBuffer, sz, m_outputBuffer, 2048);
			}
			else {

				if (sz == 0) {
					m_complete = true;
					m_remaining = mp_b64->encodeFinish(m_outputBuffer, 3072);
				}
				else
					m_remaining = mp_b64->encode(m_inputBuffer, sz, m_outputBuffer, 2048);
			}
		}

	}

	return ret;
#if 0
	unsigned int ret, fill;

	if (m_complete)
		return 0;

	fill = (maxToFill > 2000 ? 2000 : maxToFill);
	ret = 0;
	while (ret == 0 && !m_complete) {
	
		unsigned int sz = input->readBytes(m_base64Buffer, fill / 2);

		if (m_doDecode) {
			if (sz == 0)
				ret = mp_b64->decodeFinish((unsigned char *) toFill, maxToFill);
			else
				ret = mp_b64->decode(m_base64Buffer, sz, (unsigned char *) toFill, maxToFill);

			if (ret == 0)
				m_complete = true;
		}
		else {
			if (sz == 0) {
				ret = mp_b64->encodeFinish((unsigned char *) toFill, maxToFill);
				m_complete = true;
			}
			else 
				ret = mp_b64->encode(m_base64Buffer, sz, (unsigned char *) toFill, maxToFill);

		}
	}

	return ret;
#endif
}

DOMDocument *TXFMBase64::getDocument() {

	return NULL;

}

DOMNode * TXFMBase64::getFragmentNode() {

	return NULL;		// Return a null node

}

const XMLCh * TXFMBase64::getFragmentId() {

	return NULL;	// Empty string

}
