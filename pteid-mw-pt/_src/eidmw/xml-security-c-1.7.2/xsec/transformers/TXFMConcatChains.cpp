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
 * TXFMConcatChain := Takes multiple input chains and then provides
 *					  BYTE_STREAM output for each chain in order.
 *
 * $Id: TXFMConcatChains.cpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#include <xsec/framework/XSECDefs.hpp>

#include <xsec/transformers/TXFMChain.hpp>
#include <xsec/transformers/TXFMConcatChains.hpp>
#include <xsec/framework/XSECError.hpp>

XERCES_CPP_NAMESPACE_USE

TXFMConcatChains::TXFMConcatChains(DOMDocument *doc) : 
TXFMBase(doc),
mp_currentTxfm(NULL),
m_currentChain(0),
m_complete(false) {

}

TXFMConcatChains::~TXFMConcatChains() {

	size_type i, s;
	s = m_chains.size();

	for (i = 0; i < s; ++i)
		delete m_chains[i];

	m_chains.clear();

}

void TXFMConcatChains::setInput(TXFMBase *newInput) {

	size_type i = m_chains.size();

	if (i == 0) {
		throw XSECException(XSECException::TransformInputOutputFail,
			"TXFMConcatChains::setInput - Cannot concatinate new base transform until at least one chain is added");
	}

	m_chains[i-1]->getLastTxfm()->setInput(newInput);

}

void TXFMConcatChains::setInput(TXFMChain *newInputChain) {

	m_chains.push_back(newInputChain);

}
	
TXFMBase::ioType TXFMConcatChains::getInputType(void) {

	return TXFMBase::BYTE_STREAM;

}

TXFMBase::ioType TXFMConcatChains::getOutputType(void) {

	return TXFMBase::BYTE_STREAM;

}

TXFMBase::nodeType TXFMConcatChains::getNodeType(void) {

	return TXFMBase::DOM_NODE_NONE;

}

// Methods to get output data

unsigned int TXFMConcatChains::readBytes(XMLByte * const toFill, const unsigned int maxToFill) {

	if (m_complete == true)
		return 0;

	// Is this the first one?
	if (mp_currentTxfm == NULL) {

		if (m_chains.size() == 0) {
			throw XSECException(XSECException::TransformInputOutputFail,
				"TXFMConcatChains::readBytes - Cannot read bytes until chains added");
		}

		m_currentChain = 0;
		mp_currentTxfm = m_chains[m_currentChain]->getLastTxfm();

	}

	unsigned int bytesRead = 0;
	unsigned int lastBytesRead = 0;

	while (!m_complete && bytesRead < maxToFill) {

		lastBytesRead = mp_currentTxfm->readBytes(&toFill[bytesRead], maxToFill - bytesRead);
		if (lastBytesRead == 0) {

			if (++m_currentChain == m_chains.size())
				m_complete = true;
			else
				mp_currentTxfm = m_chains[m_currentChain]->getLastTxfm();

		}
		else {
			bytesRead += lastBytesRead;
		}

	}

	return bytesRead;

}

DOMDocument *TXFMConcatChains::getDocument() {

	return NULL;

}
DOMNode *TXFMConcatChains::getFragmentNode() {

	return NULL;

}

const XMLCh * TXFMConcatChains::getFragmentId() {

	return NULL;

}
	
