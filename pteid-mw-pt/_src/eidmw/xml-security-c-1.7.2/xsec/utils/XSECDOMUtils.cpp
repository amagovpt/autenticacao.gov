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
 * $Id: XSECDOMUtils.cpp 1350044 2012-06-13 22:31:37Z scantor $
 *
 */

// XSEC

#include <xsec/utils/XSECDOMUtils.hpp>
#include <xsec/framework/XSECError.hpp>
#include <xsec/xkms/XKMSConstants.hpp>

// Xerces

#include <xercesc/util/XMLUniDefs.hpp>
#include <xercesc/util/Janitor.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/TransService.hpp>

#include <string.h>

XERCES_CPP_NAMESPACE_USE

// --------------------------------------------------------------------------------
//           Utilities to manipulate DSIG namespaces
// --------------------------------------------------------------------------------

const XMLCh * getDSIGLocalName(const DOMNode *node) {

	if (!strEquals(node->getNamespaceURI(), DSIGConstants::s_unicodeStrURIDSIG))
		return NULL; //DOMString("");
	else
		return node->getLocalName();

}

const XMLCh * getDSIG11LocalName(const DOMNode *node) {

	if (!strEquals(node->getNamespaceURI(), DSIGConstants::s_unicodeStrURIDSIG11))
		return NULL; //DOMString("");
	else
		return node->getLocalName();

}

const XMLCh * getECLocalName(const DOMNode * node) {

	// Exclusive Canonicalisation namespace
	// Probably should have a generic function

	if (!strEquals(node->getNamespaceURI(), DSIGConstants::s_unicodeStrURIEC))
		return NULL;
	else
		return node->getLocalName();

}

const XMLCh * getXPFLocalName(const DOMNode * node) {

	// XPath Filter namespace

	if (!strEquals(node->getNamespaceURI(), DSIGConstants::s_unicodeStrURIXPF))
		return NULL;
	else
		return node->getLocalName();

}

const XMLCh * getXENCLocalName(const DOMNode *node) {

	// XML Encryption namespace node

	if (!strEquals(node->getNamespaceURI(), DSIGConstants::s_unicodeStrURIXENC))
		return NULL;
	else
		return node->getLocalName();

}

const XMLCh * getXENC11LocalName(const DOMNode *node) {

	// XML Encryption 1.1 namespace node

	if (!strEquals(node->getNamespaceURI(), DSIGConstants::s_unicodeStrURIXENC11))
		return NULL;
	else
		return node->getLocalName();

}

const XMLCh * getXKMSLocalName(const DOMNode *node) {

	// XKMS namespace node

	if (!strEquals(node->getNamespaceURI(), XKMSConstants::s_unicodeStrURIXKMS))
		return NULL;
	else
		return node->getLocalName();

}


// --------------------------------------------------------------------------------
//           Find a nominated DSIG node in a document
// --------------------------------------------------------------------------------

DOMNode *findDSIGNode(DOMNode *n, const char * nodeName) {

	const XMLCh * name = getDSIGLocalName(n);

	if (strEquals(name, nodeName)) {

		return n;

	}

	DOMNode *child = n->getFirstChild();

	while (child != NULL) {

		DOMNode *ret = findDSIGNode(child, nodeName);
		if (ret != NULL)
			return ret;
		child = child->getNextSibling();

	}

	return child;

}

// --------------------------------------------------------------------------------
//           Find a nominated XENC node in a document
// --------------------------------------------------------------------------------

DOMNode *findXENCNode(DOMNode *n, const char * nodeName) {

	const XMLCh * name = getXENCLocalName(n);

	if (strEquals(name, nodeName)) {

		return n;

	}

	DOMNode *child = n->getFirstChild();

	while (child != NULL) {

		DOMNode *ret = findXENCNode(child, nodeName);
		if (ret != NULL)
			return ret;
		child = child->getNextSibling();

	}

	return child;

}

// --------------------------------------------------------------------------------
//           Find particular type of node child
// --------------------------------------------------------------------------------

DOMNode *findFirstChildOfType(DOMNode *n, DOMNode::NodeType t) {

	DOMNode *c;

	if (n == NULL)
		return n;

	c = n->getFirstChild();

	while (c != NULL && c->getNodeType() != t)
		c = c->getNextSibling();

	return c;

}

DOMNode * findNextChildOfType(DOMNode *n, DOMNode::NodeType t) {

	DOMNode * s = n;

	if (s == NULL)
		return s;

	do {
		s = s->getNextSibling();
	} while (s != NULL && s->getNodeType() != t);

	return s;

}

DOMElement *findFirstElementChild(DOMNode *n) {

	DOMNode *c;

	if (n == NULL)
		return NULL;

	c = n->getFirstChild();

	while (c != NULL && c->getNodeType() != DOMNode::ELEMENT_NODE)
		c = c->getNextSibling();

	return (DOMElement *) c;

}

DOMElement * findNextElementChild(DOMNode *n) {

	DOMNode * s = n;

	if (s == NULL)
		return NULL;

	do {
		s = s->getNextSibling();
	} while (s != NULL && s->getNodeType() != DOMNode::ELEMENT_NODE);

	return (DOMElement *) s;

}

// --------------------------------------------------------------------------------
//           Make a QName
// --------------------------------------------------------------------------------

safeBuffer &makeQName(safeBuffer & qname, safeBuffer &prefix, const char * localName) {

	if (prefix[0] == '\0') {
		qname = localName;
	}
	else {
		qname = prefix;
		qname.sbStrcatIn(":");
		qname.sbStrcatIn(localName);
	}

	return qname;

}
safeBuffer &makeQName(safeBuffer & qname, const XMLCh *prefix, const char * localName) {

	if (prefix == NULL || prefix[0] == 0) {
		qname.sbTranscodeIn(localName);
	}
	else {
		qname.sbXMLChIn(prefix);
		qname.sbXMLChAppendCh(XERCES_CPP_NAMESPACE_QUALIFIER chColon);
		qname.sbXMLChCat(localName);	// Will transcode
	}

	return qname;
}

safeBuffer &makeQName(safeBuffer & qname, const XMLCh *prefix, const XMLCh * localName) {

	if (prefix == NULL || prefix[0] == 0) {
		qname.sbXMLChIn(localName);
	}
	else {
		qname.sbXMLChIn(prefix);
		qname.sbXMLChAppendCh(XERCES_CPP_NAMESPACE_QUALIFIER chColon);
		qname.sbXMLChCat(localName);
	}

	return qname;
}

// --------------------------------------------------------------------------------
//           "Quick" Transcode (low performance)
// --------------------------------------------------------------------------------



XMLT::XMLT(const char * str) {

	mp_unicodeStr = XMLString::transcode(str);

}

XMLT::~XMLT (void) {

	XSEC_RELEASE_XMLCH(mp_unicodeStr);

}

XMLCh * XMLT::getUnicodeStr(void) {

	return mp_unicodeStr;

}

// --------------------------------------------------------------------------------
//           Gather text from children
// --------------------------------------------------------------------------------

void gatherChildrenText(DOMNode * parent, safeBuffer &output) {

	DOMNode * c = parent->getFirstChild();

	output.sbXMLChIn(DSIGConstants::s_unicodeStrEmpty);

	while (c != NULL) {

		if (c->getNodeType() == DOMNode::TEXT_NODE)
			output.sbXMLChCat(c->getNodeValue());

		c = c->getNextSibling();

	}

}

// --------------------------------------------------------------------------------
//           Some UTF8 utilities
// --------------------------------------------------------------------------------

XMLCh * transcodeFromUTF8(const unsigned char * src) {

	// Take a UTF-8 buffer and transcode to UTF-16

	safeBuffer fullDest;
	fullDest.sbXMLChIn(DSIGConstants::s_unicodeStrEmpty);
	XMLCh outputBuf[2050];

	// Used to record byte sizes
	unsigned char charSizes[2050];

	// Grab a transcoder
	XMLTransService::Codes failReason;

#if defined(XSEC_XERCES_REQUIRES_MEMMGR)
	XMLTranscoder* t =
		XMLPlatformUtils::fgTransService->makeNewTranscoderFor("UTF-8",
															   failReason,
															   2*1024,
															   XMLPlatformUtils::fgMemoryManager);
#else
	XMLTranscoder* t =
		XMLPlatformUtils::fgTransService->makeNewTranscoderFor("UTF-8",
															   failReason,
															   2*1024);
#endif
	Janitor<XMLTranscoder> j_t(t);

	// Need to loop through, 2K at a time
	xsecsize_t bytesEaten, bytesEatenCounter;
    xsecsize_t charactersEaten;
	xsecsize_t totalBytesEaten = 0;
	xsecsize_t bytesToEat = XMLString::stringLen((char *) src);

	while (totalBytesEaten < bytesToEat) {

	    xsecsize_t toEat = bytesToEat - totalBytesEaten;


		if (toEat > 2048)
			toEat = 2048;

		t->transcodeFrom(&src[totalBytesEaten],
						toEat,
						outputBuf,
						2048,
						bytesEaten,
						charSizes);

		// Determine how many characters were used
		bytesEatenCounter = 0;
		charactersEaten = 0;
		while (bytesEatenCounter < bytesEaten) {
			bytesEatenCounter += charSizes[charactersEaten++];
		}

		outputBuf[charactersEaten] = chNull;
		fullDest.sbXMLChCat(outputBuf);
		totalBytesEaten += bytesEaten;
	}

	// Dup and output
	return XMLString::replicate(fullDest.rawXMLChBuffer());

}

char DSIG_EXPORT * transcodeToUTF8(const XMLCh * src) {

	// Take a UTF-16 buffer and transcode to UTF-8

	safeBuffer fullDest("");
	unsigned char outputBuf[2050];

	// Grab a transcoder
	XMLTransService::Codes failReason;

#if defined(XSEC_XERCES_REQUIRES_MEMMGR)
	XMLTranscoder* t =
		XMLPlatformUtils::fgTransService->makeNewTranscoderFor("UTF-8",
															   failReason,
															   2*1024,
															   XMLPlatformUtils::fgMemoryManager);
#else
	XMLTranscoder* t =
		XMLPlatformUtils::fgTransService->makeNewTranscoderFor("UTF-8",
															   failReason,
															   2*1024);
#endif
	Janitor<XMLTranscoder> j_t(t);

	// Need to loop through, 2K at a time
	xsecsize_t charactersEaten, charactersOutput;
	xsecsize_t totalCharsEaten = 0;
	xsecsize_t charsToEat = XMLString::stringLen(src);

	while (totalCharsEaten < charsToEat) {

	    xsecsize_t toEat = charsToEat - totalCharsEaten;

		if (toEat > 2048)
			toEat = 2048;

		charactersOutput = t->transcodeTo(&src[totalCharsEaten],
						toEat,
						(unsigned char * const) outputBuf,
						2048,
						charactersEaten,
						XMLTranscoder::UnRep_RepChar);

		outputBuf[charactersOutput] = '\0';
		fullDest.sbStrcatIn((char *) outputBuf);
		totalCharsEaten += charactersEaten;
	}

	// Dup and output
	return XMLString::replicate(fullDest.rawCharBuffer());

}

// --------------------------------------------------------------------------------
//           String decode/encode
// --------------------------------------------------------------------------------

/*
 * Distinguished names have a particular encoding that needs to be performed prior
 * to enclusion in the DOM
 */

XMLCh * encodeDName(const XMLCh * toEncode) {

	XERCES_CPP_NAMESPACE_USE;

	safeBuffer result;

	static XMLCh s_strEncodedSpace[] = {
		chBackSlash,
		chDigit_2,
		chDigit_0,
		chNull
	};

	result.sbXMLChIn(DSIGConstants::s_unicodeStrEmpty);

	if (toEncode == NULL) {
		return NULL;
	}


	// Find where the trailing whitespace starts
	const XMLCh * ws = &toEncode[XMLString::stringLen(toEncode)];

	*ws--;
	while (ws != toEncode &&
		(*ws == '\t' || *ws == '\r' || *ws ==' ' || *ws == '\n'))
		*ws--;

	// Set to first white space character, if we didn't get back to the start
	if (toEncode != ws)
		*ws++;

	// Now run through each character and encode if necessary

	const XMLCh * i = toEncode;

	if (*i == chPound) {
		// "#" Characters escaped at the start of a string
		result.sbXMLChAppendCh(chBackSlash);
	}

	while (*i != chNull && i != ws) {

		if (*i <= 0x09) {
			result.sbXMLChAppendCh(chBackSlash);
			result.sbXMLChAppendCh(chDigit_0);
			result.sbXMLChAppendCh(chDigit_0 + *i);
		}
		else if (*i <= 0x0f) {
			result.sbXMLChAppendCh(chBackSlash);
			result.sbXMLChAppendCh(chDigit_0);
			result.sbXMLChAppendCh(chLatin_A + *i);
		}
		else if (*i <= 0x19) {
			result.sbXMLChAppendCh(chBackSlash);
			result.sbXMLChAppendCh(chDigit_1);
			result.sbXMLChAppendCh(chDigit_0 + *i);
		}
		else if (*i <= 0x1f) {
			result.sbXMLChAppendCh(chBackSlash);
			result.sbXMLChAppendCh(chDigit_1);
			result.sbXMLChAppendCh(chLatin_A + *i);
		}

		else if (*i == chComma) {

			// Determine if this is an RDN separator
			const XMLCh *j = i;
			*j++;
			while (*j != chComma && *j != chEqual && *j != chNull)
				*j++;

			if (*j != chEqual)
				result.sbXMLChAppendCh(chBackSlash);

			result.sbXMLChAppendCh(*i);

		}

		else {

			if (*i == chPlus ||
				*i == chDoubleQuote ||
				*i == chBackSlash ||
				*i == chOpenAngle ||
				*i == chCloseAngle ||
				*i == chSemiColon) {

				result.sbXMLChAppendCh(chBackSlash);
			}

			result.sbXMLChAppendCh(*i);

		}

		*i++;

	}

	// Now encode trailing white space
	while (*i != chNull) {

		if (*i == ' ')
			result.sbXMLChCat(s_strEncodedSpace);
		else
			result.sbXMLChAppendCh(*i);

		*i++;

	}

	return XMLString::replicate(result.rawXMLChBuffer());

}

XMLCh * decodeDName(const XMLCh * toDecode) {

	// Take an encoded name and decode to a normal XMLCh string

	XERCES_CPP_NAMESPACE_USE;

	safeBuffer result;

	result.sbXMLChIn(DSIGConstants::s_unicodeStrEmpty);

	if (toDecode == NULL) {
		return NULL;
	}

	const XMLCh * i = toDecode;

	if (*i == chBackSlash && i[1] == chPound) {

		result.sbXMLChAppendCh(chPound);
		*i++;
		*i++;

	}

	while (*i != chNull) {

		if (*i == chBackSlash) {

			*i++;

			if (*i == chDigit_0) {

				*i++;

				if (*i >= chDigit_0 && *i <= chDigit_9) {
					result.sbXMLChAppendCh(*i - chDigit_0);
				}
				else if (*i >= chLatin_A && *i <= chLatin_F) {
					result.sbXMLChAppendCh(10 + *i - chLatin_A);
				}
				else if (*i >= chLatin_a && *i <= chLatin_f) {
					result.sbXMLChAppendCh(10 + *i - chLatin_a);
				}
				else {
					throw XSECException(XSECException::DNameDecodeError,
						"Unexpected escaped character in Distinguished name");
				}
			}

			else if (*i == chDigit_1) {

				*i++;

				if (*i >= chDigit_0 && *i <= chDigit_9) {
					result.sbXMLChAppendCh(16 + *i - chDigit_0);
				}
				else if (*i >= chLatin_A && *i <= chLatin_F) {
					result.sbXMLChAppendCh(26 + *i - chLatin_A);
				}
				else if (*i >= chLatin_a && *i <= chLatin_f) {
					result.sbXMLChAppendCh(26 + *i - chLatin_a);
				}
				else {
					throw XSECException(XSECException::DNameDecodeError,
						"Unexpected escaped character in Distinguished name");
				}
			}

			else if (*i == chDigit_2) {

				*i++;

				if (*i == '0') {
					result.sbXMLChAppendCh(' ');
				}

				else {
					throw XSECException(XSECException::DNameDecodeError,
						"Unexpected escaped character in Distinguished name");
				}

			}

			else if (*i == chComma ||
					 *i == chPlus ||
					 *i == chDoubleQuote ||
					 *i == chBackSlash ||
					 *i == chOpenAngle ||
					 *i == chCloseAngle ||
					 *i == chSemiColon) {

				result.sbXMLChAppendCh(*i);
			}

			else {

				throw XSECException(XSECException::DNameDecodeError,
					"Unexpected escaped character in Distinguished name");

			}

			*i++;

		}

		else {

			result.sbXMLChAppendCh(*i++);

		}

	}

	return XMLString::replicate(result.rawXMLChBuffer());

}

// --------------------------------------------------------------------------------
//           Misc string functions
// --------------------------------------------------------------------------------

// These three functions are pretty much lifted from XMLURL.cpp in Xerces

static bool isHexDigit(const XMLCh toCheck)
{
    if ((toCheck >= chDigit_0) && (toCheck <= chDigit_9)
    ||  (toCheck >= chLatin_A) && (toCheck <= chLatin_F)
    ||  (toCheck >= chLatin_a) && (toCheck <= chLatin_f))
    {
        return true;
    }
    return false;
}

static unsigned int xlatHexDigit(const XMLCh toXlat)
{
    if ((toXlat >= chDigit_0) && (toXlat <= chDigit_9))
        return (unsigned int)(toXlat - chDigit_0);

    if ((toXlat >= chLatin_A) && (toXlat <= chLatin_F))
        return (unsigned int)(toXlat - chLatin_A) + 10;

    return (unsigned int)(toXlat - chLatin_a) + 10;
}

XMLCh * cleanURIEscapes(const XMLCh * uriPath) {

    XMLByte *ptr, *utf8Path;
    xsecsize_t len = XMLString::stringLen(uriPath);

    ptr = utf8Path = new XMLByte[len + 1];

    for (xsecsize_t i = 0; i < len; i++) {
        unsigned int value = uriPath[i];

        if (value > 255) {
            delete[] utf8Path;
            throw XSECException(XSECException::ErrorOpeningURI, "Wide character in URI");
        }

        if (value == chPercent) {
            if (!(i + 2 < len && isHexDigit(uriPath[i + 1]) && isHexDigit(uriPath[i + 2]))) {
                delete[] utf8Path;
                throw XSECException(XSECException::ErrorOpeningURI, "Bad escape sequence in URI");
            }

            value = (xlatHexDigit(uriPath[i + 1]) * 16) + (xlatHexDigit(uriPath[i + 2]));
            i += 2;
        }

        *(ptr++) = (XMLByte) value;
    }
    *ptr = 0;

    try {
        XMLCh* unicodePath = transcodeFromUTF8(utf8Path);
        delete[] utf8Path;
        return unicodePath;
    }
    catch (XMLException&) {
    }

    delete[] utf8Path;
    throw XSECException(XSECException::ErrorOpeningURI, "Failed to transcode URI from UTF-8");
}

// --------------------------------------------------------------------------------
//           Generate Ids
// --------------------------------------------------------------------------------

void makeHexByte(XMLCh * h, unsigned char b) {

	unsigned char toConvert =  (b & 0xf0);
	toConvert = (toConvert >> 4);

	if (toConvert < 10)
		h[0] = chDigit_0 + toConvert;
	else
		h[0] = chLatin_a + toConvert - 10;

	toConvert =  (b & 0xf);

	if (toConvert < 10)
		h[1] = chDigit_0 + toConvert;
	else
		h[1] = chLatin_a + toConvert - 10;

}


XMLCh * generateId(unsigned int bytes) {

	unsigned char b[128];
	XMLCh id[258];
	unsigned int toGen = (bytes > 128 ? 16 : bytes);

	// Get the appropriate amount of random data
	// Need to zeroise to ensure valgrind is happy
	memset(b, 0, 128);
	memset(id, 0, sizeof(id));
	if (XSECPlatformUtils::g_cryptoProvider->getRandom(b, toGen) != toGen) {

		throw XSECException(XSECException::CryptoProviderError,
			"generateId - could not obtain enough random");

	}

	id[0] = chLatin_I;

	unsigned int i;
	for (i = 0; i < toGen; ++i) {

		makeHexByte(&id[1+(i*2)], b[i]);

	}

	id[1+(i*2)] = chNull;

	return XMLString::replicate(id);

}

