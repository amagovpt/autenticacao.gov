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
 * XSECCanon := Base (virtual) class for canonicalisation objects
 *
 * Author(s): Berin Lautenbach
 *
 * $ID$
 *
 * $LOG$
 *
 */

#include <xsec/canon/XSECCanon.hpp>
#include <xsec/utils/XSECDOMUtils.hpp>

#include <memory.h>

XERCES_CPP_NAMESPACE_USE

// --------------------------------------------------------------------------------
//           XSECCanon Virtual Class implementation
// --------------------------------------------------------------------------------


// Constructors

XSECCanon::XSECCanon() {};
	
XSECCanon::XSECCanon(DOMDocument *newDoc) : m_buffer() {
		
	
	mp_doc = newDoc;
	mp_startNode = mp_nextNode = newDoc;		// By default, start from startNode
	m_bufferLength = m_bufferPoint = 0; 	// Start with an empty buffer
	m_allNodesDone = false;
	
};

XSECCanon::XSECCanon(DOMDocument *newDoc, DOMNode *newStartNode) {
	
	mp_doc = newDoc;
	mp_startNode = mp_nextNode = newStartNode;
	m_bufferLength = m_bufferPoint = 0; 	// Start with an empty buffer
	m_allNodesDone = false;
	
};

// Destructors

XSECCanon::~XSECCanon() {};

// Public Methods

xsecsize_t XSECCanon::outputBuffer(unsigned char *outBuffer, xsecsize_t numBytes) {

	// numBytes of data are required to be placed in outBuffer.

	// Calculate amount left in buffer

	xsecsize_t remaining = m_bufferLength - m_bufferPoint;
	xsecsize_t bytesToGo = numBytes;
	xsecsize_t i = 0;					// current point in outBuffer


	// While we don't have enough, and have not completed - 

	while (!m_allNodesDone && (remaining < bytesToGo)) {

		// Copy what we have and get some more in the buffer
		memcpy(&outBuffer[i], &m_buffer[m_bufferPoint], remaining);
		i += remaining;
		m_bufferPoint += remaining;
		bytesToGo -= remaining;

		// Get more

		processNextNode();

		remaining = m_bufferLength - m_bufferPoint;		// This will be reset by processNextElement
													// "-bufferPoint" is just in case.

	}

	if (m_allNodesDone && (remaining < bytesToGo)) {

		// Was not enough data to fill everything up
		memcpy (&outBuffer[i], &m_buffer[m_bufferPoint], remaining);
		m_bufferPoint += remaining;
		return i + remaining;
	}
	
	// Copy the tail of the buffer

	memcpy(&outBuffer[i], &m_buffer[m_bufferPoint], bytesToGo);
	m_bufferPoint += bytesToGo;
	return (bytesToGo + i);
	
}

// setStartNode sets the starting point for the output if it is a sub-document 
// that needs canonicalisation and we want to re-start

bool XSECCanon::setStartNode(DOMNode *newStartNode) {

	mp_startNode = newStartNode;
	m_bufferPoint = 0;
	m_bufferLength = 0;
	mp_nextNode = mp_startNode;

	m_allNodesDone = false;			// Restart

	return true;			// Should check to ensure that the StartNode is part of the doc.

}


