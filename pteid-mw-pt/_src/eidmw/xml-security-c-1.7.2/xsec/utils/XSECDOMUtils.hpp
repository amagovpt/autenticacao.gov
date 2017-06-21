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
 * XSECDOMUtils:= Utilities to manipulate DOM within XML-SECURITY
 *
 * Author(s): Berin Lautenbach
 *
 * $Id: XSECDOMUtils.hpp 1350044 2012-06-13 22:31:37Z scantor $
 *
 */

#ifndef XSECDOMUTILS_HEADER
#define XSECDOMUTILS_HEADER

// XSEC

#include <xsec/utils/XSECPlatformUtils.hpp>
#include <xsec/utils/XSECSafeBuffer.hpp>
#include <xsec/dsig/DSIGConstants.hpp>

// Xerces

#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/XMLString.hpp>

#define COMPARE_STRING_LENGTH	256

// --------------------------------------------------------------------------------
//           "Quick" Transcode Class (low performance)
// --------------------------------------------------------------------------------


class DSIG_EXPORT XMLT {

public:

	XMLT(const char * str);
	~XMLT(void);

	XMLCh * getUnicodeStr(void);

private:

	XMLCh * mp_unicodeStr;
	XMLT();

};

#define MAKE_UNICODE_STRING(str) XMLT(str).getUnicodeStr()

// --------------------------------------------------------------------------------
//           Utilities to manipulate namespaces
// --------------------------------------------------------------------------------

const XMLCh DSIG_EXPORT * getDSIGLocalName(const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node);
const XMLCh DSIG_EXPORT * getDSIG11LocalName(const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node);
const XMLCh DSIG_EXPORT * getECLocalName(const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node);
const XMLCh DSIG_EXPORT * getXPFLocalName(const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node);
const XMLCh DSIG_EXPORT * getXENCLocalName(const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node);
const XMLCh DSIG_EXPORT * getXENC11LocalName(const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node);
const XMLCh DSIG_EXPORT * getXKMSLocalName(const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node);

// --------------------------------------------------------------------------------
//           Do UTF-8 <-> UTF-16 transcoding
// --------------------------------------------------------------------------------

XMLCh DSIG_EXPORT * transcodeFromUTF8(const unsigned char * src);
char DSIG_EXPORT * transcodeToUTF8(const XMLCh * src);

// --------------------------------------------------------------------------------
//           Find a nominated DSIG/XENC node in a document
// --------------------------------------------------------------------------------

XERCES_CPP_NAMESPACE_QUALIFIER DOMNode DSIG_EXPORT * findDSIGNode(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *n, const char * nodeName);
XERCES_CPP_NAMESPACE_QUALIFIER DOMNode DSIG_EXPORT * findXENCNode(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *n, const char * nodeName);

// --------------------------------------------------------------------------------
//           Find particular type of node child
// --------------------------------------------------------------------------------

XERCES_CPP_NAMESPACE_QUALIFIER DOMNode DSIG_EXPORT * findFirstChildOfType(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *n, XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::NodeType t);
XERCES_CPP_NAMESPACE_QUALIFIER DOMNode DSIG_EXPORT * findNextChildOfType(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *n, XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::NodeType t);

XERCES_CPP_NAMESPACE_QUALIFIER DOMElement DSIG_EXPORT * findFirstElementChild(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *n);
XERCES_CPP_NAMESPACE_QUALIFIER DOMElement DSIG_EXPORT * findNextElementChild(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *n);

// --------------------------------------------------------------------------------
//           Make a QName
// --------------------------------------------------------------------------------

safeBuffer DSIG_EXPORT &makeQName(safeBuffer & qname, safeBuffer &prefix, const char * localName);
safeBuffer DSIG_EXPORT &makeQName(safeBuffer & qname, const XMLCh *prefix, const char * localName);
safeBuffer DSIG_EXPORT &makeQName(safeBuffer & qname, const XMLCh *prefix, const XMLCh * localName);

// --------------------------------------------------------------------------------
//           Gather text from children
// --------------------------------------------------------------------------------

void DSIG_EXPORT gatherChildrenText(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode * parent, safeBuffer &output);

// --------------------------------------------------------------------------------
//           String decode/encode
// --------------------------------------------------------------------------------

/*
 * Distinguished names have a particular encoding that needs to be performed prior
 * to enclusion in the DOM
 */

XMLCh * encodeDName(const XMLCh * toEncode);
XMLCh * decodeDName(const XMLCh * toDecode);

// --------------------------------------------------------------------------------
//           Generate Ids
// --------------------------------------------------------------------------------

XMLCh DSIG_EXPORT * generateId(unsigned int bytes = 16);

// --------------------------------------------------------------------------------
//           String Functions 
// --------------------------------------------------------------------------------

// Remove escape (%XX) characters from URIs and return a new buffer containing the
// "cleaned" string

XMLCh * cleanURIEscapes(const XMLCh * str);

inline
bool strEquals (const XMLCh * str1, const XMLCh *str2) {

	return (XMLString::compareString(str1, str2) == 0);

}

inline
bool strEquals (const char * str1, const char *str2) {

	return (XMLString::compareString(str1, str2) == 0);

}

inline 
bool strEquals (const char * str1, const XMLCh * str2) {

	bool ret;
	XMLCh * str1XMLCh = XMLString::transcode(str1);

	if (str1XMLCh != NULL) {

		ret = (XMLString::compareString(str1XMLCh, str2) == 0);
		XSEC_RELEASE_XMLCH(str1XMLCh);

	}
	else
		ret = false;

	return ret;

}

inline 
bool strEquals (const XMLCh * str1, const char * str2) {

	bool ret;
	XMLCh * str2XMLCh = XMLString::transcode(str2);

	if (str2XMLCh != NULL) {

		ret = (XMLString::compareString(str1, str2XMLCh) == 0);
		XSEC_RELEASE_XMLCH(str2XMLCh);

	}
	else
		ret = false;

	return ret;

}

#endif /* XSECDOMUTILS_HEADER */

