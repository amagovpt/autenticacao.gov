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
 * TXFMBase := Base (virtual) class that defines a DSIG Transformer
 *
 * Author(s): Berin Lautenbach
 *
 * $Id: TXFMBase.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef TXFMBASE_INCLUDE
#define TXFMBASE_INCLUDE

#include <xercesc/dom/DOM.hpp>
#include <xsec/canon/XSECC14n20010315.hpp>
#include <xsec/utils/XSECNameSpaceExpander.hpp>
#include <xsec/utils/XSECXPathNodeList.hpp>

// Xalan

#include <xercesc/util/BinInputStream.hpp>

#include <stdlib.h>

class TXFMChain;

/** @defgroup internal Internal Classes
 * Classes marked as <b>internal</b> are used internally by the xml-security-c
 * library.  Generally there should be no requirement for these classes
 * to be manipulated directly by the calling applicataion.
 * @{
 */

/**
 * \brief Base class for transformers
 *
 * Transformers are internal classes within the xml-security-c library
 * that actually perform the transformations.
 *
 * Users interface with the library should use the DSIGTransform classes,
 * which define the DOM nodes and allow users to manipulate the actual
 * transform defined.
 */


class DSIG_EXPORT TXFMBase {

protected:

	TXFMBase				*input;			// The input source that we read from
	bool					keepComments;	// Each transform needs to tell the next whether comments are still in
	XSECNameSpaceExpander	* mp_nse;		// For expanding document name spaces
	XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument				
							* mp_expansionDoc;	// For expanding
	XSECXPathNodeList		m_XPathMap;		// For node lists if necessary

public:

	TXFMBase(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc) 
		{input = NULL; keepComments = true; mp_nse = NULL; mp_expansionDoc = doc;}
	virtual ~TXFMBase();

	// For getting/setting input/output type

	enum ioType {

		NONE			= 1,			// For when there is no Input
		BYTE_STREAM		= 2,
		DOM_NODES		= 3

	};

	enum nodeType {

		DOM_NODE_NONE				= 1,			// No nodes set
		DOM_NODE_DOCUMENT			= 2,			// This is a dom document
		DOM_NODE_DOCUMENT_FRAGMENT	= 3,			// This is a fragment only
		DOM_NODE_DOCUMENT_NODE		= 4,			// This is a fragment id;d by DOM_Node
		DOM_NODE_XPATH_NODESET		= 5 			// This is a set of nodes

	};


	// Methods to set the inputs
	// NOTE:  If this throws an exception, the implementation class
	// MUST have added the newInput to it's chain to ensure that
	// Deletion of the chain will include everything.

	virtual void setInput(TXFMBase *newInput) = 0;

	// Methods to get tranform output type and input requirement

	virtual ioType getInputType(void) = 0;
	virtual ioType getOutputType(void) = 0;
	virtual nodeType getNodeType(void) = 0;

	// Name space expansion handling
	virtual bool nameSpacesExpanded(void);
	virtual void expandNameSpaces(void);
	void deleteExpandedNameSpaces(void);

	// Comment handling

	virtual void stripComments(void) { keepComments = false;}
	virtual void activateComments(void);
	virtual bool getCommentsStatus(void) {return keepComments;}

	// Methods to get output data
	
	// BinInputStream methods:

	virtual unsigned int readBytes(XMLByte * const toFill, const unsigned int maxToFill) = 0;
	virtual XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *getDocument() = 0;
	virtual XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *getFragmentNode() = 0;
	virtual const XMLCh * getFragmentId() = 0;
	virtual XSECXPathNodeList & getXPathNodeList() {return m_XPathMap;}

	// Friends and Statics

	friend class TXFMChain;


private:

	TXFMBase();
};

/** @} */

#endif /* #define TXFMBASE_INCLUDE */
