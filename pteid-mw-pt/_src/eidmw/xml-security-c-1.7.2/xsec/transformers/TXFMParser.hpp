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
 * TXFMParser := A transformer used to transform a byte stream to DOM Nodes
 *
 * $Id: TXFMParser.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef TXFMPARSER_INCLUDE
#define TXFMPARSER_INCLUDE

#include <xsec/transformers/TXFMBase.hpp>

/**
 * \brief Transformer to handle move between bytes and DOM
 * @ingroup internal
 *
 * THere is no "parser" transformation defined in DSIG, however there
 * is a requirement to be able to move between a byte stream and
 * a list of nodes.  For example, if a ref URI points to an external
 * document, and the first transformation on that document is an 
 * XPath expression, then the library uses this class to translate the
 * BYTE stream to a DOM document.
 */


class DSIG_EXPORT TXFMParser : public TXFMBase {

public:

	// Constructors and destructors

	TXFMParser(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *);
	~TXFMParser();

	// Methods to get tranform output type and input requirement

	virtual TXFMBase::ioType getInputType(void);
	virtual TXFMBase::ioType getOutputType(void);
	virtual nodeType getNodeType(void);

	// Methods to set input data

	virtual void setInput(TXFMBase * newInput);

	// Methods to get output data

	virtual unsigned int readBytes(XMLByte * const toFill, const unsigned int maxToFill);
	virtual XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *getDocument();
	virtual XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *getFragmentNode();
	virtual const XMLCh * getFragmentId();

	// Name space management
	virtual bool nameSpacesExpanded(void);
	virtual void expandNameSpaces(void);

	
private:
	
	TXFMParser();

	XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument	* mp_parsedDoc;

};

#endif /* #define TXFMPARSER_INCLUDE */
