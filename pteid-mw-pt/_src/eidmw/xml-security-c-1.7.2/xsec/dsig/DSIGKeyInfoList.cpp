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
 * DSIGKeyInfoList := Class for Loading and storing a list of KeyInfo elements
 *
 * Author(s): Berin Lautenbach
 *
 * $Id: DSIGKeyInfoList.cpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

// XSEC Includes
#include <xsec/dsig/DSIGKeyInfoList.hpp>
#include <xsec/dsig/DSIGKeyInfoX509.hpp>
#include <xsec/dsig/DSIGKeyInfoName.hpp>
#include <xsec/dsig/DSIGKeyInfoValue.hpp>
#include <xsec/dsig/DSIGKeyInfoDEREncoded.hpp>
#include <xsec/dsig/DSIGKeyInfoPGPData.hpp>
#include <xsec/dsig/DSIGKeyInfoSPKIData.hpp>
#include <xsec/dsig/DSIGKeyInfoMgmtData.hpp>
#include <xsec/dsig/DSIGKeyInfoExt.hpp>
#include <xsec/framework/XSECError.hpp>
#include <xsec/utils/XSECDOMUtils.hpp>
#include <xsec/dsig/DSIGSignature.hpp>
#include <xsec/dsig/DSIGReference.hpp>
#include <xsec/dsig/DSIGTransformList.hpp>
#include <xsec/framework/XSECEnv.hpp>
#include <xsec/transformers/TXFMChain.hpp>
#include <xsec/transformers/TXFMBase.hpp>

#include "../xenc/impl/XENCEncryptedKeyImpl.hpp"

#include <xercesc/util/Janitor.hpp>

XERCES_CPP_NAMESPACE_USE

DSIGKeyInfoList::DSIGKeyInfoList(const XSECEnv * env) :
mp_env(env),
mp_keyInfoNode(NULL) {}

DSIGKeyInfoList::~DSIGKeyInfoList() {

	empty();

}

// Actions

void DSIGKeyInfoList::addKeyInfo(DSIGKeyInfo * ref) {

	m_keyInfoList.push_back(ref);

}

DSIGKeyInfo * DSIGKeyInfoList::removeKeyInfo(size_type index) {

	if (index < m_keyInfoList.size())
		return m_keyInfoList[index];

	return NULL;

}

size_t DSIGKeyInfoList::getSize() const {

	return m_keyInfoList.size();

}


DSIGKeyInfo * DSIGKeyInfoList::item(size_type index) {

	if (index < m_keyInfoList.size())
		return m_keyInfoList[index];

	return NULL;

}

const DSIGKeyInfo * DSIGKeyInfoList::item(size_type index) const {

	if (index < m_keyInfoList.size())
		return m_keyInfoList[index];

	return NULL;

}

void DSIGKeyInfoList::empty() {

	size_type i, s;
	s = getSize();

	for (i = 0; i < s; ++i)
		delete m_keyInfoList[i];

	m_keyInfoList.clear();

}

bool DSIGKeyInfoList::isEmpty() const {

		return (m_keyInfoList.size() == 0);

}

// --------------------------------------------------------------------------------
//           Add a KeyInfo based on XML DomNode source
// --------------------------------------------------------------------------------


bool DSIGKeyInfoList::addXMLKeyInfo(DOMNode *ki) {

	// return true if successful - does not throw if the node type is unknown

	if (ki == 0)
		return false;

	DSIGKeyInfo * k;

	if (strEquals(getDSIGLocalName(ki), "X509Data")) {

		// Have a certificate!
		XSECnew(k, DSIGKeyInfoX509(mp_env, ki));
	}

	else if (strEquals(getDSIGLocalName(ki), "KeyName")) {

		XSECnew(k, DSIGKeyInfoName(mp_env, ki));
	}

	else if (strEquals(getDSIGLocalName(ki), "KeyValue")) {

		XSECnew(k, DSIGKeyInfoValue(mp_env, ki));
	}

	else if (strEquals(getDSIGLocalName(ki), "PGPData")) {

		XSECnew(k, DSIGKeyInfoPGPData(mp_env, ki));
	}

	else if (strEquals(getDSIGLocalName(ki), "SPKIData")) {

		XSECnew(k, DSIGKeyInfoSPKIData(mp_env, ki));

	}

	else if (strEquals(getDSIGLocalName(ki), "MgmtData")) {

		XSECnew(k, DSIGKeyInfoMgmtData(mp_env, ki));

	}
    
    else if (strEquals(getDSIG11LocalName(ki), "DEREncodedKeyValue")) {

		XSECnew(k, DSIGKeyInfoDEREncoded(mp_env, ki));

	}

	else if (strEquals(getXENCLocalName(ki), "EncryptedKey")) {

		XSECnew(k, XENCEncryptedKeyImpl(mp_env, (DOMElement *) ki));

	}

	else {

	    XSECnew(k, DSIGKeyInfoExt(mp_env, ki));

	}

	// Now we know what the element type is - do the load and save

	try {
		k->load();
	}
	catch (...) {
		delete k;
		throw;
	}

	// Add
	this->addKeyInfo(k);

	return true;

}

// --------------------------------------------------------------------------------
//           Retrieve a complete KeyInfo list
// --------------------------------------------------------------------------------


bool DSIGKeyInfoList::loadListFromXML(DOMNode * node) {

	if (node == NULL || !strEquals(getDSIGLocalName(node), "KeyInfo")) {
		throw XSECException(XSECException::ExpectedDSIGChildNotFound,
			"DSIGKeyInfoList::loadListFromXML - expected KeyInfo node");
	}

	DOMNode *tmpKI = findFirstChildOfType(node, DOMNode::ELEMENT_NODE);

	while (tmpKI != 0) {

		// Find out what kind of KeyInfo child it is

		if (tmpKI != 0 && strEquals(getDSIGLocalName(tmpKI), "RetrievalMethod")) {

			// A reference to key information held elsewhere

			const XMLCh * URI = NULL;
			TXFMBase * currentTxfm;
			bool isRawX509 = false;

			DOMNamedNodeMap *atts = tmpKI->getAttributes();
			const XMLCh * name;
			XMLSize_t size;

			if (atts == 0 || (size = atts->getLength()) == 0)
				return true;

			for (XMLSize_t i = 0; i < size; ++i) {

				name = atts->item(i)->getNodeName();

				if (strEquals(name, "URI")) {
					URI  = atts->item(i)->getNodeValue();
				}

				else if (strEquals(name, "Type")) {

					// Check if this is a raw X509 cert
					if (strEquals(atts->item(i)->getNodeValue(), DSIGConstants::s_unicodeStrURIRawX509)) {
						isRawX509 = true;
					}

				}

				else if (strEquals(name, "Id")) {

					// For now ignore

				}

				else {
					safeBuffer tmp, error;

					error << (*mp_env->getSBFormatter() << name);
					tmp.sbStrcpyIn("Unknown attribute in <RetrievalMethod> Element : ");
					tmp.sbStrcatIn(error);

					throw XSECException(XSECException::UnknownDSIGAttribute, tmp.rawCharBuffer());

				}

			}

			if (isRawX509 == true) {

				if (URI == NULL) {

					throw XSECException(XSECException::ExpectedDSIGChildNotFound,
						"Expected to find a URI attribute in a rawX509RetrievalMethod KeyInfo");

				}

				DSIGKeyInfoX509 * x509;
				XSECnew(x509, DSIGKeyInfoX509(mp_env));
				x509->setRawRetrievalURI(URI);

				addKeyInfo(x509);

			}

			else {

				// Find base transform using the base URI
				currentTxfm = DSIGReference::getURIBaseTXFM(mp_env->getParentDocument(), URI, mp_env);
				TXFMChain * chain;
				XSECnew(chain, TXFMChain(currentTxfm));
				Janitor<TXFMChain> j_chain(chain);

				// Now check for transforms
				DOMNode * tmpTran = tmpKI->getFirstChild();

				while (tmpTran != 0 && (tmpTran->getNodeType() != DOMNode::ELEMENT_NODE))
					// Skip text and comments
				    tmpTran = tmpTran->getNextSibling();

				if (tmpTran != 0 && strEquals(getDSIGLocalName(tmpTran), "Transforms")) {


					// Process the transforms using the static function.
					// For the moment we don't really support remote KeyInfos, so
					// Just built the transform list, process it and then destroy it.

					DSIGTransformList * l = DSIGReference::loadTransforms(
					    tmpTran,
						mp_env->getSBFormatter(),
						mp_env);

					DSIGTransformList::TransformListVectorType::size_type size, i;
					size = l->getSize();
					for (i = 0; i < size; ++ i) {
						try {
							l->item(i)->appendTransformer(chain);
						}
						catch (...) {
							delete l;
							throw;
						}
					}

					delete l;

				}

				// Find out the type of the final transform and process accordingly

				TXFMBase::nodeType type = chain->getLastTxfm()->getNodeType();

				XSECXPathNodeList lst;
				const DOMNode * element;

				switch (type) {

				case TXFMBase::DOM_NODE_DOCUMENT :

					break;

				case TXFMBase::DOM_NODE_DOCUMENT_FRAGMENT :

					element = chain->getLastTxfm()->getFragmentNode();
					if (element != NULL)
						addXMLKeyInfo((DOMNode *) element);

					break;

				case TXFMBase::DOM_NODE_XPATH_NODESET :

					lst = chain->getLastTxfm()->getXPathNodeList();
					element = lst.getFirstNode();

					while (element != NULL) {

						// Try to add each element - just call KeyInfoList add as it will
						// do the check to see if it is a valid KeyInfo

						addXMLKeyInfo((DOMNode *) element);
						element = lst.getNextNode();

					}

					break;

				default :

					throw XSECException(XSECException::XPathError);

				}

				// Delete the transform chain
				chain->getLastTxfm()->deleteExpandedNameSpaces();

				// Janitor will clean up chain
			}

		} /* if getNodeName == Retrieval Method */

		// Now just run through each node type in turn to process "local" KeyInfos

		else {
            // This used to check the return value, and throw if the child was unknown.
            // Now, should handle all cases.
            addXMLKeyInfo(tmpKI);
		}

		if (tmpKI != NULL)
			tmpKI = tmpKI->getNextSibling();

		while (tmpKI != NULL && (tmpKI->getNodeType() != DOMNode::ELEMENT_NODE))
			tmpKI = tmpKI->getNextSibling();

	}

	return true;
}

// --------------------------------------------------------------------------------
//           Create new KeyInfo elements
// --------------------------------------------------------------------------------

DOMElement * DSIGKeyInfoList::createKeyInfo(void) {

	// Assume that someone else has looked after the DOM
	empty();

	safeBuffer str;
	DOMDocument * doc = mp_env->getParentDocument();

	makeQName(str, mp_env->getDSIGNSPrefix(), "KeyInfo");

	DOMElement * ret = doc->createElementNS(DSIGConstants::s_unicodeStrURIDSIG, str.rawXMLChBuffer());

	mp_keyInfoNode = ret;
	mp_env->doPrettyPrint(mp_keyInfoNode);

	return ret;

}


DSIGKeyInfoValue * DSIGKeyInfoList::appendDSAKeyValue(const XMLCh * P,
						   const XMLCh * Q,
						   const XMLCh * G,
						   const XMLCh * Y) {

	if (mp_keyInfoNode == NULL) {

		throw XSECException(XSECException::KeyInfoError,
			"KeyInfoList - Attempt to create DSAKeyValue before creating KeyInfo");

	}

	// Create the new element
	DSIGKeyInfoValue * v;
	XSECnew(v, DSIGKeyInfoValue(mp_env));

	mp_keyInfoNode->appendChild(v->createBlankDSAKeyValue(P, Q, G, Y));
	mp_env->doPrettyPrint(mp_keyInfoNode);

	// Add to the list
	addKeyInfo(v);

	return v;

}

DSIGKeyInfoValue * DSIGKeyInfoList::appendRSAKeyValue(const XMLCh * modulus,
						   const XMLCh * exponent) {

	if (mp_keyInfoNode == NULL) {

		throw XSECException(XSECException::KeyInfoError,
			"KeyInfoList - Attempt to create RSAKeyValue before creating KeyInfo");

	}

	// Create the new element
	DSIGKeyInfoValue * v;
	XSECnew(v, DSIGKeyInfoValue(mp_env));

	mp_keyInfoNode->appendChild(v->createBlankRSAKeyValue(modulus, exponent));
	mp_env->doPrettyPrint(mp_keyInfoNode);

	// Add to the list
	addKeyInfo(v);

	return v;

}


DSIGKeyInfoX509 * DSIGKeyInfoList::appendX509Data(void) {

	if (mp_keyInfoNode == NULL) {

		throw XSECException(XSECException::KeyInfoError,
			"KeyInfoList - Attempt to create X509Data before creating KeyInfo");

	}

	DSIGKeyInfoX509 * x;

	XSECnew(x, DSIGKeyInfoX509(mp_env));

	mp_keyInfoNode->appendChild(x->createBlankX509Data());
	mp_env->doPrettyPrint(mp_keyInfoNode);

	// Add to the list
	addKeyInfo(x);

	return x;

}

DSIGKeyInfoName * DSIGKeyInfoList::appendKeyName(const XMLCh * name, bool isDName) {

	if (mp_keyInfoNode == NULL) {

		throw XSECException(XSECException::KeyInfoError,
			"KeyInfoList - Attempt to create KeyName before creating KeyInfo");

	}

	DSIGKeyInfoName * n;

	XSECnew(n, DSIGKeyInfoName(mp_env));

	mp_keyInfoNode->appendChild(n->createBlankKeyName(name, isDName));
	mp_env->doPrettyPrint(mp_keyInfoNode);

	// Add to the list
	addKeyInfo(n);

	return n;

}

DSIGKeyInfoPGPData * DSIGKeyInfoList::appendPGPData(const XMLCh * id, const XMLCh * packet) {

	if (mp_keyInfoNode == NULL) {

		throw XSECException(XSECException::KeyInfoError,
			"KeyInfoList - Attempt to create PGPData before creating KeyInfo");

	}

	DSIGKeyInfoPGPData * p;

	XSECnew(p, DSIGKeyInfoPGPData(mp_env));

	mp_keyInfoNode->appendChild(p->createBlankPGPData(id, packet));
	mp_env->doPrettyPrint(mp_keyInfoNode);

	addKeyInfo(p);

	return p;

}

DSIGKeyInfoSPKIData * DSIGKeyInfoList::appendSPKIData(const XMLCh * sexp) {

	if (mp_keyInfoNode == NULL) {

		throw XSECException(XSECException::KeyInfoError,
			"KeyInfoList - Attempt to create SPKIData before creating KeyInfo");

	}

	DSIGKeyInfoSPKIData * s;

	XSECnew(s, DSIGKeyInfoSPKIData(mp_env));

	mp_keyInfoNode->appendChild(s->createBlankSPKIData(sexp));
	mp_env->doPrettyPrint(mp_keyInfoNode);

	addKeyInfo(s);

	return s;

}

DSIGKeyInfoMgmtData * DSIGKeyInfoList::appendMgmtData(const XMLCh * data) {

	if (mp_keyInfoNode == NULL) {

		throw XSECException(XSECException::KeyInfoError,
			"KeyInfoList - Attempt to create MgmtData before creating KeyInfo");

	}

	DSIGKeyInfoMgmtData * m;

	XSECnew(m, DSIGKeyInfoMgmtData(mp_env));

	mp_keyInfoNode->appendChild(m->createBlankMgmtData(data));
	mp_env->doPrettyPrint(mp_keyInfoNode);

	addKeyInfo(m);

	return m;

}

DSIGKeyInfoDEREncoded * DSIGKeyInfoList::appendDEREncoded(const XMLCh * data) {

	if (mp_keyInfoNode == NULL) {

		throw XSECException(XSECException::KeyInfoError,
			"KeyInfoList - Attempt to create DEREncodedKeyValue before creating KeyInfo");

	}

	DSIGKeyInfoDEREncoded * d;

	XSECnew(d, DSIGKeyInfoDEREncoded(mp_env));

	mp_keyInfoNode->appendChild(d->createBlankDEREncoded(data));
	mp_env->doPrettyPrint(mp_keyInfoNode);

	addKeyInfo(d);

	return d;

}

// --------------------------------------------------------------------------------
//           Some helper functions
// --------------------------------------------------------------------------------

void DSIGKeyInfoList::addAndInsertKeyInfo(DSIGKeyInfo * ref) {

	if (mp_keyInfoNode == NULL) {

		throw XSECException(XSECException::KeyInfoError,
			"KeyInfoList - Attempt to create child before creating KeyInfo");

	}

	mp_keyInfoNode->appendChild(ref->getKeyInfoDOMNode());
	mp_env->doPrettyPrint(mp_keyInfoNode);

	addKeyInfo(ref);

}
