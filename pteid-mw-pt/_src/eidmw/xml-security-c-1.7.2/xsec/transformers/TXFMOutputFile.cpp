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
 * TXFMOutputFile := Transform that outputs the byte stream to a file without changing
 *							  the actual data in any way
 *
 * $Id: TXFMOutputFile.cpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#include <xsec/transformers/TXFMOutputFile.hpp>
#include <xsec/framework/XSECException.hpp>

XERCES_CPP_NAMESPACE_USE

// Destructor

TXFMOutputFile::~TXFMOutputFile() {

    if (f.is_open())
        f.write("\n----- END -----\n", 17);
	f.close();

}

// Methods to set the inputs

void TXFMOutputFile::setInput(TXFMBase *newInput) {

	input = newInput;

	if (newInput->getOutputType() != TXFMBase::BYTE_STREAM) {

		throw XSECException(XSECException::TransformInputOutputFail, "OutputFile transform requires BYTE_STREAM input");

	}

	keepComments = input->getCommentsStatus();

}

bool TXFMOutputFile::setFile(char * const fileName) {

	// Open a file for outputting

	using std::ios;

    f.open(fileName, ios::binary|ios::out|ios::app);

	if (f.is_open()) {
        f.write("\n----- BEGIN -----\n", 19);
		return true;
    }

	return false;

}



// Methods to get tranform output type and input requirement

TXFMBase::ioType TXFMOutputFile::getInputType(void) {

	return TXFMBase::BYTE_STREAM;

}

TXFMBase::ioType TXFMOutputFile::getOutputType(void) {

	return TXFMBase::BYTE_STREAM;

}


TXFMBase::nodeType TXFMOutputFile::getNodeType(void) {

	return TXFMBase::DOM_NODE_NONE;

}


// Methods to get output data

unsigned int TXFMOutputFile::readBytes(XMLByte * const toFill, unsigned int maxToFill) {

	unsigned int sz;

	sz = input->readBytes(toFill, maxToFill);

	if (f.is_open())
		f.write((char *) toFill, sz);

	return sz;

}

DOMDocument * TXFMOutputFile::getDocument() {

	return NULL;

}

DOMNode * TXFMOutputFile::getFragmentNode() {

	return NULL;

};

const XMLCh * TXFMOutputFile::getFragmentId() {

	return NULL;	// Empty string

}
