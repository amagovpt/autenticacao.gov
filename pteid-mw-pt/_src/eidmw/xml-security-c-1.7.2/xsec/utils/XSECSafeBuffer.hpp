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
 * $Id: XSECSafeBuffer.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */


#ifndef XSECSAFEBUFFER_INCLUDE
#define XSECSAFEBUFFER_INCLUDE

#include <xsec/framework/XSECDefs.hpp>
#include <xercesc/util/XMLString.hpp>

class TXFMBase;

/**
 * \addtogroup internal
 * @{
 */


#define DEFAULT_SAFE_BUFFER_SIZE		1024		// Default size for a safe Buffer

 /**
 *\brief Manage buffers of arbitrary size
 *
 * The safeBuffer class is used internally in the library
 * to manage buffers of bytes or UTF-16 characters.
 *
 * It's a fairly innefficient class, as buffers are continually
 * being wrapped, coppied and enlarged, but given the nature of the
 * library, a single class that manipulates buffers of variable
 * size was felt to be preferable,
 *
 * The safeBuffer is not exposed through interface classes that
 * might be used by external functions.  In these cases, a
 * pointer to a XMLCh * buffer is used by preference.
 */

class CANON_EXPORT safeBuffer {

public:

	// For checking we are operating on the buffer correctly
	enum bufferType {

		BUFFER_UNKNOWN		= 0,
		BUFFER_CHAR			= 1,
		BUFFER_UNICODE		= 2
	};


	safeBuffer();
    safeBuffer(const safeBuffer & other);
	safeBuffer(xsecsize_t initialSize);
	safeBuffer(const char * inStr, xsecsize_t initialSize = DEFAULT_SAFE_BUFFER_SIZE);
	~safeBuffer();

	static void init(void);

	// "IN" functions - these read in information to the buffer

	void sbStrcpyIn(const char * inStr);
	void sbStrcpyIn(const safeBuffer & inStr);
    void sbStrncpyIn(const char * inStr, xsecsize_t n);
    void sbStrncpyIn(const safeBuffer & inStr, xsecsize_t n);

	void sbStrcatIn(const char * inStr);
	void sbStrcatIn(const safeBuffer & inStr);
    void sbStrncatIn(const char * inStr, xsecsize_t n);
    void sbStrinsIn(const char * inStr, xsecsize_t offset);

    void sbMemcpyIn(const void * inBuf, xsecsize_t n);
    void sbMemcpyIn(xsecsize_t offset, const void * inBuf, xsecsize_t n);

    void sbMemcpyOut(void * outBuf, xsecsize_t n) const;
    void sbMemshift(xsecsize_t toOffset, xsecsize_t fromOffset, xsecsize_t len);

	// Comparison functions

    int sbStrncmp(const char * inStr, xsecsize_t n) const;
    int sbOffsetStrcmp(const char * inStr, xsecsize_t offset) const;
    int sbOffsetStrncmp(const char * inStr, xsecsize_t offset, xsecsize_t n) const;
	int sbStrcmp(const char * inStr) const;
	int sbStrcmp(const safeBuffer & inStr) const;

#ifdef XSEC_XERCES_64BITSAFE
	long sbStrstr(const char * inStr) const;
	long sbOffsetStrstr(const char * inStr, xsecsize_t offset) const;
	// XMLCh versions
	long sbStrstr(const XMLCh * inStr) const;
#else
    int sbStrstr(const char * inStr) const;
    int sbOffsetStrstr(const char * inStr, xsecsize_t offset) const;
    // XMLCh versions
    int sbStrstr(const XMLCh * inStr) const;
#endif
    void sbStrinsIn(const XMLCh * inStr, xsecsize_t offset);

	// XMLCh and char common functions
	void sbStrlwr(void);		// Lowercase the string

	// Operators

    unsigned char & operator[](xsecsize_t n);
	safeBuffer & operator= (const safeBuffer & cpy);
	safeBuffer & operator= (const XMLCh * inStr);
	safeBuffer & operator << (TXFMBase * t);

	// Get functions

	xsecsize_t sbStrlen(void) const;
	xsecsize_t sbRawBufferSize(void) const;

	// raw buffer manipulation

	const unsigned char * rawBuffer() const;
	const char * rawCharBuffer() const;
	const XMLCh * rawXMLChBuffer() const;
    void resize(xsecsize_t sz);                 // NOTE : Only grows
	void setBufferType(bufferType bt);		    // Use with care

	// Unicode (UTF-16 manipulation)
	const XMLCh * sbStrToXMLCh(void) const;		// Note does not affect internal buffer
	void sbTranscodeIn(const XMLCh * inStr);	// Create a local string from UTF-16
	void sbTranscodeIn(const char * inStr);		// Create a UTF-16 string from local
	void sbXMLChIn(const XMLCh * in);			// Buffer holds XMLCh *
	void sbXMLChAppendCh(const XMLCh c);		// Append a Unicode character to the buffer
	void sbXMLChCat(const XMLCh *str);			// Append a UTF-16 string to the buffer
	void sbXMLChCat(const char * str);			// Append a (transcoded) local string to the buffer
	void sbXMLChCat8(const char * str);			// Append a (transcoded) UTF-8 string to the buffer

	// Sensitive data functions
	void isSensitive(void);
	void cleanseBuffer(void);

private:

	// Internal function that is used to get a string size and
	// then re-allocate if necessary

    void checkAndExpand(xsecsize_t size);
	void checkBufferType(bufferType bt) const;

	unsigned char * buffer;
	xsecsize_t      bufferSize;
	mutable XMLCh   * mp_XMLCh;
	bufferType		m_bufferType;

	// For XMLCh manipulation
	static size_t	size_XMLCh;

	// For sensitive data
	bool			m_isSensitive;
};

/** @} */

#endif /* XSECSAFEBUFFER_INCLUDE */

