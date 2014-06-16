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
 * TXFMURL := Class that takes an input URL to start of a transform pipe
 *
 * $Id: TXFMURL.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#include <xsec/transformers/TXFMBase.hpp>
#include <xsec/framework/XSECURIResolver.hpp>

// Xerces

#include <xercesc/util/BinInputStream.hpp>

/**
 * \brief Base transformer for URL inputs to chains.  Also used to
 * provide a method to provide a BinInputStream as an input method
 *
 * @ingroup internal
 */

class DSIG_EXPORT TXFMURL : public TXFMBase {

private:

	XSECURIResolver			* mp_resolver;	// Resolver passed in
	XERCES_CPP_NAMESPACE_QUALIFIER BinInputStream			
							* is;		// To handle the actual input

	bool					done;


public:

	TXFMURL(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc, XSECURIResolver * resolver);
	~TXFMURL();

	// Methods to set the inputs

	virtual void setInput(TXFMBase *newInput);
	void setInput(const XMLCh * URL);
	void setInput(XERCES_CPP_NAMESPACE_QUALIFIER BinInputStream * inputStream);

	// Methods to get tranform output type and input requirement

	virtual TXFMBase::ioType getInputType(void);
	virtual TXFMBase::ioType getOutputType(void);
	virtual nodeType getNodeType(void);

	// Methods to get output data

	virtual unsigned int readBytes(XMLByte * const toFill, const unsigned int maxToFill);
	virtual XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *getDocument();
	virtual XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *getFragmentNode();
	virtual const XMLCh * getFragmentId();
	
private :
	TXFMURL();
};

