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
 * XSECNameSpaceExander := Class for expanding out a document's name space axis
 *							and then shrinking again
 *
 * $Id: XSECNameSpaceExpander.cpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

// XSEC Includes
#include <xsec/utils/XSECNameSpaceExpander.hpp>
#include <xsec/dsig/DSIGConstants.hpp>
#include <xsec/framework/XSECError.hpp>
#include <xsec/utils/XSECDOMUtils.hpp>

XERCES_CPP_NAMESPACE_USE

XSECNameSpaceExpander::XSECNameSpaceExpander(DOMDocument *d) {

	mp_doc = d;
	mp_fragment = d->getDocumentElement();
	XSECnew(mp_formatter, XSECSafeBufferFormatter("UTF-8",XMLFormatter::NoEscapes, 
												XMLFormatter::UnRep_CharRef));

	m_expanded = false;
	
}

XSECNameSpaceExpander::XSECNameSpaceExpander(DOMElement *f) {

	mp_doc = NULL;
	mp_fragment = f;
	XSECnew(mp_formatter, XSECSafeBufferFormatter("UTF-8",XMLFormatter::NoEscapes, 
												XMLFormatter::UnRep_CharRef));

	m_expanded = false;
	
}

XSECNameSpaceExpander::~XSECNameSpaceExpander() {

	if (mp_formatter != NULL)
		delete mp_formatter;

}

void XSECNameSpaceExpander::recurse(DOMElement *n) {

	// Recursively go down the tree adding namespaces

	DOMNode *p = n->getParentNode();
	if (p->getNodeType() != DOMNode::ELEMENT_NODE)
		return;

	DOMNamedNodeMap *pmap = p->getAttributes();
	XMLSize_t psize = pmap->getLength();

	DOMNamedNodeMap *nmap = n->getAttributes();

	safeBuffer pname, pURI, nURI;
	DOMNode *finder;

	XSECNameSpaceEntry * tmpEnt;

	for (XMLSize_t i = 0; i < psize; i++) {

		// Run through each parent node to find namespaces
		pname << (*mp_formatter << pmap->item(i)->getNodeName());

		// See if this is an xmlns node
		
		if (pname.sbStrncmp("xmlns", 5) == 0) {

			// It is - see if it already exists
			finder = nmap->getNamedItem(pname.sbStrToXMLCh());
			if (finder == 0) {

				// Need to add
				n->setAttributeNS(DSIGConstants::s_unicodeStrURIXMLNS, 
					pmap->item(i)->getNodeName(),
					pmap->item(i)->getNodeValue());

				// Add it to the list so it can be removed later
				XSECnew(tmpEnt, XSECNameSpaceEntry);
				tmpEnt->m_name.sbStrcpyIn(pname);
				tmpEnt->mp_node = n;
				tmpEnt->mp_att = nmap->getNamedItem(pname.sbStrToXMLCh());
				m_lst.push_back(tmpEnt);

			}

		}

	}

	// Do the children

	DOMNode *c;

	c = n->getFirstChild();

	while (c != NULL) {
		if (c->getNodeType() == DOMNode::ELEMENT_NODE)
			recurse((DOMElement *) c);
		c = c->getNextSibling();
	}

}

int attNodeCount(DOMElement * d) {

	XMLSize_t ret;

	ret = d->getAttributes()->getLength();

	DOMNode *c;

	c = d->getFirstChild();

	while (c != NULL) {

		if (c->getNodeType() == DOMNode::ELEMENT_NODE)
			ret += attNodeCount((DOMElement *) c);

		c = c->getNextSibling();

	}

	return (int) ret;

}

void XSECNameSpaceExpander::expandNameSpaces(void) {

	if (m_expanded)
		return;				// Don't do this twice!

	DOMElement	*docElt;		// The document element - do not expand it's namespaces
	
	docElt = mp_fragment; //mp_doc->getDocumentElement();
	int count = attNodeCount(docElt);

	DOMNode *c;

	c = docElt->getFirstChild();

	while (c != NULL) {
		if (c->getNodeType() == DOMNode::ELEMENT_NODE)
			recurse((DOMElement *) c);
		c = c->getNextSibling();
	}

	m_expanded = true;

	count = attNodeCount(docElt);

}


void XSECNameSpaceExpander::deleteAddedNamespaces(void) {

	NameSpaceEntryListVectorType::size_type size = m_lst.size();
	XSECNameSpaceEntry *e;

	DOMElement *docElt = mp_fragment; //mp_doc->getDocumentElement();
	int 	count = attNodeCount(docElt);

	NameSpaceEntryListVectorType::size_type i;

	for (i = 0; i < size; ++i) {

		// Delete the element attribute, and then this node
		e = m_lst[i];
		if (e->m_name[5] == ':')
			e->mp_node->removeAttributeNS(DSIGConstants::s_unicodeStrURIXMLNS,
										MAKE_UNICODE_STRING((char *) &((e->m_name.rawBuffer())[6])));
		else
			e->mp_node->removeAttributeNS(DSIGConstants::s_unicodeStrURIXMLNS,
										MAKE_UNICODE_STRING((char *) e->m_name.rawBuffer()));

		// Delete the entry
		delete e;

	}

	// Now done - empty everything
	m_lst.clear();
	m_expanded = false;
	count = attNodeCount(docElt);

}

bool XSECNameSpaceExpander::nodeWasAdded(DOMNode *n) {

	NameSpaceEntryListVectorType::size_type size = m_lst.size();
	XSECNameSpaceEntry *e;

	NameSpaceEntryListVectorType::size_type i;
	for (i = 0; i < size; ++i) {

		// Delete the element attribute, and then this node
		e = m_lst[i];
		
		if (e->mp_att == n)
			return true;

	}

	return false;

}
