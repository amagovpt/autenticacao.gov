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
 * TXFMSB := Class that takes an input safeBuffer to start a transform pipe
 *
 * $Id: TXFMSB.cpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#include <xsec/transformers/TXFMSB.hpp>

XERCES_CPP_NAMESPACE_USE

// General includes 

#include <memory.h>

TXFMSB::TXFMSB(DOMDocument *doc) : TXFMBase(doc) {

	toOutput = 0;

}


TXFMSB::~TXFMSB() {

}

	// Methods to set the inputs

void TXFMSB::setInput(TXFMBase *newInput) {

	// Not possible as this needs to be the start of a tranform pipe

	return;

}

void TXFMSB::setInput(const safeBuffer sbIn) {

	// Assume this is a string

	sb = sbIn;
	toOutput = sb.sbStrlen();
	sbs = toOutput;

}

void TXFMSB::setInput(const safeBuffer sbIn, unsigned int sbSize) {

	// Assume this is a string

	sb = sbIn;
	if (sbSize > sb.sbRawBufferSize())
		toOutput = sb.sbRawBufferSize();
	else
		toOutput = sbSize;
	sbs = toOutput;

}


	// Methods to get tranform output type and input requirement

TXFMBase::ioType TXFMSB::getInputType(void) {

	return TXFMBase::BYTE_STREAM;

}

TXFMBase::ioType TXFMSB::getOutputType(void) {

	return TXFMBase::BYTE_STREAM;

}


TXFMBase::nodeType TXFMSB::getNodeType(void) {

	return TXFMBase::DOM_NODE_NONE;

}



	// Methods to get output data

unsigned int TXFMSB::readBytes(XMLByte * const toFill, unsigned int maxToFill) {
	
	// Return from the buffer
	
	unsigned int ret;

	if (toOutput == 0)
		return 0;

	// Check if we can just output everything left
	if (toOutput <= maxToFill) {

		memcpy((char *) toFill, &(sb.rawBuffer()[sbs - toOutput]), toOutput);
		ret = (unsigned int) toOutput;
		toOutput = 0;

		return ret;

	}

	// Output just some

	memcpy((char *) toFill, &(sb.rawBuffer()[sbs - toOutput]), maxToFill);
	ret = maxToFill;
	toOutput -= maxToFill;

	return ret;
}

DOMDocument *TXFMSB::getDocument() {

	return NULL;

};

DOMNode * TXFMSB::getFragmentNode() {

	return NULL;

};

const XMLCh * TXFMSB::getFragmentId() {

	return NULL;	// Empty string

}
