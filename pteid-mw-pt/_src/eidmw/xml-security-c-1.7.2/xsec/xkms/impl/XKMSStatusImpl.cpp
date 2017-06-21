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
 * XKMSStatusImpl := Implementation for Status elements (in KeyBinding)
 *
 * $Id: XKMSStatusImpl.cpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

// XSEC Includes

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/framework/XSECError.hpp>
#include <xsec/framework/XSECEnv.hpp>
#include <xsec/utils/XSECDOMUtils.hpp>
#include <xsec/xkms/XKMSConstants.hpp>

#include "XKMSStatusImpl.hpp"

#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/XMLUniDefs.hpp>

XERCES_CPP_NAMESPACE_USE

// --------------------------------------------------------------------------------
//           Construct/Destruct
// --------------------------------------------------------------------------------

XKMSStatusImpl::XKMSStatusImpl(
		const XSECEnv * env) :
mp_env(env),
mp_statusElement(NULL) {

	m_statusValue = XKMSStatus::StatusUndefined;

	StatusValue i;
	StatusReason j;

	/* Clean out status codes */
	for (i = Indeterminate ; i > StatusUndefined; i = (StatusValue)(i-1)) {
		for (j = XKMSStatus::Signature; j != XKMSStatus::ReasonUndefined; j = (XKMSStatus::StatusReason) (j-1)) {
	
			m_statusReasons[i-1][j-1] = NULL;

		}
	}

}

XKMSStatusImpl::XKMSStatusImpl(
		const XSECEnv * env, 
		XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * node) :
mp_env(env),
mp_statusElement(node) {

	m_statusValue = XKMSStatus::StatusUndefined;

	StatusValue i;
	StatusReason j;

	/* Clean out status codes */
	for (i = Indeterminate ; i > StatusUndefined; i = (StatusValue)(i-1)) {
		for (j = XKMSStatus::Signature; j != XKMSStatus::ReasonUndefined; j = (XKMSStatus::StatusReason) (j-1)) {
	
			m_statusReasons[i-1][j-1] = NULL;

		}
	}

}

XKMSStatusImpl::~XKMSStatusImpl() {


}

// --------------------------------------------------------------------------------
//           Load
// --------------------------------------------------------------------------------

void XKMSStatusImpl::load() {

	if (mp_statusElement == NULL) {

		// Attempt to load an empty element
		throw XSECException(XSECException::StatusError,
			"XKMSStatus::load - called on empty DOM");

	}

	/* Load the StatusValue attribute */

	mp_statusValueAttr = 
		mp_statusElement->getAttributeNodeNS(NULL, XKMSConstants::s_tagStatusValue);

	/* Decode as appropriate*/
	if (mp_statusValueAttr == NULL) {
		// Attempt to load an empty element
		throw XSECException(XSECException::StatusError,
			"XKMSStatus::load - No StatusValue attribute found");
	}

	const XMLCh * res = mp_statusValueAttr->getNodeValue();

	// This is actually an QName, but we cheat and find the ':' character by hand
	// without actually checking the qualifier.
	// TODO - CHECK the qualifier.

	int res2 = XMLString::indexOf(res, chPound);
	if (res2 == -1 || XMLString::compareNString(res, XKMSConstants::s_unicodeStrURIXKMS, res2)) {
			throw XSECException(XSECException::StatusError,
				"XKMSStatus::load - StatusValue not in XKMS Name Space");
	}
	res = &res[res2+1];

	for (m_statusValue = XKMSStatus::Indeterminate; 
		m_statusValue > XKMSStatus::StatusUndefined; 
		m_statusValue = (XKMSStatus::StatusValue) (m_statusValue-1)) {

		if (strEquals(XKMSConstants::s_tagStatusValueCodes[m_statusValue], res))
			break;

		
	}

	if (m_statusValue == XKMSStatus::StatusUndefined) {

		throw XSECException(XSECException::StatusError,
			"XKMSStatus::load - Unknown StatusValue provided");

	}

	/* Now we need to run through the status values */
	DOMElement * e = findFirstElementChild(mp_statusElement);
	while (e != NULL) {

		const XMLCh * vs = getXKMSLocalName(e);

		StatusValue v;

		if (strEquals(vs, XKMSConstants::s_tagValidReason)) {
			v = Valid;
		}
		else if (strEquals(vs, XKMSConstants::s_tagInvalidReason)) {
			v = Invalid;
		}
		else if (strEquals(vs, XKMSConstants::s_tagIndeterminateReason)) {
			v = Indeterminate;
		}
		else {
			throw XSECException(XSECException::StatusError,
				"XKMSStatus::load - Unknown Reason element");
		}

		DOMNode *t = findFirstChildOfType(e, DOMNode::TEXT_NODE);
		if (t == NULL) {
			throw XSECException(XSECException::StatusError,
				"XKMSStatus::load - Expected text node child of reason element");
		}

		/* Strip out the URI prefix */
		const XMLCh * reason = t->getNodeValue();

		int res = XMLString::indexOf(reason, chPound);
		if (res == -1 || XMLString::compareNString(reason, XKMSConstants::s_unicodeStrURIXKMS, res)) {
				throw XSECException(XSECException::StatusError,
					"XKMSStatus::load - StatusReason not in XKMS Name Space");
		}
		reason = &reason[res+1];

		/* Found out what we are! */
		XKMSStatus::StatusReason i;
		for (i = XKMSStatus::Signature; i != XKMSStatus::ReasonUndefined; i = (XKMSStatus::StatusReason) (i-1)) {

			if (strEquals(XKMSConstants::s_tagStatusReasonCodes[i], reason))
				break;

		}

		if (i == XKMSStatus::ReasonUndefined) {
			throw XSECException(XSECException::StatusError,
				"XKMSStatus::load - Unknown StatusReason");
		}

		m_statusReasons[v-1][i-1] = e;

		e = findNextElementChild(e);

	}

}

// --------------------------------------------------------------------------------
//           Create Blank Status
// --------------------------------------------------------------------------------

DOMElement * XKMSStatusImpl::createBlankStatus(StatusValue status) {

	// Get some setup values
	safeBuffer str;
	DOMDocument *doc = mp_env->getParentDocument();
	const XMLCh * prefix = mp_env->getXKMSNSPrefix();

	makeQName(str, prefix, XKMSConstants::s_tagStatus);

	mp_statusElement = doc->createElementNS(XKMSConstants::s_unicodeStrURIXKMS, 
												str.rawXMLChBuffer());

	mp_env->doPrettyPrint(mp_statusElement);

	/* Now add the StatusValue element */

	str.sbXMLChIn(XKMSConstants::s_unicodeStrURIXKMS);
	str.sbXMLChCat(XKMSConstants::s_tagStatusValueCodes[status]);

	mp_statusElement->setAttributeNS(NULL, 
		XKMSConstants::s_tagStatusValue,
		str.rawXMLChBuffer());

	mp_statusValueAttr = 
		mp_statusElement->getAttributeNodeNS(NULL, XKMSConstants::s_tagStatusValue);

	m_statusValue = status;

	return mp_statusElement;

}

// --------------------------------------------------------------------------------
//           Get methods
// --------------------------------------------------------------------------------

	/* Getter Interface Methods */
XKMSStatus::StatusValue XKMSStatusImpl::getStatusValue(void) const {

	return m_statusValue;

}

bool XKMSStatusImpl::getStatusReason(StatusValue status, StatusReason reason) const {

	if (status == StatusUndefined || reason == ReasonUndefined) {
		throw XSECException(XSECException::StatusError,
			"XKMSStatus::getStatusReason - status or reason undefined");
	}

	return (m_statusReasons[status-1][reason-1] != NULL);

}


// --------------------------------------------------------------------------------
//           Set methods
// --------------------------------------------------------------------------------

void XKMSStatusImpl::setStatusReason(StatusValue status, StatusReason reason, bool value) {

	if (status == StatusUndefined || reason == ReasonUndefined) {
		throw XSECException(XSECException::StatusError,
			"XKMSStatus::setStatusReason - status or reason undefined");
	}

	if ((m_statusReasons[status-1][reason-1] != NULL) == value)
		return;

	/* Delete if necessary */
	if (value == false) {

		DOMNode * c = m_statusReasons[status-1][reason-1];
		mp_statusElement->removeChild(c);
		c->release();
		m_statusReasons[status-1][reason-1] = NULL;
		return;

	}

		
	/* Find a previously found element to insert before*/
	DOMNode * found = NULL;

	m_statusValue = XKMSStatus::StatusUndefined;

	StatusValue i;
	StatusReason j = XKMSStatus::Signature;

	/* Clean out status codes */
	for (i = Indeterminate ; i > StatusUndefined && !(i == status && j == reason); i = (StatusValue)(i-1)) {
		for (j = XKMSStatus::Signature; j != XKMSStatus::ReasonUndefined && !(i == status && j == reason); j = (XKMSStatus::StatusReason) (j-1)) {
	
			if (m_statusReasons[i-1][j-1] != NULL)
				found = m_statusReasons[i-1][j-1];

		}
	}

	/* Now lets create our new element and its text child */
	safeBuffer str;
	DOMDocument *doc = mp_env->getParentDocument();
	// str.sbXMLChIn(XKMSConstants::s_unicodeStrURIXKMS);
	const XMLCh * prefix = mp_env->getXKMSNSPrefix();

	if (status == Valid) {
		makeQName(str, prefix, XKMSConstants::s_tagValidReason);
	}
	else if (status == Invalid) {
		makeQName(str, prefix, XKMSConstants::s_tagInvalidReason);
	}
	else {
		makeQName(str, prefix, XKMSConstants::s_tagIndeterminateReason);
	}

	DOMElement * e = doc->createElementNS(XKMSConstants::s_unicodeStrURIXKMS, 
												str.rawXMLChBuffer());

	/* Create the text node child */
	str.sbXMLChIn(XKMSConstants::s_unicodeStrURIXKMS);
	str.sbXMLChCat(XKMSConstants::s_tagStatusReasonCodes[reason]);
	e->appendChild(doc->createTextNode(str.rawXMLChBuffer()));

	/* Insert at correct place */
	if (found == NULL) {

		mp_statusElement->appendChild(e);
		mp_env->doPrettyPrint(mp_statusElement);

	}
	else {

		mp_statusElement->insertBefore(e, found);
		if (mp_env->getPrettyPrintFlag() == true)
			mp_statusElement->insertBefore(doc->createTextNode(DSIGConstants::s_unicodeStrNL), found);

	}

	m_statusReasons[status-1][reason-1] = e;

}
