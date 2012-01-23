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
 * DSIGAlgorithmHandlerDefault := Interface class to define handling of
 *								  default signature algorithms
 *
 * $Id: DSIGAlgorithmHandlerDefault.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef DSIGALGHANDLERDEFAULT_INCLUDE
#define DSIGALGHANDLERDEFAULT_INCLUDE

// XSEC Includes

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/framework/XSECAlgorithmHandler.hpp>

class TXFMChain;
class XENCEncryptionMethod;
class XSECCryptoKey;

// Xerces

class DSIGAlgorithmHandlerDefault : public XSECAlgorithmHandler {

public:
	
	
	virtual ~DSIGAlgorithmHandlerDefault() {};

	virtual XSECAlgorithmHandler * clone(void) const;

	// Signature ops

	virtual bool appendSignatureHashTxfm(
		TXFMChain * inputBytes,
		const XMLCh * URI,
		XSECCryptoKey * key
	);

	virtual unsigned int signToSafeBuffer(
		TXFMChain * inputBytes,
		const XMLCh * URI,
		XSECCryptoKey * key,
		unsigned int outputLength,
		safeBuffer & result
	);

	virtual bool verifyBase64Signature(
		TXFMChain * inputBytes,
		const XMLCh * URI,
		const char * sig,
		unsigned int outputLength,
		XSECCryptoKey * key
	);

	virtual bool appendHashTxfm(
		TXFMChain * inputBytes,
		const XMLCh * URI
	);

	// Unsupported Encryption Operations

	virtual unsigned int decryptToSafeBuffer(
		TXFMChain * cipherText,
		XENCEncryptionMethod * encryptionMethod,
		XSECCryptoKey * key,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument * doc,
		safeBuffer & result
	);

	virtual bool appendDecryptCipherTXFM(
		TXFMChain * cipherText,
		XENCEncryptionMethod * encryptionMethod,
		XSECCryptoKey * key,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument * doc
	);

	virtual bool encryptToSafeBuffer(
		TXFMChain * plainText,
		XENCEncryptionMethod * encryptionMethod,
		XSECCryptoKey * key,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument * doc,
		safeBuffer & result
	);

	virtual XSECCryptoKey * createKeyForURI(
		const XMLCh * uri,
		const unsigned char * keyBuffer,
		unsigned int keyLen
	);

private:

};

/*\@}*/

#endif /* DSIGALGHANDLERDEFAULT_INCLUDE */

