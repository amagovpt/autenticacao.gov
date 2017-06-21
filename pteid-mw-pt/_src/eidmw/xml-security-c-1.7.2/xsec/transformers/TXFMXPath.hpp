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
 * TXFMXPath := Class that performs XPath transforms
 *
 * $Id: TXFMXPath.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef TXFMXPATH_INCLUDE
#define TXFMXPATH_INCLUDE

#include <xsec/transformers/TXFMBase.hpp>
#include <xsec/utils/XSECSafeBuffer.hpp>
#include <xsec/canon/XSECC14n20010315.hpp>
#include <xsec/dsig/DSIGXPathHere.hpp>
#include <xsec/utils/XSECSafeBufferFormatter.hpp>

// Xerces

XSEC_DECLARE_XERCES_CLASS(DOMNode);
XSEC_DECLARE_XERCES_CLASS(DOMNamedNodeMap);

// Xalan

#ifndef XSEC_NO_XALAN

#include <xalanc/XalanDOM/XalanDocument.hpp>
#include <xalanc/XercesParserLiaison/XercesDocumentWrapper.hpp>
#include <xalanc/XercesParserLiaison/XercesDOMSupport.hpp>
#include <xalanc/XercesParserLiaison/XercesParserLiaison.hpp>
#include <xalanc/XPath/XPathEvaluator.hpp>
#include <xalanc/XPath/XPathProcessorImpl.hpp>
#include <xalanc/XPath/XPathFactoryDefault.hpp>
#include <xalanc/XPath/NodeRefList.hpp>
#include <xalanc/XPath/XPathEnvSupportDefault.hpp>
#include <xalanc/XPath/XPathConstructionContextDefault.hpp>
#include <xalanc/XPath/ElementPrefixResolverProxy.hpp>

#endif

#ifndef XSEC_NO_XPATH

/**
 * \brief Transformer to handle XPath transforms
 * @ingroup internal
 */


class DSIG_EXPORT TXFMXPath : public TXFMBase {

private:

	safeBuffer expr;							// The expression being worked with

	XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument			* document;
	XERCES_CPP_NAMESPACE_QUALIFIER DOMNamedNodeMap		* XPathAtts;	// Attributes that came through from the doc

	static	bool		XPathInitDone;

	DSIGXPathHere		* here;			// The function to implement here()
	XSECSafeBufferFormatter * formatter;

public:

	TXFMXPath(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc);
	~TXFMXPath();

	// Methods to set the inputs

	void setInput(TXFMBase *newInput);
	
	// Methods to get tranform output type and input requirement

	virtual TXFMBase::ioType getInputType(void);
	virtual TXFMBase::ioType getOutputType(void);
	virtual TXFMBase::nodeType getNodeType(void);

	// XPath unique

	void setNameSpace(XERCES_CPP_NAMESPACE_QUALIFIER DOMNamedNodeMap *xpAtts);
	void evaluateExpr(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *h, safeBuffer inexpr);
	void evaluateEnvelope(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *t);

	// Methods to get output data

	virtual unsigned int readBytes(XMLByte * const toFill, const unsigned int maxToFill);
	virtual XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *getDocument();
	virtual XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *getFragmentNode();
	virtual const XMLCh * getFragmentId();
	virtual XSECXPathNodeList	& getXPathNodeList();
private:
	TXFMXPath();
};

#endif

#endif
