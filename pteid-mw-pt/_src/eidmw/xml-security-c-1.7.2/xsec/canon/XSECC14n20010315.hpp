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
 * $Id: XSECC14n20010315.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef XSECC14n20010315_INCLUDE
#define XSECC14n20010315_INCLUDE

//XSEC includes
#include <xsec/framework/XSECDefs.hpp>
#include <xsec/utils/XSECSafeBuffer.hpp>
#include <xsec/utils/XSECXPathNodeList.hpp>
#include <xsec/canon/XSECCanon.hpp>
#include <xsec/canon/XSECXMLNSStack.hpp>

#include <xercesc/framework/XMLFormatter.hpp>

// General includes
#include <memory.h>
#include <vector>

XSEC_USING_XERCES(XMLFormatter);
XSEC_USING_XERCES(XMLFormatTarget);

class XSECSafeBufferFormatter;

// --------------------------------------------------------------------------------
//           Simple structure for holding a list of nodes
// --------------------------------------------------------------------------------

// NOTE: We don't use NamedNodeMap or DOMNodeList as we are unsure what might happen
// to them in the future.  Also, to add items we would have to delve into the inards
// of Xerces (and use the "...impl" classes).  Such an approach might not be supported
// in the future.

struct XSECNodeListElt {

	XERCES_CPP_NAMESPACE_QUALIFIER DOMNode	*element;	// Element referred to
	safeBuffer						sortString;	// The string that is used to sort the nodes

	XSECNodeListElt					*next,
									*last;		// For the list

};

// Used for the sorting function

#define XMLNS_PREFIX		"a"
#define ATTRIBUTE_PREFIX	"b"

#define NOURI_PREFIX         "a"
#define HAVEURI_PREFIX       "b"

// --------------------------------------------------------------------------------
//           XSECC14n20010315 Object definition
// --------------------------------------------------------------------------------

class CANON_EXPORT XSECC14n20010315 : public XSECCanon {

#if defined(XALAN_NO_NAMESPACES)
	typedef vector<char *>				CharListVectorType;
#else
	typedef std::vector<char *>			CharListVectorType;
#endif

#if defined(XALAN_SIZE_T_IN_NAMESPACE_STD)
	typedef std::size_t		size_type;
#else
	typedef size_t			size_type;
#endif


public:

	// Constructors
	XSECC14n20010315();
	XSECC14n20010315(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *newDoc);
	XSECC14n20010315(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *newDoc,
					 XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *newStartNode);
	virtual ~XSECC14n20010315();

	// XPath processor

	int XPathSelectNodes(const char * XPathExpr);
	void setXPathMap(const XSECXPathNodeList & map);

	// Comments processing
	void setCommentsProcessing(bool onoff);
	bool getCommentsProcessing(void);

	// Exclusive processing
	void setExclusive(void);
	void setExclusive(char * xmlnsList);

	// Inclusive 1.1 processing
    void setInclusive11(void);

	// Namespace processing
	void setUseNamespaceStack(bool flag) {m_useNamespaceStack = flag;}

protected:

	// Implementation of virtual function
	xsecsize_t processNextNode();

	// Test whether a name space is in the non-exclusive list
	bool inNonExclNSList(safeBuffer &ns);

private:

	void init();
	bool checkRenderNameSpaceNode(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *e,
								  XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *a);
	void stackInit(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode * n);

	// For formatting the buffers
	XSECSafeBufferFormatter		* mp_formatter;
	safeBuffer					m_formatBuffer;

	// For holding state whilst walking the DOM tree
	XSECNodeListElt	* mp_attributes,				// Start of list
					* mp_currentAttribute,			// Where we currently are in list
					* mp_firstNonNsAttribute;		// First NON XMLNS element in list
	XERCES_CPP_NAMESPACE_QUALIFIER DOMNode * mp_attributeParent;			// To return up the tree
	bool m_returnedFromChild;						// Did we get to this node from below?
	XERCES_CPP_NAMESPACE_QUALIFIER DOMNode * mp_firstElementNode;			// The root element of the document
	bool			m_firstElementProcessed;		// Has the first node been handled?
	unsigned char	* mp_charBuffer;

	// For XPath evaluation
	bool			  m_XPathSelection;				// Are we doing an XPath?
	XSECXPathNodeList m_XPathMap;					// The elements in the XPath

	// For comment processing
	bool			m_processComments;				// Whether comments are in or out (in by default)

	// For exclusive canonicalisation
	CharListVectorType		m_exclNSList;
	bool					m_exclusive;
	bool					m_exclusiveDefault;

	// For Inclusive 1.1 support
	bool                    m_incl11;

	// How do we handle namespaces?
	// Use the namespace stack if name space expansion has not been run on the document.
	// NOTE : Using the name space stack *will* cause problems if a full XPath expression
	// has been run to select the input nodeset.  Otherwise we should be fine.

	bool					m_useNamespaceStack;
	XSECXMLNSStack			m_nsStack;



};


#endif /* XSECC14n20010315_INCLUDE */


