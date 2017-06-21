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
 * DSIGXPathFilterExpr := Class that holds an XPath Filter expression
 *
 * Author(s): Berin Lautenbach
 *
 * $Id: DSIGXPathFilterExpr.cpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/dsig/DSIGXPathFilterExpr.hpp>
#include <xsec/dsig/DSIGSignature.hpp>
#include <xsec/framework/XSECException.hpp>
#include <xsec/framework/XSECEnv.hpp>


#include <xercesc/dom/DOMNode.hpp>
#include <xercesc/dom/DOMNamedNodeMap.hpp>
#include <xercesc/util/XMLUniDefs.hpp>


XERCES_CPP_NAMESPACE_USE

XMLCh filterStr[] = {

	chLatin_F,
	chLatin_i,
	chLatin_l,
	chLatin_t,
	chLatin_e,
	chLatin_r,
	chNull

};


// --------------------------------------------------------------------------------
//           Constructors and Destructors
// --------------------------------------------------------------------------------

DSIGXPathFilterExpr::DSIGXPathFilterExpr(const XSECEnv * env, DOMNode * node) :
mp_env(env),
mp_xpathFilterNode(node),
mp_NSMap(NULL),
m_loaded(false) {

}

DSIGXPathFilterExpr::DSIGXPathFilterExpr(const XSECEnv * env) :
mp_env(env),
mp_xpathFilterNode(NULL),
mp_NSMap(NULL),
m_loaded(false)  {

}


DSIGXPathFilterExpr::~DSIGXPathFilterExpr() {

	// Nothing to do at the moment

}

// --------------------------------------------------------------------------------
//           Load existing DOM structure
// --------------------------------------------------------------------------------
	
void DSIGXPathFilterExpr::load(void) {

	// Find the XPath expression

	if (mp_xpathFilterNode == NULL ||
		!strEquals(getXPFLocalName(mp_xpathFilterNode), "XPath")) {

		throw XSECException(XSECException::ExpectedDSIGChildNotFound,
			"Expected <XPath> as first node in DSIGXPathFilterExpr::load");

	}

		
	// Check for attributes - in particular any namespaces

	mp_NSMap = mp_xpathFilterNode->getAttributes();

	// Find the filter type
	DOMNode * a;
	if (mp_NSMap == NULL ||
		((a = mp_NSMap->getNamedItem(filterStr)) == NULL)) {

		throw XSECException(XSECException::ExpectedDSIGChildNotFound,
			"Expected Filter attribute of <XPath> node in in DSIGXPathFilterExpr::load");

	}

	const XMLCh * f = a->getNodeValue();
	if (strEquals(f, "intersect")) {
		m_filterType = FILTER_INTERSECT;
	}
	else if (strEquals(f, "union")) {
		m_filterType = FILTER_UNION;
	}
	else if (strEquals(f, "subtract")) {
		m_filterType = FILTER_SUBTRACT;
	}
	else {

		throw XSECException(XSECException::ExpectedDSIGChildNotFound,
				"DSIGXPathFilterExpr::load Expected on of intersect, union or subtract as filter type");
	}

	// Find the text node
	mp_exprTextNode = findFirstChildOfType(mp_xpathFilterNode, DOMNode::TEXT_NODE);

	if (mp_exprTextNode == NULL) {
		throw XSECException(XSECException::ExpectedDSIGChildNotFound,
			"Expected Text Node in beneath <XPath> in DSIGXPathFilterExpr::load");
	}

	// Gather the text - hold it in UTF16 format
	gatherChildrenText(mp_xpathFilterNode, m_expr);

	m_loaded = true;

}

// --------------------------------------------------------------------------------
//           Create a new filter
// --------------------------------------------------------------------------------

DOMElement * DSIGXPathFilterExpr::setFilter(xpathFilterType filterType,
						const XMLCh * filterExpr) {

	if (m_loaded == true) {

		throw XSECException(XSECException::XPathFilterError,
			"DSIGXPathFilterExpr::setFilter - called when already loaded");

	}

	safeBuffer str;
	const XMLCh * prefix;
	DOMDocument *doc = mp_env->getParentDocument();
	DOMElement * xe;

	// Create the XPath element
	prefix = mp_env->getXPFNSPrefix();
	makeQName(str, prefix, "XPath");
	xe = doc->createElementNS(DSIGConstants::s_unicodeStrURIXPF, str.rawXMLChBuffer());
	mp_xpathFilterNode = xe;

	// Put in correct namespace
	prefix = mp_env->getXPFNSPrefix();

	// Set the namespace attribute
	if (prefix[0] == '\0') {
		str.sbTranscodeIn("xmlns");
	}
	else {
		str.sbTranscodeIn("xmlns:");
		str.sbXMLChCat(prefix);
	}

	xe->setAttributeNS(DSIGConstants::s_unicodeStrURIXMLNS, 
							str.rawXMLChBuffer(), 
							DSIGConstants::s_unicodeStrURIXPF);

	// Set the filter type
	m_filterType = filterType;

	switch (filterType) {

	case FILTER_INTERSECT :

		xe->setAttributeNS(NULL,MAKE_UNICODE_STRING("Filter"), MAKE_UNICODE_STRING("intersect"));
		break;

	case FILTER_SUBTRACT :

		xe->setAttributeNS(NULL,MAKE_UNICODE_STRING("Filter"), MAKE_UNICODE_STRING("subtract"));
		break;

	case FILTER_UNION :

		xe->setAttributeNS(NULL,MAKE_UNICODE_STRING("Filter"), MAKE_UNICODE_STRING("union"));
		break;

	default :

		mp_xpathFilterNode->release();
		throw XSECException(XSECException::XPathFilterError,
			"DSIGXPathFilterExpr::appendFilter - Unexpected Filter Type");

	}

	// Now add the actual filter

	mp_exprTextNode = doc->createTextNode(filterExpr);
	mp_xpathFilterNode->appendChild(mp_exprTextNode);

	mp_NSMap = mp_xpathFilterNode->getAttributes();

	m_expr.sbXMLChIn(filterExpr);
	m_loaded = true;
	return xe;

}


// --------------------------------------------------------------------------------
//           Find the type
// --------------------------------------------------------------------------------

xpathFilterType DSIGXPathFilterExpr::getFilterType(void) {

	if (m_loaded == false) {
		throw XSECException(XSECException::LoadEmptyXPathFilter,
			"DSIGXPathFilterExpr::Element node loaded");
	}

	return m_filterType;

}

// --------------------------------------------------------------------------------
//           Set and clear namespaces
// --------------------------------------------------------------------------------

void DSIGXPathFilterExpr::setNamespace(const XMLCh * prefix, const XMLCh * value) {

	if (mp_xpathFilterNode == NULL) {

		throw XSECException(XSECException::XPathFilterError,
			"DSIGXPathFilterExpr::setNamespace - load not called");

	}
	
	safeBuffer str;

	str.sbTranscodeIn("xmlns:");
	str.sbXMLChCat(prefix);

	DOMElement *x;

	x = static_cast <DOMElement *> (mp_xpathFilterNode);

	x->setAttributeNS(DSIGConstants::s_unicodeStrURIXMLNS,
		str.rawXMLChBuffer(),
		value);

	mp_NSMap = mp_xpathFilterNode->getAttributes();


}

void DSIGXPathFilterExpr::deleteNamespace(const XMLCh * prefix) {

	if (mp_xpathFilterNode == NULL) {

		throw XSECException(XSECException::XPathFilterError,
			"DSIGXPathFilterExpr::deleteNamespace - load not called");

	}

	DOMElement *x;

	x = static_cast <DOMElement *> (mp_xpathFilterNode);

	x->removeAttributeNS(DSIGConstants::s_unicodeStrURIXMLNS,
		prefix);

}



	/**
	 * \brief Add a new namespace to the list to be used
	 *
	 * Add a new namespace to the XPath Element.
	 *
	 * @param prefix NCName of the Namespace to set
	 * @param value The string with the URI to set
	 */

//	void setNamespace(const char * prefix, const char * value);

	/**
	 * \brief Get the list of namespaces.
	 *
	 * Returns the DOMNamedNodeMap of the attributes of the XPath transform
	 * node.  This <em>should</em> only contain namespaces.
	 *
	 * @returns A pointer to the NamedNodeMap
	 */

//	DOMNamedNodeMap * getNamespaces(void) {
//		return mp_NSMap;
//	}

	/**
	 * \brief Delete a namespace to the list to be used
	 *
	 * Delete a namespace from the XPath Element.
	 *
	 * @param prefix NCName of the Namespace to delete
	 * @throws XSECException if the NCName does not exist
	 *
	 */

//	void deleteNamespace(const char * prefix);

	//@}
	
