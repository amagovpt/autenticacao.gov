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
 * XSECSafeBufferFormatter := Class for formatting DOMStrings into SafeBuffers
 *
 * Author(s): Berin Lautenbach
 *
 * $Id: XSECSafeBufferFormatter.hpp 1482601 2013-05-14 21:31:27Z scantor $
 *
 */

#ifndef XSECSAFEBUFFERFORMATTER_INCLUDE
#define XSECSAFEBUFFERFORMATTER_INCLUDE

// XSEC includes

#include <xsec/utils/XSECSafeBuffer.hpp>

// Xerces includes

#include <xercesc/framework/XMLFormatter.hpp>

/** @addtogroup internal
  * @{
  */

class sbFormatTarget : public XERCES_CPP_NAMESPACE_QUALIFIER XMLFormatTarget
{
public:

	sbFormatTarget()  {m_offset = 0;}
    ~sbFormatTarget() {}

	void setBuffer (safeBuffer * toSet) {m_buffer = toSet;};


    // -----------------------------------------------------------------------
    //  Implementations of the format target interface
	//  Based on Xerces/Xalan example code
    // -----------------------------------------------------------------------

    void writeChars(const   XMLByte* const  toWrite,
                    const xsecsize_t    count,
                    XERCES_CPP_NAMESPACE_QUALIFIER XMLFormatter * const formatter)
    {
         m_buffer->sbMemcpyIn(m_offset, (char *) toWrite, count);
		 m_buffer->setBufferType(safeBuffer::BUFFER_CHAR);
		 m_offset += count;
		(*m_buffer)[m_offset] = '\0';
    };

	void reset(void) {m_offset = 0;(*m_buffer)[0] = '\0';}

private:

    sbFormatTarget(const sbFormatTarget& other);
    void operator=(const sbFormatTarget& rhs);

	safeBuffer					* m_buffer;		// Buffer to write to
	xsecsize_t				      m_offset;
};

/**
 * \brief Formatter for outputting to a safeBuffer
 *
 * The XSECSafeBufferFormatter class is used as an internal class
 * to perform encoding translations with a safeBuffer as a target
 */

class CANON_EXPORT XSECSafeBufferFormatter {

	XERCES_CPP_NAMESPACE_QUALIFIER XMLFormatter
						* formatter;		// To actually perform the formatting
	safeBuffer			formatBuffer;		// Storage of translated strings
	sbFormatTarget		* sbf;				// Format target used by XMLFormatter

public:

	// Constructor

	XSECSafeBufferFormatter(
		const XMLCh * const						outEncoding,
		const XERCES_CPP_NAMESPACE_QUALIFIER XMLFormatter::EscapeFlags
				escapeFlags=XERCES_CPP_NAMESPACE_QUALIFIER XMLFormatter::NoEscapes,
		const XERCES_CPP_NAMESPACE_QUALIFIER XMLFormatter::UnRepFlags
				unrepFlags=XERCES_CPP_NAMESPACE_QUALIFIER XMLFormatter::UnRep_Fail
	);

	XSECSafeBufferFormatter(
		const char * const						outEncoding,
		const XERCES_CPP_NAMESPACE_QUALIFIER XMLFormatter::EscapeFlags
				escapeFlags=XERCES_CPP_NAMESPACE_QUALIFIER XMLFormatter::NoEscapes,
		const XERCES_CPP_NAMESPACE_QUALIFIER XMLFormatter::UnRepFlags
				unrepFlags=XERCES_CPP_NAMESPACE_QUALIFIER XMLFormatter::UnRep_Fail
	);

	// Destructor

	~XSECSafeBufferFormatter();

	// Reimplementation of XMLFormatter functions

	void  formatBuf (
		const XMLCh *const toFormat,
		const xsecsize_t count,
		const XERCES_CPP_NAMESPACE_QUALIFIER XMLFormatter::EscapeFlags
				escapeFlags=XERCES_CPP_NAMESPACE_QUALIFIER XMLFormatter::DefaultEscape,
		const XERCES_CPP_NAMESPACE_QUALIFIER XMLFormatter::UnRepFlags
				unrepFlags=XERCES_CPP_NAMESPACE_QUALIFIER XMLFormatter::DefaultUnRep
	);		// Format a buffer

	XSECSafeBufferFormatter&  operator<< (
		const XMLCh *const toFormat);					// Format a buffer

	XSECSafeBufferFormatter&  operator<< (
		const XMLCh toFormat);							// Format a character

	const XMLCh*  getEncodingName ()const;				// Get current encoding

	void  setEscapeFlags (const XERCES_CPP_NAMESPACE_QUALIFIER XMLFormatter::EscapeFlags newFlags);
	void  setUnRepFlags (const XERCES_CPP_NAMESPACE_QUALIFIER XMLFormatter::UnRepFlags newFlags);

	XSECSafeBufferFormatter&  operator<< (const XERCES_CPP_NAMESPACE_QUALIFIER XMLFormatter::EscapeFlags newFlags);
	XSECSafeBufferFormatter&  operator<< (const XERCES_CPP_NAMESPACE_QUALIFIER XMLFormatter::UnRepFlags newFlags);

	// Friends for working with safestrings

	friend safeBuffer& operator<< (safeBuffer &to, const XSECSafeBufferFormatter & from);

private:

	// Unimplemented

	XSECSafeBufferFormatter() {};


};

/** @} */

#endif /* XSECSAFEBUFFERFORMATTER_INCLUDE */
