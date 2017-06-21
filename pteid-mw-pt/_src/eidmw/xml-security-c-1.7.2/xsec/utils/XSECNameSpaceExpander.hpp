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
 * XSECNameSpaceHolder := Container class for holding and managing the name space stack
 *						  Used when running through a DOM document
 *
 * $Id: XSECNameSpaceExpander.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef XSECNAMESPACEEXPANDER_HEADER
#define XSECNAMESPACEEXPANDER_HEADER

// XSEC Includes
#include <xsec/framework/XSECDefs.hpp>
#include <xsec/utils/XSECSafeBuffer.hpp>
#include <xsec/utils/XSECSafeBufferFormatter.hpp>

// Xerces Includes
XSEC_DECLARE_XERCES_CLASS(DOMDocument);
XSEC_DECLARE_XERCES_CLASS(DOMNode);
XSEC_DECLARE_XERCES_CLASS(DOMElement);

#include <vector>

// --------------------------------------------------------------------------------
//           Structure Definition for the nodes within the list of nodes
// --------------------------------------------------------------------------------

struct XSECNameSpaceEntry {

	// Variables
	safeBuffer									m_name;			// The name for this name space
	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement	* mp_node;		// The Element Node owner
	XERCES_CPP_NAMESPACE_QUALIFIER DOMNode		* mp_att;		// The added attribute node
			
};

// --------------------------------------------------------------------------------
//           Class definition for the list
// --------------------------------------------------------------------------------

/**
 * @ingroup pubsig
 */
/*\@{*/

/**
 * @brief Class to "expand" name spaces
 *
 * For most things, a DOM model interoperates well with XPath.  Unfortunately,
 * name-spaces are the one main problem.  In particular, the XPath spec
 * states that every element node has an attribute node for its own 
 * namespaces, and one for namespaces above that are in scope.
 *
 * In the DOM scheme of things, a namespace is only available in the node in
 * which it is defined.  Normally this is not a problem, you can just just
 * refer backwards until you find the namespace you need.  However, for XPath
 * expressions that select namespace nodes, we need to actually promulgate
 * the name-spaces down to every node where they are visible so that the XPath
 * selection will work properly.
 *
 * This is important for Canonicalisation of the found nodes, but we cannot
 * do this only in the canonicaliser as it does not internally understand how
 * to do DSIG style XPath.  So the XPath is done externally, and the 
 * resultant node set (including any selected "Expanded" attribute nodes).
 * are passed in.
 *
 * The expander therefore handles the propogation of the namespace nodes, and
 * removes the propogated nodes when it goes out of scope (or when
 * deleteAddedNamespaces() is called).
 *
 */


class CANON_EXPORT XSECNameSpaceExpander {


#if defined(XALAN_NO_NAMESPACES)
	typedef vector<XSECNameSpaceEntry *>			NameSpaceEntryListVectorType;
#else
	typedef std::vector<XSECNameSpaceEntry *>		NameSpaceEntryListVectorType;
#endif


public:

    /** @name Constructors and Destructors */
    //@{
	
    /**
	 * \brief Main constructure
	 *
	 * Use this constructor to expand namespaces through an entire document.
	 *
	 * @param d The DOM document to be expanded.
	 */

	XSECNameSpaceExpander(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *d);			// Constructor

    /**
	 * \brief Fragment constructor
	 *
	 * Use this constructor to expand namespaces in a given fragment only.
	 * @note The fragment does not need to be rooted in an actual document.
	 *
	 * @param f The starting element of the fragment to be expanded.
	 */

	XSECNameSpaceExpander(XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *f);		    // frag Constructor

	~XSECNameSpaceExpander();						// Default destructor

	//@}

	// Operate 

	/**
	 * \brief Expand namespaces.
	 *
	 * Perform the expansion operation and create a list of all added nodes.
	 */

	void expandNameSpaces(void);

	/**
	 * \brief Collapse name-spaces
	 *
	 * Delete all namespaces added in exandNameSpaces() (using the list that
	 * was created at that time
	 */

	void deleteAddedNamespaces(void);

	// Check if a node is an added node
	bool nodeWasAdded(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *n);

private:  // Functions

	XSECNameSpaceExpander(void);					// No default constructor
	void recurse(XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *n);

	// data
	
	NameSpaceEntryListVectorType	m_lst;			// List of added name spaces
	XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument						
									* mp_doc;		// The owner document
	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement                      
									* mp_fragment;  // If we are doing a fragment
	bool							m_expanded;		// Have we expanded already?
	XSECSafeBufferFormatter			* mp_formatter;

};

#endif /* XSECNAMESPACEEXPANDER_HEADER */

