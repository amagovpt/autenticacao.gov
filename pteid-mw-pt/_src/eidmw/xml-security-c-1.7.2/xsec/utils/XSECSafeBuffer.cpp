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
 * XSECSafeBuffer := a class for storing expanding amounts of information.
 *
 * Author(s): Berin Lautenbach
 *
 * $Id: XSECSafeBuffer.cpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

// XSEC includes

#include <xsec/utils/XSECSafeBuffer.hpp>
#include <xsec/utils/XSECDOMUtils.hpp>
#include <xsec/framework/XSECError.hpp>
#include <xsec/transformers/TXFMBase.hpp>

#include <xercesc/util/XMLUniDefs.hpp>

XSEC_USING_XERCES(XMLString);

// Standard includes

#include <stdlib.h>
#include <string.h>

size_t safeBuffer::size_XMLCh;

#if defined (_MSC_VER)
#pragma warning(disable: 4311)
#endif

void safeBuffer::checkAndExpand(xsecsize_t size) {

	// For a given size, check it will fit (with one byte spare)
	// and expand if necessary

	if (size + 1 < bufferSize)
		return;

	// Make the new size twice the size of the new string requirement
	xsecsize_t newBufferSize = size * 2;

	unsigned char * newBuffer = new unsigned char[newBufferSize];
	memset((void *) newBuffer, 0, newBufferSize);
	memcpy(newBuffer, buffer, bufferSize);

	// If we are sensitive, clean the old buffer
	if (m_isSensitive == true)
		cleanseBuffer();

	// clean up
	bufferSize = newBufferSize;
	delete[] buffer;
	buffer = newBuffer;

}

void safeBuffer::checkBufferType(bufferType bt) const {

	if (bt != m_bufferType) {

		throw XSECException(XSECException::SafeBufferError,
			"Attempt to perform an operation on a buffer of incorrect type");
	}

}


void safeBuffer::setBufferType(bufferType bt) {

	m_bufferType = bt;

}

void safeBuffer::resize(xsecsize_t sz) {

	checkAndExpand(sz);

}

safeBuffer::safeBuffer(xsecsize_t initialSize) {

	// Initialise the buffer with a set size string

	bufferSize = initialSize;
	buffer = new unsigned char[initialSize];
	memset((void *) buffer, 0, bufferSize);
	mp_XMLCh = NULL;
	m_isSensitive = false;

}

safeBuffer::safeBuffer() {

	bufferSize = DEFAULT_SAFE_BUFFER_SIZE;
	buffer = new unsigned char[bufferSize];
	memset((void *) buffer, 0, bufferSize);
	mp_XMLCh = NULL;
	m_bufferType = BUFFER_UNKNOWN;
	m_isSensitive = false;

}

safeBuffer::safeBuffer(const char * inStr, xsecsize_t initialSize) {
    // Initialise with a string
    bufferSize = ((xsecsize_t) strlen(inStr) > initialSize ? (xsecsize_t) (strlen(inStr) * 2) : initialSize);

	buffer = new unsigned char[bufferSize];
	memset((void *) buffer, 0, bufferSize);
	strcpy((char *) buffer, inStr);
	mp_XMLCh = NULL;
	m_bufferType = BUFFER_CHAR;
	m_isSensitive = false;

}

safeBuffer::safeBuffer(const safeBuffer & other) {

	// Copy constructor

	bufferSize = other.bufferSize;
	buffer = new unsigned char [bufferSize];

	memcpy(buffer, other.buffer, bufferSize);

	if (other.mp_XMLCh != NULL) {

		mp_XMLCh = XMLString::replicate(other.mp_XMLCh);

	}
	else {

		mp_XMLCh = NULL;

	}

	m_bufferType = other.m_bufferType;
	m_isSensitive = other.m_isSensitive;

}

safeBuffer::~safeBuffer() {


	if (buffer != NULL) {
		if (m_isSensitive == true)
			cleanseBuffer();
		delete[] buffer;
	}

	if (mp_XMLCh != NULL)
		XSEC_RELEASE_XMLCH(mp_XMLCh);

}

void safeBuffer::init (void) {

	size_XMLCh = sizeof(XMLCh);

}

// "IN" functions - these read in information to the buffer

void safeBuffer::sbStrcpyIn(const char * inStr) {

	// Copy a string into the safe buffer
    checkAndExpand((xsecsize_t) strlen(inStr));
	strcpy((char *) buffer, inStr);
	m_bufferType = BUFFER_CHAR;

}

void safeBuffer::sbStrcpyIn(const safeBuffer & inStr) {

	inStr.checkBufferType(BUFFER_CHAR);
    checkAndExpand((xsecsize_t) strlen((char *) inStr.buffer));
	strcpy((char *) buffer, (char *) inStr.buffer);
	m_bufferType = BUFFER_CHAR;

}


void safeBuffer::sbStrncpyIn(const char * inStr, xsecsize_t n) {
    xsecsize_t len = (xsecsize_t) strlen(inStr);
	checkAndExpand((n < len) ? n : len);
	strncpy((char *) buffer, inStr, n);
	m_bufferType = BUFFER_CHAR;

}

void safeBuffer::sbStrncpyIn(const safeBuffer & inStr, xsecsize_t n) {

	inStr.checkBufferType(BUFFER_CHAR);
	checkAndExpand(n);
	strncpy((char *) buffer, (char *) inStr.buffer, n);
	buffer[n] = '\0';
	m_bufferType = BUFFER_CHAR;


}


void safeBuffer::sbStrcatIn(const char * inStr) {

	checkBufferType(BUFFER_CHAR);
    checkAndExpand((xsecsize_t) (strlen((char *) buffer) + strlen(inStr)));
	strcat((char *) buffer, inStr);

}

void safeBuffer::sbStrcatIn(const safeBuffer & inStr) {

	checkBufferType(BUFFER_CHAR);
    checkAndExpand((xsecsize_t) (strlen((char *) buffer) + strlen((char *) inStr.buffer) + 2));
	strcat((char *) buffer, (char *) inStr.buffer);

}

void safeBuffer::sbStrncatIn(const char * inStr, xsecsize_t n) {
    checkBufferType(BUFFER_CHAR);
    xsecsize_t len = (xsecsize_t) strlen(inStr);
    checkAndExpand(((n < len) ? n : len) + (xsecsize_t) strlen((char *) buffer) + 2);
	strncat((char *) buffer, inStr, n);

}

void safeBuffer::sbMemcpyIn(const void * inBuf, xsecsize_t n) {

	checkAndExpand(n);
	memcpy(buffer, inBuf, n);
	m_bufferType = BUFFER_UNKNOWN;

}

void safeBuffer::sbMemcpyIn(xsecsize_t offset, const void * inBuf, xsecsize_t n) {

	checkAndExpand(n + offset);
	memcpy(&buffer[offset], inBuf, n);
	m_bufferType = BUFFER_UNKNOWN;
}

void safeBuffer::sbStrinsIn(const char * inStr, xsecsize_t offset) {

    checkBufferType(BUFFER_CHAR);

    xsecsize_t bl = (xsecsize_t) strlen((char *) buffer);
    xsecsize_t il = (xsecsize_t) strlen((char *) inStr);

	if (offset > bl) {
		throw XSECException(XSECException::SafeBufferError,
			"Attempt to insert string after termination point");
	}

	checkAndExpand(bl + il);

	memmove(&buffer[offset + il], &buffer[offset], bl - offset + 1);
	memcpy(&buffer[offset], inStr, il);

}

void safeBuffer::sbStrinsIn(const XMLCh * inStr, xsecsize_t offset) {

    checkBufferType(BUFFER_UNICODE);

    xsecsize_t bl = XMLString::stringLen((XMLCh *) buffer) * size_XMLCh;
    xsecsize_t il = XMLString::stringLen((XMLCh *) inStr) * size_XMLCh;

    xsecsize_t xoffset = offset * size_XMLCh;
	if (xoffset > bl) {
		throw XSECException(XSECException::SafeBufferError,
			"Attempt to insert string after termination point");
	}

	checkAndExpand(bl + il);

	memmove(&buffer[xoffset + il], &buffer[xoffset], bl - xoffset + size_XMLCh);
	memcpy(&buffer[xoffset], inStr, il);

}


void safeBuffer::sbMemcpyOut(void *outBuf, xsecsize_t n) const {

	// WARNING - JUST ASSUMES OUTPUT BUFFER LONG ENOUGH
	// ALSO MAKES NO ASSUMPTION OF THE BUFFER TYPE

	memcpy(outBuf, buffer, n);

}

void safeBuffer::sbMemshift(xsecsize_t toOffset, xsecsize_t fromOffset, xsecsize_t len) {

	// Move data in the buffer around
	checkAndExpand(len + (toOffset > fromOffset ? toOffset : fromOffset));

	memmove(&buffer[toOffset], &buffer[fromOffset], len);

}


// Comparisons

int safeBuffer::sbStrncmp(const char *inStr, xsecsize_t n) const {

	checkBufferType(BUFFER_CHAR);
	return (strncmp((char *) buffer, inStr, n));

}

int safeBuffer::sbStrcmp(const char *inStr) const {

	checkBufferType(BUFFER_CHAR);
	return (strcmp((char *) buffer, inStr));

}

int safeBuffer::sbStrcmp(const safeBuffer & inStr) const {

	checkBufferType(BUFFER_CHAR);
	return (strcmp((char *) buffer, (char *) inStr.buffer));

}

int safeBuffer::sbOffsetStrcmp(const char * inStr, xsecsize_t offset) const {

    checkBufferType(BUFFER_CHAR);
    xsecsize_t bl = (xsecsize_t) strlen((char *) buffer);

	if (offset > bl)
		return -1;

	return (strcmp((char *) &buffer[offset], inStr));

}

int safeBuffer::sbOffsetStrncmp(const char * inStr, xsecsize_t offset, xsecsize_t n) const {

    checkBufferType(BUFFER_CHAR);
    xsecsize_t bl = (xsecsize_t) strlen((char *) buffer);
	if (offset > bl)
		return -1;

	return (strncmp((char *) &buffer[offset], inStr, n));

}

#ifdef XSEC_XERCES_64BITSAFE
long safeBuffer::sbStrstr(const char * inStr) const {
#else
int safeBuffer::sbStrstr(const char * inStr) const {
#endif

	checkBufferType(BUFFER_CHAR);
	char* p = strstr((char *) buffer, inStr);

	if (p == NULL)
		return -1;

	long d = (unsigned long) p - (unsigned long) buffer;
	if (d < 0 || d > bufferSize)
		return -1;

	return d;

}

#ifdef XSEC_XERCES_64BITSAFE
long safeBuffer::sbStrstr(const XMLCh * inStr) const {
#else
int safeBuffer::sbStrstr(const XMLCh * inStr) const {
#endif

	checkBufferType(BUFFER_UNICODE);
	XMLCh* p = XMLString::findAny((XMLCh *) buffer, inStr);

	if (p == NULL)
		return -1;

	long d = ((unsigned long) ((p - (unsigned long) buffer)) / size_XMLCh);
	if (d < 0 || d > bufferSize)
		return -1;

	return d;

}

#ifdef XSEC_XERCES_64BITSAFE
long safeBuffer::sbOffsetStrstr(const char * inStr, xsecsize_t offset) const {
#else
int safeBuffer::sbOffsetStrstr(const char * inStr, xsecsize_t offset) const {
#endif

	checkBufferType(BUFFER_CHAR);
	xsecsize_t bl = (xsecsize_t) strlen((char *) buffer);

	if (offset > bl)
		return -1;

	char* p = strstr((char *) &buffer[offset], inStr);

	if (p == NULL)
		return -1;

	long d = (unsigned long) p - (unsigned long) buffer;
	if (d < 0 || d > bufferSize)
		return -1;

	return d;

}

// XMLCh and char common functions

void safeBuffer::sbStrlwr(void) {

	if (m_bufferType == BUFFER_UNKNOWN) {

		throw XSECException(XSECException::SafeBufferError,
			"Attempt to perform an operation on a buffer of incorrect type");

	}

	if (m_bufferType == BUFFER_CHAR) {

	    xsecsize_t i, l = (xsecsize_t) strlen((char *) buffer);

		for (i = 0; i < l; ++i) {
			if (buffer[i] >= 'A' && buffer[i] <= 'Z')
				buffer[i] = (buffer[i] - 'A') + 'a';
		}

	}

	else {

		XMLCh * b = (XMLCh *) buffer;
		xsecsize_t i, l = XMLString::stringLen(b);

		for (i = 0; i < l; ++i) {
			if (b[i] >= XERCES_CPP_NAMESPACE_QUALIFIER chLatin_A && b[i] <= XERCES_CPP_NAMESPACE_QUALIFIER chLatin_Z)
				b[i] = (b[i] - XERCES_CPP_NAMESPACE_QUALIFIER chLatin_A) + XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a;
		}

	}

}
// Operators

unsigned char & safeBuffer::operator[](xsecsize_t n) {

	// If the character is outside our range (but +ve), then simply increase
	// the buffer size - NOTE: it is not our problem if the caller does
	// not realise they are outside the buffer, we are simply trying to ensure
	// the call is "safe"

	checkAndExpand(n);

	return buffer[n];

}

safeBuffer & safeBuffer::operator= (const safeBuffer & cpy) {

	if (bufferSize != cpy.bufferSize) {

		if (bufferSize != 0) {

			if (m_isSensitive == true)
				cleanseBuffer();

			delete [] buffer;
		}

		buffer = new unsigned char [cpy.bufferSize];
		bufferSize = cpy.bufferSize;

	}

	memcpy(buffer, cpy.buffer, bufferSize);
	m_bufferType = cpy.m_bufferType;
	// Once we are sensitive, we are always sensitive
	m_isSensitive = m_isSensitive || cpy.m_isSensitive;

	return *this;
}

safeBuffer & safeBuffer::operator= (const XMLCh * inStr) {

	checkAndExpand(XMLString::stringLen(inStr) * size_XMLCh);
	XMLString::copyString((XMLCh *) buffer, inStr);
	m_bufferType = BUFFER_UNICODE;
	return *this;

}

safeBuffer & safeBuffer::operator << (TXFMBase * t) {

	// Read into buffer the output of the transform
	xsecsize_t offset = 0;
	unsigned char inBuf[2048];
	xsecsize_t bytesRead;

	while ((bytesRead = t->readBytes(inBuf, 2000)) > 0) {

		checkAndExpand(offset + bytesRead + 1);
		memcpy(&buffer[offset], inBuf, bytesRead);
		offset += bytesRead;

	}

	m_bufferType = BUFFER_CHAR;
	buffer[offset] = '\0';

	return *this;
}


// Unicode Functions

const XMLCh * safeBuffer::sbStrToXMLCh(void) const {

	checkBufferType(BUFFER_CHAR);
	if (mp_XMLCh != NULL)
		XSEC_RELEASE_XMLCH(mp_XMLCh);

	mp_XMLCh = XMLString::transcode((char *) buffer);

	return mp_XMLCh;

}

void safeBuffer::sbTranscodeIn(const XMLCh * inStr) {

	// Transcode the string to the local code page and store in the buffer
	char * t;

	t = XMLString::transcode(inStr);

	assert (t != 0);


	// Now copy into our local buffer - a bit inefficient but better in the long run
	// as a buffer that is the exact size is likely to be deleted anyway during a
	// concat operation

	xsecsize_t len = (xsecsize_t) strlen(t) + 1;
	checkAndExpand(len);
	strcpy((char *) buffer, t);
	m_bufferType = BUFFER_CHAR;

	XSEC_RELEASE_XMLCH(t);

}

void safeBuffer::sbTranscodeIn(const char * inStr) {

	// Transcode the string to the local code page and store in the buffer
	XMLCh * t;

	t = XMLString::transcode(inStr);

	assert (t != 0);

	// Copy into local buffer

	xsecsize_t len = XMLString::stringLen(t) + 1;
    len *= (xsecsize_t) size_XMLCh;
	checkAndExpand(len);

	XMLString::copyString((XMLCh *) buffer, t);
	m_bufferType = BUFFER_UNICODE;

	XSEC_RELEASE_XMLCH(t);

}


void safeBuffer::sbXMLChIn(const XMLCh * in) {

	checkAndExpand((XMLString::stringLen(in) + 1) * size_XMLCh);

	XMLString::copyString((XMLCh *) buffer, in);
	m_bufferType = BUFFER_UNICODE;

}

void safeBuffer::sbXMLChAppendCh(const XMLCh c) {

	checkBufferType(BUFFER_UNICODE);
	xsecsize_t len = XMLString::stringLen((XMLCh *) buffer);

	checkAndExpand((len + 2) * size_XMLCh);

	((XMLCh *) buffer)[len++] = c;
	((XMLCh *) buffer)[len] = 0;

}

void safeBuffer::sbXMLChCat(const XMLCh *str) {

	checkBufferType(BUFFER_UNICODE);
	xsecsize_t len = XMLString::stringLen((XMLCh *) buffer) * size_XMLCh;
	len += XMLString::stringLen(str) * size_XMLCh;
	len += (2 * ((xsecsize_t) size_XMLCh));

	checkAndExpand(len);

	XMLString::catString((XMLCh *) buffer, str);

}

void safeBuffer::sbXMLChCat(const char * str) {

	checkBufferType(BUFFER_UNICODE);
	xsecsize_t len = XMLString::stringLen((XMLCh *) buffer) * size_XMLCh;

	XMLCh * t = XMLString::transcode(str);

	assert (t != NULL);

	len += XMLString::stringLen(t);
	len += (xsecsize_t) (2 * size_XMLCh);

	checkAndExpand(len);

	XMLString::catString((XMLCh *) buffer, t);

	XSEC_RELEASE_XMLCH(t);
}

void safeBuffer::sbXMLChCat8(const char * str) {

	checkBufferType(BUFFER_UNICODE);

	XMLCh * toAdd = transcodeFromUTF8((const unsigned char *) str);
	sbXMLChCat(toAdd);
	XSEC_RELEASE_XMLCH(toAdd);

}

// Get functions

xsecsize_t safeBuffer::sbStrlen(void) const {

    checkBufferType(BUFFER_CHAR);
    return (xsecsize_t) (strlen ((char *) buffer));

}

xsecsize_t safeBuffer::sbRawBufferSize(void) const {

    return bufferSize;

}


// raw buffer manipulation

const unsigned char * safeBuffer::rawBuffer() const {

	return buffer;

}

const char * safeBuffer::rawCharBuffer() const {

	return (char *) buffer;

}

const XMLCh * safeBuffer::rawXMLChBuffer() const {

	return (XMLCh *) buffer;

}

// Sensitive data functions

void safeBuffer::isSensitive(void) {

	m_isSensitive = true;

}

void safeBuffer::cleanseBuffer(void) {

	// Cleanse the main buffer
	for (xsecsize_t i = 0; i < bufferSize; ++i)
		buffer[i] = 0;

}
