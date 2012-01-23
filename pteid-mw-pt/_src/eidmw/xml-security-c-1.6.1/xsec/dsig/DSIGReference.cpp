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
 * DSIG_Reference := Class for handling a DSIG reference element
 *
 * $Id: DSIGReference.cpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

// XSEC includes

#include <xsec/dsig/DSIGReference.hpp>
#include <xsec/transformers/TXFMChain.hpp>
#include <xsec/transformers/TXFMURL.hpp>
#include <xsec/transformers/TXFMDocObject.hpp>
#include <xsec/transformers/TXFMOutputFile.hpp>
#include <xsec/transformers/TXFMSHA1.hpp>
#include <xsec/transformers/TXFMMD5.hpp>
#include <xsec/transformers/TXFMBase64.hpp>
#include <xsec/transformers/TXFMSB.hpp>
#include <xsec/transformers/TXFMXPath.hpp>
#include <xsec/transformers/TXFMC14n.hpp>
#include <xsec/transformers/TXFMXSL.hpp>
#include <xsec/transformers/TXFMEnvelope.hpp>
#include <xsec/dsig/DSIGConstants.hpp>
#include <xsec/dsig/DSIGSignature.hpp>
#include <xsec/dsig/DSIGTransformList.hpp>
#include <xsec/dsig/DSIGTransformBase64.hpp>
#include <xsec/dsig/DSIGTransformEnvelope.hpp>
#include <xsec/dsig/DSIGTransformXPath.hpp>
#include <xsec/dsig/DSIGTransformXPathFilter.hpp>
#include <xsec/dsig/DSIGTransformXSL.hpp>
#include <xsec/dsig/DSIGTransformC14n.hpp>

#include <xsec/framework/XSECError.hpp>
#include <xsec/framework/XSECEnv.hpp>
#include <xsec/framework/XSECAlgorithmHandler.hpp>
#include <xsec/framework/XSECAlgorithmMapper.hpp>
#include <xsec/utils/XSECPlatformUtils.hpp>
#include <xsec/utils/XSECDOMUtils.hpp>
#include <xsec/utils/XSECBinTXFMInputStream.hpp>

// Xerces

#include <xercesc/util/XMLNetAccessor.hpp>
#include <xercesc/util/XMLUniDefs.hpp>
#include <xercesc/util/Janitor.hpp>

XERCES_CPP_NAMESPACE_USE

#include <iostream>

// --------------------------------------------------------------------------------
//           Some useful strings
// --------------------------------------------------------------------------------


static const XMLCh  s_unicodeStrURI[] =
{
        XERCES_CPP_NAMESPACE_QUALIFIER chLatin_U,
		XERCES_CPP_NAMESPACE_QUALIFIER chLatin_R,
		XERCES_CPP_NAMESPACE_QUALIFIER chLatin_I,
		XERCES_CPP_NAMESPACE_QUALIFIER chNull
};

static const XMLCh  s_unicodeStrxpointer[] =
{
        XERCES_CPP_NAMESPACE_QUALIFIER chLatin_x,
		XERCES_CPP_NAMESPACE_QUALIFIER chLatin_p,
		XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o,
		XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i,
		XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n,
		XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t,
		XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e,
		XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r,
		XERCES_CPP_NAMESPACE_QUALIFIER chNull
};

static const XMLCh s_unicodeStrRootNode[] =
{
		XERCES_CPP_NAMESPACE_QUALIFIER chOpenParen,
		XERCES_CPP_NAMESPACE_QUALIFIER chForwardSlash,
		XERCES_CPP_NAMESPACE_QUALIFIER chCloseParen,
		XERCES_CPP_NAMESPACE_QUALIFIER chNull
};

// --------------------------------------------------------------------------------
//           Constructors and Destructors
// --------------------------------------------------------------------------------


DSIGReference::DSIGReference(const XSECEnv * env, DOMNode *dom) {

	mp_referenceNode = dom;
	mp_env = env;

	// Should throw an exception if the node is not a REFERENCE element

	XSECnew(mp_formatter, XSECSafeBufferFormatter("UTF-8",XMLFormatter::NoEscapes,
												XMLFormatter::UnRep_CharRef));

	mp_preHash = NULL;
	mp_manifestList = NULL;
	me_hashMethod = HASH_NONE;
	mp_transformsNode = NULL;
	mp_transformList = NULL;
	mp_URI = NULL;
	m_isManifest = false;
	mp_algorithmURI = NULL;
	m_loaded = false;

}

DSIGReference::DSIGReference(const XSECEnv * env) {

	mp_env = env;
	mp_referenceNode = NULL;
	mp_transformsNode = NULL;
	mp_transformList = NULL;

	XSECnew(mp_formatter, XSECSafeBufferFormatter("UTF-8",XMLFormatter::NoEscapes,
											XMLFormatter::UnRep_CharRef));

	mp_preHash = NULL;		// By default no "special" transform
	mp_manifestList = NULL;
	me_hashMethod = HASH_NONE;
	mp_URI = NULL;
	m_isManifest = false;
	mp_algorithmURI = NULL;
	m_loaded = false;

};

DSIGReference::~DSIGReference() {

	// Destroy any associated transforms

	if (mp_transformList != NULL) {

		delete mp_transformList;
		mp_transformList = NULL;

	}

	if (mp_formatter != NULL)
		delete mp_formatter;

	if (mp_manifestList != NULL)
		delete mp_manifestList;

};

// --------------------------------------------------------------------------------
//           Creation of Transforms
// --------------------------------------------------------------------------------

void DSIGReference::createTransformList(void) {

	// Creates the transforms list
	safeBuffer str;
	const XMLCh * prefix;
	DOMDocument *doc = mp_env->getParentDocument();

	prefix = mp_env->getDSIGNSPrefix();

	if (mp_transformsNode == NULL) {

		// Need to create a transforms node
		makeQName(str, prefix, "Transforms");
		mp_transformsNode = doc->createElementNS(DSIGConstants::s_unicodeStrURIDSIG, str.rawXMLChBuffer());
		mp_referenceNode->insertBefore(mp_transformsNode, mp_referenceNode->getFirstChild());
		if (mp_env->getPrettyPrintFlag() == true)
			mp_referenceNode->insertBefore(doc->createTextNode(DSIGConstants::s_unicodeStrNL), mp_transformsNode);
		mp_env->doPrettyPrint(mp_transformsNode);

		// Create the list
		XSECnew(mp_transformList, DSIGTransformList());
	}

}

void DSIGReference::addTransform(DSIGTransform * txfm, DOMElement * txfmElt) {

	if (mp_transformList == NULL)
		createTransformList();

	mp_transformsNode->appendChild(txfmElt);
	mp_env->doPrettyPrint(mp_transformsNode);

	mp_transformList->addTransform(txfm);
}


DSIGTransformEnvelope * DSIGReference::appendEnvelopedSignatureTransform() {

	DOMElement *txfmElt;
	DSIGTransformEnvelope * txfm;

	XSECnew(txfm, DSIGTransformEnvelope(mp_env));
	txfmElt = txfm->createBlankTransform(mp_env->getParentDocument());

	addTransform(txfm, txfmElt);

	return txfm;

}

DSIGTransformBase64 * DSIGReference::appendBase64Transform() {

	DOMElement *txfmElt;
	DSIGTransformBase64 * txfm;

	XSECnew(txfm, DSIGTransformBase64(mp_env));
	txfmElt = txfm->createBlankTransform(mp_env->getParentDocument());

	addTransform(txfm, txfmElt);

	return txfm;

}

DSIGTransformXSL * DSIGReference::appendXSLTransform(DOMNode * stylesheet) {

	DOMElement *txfmElt;
	DSIGTransformXSL * txfm;

	XSECnew(txfm, DSIGTransformXSL(mp_env));
	txfmElt = txfm->createBlankTransform(mp_env->getParentDocument());
	txfm->setStylesheet(stylesheet);

	addTransform(txfm, txfmElt);

	return txfm;

}

DSIGTransformC14n * DSIGReference::appendCanonicalizationTransform(canonicalizationMethod cm) {

	DOMElement *txfmElt;
	DSIGTransformC14n * txfm;

	XSECnew(txfm, DSIGTransformC14n(mp_env));
	txfmElt = txfm->createBlankTransform(mp_env->getParentDocument());
	txfm->setCanonicalizationMethod(cm);

	addTransform(txfm, txfmElt);

	return txfm;

}

DSIGTransformC14n * DSIGReference::appendCanonicalizationTransform(
		const XMLCh * canonicalizationAlgorithmURI) {

	canonicalizationMethod cm;
	XSECmapURIToCanonicalizationMethod(canonicalizationAlgorithmURI, cm);

	return appendCanonicalizationTransform(cm);

}

DSIGTransformXPath * DSIGReference::appendXPathTransform(const char * expr) {

	DOMElement *txfmElt;
	DSIGTransformXPath * txfm;

	XSECnew(txfm, DSIGTransformXPath(mp_env));
	txfmElt = txfm->createBlankTransform(mp_env->getParentDocument());
	txfm->setExpression(expr);

	addTransform(txfm, txfmElt);

	return txfm;
}

DSIGTransformXPathFilter * DSIGReference::appendXPathFilterTransform(void) {

	DOMElement *txfmElt;
	DSIGTransformXPathFilter * txfm;

	XSECnew(txfm, DSIGTransformXPathFilter(mp_env));
	txfmElt = txfm->createBlankTransform(mp_env->getParentDocument());

	addTransform(txfm, txfmElt);
	mp_env->doPrettyPrint(txfmElt);

	return txfm;
}



// --------------------------------------------------------------------------------
//           Creation of blanks
// --------------------------------------------------------------------------------


DOMElement *DSIGReference::createBlankReference(const XMLCh * URI, hashMethod hm, char * type) {

	// Deprecated - use the algorithm URI based method instead

	safeBuffer hURI;

	if (hashMethod2URI(hURI, hm) == false) {
		throw XSECException(XSECException::UnknownSignatureAlgorithm,
			"DSIGReference::createBlankReference - Hash method unknown");
	}

	return createBlankReference(URI, hURI.sbStrToXMLCh(), MAKE_UNICODE_STRING(type));

}

DOMElement *DSIGReference::createBlankReference(const XMLCh * URI,
												const XMLCh * hashAlgorithmURI,
												const XMLCh * type) {

	// Reset this Reference just in case

	m_isManifest = false;
	mp_preHash = NULL;
	mp_manifestList = NULL;
	mp_transformsNode = NULL;
	mp_transformList = NULL;

	XSECmapURIToHashMethod(hashAlgorithmURI, me_hashMethod);

	safeBuffer str;
	DOMDocument *doc = mp_env->getParentDocument();
	const XMLCh * prefix = mp_env->getDSIGNSPrefix();

	makeQName(str, prefix, "Reference");

	DOMElement *ret = doc->createElementNS(DSIGConstants::s_unicodeStrURIDSIG, str.rawXMLChBuffer());
	mp_referenceNode = ret;

	// Set type
	if (type != NULL)
		ret->setAttributeNS(NULL, MAKE_UNICODE_STRING("Type"), type);

	// Set URI
	if (URI != NULL) {
		ret->setAttributeNS(NULL, s_unicodeStrURI, URI);
		mp_URI = ret->getAttributeNS(NULL, s_unicodeStrURI); // Used later on as a pointer
	}
	else {
		// Anonymous reference
		mp_URI = NULL;
	}

	// Create hash and hashValue nodes
	makeQName(str, prefix, "DigestMethod");
	DOMElement *digestMethod = doc->createElementNS(DSIGConstants::s_unicodeStrURIDSIG, str.rawXMLChBuffer());
	mp_env->doPrettyPrint(ret);
	ret->appendChild(digestMethod);
	mp_env->doPrettyPrint(ret);

	digestMethod->setAttributeNS(NULL, DSIGConstants::s_unicodeStrAlgorithm,
		hashAlgorithmURI);

	// Retrieve the attribute value for later use
	mp_algorithmURI =
		digestMethod->getAttributeNS(NULL, DSIGConstants::s_unicodeStrAlgorithm);


	// DigestValue

	makeQName(str, prefix, "DigestValue");
	mp_hashValueNode = doc->createElementNS(DSIGConstants::s_unicodeStrURIDSIG, str.rawXMLChBuffer());
	ret->appendChild(mp_hashValueNode);
	mp_env->doPrettyPrint(ret);
	mp_hashValueNode->appendChild(doc->createTextNode(MAKE_UNICODE_STRING("Not yet calculated")));

	m_loaded = true;
	return ret;

}

// --------------------------------------------------------------------------------
//           setPreHashTransform - to set a transform to be used pre-hash
// --------------------------------------------------------------------------------


void DSIGReference::setPreHashTXFM(TXFMBase * t) {

	mp_preHash = t;

}

//           setId
// --------------------------------------------------------------------------------

void DSIGReference::setId(const XMLCh *id) {

    if (mp_referenceNode)
        ((DOMElement*)mp_referenceNode)->setAttributeNS(NULL, MAKE_UNICODE_STRING("Id"), id);

}

// --------------------------------------------------------------------------------
//           setType
// --------------------------------------------------------------------------------

void DSIGReference::setType(const XMLCh *type) {

    if (mp_referenceNode)
        ((DOMElement*)mp_referenceNode)->setAttributeNS(NULL, MAKE_UNICODE_STRING("Type"), type);

}

// --------------------------------------------------------------------------------

// --------------------------------------------------------------------------------
//           isManifest
// --------------------------------------------------------------------------------

bool DSIGReference::isManifest() {

	return m_isManifest;

}

// --------------------------------------------------------------------------------
//           getURI
// --------------------------------------------------------------------------------

const XMLCh * DSIGReference::getURI() {

	return mp_URI;

}

// --------------------------------------------------------------------------------
//           getManifestReferenceList
// --------------------------------------------------------------------------------

DSIGReferenceList * DSIGReference::getManifestReferenceList() {

	return mp_manifestList;

}

// --------------------------------------------------------------------------------
//           getURIBaseTransform
// --------------------------------------------------------------------------------

TXFMBase * DSIGReference::getURIBaseTXFM(DOMDocument * doc,
										 const XMLCh * URI,
										 const XSECEnv * env) {

	// Determine if this is a full URL or a pointer to a URL

	if (URI == NULL || (URI[0] != 0 &&
		URI[0] != XERCES_CPP_NAMESPACE_QUALIFIER chPound)) {

		TXFMURL * retTransform;

		// Have a URL!

		XSECnew(retTransform, TXFMURL(doc, env->getURIResolver()));

		try {
			((TXFMURL *) retTransform)->setInput(URI);
		}
		catch (...) {

			delete retTransform;
			throw;

		}

		return retTransform;

	}

	// Have a fragment URI from the local document
	TXFMDocObject * to;
	XSECnew(to, TXFMDocObject(doc));
	Janitor<TXFMDocObject> j_to(to);
	to->setEnv(env);

	// Find out what sort of object pointer this is.

	if (URI[0] == 0) {

		// empty pointer - use the document itself
		to->setInput(doc);
		to->stripComments();

	}

	else if (XMLString::compareNString(&URI[1], s_unicodeStrxpointer, 8) == 0) {

		// Have an xpointer
		if (strEquals(s_unicodeStrRootNode, &URI[9]) == true) {
			// Root node
			to->setInput(doc);

		}

		else if (URI[9] == XERCES_CPP_NAMESPACE_QUALIFIER chOpenParen &&
			     URI[10] == XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i &&
				 URI[11] == XERCES_CPP_NAMESPACE_QUALIFIER chLatin_d &&
				 URI[12] == XERCES_CPP_NAMESPACE_QUALIFIER chOpenParen &&
				 URI[13] == XERCES_CPP_NAMESPACE_QUALIFIER chSingleQuote) {

			xsecsize_t len = XMLString::stringLen(&URI[14]);

			XMLCh tmp[512];

			if (len > 511)
				len = 511;

			xsecsize_t j = 14, i = 0;

			// Have an ID
			while (URI[j] != '\'') {
				tmp[i++] = URI[j++];
			}
			tmp[i] = XERCES_CPP_NAMESPACE_QUALIFIER chNull;

			to->setInput(doc, tmp);

		}

		else {

			throw XSECException(XSECException::UnsupportedXpointerExpr);

		}

		// Keep comments in these situations
		to->activateComments();
	}

	else {

		to->setInput(doc, &URI[1]);

		// Remove comments
		to->stripComments();

	}

	j_to.release();
	return to;

}

// --------------------------------------------------------------------------------
//           load
// --------------------------------------------------------------------------------

void DSIGReference::load(void) {

	// Load reference info from XML

	DOMNamedNodeMap *atts = mp_referenceNode->getAttributes();
	DOMNode *tmpElt;

	const XMLCh * name;
	safeBuffer sbName;

	if (atts != 0) {

		XMLSize_t size = atts->getLength();

		for (XMLSize_t i = 0; i < size; ++i) {

			name = atts->item(i)->getNodeName();
			sbName << (*mp_formatter << atts->item(i)->getNodeName());

			if (strEquals(name, s_unicodeStrURI)) {
				mp_URI = atts->item(i)->getNodeValue();
			}

			else if (strEquals(name, "Type")) {

				// Check if a manifest, otherwise ignore for now
				if (strEquals(atts->item(i)->getNodeValue(), DSIGConstants::s_unicodeStrURIMANIFEST))
					m_isManifest = true;

			}

			else if (strEquals(name, "Id")) {

				// For now ignore

			}

			else if (sbName.sbStrncmp("xmlns", 5) == 0) {

				// Ignore name spaces

			}

			else {
				//safeBuffer tmp, error;

				//error << (*mp_formatter << name);
				//tmp.sbStrcpyIn("Unknown attribute in <Reference> Element : ");
				//tmp.sbStrcatIn(error);

				throw XSECException(XSECException::UnknownDSIGAttribute,
					"Unknown attribute in <Reference> Element");

			}

		}

	}

	// Now check for Transforms
	tmpElt = mp_referenceNode->getFirstChild();

	while (tmpElt != 0 && (tmpElt->getNodeType() != DOMNode::ELEMENT_NODE))
		// Skip text and comments
		tmpElt = tmpElt->getNextSibling();

	if (tmpElt == 0) {

			throw XSECException(XSECException::ExpectedDSIGChildNotFound,
					"Expected <Transforms> or <DigestMethod> within <Reference>");

	}

	if (strEquals(getDSIGLocalName(tmpElt), "Transforms")) {

		// Store node for later use
		mp_transformsNode = tmpElt;

		// Load the transforms
		mp_transformList = loadTransforms(tmpElt, mp_formatter, mp_env);

		// Find next node
		tmpElt = tmpElt->getNextSibling();
		while (tmpElt != 0 && (tmpElt->getNodeType() != DOMNode::ELEMENT_NODE))
			tmpElt = tmpElt->getNextSibling();


	} /* if tmpElt node type = transforms */
	else
		mp_transformList = NULL;


	if (tmpElt == NULL || !strEquals(getDSIGLocalName(tmpElt), "DigestMethod")) {

		throw XSECException(XSECException::ExpectedDSIGChildNotFound,
							"Expected <DigestMethod> element");

	}

	// Determine what the digest method actually is

	atts = tmpElt->getAttributes();
	unsigned int i;

	for (i = 0; i < atts->getLength() &&
		!strEquals(atts->item(i)->getNodeName(), DSIGConstants::s_unicodeStrAlgorithm); ++i);


	if (i == atts->getLength()) {

		throw XSECException(XSECException::ExpectedDSIGChildNotFound,
							"Expected 'Algorithm' attribute in <DigestMethod>");

	}

	mp_algorithmURI = atts->item(i)->getNodeValue();

	// Determine hashing algorithm
	XSECmapURIToHashMethod(mp_algorithmURI, me_hashMethod);

	// Find the hash value node

	tmpElt = tmpElt->getNextSibling();

	while (tmpElt != 0 && !(strEquals(getDSIGLocalName(tmpElt), "DigestValue")))
		tmpElt = tmpElt->getNextSibling();

	if (tmpElt == 0) {

		throw XSECException(XSECException::ExpectedDSIGChildNotFound,
			"Expected <DigestValue> within <Reference>");

	}

	mp_hashValueNode = tmpElt;

	// If we are a manifest, then we need to load the manifest references

	if (m_isManifest) {

		// Find the manifest node - we cheat and use a transform
		TXFMBase				* docObject;
		DOMNode					* manifestNode, * referenceNode;

		docObject = getURIBaseTXFM(mp_referenceNode->getOwnerDocument(), mp_URI,
			mp_env);

		manifestNode = docObject->getFragmentNode();
		delete docObject;

		// Now search downwards to find a <Manifest>
		if (manifestNode == 0 || manifestNode->getNodeType() != DOMNode::ELEMENT_NODE ||
			(!strEquals(getDSIGLocalName(manifestNode), "Object") && !strEquals(getDSIGLocalName(manifestNode), "Manifest"))) {

			throw XSECException(XSECException::ExpectedDSIGChildNotFound,
				"Expected <Manifest> or <Object> URI for Manifest Type <Reference>");

		}

		if (strEquals(getDSIGLocalName(manifestNode), "Object")) {

			// Find Manifest child
			manifestNode = manifestNode->getFirstChild();
			while (manifestNode != 0 && manifestNode->getNodeType() != DOMNode::ELEMENT_NODE)
				manifestNode = manifestNode->getNextSibling();

			if (manifestNode == 0 || !strEquals(getDSIGLocalName(manifestNode), "Manifest"))
				throw XSECException(XSECException::ExpectedDSIGChildNotFound,
				"Expected <Manifest> as child of <Object> for Manifest Type <Reference>");

		}

		// Now have the manifest node, find the first reference and load!
		referenceNode = manifestNode->getFirstChild();

		while (referenceNode != 0 && !strEquals(getDSIGLocalName(referenceNode), "Reference"))
			referenceNode = referenceNode->getNextSibling();

		if (referenceNode == 0)
			throw XSECException(XSECException::ExpectedDSIGChildNotFound,
			"Expected <Reference> as child of <Manifest>");

		// Have reference node, so lets create a list!
		mp_manifestList = DSIGReference::loadReferenceListFromXML(mp_env, referenceNode);

	} /* m_isManifest */

	m_loaded = true;

}

// --------------------------------------------------------------------------------
//           createReferenceListFromXML
// --------------------------------------------------------------------------------

DSIGReferenceList *DSIGReference::loadReferenceListFromXML(const XSECEnv * env, DOMNode *firstReference) {

	// Have the first reference element in the document,
	// so want to find and load them all

	DOMNode *tmpRef = firstReference;
	DSIGReferenceList * refList;
	DSIGReference * r;

	XSECnew(refList, DSIGReferenceList());
	Janitor<DSIGReferenceList> j_refList(refList);

	while (tmpRef != 0) {

		// Must be an element node

		if (tmpRef->getNodeType() != DOMNode::ELEMENT_NODE ||
			!strEquals(getDSIGLocalName(tmpRef), "Reference")) {

			throw XSECException(XSECException::ExpectedDSIGChildNotFound,
				"Expected <Reference> as child of <SignedInfo>");

		}

		XSECnew(r, DSIGReference(env, tmpRef));

		refList->addReference(r);

		// Load the reference before moving on
		r->load();

		// Find next element Node
		tmpRef = tmpRef->getNextSibling();

		while (tmpRef != 0 && tmpRef->getNodeType() != DOMNode::ELEMENT_NODE)
			tmpRef = tmpRef->getNextSibling();

	}

	j_refList.release();
	return refList;

}
// --------------------------------------------------------------------------------
//           Get the Canonicalised BYTE_STREAM of the reference data (and TXFMS)
// --------------------------------------------------------------------------------

XSECBinTXFMInputStream * DSIGReference::makeBinInputStream(void) const {

	// First set up for input

	TXFMChain * txfmChain;
	TXFMBase * currentTxfm;

	if (m_loaded == false) {

		throw XSECException(XSECException::NotLoaded,
			"calculateHash() called in DSIGReference before load()");

	}

	// Find base transform
	currentTxfm = getURIBaseTXFM(mp_referenceNode->getOwnerDocument(), mp_URI,
		mp_env);

	// Set up the transform chain

	txfmChain = createTXFMChainFromList(currentTxfm, mp_transformList);
	Janitor<TXFMChain> j_txfmChain(txfmChain);

	DOMDocument *d = mp_referenceNode->getOwnerDocument();

	// All transforms done.  If necessary, change the type from nodes to bytes

	if (txfmChain->getLastTxfm()->getOutputType() == TXFMBase::DOM_NODES) {

		TXFMC14n * c14n;
		XSECnew(c14n, TXFMC14n(d));
		txfmChain->appendTxfm(c14n);

	}

	// Now create the InputStream

	XSECBinTXFMInputStream * ret;
	ret = new XSECBinTXFMInputStream(txfmChain);	// Probs with MSVC++ mean no XSECnew
	j_txfmChain.release();		// Now owned by "ret"

	return ret;

}

// --------------------------------------------------------------------------------
//           Hash a reference list
// --------------------------------------------------------------------------------


void DSIGReference::hashReferenceList(DSIGReferenceList *lst, bool interlocking) {

// Run through a list of hashes and checkHash for each one

	DSIGReference * r;
	int i = (int) lst->getSize();
	safeBuffer errStr;
	errStr.sbXMLChIn(DSIGConstants::s_unicodeStrEmpty);

	// Run a VERY naieve process at the moment that assumes the list will "settle"
	// after N iterations through the list.  This will settle any inter-locking references
	// Where a hash in a later calculated reference could impact an already calculated hash
	// in a previous references
	//
	// If interlocking is set to false, assume there are no interacting <Reference> nodes

	do {

		for (int j = 0; j < i; ++j) {

			r = lst->item(j);

			// If this is a manifest we need to set all the references in the manifest as well

			if (r->isManifest())
				hashReferenceList(r->getManifestReferenceList());

			// Re-ordered as per suggestion by Peter Gubis to make it more likely
			// that hashes are correct on first pass when manifests are involved

			r->setHash();

		}

	} while (interlocking && !DSIGReference::verifyReferenceList(lst, errStr) && i-- >= 0);
}

// --------------------------------------------------------------------------------
//           Verify reference list
// --------------------------------------------------------------------------------

bool DSIGReference::verifyReferenceList(DSIGReferenceList * lst, safeBuffer &errStr) {

	// Run through a list of hashes and checkHash for each one

	XSEC_USING_XERCES(NetAccessorException);

	DSIGReference * r;
	bool res = true;

	int size = (lst ? (int) lst->getSize() : 0);

	for (int i = 0; i < size; ++i) {

		r = lst->item(i);

		try {
			if (!r->checkHash()) {

				// Failed
				errStr.sbXMLChCat("Reference URI=\"");
				errStr.sbXMLChCat(r->getURI());
				errStr.sbXMLChCat("\" failed to verify\n");

				res = false;

			}
		}
		catch (NetAccessorException e) {

			res = false;

			errStr.sbXMLChCat("Error accessing network URI=\"");
			errStr.sbXMLChCat(r->getURI());
			errStr.sbXMLChCat("\".  Reference failed to verify\n");

		}
		catch (XSECException e) {

			if (e.getType() != XSECException::HTTPURIInputStreamError)
				throw;

			res = false;

			errStr.sbXMLChCat("Error accessing network URI=\"");
			errStr.sbXMLChCat(r->getURI());
			errStr.sbXMLChCat("\".  Reference failed to verify\n");

		}

		// if a manifest, check the manifest list
		if (r->isManifest())
			res = res & verifyReferenceList(r->getManifestReferenceList(), errStr);

	}

	return res;
}

// --------------------------------------------------------------------------------
//           processTransforms
// --------------------------------------------------------------------------------

TXFMChain * DSIGReference::createTXFMChainFromList(TXFMBase * input,
							DSIGTransformList * lst) {

	TXFMChain * ret;
	XSECnew(ret, TXFMChain(input));

	if (lst == NULL)
		return ret;

	Janitor<TXFMChain> j_ret(ret);

	DSIGTransformList::TransformListVectorType::size_type size, i;

	size = lst->getSize();

	if (size > 0) {

		// Iterate through the list

		for (i = 0; i < size; ++i) {

			lst->item(i)->appendTransformer(ret);

		}

	}

	j_ret.release();
	return ret;

}

// --------------------------------------------------------------------------------
//           loadTransforms
// --------------------------------------------------------------------------------

DSIGTransformList * DSIGReference::loadTransforms(
							DOMNode *transformsNode,
							XSECSafeBufferFormatter * formatter,
							const XSECEnv * env) {

	// This is defined as a static function, not because it makes use of any static variables
	// in the DSIGReference class, but to neatly link it to the other users

	if (transformsNode == 0 || (!strEquals(getDSIGLocalName(transformsNode), "Transforms") &&
		!strEquals(getXENCLocalName(transformsNode), "Transforms"))) {

			throw XSECException(XSECException::ExpectedDSIGChildNotFound,
					"Expected <Transforms> in function DSIGReference::processTransforms");

	}

	// Create the list
	DSIGTransformList * lst;
	XSECnew(lst, DSIGTransformList());
	Janitor<DSIGTransformList> j_lst(lst);

	// Find First transform

	DOMNode * transforms = transformsNode->getFirstChild();
	while (transforms != NULL && transforms->getNodeType() != DOMNode::ELEMENT_NODE)
		transforms = transforms->getNextSibling();

	while (transforms != NULL) {

		// Process each transform in turn
		if (!strEquals(getDSIGLocalName(transforms), "Transform")) {

			// Not what we expected to see!
			safeBuffer tmp, error;

			error << (*formatter << getDSIGLocalName(transforms));
			tmp.sbStrcpyIn("Unknown attribute in <Transforms> - Expected <Transform> found ");
			tmp.sbStrcatIn(error);
			tmp.sbStrcatIn(">.");

			throw XSECException(XSECException::ExpectedDSIGChildNotFound, error.rawCharBuffer());

		}

		DOMNamedNodeMap * transformAtts = transforms->getAttributes();

		unsigned int i;

		for (i = 0; i < transformAtts->getLength() &&
				!strEquals(transformAtts->item(i)->getNodeName(), DSIGConstants::s_unicodeStrAlgorithm); ++i);

		if (i == transformAtts->getLength()) {

			throw XSECException(XSECException::ExpectedDSIGChildNotFound,
				"Algorithm attribute not found in <Transform> element");

		}

		safeBuffer algorithm;
		algorithm << (*formatter << transformAtts->item(i)->getNodeValue());

		// Determine what the transform is

		if (algorithm.sbStrcmp(URI_ID_BASE64) == 0) {

			DSIGTransformBase64 * b;
			XSECnew(b, DSIGTransformBase64(env, transforms));
			lst->addTransform(b);
			b->load();
		}

		else if (algorithm.sbStrcmp(URI_ID_XPATH) == 0) {

			DSIGTransformXPath * x;
			XSECnew(x, DSIGTransformXPath(env, transforms));
			lst->addTransform(x);
			x->load();
		}

		else if (algorithm.sbStrcmp(URI_ID_XPF) == 0) {

			DSIGTransformXPathFilter * xpf;

			XSECnew(xpf, DSIGTransformXPathFilter(env, transforms));
			lst->addTransform(xpf);
			xpf->load();

		}

		else if (algorithm.sbStrcmp(URI_ID_ENVELOPE) == 0) {

			DSIGTransformEnvelope * e;
			XSECnew(e, DSIGTransformEnvelope(env, transforms));
			lst->addTransform(e);
			e->load();
		}

		else if (algorithm.sbStrcmp(URI_ID_XSLT) == 0) {

			DSIGTransformXSL * x;
			XSECnew(x, DSIGTransformXSL(env, transforms));
			lst->addTransform(x);
			x->load();

		}


		else if (algorithm.sbStrcmp(URI_ID_C14N_COM) == 0 ||
			     algorithm.sbStrcmp(URI_ID_C14N_NOC) == 0 ||
			     algorithm.sbStrcmp(URI_ID_C14N11_COM) == 0 ||
                 algorithm.sbStrcmp(URI_ID_C14N11_NOC) == 0 ||
				 algorithm.sbStrcmp(URI_ID_EXC_C14N_COM) == 0 ||
				 algorithm.sbStrcmp(URI_ID_EXC_C14N_NOC) == 0) {

			DSIGTransformC14n * c;
			XSECnew(c, DSIGTransformC14n(env, transforms));
			lst->addTransform(c);
			c->load();

		}

		else {

			// Not what we expected to see!
			safeBuffer tmp;

			tmp.sbStrcpyIn("Unknown transform : ");
			tmp.sbStrcatIn(algorithm);
			tmp.sbStrcatIn(" found.");

			throw XSECException(XSECException::UnknownTransform, tmp.rawCharBuffer());
		}

		// Now find next element

		transforms = transforms->getNextSibling();
		while (transforms != NULL && transforms->getNodeType() != DOMNode::ELEMENT_NODE)
			transforms = transforms->getNextSibling();

	} /* while (transforms != NULL) */

	j_lst.release();
	return lst;
}

void DSIGReference::setExternalHash(XMLByte *hash)
{

	XMLByte base64Hash [CRYPTO_MAX_HASH_SIZE * 2];
	unsigned int base64HashLen;
	// Calculate the base64 value

	XSECCryptoBase64 *	b64 = XSECPlatformUtils::g_cryptoProvider->base64();

	if (!b64) {

		throw XSECException(XSECException::CryptoProviderError,
				"Error requesting Base64 object from Crypto Provider");

	}

	Janitor<XSECCryptoBase64> j_b64(b64);

	b64->encodeInit();
	base64HashLen = b64->encode(hash,
								20,  //SHA-1 Hash assumed
								base64Hash,
								CRYPTO_MAX_HASH_SIZE * 2);
	base64HashLen += b64->encodeFinish(&base64Hash[base64HashLen],
										(CRYPTO_MAX_HASH_SIZE * 2) - base64HashLen);

	// Ensure the string is terminated
	if (base64Hash[base64HashLen-1] == '\n')
		base64Hash[base64HashLen-1] = '\0';
	else
		base64Hash[base64HashLen] = '\0';

	// Now find the correct text node to re-set

	DOMNode *tmpElt = mp_hashValueNode;

	if (mp_hashValueNode == 0) {

		throw XSECException(XSECException::NotLoaded,
			"setHash() called in DSIGReference before load()");

	}

	tmpElt = mp_hashValueNode->getFirstChild();

	while (tmpElt != NULL && tmpElt->getNodeType() != DOMNode::TEXT_NODE)
		tmpElt = tmpElt->getNextSibling();

	if (tmpElt == NULL) {
		// Need to create the underlying TEXT_NODE
		DOMDocument *doc = mp_referenceNode->getOwnerDocument();
		tmpElt = doc->createTextNode(MAKE_UNICODE_STRING((char *) base64Hash));
		mp_hashValueNode->appendChild(tmpElt);
	}
	else {
		tmpElt->setNodeValue(MAKE_UNICODE_STRING((char *) base64Hash));
	}
	std::cerr << "Debug: setExternalHash() finished" << std::endl;
}

// --------------------------------------------------------------------------------
//           Set hash
// --------------------------------------------------------------------------------
//
void DSIGReference::setHash(void) {

	// First determine the hash value
	XMLByte calculatedHashVal[CRYPTO_MAX_HASH_SIZE];	// The hash that we determined
	unsigned int calculatedHashLen;
	XMLByte base64Hash [CRYPTO_MAX_HASH_SIZE * 2];
	unsigned int base64HashLen;

	calculatedHashLen = calculateHash(calculatedHashVal, CRYPTO_MAX_HASH_SIZE);

	// Calculate the base64 value

	XSECCryptoBase64 *	b64 = XSECPlatformUtils::g_cryptoProvider->base64();

	if (!b64) {

		throw XSECException(XSECException::CryptoProviderError,
				"Error requesting Base64 object from Crypto Provider");

	}

	Janitor<XSECCryptoBase64> j_b64(b64);

	b64->encodeInit();
	base64HashLen = b64->encode(calculatedHashVal,
								calculatedHashLen,
								base64Hash,
								CRYPTO_MAX_HASH_SIZE * 2);
	base64HashLen += b64->encodeFinish(&base64Hash[base64HashLen],
										(CRYPTO_MAX_HASH_SIZE * 2) - base64HashLen);

	// Ensure the string is terminated
	if (base64Hash[base64HashLen-1] == '\n')
		base64Hash[base64HashLen-1] = '\0';
	else
		base64Hash[base64HashLen] = '\0';

	// Now find the correct text node to re-set

	DOMNode *tmpElt = mp_hashValueNode;

	if (mp_hashValueNode == 0) {

		throw XSECException(XSECException::NotLoaded,
			"setHash() called in DSIGReference before load()");

	}

	tmpElt = mp_hashValueNode->getFirstChild();

	while (tmpElt != NULL && tmpElt->getNodeType() != DOMNode::TEXT_NODE)
		tmpElt = tmpElt->getNextSibling();

	if (tmpElt == NULL) {
		// Need to create the underlying TEXT_NODE
		DOMDocument *doc = mp_referenceNode->getOwnerDocument();
		tmpElt = doc->createTextNode(MAKE_UNICODE_STRING((char *) base64Hash));
		mp_hashValueNode->appendChild(tmpElt);
	}
	else {
		tmpElt->setNodeValue(MAKE_UNICODE_STRING((char *) base64Hash));
	}

}




// --------------------------------------------------------------------------------
//           Create hash
// --------------------------------------------------------------------------------

unsigned int DSIGReference::calculateHash(XMLByte *toFill, unsigned int maxToFill) {

	// Determine the hash value of the element

	// First set up for input

	TXFMBase * currentTxfm;
	TXFMChain * chain;

	unsigned int size;

	if (m_loaded == false) {

		throw XSECException(XSECException::NotLoaded,
			"calculateHash() called in DSIGReference before load()");

	}

	// Find base transform
	currentTxfm = getURIBaseTXFM(mp_referenceNode->getOwnerDocument(), mp_URI,
		mp_env);

	// Now build the transforms list
	// Note this passes ownership of currentTxfm to the function, so it is the
	// responsibility of createTXFMChain to ensure it gets deleted if this throws.

	chain = createTXFMChainFromList(currentTxfm, mp_transformList);
	Janitor<TXFMChain> j_chain(chain);

	DOMDocument *d = mp_referenceNode->getOwnerDocument();

	// All transforms done.  If necessary, change the type from nodes to bytes

	if (chain->getLastTxfm()->getOutputType() == TXFMBase::DOM_NODES) {

		TXFMC14n * c14n;
		XSECnew(c14n, TXFMC14n(d));
		chain->appendTxfm(c14n);

	}

	// Check to see if there is a final "application" transform prior to the hash

	if (mp_preHash != NULL) {

		chain->appendTxfm(mp_preHash);
		mp_preHash = NULL;	// Can't be re-used

	}

    // Check for debugging sink for the data
    TXFMBase* sink = XSECPlatformUtils::GetReferenceLoggingSink(d);
    if (sink)
        chain->appendTxfm(sink);


	// Get the mapping for the hash transform

	XSECAlgorithmHandler * handler =
		XSECPlatformUtils::g_algorithmMapper->mapURIToHandler(mp_algorithmURI);

	if (handler == NULL) {


		throw XSECException(XSECException::SigVfyError,
			"Hash method unknown in DSIGReference::calculateHash()");

	}

	if (!handler->appendHashTxfm(chain, mp_algorithmURI)) {

		throw XSECException(XSECException::SigVfyError,
			"Unexpected error in handler whilst appending Hash transform");

	}

	// Now we have the hashing transform, run it.

	size = chain->getLastTxfm()->readBytes(toFill, maxToFill);

	// Clean out document if necessary
	chain->getLastTxfm()->deleteExpandedNameSpaces();

	return size;

}

// --------------------------------------------------------------------------------
//           Read hash
// --------------------------------------------------------------------------------

unsigned int DSIGReference::readHash(XMLByte *toFill, unsigned int maxToFill) {

	// Determine the hash value stored in the reference

	// First set up for input

	unsigned int size;
	DOMNode *tmpElt;
	//const XMLCh * stringHash;

	TXFMBase * nextInput;

	DOMDocument *d = mp_referenceNode->getOwnerDocument();

	safeBuffer b64HashVal;

	// Find the hash value

	tmpElt = mp_referenceNode->getFirstChild();

	while (tmpElt != 0 && !strEquals(getDSIGLocalName(tmpElt), "DigestValue"))
		tmpElt = tmpElt->getNextSibling();

	if (tmpElt == NULL)
		// ERROR
		return 0;

	// Now read the DOMString of the hash

	tmpElt = tmpElt->getFirstChild();
	while (tmpElt != NULL && tmpElt->getNodeType() != DOMNode::TEXT_NODE)
		tmpElt = tmpElt->getNextSibling();

	if (tmpElt == NULL)
		// Something wrong with the underlying XML if no text was found
		throw XSECException(XSECException::NoHashFoundInDigestValue);

	b64HashVal << (*mp_formatter << tmpElt->getNodeValue());

	// Now have the value of the string - create a transform around it

	XSECnew(nextInput, TXFMSB(d));
	((TXFMSB *) nextInput)->setInput(b64HashVal);

	// Create a transform chain (really as a janitor for the entire list)
	TXFMChain * chain;
	XSECnew(chain, TXFMChain(nextInput));
	Janitor<TXFMChain> j_chain(chain);

	// Now create the base64 transform

	XSECnew(nextInput, TXFMBase64(d));
	chain->appendTxfm(nextInput);

	// Now get the value

	size = chain->getLastTxfm()->readBytes(toFill, maxToFill);

	// Clear any documentat modifications

	chain->getLastTxfm()->deleteExpandedNameSpaces();

	return size;

}



// --------------------------------------------------------------------------------
//           Check a reference
// --------------------------------------------------------------------------------

bool DSIGReference::checkHash() {

	// Determine the hash value of the element and check if matches that stored in the
	// DigestValue part of the element

	// First set up for input

	XMLByte calculatedHashVal[CRYPTO_MAX_HASH_SIZE];		// The hash that we determined
	XMLByte readHashVal[CRYPTO_MAX_HASH_SIZE];			// The hash in the element

	unsigned int calculatedHashSize, i;

	if ((calculatedHashSize = calculateHash(calculatedHashVal, CRYPTO_MAX_HASH_SIZE)) == 0)
		return false;

	if (readHash(readHashVal, CRYPTO_MAX_HASH_SIZE) != calculatedHashSize)
		return false;

	for (i = 0; i < calculatedHashSize; ++i) {
		if (calculatedHashVal[i] != readHashVal[i])
			return false;

	}

	// Got through with flying colours!
	return true;

}





