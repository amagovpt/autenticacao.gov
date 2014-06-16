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
 * TXFMXPath := Class that performs XPath transforms
 *
 * $Id: TXFMXPath.cpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */


#include <xsec/transformers/TXFMXPath.hpp>
#include <xsec/transformers/TXFMParser.hpp>
#include <xsec/dsig/DSIGConstants.hpp>
#include <xsec/utils/XSECDOMUtils.hpp>
#include <xsec/framework/XSECError.hpp>

#ifndef XSEC_NO_XALAN

#if defined(_MSC_VER)
#	pragma warning(disable: 4267)
#endif

#include <xalanc/XPath/XObjectFactoryDefault.hpp>
#include <xalanc/XPath/XPathExecutionContextDefault.hpp>

#if defined(_MSC_VER)
#	pragma warning(default: 4267)
#endif

// Xalan namespace usage
XALAN_USING_XALAN(XPathProcessorImpl)
XALAN_USING_XALAN(XercesDOMSupport)
XALAN_USING_XALAN(XercesParserLiaison)
XALAN_USING_XALAN(XercesDocumentWrapper)
XALAN_USING_XALAN(XercesWrapperNavigator)
XALAN_USING_XALAN(XPathEvaluator)
XALAN_USING_XALAN(XPathFactoryDefault)
XALAN_USING_XALAN(XPathConstructionContextDefault)
XALAN_USING_XALAN(XalanDocument)
XALAN_USING_XALAN(XalanNode)
XALAN_USING_XALAN(XalanDOMChar)
XALAN_USING_XALAN(XPathEnvSupportDefault)
XALAN_USING_XALAN(XObjectFactoryDefault)
XALAN_USING_XALAN(XPathExecutionContextDefault)
XALAN_USING_XALAN(ElementPrefixResolverProxy)
XALAN_USING_XALAN(XPath)
XALAN_USING_XALAN(NodeRefListBase)
XALAN_USING_XALAN(XSLTResultTarget)
XALAN_USING_XALAN(XSLException)

#endif

XERCES_CPP_NAMESPACE_USE

#if !defined(XSEC_NO_XPATH)

#include <iostream>

#define KLUDGE_PREFIX "berindsig"

// Helper function

void setXPathNS(DOMDocument *d, 
				DOMNamedNodeMap *xAtts, 
			    XSECXPathNodeList &addedNodes,
				XSECSafeBufferFormatter *formatter,
				XSECNameSpaceExpander * nse) {

	// if set then set the name spaces in the attribute list else clear them

	DOMElement * e = d->getDocumentElement();

	if (e == NULL) {

		throw XSECException(XSECException::XPathError, "Element node not found in Document");

	}

	if (xAtts != 0) {

		int xAttsCount = xAtts->getLength();	

		// Check all is OK with the Xalan Document and first element

		if (d == NULL) {

			throw XSECException(XSECException::XPathError, "Attempt to define XPath Name Space before setInput called");

		}

		// Run through each attribute looking for name spaces
		const XMLCh *xpName;
		safeBuffer xpNameSB;
		const XMLCh *xpLocalName;
		const XMLCh *xpValue;

		for (int xCounter = 0; xCounter < xAttsCount; ++xCounter) {

			if (nse == NULL || !nse->nodeWasAdded(xAtts->item(xCounter))) {
				
				xpName = xAtts->item(xCounter)->getNodeName();
				xpNameSB << (*formatter << xpName);
				
				if (xpNameSB.sbStrncmp("xmlns", 5) == 0) {

					// Check whether a node of this name already exists
					xpLocalName = xAtts->item(xCounter)->getLocalName();
					xpValue = xAtts->item(xCounter)->getNodeValue();
					if (e->hasAttributeNS(DSIGConstants::s_unicodeStrURIXMLNS, xpLocalName) == false) {

						// Nope
	
						e->setAttributeNS(DSIGConstants::s_unicodeStrURIXMLNS, xpName, xpValue);
						addedNodes.addNode(e->getAttributeNodeNS(DSIGConstants::s_unicodeStrURIXMLNS, xpLocalName));
					}

				}

			}

		}

	}

	// Insert the kludge namespace
	safeBuffer k("xmlns:");
	k.sbStrcatIn(KLUDGE_PREFIX);

	e->setAttributeNS(DSIGConstants::s_unicodeStrURIXMLNS,
					 MAKE_UNICODE_STRING(k.rawCharBuffer()),
					 DSIGConstants::s_unicodeStrURIDSIG);
}

void clearXPathNS(DOMDocument *d, 
				  XSECXPathNodeList &toRemove,
				  XSECSafeBufferFormatter *formatter,
				  XSECNameSpaceExpander * nse) {

	// Clear the XPath name spaces in the document element attribute list

	DOMElement * e = d->getDocumentElement();

	if (e == NULL) {

		throw XSECException(XSECException::XPathError, "Element node not found in Document");

	}

	// Run through each node in the added nodes

	const DOMNode * r = toRemove.getFirstNode();
	while (r != NULL) {
		e->removeAttributeNS(DSIGConstants::s_unicodeStrURIXMLNS, 
					r->getLocalName());
		r = toRemove.getNextNode();
	}

	e->removeAttributeNS(DSIGConstants::s_unicodeStrURIXMLNS,
					 MAKE_UNICODE_STRING(KLUDGE_PREFIX));

}

TXFMXPath::TXFMXPath(DOMDocument *doc) : 
	TXFMBase(doc) {

	document = NULL;
	XPathAtts = NULL;

	// Formatter is used for handling attribute name space inputs

	XSECnew(formatter, XSECSafeBufferFormatter("UTF-8",XMLFormatter::NoEscapes, 
												XMLFormatter::UnRep_CharRef));

}

TXFMXPath::~TXFMXPath() {

	if (formatter != NULL) 
		delete formatter;
	
}

void TXFMXPath::setNameSpace(DOMNamedNodeMap *xpAtts) {

	// A name space needs to be set on the document

	XPathAtts = xpAtts;

}

// Methods to set the inputs

void TXFMXPath::setInput(TXFMBase *newInput) {

	if (newInput->getOutputType() == TXFMBase::BYTE_STREAM) {

		//throw XSECException(XSECException::TransformInputOutputFail, "C14n canonicalisation transform requires DOM_NODES input");
		// Need to parse into DOM_NODES
		TXFMParser * parser;
		XSECnew(parser, TXFMParser(mp_expansionDoc));
		try{
			parser->setInput(newInput);
		}
		catch (...) {
			delete parser;
			input = newInput;
			throw;
		}

		input = parser;
		parser->expandNameSpaces();
	}
	else
		input = newInput;

	// Set up for the new document
	document = input->getDocument();

	// Expand if necessary
	this->expandNameSpaces();

	keepComments = input->getCommentsStatus();

}

bool separator(unsigned char c) {

	if (c >= 'a' && c <= 'z')
		return false;

	if (c >= 'A' && c <= 'Z')
		return false;

	return true;

}

XalanNode * findHereNodeFromXalan(XercesWrapperNavigator * xwn, XalanNode * n, DOMNode *h) {
	
	const DOMNode * m = xwn->mapNode(n);
	const XalanNode * ret;

	if (m == h)
		return n;

	// Not this one - check the children

	XalanNode * c = n->getFirstChild();

	while (c != 0) {
		ret = findHereNodeFromXalan(xwn, c, h);
		if (ret != 0)
			return (XalanNode *) ret;
		c = c->getNextSibling();
	}

	return 0;
}
		


void TXFMXPath::evaluateExpr(DOMNode *h, safeBuffer inexpr) {

	// Temporarily add any necessary name spaces into the document

	XSECXPathNodeList addedNodes;
	setXPathNS(document, XPathAtts, addedNodes, formatter, mp_nse);

	XPathProcessorImpl	xppi;					// The processor
	XercesParserLiaison xpl;
#if XALAN_VERSION_MAJOR == 1 && XALAN_VERSION_MINOR > 10
	XercesDOMSupport	xds(xpl);
#else
	XercesDOMSupport	xds;
#endif
	XPathEvaluator		xpe;
	XPathFactoryDefault xpf;
	XPathConstructionContextDefault xpcc;

	XalanDocument		* xd;
	XalanNode			* contextNode;

	// Xalan can throw exceptions in all functions, so do one broad catch point.

	try {
	
		// Map to Xalan
		xd = xpl.createDocument(document);

		// For performing mapping
		XercesDocumentWrapper *xdw = xpl.mapDocumentToWrapper(xd);
		XercesWrapperNavigator xwn(xdw);

		// Map the "here" node - but only if part of current document

		XalanNode * hereNode = NULL;

		if (h->getOwnerDocument() == document) {
			
			hereNode = xwn.mapNode(h);

			if (hereNode == NULL) {

				hereNode = findHereNodeFromXalan(&xwn, xd, h);

				if (hereNode == NULL) {

					throw XSECException(XSECException::XPathError,
					   "Unable to find here node in Xalan Wrapper map");
				}

			}
		}

		// Now work out what we have to set up in the new processing

		TXFMBase::nodeType inputType = input->getNodeType();

		XalanDOMString cd;		// For the moment assume the root is the context

		const XalanDOMChar * cexpr;

		safeBuffer contextExpr;

		switch (inputType) {

		case DOM_NODE_DOCUMENT :
		case DOM_NODE_XPATH_NODESET :
			// do XPath over the whole document and, if the input was an 
			// XPath Nodeset, then later intersect the result with the input nodelist			
			cd = XalanDOMString("/");		// Root node
			cexpr = cd.c_str();

			// The context node is the "root" node
			contextNode =
				xpe.selectSingleNode(
				xds,
				xd,
				cexpr,
				xd->getDocumentElement());

			break;

		case DOM_NODE_DOCUMENT_FRAGMENT :
			{

				// Need to map the DOM_Node that we are given from the input to the appropriate XalanNode

				// Create the XPath expression to find the node

				if (input->getFragmentId() != NULL) {

					contextExpr.sbTranscodeIn("//descendant-or-self::node()[attribute::Id='");
					contextExpr.sbXMLChCat(input->getFragmentId());
					contextExpr.sbXMLChCat("']");

					// Map the node

					contextNode = 
						xpe.selectSingleNode(
						xds,
						xd,
						contextExpr.rawXMLChBuffer(), //XalanDOMString((char *) contextExpr.rawBuffer()).c_str(), 
						xd->getDocumentElement());


					if (contextNode == NULL) {
						// Last Ditch
						contextNode = xwn.mapNode(input->getFragmentNode());

					}

				}
				else
					contextNode = xwn.mapNode(input->getFragmentNode());

				if (contextNode == NULL) {

					// Something wrong
					throw XSECException(XSECException::XPathError, "Error mapping context node");

				}

				break;
			}

		default :

			throw XSECException(XSECException::XPathError);	// Should never get here

		}

		safeBuffer str;
		XPathEnvSupportDefault xpesd;
		XObjectFactoryDefault			xof;
		XPathExecutionContextDefault	xpec(xpesd, xds, xof);

		ElementPrefixResolverProxy pr(xd->getDocumentElement(), xpesd, xds);

		// Work around the fact that the XPath implementation is designed for XSLT, so does
		// not allow here() as a NCName.

		// THIS IS A KLUDGE AND SHOULD BE DONE BETTER

		int offset = 0;
		safeBuffer k(KLUDGE_PREFIX);
		k.sbStrcatIn(":");

		offset = inexpr.sbStrstr("here()");

		while (offset >= 0) {

			if (offset == 0 || offset == 1 || 
				(!(inexpr[offset - 1] == ':' && inexpr[offset - 2] != ':') &&
				separator(inexpr[offset - 1]))) {

				inexpr.sbStrinsIn(k.rawCharBuffer(), offset);

			}

			offset = inexpr.sbOffsetStrstr("here()", offset + 11);

		}

		// Install the External function in the Environment handler

		if (hereNode != NULL) {

			xpesd.installExternalFunctionLocal(XalanDOMString(URI_ID_DSIG), XalanDOMString("here"), DSIGXPathHere(hereNode));

		}

		str.sbStrcpyIn("(descendant-or-self::node() | descendant-or-self::node()/attribute::* | descendant-or-self::node()/namespace::*)[");
		str.sbStrcatIn(inexpr);
		str.sbStrcatIn("]");

		XPath * xp = xpf.create();

		XalanDOMString Xexpr((char *) str.rawBuffer());
		xppi.initXPath(*xp, xpcc, Xexpr, pr);
		
		// Now resolve

		XObjectPtr xObj = xp->execute(contextNode, pr, xpec);

		// Now map to a list that others can use (naieve list at this time)

		const NodeRefListBase&	lst = xObj->nodeset();
		
		int size = (int) lst.getLength();
		const DOMNode *item;
		
		for (int i = 0; i < size; ++ i) {

			if (lst.item(i) == xd)
				m_XPathMap.addNode(document);
			else {
				item = xwn.mapNode(lst.item(i));
				m_XPathMap.addNode(item);
			}
		}

		if (inputType == DOM_NODE_XPATH_NODESET) {
			//the input list was a XPATH nodeset, so we must intersect the 
			// results of the XPath processing done above with the input nodeset
			m_XPathMap.intersect(input->getXPathNodeList());
		}
	}

	catch (XSLException &e) {

		safeBuffer msg;

		// Whatever happens - fix any changes to the original document
		clearXPathNS(document, addedNodes, formatter, mp_nse);
	
		// Collate the exception message into an XSEC message.		
		msg.sbTranscodeIn("Xalan Exception : ");
#if defined (XSEC_XSLEXCEPTION_RETURNS_DOMSTRING)
		msg.sbXMLChCat(e.getType().c_str());
#else
		msg.sbXMLChCat(e.getType());
#endif
		msg.sbXMLChCat(" caught.  Message : ");
		msg.sbXMLChCat(e.getMessage().c_str());

		throw XSECException(XSECException::XPathError,
			msg.rawXMLChBuffer());
	}
	
	clearXPathNS(document, addedNodes, formatter, mp_nse);

}

void TXFMXPath::evaluateEnvelope(DOMNode *t) {

	// A special case where the XPath expression is already known

	if (document == NULL) {

		throw XSECException(XSECException::XPathError, 
		   "Attempt to define XPath Name Space before setInput called");

	}

	DOMElement * e = document->getDocumentElement();

	if (e == NULL) {

		throw XSECException(XSECException::XPathError, 
              "Element node not found in Document");

	}

	// Set the xmlns:dsig="http://www.w3.org/2000/09/xmldsig#"

	e->setAttributeNS(DSIGConstants::s_unicodeStrURIXMLNS, MAKE_UNICODE_STRING("xmlns:dsig"), DSIGConstants::s_unicodeStrURIDSIG);
	
	
	// Evaluate

	evaluateExpr(t, XPATH_EXPR_ENVELOPE);

	// Now we are done, remove the namespace
	
	e->removeAttributeNS(DSIGConstants::s_unicodeStrURIXMLNS, MAKE_UNICODE_STRING("dsig"));

}
	
// Methods to get tranform output type and input requirement

TXFMBase::ioType TXFMXPath::getInputType(void) {

	return TXFMBase::DOM_NODES;

}
TXFMBase::ioType TXFMXPath::getOutputType(void) {

	return TXFMBase::DOM_NODES;

}

TXFMBase::nodeType TXFMXPath::getNodeType(void) {

	return TXFMBase::DOM_NODE_XPATH_NODESET;

}

// Methods to get output data

unsigned int TXFMXPath::readBytes(XMLByte * const toFill, unsigned int maxToFill) {

	return 0;

}

DOMDocument *TXFMXPath::getDocument() {

	return document;

}

DOMNode *TXFMXPath::getFragmentNode() {

	return NULL;

}

const XMLCh * TXFMXPath::getFragmentId() {

	return NULL;	// Empty string

}

XSECXPathNodeList	& TXFMXPath::getXPathNodeList() {

	return m_XPathMap;

}

#endif /* NO_XPATH */
