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
 * TXFMDocObject := Class that takes an input Document object ID to start a txfm pipe
 *
 * Author(s): Berin Lautenbach
 *
 * $Id: TXFMDocObject.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef TXFMDOCOBJECT_INCLUDE
#define TXFMDOCOBJECT_INCLUDE

#include <xsec/transformers/TXFMBase.hpp>
#include <xsec/utils/XSECSafeBuffer.hpp>
#include <xsec/framework/XSECEnv.hpp>

// Xerces

#include <xercesc/dom/DOM.hpp>

/**
 * \brief Transformer that defines a document start to a transform chain
 * @ingroup internal
 */


class DSIG_EXPORT TXFMDocObject : public TXFMBase {

public:

	TXFMDocObject(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc);
	~TXFMDocObject();

	// Methods to set the inputs

	void setInput(TXFMBase *newInput);
	void setInput(
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc, 
		const XMLCh * newFragmentId
	);
	void setInput(
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc, 
		XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *newFragmentObject
	);
	void setInput(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc);

	// Environment - when searching for IDs we need to know about the user
	// environment (namely - how to handle ID attribute names)

	void setEnv(const XSECEnv * env);

	// Methods to get tranform output type and input requirement

	TXFMBase::ioType getInputType(void);
	TXFMBase::ioType getOutputType(void);
	TXFMBase::nodeType getNodeType(void);

	// Methods to get output data

	unsigned int readBytes(XMLByte * const toFill, const unsigned int maxToFill);
	virtual XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument * getDocument();
	virtual XERCES_CPP_NAMESPACE_QUALIFIER DOMNode * getFragmentNode();
	virtual const XMLCh * getFragmentId();
	
private:

	XMLCh	* fragmentId;				// The identifier of the object
	XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument 
			* document;					// The DOM document we are working with
	XERCES_CPP_NAMESPACE_QUALIFIER DOMNode 
			* fragmentObject;			// The object that contains the doc fragment to use
	TXFMBase::nodeType type;			// The type of nodes this holds

	const XSECEnv * mp_env;

	
	TXFMDocObject();
};

#endif
