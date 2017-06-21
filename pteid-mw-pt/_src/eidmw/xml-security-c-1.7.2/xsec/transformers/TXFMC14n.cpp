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
 * TXFMC14n := Class that performs C14n canonicalisation
 *
 * Author(s): Berin Lautenbach
 *
 * $Id: TXFMC14n.cpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#include <xsec/transformers/TXFMC14n.hpp>
#include <xsec/framework/XSECException.hpp>
#include <xsec/transformers/TXFMParser.hpp>
#include <xsec/framework/XSECError.hpp>

XERCES_CPP_NAMESPACE_USE

TXFMC14n::TXFMC14n(DOMDocument *doc) : TXFMBase(doc) {

	mp_c14n = NULL;

}
TXFMC14n::~TXFMC14n() {

	if (mp_c14n != NULL) {
		delete mp_c14n;
	}

}

// Methods to set the inputs

void TXFMC14n::setInput(TXFMBase *newInput) {

	if (newInput->getOutputType() == TXFMBase::BYTE_STREAM) {

		//throw XSECException(XSECException::TransformInputOutputFail, "C14n canonicalisation transform requires DOM_NODES input");
		// Need to parse into DOM_NODES
		TXFMParser * parser;
		XSECnew(parser, TXFMParser(mp_expansionDoc));
		try{
			parser->setInput(newInput);
		}
		catch (...) {
			delete parser;
			input = newInput;		// Ensure chain will be deleted.
			throw;
		}

		input = parser;
	}
	else
		input = newInput;

	// Set up for comments  - by default we ALWAYS strip comments

	keepComments = false;

	TXFMBase::nodeType type = input->getNodeType();

	switch (type) {

	case TXFMBase::DOM_NODE_DOCUMENT :

		XSECnew(mp_c14n, XSECC14n20010315(input->getDocument()));
		// Expand name spaces
		//input->expandNameSpaces();

		break;

	case TXFMBase::DOM_NODE_DOCUMENT_NODE :
	case TXFMBase::DOM_NODE_DOCUMENT_FRAGMENT :

		XSECnew(mp_c14n, XSECC14n20010315(input->getDocument(), input->getFragmentNode()));
		//input->expandNameSpaces();
		break;

	case TXFMBase::DOM_NODE_XPATH_NODESET :

		XSECnew(mp_c14n, XSECC14n20010315(input->getDocument()));
		mp_c14n->setXPathMap(input->getXPathNodeList());
		break;

	default :

		throw XSECException(XSECException::XPathError);

	}

	mp_c14n->setCommentsProcessing(keepComments);			// By default we strip comments
	// Do we use the namespace map?
	mp_c14n->setUseNamespaceStack(!input->nameSpacesExpanded());

}

void TXFMC14n::activateComments(void) {

	if (input != NULL)
		keepComments = input->getCommentsStatus();
	else
		keepComments = true;

	if (mp_c14n != NULL)
		mp_c14n->setCommentsProcessing(keepComments);

}

void TXFMC14n::setExclusive() {

	if (mp_c14n != NULL)
		mp_c14n->setExclusive();

}

void TXFMC14n::setExclusive(safeBuffer & NSList) {

	if (mp_c14n != NULL)
		mp_c14n->setExclusive((char *) NSList.rawBuffer());

}

void TXFMC14n::setInclusive11() {

    if (mp_c14n != NULL)
        mp_c14n->setInclusive11();

}

// Methods to get tranform output type and input requirement

TXFMBase::ioType TXFMC14n::getInputType(void) {

	return TXFMBase::DOM_NODES;

}

TXFMBase::ioType TXFMC14n::getOutputType(void) {

	return TXFMBase::BYTE_STREAM;

}

TXFMBase::nodeType TXFMC14n::getNodeType(void) {

	return TXFMBase::DOM_NODE_NONE;

}

// Methods to get output data

unsigned int TXFMC14n::readBytes(XMLByte * const toFill, unsigned int maxToFill) {

	if (mp_c14n == NULL)

		return 0;

	return (unsigned int) mp_c14n->outputBuffer(toFill, maxToFill);

}

DOMDocument * TXFMC14n::getDocument() {

	return NULL;

}

DOMNode * TXFMC14n::getFragmentNode() {

	return NULL;

}

const XMLCh * TXFMC14n::getFragmentId() {

	return NULL;	// Empty string

}


