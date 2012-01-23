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
 * XSECXMLNSStack := NS Stack for simple canonicalisation
 *
 * $Id: XSECXMLNSStack.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef XSECXMLNSSTACK_INCLUDE
#define XSECXMLNSSTACK_INCLUDE

// XSEC
#include <xsec/framework/XSECDefs.hpp>

// Xerces

XSEC_DECLARE_XERCES_CLASS(DOMNode)

// General

#include <vector>
#include <stack>

// --------------------------------------------------------------------------------
//           Holder structures
// --------------------------------------------------------------------------------

struct XSECNSHolderStruct;
struct XSECNSElementStruct;

typedef struct XSECNSHolderStruct XSECNSHolder;
typedef struct XSECNSElementStruct XSECNSElement;

#if defined(XSEC_NO_NAMESPACES)
	typedef vector<XSECNSHolder *>				XSECNSHolderVectorType;
	typedef stack<XSECNSElement *>              XSECNSElementStackType;
#else
	typedef std::vector<XSECNSHolder *>			XSECNSHolderVectorType;
	typedef std::stack<XSECNSElement *>         XSECNSElementStackType;
#endif


// --------------------------------------------------------------------------------
//           The stack
// --------------------------------------------------------------------------------


class XSECXMLNSStack {

public:

	XSECXMLNSStack();
	~XSECXMLNSStack();

	// Stack functions
	void pushElement(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode * elt);
	void popElement();

	// NS Functions
	void addNamespace(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode * ns);
	XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *
		getFirstNamespace(void);
	XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *
		getNextNamespace(void);
	// Mark the namespace as printed
	void printNamespace(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode * ns, 
		XERCES_CPP_NAMESPACE_QUALIFIER DOMNode * elt);
	// Return whether the stack includes xmlns="something"
	bool isNonEmptyDefaultNS(void);

private:

	// The stack holds all the elements, with a linked list of NS nodes
	// THat were pushed in for each element
	XSECNSElementStackType m_elements;
	// The vector holds all the "currently visible" namespaces for this element
	XSECNSHolderVectorType m_currentNS;
	// Hold a pointer into the visible namespace list for the current element
	XSECNSHolderVectorType::iterator m_currentNSIterator;

};


#endif /* XSECXMLNSSTACK_INCLUDE */

