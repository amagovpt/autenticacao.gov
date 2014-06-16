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
 * XSECSOAPRequestorSimple := (Very) Basic implementation of a SOAP
 *                         HTTP wrapper for testing the client code.
 *
 *
 * $Id: XSECSOAPRequestorSimple.cpp 1493962 2013-06-17 22:32:41Z scantor $
 *
 */

#include <xsec/framework/XSECError.hpp>
#include <xsec/utils/XSECSafeBuffer.hpp>
#include <xsec/utils/XSECDOMUtils.hpp>
#include <xsec/utils/XSECSOAPRequestorSimple.hpp>
#include <xsec/xkms/XKMSConstants.hpp>

#include "../utils/XSECAutoPtr.hpp"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/framework/XMLFormatter.hpp>
#include <xercesc/framework/MemBufFormatTarget.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLNetAccessor.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/XMLExceptMsgs.hpp>
#include <xercesc/util/Janitor.hpp>
#include <xercesc/util/XMLUniDefs.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>

XERCES_CPP_NAMESPACE_USE

// --------------------------------------------------------------------------------
//           Strings for constructing SOAP envelopes
// --------------------------------------------------------------------------------

static XMLCh s_prefix[] = {

	chLatin_e,
	chLatin_n,
	chLatin_v,
	chNull
};

static XMLCh s_Envelope[] = {

	chLatin_E,
	chLatin_n,
	chLatin_v,
	chLatin_e,
	chLatin_l,
	chLatin_o,
	chLatin_p,
	chLatin_e,
	chNull
};

static XMLCh s_Body[] = {

	chLatin_B,
	chLatin_o,
	chLatin_d,
	chLatin_y,
	chNull
};

// --------------------------------------------------------------------------------
//           Constructors and Destructors
// --------------------------------------------------------------------------------


/* NOTE: This is initialised via the platform specific code */

XSECSOAPRequestorSimple::~XSECSOAPRequestorSimple() {
}


// --------------------------------------------------------------------------------
//           Wrap and serialise the request message
// --------------------------------------------------------------------------------

char * XSECSOAPRequestorSimple::wrapAndSerialise(DOMDocument * request) {

	// Prepare the serialiser

	XMLCh tempStr[100];
	XMLString::transcode("Core", tempStr, 99);    
	DOMImplementation *impl = DOMImplementationRegistry::getDOMImplementation(tempStr);
#if defined (XSEC_XERCES_DOMLSSERIALIZER)
    // DOM L3 version as per Xerces 3.0 API
    DOMLSSerializer   *theSerializer = ((DOMImplementationLS*)impl)->createLSSerializer();
    Janitor<DOMLSSerializer> j_theSerializer(theSerializer);

    // Get the config so we can set up pretty printing
    DOMConfiguration *dc = theSerializer->getDomConfig();
    dc->setParameter(XMLUni::fgDOMWRTFormatPrettyPrint, false);

    // Now create an output object to format to UTF-8
    DOMLSOutput *theOutput = ((DOMImplementationLS*)impl)->createLSOutput();
    Janitor<DOMLSOutput> j_theOutput(theOutput);
	MemBufFormatTarget *formatTarget = new MemBufFormatTarget;
	Janitor<MemBufFormatTarget> j_formatTarget(formatTarget);

    theOutput->setEncoding(MAKE_UNICODE_STRING("UTF-8"));
    theOutput->setByteStream(formatTarget);

#else
	DOMWriter         *theSerializer = ((DOMImplementationLS*)impl)->createDOMWriter();
	Janitor<DOMWriter> j_theSerializer(theSerializer);

	theSerializer->setEncoding(MAKE_UNICODE_STRING("UTF-8"));
	if (theSerializer->canSetFeature(XMLUni::fgDOMWRTFormatPrettyPrint, false))
		theSerializer->setFeature(XMLUni::fgDOMWRTFormatPrettyPrint, false);

	MemBufFormatTarget *formatTarget = new MemBufFormatTarget;
	Janitor<MemBufFormatTarget> j_formatTarget(formatTarget);

#endif

	if (m_envelopeType != ENVELOPE_NONE) {

		// Create a new document to wrap the request in
		safeBuffer str;

		makeQName(str, s_prefix, s_Envelope);

		DOMDocument *doc;
		
		if (m_envelopeType == ENVELOPE_SOAP11) {
			doc = impl->createDocument(
				XKMSConstants::s_unicodeStrURISOAP11,
						str.rawXMLChBuffer(),
						NULL);
			DOMElement *rootElem = doc->getDocumentElement();

			makeQName(str, s_prefix, s_Body);
			DOMElement *body = doc->createElementNS(
					XKMSConstants::s_unicodeStrURISOAP11,
					str.rawXMLChBuffer());

			rootElem->appendChild(body);

			// Now replicate the request into the document
			DOMElement * reqElement = (DOMElement *) doc->importNode(request->getDocumentElement(), true);
			body->appendChild(reqElement);
		}
		else {
			doc = impl->createDocument(
				XKMSConstants::s_unicodeStrURISOAP12,
						str.rawXMLChBuffer(),
						NULL);
			DOMElement *rootElem = doc->getDocumentElement();

			makeQName(str, s_prefix, s_Body);
			DOMElement *body = doc->createElementNS(
					XKMSConstants::s_unicodeStrURISOAP12,
					str.rawXMLChBuffer());

			rootElem->appendChild(body);

			// Now replicate the request into the document
			DOMElement * reqElement = (DOMElement *) doc->importNode(request->getDocumentElement(), true);
			body->appendChild(reqElement);
		}


		// OK - Now we have the SOAP request as a document, we serialise to a string buffer
		// and return

#if defined (XSEC_XERCES_DOMLSSERIALIZER)
        theSerializer->write(doc, theOutput);
#else
		theSerializer->writeNode(formatTarget, *doc);
#endif
		doc->release();

	}
	else {
#if defined (XSEC_XERCES_DOMLSSERIALIZER)
        theSerializer->write(request, theOutput);
#else
		theSerializer->writeNode(formatTarget, *request);
#endif
	}

	// Now replicate the buffer
	return XMLString::replicate((const char *) formatTarget->getRawBuffer());

}

// --------------------------------------------------------------------------------
//           UnWrap and de-serialise the response message
// --------------------------------------------------------------------------------

DOMDocument * XSECSOAPRequestorSimple::parseAndUnwrap(const char * buf, unsigned int len) {

	XercesDOMParser parser;
	parser.setDoNamespaces(true);
	parser.setLoadExternalDTD(false);

	SecurityManager securityManager;
	securityManager.setEntityExpansionLimit(XSEC_ENTITY_EXPANSION_LIMIT);
	parser.setSecurityManager(&securityManager);

	// Create an input source

	MemBufInputSource memIS((const XMLByte*) buf, len, "XSECMem");

	parser.parse(memIS);
	xsecsize_t errorCount = parser.getErrorCount();
    if (errorCount > 0)
		throw XSECException(XSECException::HTTPURIInputStreamError,
							"Error parsing response message");

	if (m_envelopeType == ENVELOPE_NONE) {

		return parser.adoptDocument();

	}

	DOMDocument * responseDoc = parser.getDocument();

	// Must be a SOAP message of some kind - so lets remove the wrapper.
	// First create a new document for the Response message

	XMLCh tempStr[100];
	XMLString::transcode("Core", tempStr, 99);    
	DOMImplementation *impl = DOMImplementationRegistry::getDOMImplementation(tempStr);

	DOMDocument * retDoc = impl->createDocument();

	// Find the base of the response


	DOMNode * e = responseDoc->getDocumentElement();

	e = e->getFirstChild();

	while (e != NULL && (e->getNodeType() != DOMNode::ELEMENT_NODE || !strEquals(e->getLocalName(), "Body")))
		e = e->getNextSibling();

	if (e == NULL)
		throw XSECException(XSECException::HTTPURIInputStreamError,
							"Could not find SOAP body");

	e = findFirstChildOfType(e, DOMNode::ELEMENT_NODE);

	if (e == NULL)
		throw XSECException(XSECException::HTTPURIInputStreamError,
							"Could not find message within SOAP body");

	/* See if this is a soap fault */
	if (strEquals(e->getLocalName(), "Fault")) {

		// Something has gone wrong somewhere!
		safeBuffer sb;
		sb.sbTranscodeIn("SOAP Fault : ");

		// Find the fault code

		e = findFirstElementChild(e);
		while (e != NULL && !strEquals(e->getLocalName(), "Code"))
			e = findNextElementChild(e);
		
		if (e != NULL) {
			DOMNode * c = findFirstElementChild(e);
			while (c != NULL && !strEquals(c->getLocalName(), "Value"))
				c = findNextElementChild(c);
			if (c != NULL) {
				DOMNode * t = findFirstChildOfType(c, DOMNode::TEXT_NODE);
				if (t != NULL) {
					sb.sbXMLChCat(t->getNodeValue());
					sb.sbXMLChCat(" : ");
				}
			}
		}

		// Find the reason
		while (e != NULL && !strEquals(e->getLocalName(), "Reason"))
			e = findNextElementChild(e);

		if (e != NULL) {
			DOMNode * t = findFirstChildOfType(e, DOMNode::TEXT_NODE);
			if (t != NULL) {
				sb.sbXMLChCat(t->getNodeValue());
			}

		}

		retDoc->release();

		XSECAutoPtrChar msg(sb.rawXMLChBuffer());

		throw XSECException(XSECException::HTTPURIInputStreamError,
							msg.get());
	}


	retDoc->appendChild(retDoc->importNode(e, true));

	return retDoc;

}

// --------------------------------------------------------------------------------
//           Envelope Type handling
// --------------------------------------------------------------------------------

void XSECSOAPRequestorSimple::setEnvelopeType(envelopeType et) {

	m_envelopeType = et;

}
