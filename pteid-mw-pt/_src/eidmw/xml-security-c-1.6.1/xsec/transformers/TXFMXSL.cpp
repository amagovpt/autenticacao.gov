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
 * TXFMXSL := Class that performs XPath transforms
 *
 * $Id: TXFMXSL.cpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#include <xsec/transformers/TXFMXSL.hpp>
#include <xsec/dsig/DSIGConstants.hpp>
#include <xsec/framework/XSECError.hpp>

#ifndef XSEC_NO_XSLT

// Xerces
#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/util/Janitor.hpp>

XERCES_CPP_NAMESPACE_USE

#include <iostream>
#include <strstream>
#include <fstream>

XALAN_USING_XALAN(XSLTResultTarget)

// Function used to output data to a safeBuffer
extern "C" {

typedef struct TransformXSLOutputHolderStruct {

	safeBuffer	buffer;
	int			offset;

} TransformXSLOutputHolder;

CallbackSizeType TransformXSLOutputFn(const char * s, CallbackSizeType sz, void * data) {

	TransformXSLOutputHolder * output = (TransformXSLOutputHolder *) data;

	output->buffer.sbMemcpyIn(output->offset, s, (int) sz);
	output->offset += (int) sz;
	output->buffer[output->offset] = '\0';

	return sz;

}
}

// -----------------------------------------------------------------------
//  For expanding name spaces when necessary
// -----------------------------------------------------------------------

bool TXFMXSL::nameSpacesExpanded(void) {

	// NOTE : Do not check inputs as this has its own document

	return (mp_nse != NULL);

}

void TXFMXSL::expandNameSpaces(void) {

	if (mp_nse != NULL)
		return;		// Already done
	
	if (docOut != NULL) {

		XSECnew(mp_nse, XSECNameSpaceExpander(docOut));

		mp_nse->expandNameSpaces();

	}

}

// -----------------------------------------------------------------------
//  Transform functions
// -----------------------------------------------------------------------


TXFMXSL::TXFMXSL(DOMDocument *doc) : 
	TXFMBase(doc),
#if XALAN_VERSION_MAJOR == 1 && XALAN_VERSION_MINOR > 10
xds(xpl)
#else
#if defined XSEC_XERCESPARSERLIAISON_REQS_DOMSUPPORT
xpl(xds) 
#else
xpl()
#endif
#endif
{

	// Zeroise all the pointers

	xd = NULL;

}

TXFMXSL::~TXFMXSL() {

	if (docOut != NULL) {
		if (mp_nse != NULL) {
			delete mp_nse;	// Don't bother collapsing
			mp_nse = NULL;
		}
		docOut->release();	
	}
}

// Methods to set the inputs

void TXFMXSL::setInput(TXFMBase *newInput) {

	input = newInput;

	if (newInput->getOutputType() != TXFMBase::BYTE_STREAM) {

		throw XSECException(XSECException::TransformInputOutputFail, "XSL requires DOM_NODES input type");

	}

	// Should have a method to check if the input is a straight URL - if it is, just read the
	// URL name and create an XSLTInputSource with this as the input ID.

	int size = 0;
	int count = 0;
	unsigned char buf[512];

	while ((count = input->readBytes((XMLByte *) buf, 512)) != 0) {

		sbInDoc.sbMemcpyIn(size, buf, count);
		size += count;

	}

	sbInDoc[size] = '\0';

}

void TXFMXSL::evaluateStyleSheet(const safeBuffer &sbStyleSheet) {

	// Set up iostreams for input
	std::istrstream	theXMLStream((char *) sbInDoc.rawBuffer(), (int) strlen((char *) sbInDoc.rawBuffer()));
	std::istrstream	theXSLStream((char *) sbStyleSheet.rawBuffer(), (int) strlen((char *) sbStyleSheet.rawBuffer()));

	// Now resolve

	XalanTransformer xt;
	TransformXSLOutputHolder txoh;
	txoh.buffer.sbStrcpyIn("");
	txoh.offset = 0;
	
	/*int res = */
	xt.transform(&theXMLStream, &theXSLStream, (void *) & txoh, TransformXSLOutputFn);

	// Should check res

	// Now use xerces to "re parse" this back into a DOM_Nodes document
	XercesDOMParser * parser = new XercesDOMParser;
	Janitor<XercesDOMParser> j_parser(parser);

	parser->setDoNamespaces(true);
	parser->setCreateEntityReferenceNodes(true);
	parser->setDoSchema(true);

	// Create an input source

	MemBufInputSource* memIS = new MemBufInputSource ((const XMLByte*) txoh.buffer.rawBuffer(), txoh.offset, "XSECMem");
	Janitor<MemBufInputSource> j_memIS(memIS);

	int errorCount = 0;

	parser->parse(*memIS);
    errorCount = parser->getErrorCount();
    if (errorCount > 0)
		throw XSECException(XSECException::XSLError, "Errors occured when XSL result was parsed back to DOM_Nodes");

    docOut = parser->adoptDocument();

	// Janitors clean up

}

// Methods to get tranform output type and input requirement

TXFMBase::ioType TXFMXSL::getInputType(void) {

	return TXFMBase::DOM_NODES;

}
TXFMBase::ioType TXFMXSL::getOutputType(void) {

	return TXFMBase::DOM_NODES;

}

TXFMBase::nodeType TXFMXSL::getNodeType(void) {

	return TXFMBase::DOM_NODE_DOCUMENT;

}

// Methods to get output data

unsigned int TXFMXSL::readBytes(XMLByte * const toFill, unsigned int maxToFill) {

	return 0;

}

DOMDocument * TXFMXSL::getDocument() {

	return docOut;

}

DOMNode * TXFMXSL::getFragmentNode() {

	return NULL;

}

const XMLCh * TXFMXSL::getFragmentId() {

	return NULL;	// Empty string

}

#endif /* NO_XSLT */
