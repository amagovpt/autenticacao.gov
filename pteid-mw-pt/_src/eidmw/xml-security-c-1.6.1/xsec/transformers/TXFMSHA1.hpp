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
 * $Id: TXFMSHA1.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

// XSEC Includes

#include <xsec/transformers/TXFMBase.hpp>
#include <xsec/enc/XSECCryptoProvider.hpp>

/**
 * \brief Transformer to handle create a SHA-1 hash from a chain
 * @ingroup internal
 */

class DSIG_EXPORT TXFMSHA1 : public TXFMBase {

private:

	XSECCryptoHash		* mp_h;							// To hold the hash
	unsigned char		md_value[CRYPTO_MAX_HASH_SIZE];	// Final output
	unsigned int		md_len;							// Length of digest

	unsigned int		toOutput;						// Amount still to output

public:

	TXFMSHA1(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc, hashMethod hm = HASH_SHA1, XSECCryptoKey * key = NULL);
	~TXFMSHA1();

	// Methods to get tranform output type and input requirement

	virtual TXFMBase::ioType getInputType(void);
	virtual TXFMBase::ioType getOutputType(void);
	virtual nodeType getNodeType(void);

	// Methods to set input data

	virtual void setInput(TXFMBase * inputT);

	// Methods to get output data

	virtual unsigned int readBytes(XMLByte * const toFill, const unsigned int maxToFill);
	virtual XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *getDocument();
	virtual XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *getFragmentNode();
	virtual const XMLCh * getFragmentId();
	
private:
	TXFMSHA1();
};


