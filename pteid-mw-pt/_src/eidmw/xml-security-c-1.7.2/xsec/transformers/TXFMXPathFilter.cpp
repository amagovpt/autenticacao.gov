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
 * TXFMXPathFilter := Class that performs XPath transforms
 *
 * $Id: TXFMXPathFilter.cpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */


#include <xsec/transformers/TXFMXPathFilter.hpp>
#include <xsec/transformers/TXFMParser.hpp>
#include <xsec/dsig/DSIGConstants.hpp>
#include <xsec/utils/XSECDOMUtils.hpp>
#include <xsec/framework/XSECError.hpp>
#include <xsec/dsig/DSIGXPathFilterExpr.hpp>
#include <xsec/dsig/DSIGXPathHere.hpp>

#include <xercesc/util/Janitor.hpp>

XERCES_CPP_NAMESPACE_USE

#ifndef XSEC_NO_XALAN

#if defined(_MSC_VER)
#	pragma warning(disable: 4267)
#endif

#include <xalanc/XalanDOM/XalanDocument.hpp>
#include <xalanc/XalanDOM/XalanDOMString.hpp>
#include <xalanc/XercesParserLiaison/XercesDocumentWrapper.hpp>
#include <xalanc/XercesParserLiaison/XercesDOMSupport.hpp>
#include <xalanc/XercesParserLiaison/XercesParserLiaison.hpp>
#include <xalanc/XPath/XPathEvaluator.hpp>
#include <xalanc/XPath/XPathProcessorImpl.hpp>
#include <xalanc/XPath/XPathFactoryDefault.hpp>
#include <xalanc/XPath/NodeRefList.hpp>
#include <xalanc/XPath/XPathEnvSupportDefault.hpp>
#include <xalanc/XPath/XPathConstructionContextDefault.hpp>
#include <xalanc/XPath/ElementPrefixResolverProxy.hpp>
#include <xalanc/XPath/XObjectFactoryDefault.hpp>
#include <xalanc/XPath/XPathExecutionContextDefault.hpp>
#include <xalanc/XSLT/XSLTResultTarget.hpp>

#if defined(_MSC_VER)
#	pragma warning(default: 4267)
#endif

// Xalan namespace usage
XALAN_USING_XALAN(XPathProcessorImpl)
XALAN_USING_XALAN(XalanDOMString)
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
XALAN_USING_XALAN(XObjectPtr)
XALAN_USING_XALAN(XPathExecutionContextDefault)
XALAN_USING_XALAN(ElementPrefixResolverProxy)
XALAN_USING_XALAN(XPath)
XALAN_USING_XALAN(NodeRefListBase)
XALAN_USING_XALAN(XSLTResultTarget)
XALAN_USING_XALAN(XSLException)

#endif

#if !defined(XSEC_NO_XPATH)

#include <iostream>

#define KLUDGE_PREFIX "berindsig"

// Helper functions - come from DSIGXPath

void setXPathNS(DOMDocument *d, 
				DOMNamedNodeMap *xAtts, 
			    XSECXPathNodeList &addedNodes,
				XSECSafeBufferFormatter *formatter,
				XSECNameSpaceExpander * nse);

void clearXPathNS(DOMDocument *d, 
				  XSECXPathNodeList &toRemove,
				  XSECSafeBufferFormatter *formatter,
				  XSECNameSpaceExpander * nse);

bool separator(unsigned char c);
XalanNode * findHereNodeFromXalan(XercesWrapperNavigator * xwn, XalanNode * n, DOMNode *h);



TXFMXPathFilter::TXFMXPathFilter(DOMDocument *doc) : 
	TXFMBase(doc) {

	document = NULL;
	XSECnew(mp_formatter, XSECSafeBufferFormatter("UTF-8",XMLFormatter::NoEscapes, 
												XMLFormatter::UnRep_CharRef));

}

TXFMXPathFilter::~TXFMXPathFilter() {

	lstsVectorType::iterator i;

	for (i = m_lsts.begin(); i != m_lsts.end(); ++i) {

		if ((*i)->lst != NULL)
			delete ((*i)->lst);

		delete (*i);

	}

	if (mp_formatter != NULL) 
		delete mp_formatter;

	
}

// Methods to set the inputs

void TXFMXPathFilter::setInput(TXFMBase *newInput) {

	if (newInput->getOutputType() == TXFMBase::BYTE_STREAM) {

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

XSECXPathNodeList * TXFMXPathFilter::evaluateSingleExpr(DSIGXPathFilterExpr *expr) {

	// Have a single expression that we wish to find the resultant nodeset
	// for

	XSECXPathNodeList addedNodes;
	setXPathNS(document, expr->mp_NSMap, addedNodes, mp_formatter, mp_nse);

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

		// Map the "here" node

		XalanNode * hereNode = NULL;

		hereNode = xwn.mapNode(expr->mp_xpathFilterNode);

		if (hereNode == NULL) {

			hereNode = findHereNodeFromXalan(&xwn, xd, expr->mp_exprTextNode);

			if (hereNode == NULL) {

				throw XSECException(XSECException::XPathFilterError,
				   "Unable to find here node in Xalan Wrapper map");
			}

		}

		// Now work out what we have to set up in the new processing

		XalanDOMString cd;		// For XPath Filter, the root is always the context node

		cd = XalanDOMString("/");		// Root node

		// The context node is the "root" node
		contextNode =
			xpe.selectSingleNode(
			xds,
			xd,
			cd.c_str(),
			xd->getDocumentElement());

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

		// Map the expression into a local code page string (silly - should be XMLCh)
		safeBuffer exprSB;
		exprSB << (*mp_formatter << expr->m_expr.rawXMLChBuffer());

		offset = exprSB.sbStrstr("here()");

		while (offset >= 0) {

			if (offset == 0 || offset == 1 || 
				(!(exprSB[offset - 1] == ':' && exprSB[offset - 2] != ':') &&
				separator(exprSB[offset - 1]))) {

				exprSB.sbStrinsIn(k.rawCharBuffer(), offset);

			}

			offset = exprSB.sbOffsetStrstr("here()", offset + 11);

		}

		// Install the External function in the Environment handler

		if (hereNode != NULL) {

			xpesd.installExternalFunctionLocal(XalanDOMString(URI_ID_DSIG), XalanDOMString("here"), DSIGXPathHere(hereNode));

		}

		XPath * xp = xpf.create();

		XalanDOMString Xexpr((char *) exprSB.rawBuffer());
		xppi.initXPath(*xp, xpcc, Xexpr, pr);
		
		// Now resolve

		XObjectPtr xObj = xp->execute(contextNode, pr, xpec);

		// Now map to a list that others can use (naieve list at this time)

		const NodeRefListBase&	lst = xObj->nodeset();
		
		int size = (int) lst.getLength();
		const DOMNode *item;
		
		XSECXPathNodeList * ret;
		XSECnew(ret, XSECXPathNodeList);
		Janitor<XSECXPathNodeList> j_ret(ret);

		for (int i = 0; i < size; ++ i) {

			if (lst.item(i) == xd)
				ret->addNode(document);
			else {
				item = xwn.mapNode(lst.item(i));
				ret->addNode(item);
			}
		}

		xpesd.uninstallExternalFunctionGlobal(XalanDOMString(URI_ID_DSIG), XalanDOMString("here"));

		clearXPathNS(document, addedNodes, mp_formatter, mp_nse);

		j_ret.release();
		return ret;

	}

	catch (XSLException &e) {

		safeBuffer msg;

		// Whatever happens - fix any changes to the original document
		clearXPathNS(document, addedNodes, mp_formatter, mp_nse);
	
		// Collate the exception message into an XSEC message.		
		msg.sbTranscodeIn("Xalan Exception : ");
#if defined (XSEC_XSLEXCEPTION_RETURNS_DOMSTRING)
		msg.sbXMLChCat(e.getType().c_str());
#else
		msg.sbXMLChCat(e.getType());
#endif
		msg.sbXMLChCat(" caught.  Message : ");
		msg.sbXMLChCat(e.getMessage().c_str());

		throw XSECException(XSECException::XPathFilterError,
			msg.rawXMLChBuffer());

	}

	catch (...) {
		clearXPathNS(document, addedNodes, mp_formatter, mp_nse);
		throw;
	}

	return NULL;
}

bool TXFMXPathFilter::checkNodeInInput(DOMNode * n, DOMNode * attParent) {

	if (mp_fragment != NULL) {

		DOMNode * p = n;

		/* Check attParent here to reduce cycles */
		if (attParent != NULL) {
			if (p == mp_fragment)
				return true;
			p = attParent;
		}

		while (p != NULL) {

			if (p == mp_fragment)
				return true;
			
			p = p->getParentNode();

		}

		return false;

	}

	return mp_inputList->hasNode(n);

}

bool TXFMXPathFilter::checkNodeInScope(DOMNode * n) {

	// Walk backwards through the lists
	lstsVectorType::iterator lstsIter;

	lstsIter = m_lsts.end();
	filterSetHolder *sh;

	while (lstsIter != m_lsts.begin()) {

		lstsIter--;
		sh = *lstsIter;
		if (sh->ancestorInScope != NULL) {

			// Have an ancestor in scope, so this node is in this list
			if (sh->type == FILTER_UNION)
				// Got this far, must be OK!
				return true;
			if (sh->type == FILTER_SUBTRACT)
				return false;

		}
		else {

			if (sh->type == FILTER_INTERSECT)
				return false;

		}

	}

	return true;

}
#if 1
void TXFMXPathFilter::walkDocument(DOMNode * n) {

	// Non-recursive version

	DOMNode * current = n;
	DOMNode * next;
	DOMNode * attParent = NULL; 	/* Assign NULL to remove spurious Forte warning */
	bool done = false;
	bool treeUp = false;
	DOMNamedNodeMap * atts = n->getAttributes();
	int attsSize = -1;
	int currentAtt = -1;
	lstsVectorType::iterator lstsIter;

	while (done == false && current != NULL) {

		if (treeUp == true) {

			if (current == n) {

				// We are complete.
				done = true;

			}

			else {

				// Remove this node from the ancestor lists
				for (lstsIter = m_lsts.begin(); lstsIter != m_lsts.end(); ++lstsIter) {

					if ((*lstsIter)->ancestorInScope == current) {
						(*lstsIter)->ancestorInScope = NULL;
					}
				}


				// Check for another sibling
				next = current->getNextSibling();

				if (next == NULL) {

					current = current->getParentNode();
					treeUp = true;
				}

				else {

					current = next;
					treeUp = false;

				}

			}

		} /* treeUp == true */

		else {

			// Check if the current node is in the result set.  The walk the children

			// First check if this node is in any lists, and if so,
			// set the appropriate ancestor nodes (if necessary)

			for (lstsIter = m_lsts.begin(); lstsIter != m_lsts.end(); ++lstsIter) {

				if ((*lstsIter)->ancestorInScope == NULL &&
					(*lstsIter)->lst->hasNode(current)) {

					(*lstsIter)->ancestorInScope = current;

				}

				// 
			}

			// Now that the ancestor setup is done, check to see if this node is 
			// in scope.

			if (checkNodeInScope(current) && 
				checkNodeInInput(current, (atts != NULL ? attParent : NULL))) {

				m_xpathFilterMap.addNode(current);

			}

			// Now find the next node!

			if (atts != NULL) {

				// Working on an attribute list
				currentAtt++;

				if (currentAtt == attsSize) {

					// Attribute list complete
					atts = NULL;
					current = attParent;
					next = current->getFirstChild();
					if (next == NULL)
						treeUp = true;
					else {
						current = next;
						treeUp = false;
					}

				}

				else {

					current = atts->item(currentAtt);

				}

			}

			else {
				// Working on an element or other non-attribute node
				atts = current->getAttributes();

				if (atts != NULL && ((attsSize = atts->getLength()) > 0)) {

					currentAtt = 0;
					attParent = current;
					current = atts->item(0);
					treeUp = false;

				}

				else {

					atts = NULL;

					next = current->getFirstChild();

					if (next != NULL) {
						current = next;
						treeUp = false;
					}

					else {

						treeUp = true;

					}

				}
			} /* ! atts == NULL */
		}
	} /* while */
}
#endif
#if 0

void TXFMXPathFilter::walkDocument(DOMNode * n) {

	// Check if the current node is in the result set.  The walk the children
	lstsVectorType::iterator lstsIter;

	// First check if this node is in any lists, and if so,
	// set the appropriate ancestor nodes (if necessary)

	for (lstsIter = m_lsts.begin(); lstsIter != m_lsts.end(); ++lstsIter) {

		if ((*lstsIter)->ancestorInScope == NULL &&
			(*lstsIter)->lst->hasNode(n)) {

			(*lstsIter)->ancestorInScope = n;

		}
	}

	// Now that the ancestor setup is done, check to see if this node is 
	// in scope.

	if (checkNodeInScope(n) && checkNodeInInput(n)) {

		m_xpathFilterMap.addNode(n);

	}

	// Do any attributes

	DOMNamedNodeMap * atts = n->getAttributes();
	
	if (atts != NULL) {

		unsigned int s = atts->getLength();
		for (unsigned int i = 0; i < s; ++i) {

			walkDocument(atts->item(i));

		}

	}

	// Do any childeren

	DOMNode * c = n->getFirstChild();

	while (c != NULL) {

		walkDocument(c);
		c = c->getNextSibling();

	}

	// Now remove from ancestor lists if we are that ancestor
	for (lstsIter = m_lsts.begin(); lstsIter != m_lsts.end(); ++lstsIter) {

		if ((*lstsIter)->ancestorInScope == n) {
			(*lstsIter)->ancestorInScope = NULL;
		}
	}

}
#endif
void TXFMXPathFilter::evaluateExprs(DSIGTransformXPathFilter::exprVectorType * exprs) {

	if (exprs == NULL || exprs->size() < 1) {

		throw XSECException(XSECException::XPathFilterError,
			"TXFMXPathFilter::evaluateExpr - no expression list set");

	}

	DSIGTransformXPathFilter::exprVectorType::iterator i;

	for (i = exprs->begin(); i != exprs->end(); ++i) {

		XSECXPathNodeList * lst = evaluateSingleExpr(*i);
		filterSetHolder * sh;
		XSECnew(sh, filterSetHolder);

		sh->lst = lst;
		sh->type = (*i)->m_filterType;
		sh->ancestorInScope = NULL;

		if (lst != NULL) {

			m_lsts.push_back(sh);

		}

	}

	// Well we appear to have successfully run through all the nodelists!

	mp_fragment = NULL;
	mp_inputList = NULL;

	// Find the input nodeset
	TXFMBase::nodeType inputType = input->getNodeType();
	switch (inputType) {

	case DOM_NODE_DOCUMENT :

		mp_fragment = document;
		break;

	case DOM_NODE_DOCUMENT_FRAGMENT :

		mp_fragment = input->getFragmentNode();
		break;

	case DOM_NODE_XPATH_NODESET :

		mp_inputList = &(input->getXPathNodeList());
		break;

	default :

		throw XSECException(XSECException::XPathFilterError,
			"TXFMXPathFilter::evaluateExprs - unknown input type");

	}

	// Now just recurse through each node in the document
	walkDocument(document);


}
	
	
// Methods to get tranform output type and input requirement

TXFMBase::ioType TXFMXPathFilter::getInputType(void) {

	return TXFMBase::DOM_NODES;

}
TXFMBase::ioType TXFMXPathFilter::getOutputType(void) {

	return TXFMBase::DOM_NODES;

}

TXFMBase::nodeType TXFMXPathFilter::getNodeType(void) {

	return TXFMBase::DOM_NODE_XPATH_NODESET;

}

// Methods to get output data

unsigned int TXFMXPathFilter::readBytes(XMLByte * const toFill, unsigned int maxToFill) {

	return 0;

}

DOMDocument *TXFMXPathFilter::getDocument() {

	return document;

}

DOMNode *TXFMXPathFilter::getFragmentNode() {

	return NULL;

}

const XMLCh * TXFMXPathFilter::getFragmentId() {

	return NULL;	// Empty string

}

XSECXPathNodeList	& TXFMXPathFilter::getXPathNodeList() {

	return m_xpathFilterMap;

}

#endif /* NO_XPATH */



