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
 * TXFMXSL := Class that performs XML Stylesheet Language transforms
 *
 * $Id: TXFMXSL.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#include <xsec/transformers/TXFMBase.hpp>

// Xerces

#include <xercesc/dom/DOM.hpp>

// Xalan

#ifndef XSEC_NO_XPATH

#include <xalanc/XalanDOM/XalanDocument.hpp>
#include <xalanc/XercesParserLiaison/XercesDOMSupport.hpp>
#include <xalanc/XercesParserLiaison/XercesParserLiaison.hpp>
#include <xalanc/XPath/NodeRefList.hpp>
#include <xalanc/XPath/ElementPrefixResolverProxy.hpp>
#include <xalanc/XalanTransformer/XalanTransformer.hpp>

// Xalan Namespace usage
XALAN_USING_XALAN(XercesDOMSupport)
XALAN_USING_XALAN(XercesParserLiaison)
XALAN_USING_XALAN(XalanDocument)
XALAN_USING_XALAN(XalanTransformer)

#endif

#ifndef XSEC_NO_XSLT

/**
 * \brief Transformer to handle XSLT transforms
 * @ingroup internal
 */

class DSIG_EXPORT TXFMXSL : public TXFMBase {

private:

	safeBuffer expr;							// The expression being worked with

	XercesDOMSupport	xds;
	XercesParserLiaison xpl;

	XalanDocument		* xd;

	safeBuffer			sbInDoc;

	XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument		
						* document;
	
	XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument		
						* docOut;			// The output from the transformation

public:

	TXFMXSL(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc);
	~TXFMXSL();

	// Methods to set the inputs

	void setInput(TXFMBase *newInput);
	
	// Methods to get tranform output type and input requirement

	virtual TXFMBase::ioType getInputType(void);
	virtual TXFMBase::ioType getOutputType(void);
	virtual nodeType getNodeType(void);

	// We do our own name spaces - we have a new document!
	
	virtual bool nameSpacesExpanded(void);
	virtual void expandNameSpaces(void);


	// XSL Unique

	void evaluateStyleSheet(const safeBuffer &sbStyleSheet);

	// Methods to get output data

	virtual unsigned int readBytes(XMLByte * const toFill, const unsigned int maxToFill);
	virtual XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *getDocument();
	virtual XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *getFragmentNode();
	virtual const XMLCh * getFragmentId();
	
private:
	TXFMXSL();

};


#endif /* No XSLT */
