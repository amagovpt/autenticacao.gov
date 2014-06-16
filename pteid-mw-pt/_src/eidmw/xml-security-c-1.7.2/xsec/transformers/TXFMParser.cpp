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
 * Author(s): Berin Lautenbach
 *
 * $Id: TXFMParser.cpp 1493962 2013-06-17 22:32:41Z scantor $
 *
 */

#include <xsec/transformers/TXFMParser.hpp>
#include <xsec/transformers/TXFMChain.hpp>
#include <xsec/utils/XSECPlatformUtils.hpp>
#include <xsec/framework/XSECError.hpp>
#include <xsec/utils/XSECTXFMInputSource.hpp>

#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/util/Janitor.hpp>

XERCES_CPP_NAMESPACE_USE

TXFMParser::TXFMParser(DOMDocument * doc) : 
TXFMBase(doc),
mp_parsedDoc(NULL) {


};

TXFMParser::~TXFMParser() {

	if (mp_parsedDoc != NULL) {

		if (mp_nse != NULL) {
			delete mp_nse;	// Don't bother collapsing
			mp_nse = NULL;
		}

		mp_parsedDoc->release();

	}

	mp_parsedDoc = NULL;


};

// -----------------------------------------------------------------------
//  For expanding name spaces when necessary
// -----------------------------------------------------------------------

bool TXFMParser::nameSpacesExpanded(void) {

	// NOTE : Do not check inputs as this has its own document

	return (mp_nse != NULL);

}

void TXFMParser::expandNameSpaces(void) {

	if (mp_nse != NULL)
		return;		// Already done
	
	if (mp_parsedDoc != NULL) {

		XSECnew(mp_nse, XSECNameSpaceExpander(mp_parsedDoc));

		mp_nse->expandNameSpaces();

	}

}

// -----------------------------------------------------------------------
//  Worker function
// -----------------------------------------------------------------------

void TXFMParser::setInput(TXFMBase *newInput) {

	// This transformer terminates all previous inputs and deletes
	// the chain.

	input = newInput;

	// Create a InputStream
	TXFMChain * chain;
	XSECnew(chain, TXFMChain(newInput, false));
	Janitor<TXFMChain> j_chain(chain);

	XSECTXFMInputSource is(chain, false);

	// Create a XercesParser and parse!
	XercesDOMParser parser;

	parser.setDoNamespaces(true);
	parser.setLoadExternalDTD(false);

	SecurityManager securityManager;
	securityManager.setEntityExpansionLimit(XSEC_ENTITY_EXPANSION_LIMIT);
	parser.setSecurityManager(&securityManager);

	parser.parse(is);
    xsecsize_t errorCount = parser.getErrorCount();
    if (errorCount > 0)
		throw XSECException(XSECException::XSLError, "Errors occured parsing BYTE STREAM");

    mp_parsedDoc = parser.adoptDocument();

	// Clean up

	keepComments = newInput->getCommentsStatus();

}

	// Methods to get tranform output type and input requirement

TXFMBase::ioType TXFMParser::getInputType(void) {

	return TXFMBase::BYTE_STREAM;

}
TXFMBase::ioType TXFMParser::getOutputType(void) {

	return TXFMBase::DOM_NODES;

}


TXFMBase::nodeType TXFMParser::getNodeType(void) {

	return TXFMBase::DOM_NODE_DOCUMENT;

}

	// Methods to get output data

unsigned int TXFMParser::readBytes(XMLByte * const toFill, unsigned int maxToFill) {
	
	return 0;

}

DOMDocument *TXFMParser::getDocument() {

	return mp_parsedDoc;

}

DOMNode * TXFMParser::getFragmentNode() {

	return NULL;		// Return a null node

}

const XMLCh * TXFMParser::getFragmentId() {

	return NULL;	// Empty string

}
