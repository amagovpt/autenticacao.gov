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
 * XKMSKeyBindingAbstractTypeImpl := Implementation of base for KeyBinding elements
 *
 * $Id: XKMSKeyBindingAbstractTypeImpl.cpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/framework/XSECError.hpp>
#include <xsec/framework/XSECEnv.hpp>
#include <xsec/xkms/XKMSConstants.hpp>
#include <xsec/utils/XSECDOMUtils.hpp>
#include <xsec/dsig/DSIGKeyInfoList.hpp>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/XMLUniDefs.hpp>
#include <xercesc/util/Janitor.hpp>

#include "XKMSKeyBindingAbstractTypeImpl.hpp"
#include "XKMSUseKeyWithImpl.hpp"

XERCES_CPP_NAMESPACE_USE

// --------------------------------------------------------------------------------
//           Constructor/Destructor
// --------------------------------------------------------------------------------

XKMSKeyBindingAbstractTypeImpl::XKMSKeyBindingAbstractTypeImpl(
		const XSECEnv * env) :
mp_keyBindingAbstractTypeElement(NULL),
mp_env(env),
mp_idAttr(NULL),
mp_keyUsageSignatureElement(NULL),
mp_keyUsageEncryptionElement(NULL),
mp_keyUsageExchangeElement(NULL),
mp_keyInfoElement(NULL) {

	XSECnew(mp_keyInfoList, DSIGKeyInfoList(mp_env));

}

XKMSKeyBindingAbstractTypeImpl::XKMSKeyBindingAbstractTypeImpl(
		const XSECEnv * env, 
		XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * node) :
mp_keyBindingAbstractTypeElement(node),
mp_env(env),
mp_idAttr(NULL),
mp_keyUsageSignatureElement(NULL),
mp_keyUsageEncryptionElement(NULL),
mp_keyUsageExchangeElement(NULL),
mp_keyInfoElement(NULL) {

	XSECnew(mp_keyInfoList, DSIGKeyInfoList(mp_env));

}

XKMSKeyBindingAbstractTypeImpl::~XKMSKeyBindingAbstractTypeImpl() {

	if (mp_keyInfoList != NULL)
		delete mp_keyInfoList;

	UseKeyWithVectorType::iterator i;
	for (i = m_useKeyWithList.begin(); i != m_useKeyWithList.end(); ++i)
		delete (*i);

}

// --------------------------------------------------------------------------------
//           Load from DOM
// --------------------------------------------------------------------------------

void XKMSKeyBindingAbstractTypeImpl::load(void) {

	if (mp_keyBindingAbstractTypeElement == NULL) {
		throw XSECException(XSECException::ExpectedXKMSChildNotFound,
			"XKMSKeyBindingAbstractTypeImpl::load - called on empty DOM");
	}

	// Id
	mp_idAttr = 
		mp_keyBindingAbstractTypeElement->getAttributeNodeNS(NULL, XKMSConstants::s_tagId);

	DOMElement * tmpElt = findFirstElementChild(mp_keyBindingAbstractTypeElement);

	if (tmpElt != NULL && strEquals(getDSIGLocalName(tmpElt), XKMSConstants::s_tagKeyInfo)) {

		if (mp_keyInfoList != NULL)
			delete mp_keyInfoList;

		XSECnew(mp_keyInfoList, DSIGKeyInfoList(mp_env));

		mp_keyInfoList->loadListFromXML(tmpElt);
		mp_keyInfoElement = tmpElt;

		tmpElt = findNextElementChild(tmpElt);

	}

	while (tmpElt != NULL && strEquals(getXKMSLocalName(tmpElt), XKMSConstants::s_tagKeyUsage)) {

		DOMNode * txt = findFirstChildOfType(tmpElt, DOMNode::TEXT_NODE);

		if (txt == NULL) {
			throw XSECException(XSECException::ExpectedXKMSChildNotFound,
				"XKMSKeyBindingAbstractTypeImpl::load - Require text node beneath <KeyUsage>");
		}

		const XMLCh * usageStr = txt->getNodeValue();
		int index = XMLString::indexOf(usageStr, chPound);
		if (index == -1 || XMLString::compareNString(usageStr, XKMSConstants::s_unicodeStrURIXKMS, index)) {
			throw XSECException(XSECException::XKMSError,
				"XKMSResultType::load - KeyUsage not in XKMS Name Space");
		}

		usageStr = &usageStr[index+1];


		if (strEquals(usageStr, XKMSConstants::s_tagEncryption)) {
			mp_keyUsageEncryptionElement = tmpElt;
		} else 	if (strEquals(usageStr, XKMSConstants::s_tagExchange)) {
			mp_keyUsageExchangeElement = tmpElt;
		} else if (strEquals(usageStr, XKMSConstants::s_tagSignature)) {
			mp_keyUsageSignatureElement = tmpElt;
		} else {
			throw XSECException(XSECException::ExpectedXKMSChildNotFound,
				"XKMSKeyBindingAbstractTypeImpl::load - require Encryption, Exchange or Signature text node beneath <KeyUsage>");
		}

		tmpElt = findNextElementChild(tmpElt);
	}

	while (tmpElt != NULL && strEquals(getXKMSLocalName(tmpElt), XKMSConstants::s_tagUseKeyWith)) {

		XKMSUseKeyWithImpl *ukw;
		XSECnew(ukw, XKMSUseKeyWithImpl(mp_env, tmpElt));

		m_useKeyWithList.push_back(ukw);
		ukw->load();

		tmpElt = findNextElementChild(tmpElt);
	}
}

// --------------------------------------------------------------------------------
//           Create Blank
// --------------------------------------------------------------------------------

DOMElement * XKMSKeyBindingAbstractTypeImpl::createBlankKeyBindingAbstractType(const XMLCh * tag) {

	// Get some setup values
	safeBuffer str;
	DOMDocument *doc = mp_env->getParentDocument();
	const XMLCh * prefix = mp_env->getXKMSNSPrefix();

	makeQName(str, prefix, tag);

	mp_keyBindingAbstractTypeElement = doc->createElementNS(XKMSConstants::s_unicodeStrURIXKMS, 
												str.rawXMLChBuffer());

	mp_env->doPrettyPrint(mp_keyBindingAbstractTypeElement);

	return mp_keyBindingAbstractTypeElement;

}

// --------------------------------------------------------------------------------
//           Getter methods
// --------------------------------------------------------------------------------

DOMElement * XKMSKeyBindingAbstractTypeImpl::getElement(void) const {

	return mp_keyBindingAbstractTypeElement;

}

const XMLCh * XKMSKeyBindingAbstractTypeImpl::getId(void) const {

	return (mp_idAttr != NULL ? mp_idAttr->getNodeValue() : NULL);

}

DSIGKeyInfoList * XKMSKeyBindingAbstractTypeImpl::getKeyInfoList(void) const {

	return mp_keyInfoList;

}

bool XKMSKeyBindingAbstractTypeImpl::getEncryptionKeyUsage(void) const {

	return (mp_keyUsageEncryptionElement != NULL || 
		(mp_keyUsageExchangeElement == NULL && mp_keyUsageSignatureElement == NULL));

}

bool XKMSKeyBindingAbstractTypeImpl::getSignatureKeyUsage(void) const {

	return (mp_keyUsageSignatureElement != NULL || 
		(mp_keyUsageExchangeElement == NULL && mp_keyUsageEncryptionElement == NULL));

}

bool XKMSKeyBindingAbstractTypeImpl::getExchangeKeyUsage(void) const {

	return (mp_keyUsageExchangeElement != NULL || 
		(mp_keyUsageEncryptionElement == NULL && mp_keyUsageSignatureElement == NULL));

}

// --------------------------------------------------------------------------------
//			KeyInfo elements
// --------------------------------------------------------------------------------

DSIGKeyInfoList * XKMSKeyBindingAbstractTypeImpl::getKeyInfoList() {

	return mp_keyInfoList;

}

void XKMSKeyBindingAbstractTypeImpl::clearKeyInfo(void) {

	if (mp_keyInfoElement == NULL)
		return;

	if (mp_keyBindingAbstractTypeElement->removeChild(mp_keyInfoElement) != mp_keyInfoElement) {

		throw XSECException(XSECException::ExpectedDSIGChildNotFound,
			"Attempted to remove KeyInfo node but it is no longer a child of <KeyBindingAbstractType>");

	}

	mp_keyInfoElement->release();		// No longer required

	mp_keyInfoElement = NULL;

	// Clear out the list
	mp_keyInfoList->empty();

}

void XKMSKeyBindingAbstractTypeImpl::createKeyInfoElement(void) {

	if (mp_keyInfoElement != NULL)
		return;

	safeBuffer str;

	const XMLCh * prefixNS = mp_env->getDSIGNSPrefix();
	makeQName(str, prefixNS, "KeyInfo");

	mp_keyInfoElement = mp_keyInfoList->createKeyInfo();

	// Needs to be first element
	DOMNode * insertBefore = mp_keyBindingAbstractTypeElement->getFirstChild();

	if (insertBefore != NULL) {

		if (mp_env->getPrettyPrintFlag() == true)
			mp_keyBindingAbstractTypeElement->insertBefore(mp_env->getParentDocument()->createTextNode(DSIGConstants::s_unicodeStrNL), insertBefore);
		mp_keyBindingAbstractTypeElement->insertBefore(mp_keyInfoElement, insertBefore);

	}

	else {

		mp_keyBindingAbstractTypeElement->appendChild(mp_keyInfoElement);
		mp_env->doPrettyPrint(mp_keyBindingAbstractTypeElement);

	}
	
	// Need to add the DS namespace

	if (prefixNS[0] == '\0') {
		str.sbTranscodeIn("xmlns");
	}
	else {
		str.sbTranscodeIn("xmlns:");
		str.sbXMLChCat(prefixNS);
	}

	mp_keyInfoElement->setAttributeNS(DSIGConstants::s_unicodeStrURIXMLNS, 
							str.rawXMLChBuffer(), 
							DSIGConstants::s_unicodeStrURIDSIG);

}


DSIGKeyInfoName * XKMSKeyBindingAbstractTypeImpl::appendKeyName(const XMLCh * name, bool isDName) {

	createKeyInfoElement();
	return mp_keyInfoList->appendKeyName(name, isDName);

}


DSIGKeyInfoValue * XKMSKeyBindingAbstractTypeImpl::appendDSAKeyValue(const XMLCh * P, 
						   const XMLCh * Q, 
						   const XMLCh * G, 
						   const XMLCh * Y) {

	createKeyInfoElement();
	return mp_keyInfoList->appendDSAKeyValue(P, Q, G, Y);

}

DSIGKeyInfoValue * XKMSKeyBindingAbstractTypeImpl::appendRSAKeyValue(const XMLCh * modulus, 
						   const XMLCh * exponent) {

	createKeyInfoElement();
	return mp_keyInfoList->appendRSAKeyValue(modulus, exponent);

}


DSIGKeyInfoX509 * XKMSKeyBindingAbstractTypeImpl::appendX509Data(void) {

	createKeyInfoElement();
	return mp_keyInfoList->appendX509Data();

}

DSIGKeyInfoPGPData * XKMSKeyBindingAbstractTypeImpl::appendPGPData(const XMLCh * id, const XMLCh * packet) {

	createKeyInfoElement();
	return mp_keyInfoList->appendPGPData(id, packet);

}

DSIGKeyInfoSPKIData * XKMSKeyBindingAbstractTypeImpl::appendSPKIData(const XMLCh * sexp) {

	createKeyInfoElement();
	return mp_keyInfoList->appendSPKIData(sexp);

}

DSIGKeyInfoMgmtData * XKMSKeyBindingAbstractTypeImpl::appendMgmtData(const XMLCh * data) {

	createKeyInfoElement();
	return mp_keyInfoList->appendMgmtData(data);

}

// --------------------------------------------------------------------------------
//           Setter methods
// --------------------------------------------------------------------------------

void XKMSKeyBindingAbstractTypeImpl::setId(const XMLCh * id) {

	// Setup the id
	XMLCh * myId;

	if (id == NULL)
		myId = generateId();

	mp_keyBindingAbstractTypeElement->setAttributeNS(NULL, XKMSConstants::s_tagId, id ? id : myId);
	if (id == NULL)
	    XSEC_RELEASE_XMLCH(myId);
#if defined (XSEC_XERCES_HAS_SETIDATTRIBUTE)
	mp_keyBindingAbstractTypeElement->setIdAttributeNS(NULL, XKMSConstants::s_tagId);
#endif
	mp_idAttr = 
		mp_keyBindingAbstractTypeElement->getAttributeNodeNS(NULL, XKMSConstants::s_tagId);

}

DOMElement * XKMSKeyBindingAbstractTypeImpl::setKeyUsage(const XMLCh * usage) {

	/* Create the element and string*/
	safeBuffer str;
	DOMDocument *doc = mp_env->getParentDocument();
	const XMLCh * prefix = mp_env->getXKMSNSPrefix();

	makeQName(str, prefix, XKMSConstants::s_tagKeyUsage);

	/* Create the element */
	DOMElement * e = doc->createElementNS(XKMSConstants::s_unicodeStrURIXKMS, 
		str.rawXMLChBuffer());

	str.sbXMLChIn(XKMSConstants::s_unicodeStrURIXKMS);
	str.sbXMLChCat(usage);
	e->appendChild(doc->createTextNode(str.rawXMLChBuffer()));

	/* Now find where it goes */

	DOMElement * t = findFirstElementChild(mp_keyBindingAbstractTypeElement);
	while (t != NULL) {
		if (!strEquals(getDSIGLocalName(t), XKMSConstants::s_tagKeyInfo) &&
			!strEquals(getXKMSLocalName(t), XKMSConstants::s_tagKeyUsage))
			break;

		t = findNextElementChild(t);
	}

	// Append the element
	if (t == NULL) {
		mp_keyBindingAbstractTypeElement->appendChild(e);
		mp_env->doPrettyPrint(mp_keyBindingAbstractTypeElement);
	}
	else {
		mp_keyBindingAbstractTypeElement->insertBefore(e, t);
		if (mp_env->getPrettyPrintFlag()) {
			mp_keyBindingAbstractTypeElement->insertBefore(
				mp_env->getParentDocument()->createTextNode(DSIGConstants::s_unicodeStrNL),
				t);
		}
	}

	return e;

}

void XKMSKeyBindingAbstractTypeImpl::setEncryptionKeyUsage(void) {

	if (mp_keyUsageEncryptionElement != NULL)
		return;

	mp_keyUsageEncryptionElement = setKeyUsage(XKMSConstants::s_tagEncryption);

}
void XKMSKeyBindingAbstractTypeImpl::setSignatureKeyUsage(void) {

	if (mp_keyUsageSignatureElement != NULL)
		return;

	mp_keyUsageSignatureElement = setKeyUsage(XKMSConstants::s_tagSignature);

}
void XKMSKeyBindingAbstractTypeImpl::setExchangeKeyUsage(void) {

	if (mp_keyUsageExchangeElement != NULL)
		return;

	mp_keyUsageExchangeElement = setKeyUsage(XKMSConstants::s_tagExchange);

}


// --------------------------------------------------------------------------------
//           UseKeyWith handling
// --------------------------------------------------------------------------------

int XKMSKeyBindingAbstractTypeImpl::getUseKeyWithSize(void) const {

	return (int) m_useKeyWithList.size();

}

XKMSUseKeyWith * XKMSKeyBindingAbstractTypeImpl::getUseKeyWithItem(int item) const {

	if (item < 0 || item >= (int) m_useKeyWithList.size()) {
		throw XSECException(XSECException::XKMSError,
			"XKMSKeyBindingAbstractType::getUseKeyWithItem - item out of range");
	}

	return m_useKeyWithList[item];

}

XKMSUseKeyWith * XKMSKeyBindingAbstractTypeImpl::appendUseKeyWithItem(
			const XMLCh * application,  
			const XMLCh * identifier) {

	XKMSUseKeyWithImpl * u;

	XSECnew(u, XKMSUseKeyWithImpl(mp_env));

	m_useKeyWithList.push_back(u);

	DOMElement * e = u->createBlankUseKeyWith(application, identifier);

	// Find where to append the element
	DOMElement * t = findFirstElementChild(mp_keyBindingAbstractTypeElement);
	while (t != NULL) {
		if (!strEquals(getDSIGLocalName(t), XKMSConstants::s_tagKeyInfo) &&
			!strEquals(getXKMSLocalName(t), XKMSConstants::s_tagKeyUsage) &&
			!strEquals(getXKMSLocalName(t), XKMSConstants::s_tagUseKeyWith))
			break;

		t = findNextElementChild(t);
	}

	// Append the element
	if (t == NULL) {
		mp_keyBindingAbstractTypeElement->appendChild(e);
		mp_env->doPrettyPrint(mp_keyBindingAbstractTypeElement);
	}
	else {
		mp_keyBindingAbstractTypeElement->insertBefore(e, t);
		if (mp_env->getPrettyPrintFlag()) {
			mp_keyBindingAbstractTypeElement->insertBefore(
				mp_env->getParentDocument()->createTextNode(DSIGConstants::s_unicodeStrNL),
				t);
		}
	}

	return u;

}
