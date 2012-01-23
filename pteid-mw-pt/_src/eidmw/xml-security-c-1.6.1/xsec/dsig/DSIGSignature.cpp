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
 * DSIGSignature := Class for checking and setting up signature nodes in a DSIG signature					 
 *
 * Author(s): Berin Lautenbach
 *
 * $Id: DSIGSignature.cpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

// XSEC Includes
#include <xsec/dsig/DSIGSignature.hpp>
#include <xsec/dsig/DSIGConstants.hpp>
#include <xsec/dsig/DSIGKeyInfoX509.hpp>
#include <xsec/dsig/DSIGObject.hpp>
#include <xsec/dsig/DSIGReference.hpp>
#include <xsec/dsig/DSIGTransformList.hpp>
#include <xsec/dsig/DSIGKeyInfoValue.hpp>
#include <xsec/dsig/DSIGKeyInfoX509.hpp>
#include <xsec/dsig/DSIGKeyInfoName.hpp>
#include <xsec/dsig/DSIGKeyInfoPGPData.hpp>
#include <xsec/dsig/DSIGKeyInfoSPKIData.hpp>
#include <xsec/dsig/DSIGKeyInfoMgmtData.hpp>
#include <xsec/dsig/DSIGAlgorithmHandlerDefault.hpp>
#include <xsec/enc/XSECCryptoKeyDSA.hpp>
#include <xsec/enc/XSECCryptoKeyRSA.hpp>
#include <xsec/enc/XSECKeyInfoResolver.hpp>
#include <xsec/framework/XSECError.hpp>
#include <xsec/framework/XSECAlgorithmHandler.hpp>
#include <xsec/framework/XSECAlgorithmMapper.hpp>
#include <xsec/framework/XSECEnv.hpp>
#include <xsec/framework/XSECURIResolver.hpp>
#include <xsec/transformers/TXFMDocObject.hpp>
#include <xsec/transformers/TXFMOutputFile.hpp>
#include <xsec/transformers/TXFMSHA1.hpp>
#include <xsec/transformers/TXFMBase64.hpp>
#include <xsec/transformers/TXFMC14n.hpp>
#include <xsec/transformers/TXFMChain.hpp>
#include <xsec/utils/XSECBinTXFMInputStream.hpp>
#include <xsec/utils/XSECDOMUtils.hpp>
#include <xsec/utils/XSECPlatformUtils.hpp>

// Xerces includes

#include <xercesc/dom/DOMNamedNodeMap.hpp>
#include <xercesc/util/Janitor.hpp>

XERCES_CPP_NAMESPACE_USE

// --------------------------------------------------------------------------------
//           Init
// --------------------------------------------------------------------------------


void DSIGSignature::Initialise(void) {

	DSIGAlgorithmHandlerDefault def;
	
	// Register default signature algorithm handlers

	XSECPlatformUtils::registerAlgorithmHandler(DSIGConstants::s_unicodeStrURIRSA_SHA1, def);
	XSECPlatformUtils::registerAlgorithmHandler(DSIGConstants::s_unicodeStrURIRSA_MD5, def);
	XSECPlatformUtils::registerAlgorithmHandler(DSIGConstants::s_unicodeStrURIRSA_SHA224, def);
	XSECPlatformUtils::registerAlgorithmHandler(DSIGConstants::s_unicodeStrURIRSA_SHA256, def);
	XSECPlatformUtils::registerAlgorithmHandler(DSIGConstants::s_unicodeStrURIRSA_SHA384, def);
	XSECPlatformUtils::registerAlgorithmHandler(DSIGConstants::s_unicodeStrURIRSA_SHA512, def);
	
    XSECPlatformUtils::registerAlgorithmHandler(DSIGConstants::s_unicodeStrURIDSA_SHA1, def);
    XSECPlatformUtils::registerAlgorithmHandler(DSIGConstants::s_unicodeStrURIDSA_SHA256, def);

    XSECPlatformUtils::registerAlgorithmHandler(DSIGConstants::s_unicodeStrURIECDSA_SHA1, def);
	XSECPlatformUtils::registerAlgorithmHandler(DSIGConstants::s_unicodeStrURIECDSA_SHA256, def);
	XSECPlatformUtils::registerAlgorithmHandler(DSIGConstants::s_unicodeStrURIECDSA_SHA384, def);
	XSECPlatformUtils::registerAlgorithmHandler(DSIGConstants::s_unicodeStrURIECDSA_SHA512, def);

    XSECPlatformUtils::registerAlgorithmHandler(DSIGConstants::s_unicodeStrURIHMAC_SHA1, def);
	XSECPlatformUtils::registerAlgorithmHandler(DSIGConstants::s_unicodeStrURIHMAC_SHA224, def);
	XSECPlatformUtils::registerAlgorithmHandler(DSIGConstants::s_unicodeStrURIHMAC_SHA256, def);
	XSECPlatformUtils::registerAlgorithmHandler(DSIGConstants::s_unicodeStrURIHMAC_SHA384, def);
	XSECPlatformUtils::registerAlgorithmHandler(DSIGConstants::s_unicodeStrURIHMAC_SHA512, def);

	// Register default hashing algorithm handlers

	XSECPlatformUtils::registerAlgorithmHandler(DSIGConstants::s_unicodeStrURISHA1, def);
	XSECPlatformUtils::registerAlgorithmHandler(DSIGConstants::s_unicodeStrURISHA224, def);
	XSECPlatformUtils::registerAlgorithmHandler(DSIGConstants::s_unicodeStrURISHA256, def);
	XSECPlatformUtils::registerAlgorithmHandler(DSIGConstants::s_unicodeStrURISHA384, def);
	XSECPlatformUtils::registerAlgorithmHandler(DSIGConstants::s_unicodeStrURISHA512, def);
	XSECPlatformUtils::registerAlgorithmHandler(DSIGConstants::s_unicodeStrURIMD5, def);

}

// --------------------------------------------------------------------------------
//           Some useful utility functions
// --------------------------------------------------------------------------------


#if 0

bool compareBase64StringToRaw(safeBuffer &b64SB, 
							  unsigned char * raw, 
							  unsigned int rawLen, 
							  unsigned int maxCompare = 0) {
	// Decode a base64 buffer and then compare the result to a raw buffer
	// Compare at most maxCompare bits (if maxComare > 0)
	// Note - whilst the other parameters are bytes, maxCompare is bits

	unsigned char outputStr[1024];
	unsigned char b64Str[1024];
	unsigned int outputLen = 0;
	
	XSECCryptoBase64 * b64 = XSECPlatformUtils::g_cryptoProvider->base64();
	
	if (!b64) {

		throw XSECException(XSECException::CryptoProviderError, 
				"Error requesting Base64 object from Crypto Provider");

	}

	Janitor<XSECCryptoBase64> j_b64(b64);

	strncpy((char *) b64Str, (char *) b64SB.rawBuffer(), 1023);
	b64Str[1023] = '\0';	// Just in case

	b64->decodeInit();
	outputLen = b64->decode((unsigned char *) b64Str, (unsigned int) strlen((char *) b64Str), outputStr, 1024);
	outputLen += b64->decodeFinish(&outputStr[outputLen], 1024 - outputLen);

	// Compare

	div_t d;
	unsigned int maxCompareBytes, maxCompareBits;
	maxCompareBits = 0;

	unsigned int size;

	if (maxCompare > 0) {
		d = div(maxCompare, 8);
		maxCompareBytes = d.quot;
		if (d.rem != 0)
			maxCompareBytes++;

		if (rawLen < maxCompareBytes && outputLen < maxCompareBytes) {
			if (rawLen != outputLen)
				return false;
			size = rawLen;
		}
		else if (rawLen < maxCompareBytes || outputLen < maxCompareBytes) {
			return false;
		}
		else
			size = maxCompareBytes;
	}
	else {

		if (rawLen != outputLen)
			return false;

		size = rawLen;

	}

	// Compare bytes
	unsigned int i, j;
	for (i = 0; i < size; ++ i) {
		if (raw[i] != outputStr[i])
			return false;
	}

	// Compare bits

	char mask = 0x01;
	if (maxCompare != 0) {
	    for (j = 0 ; j < (unsigned int) d.rem; ++i) {

		    if ((raw[i] & mask) != (outputStr[i] & mask))
			    return false;

			mask = mask << 1;
		}
	}

	return true;

}


void convertRawToBase64String(safeBuffer &b64SB, 
							  unsigned char * raw, 
							  unsigned int rawLen, 
							  unsigned int maxBits = 0) {

	// Translate the rawbuffer (at most maxBits or rawLen - whichever is smaller)
	// to a base64 string

	unsigned char b64Str[1024];
	unsigned int outputLen = 0;
	
	XSECCryptoBase64 * b64 = XSECPlatformUtils::g_cryptoProvider->base64();
	
	if (!b64) {

		throw XSECException(XSECException::CryptoProviderError, 
				"Error requesting Base64 object from Crypto Provider");

	}

	Janitor<XSECCryptoBase64> j_b64(b64);

	// Determine length to translate
	unsigned int size;

	if (maxBits > 0) {
		div_t d = div(maxBits, 8);
		size = d.quot;
		if (d.rem != 0)
			++size;
		
		if (size > rawLen)
			size = rawLen;
	}

	else
		size = rawLen;

	b64->encodeInit();
	outputLen = b64->encode((unsigned char *) raw, rawLen, b64Str, 1024);
	outputLen += b64->encodeFinish(&b64Str[outputLen], 1024 - outputLen);
	b64Str[outputLen] = '\0';

	// Copy out

	b64SB.sbStrcpyIn((char *) b64Str);

}

#endif /* 0 */

// --------------------------------------------------------------------------------
//           Get the Canonicalised BYTE_STREAM of the SignedInfo
// --------------------------------------------------------------------------------

XSECBinTXFMInputStream * DSIGSignature::makeBinInputStream(void) const {

	TXFMBase * txfm;

	// Create the starting point for the transform list

	XSECnew(txfm, TXFMDocObject(mp_doc));

	TXFMChain * chain;
	XSECnew(chain, TXFMChain(txfm));
	Janitor<TXFMChain> j_chain(chain);

	((TXFMDocObject *) txfm)->setInput(mp_doc, mp_signedInfo->getDOMNode());
	
	// canonicalise the SignedInfo content

	switch (mp_signedInfo->getCanonicalizationMethod()) {

	case CANON_C14N_NOC :

		XSECnew(txfm, TXFMC14n(mp_doc));
		chain->appendTxfm(txfm);
		txfm->stripComments();
		
		break;

	case CANON_C14N_COM :

		XSECnew(txfm, TXFMC14n(mp_doc));
		chain->appendTxfm(txfm);
		txfm->activateComments();

		break;

	case CANON_C14NE_NOC :

		XSECnew(txfm, TXFMC14n(mp_doc));
		chain->appendTxfm(txfm);
		((TXFMC14n *) txfm)->setExclusive();
		txfm->stripComments();
		
		break;

	case CANON_C14NE_COM :

		XSECnew(txfm, TXFMC14n(mp_doc));
		chain->appendTxfm(txfm);
		((TXFMC14n *) txfm)->setExclusive();
		txfm->activateComments();

		break;

	default :

		throw XSECException(XSECException::SigVfyError,
			"Canonicalisation method unknown in DSIGSignature::makeBinInputStream()");

	}

	// Now create the InputStream

	XSECBinTXFMInputStream * ret;

	ret = new XSECBinTXFMInputStream(chain);
	j_chain.release();

	return ret;

}


// --------------------------------------------------------------------------------
//           Get the list of references
// --------------------------------------------------------------------------------

DSIGReferenceList * DSIGSignature::getReferenceList(void) {

	return mp_signedInfo ? mp_signedInfo->getReferenceList() : NULL;

}

const DSIGReferenceList * DSIGSignature::getReferenceList(void) const {

	return mp_signedInfo ? mp_signedInfo->getReferenceList() : NULL;

}

const XMLCh * DSIGSignature::getAlgorithmURI() const {

    return mp_signedInfo ? mp_signedInfo->getAlgorithmURI() : NULL;

}

// --------------------------------------------------------------------------------
//           Set and Get Resolvers
// --------------------------------------------------------------------------------


void DSIGSignature::setURIResolver(XSECURIResolver * resolver) {

	mp_env->setURIResolver(resolver);

}

XSECURIResolver * DSIGSignature::getURIResolver(void) const {

	return mp_env->getURIResolver();

}

void DSIGSignature::setKeyInfoResolver(XSECKeyInfoResolver * resolver) {

	if (mp_KeyInfoResolver != 0)
		delete mp_KeyInfoResolver;

	mp_KeyInfoResolver = resolver->clone();

}

XSECKeyInfoResolver * DSIGSignature::getKeyInfoResolver(void) const {

	return mp_KeyInfoResolver;

}

// --------------------------------------------------------------------------------
//           Object Handling
// --------------------------------------------------------------------------------

DSIGObject * DSIGSignature::appendObject(void) {

	DSIGObject * ret;
	XSECnew(ret, DSIGObject(mp_env));
	DOMElement * elt = ret->createBlankObject();

	mp_sigNode->appendChild(elt);
	mp_env->doPrettyPrint(mp_sigNode);

	m_objects.push_back(ret);

	return ret;

}

int DSIGSignature::getObjectLength(void) const {

	return (unsigned int) m_objects.size();

}

DSIGObject * DSIGSignature::getObjectItem(int i) {

	if ( i < 0 || i >= ((int) m_objects.size())) {
		throw XSECException(XSECException::ObjectError,
			"DSIGSignature::getObjectItem - index out of range");
	}

	return m_objects[i];

}

const DSIGObject * DSIGSignature::getObjectItem(int i) const {

	if ( i < 0 || i >= ((int) m_objects.size())) {
		throw XSECException(XSECException::ObjectError,
			"DSIGSignature::getObjectItem - index out of range");
	}

	return m_objects[i];

}

// --------------------------------------------------------------------------------
//           Signature
// --------------------------------------------------------------------------------

// Constructors and Destructors

DSIGSignature::DSIGSignature(DOMDocument *doc, DOMNode *sigNode) :
m_keyInfoList(0),
m_errStr("") {

	mp_doc = doc;
	mp_sigNode = sigNode;
	mp_signingKey = NULL;
	mp_signedInfo = NULL;
	mp_KeyInfoResolver = NULL;
	mp_KeyInfoNode = NULL;
	m_loaded = false;
	m_interlockingReferences = false;

	// Set up our formatter
	XSECnew(mp_formatter, XSECSafeBufferFormatter("UTF-8",XMLFormatter::NoEscapes, 
												XMLFormatter::UnRep_CharRef));

	// Set up the environment
	XSECnew(mp_env, XSECEnv(doc));

	m_keyInfoList.setEnvironment(mp_env);

}

DSIGSignature::DSIGSignature(void) :
m_keyInfoList(0),
m_errStr("") {
	mp_doc = NULL;
	mp_sigNode = NULL;
	mp_signingKey = NULL;
	mp_signedInfo = NULL;
	mp_KeyInfoResolver = NULL;
	mp_KeyInfoNode = NULL;
	m_loaded = false;
	m_interlockingReferences = false;

	// Set up our formatter
	XSECnew(mp_formatter, XSECSafeBufferFormatter("UTF-8",XMLFormatter::NoEscapes, 
												XMLFormatter::UnRep_CharRef));

	XSECnew(mp_env, XSECEnv(NULL));

	m_keyInfoList.setEnvironment(mp_env);

}

DSIGSignature::~DSIGSignature() {

	if (mp_env != NULL)
		delete mp_env;

	if (mp_signingKey != NULL) {

		delete mp_signingKey;
		mp_signingKey = NULL;

	}

	if (mp_signedInfo != NULL) {

		delete mp_signedInfo;
		mp_signedInfo = NULL;

	}

	if (mp_formatter != NULL) {

		delete mp_formatter;
		mp_formatter = NULL;
	}

	if (mp_KeyInfoResolver != NULL) {
		delete mp_KeyInfoResolver;
		mp_KeyInfoResolver = NULL;
	}

	// Delete any object items
	for (int i = 0; i < ((int) m_objects.size()); ++i) {
		delete (m_objects[i]);
	}

}

// Actions

const XMLCh * DSIGSignature::getErrMsgs() const {

	return m_errStr.rawXMLChBuffer();

}

// --------------------------------------------------------------------------------
//           Pretty Printing
// --------------------------------------------------------------------------------

void DSIGSignature::setPrettyPrint(bool flag) {

	mp_env->setPrettyPrintFlag(flag);

}


bool DSIGSignature::getPrettyPrint(void) const {

	return mp_env->getPrettyPrintFlag();

}

// --------------------------------------------------------------------------------
//           Creating signatures from blank
// --------------------------------------------------------------------------------

void DSIGSignature::setDSIGNSPrefix(const XMLCh * prefix) {

	mp_env->setDSIGNSPrefix(prefix);

}

void DSIGSignature::setECNSPrefix(const XMLCh * prefix) {

	mp_env->setECNSPrefix(prefix);

}

void DSIGSignature::setXPFNSPrefix(const XMLCh * prefix) {

	mp_env->setXPFNSPrefix(prefix);

}

// get

const XMLCh * DSIGSignature::getDSIGNSPrefix() const {

	return mp_env->getDSIGNSPrefix();

}


const XMLCh * DSIGSignature::getECNSPrefix() const {

	return mp_env->getECNSPrefix();

}

const XMLCh * DSIGSignature::getXPFNSPrefix() const {

	return mp_env->getXPFNSPrefix();

}

// --------------------------------------------------------------------------------
//           Creating Blank Signature
// --------------------------------------------------------------------------------

DOMElement *DSIGSignature::createBlankSignature(DOMDocument *doc,
			canonicalizationMethod cm,
			signatureMethod	sm,
			hashMethod hm) {

	// This is now deprecated.  Because this is so, we go the long way - translate
	// to URI and then call the "standard" method, which will translate back to 
	// internal enums if possible

	const XMLCh * cURI;
	safeBuffer sURI;

	if ((cURI = canonicalizationMethod2UNICODEURI(cm)) == NULL) {
		throw XSECException(XSECException::UnknownCanonicalization,
			"DSIGSignature::createBlankSignature - Canonicalisation method unknown");
	}

	if (signatureHashMethod2URI(sURI, sm, hm) == false) {
		throw XSECException(XSECException::UnknownSignatureAlgorithm,
			"DSIGSignature::createBlankSignature - Signature/Hash method unknown");
	}

	return createBlankSignature(doc, cURI, sURI.sbStrToXMLCh());

}


DOMElement *DSIGSignature::createBlankSignature(
		DOMDocument *doc,
		const XMLCh * canonicalizationAlgorithmURI,
		const XMLCh * signatureAlgorithmURI) {

	// "New" method to create a blank signature, based on URIs.

	mp_doc = doc;
	mp_env->setParentDocument(doc);

	const XMLCh * prefixNS = mp_env->getDSIGNSPrefix();

	safeBuffer str;

	makeQName(str, prefixNS, "Signature");

	DOMElement *sigNode = doc->createElementNS(DSIGConstants::s_unicodeStrURIDSIG, str.rawXMLChBuffer());

	if (prefixNS[0] == '\0') {
		str.sbTranscodeIn("xmlns");
	}
	else {
		str.sbTranscodeIn("xmlns:");
		str.sbXMLChCat(prefixNS);
	}

	sigNode->setAttributeNS(DSIGConstants::s_unicodeStrURIXMLNS, 
							str.rawXMLChBuffer(), 
							DSIGConstants::s_unicodeStrURIDSIG);

	mp_sigNode = sigNode;

	mp_env->doPrettyPrint(mp_sigNode);

	// Create the skeleton SignedInfo
	XSECnew(mp_signedInfo, DSIGSignedInfo(mp_doc, mp_formatter, mp_env));
	
	mp_sigNode->appendChild(mp_signedInfo->createBlankSignedInfo(
		canonicalizationAlgorithmURI, signatureAlgorithmURI));
	mp_env->doPrettyPrint(mp_sigNode);

	// Create a dummy signature value (dummy until signed)

	makeQName(str, mp_env->getDSIGNSPrefix(), "SignatureValue");
	DOMElement *sigValNode = doc->createElementNS(DSIGConstants::s_unicodeStrURIDSIG, 
												  str.rawXMLChBuffer());
	mp_signatureValueNode = sigValNode;
	mp_sigNode->appendChild(sigValNode);
	mp_env->doPrettyPrint(mp_sigNode);

	// Some text to mark this as a template only
	sigValNode->appendChild(doc->createTextNode(MAKE_UNICODE_STRING("Not yet signed")));

	m_loaded = true;
	
	return sigNode;
}



// --------------------------------------------------------------------------------
//           Creating References
// --------------------------------------------------------------------------------

DSIGReference * DSIGSignature::createReference(const XMLCh * URI, 
								hashMethod hm, 
								char * type) {

	return mp_signedInfo->createReference(URI, hm, type);

}

DSIGReference * DSIGSignature::createReference(
		const XMLCh * URI,
		const XMLCh * hashAlgorithmURI, 
		const XMLCh * type) {

	return mp_signedInfo->createReference(URI, hashAlgorithmURI, type);

}

DSIGReference * DSIGSignature::removeReference(DSIGReferenceList::size_type index) {
    return mp_signedInfo ? mp_signedInfo->removeReference(index) : NULL;
}

// --------------------------------------------------------------------------------
//           Manipulation of KeyInfo elements
// --------------------------------------------------------------------------------

void DSIGSignature::clearKeyInfo(void) {

	if (mp_KeyInfoNode == 0)
		return;

	if (mp_sigNode->removeChild(mp_KeyInfoNode) != mp_KeyInfoNode) {

		throw XSECException(XSECException::ExpectedDSIGChildNotFound,
			"Attempted to remove KeyInfo node but it is no longer a child of <Signature>");

	}

	mp_KeyInfoNode->release();		// No longer required

	mp_KeyInfoNode = NULL;

	// Clear out the list
	m_keyInfoList.empty();

}

void DSIGSignature::createKeyInfoElement(void) {

	if (mp_KeyInfoNode != NULL)
		return;

	safeBuffer str;

	makeQName(str, mp_env->getDSIGNSPrefix(), "KeyInfo");

	mp_KeyInfoNode = m_keyInfoList.createKeyInfo();

	// Append the node to the end of the signature
	
	DOMNode * afterSignatureValue = mp_signatureValueNode->getNextSibling();
	while (afterSignatureValue != 0 && afterSignatureValue->getNodeType() != DOMNode::ELEMENT_NODE)
		afterSignatureValue = afterSignatureValue->getNextSibling();

	if (afterSignatureValue == 0) {
		mp_sigNode->appendChild(mp_KeyInfoNode);
		mp_env->doPrettyPrint(mp_sigNode);
	}
	else {
		mp_sigNode->insertBefore(mp_KeyInfoNode, afterSignatureValue);
		if (mp_env->getPrettyPrintFlag() == true)
			mp_sigNode->insertBefore(mp_doc->createTextNode(DSIGConstants::s_unicodeStrNL),
				afterSignatureValue);
	}

}

DSIGKeyInfoValue * DSIGSignature::appendDSAKeyValue(const XMLCh * P, 
						   const XMLCh * Q, 
						   const XMLCh * G, 
						   const XMLCh * Y) {

	createKeyInfoElement();
	return m_keyInfoList.appendDSAKeyValue(P, Q, G, Y);

}

DSIGKeyInfoValue * DSIGSignature::appendRSAKeyValue(const XMLCh * modulus, 
						   const XMLCh * exponent) {

	createKeyInfoElement();
	return m_keyInfoList.appendRSAKeyValue(modulus, exponent);

}


DSIGKeyInfoX509 * DSIGSignature::appendX509Data(void) {

	createKeyInfoElement();
	return m_keyInfoList.appendX509Data();

}

DSIGKeyInfoName * DSIGSignature::appendKeyName(const XMLCh * name, bool isDName) {

	createKeyInfoElement();
	return m_keyInfoList.appendKeyName(name, isDName);

}

DSIGKeyInfoPGPData * DSIGSignature::appendPGPData(const XMLCh * id, const XMLCh * packet) {

	createKeyInfoElement();
	return m_keyInfoList.appendPGPData(id, packet);

}

DSIGKeyInfoSPKIData * DSIGSignature::appendSPKIData(const XMLCh * sexp) {

	createKeyInfoElement();
	return m_keyInfoList.appendSPKIData(sexp);

}

DSIGKeyInfoMgmtData * DSIGSignature::appendMgmtData(const XMLCh * data) {

	createKeyInfoElement();
	return m_keyInfoList.appendMgmtData(data);

}

// --------------------------------------------------------------------------------
//           Working on Existing templates
// --------------------------------------------------------------------------------


void DSIGSignature::load(void) {

	// Load all the information from the source document into local variables for easier
	// manipulation by the other functions in the class

	if (mp_sigNode == NULL) {

		// Attempt to load an empty signature element
		throw XSECException(XSECException::LoadEmptySignature);

	}

	if (!strEquals(getDSIGLocalName(mp_sigNode), "Signature")) {

		throw XSECException(XSECException::LoadNonSignature);

	}

	m_loaded = true;

	// Find the prefix being used so that we can later use it to manipulate the signature
	mp_env->setDSIGNSPrefix(mp_sigNode->getPrefix());

	// Now check for SignedInfo
	DOMNode *tmpElt = mp_sigNode->getFirstChild();

	while (tmpElt != 0 && (tmpElt->getNodeType() != DOMNode::ELEMENT_NODE))
		// Skip text and comments
		tmpElt = tmpElt->getNextSibling();

	if (tmpElt == 0 || !strEquals(getDSIGLocalName(tmpElt), "SignedInfo")) {

			throw XSECException(XSECException::ExpectedDSIGChildNotFound, 
					"Expected <SignedInfo> as first child of <Signature>");

	}

	// Have a signed info

	XSECnew(mp_signedInfo, DSIGSignedInfo(mp_doc, mp_formatter, tmpElt, mp_env));
	mp_signedInfo->load();

	// Look at Signature Value
	tmpElt = tmpElt->getNextSibling();

	while (tmpElt != 0 && tmpElt->getNodeType() != DOMNode::ELEMENT_NODE)
		tmpElt = tmpElt->getNextSibling();

	if (tmpElt == 0 || !strEquals(getDSIGLocalName(tmpElt), "SignatureValue")) {

		throw XSECException(XSECException::ExpectedDSIGChildNotFound,
			"Expected <SignatureValue> node");

	}

	DOMNode *tmpSV = tmpElt->getFirstChild();
	while (tmpSV != 0 && tmpSV->getNodeType() != DOMNode::TEXT_NODE)
		tmpSV = tmpSV->getNextSibling();

	if (tmpSV == 0)
		throw XSECException(XSECException::ExpectedDSIGChildNotFound,
		"Expected TEXT child of <SignatureValue>");

	mp_signatureValueNode = tmpElt;
	
	// The signature value is transcoded to local code page, as it is easier
	// to work with, and should be low ASCII in any case (Base64)

	m_signatureValueSB.sbTranscodeIn(tmpSV->getNodeValue());


	// Now look at KeyInfo
	tmpElt = tmpElt->getNextSibling();

	while (tmpElt != 0 && !((tmpElt->getNodeType() == DOMNode::ELEMENT_NODE) && 
		strEquals(getDSIGLocalName(tmpElt), "KeyInfo")))
		tmpElt = tmpElt->getNextSibling();

	if (tmpElt != 0 && strEquals(getDSIGLocalName(tmpElt), "KeyInfo")) {

		// Have a keyInfo

		mp_KeyInfoNode = tmpElt;		// In case we later want to manipulate it

		m_keyInfoList.loadListFromXML(tmpElt);

		tmpElt = findNextElementChild(tmpElt);
	}

	while (tmpElt != 0 && strEquals(getDSIGLocalName(tmpElt), "Object")) {

		DSIGObject * obj;
		XSECnew(obj, DSIGObject(mp_env, tmpElt));
		obj->load();

		m_objects.push_back(obj);

		tmpElt = findNextElementChild(tmpElt);

	}
/*
	* Strictly speaking, this should remain, but it causes too many problems with non
	* conforming signatures

	if (tmpElt != 0) {
		throw XSECException(XSECException::ExpectedDSIGChildNotFound, 
			"DSIGSignature::load - Unexpected (non Object) Element found at end of signature");
	}
*/
}

TXFMChain * DSIGSignature::getSignedInfoInput(void) {

	TXFMBase * txfm;
	TXFMChain * chain;

	// First we calculate the hash.  Start off by creating a starting point
	XSECnew(txfm, TXFMDocObject(mp_doc));
	XSECnew(chain, TXFMChain(txfm));
	Janitor<TXFMChain> j_chain(chain);

	((TXFMDocObject *) txfm)->setInput(mp_doc, mp_signedInfo->getDOMNode());
	
	// canonicalise the SignedInfo content

	switch (mp_signedInfo->getCanonicalizationMethod()) {

	case CANON_C14N_NOC :

		XSECnew(txfm, TXFMC14n(mp_doc));
		chain->appendTxfm(txfm);
		txfm->stripComments();
		
		break;

	case CANON_C14N_COM :

		XSECnew(txfm, TXFMC14n(mp_doc));
		chain->appendTxfm(txfm);
		txfm->activateComments();

		break;

	case CANON_C14NE_NOC :

		XSECnew(txfm, TXFMC14n(mp_doc));
		chain->appendTxfm(txfm);
		((TXFMC14n *) txfm)->setExclusive();
		txfm->stripComments();
		
		break;

	case CANON_C14NE_COM :

		XSECnew(txfm, TXFMC14n(mp_doc));
		chain->appendTxfm(txfm);
		((TXFMC14n *) txfm)->setExclusive();
		txfm->activateComments();

		break;

	default :

		throw XSECException(XSECException::SigVfyError,
			"Canonicalisation method unknown in DSIGSignature::calculateSignedInfoHash()");

	}
	
	j_chain.release();
	return chain;

}

unsigned int DSIGSignature::calculateSignedInfoHash(unsigned char * hashBuf, 
													unsigned int hashBufLen) {

	// Get the SignedInfo input bytes
	TXFMChain * chain = getSignedInfoInput();
	Janitor<TXFMChain> j_chain(chain);

	// Check for debugging sink for the data
    TXFMBase* sink = XSECPlatformUtils::GetReferenceLoggingSink(mp_doc);
    if (sink)
        chain->appendTxfm(sink);

	// Setup Hash
	// First find the appropriate handler for the URI
	XSECAlgorithmHandler * handler = 
		XSECPlatformUtils::g_algorithmMapper->mapURIToHandler(
					mp_signedInfo->getAlgorithmURI());

	if (handler == NULL) {


		throw XSECException(XSECException::SigVfyError,
			"Hash method unknown in DSIGSignature::calculateSignedInfoHash()");

	}

	if (!handler->appendSignatureHashTxfm(chain, mp_signedInfo->getAlgorithmURI(), mp_signingKey)) {

		throw XSECException(XSECException::SigVfyError,
			"Unexpected error in handler whilst appending Signature Hash transform");

	}


	// Write hash to the buffer
	return chain->getLastTxfm()->readBytes((XMLByte *) hashBuf, hashBufLen);

}

unsigned int DSIGSignature::calculateSignedInfoAndReferenceHash(unsigned char * hashBuf, 
													unsigned int hashBufLen) {

	// Set up the reference list hashes - including any manifests
	mp_signedInfo->hash(m_interlockingReferences);
	// calculaet signed InfoHash
	return calculateSignedInfoHash(hashBuf,hashBufLen);
}

// --------------------------------------------------------------------------------
//           Verify a signature
// --------------------------------------------------------------------------------

bool DSIGSignature::verifySignatureOnlyInternal(void) {

	unsigned char hash[4096];
	int hashLen;

	if (!m_loaded) {

		// Need to call "load" prior to checking a signature
		throw XSECException(XSECException::SigVfyError,
					"DSIGSignature::verify() called prior to DSIGSignature::load()");

	}

	// FIX: CVE-2009-0217

	if (mp_signedInfo->getHMACOutputLength() > 0 && mp_signedInfo->getHMACOutputLength() < 80) {
	    throw XSECException(XSECException::SigVfyError,
            "DSIGSignature::verify() - HMACOutputLength is unsafe");
	}

	// Try to find a key
	if (mp_signingKey == NULL) {

//		// Try to load a key from the KeyInfo list
//		if ((mp_signingKey = m_keyInfoList.findKey()) == NULL) {

//			throw XSECException(XSECException::SigVfyError,
//				"DSIGSignature::verify() - no verification key loaded and cannot determine from KeyInfo list");
//		}

		if (mp_KeyInfoResolver == NULL) {
			
			throw XSECException(XSECException::SigVfyError,
				"DSIGSignature::verify() - no verification key loaded and no KeyInfoResolver loaded");

		}
		
		if ((mp_signingKey = mp_KeyInfoResolver->resolveKey(&m_keyInfoList)) == NULL) {

			throw XSECException(XSECException::SigVfyError,
				"DSIGSignature::verify() - no verification key loaded and cannot determine from KeyInfoResolver");
		}

	}

	// Get the SignedInfo input bytes
	TXFMChain * chain = getSignedInfoInput();
	Janitor<TXFMChain> j_chain(chain);

	hashLen = calculateSignedInfoHash(hash, 4096);

	// Now set up to verify
	// First find the appropriate handler for the URI
	XSECAlgorithmHandler * handler = 
		XSECPlatformUtils::g_algorithmMapper->mapURIToHandler(
					mp_signedInfo->getAlgorithmURI());

	if (handler == NULL) {

		throw XSECException(XSECException::SigVfyError,
			"Hash method unknown in DSIGSignature::verifySignatureOnlyInternal()");

	}

	bool sigVfyRet = handler->verifyBase64Signature(chain, 
		mp_signedInfo->getAlgorithmURI(), 
		m_signatureValueSB.rawCharBuffer(), 
		mp_signedInfo->getHMACOutputLength(),
		mp_signingKey);

	if (!sigVfyRet)
		m_errStr.sbXMLChCat("Validation of <SignedInfo> failed");

	return sigVfyRet;

}

bool DSIGSignature::verifySignatureOnly(void) {

	m_errStr.sbTranscodeIn("");
	return verifySignatureOnlyInternal();

}

bool DSIGSignature::verify(void) {

	// We have a (hopefully) fully loaded signature.  Need to 
	// verify

	bool referenceCheckResult;

	if (!m_loaded) {

		// Need to call "load" prior to checking a signature
		throw XSECException(XSECException::SigVfyError,
					"DSIGSignature::verify() called prior to DSIGSignature::load()");

	}

	// Reset
	m_errStr.sbXMLChIn(DSIGConstants::s_unicodeStrEmpty);

	// First thing to do is check the references

	referenceCheckResult = mp_signedInfo->verify(m_errStr);

	// Check the signature

	bool sigVfyResult = verifySignatureOnlyInternal();

	return sigVfyResult & referenceCheckResult;
}


void DSIGSignature::signExternal(XMLByte *signature, unsigned int siglen) {

	XMLByte base64Sig [siglen*2];
	unsigned int base64SigLen;

	if (!m_loaded) {

		// Need to call "load" prior to checking a signature
		throw XSECException(XSECException::SigVfyError,
					"DSIGSignature::sign() called prior to DSIGSignature::load()");

	}

	// Calculate the base64 value

	XSECCryptoBase64 *	b64 = XSECPlatformUtils::g_cryptoProvider->base64();

	if (!b64) {

		throw XSECException(XSECException::CryptoProviderError,
				"Error requesting Base64 object from Crypto Provider");

	}

	Janitor<XSECCryptoBase64> j_b64(b64);

	b64->encodeInit();
	base64SigLen = b64->encode(signature, siglen,  base64Sig, 1024);
	base64SigLen += b64->encodeFinish(&base64Sig[base64SigLen],
										(siglen * 2) - base64SigLen);

	// Ensure the string is terminated
	if (base64Sig[base64SigLen-1] == '\n')
		base64Sig[base64SigLen-1] = '\0';
	else
		base64Sig[base64SigLen] = '\0';


	// Now we have the signature - place it in the DOM structures

	DOMNode *tmpElt = mp_signatureValueNode->getFirstChild();

	while (tmpElt != NULL && tmpElt->getNodeType() != DOMNode::TEXT_NODE)
		tmpElt = tmpElt->getNextSibling();

	if (tmpElt == NULL) {
		// Need to create the underlying TEXT_NODE
		DOMDocument * doc = mp_signatureValueNode->getOwnerDocument();
		tmpElt = doc->createTextNode(MAKE_UNICODE_STRING((char *) base64Sig));
		mp_signatureValueNode->appendChild(tmpElt);
	}
	else {
		tmpElt->setNodeValue(MAKE_UNICODE_STRING((char *) base64Sig));
	}

	// And copy to the local buffer TODO: Not sure if this is useful
	//m_signatureValueSB = b64Buf;
}

// --------------------------------------------------------------------------------
//           Sign the XML document that has been previously loaded
// --------------------------------------------------------------------------------

void DSIGSignature::sign(void) {

	// We have a (hopefully) fully loaded signature.  Need to 
	// sign

	if (!m_loaded) {

		// Need to call "load" prior to checking a signature
		throw XSECException(XSECException::SigVfyError,
					"DSIGSignature::sign() called prior to DSIGSignature::load()");

	}

	// Check we have a key
	if (mp_signingKey == NULL) {

		throw XSECException(XSECException::SigVfyError,
			"DSIGSignature::sign() - no signing key loaded");
		

	}

	// Reset error string in case we have any reference problems.
	m_errStr.sbXMLChIn(DSIGConstants::s_unicodeStrEmpty);

	// Set up the reference list hashes - including any manifests
	mp_signedInfo->hash(m_interlockingReferences);

	// Get the SignedInfo input bytes
	TXFMChain * chain = getSignedInfoInput();
	Janitor<TXFMChain> j_chain(chain);

	// Calculate the hash to be signed

	safeBuffer b64Buf;

	XSECAlgorithmHandler * handler = 
		XSECPlatformUtils::g_algorithmMapper->mapURIToHandler(
					mp_signedInfo->getAlgorithmURI());

	if (handler == NULL) {


		throw XSECException(XSECException::SigVfyError,
			"Hash method unknown in DSIGSignature::sign()");

	}

	if (!handler->signToSafeBuffer(chain, mp_signedInfo->getAlgorithmURI(), 
								   mp_signingKey, mp_signedInfo->getHMACOutputLength(), b64Buf)) {

		throw XSECException(XSECException::SigVfyError,
			"Unexpected error in handler whilst appending Signature Hash transform");

	}

	// Now we have the signature - place it in the DOM structures

	DOMNode *tmpElt = mp_signatureValueNode->getFirstChild();

	while (tmpElt != NULL && tmpElt->getNodeType() != DOMNode::TEXT_NODE)
		tmpElt = tmpElt->getNextSibling();

	if (tmpElt == NULL) {
		// Need to create the underlying TEXT_NODE
		DOMDocument * doc = mp_signatureValueNode->getOwnerDocument();
		tmpElt = doc->createTextNode(b64Buf.sbStrToXMLCh());
		mp_signatureValueNode->appendChild(tmpElt);
	}
	else {
		tmpElt->setNodeValue(b64Buf.sbStrToXMLCh());
	}

	// And copy to the local buffer
	m_signatureValueSB = b64Buf;
	
}

// --------------------------------------------------------------------------------
//           Key Management
// --------------------------------------------------------------------------------

void DSIGSignature::setSigningKey(XSECCryptoKey *k) {

	if (mp_signingKey != NULL)
		delete mp_signingKey;

	mp_signingKey = k;

}

// --------------------------------------------------------------------------------
//           ID Handling
// --------------------------------------------------------------------------------

/*
 * ID handling is really all done within the environment object - just pass the
 * calls straight through
 */

void DSIGSignature::setIdByAttributeName(bool flag) {

	mp_env->setIdByAttributeName(flag);

}

bool DSIGSignature::getIdByAttributeName(void) const {

	return mp_env->getIdByAttributeName();

}


void DSIGSignature::registerIdAttributeName(const XMLCh * name) {

	mp_env->registerIdAttributeName(name);

}

bool DSIGSignature::deregisterIdAttributeName(const XMLCh * name) {

	return mp_env->deregisterIdAttributeName(name);

}

void DSIGSignature::registerIdAttributeNameNS(const XMLCh * ns, const XMLCh * name) {

	mp_env->registerIdAttributeNameNS(ns, name);

}

bool DSIGSignature::deregisterIdAttributeNameNS(const XMLCh * ns, const XMLCh * name) {

	return mp_env->deregisterIdAttributeNameNS(ns, name);

}

// --------------------------------------------------------------------------------
//           Other functions
// --------------------------------------------------------------------------------

const XMLCh * DSIGSignature::getSignatureValue(void) const {

	if (mp_signatureValueNode == NULL)
		return NULL;

	return findFirstChildOfType(mp_signatureValueNode, DOMNode::TEXT_NODE)->getNodeValue();

}

