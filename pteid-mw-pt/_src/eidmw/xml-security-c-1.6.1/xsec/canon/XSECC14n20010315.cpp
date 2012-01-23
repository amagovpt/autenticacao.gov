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
 * XSECC14n20010315 := Canonicaliser object to process XML document in line with
 *					     RFC 3076
 *
 * Author(s): Berin Lautenbach
 *
 * $Id: XSECC14n20010315.cpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

//XSEC includes
#include <xsec/framework/XSECDefs.hpp>
#include <xsec/framework/XSECError.hpp>
#include <xsec/canon/XSECC14n20010315.hpp>
#include <xsec/utils/XSECDOMUtils.hpp>
#include <xsec/utils/XSECSafeBufferFormatter.hpp>

// Xerces includes
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMNamedNodeMap.hpp>
#include <xercesc/util/XMLUniDefs.hpp>

XERCES_CPP_NAMESPACE_USE

#ifndef XSEC_NO_XALAN

// Xalan includes
#include <xalanc/XalanDOM/XalanDocument.hpp>
#include <xalanc/XercesParserLiaison/XercesDocumentWrapper.hpp>
#include <xalanc/XercesParserLiaison/XercesDOMSupport.hpp>
#include <xalanc/XercesParserLiaison/XercesParserLiaison.hpp>
#include <xalanc/XPath/XPathEvaluator.hpp>
#include <xalanc/XPath/NodeRefList.hpp>

// Namespace definitions
XALAN_USING_XALAN(XPathEvaluator)
XALAN_USING_XALAN(XercesDOMSupport)
XALAN_USING_XALAN(XercesParserLiaison)
XALAN_USING_XALAN(XalanDocument)
XALAN_USING_XALAN(XalanNode)
XALAN_USING_XALAN(XalanElement)
XALAN_USING_XALAN(XalanDOMString)
XALAN_USING_XALAN(XalanDOMChar)
XALAN_USING_XALAN(NodeRefList)
XALAN_USING_XALAN(XercesDocumentWrapper)
XALAN_USING_XALAN(XercesWrapperNavigator)
XALAN_USING_XALAN(c_wstr)

#endif

// General includes
#include <stdlib.h>
#include <string.h>

#include <iostream>


// --------------------------------------------------------------------------------
//           Some useful utilities
// --------------------------------------------------------------------------------

// Find a node in an XSECNodeList

bool NodeInList(const XSECNodeListElt * lst, const DOMNode * toFind) {

	const XSECNodeListElt * tmp = lst;

	while (tmp != NULL) {

		if (tmp->element == toFind)
			return true;

		tmp = tmp->next;

	}

	return false;

}

XSECNodeListElt * insertNodeIntoList(XSECNodeListElt * lst, XSECNodeListElt *toIns) {

	XSECNodeListElt *tmp, *last;

	if (lst == NULL) {

		// Goes at start
		toIns->next = NULL;
		toIns->last = NULL;

		return toIns;

	} /* if mp_attributes == NULL */

	// Need to run through start of list

	tmp = lst;
	last = NULL;
	int res = -1;   // Used to remove a gcc warning

	while ((tmp != NULL) &&
		((res = toIns->sortString.sbStrcmp(tmp->sortString)) >= 0)) {

		last = tmp;
		tmp = tmp->next;

	} /* while */

	if (res ==0) {

		// Already exists!
		delete toIns;
		return lst;

	}

	if (last == NULL) {

		// It sits before first element

		toIns->next = lst;
		toIns->last = NULL;
		lst->last = tmp;

		return toIns;

	} /* if last == NULL */

	// We have found where it goes

	toIns->next = tmp;
	toIns->last = last;

	if (tmp != NULL)
		tmp->last = toIns;

	last->next = toIns;

	return lst;

}

// --------------------------------------------------------------------------------
//           Exclusive Canonicalisation Methods
// --------------------------------------------------------------------------------


bool visiblyUtilises(DOMNode *node, safeBuffer &ns) {

	// Test whether the node uses the name space passed in

	if (strEquals(node->getPrefix(), (char *) ns.rawBuffer()))
		return true;

	if (ns.sbStrcmp("") == 0)
		return false;		// Attributes are never in default namespace

	// Check the attributes
	DOMNamedNodeMap *atts = node->getAttributes();
	if (atts == NULL)
		return false;

	XMLSize_t size = atts->getLength();

	for (XMLSize_t i = 0; i < size; ++i) {

		if (strEquals(atts->item(i)->getPrefix(), (char *) ns.rawBuffer()) &&
			!strEquals(atts->item(i)->getLocalName(), "xmlns"))
			return true;

	}

	return false;

}

bool XSECC14n20010315::inNonExclNSList(safeBuffer &ns) {

	int size = (int) m_exclNSList.size();

	for (int i = 0; i < size; ++i) {

		if (!strcmp((char *) ns.rawBuffer(), m_exclNSList[i]))
			return true;

	}

	return false;

}

void XSECC14n20010315::setInclusive11(void) {
    m_incl11 = true;
    m_exclusive = false;
    m_exclusiveDefault = false;
}

void XSECC14n20010315::setExclusive(void) {

	m_exclusive = true;
	m_exclusiveDefault = true;
	m_incl11 = false;
}

void XSECC14n20010315::setExclusive(char * xmlnsList) {

	char * nsBuf;

	setExclusive();

	// Set up the define non-exclusive prefixes

	nsBuf = new char [strlen(xmlnsList) + 1];

	if (nsBuf == NULL) {

		throw XSECException (XSECException::MemoryAllocationFail,
			"Error allocating a string buffer in XSECC14n20010315::setExclusive");

	}

	int i, j;

	i = 0;

	while (xmlnsList[i] != '\0') {

		while (xmlnsList[i] == ' ' ||
			   xmlnsList[i] == '\0' ||
			   xmlnsList[i] == '\t' ||
			   xmlnsList[i] == '\r' ||
			   xmlnsList[i] == '\n')

			   ++i;	// Skip white space

		j = 0;
		while (!(xmlnsList[i] == ' ' ||
			   xmlnsList[i] == '\0' ||
			   xmlnsList[i] == '\t' ||
			   xmlnsList[i] == '\r' ||
			   xmlnsList[i] == '\n'))

			   nsBuf[j++] = xmlnsList[i++];	// Copy name

		// Terminate the string
		nsBuf[j] = '\0';
		if (strcmp(nsBuf, "#default") == 0) {

			// Default is not to be exclusive
			m_exclusiveDefault = false;

		}

		else {

			// Add this to the list
			m_exclNSList.push_back(strdup(nsBuf));

		}

	}

	delete[] nsBuf;

}





// --------------------------------------------------------------------------------
//           XSECC14n20010315 methods
// --------------------------------------------------------------------------------


void XSECC14n20010315::stackInit(DOMNode * n) {

	if (n == NULL)
		return;

	stackInit(n->getParentNode());
	m_nsStack.pushElement(n);
	XMLSize_t size;

	DOMNamedNodeMap *tmpAtts = n->getAttributes();
	safeBuffer currentName;

	if (tmpAtts != NULL)
		size = tmpAtts->getLength();
	else
		size = 0;

	XMLSize_t i;

	for (i = 0; i < size; ++i) {

		currentName << (*mp_formatter << tmpAtts->item(i)->getNodeName());

		if (currentName.sbStrncmp("xmlns", 5) == 0)
			m_nsStack.addNamespace(tmpAtts->item(i));

	}

}


// Constructors

void XSECC14n20010315::init() {

	// This does the work of setting us up and checks to make sure everyhing is OK

	// Set up the Xerces formatter

	XSECnew(mp_formatter, XSECSafeBufferFormatter("UTF-8",XMLFormatter::NoEscapes,
												XMLFormatter::UnRep_CharRef));

	// Set up for first attribute list

	mp_attributes = mp_currentAttribute = mp_firstNonNsAttribute = NULL;

	// By default process comments
	m_processComments = true;

	// Set up for tree walking

	m_returnedFromChild = false;
	mp_firstElementNode = mp_startNode;
	m_firstElementProcessed = false;

	// XPath setup
	m_XPathSelection = false;
	m_XPathMap.clear();

	// Exclusive Canonicalisation setup

	m_exclNSList.clear();
	m_exclusive = false;
	m_exclusiveDefault = false;

	// Default to 1.0 mode
	m_incl11 = false;

	// Namespace handling
	m_useNamespaceStack = true;

	// INitialise the stack - even if we don't use it later, at least this sets us up
	if (mp_startNode != NULL) {
		stackInit(mp_startNode->getParentNode());
	}

}


XSECC14n20010315::XSECC14n20010315() {};
XSECC14n20010315::XSECC14n20010315(DOMDocument *newDoc) : XSECCanon(newDoc) {

	// Just call the init function;

	init();

};
XSECC14n20010315::XSECC14n20010315(DOMDocument *newDoc,
								   DOMNode *newStartNode) : XSECCanon(newDoc, newStartNode) {

	// Just call the init function

	init();

}

XSECC14n20010315::~XSECC14n20010315() {

	if (mp_formatter != NULL)
		delete mp_formatter;

	// Clear out the exclusive namespace list
	int size = (int) m_exclNSList.size();

	for (int i = 0; i < size; ++i) {

		free(m_exclNSList[i]);

	}

	m_exclNSList.clear();

    while (mp_attributes != NULL) {

        mp_currentAttribute = mp_attributes->next;
        delete mp_attributes;
        mp_attributes = mp_currentAttribute;
    }

    mp_attributes = mp_currentAttribute = mp_firstNonNsAttribute = NULL;
}

// --------------------------------------------------------------------------------
//           XSECC14n20010315 Comments procesing
// --------------------------------------------------------------------------------


void XSECC14n20010315::setCommentsProcessing(bool onoff) {

	m_processComments = onoff;

}

bool XSECC14n20010315::getCommentsProcessing(void) {

	return m_processComments;

}


// --------------------------------------------------------------------------------
//           XSECC14n20010315 XPathSelectNodes method
// --------------------------------------------------------------------------------

// Use an XPath expression to select a subset of nodes from the document


int XSECC14n20010315::XPathSelectNodes(const char * XPathExpr) {

#ifdef XSEC_NO_XPATH

	throw XSECException(XSECException::UnsupportedFunction,
		"This library has been compiled without XPath support");

#else

	XPathEvaluator::initialize();

	// We use Xalan to process the Xerces DOM tree and get the XPath nodes

#if XALAN_VERSION_MAJOR == 1 && XALAN_VERSION_MINOR > 10
	XercesParserLiaison theParserLiaison;
	XercesDOMSupport theDOMSupport(theParserLiaison);
#else
	XercesDOMSupport theDOMSupport;
#if defined XSEC_XERCESPARSERLIAISON_REQS_DOMSUPPORT
	XercesParserLiaison theParserLiaison(theDOMSupport);
#else
	XercesParserLiaison theParserLiaison;
#endif
#endif // XALAN_VERSION_MAJOR == 1 && XALAN_VERSION_MINOR > 10

	if (mp_doc == 0) {
		throw XSECException(XSECException::UnsupportedFunction,
			"XPath selection only supported in C14n for full documents");
	}

	DOMElement* theXercesNode = mp_doc->createElement(c_wstr(XalanDOMString("ns")));
	theXercesNode->setAttribute(c_wstr(XalanDOMString("xmlns:ietf")), c_wstr(XalanDOMString("http://www.ietf.org")));

	XalanDocument* theDoc = theParserLiaison.createDocument(mp_doc);

	// Set up the XPath evaluator

	XPathEvaluator	theEvaluator;

	// OK, let's find the context node...

	XalanDOMString cd = XalanDOMString("/");	// For the moment assume the root is the context

	const XalanDOMChar * cexpr = cd.c_str();

	XalanNode* const	theContextNode =
		theEvaluator.selectSingleNode(
		theDOMSupport,
		theDoc,
		cexpr,
		theDoc->getDocumentElement());

	if (theContextNode == 0)
	{

		// No appropriate nodes.
		return 0;

	}
	// OK, let's evaluate the expression...

	XalanDOMString ed = XalanDOMString(XPathExpr);
	const XalanDOMChar * expr = ed.c_str();

#if defined XSEC_SELECTNODELIST_REQS_NODEREFLIST

	NodeRefList output;

	NodeRefList	theResult(
		theEvaluator.selectNodeList(
		output,
		theDOMSupport,
		theContextNode,
		expr,
		theDoc->getElementById(XalanDOMString("ns"))));

#else

	NodeRefList	theResult(
		theEvaluator.selectNodeList(
		theDOMSupport,
		theContextNode,
		expr,
		theDoc->getElementById(XalanDOMString("ns"))));
		//theDoc->getDocumentElement()));
#endif

	//XercesDocumentBridge *theBridge = theParserLiaison.mapDocument(theDoc);
	XercesDocumentWrapper *theWrapper = theParserLiaison.mapDocumentToWrapper(theDoc);
	XercesWrapperNavigator theWrapperNavigator(theWrapper);

	int size = (int) theResult.getLength();
	const DOMNode *item;

	for (int i = 0; i < size; ++ i) {

		item = theWrapperNavigator.mapNode(theResult.item(i));
		m_XPathMap.addNode(item);
		//tmp->element = theBridgeNavigator.mapNode(theResult.item(i));
	}

	m_XPathSelection = true;
	return size;

#endif

}

void XSECC14n20010315::setXPathMap(const XSECXPathNodeList & map) {

	// XPath already done!

	m_XPathMap = map;
	m_XPathSelection = true;

}

// --------------------------------------------------------------------------------
//           XSECC14n20010315 processNextNode method
// --------------------------------------------------------------------------------

safeBuffer c14nCleanText(safeBuffer &input) {

	/* c14n Requires :

		& -> &amp
		< -> &lt
		> -> &gt
		LF -> &#xD

	*/

	xsecsize_t len = input.sbStrlen();
	safeBuffer ret;

	xsecsize_t i, j;
	unsigned char c;

	j = 0;
	for (i = 0; i < len; ++i) {

		c = input[i];

		switch (c) {

		case '&' :

			ret[j++] = '&';
			ret[j++] = 'a';
			ret[j++] = 'm';
			ret[j++] = 'p';
			ret[j++] = ';';

			break;

		case '<' :

			ret[j++] = '&';
			ret[j++] = 'l';
			ret[j++] = 't';
			ret[j++] = ';';

			break;

		case '>' :

			ret[j++] = '&';
			ret[j++] = 'g';
			ret[j++] = 't';
			ret[j++] = ';';

			break;

		case 0xD :

			ret[j++] = '&';
			ret[j++] = '#';
			ret[j++] = 'x';
			ret[j++] = 'D';
			ret[j++] = ';';

			break;

		default :

			ret[j++] = c;

		}

	}

	// final character:

	ret[j] = '\0';

	ret.setBufferType(safeBuffer::BUFFER_CHAR);

	return ret;

}

safeBuffer c14nCleanAttribute(safeBuffer &input) {

	/* c14n Requires :

		& -> &amp
		< -> &lt
		" -> &quot
		#x9 ->&#x9
		#xA ->&#xA
		LF -> &#xD

	*/

	xsecsize_t len = input.sbStrlen();
	safeBuffer ret;

	xsecsize_t i, j;
	unsigned char c;

	j = 0;
	for (i = 0; i < len; ++i) {

		c = input[i];

		switch (c) {

		case '&' :

			ret[j++] = '&';
			ret[j++] = 'a';
			ret[j++] = 'm';
			ret[j++] = 'p';
			ret[j++] = ';';

			break;

		case '"' :

			ret[j++] = '&';
			ret[j++] = 'q';
			ret[j++] = 'u';
			ret[j++] = 'o';
			ret[j++] = 't';
			ret[j++] = ';';

			break;

		case '<' :

			ret[j++] = '&';
			ret[j++] = 'l';
			ret[j++] = 't';
			ret[j++] = ';';

			break;

		case 0x9 :

			ret[j++] = '&';
			ret[j++] = '#';
			ret[j++] = 'x';
			ret[j++] = '9';
			ret[j++] = ';';

			break;

		case 0xA :

			ret[j++] = '&';
			ret[j++] = '#';
			ret[j++] = 'x';
			ret[j++] = 'A';
			ret[j++] = ';';

			break;

		case 0xD :

			ret[j++] = '&';
			ret[j++] = '#';
			ret[j++] = 'x';
			ret[j++] = 'D';
			ret[j++] = ';';

			break;

		default :

			ret[j++] = c;

		}

	}

	// final character:

	ret[j] = '\0';
	ret.setBufferType(safeBuffer::BUFFER_CHAR);

	return ret;

}

bool XSECC14n20010315::checkRenderNameSpaceNode(DOMNode *e, DOMNode *a) {

	DOMNode *parent;
	DOMNode *att;
	DOMNamedNodeMap *atts;

	// If XPath and node not selected, then never print
	if (m_XPathSelection && ! m_XPathMap.hasNode(a))
		return false;

    // BUGFIX: we need to skip xmlns:xml if the value is http://www.w3.org/XML/1998/namespace
    if (strEquals(a->getLocalName(), "xml") && strEquals(a->getNodeValue(), "http://www.w3.org/XML/1998/namespace"))
        return false;

	// First - are we exclusive?

	safeBuffer localName;
	bool processAsExclusive = false;

	if (m_exclusive) {

		if (strEquals(a->getNodeName(), "xmlns")) {
			processAsExclusive = m_exclusiveDefault;
		}
		else {
			localName << (*mp_formatter << a->getLocalName());
			processAsExclusive = !inNonExclNSList(localName);
		}

	}

	if (processAsExclusive) {

		// Is the parent in the  node-set?
		if (m_XPathSelection && !m_XPathMap.hasNode(e))
			return false;

		// Is the name space visibly utilised?
		localName << (*mp_formatter << a->getLocalName());

		if (localName.sbStrcmp("xmlns") == 0)
			localName[0] = '\0';			// Is this correct or should Xerces return "" for default?

		if (!visiblyUtilises(e, localName))
			return false;

		// If we are the top node, then this has never been printer
		if (e == mp_firstElementNode)
			return true;

		// Make sure previous nodes do not use the name space (and have it printed)
		parent = e->getParentNode();

		while (parent != NULL) {

			if (!m_XPathSelection || m_XPathMap.hasNode(parent)) {

				// An output ancestor
				if (visiblyUtilises(parent, localName)) {

					// Have a hit!
					while (parent != NULL) {
						atts = parent->getAttributes();
						att = (atts != NULL) ? atts->getNamedItem(a->getNodeName()) : NULL;
						if (att != NULL && (!m_XPathSelection || m_XPathMap.hasNode(att))) {

							// Check URI is the same
							if (strEquals(att->getNodeValue(), a->getNodeValue()))
								return false;

							return true;

						}

						// If we are using the namespace stack, we need to go up until
						// we found the defining attribute for this node
						if (m_useNamespaceStack) {
							parent = parent->getParentNode();
						}
						else

							return true;
					}
					// Even if we are using the namespace stack, we have never
					// printed this namespace
					return true;
				}
			}

			if (parent == mp_firstElementNode)
				parent = NULL;
			else
				parent = parent->getParentNode();

		}

		// Didn't find it rendered!
		return true;

	}

	// Either we are now in non-exclusive mode, or the name space in question
	// Is to be treated as non-exclusive

	// Never directly render a default
	if (strEquals(a->getNodeName(), "xmlns") && strEquals(a->getNodeValue(), ""))
		return false;

	// If using a namespace stack, then we need to check whether the current node is in the nodeset
	// Only really necessary for envelope txfms in boundary conditions

	if (m_useNamespaceStack && m_XPathSelection && !m_XPathMap.hasNode(e))
		return false;

	// Otherwise, of node is at base of selected document, then print
	if (e == mp_firstElementNode)
		return true;

	if (m_useNamespaceStack) {

		// In this case, we need to go up until we find the namespace definition
		// in question

		parent = e->getParentNode();
		while (parent != NULL) {

			if (!m_XPathSelection || m_XPathMap.hasNode(parent)) {
				DOMNamedNodeMap *pmap = parent->getAttributes();
				DOMNode *pns;
				if (pmap)
					pns = pmap->getNamedItem(a->getNodeName());
				else
					pns = NULL;

				if (pns != NULL) {

					// Note we don't check XPath inclusion, as we shouldn't be
					// using the namespace stack for XPath expressions

					if (strEquals(pns->getNodeValue(), a->getNodeValue()))
						return false;
					else
						return true;		// Was defined but differently
				}
			}
			parent = parent->getParentNode();
		}
		// Obviously we haven't found it!
		return true;
	}

	// Find the parent and check if the node is already defined or if the node
	// was out of scope
	parent = e->getParentNode();
//	if (m_XPathSelection && !m_XPathMap.hasNode(parent))
//		return true;

	while (m_XPathSelection && parent != NULL && !m_XPathMap.hasNode(parent))
		parent = parent->getParentNode();

	if (parent == NULL)
		return true;

	DOMNamedNodeMap *pmap = parent->getAttributes();

	DOMNode *pns = pmap->getNamedItem(a->getNodeName());

	if (pns != NULL) {

		if (m_XPathSelection && !m_XPathMap.hasNode(pns))
			return true;			// Not printed in previous node

		if (strEquals(pns->getNodeValue(), a->getNodeValue()))
			return false;
		else
			return true;		// Was defined but differently

	}

	return true;			// Not defined in previous node

}



	// Check an attribute to see if we should output


// This is the main worker function of this class

xsecsize_t XSECC14n20010315::processNextNode() {

	// The information currently in the buffer has all been used.  We now process the
	// next node.

	DOMNode *next;				// For working (had *ns)
	DOMNamedNodeMap *tmpAtts;	//  "     "
	safeBuffer currentName(128), currentValue(1024), sbWork;
	bool done, xmlnsFound;


	if (m_allNodesDone) {

		return 0;					// No bytes copied because nothing more to be done

	}

	// Always zeroise buffers to make work simpler
	m_bufferLength = m_bufferPoint = 0;
	m_buffer.sbStrcpyIn("");

	// Find out if this is a node to process
	bool processNode;
	int nodeT;

	if (mp_nextNode == 0) {

		// Dummy element - we need to insert a default namespace

		nodeT = DOMNode::ATTRIBUTE_NODE;
		processNode = true;

	}
	else {

		processNode = ((!m_XPathSelection) || (m_XPathMap.hasNode(mp_nextNode)));
		nodeT = mp_nextNode->getNodeType();

	}

	switch (nodeT) {

	case DOMNode::DOCUMENT_NODE : // Start of a document

		// Check if finished
		if (m_returnedFromChild) {

			// All done!
			m_allNodesDone = true;

			return 0;

		}

		// In c14n we don't actually do anything for a document node except
		// process the childeren

		mp_firstElementNode = ((DOMDocument *) mp_nextNode)->getDocumentElement();
		next = mp_nextNode->getFirstChild();

		if (next == NULL) {

			// Empty document?
			m_allNodesDone = true;

		}

		mp_nextNode = next;
		m_bufferLength = m_bufferPoint = 0;		// To ensure nobody copies "nothing"

		return 0;

	case DOMNode::DOCUMENT_TYPE_NODE : // Ignore me

		m_returnedFromChild = true;
		m_buffer.sbStrcpyIn("");
		break;

	case DOMNode::PROCESSING_INSTRUCTION_NODE : // Just print

		if (processNode) {
			if ((mp_nextNode->getParentNode() == mp_doc) && m_firstElementProcessed) {

				// this is a top level node and first element done
				m_buffer.sbStrcpyIn("\x00A<?");

			}
			else
				m_buffer.sbStrcpyIn("<?");

			m_formatBuffer << (*mp_formatter << mp_nextNode->getNodeName());
			m_buffer.sbStrcatIn(m_formatBuffer);

			m_formatBuffer << (*mp_formatter << ((DOMProcessingInstruction *) mp_nextNode)->getData());
			if (m_formatBuffer.sbStrlen() > 0) {
				m_buffer.sbStrcatIn(" ");
				m_buffer.sbStrcatIn(m_formatBuffer);
			}

			m_buffer.sbStrcatIn("?>");

			if ((mp_nextNode->getParentNode() == mp_doc) && !m_firstElementProcessed) {

				// this is a top level node and first element done
				m_buffer.sbStrcatIn("\x00A");

			}
		}

		// Node fully processed
		m_returnedFromChild = true;

		break;


	case DOMNode::COMMENT_NODE : // Just print out

		if (processNode && m_processComments) {
			if ((mp_nextNode->getParentNode() == mp_doc) && m_firstElementProcessed) {

				// this is a top level node and first element done
				m_buffer.sbStrcpyIn("\x00A<!--");

			}
			else
				m_buffer.sbStrcpyIn("<!--");

			m_formatBuffer << (*mp_formatter << mp_nextNode->getNodeValue());

			if (m_formatBuffer.sbStrlen() > 0) {
				m_buffer.sbStrcatIn(m_formatBuffer);
			}

			m_buffer.sbStrcatIn("-->");

			if ((mp_nextNode->getParentNode() == mp_doc) && !m_firstElementProcessed) {

				// this is a top level node and first element done
				m_buffer.sbStrcatIn("\x00A");

			}
		}

		m_returnedFromChild = true;	// Fool the tree processor

		break;

	case DOMNode::CDATA_SECTION_NODE :
	case DOMNode::TEXT_NODE : // Straight copy for now

		if (processNode) {
			m_formatBuffer << (*mp_formatter << mp_nextNode->getNodeValue());

			// Do c14n cleaning on the text string

			m_buffer = c14nCleanText(m_formatBuffer);

		}

		// Fall through
		m_returnedFromChild = true;		// Fool the tree processor
		break;

	case DOMNode::ELEMENT_NODE : // This is an element that we can easily process

		// If we are going "up" then we simply close off the element

		if (m_returnedFromChild) {
			if (processNode) {
				m_buffer.sbStrcpyIn ("</");
				m_formatBuffer << (*mp_formatter << mp_nextNode->getNodeName());
				m_buffer.sbStrcatIn(m_formatBuffer);
				m_buffer.sbStrcatIn(">");
			}

			if (m_useNamespaceStack)
				m_nsStack.popElement();

			break;
		}

		if (processNode) {

			m_buffer.sbStrcpyIn("<");
			m_formatBuffer << (*mp_formatter << mp_nextNode->getNodeName());
			m_buffer.sbStrcatIn(m_formatBuffer);
		}

		// We now set up for attributes and name spaces
		if (m_useNamespaceStack)
			m_nsStack.pushElement(mp_nextNode);

		mp_attributes = NULL;
		tmpAtts = mp_nextNode->getAttributes();
		next = mp_nextNode;

		done = false;
		xmlnsFound = false;

		while (!done) {

			// Need to sort the attributes

			XMLSize_t size;

			if (tmpAtts != NULL)
				size = tmpAtts->getLength();
			else
				size = 0;

			XSECNodeListElt *toIns;
			XMLSize_t i;

			for (i = 0; i < size; ++i) {

				// Get the name and value of the attribute
				currentName << (*mp_formatter << tmpAtts->item(i)->getNodeName());
				currentValue << (*mp_formatter << tmpAtts->item(i)->getNodeValue());

				// Build the string used to sort this node

				if ((next == mp_nextNode) && currentName.sbStrncmp("xmlns", 5) == 0) {

					// Are we using the namespace stack?  If so - store this for later
					// processing
					if (m_useNamespaceStack) {
						m_nsStack.addNamespace(tmpAtts->item(i));
					}
					else {

						// Is this the default?
						if (currentName.sbStrcmp("xmlns") == 0 &&
							(!m_XPathSelection || m_XPathMap.hasNode(tmpAtts->item(i))) &&
							!currentValue.sbStrcmp("") == 0)
							xmlnsFound = true;

						// A namespace node - See if we need to output
						if (checkRenderNameSpaceNode(mp_nextNode, tmpAtts->item(i))) {

							// Add to the list

							m_formatBuffer << (*mp_formatter << tmpAtts->item(i)->getNodeName());
							if (m_formatBuffer[5] == ':')
								currentName.sbStrcpyIn((char *) &m_formatBuffer[6]);
							else
								currentName.sbStrcpyIn("");

							toIns = new XSECNodeListElt;
							toIns->element = tmpAtts->item(i);

							// Build and insert name space node
							toIns->sortString.sbStrcpyIn(XMLNS_PREFIX);
							toIns->sortString.sbStrcatIn(currentName);

							// Insert node
							mp_attributes = insertNodeIntoList(mp_attributes, toIns);

						}
					}

				}

				else {

					// A "normal" attribute - only process if selected or no XPath or is an
					// XML node from a previously un-printed Element node

					bool XMLElement = (next != mp_nextNode) && (!m_exclusive) && !currentName.sbStrncmp("xml:", 4) &&
                        (!m_incl11 || currentName.sbStrcmp("xml:id"));

					// If we have an XML element, make sure it was not printed between this
					// node and the node currently  being worked on

					if (XMLElement) {

						DOMNode *t = mp_nextNode->getParentNode();
						if (m_XPathSelection && m_XPathMap.hasNode(t))
							XMLElement = false;
						else {

							// This is a real node that we have to check

							t = mp_nextNode;
							while (t != next) {
								DOMNamedNodeMap *ta;
								XMLSize_t sz;

								ta = t->getAttributes();

								if (ta != NULL)
									sz = ta->getLength();
								else
									sz = 0;

								for (XMLSize_t j = 0; j < sz; ++j) {

									if (strEquals(ta->item(j)->getNodeName(),
										tmpAtts->item(i)->getNodeName()) == true) {

										XMLElement = false;
										break;

									}

								}

								t = t->getParentNode();

							}

						}

					}



					if ((!m_XPathSelection && next == mp_nextNode) || XMLElement || ((next == mp_nextNode) && m_XPathMap.hasNode(tmpAtts->item(i)))) {

						toIns = new XSECNodeListElt;
						toIns->element = tmpAtts->item(i);

						// First the correct prefix to ensure will be sorted
						// in correct placing against XMLNS nodes

						toIns->sortString.sbStrcpyIn(ATTRIBUTE_PREFIX);

						// Find the namespace URI
						const XMLCh * nsURI =
							tmpAtts->item(i)->getNamespaceURI();

						if (nsURI == NULL) {
							toIns->sortString.sbStrcatIn(NOURI_PREFIX);
						}
						else {
							m_formatBuffer << (*mp_formatter << nsURI);
							toIns->sortString.sbStrcatIn(HAVEURI_PREFIX);
							toIns->sortString.sbStrcatIn(m_formatBuffer);
						}

						// Append the local name as the secondary key
						const XMLCh * ln = tmpAtts->item(i)->getNodeName();
						int index = XMLString::indexOf(ln, chColon);
						if (index >= 0)
							ln = &ln[index+1];
						m_formatBuffer << (*mp_formatter << ln);
						toIns->sortString.sbStrcatIn(m_formatBuffer);

						// Insert node
						mp_attributes = insertNodeIntoList(mp_attributes, toIns);

					} /* else (sbStrCmp xmlns) */
				}
			} /* for */
#if 1
			// Now go upwards and find parent for xml name spaces
			if (processNode && (m_XPathSelection || mp_nextNode == mp_firstElementNode)) {
				next = next->getParentNode();

				if (next == 0) // || NodeInList(mp_XPathMap, next))
					done = true;
				else
					tmpAtts = next->getAttributes();
			}
			else
#endif
				done = true;

		} /* while tmpAtts != NULL */

		// Now add namespace nodes - but only if we are using the namespace stack
		// (They have already been added otherwise
		if (m_useNamespaceStack) {

			DOMNode * nsnode = m_nsStack.getFirstNamespace();
			while (nsnode != NULL) {
				// Get the name and value of the attribute
				currentName << (*mp_formatter << nsnode->getNodeName());
				currentValue << (*mp_formatter << nsnode->getNodeValue());

				// Is this the default?
				if (currentName.sbStrcmp("xmlns") == 0 &&
					(!m_XPathSelection || m_XPathMap.hasNode(nsnode)) &&
					!currentValue.sbStrcmp("") == 0)
					xmlnsFound = true;

				// A namespace node - See if we need to output
				if (checkRenderNameSpaceNode(mp_nextNode, nsnode)) {

					// Add to the list
					XSECNodeListElt *toIns;

					m_formatBuffer << (*mp_formatter << nsnode->getNodeName());
					if (m_formatBuffer[5] == ':')
						currentName.sbStrcpyIn((char *) &m_formatBuffer[6]);
					else
						currentName.sbStrcpyIn("");

					toIns = new XSECNodeListElt;
					toIns->element = nsnode;

					// Build and insert name space node
					toIns->sortString.sbStrcpyIn(XMLNS_PREFIX);
					toIns->sortString.sbStrcatIn(currentName);

					// Insert node
					mp_attributes = insertNodeIntoList(mp_attributes, toIns);

					// Mark as printed in the NS Stack
					m_nsStack.printNamespace(nsnode, mp_nextNode);

				}
				nsnode = m_nsStack.getNextNamespace();
			}

			// Fix for bug#47353, make sure we set xmlnsFound regardless of what the printing process saw.
	        if (!xmlnsFound)
	            xmlnsFound = m_nsStack.isNonEmptyDefaultNS();

		} /* if (m_useNamespaceStack) */


		// Check to see if we add xmlns=""
		if (processNode && !xmlnsFound && mp_nextNode != mp_firstElementNode) {

			// Is this exclusive?

			safeBuffer sbLocalName("");

			if (m_exclusiveDefault) {

				if (visiblyUtilises(mp_nextNode, sbLocalName)) {

					// May have to output!
					next = mp_nextNode->getParentNode();

					while (next != NULL) {

						if (!m_XPathSelection || m_useNamespaceStack || m_XPathMap.hasNode(next)) {

							DOMNode *tmpAtt;

							// An output ancestor
							if (visiblyUtilises(next, sbLocalName)) {
								DOMNode * nextAttParent = next;

								while (nextAttParent != NULL) {
									// Have a hit!
									tmpAtts = nextAttParent->getAttributes();
									if (tmpAtts != NULL)
										tmpAtt = tmpAtts->getNamedItem(DSIGConstants::s_unicodeStrXmlns);
									if (tmpAtts != NULL && tmpAtt != NULL && (!m_XPathSelection || m_useNamespaceStack || m_XPathMap.hasNode(tmpAtt))) {

										// Check URI is the same
										if (!strEquals(tmpAtt->getNodeValue(), "")) {
											xmlnsFound = true;
											nextAttParent = NULL;
										}
									}
									else {

										// Doesn't have a default namespace in the node-set
										next = nextAttParent = NULL;
										break;

									}

									if (m_useNamespaceStack && nextAttParent)
										nextAttParent = nextAttParent->getParentNode();
									else
										nextAttParent = NULL;
								}


							}
						}

						if (next)
							next = next->getParentNode();
					}

				}
			} /* m_exclusiveDefault */

			else {

				//DOM_Node next;

				next = mp_nextNode->getParentNode();
				while (!xmlnsFound && next != NULL) {
					while (next != NULL && !m_useNamespaceStack && (m_XPathSelection && !m_XPathMap.hasNode(next)))
						next = next->getParentNode();

					XMLSize_t size;
					if (next != NULL)
						tmpAtts = next->getAttributes();

					if (next != NULL && tmpAtts != NULL)
						size = tmpAtts->getLength();
					else
						size = 0;

					for (XMLSize_t i = 0; i < size; ++i) {

						currentName << (*mp_formatter << tmpAtts->item(i)->getNodeName());
						currentValue << (*mp_formatter << tmpAtts->item(i)->getNodeValue());

						if ((currentName.sbStrcmp("xmlns") == 0) &&
							(m_useNamespaceStack || !m_XPathSelection || m_XPathMap.hasNode(tmpAtts->item(i)))) {
							if (currentValue.sbStrcmp("") != 0) {
								xmlnsFound = true;
							}
							else {
								xmlnsFound = false;
								next = NULL;
							}
						}

					}
					if (m_useNamespaceStack && next != NULL)
						next = next->getParentNode();
					else
						next = NULL;
				}
			}

			// Did we find a non empty namespace?
			if (xmlnsFound) {

				currentName.sbStrcpyIn("");		// Don't include xmlns prefix
				XSECNodeListElt * toIns;

				toIns = new XSECNodeListElt;
				toIns->element = NULL;		// To trigger the state engine

				// Build and insert name space node
				toIns->sortString.sbStrcpyIn(XMLNS_PREFIX);
				toIns->sortString.sbStrcatIn(currentName);

				// Insert node
				mp_attributes = insertNodeIntoList(mp_attributes, toIns);
			}
		}


		if (mp_attributes != NULL) {

			// Now we have set up the attribute list, set next node and return!

			mp_attributeParent = mp_nextNode;
			mp_nextNode = mp_attributes->element;
			mp_currentAttribute = mp_attributes;
			m_bufferLength = m_buffer.sbStrlen();
			m_bufferPoint = 0;

			return m_bufferLength;

		} /* attrributes != NULL */


		if (processNode)
			m_buffer.sbStrcatIn(">");

		// Fall through to find next node

		break;

	case DOMNode::ATTRIBUTE_NODE : // Output attr_name="value"

		// Always process an attribute node as we have already checked they should
		// be printed

		m_buffer.sbStrcpyIn(" ");

		if (mp_nextNode != 0) {

			m_formatBuffer << (*mp_formatter << mp_nextNode->getNodeName());
			m_buffer.sbStrcatIn(m_formatBuffer);

			m_buffer.sbStrcatIn("=\"");

			m_formatBuffer << (*mp_formatter << mp_nextNode->getNodeValue());
			sbWork = c14nCleanAttribute(m_formatBuffer);
			m_buffer.sbStrcatIn(sbWork);

			m_buffer.sbStrcatIn("\"");
		}
		else {
			m_buffer.sbStrcatIn("xmlns");
			m_buffer.sbStrcatIn("=\"");
			m_buffer.sbStrcatIn("\"");
		}


		// Now see if next node is an attribute

		mp_currentAttribute = mp_currentAttribute->next;
		if (mp_currentAttribute != NULL) {

			// Easy case
			mp_nextNode = mp_currentAttribute->element;
			m_bufferLength = m_buffer.sbStrlen();
			m_bufferPoint = 0;

			return m_bufferLength;


		} /* if mp_currentAttributes != NULL) */

		// need to clear out the node list
		while (mp_attributes != NULL) {

			mp_currentAttribute = mp_attributes->next;
			delete mp_attributes;
			mp_attributes = mp_currentAttribute;
		}

		mp_attributes = mp_currentAttribute = mp_firstNonNsAttribute = NULL;

		// return us to the element node
		mp_nextNode = mp_attributeParent;

		// End the element definition
		if (!m_XPathSelection || (m_XPathMap.hasNode(mp_nextNode)))
			m_buffer.sbStrcatIn(">");

		m_returnedFromChild = false;


		break;

	default:

		break;

	}

	// A node has fallen through to the default case for finding the next node.

	m_bufferLength = m_buffer.sbStrlen();;
	m_bufferPoint = 0;

	// Firstly, was the last piece of processing because we "came up" from a child node?

	if (m_returnedFromChild) {

		if (mp_nextNode == mp_startNode) {

			// we have closed off the document!
			m_allNodesDone = true;
			return m_bufferLength;

		}

		if (mp_nextNode == mp_firstElementNode) {

			// we have closed off the main mp_doc elt
			m_firstElementProcessed = true;

		}

	}

	else {

		// Going down - so check for children nodes
		next = mp_nextNode->getFirstChild();

		if (next != NULL)

			mp_nextNode = next;

		else

			// No children, so need to close this node off!
			m_returnedFromChild = true;

		return m_bufferLength;

	}

	// If we get here all childeren (if there are any) are done

	next = mp_nextNode->getNextSibling();
	if (next != NULL) {

		m_returnedFromChild = false;
		mp_nextNode = next;
		return m_bufferLength;

	}

	// No more nodes at this level either!

	mp_nextNode = mp_nextNode->getParentNode();
	m_returnedFromChild = true;
	return m_bufferLength;

}
