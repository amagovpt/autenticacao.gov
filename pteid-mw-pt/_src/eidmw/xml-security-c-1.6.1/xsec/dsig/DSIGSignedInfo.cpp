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
 * DSIGSignedInfo := Class for checking and setting up signed Info nodes in a DSIG signature
 *
 * $Id: DSIGSignedInfo.cpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

// XSEC Includes
#include <xsec/dsig/DSIGSignedInfo.hpp>
#include <xsec/dsig/DSIGReference.hpp>
#include <xsec/framework/XSECError.hpp>
#include <xsec/framework/XSECEnv.hpp>
#include <xsec/utils/XSECDOMUtils.hpp>
#include <xsec/dsig/DSIGSignature.hpp>

#include <xercesc/util/Janitor.hpp>

XERCES_CPP_NAMESPACE_USE

// Constructors and Destructors

DSIGSignedInfo::DSIGSignedInfo(DOMDocument *doc, 
		XSECSafeBufferFormatter * pFormatter, 
		DOMNode *signedInfoNode, const XSECEnv * env) {

	mp_doc = doc;
	m_HMACOutputLength = 0;
	mp_formatter = pFormatter;
	mp_signedInfoNode = signedInfoNode;
	m_signatureMethod = SIGNATURE_NONE;
	mp_env = env;
	mp_referenceList = NULL;
	m_loaded = false;

}

DSIGSignedInfo::DSIGSignedInfo(DOMDocument *doc, 
		XSECSafeBufferFormatter * pFormatter, 
		const XSECEnv * env) {

	mp_doc = doc;
	m_HMACOutputLength = 0;
	mp_formatter = pFormatter;
	mp_signedInfoNode = NULL;
	m_signatureMethod = SIGNATURE_NONE;
	mp_env = env;
	mp_referenceList = NULL;
	m_loaded = false;

}

DSIGSignedInfo::~DSIGSignedInfo() {

	mp_formatter = NULL;
	mp_env = NULL;
	if (mp_referenceList != NULL) {

		delete mp_referenceList;
		mp_referenceList = NULL;

	}

}

signatureMethod DSIGSignedInfo::getSignatureMethod(void) {

	return m_signatureMethod;

}

DOMNode * DSIGSignedInfo::getDOMNode() {

	return mp_signedInfoNode;

}

canonicalizationMethod DSIGSignedInfo::getCanonicalizationMethod(void) {

	return m_canonicalizationMethod;

}

hashMethod DSIGSignedInfo::getHashMethod() {

	return m_hashMethod;

}

int DSIGSignedInfo::getHMACOutputLength() {

	return m_HMACOutputLength;

}




// --------------------------------------------------------------------------------
//           Verify each reference element
// --------------------------------------------------------------------------------


bool DSIGSignedInfo::verify(safeBuffer &errStr) {

	return DSIGReference::verifyReferenceList(mp_referenceList, errStr);

}

// --------------------------------------------------------------------------------
//           Calculate and set hash values for each reference element
// --------------------------------------------------------------------------------

void DSIGSignedInfo::hash(bool interlockingReferences) {

	DSIGReference::hashReferenceList(mp_referenceList, interlockingReferences);

}

// --------------------------------------------------------------------------------
//           Create an empty reference in the signed info
// --------------------------------------------------------------------------------


DSIGReference * DSIGSignedInfo::createReference(const XMLCh * URI, 
								hashMethod hm, 
								char * type) {

	safeBuffer hURI;

	if (hashMethod2URI(hURI, hm) == false) {
		throw XSECException(XSECException::UnknownSignatureAlgorithm,
			"DSIGSignedInfo::createReference - Hash method unknown");
	}
	
	return createReference(URI, hURI.sbStrToXMLCh(), MAKE_UNICODE_STRING(type));;
}

DSIGReference * DSIGSignedInfo::createReference(
		const XMLCh * URI,
		const XMLCh * hashAlgorithmURI, 
		const XMLCh * type) {

	DSIGReference * ref;
	XSECnew(ref, DSIGReference(mp_env));
	Janitor<DSIGReference> j_ref(ref);

	DOMNode *refNode = ref->createBlankReference(URI, hashAlgorithmURI, type);

	// Add the node to the end of the childeren
	mp_signedInfoNode->appendChild(refNode);
	mp_env->doPrettyPrint(mp_signedInfoNode);

	// Add to the reference List
	j_ref.release();
	mp_referenceList->addReference(ref);

	return ref;
}

DSIGReference * DSIGSignedInfo::removeReference(DSIGReferenceList::size_type index) {

    DSIGReference* ret = mp_referenceList ? mp_referenceList->removeReference(index): NULL;
    if (ret && mp_signedInfoNode) {
        mp_signedInfoNode->removeChild(ret->mp_referenceNode);
        mp_env->doPrettyPrint(mp_signedInfoNode);
    }

    return ret;

}


// --------------------------------------------------------------------------------
//           Create an empty SignedInfo
// --------------------------------------------------------------------------------

// deprecated

DOMElement *DSIGSignedInfo::createBlankSignedInfo(canonicalizationMethod cm,
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

	return createBlankSignedInfo(cURI, sURI.sbStrToXMLCh());

}

DOMElement * DSIGSignedInfo::createBlankSignedInfo(
			const XMLCh * canonicalizationAlgorithmURI,
			const XMLCh * signatureAlgorithmURI) {

	safeBuffer str;
	const XMLCh * prefixNS = mp_env->getDSIGNSPrefix();

	makeQName(str, prefixNS, "SignedInfo");

	DOMElement *ret = mp_doc->createElementNS(DSIGConstants::s_unicodeStrURIDSIG, 
								str.rawXMLChBuffer());
	
	mp_signedInfoNode = ret;

	// Now create the algorithm parts
	XSECmapURIToCanonicalizationMethod(canonicalizationAlgorithmURI, m_canonicalizationMethod);
	XSECmapURIToSignatureMethods(signatureAlgorithmURI, m_signatureMethod, m_hashMethod);

	// Canonicalisation

	DOMElement *canMeth = mp_doc->createElementNS(DSIGConstants::s_unicodeStrURIDSIG, 
			makeQName(str, prefixNS, "CanonicalizationMethod").rawXMLChBuffer());

	mp_env->doPrettyPrint(mp_signedInfoNode);
	mp_signedInfoNode->appendChild(canMeth);
	mp_env->doPrettyPrint(mp_signedInfoNode);

	canMeth->setAttributeNS(NULL,DSIGConstants::s_unicodeStrAlgorithm,
		canonicalizationAlgorithmURI);

	// Now the SignatureMethod

	DOMElement *sigMeth = mp_doc->createElementNS(DSIGConstants::s_unicodeStrURIDSIG, 
			makeQName(str, prefixNS, "SignatureMethod").rawXMLChBuffer());

	mp_signedInfoNode->appendChild(sigMeth);
	mp_env->doPrettyPrint(mp_signedInfoNode);

	sigMeth->setAttributeNS(NULL, DSIGConstants::s_unicodeStrAlgorithm, 
		signatureAlgorithmURI);

	// Store the algorithm URI internally
	mp_algorithmURI = sigMeth->getAttributeNS(NULL, DSIGConstants::s_unicodeStrAlgorithm);

	// Create an empty reference list

	XSECnew(mp_referenceList, DSIGReferenceList());

	return ret;

}


// --------------------------------------------------------------------------------
//           Load the SignedInfo
// --------------------------------------------------------------------------------

void DSIGSignedInfo::load(void) {


	if (mp_signedInfoNode == 0) {

		// Attempt to load an empty signature element
		throw XSECException(XSECException::LoadEmptySignedInfo);

	}

	if (!strEquals(getDSIGLocalName(mp_signedInfoNode), "SignedInfo")) {

		throw XSECException(XSECException::LoadNonSignedInfo);

	}

	DOMNode * tmpSI = mp_signedInfoNode->getFirstChild();

	// Check for CanonicalizationMethod

	while (tmpSI != 0 && (tmpSI->getNodeType() != DOMNode::ELEMENT_NODE))
		// Skip text and comments
		tmpSI = tmpSI->getNextSibling();

	if (tmpSI == 0 || !strEquals(getDSIGLocalName(tmpSI), "CanonicalizationMethod")) {

		throw XSECException(XSECException::ExpectedDSIGChildNotFound, 
				"Expected <CanonicalizationMethod> as first child of <SignedInfo>");

	}

	// Determine what the canonicalization method is
	DOMNamedNodeMap *tmpAtts = tmpSI->getAttributes();

	DOMNode *algorithm = tmpAtts->getNamedItem(DSIGConstants::s_unicodeStrAlgorithm);

	if (algorithm == 0) {

		throw XSECException(XSECException::ExpectedDSIGChildNotFound,
					"Expected Algorithm attribute in <CanonicalizationMethod>");

	}

	safeBuffer tmpSB;

	tmpSB << (*mp_formatter << algorithm->getNodeValue());

	if (tmpSB.sbStrcmp(URI_ID_C14N_NOC) == 0) {

		m_canonicalizationMethod = CANON_C14N_NOC;

	}

	else if (tmpSB.sbStrcmp(URI_ID_C14N_COM) == 0) {

		m_canonicalizationMethod = CANON_C14N_COM;

	}

	else if (tmpSB.sbStrcmp(URI_ID_C14N11_NOC) == 0) {

        m_canonicalizationMethod = CANON_C14N11_NOC;

    }

    else if (tmpSB.sbStrcmp(URI_ID_C14N11_COM) == 0) {

        m_canonicalizationMethod = CANON_C14N11_COM;

    }

    else if (tmpSB.sbStrcmp(URI_ID_EXC_C14N_COM) == 0) {

		m_canonicalizationMethod = CANON_C14NE_COM;

	}

	else if (tmpSB.sbStrcmp(URI_ID_EXC_C14N_NOC) == 0) {

		m_canonicalizationMethod = CANON_C14NE_NOC;

	}

	else

		throw XSECException(XSECException::UnknownCanonicalization);

	// Now load the SignatureMethod

	tmpSI = tmpSI->getNextSibling();

	while (tmpSI != 0 && (tmpSI->getNodeType() != DOMNode::ELEMENT_NODE))
		// Skip text and comments
		tmpSI = tmpSI->getNextSibling();

	if (tmpSI == 0 || !strEquals(getDSIGLocalName(tmpSI), "SignatureMethod")) {

		throw XSECException(XSECException::ExpectedDSIGChildNotFound, 
				"Expected <SignatureMethod> as child of <SignedInfo>");
	}


	// Determine the algorithms used to sign this document

	tmpAtts = tmpSI->getAttributes();

	algorithm = tmpAtts->getNamedItem(DSIGConstants::s_unicodeStrAlgorithm);
	
	if (algorithm == 0) {

		throw XSECException(XSECException::ExpectedDSIGChildNotFound,
					"Expected Algorithm attribute in <SignatureMethod>");

	}
	
	mp_algorithmURI = algorithm->getNodeValue();

	/* NOTE - as of version 1.3.1 all code relating to parsing the algorithm 
	 * has been removed.  This should all be handled inside the algorithm mappers.
	 * Having code here restricts available algorithms, as this code is not extended for
	 * new algorthms.
	 */

	/* Look for maximum output value. Really only applies to HMACs, but as we no
	 * longer know at this point if this is an HMAC, we need to check. */

	DOMNode *tmpSOV = tmpSI->getFirstChild();
	while (tmpSOV != NULL && 
		tmpSOV->getNodeType() != DOMNode::ELEMENT_NODE && 
		!strEquals(getDSIGLocalName(tmpSOV), "HMACOutputLength"))
		tmpSOV = tmpSOV->getNextSibling();

	if (tmpSOV != NULL) {

		// Have a max output value!
		tmpSOV = tmpSOV->getFirstChild();
		while (tmpSOV != NULL && tmpSOV->getNodeType() != DOMNode::TEXT_NODE)
			tmpSOV = tmpSOV->getNextSibling();

		if (tmpSOV != NULL) {

			safeBuffer val;
			val << (*mp_formatter << tmpSOV->getNodeValue());
			m_HMACOutputLength = atoi((char *) val.rawBuffer());

		}
	}

	// Now look at references....

	tmpSI = tmpSI->getNextSibling();

	// Run through the rest of the elements until done

	while (tmpSI != 0 && (tmpSI->getNodeType() != DOMNode::ELEMENT_NODE))
		// Skip text and comments
		tmpSI = tmpSI->getNextSibling();

	if (tmpSI != NULL) {

		// Have an element node - should be a reference, so let's load the list

		mp_referenceList = DSIGReference::loadReferenceListFromXML(mp_env, tmpSI);

	}

}

