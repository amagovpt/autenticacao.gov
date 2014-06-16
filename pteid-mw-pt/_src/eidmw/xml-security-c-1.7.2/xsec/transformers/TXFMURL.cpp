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
 * $Id: TXFMURL.cpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#include <xsec/transformers/TXFMURL.hpp>
#include <xsec/framework/XSECError.hpp>

// To catch exceptions

#include <xercesc/util/XMLNetAccessor.hpp>

XERCES_CPP_NAMESPACE_USE

// General includes 

#include <stdlib.h>

TXFMURL::TXFMURL(DOMDocument *doc, XSECURIResolver * resolver) : TXFMBase(doc) {

	is = NULL;			// To ensure later able to delete if not used properly

	mp_resolver = resolver;

	done = false;
}


TXFMURL::~TXFMURL() {

	if (is != NULL)
		delete is;
}

	// Methods to set the inputs

void TXFMURL::setInput(TXFMBase *newInput) {

	// Not possible as this needs to be the start of a tranform pipe

	return;

}

void TXFMURL::setInput(const XMLCh * URL) {

	// Assume we have already checked that this is a valid URL


	if (mp_resolver != NULL)
		is = mp_resolver->resolveURI(URL);

	if (is == NULL) {

		throw XSECException(XSECException::ErrorOpeningURI,
			"An error occurred in TXFMURL when resolving URL");

	}

}

void TXFMURL::setInput(BinInputStream * inputStream) {

	if (is != NULL)
		delete is;

	is = inputStream;

}

	// Methods to get tranform output type and input requirement

TXFMBase::ioType TXFMURL::getInputType(void) {

	return TXFMBase::BYTE_STREAM;

}

TXFMBase::ioType TXFMURL::getOutputType(void) {

	return TXFMBase::BYTE_STREAM;

}

TXFMBase::nodeType TXFMURL::getNodeType(void) {

	return TXFMBase::DOM_NODE_NONE;

}


	// Methods to get output data

unsigned int TXFMURL::readBytes(XMLByte * const toFill, unsigned int maxToFill) {
	
	XSEC_USING_XERCES(NetAccessorException);

	// Simply read in from the input stream

	unsigned int ret;

	if (done || is == NULL) return 0;

	try {
		ret = (unsigned int) is->readBytes(toFill, maxToFill);
	}
	catch (NetAccessorException e) {

		// Naieve - but assume this means the socket has closed (end of stream)
		ret = 0;

	}

	//if (ret < maxToFill)
	if (ret <= 0)
		done = true;

	return ret;

}

DOMDocument *TXFMURL::getDocument() {

	return NULL;

};

DOMNode *TXFMURL::getFragmentNode() {

	return NULL;

};

const XMLCh * TXFMURL::getFragmentId() {

	return NULL;	// Empty string

}
