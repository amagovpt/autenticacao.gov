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
 * DSIGObject := Defines the container class used by dsig to hold objects
 *				 inside a signture
 *
 * $Id: DSIGObject.cpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

// XSEC Includes

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/framework/XSECEnv.hpp>
#include <xsec/framework/XSECError.hpp>
#include <xsec/dsig/DSIGConstants.hpp>
#include <xsec/dsig/DSIGObject.hpp>
#include <xsec/utils/XSECDOMUtils.hpp>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/XMLUniDefs.hpp>

XERCES_CPP_NAMESPACE_USE

// --------------------------------------------------------------------------------
//			String Constants
// --------------------------------------------------------------------------------

static XMLCh s_Object[] = {

	chLatin_O,
	chLatin_b,
	chLatin_j,
	chLatin_e,
	chLatin_c,
	chLatin_t,
	chNull
};

static XMLCh s_Id[] = {

	chLatin_I,
	chLatin_d,
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
//           Constructors/Destructor
// --------------------------------------------------------------------------------

DSIGObject::DSIGObject(const XSECEnv * env, XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *dom) {

	mp_env = env;
	mp_objectNode = dom;
	mp_idAttr = NULL;
	mp_mimeTypeAttr = NULL;
	mp_encodingAttr = NULL;

}

DSIGObject::DSIGObject(const XSECEnv * env) {

	mp_env = env;
	mp_objectNode = NULL;
	mp_idAttr = NULL;
	mp_mimeTypeAttr = NULL;
	mp_encodingAttr = NULL;

}



DSIGObject::~DSIGObject() {}

// --------------------------------------------------------------------------------
//           Library only
// --------------------------------------------------------------------------------


void DSIGObject::load(void) {

	if (mp_objectNode == NULL || 
		mp_objectNode->getNodeType() != DOMNode::ELEMENT_NODE || 
		!strEquals(getDSIGLocalName(mp_objectNode), s_Object)) {

		throw XSECException(XSECException::ObjectError,
			"Expected <Object> Node in DSIGObject::load");

	}


	mp_idAttr = ((DOMElement *) mp_objectNode)->getAttributeNodeNS(NULL, s_Id);
	if (mp_idAttr) {
#if defined (XSEC_XERCES_HAS_SETIDATTRIBUTE)
	    ((DOMElement *) mp_objectNode)->setIdAttributeNS(NULL, s_Id);
#elif defined (XSEC_XERCES_HAS_BOOLSETIDATTRIBUTE)
		((DOMElement *) mp_objectNode)->setIdAttributeNS(NULL, s_Id, true);
#endif
	}

	mp_mimeTypeAttr = ((DOMElement *) mp_objectNode)->getAttributeNodeNS(NULL, s_MimeType);
	mp_encodingAttr = ((DOMElement *) mp_objectNode)->getAttributeNodeNS(NULL, s_Encoding);

}


DOMElement * DSIGObject::createBlankObject(void) {

	safeBuffer str;
	const XMLCh * prefix;
	DOMDocument *doc = mp_env->getParentDocument();

	prefix = mp_env->getDSIGNSPrefix();
	
	// Create the transform node
	makeQName(str, prefix, s_Object);
	mp_objectNode = doc->createElementNS(DSIGConstants::s_unicodeStrURIDSIG, str.rawXMLChBuffer());

	mp_idAttr = NULL;
	mp_mimeTypeAttr = NULL;
	mp_encodingAttr = NULL;

	return (DOMElement *) mp_objectNode;

}

// --------------------------------------------------------------------------------
//           Get functions
// --------------------------------------------------------------------------------


const XMLCh * DSIGObject::getId(void) {

	if (mp_idAttr != NULL)
		return mp_idAttr->getNodeValue();

	return NULL;

}

const XMLCh * DSIGObject::getMimeType(void) {

	if (mp_mimeTypeAttr != NULL)
		return mp_mimeTypeAttr->getNodeValue();

	return NULL;

}


const XMLCh * DSIGObject::getEncoding(void) {

	if (mp_encodingAttr != NULL)
		return mp_encodingAttr->getNodeValue();

	return NULL;

}

const DOMElement * DSIGObject::getElement(void) {

	return (DOMElement *) mp_objectNode;

}

// --------------------------------------------------------------------------------
//           Set Functions
// --------------------------------------------------------------------------------


void DSIGObject::setId(const XMLCh * id) {

	if (mp_idAttr != NULL) {

		mp_idAttr->setNodeValue(id);

	}

	else {

		((DOMElement *) mp_objectNode)->setAttributeNS(NULL, s_Id, id);
		// Mark as an ID
#if defined (XSEC_XERCES_HAS_SETIDATTRIBUTE)
		((DOMElement *) mp_objectNode)->setIdAttributeNS(NULL, s_Id);
#elif defined (XSEC_XERCES_HAS_BOOLSETIDATTRIBUTE)
		((DOMElement *) mp_objectNode)->setIdAttributeNS(NULL, s_Id, true);
#endif
		mp_idAttr = ((DOMElement *) mp_objectNode)->getAttributeNodeNS(NULL, s_Id);

	}

}


void DSIGObject::setMimeType(const XMLCh * type) {

	if (mp_mimeTypeAttr != NULL) {

		mp_mimeTypeAttr->setNodeValue(type);

	}

	else {

		((DOMElement *) mp_objectNode)->setAttributeNS(NULL, s_MimeType, type);
		mp_mimeTypeAttr = ((DOMElement *) mp_objectNode)->getAttributeNodeNS(NULL, s_MimeType);

	}

}

void DSIGObject::setEncoding(const XMLCh * encoding) {

	if (mp_encodingAttr != NULL) {

		mp_encodingAttr->setNodeValue(encoding);

	}

	else {

		((DOMElement *) mp_objectNode)->setAttributeNS(NULL, s_Encoding, encoding);
		mp_encodingAttr = ((DOMElement *) mp_objectNode)->getAttributeNodeNS(NULL, s_Encoding);

	}

}


void DSIGObject::appendChild(DOMNode * child) {

	if (mp_objectNode == NULL) {

		throw XSECException(XSECException::ObjectError,
			"DSIGObject::appendChild - Object node has not been created");

	}

	mp_objectNode->appendChild(child);

}

