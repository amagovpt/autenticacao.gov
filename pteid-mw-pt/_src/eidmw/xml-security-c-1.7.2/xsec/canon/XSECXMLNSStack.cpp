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
 * $Id: XSECXMLNSStack.cpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */


// XSEC
#include <xsec/framework/XSECDefs.hpp>
#include <xsec/framework/XSECError.hpp>
#include <xsec/utils/XSECDOMUtils.hpp>
#include <xsec/canon/XSECXMLNSStack.hpp>

// Xerces

XERCES_CPP_NAMESPACE_USE

// --------------------------------------------------------------------------------
//           Holder structures
// --------------------------------------------------------------------------------

typedef struct XSECNSHolderStruct {

	XERCES_CPP_NAMESPACE_QUALIFIER DOMNode	* mp_ns;		// Actual NS attribute
	XERCES_CPP_NAMESPACE_QUALIFIER DOMNode	* mp_owner;		// Owner Element

	struct XSECNSHolderStruct				* mp_hides;		// WHat does this NS hide?
	struct XSECNSHolderStruct				* mp_next;		// Next in list

	XERCES_CPP_NAMESPACE_QUALIFIER DOMNode	* mp_printed;	// Node at which it was printed

	bool									m_isDefault;	// Is this a default NS?

} XSECNSHolder;

typedef struct XSECNSElementStruct {

	XERCES_CPP_NAMESPACE_QUALIFIER DOMNode	* mp_elt;		// Element
	struct XSECNSHolderStruct				* mp_firstNS;	// WHat does this NS hide?

} XSECNSElement;

// --------------------------------------------------------------------------------
//           Construct/Destruct
// --------------------------------------------------------------------------------


XSECXMLNSStack::XSECXMLNSStack() {

}

XSECXMLNSStack::~XSECXMLNSStack() {

	// Need to iterate through the Stack to 
	XSECNSElement * t;
	XSECNSHolder * u, *v;
	
	while (m_elements.size() > 0) {
		t = m_elements.top();
		u = t->mp_firstNS;
		while (u != NULL) {
			v = u->mp_next;
			delete u;
			u = v;
		}
		delete t;
		m_elements.pop();
	}
}


// --------------------------------------------------------------------------------
//           Stack Functions
// --------------------------------------------------------------------------------
void XSECXMLNSStack::pushElement(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode * elt) {

	XSECNSElement * t;
	XSECnew(t, XSECNSElement);

	t->mp_elt = elt;
	t->mp_firstNS = NULL;

	m_elements.push(t);

}

void XSECXMLNSStack::popElement() {

	// First - are there any namespaces to remove from the currently visible list?
	XSECNSHolder * t, *u;
	XSECNSElement * e = m_elements.top();

	// Iterate through the current namespaces vector
	XSECNSHolderVectorType::iterator it = m_currentNS.begin();

	while (it != m_currentNS.end()) {

		t = *it;
		if (t->mp_owner == e->mp_elt) {

			// Need to delete this
			m_currentNS.erase(it);
			if (t->mp_hides != NULL) {
				m_currentNS.push_back(t->mp_hides);
			}
			// TODO - Fix this, it is naieve and slow
			it = m_currentNS.begin();

		}
		else {
			if (t->mp_printed == e->mp_elt)
				t->mp_printed = NULL;
			it++;
		}

	}

	// OK - Now we delete the NS nodes
	t = e->mp_firstNS;
	while (t != NULL) {
		u = t->mp_next;
		delete t;
		t = u;
	}

	// Now delete the element holder itself
	m_elements.pop();
	delete e;

}


// --------------------------------------------------------------------------------
//           NS Addition
// --------------------------------------------------------------------------------

void XSECXMLNSStack::addNamespace(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode * ns) {

	// Create the new entry for this node
	XSECNSHolder * t, *u;
	XSECnew(t, XSECNSHolder);

	t->mp_hides = NULL;
	t->mp_next = NULL;
	t->mp_ns = ns;
	t->mp_owner = (m_elements.top())->mp_elt;
	t->mp_printed = NULL;

	t->m_isDefault = strEquals(ns->getNodeName(), DSIGConstants::s_unicodeStrXmlns);

	// Does this hide something in the current namespace list?
	XSECNSHolderVectorType::iterator it = m_currentNS.begin();

	while (it != m_currentNS.end()) {
		u = (*it);
		if (strEquals(u->mp_ns->getNodeName(), ns->getNodeName())) {

			// This hides a current NS entry
			t->mp_hides = u;
			m_currentNS.erase(it);
			it = m_currentNS.end();
		}
		else
			it++;
	}

	// Now push it onto the namespace vector
	m_currentNS.push_back(t);

	// Add me to the current element's namespaces
	XSECNSElement * e = m_elements.top();
	
	t->mp_next = e->mp_firstNS;
	e->mp_firstNS = t;

}

void XSECXMLNSStack::printNamespace(DOMNode * ns, DOMNode * elt) {

	XSECNSHolder * t;
	XSECNSHolderVectorType::iterator it = m_currentNS.begin();

	while (it != m_currentNS.end()) {
		t = (*it);
		// Fix for bug#47353, go ahead and track printing of default namespaces.
		if (t->mp_ns == ns) { //  && t->m_isDefault == false) {
			t->mp_printed = elt;
			break;
		}
		it++;
	}
}

// --------------------------------------------------------------------------------
//           NS Searching
// --------------------------------------------------------------------------------

DOMNode * XSECXMLNSStack::getFirstNamespace(void) {

	m_currentNSIterator = m_currentNS.begin();

	while (m_currentNSIterator != m_currentNS.end() && 
		(*m_currentNSIterator)->mp_printed != NULL)
		m_currentNSIterator++;

	if (m_currentNSIterator != m_currentNS.end())
		return (*m_currentNSIterator)->mp_ns;

	return NULL;

}

DOMNode * XSECXMLNSStack::getNextNamespace(void) {
	if (m_currentNSIterator == m_currentNS.end()) 
		return NULL;
	m_currentNSIterator++;
	while (m_currentNSIterator != m_currentNS.end() && 
		(*m_currentNSIterator)->mp_printed != NULL)
		m_currentNSIterator++;

	if (m_currentNSIterator == m_currentNS.end()) 
		return NULL;

	return (*m_currentNSIterator)->mp_ns;

}

// Fix for bug#47353, explicit check for non-empty default NS decl.
bool XSECXMLNSStack::isNonEmptyDefaultNS(void) {
    for (XSECNSHolderVectorType::iterator it = m_currentNS.begin(); it != m_currentNS.end(); ++it) {
        if ((*it)->m_isDefault) {
            const XMLCh* val = (*it)->mp_ns->getNodeValue();
            if (val && *val)
                return true;
        }
    }
    return false;
}
