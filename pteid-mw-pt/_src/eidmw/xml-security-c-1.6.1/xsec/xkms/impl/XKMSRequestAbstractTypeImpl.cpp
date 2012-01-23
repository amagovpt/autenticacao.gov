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
 * XKMSRequestAbstractTypeImpl := Implementation class for XKMS Request messages
 *
 * $Id: XKMSRequestAbstractTypeImpl.cpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

// XSEC Includes

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/framework/XSECError.hpp>
#include <xsec/framework/XSECEnv.hpp>
#include <xsec/xkms/XKMSConstants.hpp>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMNodeList.hpp>

#include "XKMSRequestAbstractTypeImpl.hpp"
#include "XKMSRespondWithImpl.hpp"
#include "XKMSResponseMechanismImpl.hpp"

XERCES_CPP_NAMESPACE_USE

// --------------------------------------------------------------------------------
//           Construct/Destruct
// --------------------------------------------------------------------------------

XKMSRequestAbstractTypeImpl::XKMSRequestAbstractTypeImpl(
	const XSECEnv * env) :
m_msg(env)
{
	mp_originalRequestIdAttr = NULL;
	mp_responseLimitAttr = NULL;
}

XKMSRequestAbstractTypeImpl::XKMSRequestAbstractTypeImpl(
	const XSECEnv * env, 
	DOMElement * node) :
m_msg(env, node)
{
	mp_originalRequestIdAttr = NULL;
	mp_responseLimitAttr = NULL;
}

XKMSRequestAbstractTypeImpl::~XKMSRequestAbstractTypeImpl() {

	RespondWithVectorType::iterator i;

	for (i = m_respondWithList.begin(); i != m_respondWithList.end(); ++i) {
		delete (*i);
	}

	ResponseMechanismVectorType::iterator j;

	for (j = m_responseMechanismList.begin(); j != m_responseMechanismList.end(); ++j) {
		delete (*j);
	}
};

// --------------------------------------------------------------------------------
//           Load
// --------------------------------------------------------------------------------

void XKMSRequestAbstractTypeImpl::load(void) {

	if (m_msg.mp_messageAbstractTypeElement == NULL) {

		// Attempt to load an empty element
		throw XSECException(XSECException::RequestAbstractTypeError,
			"XKMSRequestAbstractType::load - called on empty DOM");

	}

	// Get any respond with elements
	DOMNodeList * nl = m_msg.mp_messageAbstractTypeElement->getElementsByTagNameNS(
		XKMSConstants::s_unicodeStrURIXKMS,
		XKMSConstants::s_tagRespondWith);

	if (nl != NULL) {

		XKMSRespondWithImpl * rw;
		for (unsigned int i = 0; i < nl->getLength() ; ++ i) {

			XSECnew(rw, XKMSRespondWithImpl(m_msg.mp_env, (DOMElement *) nl->item(i)));
			rw->load();
			m_respondWithList.push_back(rw);

		}

	}

	// Get any ResponseMechanism elements
	nl = m_msg.mp_messageAbstractTypeElement->getElementsByTagNameNS(
		XKMSConstants::s_unicodeStrURIXKMS,
		XKMSConstants::s_tagResponseMechanism);

	if (nl != NULL) {

		XKMSResponseMechanismImpl * rm;
		for (unsigned int i = 0; i < nl->getLength() ; ++ i) {

			XSECnew(rm, XKMSResponseMechanismImpl(m_msg.mp_env, (DOMElement *) nl->item(i)));
			rm->load();
			m_responseMechanismList.push_back(rm);

		}

	}

	mp_originalRequestIdAttr = 
		m_msg.mp_messageAbstractTypeElement->getAttributeNodeNS(NULL, XKMSConstants::s_tagOriginalRequestId);

	mp_responseLimitAttr = 
		m_msg.mp_messageAbstractTypeElement->getAttributeNodeNS(NULL, XKMSConstants::s_tagResponseLimit);
	
		
	m_msg.load();

}

// --------------------------------------------------------------------------------
//           Create from scratch
// --------------------------------------------------------------------------------

DOMElement * XKMSRequestAbstractTypeImpl::createBlankRequestAbstractType(
		const XMLCh * tag,
		const XMLCh * service,
		const XMLCh * id) {

	return m_msg.createBlankMessageAbstractType(tag, service, id);

}

// --------------------------------------------------------------------------------
//           Get/Set interface methods
// --------------------------------------------------------------------------------

const XMLCh * XKMSRequestAbstractTypeImpl::getOriginalRequestId(void) const {

	if (mp_originalRequestIdAttr != NULL)
		return mp_originalRequestIdAttr->getNodeValue();

	return NULL;
}

void XKMSRequestAbstractTypeImpl::setOriginalRequestId(const XMLCh * id) {
	
	if (m_msg.mp_messageAbstractTypeElement == NULL) {

		// Attempt update when not initialised
		throw XSECException(XSECException::MessageAbstractTypeError,
			"XKMSRequestAbstractType::setOriginalRequestId - called on non-initialised structure");

	}

	m_msg.mp_messageAbstractTypeElement->setAttributeNS(NULL, XKMSConstants::s_tagOriginalRequestId, id);
	mp_originalRequestIdAttr = 
		m_msg.mp_messageAbstractTypeElement->getAttributeNodeNS(NULL, XKMSConstants::s_tagOriginalRequestId);

}

unsigned int XKMSRequestAbstractTypeImpl::getResponseLimit(void) const {

	if (mp_responseLimitAttr == NULL)
		return 0;

	unsigned int ret;

	if (!XMLString::textToBin(mp_responseLimitAttr->getValue(), ret))
		return 0;

	return ret;
}

void XKMSRequestAbstractTypeImpl::setResponseLimit(unsigned int limit) {

	if (m_msg.mp_messageAbstractTypeElement == NULL) {

		// Attempt update when not initialised
		throw XSECException(XSECException::MessageAbstractTypeError,
			"XKMSRequestAbstractType::setResponseLimit - called on non-initialised structure");

	}

	/* Convert the number to a string */
	XMLCh limitStr[10];
	XMLString::binToText(limit, limitStr, 9, 10);

	m_msg.mp_messageAbstractTypeElement->setAttributeNS(NULL, XKMSConstants::s_tagResponseLimit, limitStr);
	mp_responseLimitAttr = 
		m_msg.mp_messageAbstractTypeElement->getAttributeNodeNS(NULL, XKMSConstants::s_tagResponseLimit);

}

// --------------------------------------------------------------------------------
//           RespondWith handling
// --------------------------------------------------------------------------------

int XKMSRequestAbstractTypeImpl::getRespondWithSize(void) {

	return (int) m_respondWithList.size();

}

XKMSRespondWith * XKMSRequestAbstractTypeImpl::getRespondWithItem(int item) {

	if (item < 0 || item >= (int) m_respondWithList.size()) {

		throw XSECException(XSECException::RequestAbstractTypeError,
			"XKMSRequestAbstractTypeImpl::getRespondWithItem - item out of range");

	}

	return m_respondWithList[item];

}

const XMLCh * XKMSRequestAbstractTypeImpl::getRespondWithItemStr(int item) {

	if (item < 0 || item >= (int) m_respondWithList.size()) {

		throw XSECException(XSECException::RequestAbstractTypeError,
			"XKMSRequestAbstractTypeImpl::getRespondWithItem - item out of range");

	}

	return m_respondWithList[item]->getRespondWithString();

}

void XKMSRequestAbstractTypeImpl::appendRespondWithItem(XKMSRespondWith * item) {

}

void XKMSRequestAbstractTypeImpl::appendRespondWithItem(const XMLCh * item) {

	XKMSRespondWithImpl * rw;
	XSECnew(rw, XKMSRespondWithImpl(m_msg.mp_env));

	// Create the RespondWith object
	DOMElement * elt = rw->createBlankRespondWith(item);

	// Add to the item
	DOMElement * c = findFirstElementChild(m_msg.mp_messageAbstractTypeElement);
	while (c != NULL) {

		if (!strEquals(getXKMSLocalName(c), XKMSConstants::s_tagResponseMechanism))
			break;

	}

	if (c != NULL) {
		m_msg.mp_messageAbstractTypeElement->insertBefore(elt, c);
		if (m_msg.mp_env->getPrettyPrintFlag()) {
			m_msg.mp_messageAbstractTypeElement->insertBefore(
				m_msg.mp_env->getParentDocument()->createTextNode(DSIGConstants::s_unicodeStrNL), c);
		}
	}
	else {
		m_msg.mp_messageAbstractTypeElement->appendChild(elt);
		m_msg.mp_env->doPrettyPrint(m_msg.mp_messageAbstractTypeElement);
	}

	// Add to the list
	m_respondWithList.push_back(rw);

}


// --------------------------------------------------------------------------------
//           ResponseMechanism handling
// --------------------------------------------------------------------------------

int XKMSRequestAbstractTypeImpl::getResponseMechanismSize(void) {

	return (int) m_responseMechanismList.size();

}

XKMSResponseMechanism * XKMSRequestAbstractTypeImpl::getResponseMechanismItem(int item) {

	if (item < 0 || item >= (int) m_responseMechanismList.size()) {

		throw XSECException(XSECException::RequestAbstractTypeError,
			"XKMSRequestAbstractTypeImpl::getResponseMechanismItem - item out of range");

	}

	return m_responseMechanismList[item];

}

const XMLCh * XKMSRequestAbstractTypeImpl::getResponseMechanismItemStr(int item) {

	if (item < 0 || item >= (int) m_responseMechanismList.size()) {

		throw XSECException(XSECException::RequestAbstractTypeError,
			"XKMSRequestAbstractTypeImpl::getResponseMechanismItem - item out of range");

	}

	return m_responseMechanismList[item]->getResponseMechanismString();

}

void XKMSRequestAbstractTypeImpl::appendResponseMechanismItem(XKMSResponseMechanism * item) {

}

void XKMSRequestAbstractTypeImpl::appendResponseMechanismItem(const XMLCh * item) {

	XKMSResponseMechanismImpl * rw;
	XSECnew(rw, XKMSResponseMechanismImpl(m_msg.mp_env));

	// Create the ResponseMechanism object
	DOMElement * elt = rw->createBlankResponseMechanism(item);

	// Add to the item
	DOMElement * c = findFirstElementChild(m_msg.mp_messageAbstractTypeElement);
	while (c != NULL) {

		if (!strEquals(getXKMSLocalName(c), XKMSConstants::s_tagResponseMechanism))
			break;

		c = findNextElementChild(c);

	}

	if (c != NULL) {
		m_msg.mp_messageAbstractTypeElement->insertBefore(elt, c);
		if (m_msg.mp_env->getPrettyPrintFlag()) {
			m_msg.mp_messageAbstractTypeElement->insertBefore(
				m_msg.mp_env->getParentDocument()->createTextNode(DSIGConstants::s_unicodeStrNL), c);
		}
	}
	else {
		m_msg.mp_messageAbstractTypeElement->appendChild(elt);
		m_msg.mp_env->doPrettyPrint(m_msg.mp_messageAbstractTypeElement);
	}

	// Add to the list
	m_responseMechanismList.push_back(rw);

}

void XKMSRequestAbstractTypeImpl::removeResponseMechanismItem(int item) {

	if (item < 0 || item >= (int) m_responseMechanismList.size()) {

		throw XSECException(XSECException::RequestAbstractTypeError,
			"XKMSRequestAbstractTypeImpl::getResponseMechanismItem - item out of range");

	}

	XKMSResponseMechanism *rm = m_responseMechanismList[item];
	DOMNode * m = rm->getElement();
	m->getParentNode()->removeChild(m);
	m->release();

	// Now clean up our structure.
	ResponseMechanismVectorType::iterator i;
	int j = 0;
	i = m_responseMechanismList.begin();
	while (j < item) {
		j++;
		i++;
	}
	m_responseMechanismList.erase(i);

}


