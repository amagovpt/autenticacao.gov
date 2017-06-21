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
 * TXFMSHA1 := Class that performs a SHA1 transform
 *
 * $Id: TXFMSHA1.cpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

// XSEC

#include <xsec/transformers/TXFMSHA1.hpp>
#include <xsec/utils/XSECPlatformUtils.hpp>
#include <xsec/framework/XSECException.hpp>

XERCES_CPP_NAMESPACE_USE

TXFMSHA1::TXFMSHA1(DOMDocument *doc, hashMethod hm,
									 XSECCryptoKey * key) : TXFMBase (doc) {

	toOutput = 0;					// Nothing yet to output
	int hashLen = 0;

	switch (hm) {
	case HASH_SHA224 :
		hashLen = 224;
		break;
	case HASH_SHA256 :
		hashLen = 256;
		break;
	case HASH_SHA384 :
		hashLen = 384;
		break;
	case HASH_SHA512 :
		hashLen = 512;
		break;
	default:
		hashLen = 160;
	}

	if (key == NULL)
		// Get a SHA1 worker
		mp_h = XSECPlatformUtils::g_cryptoProvider->hashSHA(hashLen);
	else {
		// Get an HMAC Sha1
		
		mp_h = XSECPlatformUtils::g_cryptoProvider->hashHMACSHA(hashLen);
		if (!mp_h) {

			throw XSECException(XSECException::CryptoProviderError, 
					"Error requesting SHA1 object from Crypto Provider");

		}
		mp_h->setKey(key);

	}

	
	if (!mp_h) {

		throw XSECException(XSECException::CryptoProviderError, 
				"Error requesting SHA1 object from Crypto Provider");

	}
									
};

TXFMSHA1::~TXFMSHA1() {

	// Clean up
	if (mp_h)
		delete mp_h;

};

	// Methods to set the inputs

//void TXFMSHA1::setInput(TXFMBase *input);

	// Methods to get tranform output type and input requirement

TXFMBase::ioType TXFMSHA1::getInputType(void) {

	return TXFMBase::BYTE_STREAM;

}
TXFMBase::ioType TXFMSHA1::getOutputType(void) {

	return TXFMBase::BYTE_STREAM;

}


TXFMBase::nodeType TXFMSHA1::getNodeType(void) {

	return TXFMBase::DOM_NODE_NONE;

}

	// Methods to set input data

void TXFMSHA1::setInput(TXFMBase * inputT) {

	input = inputT;

	keepComments = input->getCommentsStatus();

	// Now run through the data
	unsigned char buffer[1024];
	unsigned int size;

	while ((size = input->readBytes((XMLByte *) buffer, 1024)) != 0) {
#if 0
		// Some useful debbugging code
		FILE * f = fopen("debug.out","a+b");
		fwrite(buffer, size, 1, f);
		fclose(f);
#endif
		mp_h->hash(buffer, size);
	}
	
	// Finalise

	md_len = mp_h->finish(md_value, CRYPTO_MAX_HASH_SIZE);

	toOutput = md_len;

}


unsigned int TXFMSHA1::readBytes(XMLByte * const toFill, unsigned int maxToFill) {
	
	unsigned int ret;

	if (toOutput == 0)
		return 0;

	// Check if we can just output everything left
	if (toOutput <= maxToFill) {

		memcpy((char *) toFill, &md_value[md_len - toOutput], toOutput);
		ret = toOutput;
		toOutput = 0;

		return ret;

	}

	// Output just some

	memcpy((char *) toFill, &md_value[md_len - toOutput], maxToFill);
	ret = maxToFill;
	toOutput -= maxToFill;

	return ret;

}

DOMDocument * TXFMSHA1::getDocument() {

	return NULL;

}

DOMNode * TXFMSHA1::getFragmentNode() {

	return NULL;		// Return a null node

};

const XMLCh * TXFMSHA1::getFragmentId() {

	return NULL;	// Empty string

}
