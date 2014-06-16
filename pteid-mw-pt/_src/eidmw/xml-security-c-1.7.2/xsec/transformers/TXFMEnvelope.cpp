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
 * TXFMEnvelope := Class that calculates an Envelope with an XPath evaluator
 *
 * $Id: TXFMEnvelope.cpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#include <xsec/transformers/TXFMEnvelope.hpp>
#include <xsec/framework/XSECException.hpp>
#include <xsec/utils/XSECDOMUtils.hpp>

#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/XMLUniDefs.hpp>

XERCES_CPP_NAMESPACE_USE


TXFMEnvelope::TXFMEnvelope(DOMDocument *doc) :
TXFMBase(doc) {


}

TXFMEnvelope::~TXFMEnvelope() {


}

// Methods to set the inputs

void TXFMEnvelope::setInput(TXFMBase *newInput) {

	input = newInput;

	if (newInput->getOutputType() != TXFMBase::DOM_NODES) {

		throw XSECException(XSECException::TransformInputOutputFail, "XPath requires DOM_NODES input type");

	}

	// Expand if necessary
	//this->expandNameSpaces();

	keepComments = input->getCommentsStatus();

	// Set up for the new document
	mp_document = input->getDocument();

	// Now work out what we have to set up in the new processing

	TXFMBase::nodeType inputType = input->getNodeType();


	switch (inputType) {

	case DOM_NODE_DOCUMENT :

		mp_startNode = mp_document;
		break;

	case DOM_NODE_DOCUMENT_FRAGMENT :

		mp_startNode = input->getFragmentNode();
		break;

	default :

		throw XSECException(XSECException::EnvelopeError);	// Should never get here

	}

	// Ready to evaluate

}


// Methods to get tranform output type and input requirement

TXFMBase::ioType TXFMEnvelope::getInputType(void) {

	return TXFMBase::DOM_NODES;

}

TXFMBase::ioType TXFMEnvelope::getOutputType(void) {

	return TXFMBase::DOM_NODES;

}

TXFMBase::nodeType TXFMEnvelope::getNodeType(void) {

	return TXFMBase::DOM_NODE_XPATH_NODESET;

}

// Envelope (and XPath) unique

void addEnvelopeNode(DOMNode *startNode, XSECXPathNodeList & XPathMap, DOMNode * sigNode) {

	XSEC_USING_XERCES(DOMNamedNodeMap);
	
	DOMNode *tmp;
	DOMNamedNodeMap *atts;
	XMLSize_t attsSize, i;
	
	if (startNode == sigNode)
		return;

	XPathMap.addNode(startNode);

	if (startNode->getNodeType() == DOMNode::ELEMENT_NODE) {

		atts = startNode->getAttributes();
		if (atts != NULL)
			attsSize = atts->getLength();
		else
			attsSize = 0;

		for (i = 0; i < attsSize; ++i) {

			tmp = atts->item(i);
			XPathMap.addNode(tmp);

		}

	}

	// Now do any childeren
	
	tmp = startNode->getFirstChild();

	while (tmp != NULL) {

		addEnvelopeNode(tmp, XPathMap, sigNode);
		tmp = tmp->getNextSibling();

	}
}

void addEnvelopeParentNSNodes(DOMNode *startNode, XSECXPathNodeList & XPathMap) {

	XSEC_USING_XERCES(DOMNamedNodeMap);
	
	DOMNode *tmp;
	DOMNamedNodeMap *atts;
	XMLSize_t attsSize, i;
	
	if (startNode == NULL)
		return;

	if (startNode->getNodeType() == DOMNode::ELEMENT_NODE) {

		atts = startNode->getAttributes();
		if (atts != NULL)
			attsSize = atts->getLength();
		else
			attsSize = 0;

		for (i = 0; i < attsSize; ++i) {

			tmp = atts->item(i);
			if (XMLString::compareNString(tmp->getNodeName(), DSIGConstants::s_unicodeStrXmlns, 5) == 0 &&
				(tmp->getNodeName()[5] == chNull || tmp->getNodeName()[5] == chColon))
				XPathMap.addNode(tmp);

		}

	}

	// Now do parent
	addEnvelopeParentNSNodes(startNode->getParentNode(), XPathMap);
	
}

void TXFMEnvelope::evaluateEnvelope(DOMNode *t) {

	DOMNode *sigNode;

	// Find the signature node
	sigNode = t->getParentNode();

	while (sigNode != NULL && !strEquals(getDSIGLocalName(sigNode), "Signature"))
		sigNode = sigNode->getParentNode();

	if (sigNode == NULL) {

		throw XSECException(XSECException::EnvelopeError,
			"Unable to find signature owner of node passed to Envelope Transform");

	}

	// Check if sigNode is an ancestor of mp_startNode - if so, just return
	DOMNode * c = mp_startNode;
	while (c != NULL) {

		if (c == sigNode)
			return;

		c = c->getParentNode();

	}

	addEnvelopeNode(mp_startNode, m_XPathMap, sigNode);
	addEnvelopeParentNSNodes(mp_startNode->getParentNode(), m_XPathMap);

}

// Methods to get output data

unsigned int TXFMEnvelope::readBytes(XMLByte * const toFill, unsigned int maxToFill) {

	return 0;

}

DOMDocument *TXFMEnvelope::getDocument() {

	return mp_document;

}

DOMNode *TXFMEnvelope::getFragmentNode() {

	return NULL;

}

const XMLCh * TXFMEnvelope::getFragmentId() {

	return NULL;	// Empty string

}

XSECXPathNodeList	& TXFMEnvelope::getXPathNodeList() {

	return m_XPathMap;

}
