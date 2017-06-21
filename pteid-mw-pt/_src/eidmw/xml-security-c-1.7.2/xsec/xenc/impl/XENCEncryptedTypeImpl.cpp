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
 * XENCEncryptedTypeImpl := Implementation of the EncryptedType interface
 * element
 *
 * $Id: XENCEncryptedTypeImpl.cpp 1350038 2012-06-13 22:21:29Z scantor $
 *
 */

#include <xsec/framework/XSECDefs.hpp>

#include "XENCCipherImpl.hpp"
#include "XENCCipherDataImpl.hpp"
#include "XENCEncryptedTypeImpl.hpp"
#include "XENCEncryptionMethodImpl.hpp"
#include "../../utils/XSECAutoPtr.hpp"

#include <xsec/xenc/XENCEncryptedKey.hpp>

#include <xsec/framework/XSECError.hpp>
#include <xsec/utils/XSECDOMUtils.hpp>
#include <xsec/transformers/TXFMBase64.hpp>
#include <xsec/transformers/TXFMChain.hpp>
#include <xsec/transformers/TXFMSB.hpp>
#include <xsec/transformers/TXFMC14n.hpp>
#include <xsec/framework/XSECEnv.hpp>
#include <xsec/dsig/DSIGReference.hpp>

#include <xercesc/util/XMLUniDefs.hpp>
#include <xercesc/util/Janitor.hpp>

XERCES_CPP_NAMESPACE_USE

// --------------------------------------------------------------------------------
//			UNICODE Strings
// --------------------------------------------------------------------------------

static XMLCh s_EncryptionMethod[] = {

	chLatin_E,
	chLatin_n,
	chLatin_c,
	chLatin_r,
	chLatin_y,
	chLatin_p,
	chLatin_t,
	chLatin_i,
	chLatin_o,
	chLatin_n,
	chLatin_M,
	chLatin_e,
	chLatin_t,
	chLatin_h,
	chLatin_o,
	chLatin_d,
	chNull,
};

static XMLCh s_KeyInfo[] = {

	chLatin_K,
	chLatin_e,
	chLatin_y,
	chLatin_I,
	chLatin_n,
	chLatin_f,
	chLatin_o,
	chNull,
};

static XMLCh s_CipherData[] = {

	chLatin_C,
	chLatin_i,
	chLatin_p,
	chLatin_h,
	chLatin_e,
	chLatin_r,
	chLatin_D,
	chLatin_a,
	chLatin_t,
	chLatin_a,
	chNull,
};

static XMLCh s_Type[] = {
	
	chLatin_T,
	chLatin_y,
	chLatin_p,
	chLatin_e,
	chNull
};

static XMLCh s_MimeType[] = {
	
	chLatin_M,
	chLatin_i,
	chLatin_m,
	chLatin_e,
	chLatin_T,
	chLatin_y,
	chLatin_p,
	chLatin_e,
	chNull
};


static XMLCh s_Encoding[] = {
	
	chLatin_E,
	chLatin_n,
	chLatin_c,
	chLatin_o,
	chLatin_d,
	chLatin_i,
	chLatin_n,
	chLatin_g,
	chNull
};

// --------------------------------------------------------------------------------
//			Constructors and Destructors
// --------------------------------------------------------------------------------

XENCEncryptedTypeImpl::XENCEncryptedTypeImpl(const XSECEnv * env) :
mp_env(env),
mp_encryptedTypeElement(NULL),
mp_keyInfoElement(NULL),
mp_cipherDataElement(NULL),
mp_cipherData(NULL),
mp_encryptionMethod(NULL),
m_keyInfoList(env),
mp_typeAttr(NULL),
mp_mimeTypeAttr(NULL),
mp_encodingAttr(NULL) {

}


XENCEncryptedTypeImpl::XENCEncryptedTypeImpl(const XSECEnv * env, DOMElement * node) :
mp_env(env),
mp_encryptedTypeElement(node),
mp_keyInfoElement(NULL),
mp_cipherDataElement(NULL),
mp_cipherData(NULL),
mp_encryptionMethod(NULL),
m_keyInfoList(env),
mp_typeAttr(NULL),
mp_mimeTypeAttr(NULL),
mp_encodingAttr(NULL) {

}

XENCEncryptedTypeImpl::~XENCEncryptedTypeImpl() {

	if (mp_cipherData != NULL)
		delete mp_cipherData;

	if (mp_encryptionMethod != NULL)
		delete mp_encryptionMethod;

}

// --------------------------------------------------------------------------------
//			Load DOM Structures
// --------------------------------------------------------------------------------

void XENCEncryptedTypeImpl::load() {

	if (mp_encryptedTypeElement == NULL) {

		// Attempt to load an empty encryptedType element
		throw XSECException(XSECException::EncryptedTypeError,
			"XENCEncryptedType::load - called on empty DOM");

	}

	// Type
	mp_typeAttr = mp_encryptedTypeElement->getAttributeNodeNS(NULL, s_Type);
	// MimeType
	mp_mimeTypeAttr = mp_encryptedTypeElement->getAttributeNodeNS(NULL, s_MimeType);
	// Encoding
	mp_encodingAttr = mp_encryptedTypeElement->getAttributeNodeNS(NULL, s_Encoding);

	// Don't know what the node name should be (held by super class), 
	// so go straight to the children
	
	DOMElement *tmpElt = (DOMElement *) findFirstChildOfType(mp_encryptedTypeElement, DOMNode::ELEMENT_NODE);

	if (tmpElt != NULL && strEquals(getXENCLocalName(tmpElt), s_EncryptionMethod)) {

		XSECnew(mp_encryptionMethod, XENCEncryptionMethodImpl(mp_env, tmpElt));
		mp_encryptionMethod->load();

		tmpElt = findNextElementChild(tmpElt);

	}

	if (tmpElt != NULL && strEquals(getDSIGLocalName(tmpElt), s_KeyInfo)) {

		// Load
		mp_keyInfoElement = tmpElt;
		m_keyInfoList.loadListFromXML(tmpElt);

		// Find the next node

		tmpElt = findNextElementChild(tmpElt);

	}

	if (tmpElt != NULL && strEquals(getXENCLocalName(tmpElt), s_CipherData)) {

		mp_cipherDataElement = tmpElt;

		XSECnew(mp_cipherData, XENCCipherDataImpl(mp_env, tmpElt));
		mp_cipherData->load();
		tmpElt = findNextElementChild(tmpElt);

	}

	else {

		throw XSECException(XSECException::ExpectedXENCChildNotFound,
			"Expected <CipherData> child of <EncryptedType>");

	}

	// Should check for EncryptionProperties

}

// --------------------------------------------------------------------------------
//			Create a blank structure
// --------------------------------------------------------------------------------

DOMElement * XENCEncryptedTypeImpl::createBlankEncryptedType(
						XMLCh * localName,
						XENCCipherData::XENCCipherDataType type, 
						const XMLCh * algorithm,
						const XMLCh * value) {

	// Reset
	mp_cipherData = NULL;
	mp_encryptionMethod = NULL;

	// Get some setup values
	safeBuffer str;
	DOMDocument *doc = mp_env->getParentDocument();
	const XMLCh * prefix = mp_env->getXENCNSPrefix();

	makeQName(str, prefix, localName);

	mp_encryptedTypeElement = doc->createElementNS(DSIGConstants::s_unicodeStrURIXENC, str.rawXMLChBuffer());

	// Set namespace
	if (prefix[0] == XERCES_CPP_NAMESPACE_QUALIFIER chNull) {
		str.sbTranscodeIn("xmlns");
	}
	else {
		str.sbTranscodeIn("xmlns:");
		str.sbXMLChCat(prefix);
	}

	mp_encryptedTypeElement->setAttributeNS(DSIGConstants::s_unicodeStrURIXMLNS, 
							str.rawXMLChBuffer(), 
							DSIGConstants::s_unicodeStrURIXENC);

	mp_env->doPrettyPrint(mp_encryptedTypeElement);

	// Create the EncryptionMethod
	if (algorithm != NULL) {

		XSECnew(mp_encryptionMethod, XENCEncryptionMethodImpl(mp_env));
		DOMNode * encryptionMethodNode = 
			mp_encryptionMethod->createBlankEncryptedMethod(algorithm);

		mp_encryptedTypeElement->appendChild(encryptionMethodNode);

		mp_env->doPrettyPrint(mp_encryptedTypeElement);

	}

	// Create the cipher Data
	XSECnew(mp_cipherData, XENCCipherDataImpl(mp_env));
	mp_cipherDataElement = mp_cipherData->createBlankCipherData(type, value);

	// Add to EncryptedType
	mp_encryptedTypeElement->appendChild(mp_cipherDataElement);

	mp_env->doPrettyPrint(mp_encryptedTypeElement);

	return mp_encryptedTypeElement;

}

// --------------------------------------------------------------------------------
//			Create a txfm chain for this transform list
// --------------------------------------------------------------------------------

TXFMChain * XENCEncryptedTypeImpl::createCipherTXFMChain(void) {

	if (mp_cipherData->getCipherDataType() == XENCCipherData::VALUE_TYPE) {

		TXFMChain * chain;

		// Given we already have this in memory, we transcode to
		// local code page and then transform

		XSECAutoPtrChar b64(mp_cipherData->getCipherValue()->getCipherString());

		try {
            TXFMSB *sb;
            XSECnew(sb, TXFMSB(mp_env->getParentDocument()));

            safeBuffer temp(b64.get());
            sb->setInput(temp);

            // Create a chain
            XSECnew(chain, TXFMChain(sb));

            // Create a base64 decoder
            TXFMBase64 * tb64;
            XSECnew(tb64, TXFMBase64(mp_env->getParentDocument()));

            chain->appendTxfm(tb64);

            return chain;
		}
        catch (...) {
            throw;
		}
	}

	else if (mp_cipherData->getCipherDataType() == XENCCipherData::REFERENCE_TYPE) {

		TXFMChain * chain;
		TXFMBase * b = DSIGReference::getURIBaseTXFM(mp_env->getParentDocument(), mp_cipherData->getCipherReference()->getURI(), mp_env);

		chain = DSIGReference::createTXFMChainFromList(b, mp_cipherData->getCipherReference()->getTransforms());
		Janitor<TXFMChain> j_chain(chain);

		if (chain->getLastTxfm()->getOutputType() == TXFMBase::DOM_NODES) {

			TXFMC14n * c14n;
			XSECnew(c14n, TXFMC14n(mp_env->getParentDocument()));
			chain->appendTxfm(c14n);

		}

		j_chain.release();
		return chain;

	}

	else {

		throw XSECException(XSECException::EncryptedTypeError,
			"XENCEncryptedType::createDecryptionTXFMChain - Unknown type of CipherData");

	}


}

// --------------------------------------------------------------------------------
//			Get Methods
// --------------------------------------------------------------------------------

XENCCipherData * XENCEncryptedTypeImpl::getCipherData(void) const {

	return mp_cipherData;

}

XENCEncryptionMethod * XENCEncryptedTypeImpl::getEncryptionMethod(void) const {
	
	return mp_encryptionMethod;

}

// --------------------------------------------------------------------------------
//			KeyInfo elements
// --------------------------------------------------------------------------------

void XENCEncryptedTypeImpl::clearKeyInfo(void) {

	if (mp_keyInfoElement == NULL)
		return;

	if (mp_encryptedTypeElement->removeChild(mp_keyInfoElement) != mp_keyInfoElement) {

		throw XSECException(XSECException::ExpectedDSIGChildNotFound,
			"Attempted to remove KeyInfo node but it is no longer a child of <EncryptedType>");

	}

	mp_keyInfoElement->release();		// No longer required

	mp_keyInfoElement = NULL;

	// Clear out the list
	m_keyInfoList.empty();

}

void XENCEncryptedTypeImpl::createKeyInfoElement(void) {

	if (mp_keyInfoElement != NULL)
		return;

	safeBuffer str;

	const XMLCh * prefixNS = mp_env->getDSIGNSPrefix();
	makeQName(str, prefixNS, "KeyInfo");

	mp_keyInfoElement = m_keyInfoList.createKeyInfo();

	// Place the node before the CipherData node
	if (mp_cipherDataElement == NULL) {

		throw XSECException(XSECException::EncryptedTypeError,
			"XENCEncryptedTypeImpl::createKeyInfoElement - unable to find CipherData node");

	}

	mp_encryptedTypeElement->insertBefore(mp_keyInfoElement, mp_cipherDataElement);

	if (mp_env->getPrettyPrintFlag() == true)
		mp_encryptedTypeElement->insertBefore(mp_env->getParentDocument()->createTextNode(DSIGConstants::s_unicodeStrNL), mp_cipherDataElement);
	
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

DSIGKeyInfoValue * XENCEncryptedTypeImpl::appendDSAKeyValue(const XMLCh * P, 
						   const XMLCh * Q, 
						   const XMLCh * G, 
						   const XMLCh * Y) {

	createKeyInfoElement();
	return m_keyInfoList.appendDSAKeyValue(P, Q, G, Y);

}

DSIGKeyInfoValue * XENCEncryptedTypeImpl::appendRSAKeyValue(const XMLCh * modulus, 
						   const XMLCh * exponent) {

	createKeyInfoElement();
	return m_keyInfoList.appendRSAKeyValue(modulus, exponent);

}


DSIGKeyInfoX509 * XENCEncryptedTypeImpl::appendX509Data(void) {

	createKeyInfoElement();
	return m_keyInfoList.appendX509Data();

}

DSIGKeyInfoName * XENCEncryptedTypeImpl::appendKeyName(const XMLCh * name, bool isDName) {

	createKeyInfoElement();
	return m_keyInfoList.appendKeyName(name, isDName);

}

void XENCEncryptedTypeImpl::appendEncryptedKey(XENCEncryptedKey * encryptedKey) {

	createKeyInfoElement();
	m_keyInfoList.addAndInsertKeyInfo(encryptedKey);

}

// --------------------------------------------------------------------------------
//			Type URI handling
// --------------------------------------------------------------------------------

const XMLCh * XENCEncryptedTypeImpl::getType(void) const {

	if (mp_typeAttr != NULL)
		return mp_typeAttr->getNodeValue();

	return NULL;

}

void XENCEncryptedTypeImpl::setType(const XMLCh * uri) {

	if (mp_typeAttr != NULL) {
		mp_typeAttr->setNodeValue(uri);
	}
	else {

		// Need to create the node
		mp_encryptedTypeElement->setAttributeNS(NULL, s_Type, uri);
		mp_typeAttr = mp_encryptedTypeElement->getAttributeNodeNS(NULL, s_Type);

		if (mp_typeAttr == NULL) {

			throw XSECException(XSECException::InternalError,
				"XENCEncryptedTypeImpl::setTypeURI - Cannot find the attribute I just added");

		}

	}
}

// --------------------------------------------------------------------------------
//			MimeType handling
// --------------------------------------------------------------------------------

const XMLCh * XENCEncryptedTypeImpl::getMimeType(void) const {

	if (mp_mimeTypeAttr != NULL)
		return mp_mimeTypeAttr->getNodeValue();

	return NULL;

}

void XENCEncryptedTypeImpl::setMimeType(const XMLCh * mimeType) {

	if (mp_mimeTypeAttr != NULL) {
		mp_mimeTypeAttr->setNodeValue(mimeType);
	}
	else {

		// Need to create the node
		mp_encryptedTypeElement->setAttributeNS(NULL, s_MimeType, mimeType);
		mp_mimeTypeAttr = mp_encryptedTypeElement->getAttributeNodeNS(NULL, s_MimeType);
		if (mp_mimeTypeAttr == NULL) {

			throw XSECException(XSECException::InternalError,
				"XENCEncryptedTypeImpl::setMimeType - Cannot find the attribute I just added");

		}

	}
}

// --------------------------------------------------------------------------------
//			Encoding handling
// --------------------------------------------------------------------------------

const XMLCh * XENCEncryptedTypeImpl::getEncoding(void) const {

	if (mp_encodingAttr != NULL)
		return mp_encodingAttr->getNodeValue();

	return NULL;

}

void XENCEncryptedTypeImpl::setEncoding(const XMLCh * uri) {

	if (mp_encodingAttr != NULL) {
		mp_encodingAttr->setNodeValue(uri);
	}
	else {

		// Need to create the node
		mp_encryptedTypeElement->setAttributeNS(NULL, s_Encoding, uri);
		mp_encodingAttr = mp_encryptedTypeElement->getAttributeNodeNS(NULL, s_Encoding);

		if (mp_encodingAttr == NULL) {

			throw XSECException(XSECException::InternalError,
				"XENCEncryptedTypeImpl::setEncodingURI - Cannot find the attribute I just added");

		}

	}
}
