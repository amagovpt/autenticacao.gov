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
 * XSECAutoPtr := internal classes for RAII handling of transcoded data
 *
 * Author(s): Scott Cantor
 *
 * $Id:$
 *
 */


#ifndef XSECAUTOPTR_INCLUDE
#define XSECAUTOPTR_INCLUDE

#include <xsec/framework/XSECDefs.hpp>
#include <xercesc/util/XMLString.hpp>

XSEC_USING_XERCES(XMLString);

/**
 * \addtogroup internal
 * @{
 */

 /**
  * A minimal auto_ptr-like class that can copy or transcode a buffer into
  * the local code page and free the result automatically.
  *
  * Needed because a standard auto_ptr would use delete on the resulting
  * pointer.
  */
class XSECAutoPtrChar
{
    XSECAutoPtrChar(const XSECAutoPtrChar&);
    XSECAutoPtrChar& operator=(const XSECAutoPtrChar&);
public:
    XSECAutoPtrChar() : m_buf(NULL) {
    }

    XSECAutoPtrChar(const XMLCh* src) : m_buf(xercesc::XMLString::transcode(src)) {
    }

    XSECAutoPtrChar(const char* src) : m_buf(xercesc::XMLString::replicate(src)) {
    }

    ~XSECAutoPtrChar() {
        XSEC_RELEASE_XMLCH(m_buf);
    }

    const char* get() const {
        return m_buf;
    }

    char* release() {
        char* temp=m_buf; m_buf=NULL; return temp;
    }

private:
    char* m_buf;
};

/**
 * A minimal auto_ptr-like class that can copy or transcode a buffer into
 * 16-bit Unicode and free the result automatically.
 *
 * Needed because a standard auto_ptr would use delete on the resulting
 * pointer.
 */
class XSECAutoPtrXMLCh
{
    XSECAutoPtrXMLCh(const XSECAutoPtrXMLCh&);
    XSECAutoPtrXMLCh& operator=(const XSECAutoPtrXMLCh&);
public:
    XSECAutoPtrXMLCh() : m_buf(NULL) {
    }

    XSECAutoPtrXMLCh(const char* src) : m_buf(xercesc::XMLString::transcode(src)) {
    }

    XSECAutoPtrXMLCh(const XMLCh* src) : m_buf(xercesc::XMLString::replicate(src)) {
    }

    ~XSECAutoPtrXMLCh() {
        XSEC_RELEASE_XMLCH(m_buf);
    }

    const XMLCh* get() const {
        return m_buf;
    }

    XMLCh* release() {
        XMLCh* temp=m_buf; m_buf=NULL; return temp;
    }

private:
    XMLCh* m_buf;
};

/** @} */

#endif /* XSECAUTOPTR_INCLUDE */

