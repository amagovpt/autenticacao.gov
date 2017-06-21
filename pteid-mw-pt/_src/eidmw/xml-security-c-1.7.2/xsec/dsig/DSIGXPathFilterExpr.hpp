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
 * $Id: DSIGXPathFilterExpr.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef DSIGXPATHFILTEREXPR_INCLUDE
#define DSIGXPATHFILTEREXPR_INCLUDE

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/dsig/DSIGConstants.hpp>
#include <xsec/utils/XSECSafeBuffer.hpp>


XSEC_DECLARE_XERCES_CLASS(DOMNamedNodeMap);
XSEC_DECLARE_XERCES_CLASS(DOMNode);
XSEC_DECLARE_XERCES_CLASS(DOMElement);

class XSECEnv;

/**
 * @ingroup pubsig
 */

/**
 * @brief Class used to hold (and manipulate) individual expressions
 *        in an XPathFilter transform
 *
 * @see TXFMXPathFilter
 * @see DSIGTransformXpathFilter
 *
 */

class DSIG_EXPORT DSIGXPathFilterExpr {

public:

	/** @name Constructors and Destructors */
	//@{

	/**
	 * \brief Constructor used for existing XML signatures
	 *
	 * Node already exists and is part of an existing XPathFilter tree
	 *
	 * @param env The operating environment
	 * @param node The node that will be used to read the expression in
	 */

	DSIGXPathFilterExpr(const XSECEnv * env, XERCES_CPP_NAMESPACE_QUALIFIER DOMNode * node);

	/**
	 * \brief Builder constructor
	 *
	 * Used to create the DOM structure and DSIGSignature elements
	 *
	 * @param env The operating Environment
	 */

	DSIGXPathFilterExpr(const XSECEnv * env);

	/**
	 * \brief Destructor.
	 *
	 * Destroy the DSIGSignature elements.
	 *
	 * Does not destroy any associated DOM Nodes
	 */
		  
	~DSIGXPathFilterExpr();
	
	//@}

	/** @name Set and get Information */

	//@{

	/**
	 * \brief Read in existing structure
	 *
	 * Reads DOM structure of the XPath expression
	 */

	void load(void);

	/**
	 * \brief Get the filter type
	 *
	 * Returns the type of this particular XPath filter
	 *
	 * @returns The filter type of this expression
	 */

	xpathFilterType getFilterType(void);

	/**
	 * \brief create from blank
	 *
	 * Given the filter type and XPath expression, setup the
	 * DOMNodes and variables to allow signing and validation
	 *
	 * @param filterType Type of this filter to add
	 * @param filterExpr The XPath expression
	 */

	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * setFilter(xpathFilterType filterType,
						const XMLCh * filterExpr);

	/**
	 * \brief Get the filter expression
	 *
	 * Returns an XMLCh string containing the filter expression
	 *
	 * @returns The filter expression
	 */

	const XMLCh * getFilter(void) {return m_expr.rawXMLChBuffer();}

	/**
	 * \brief Add a new namespace to the list to be used
	 *
	 * Add a new namespace to the XPath Element.
	 *
	 * @param prefix NCName of the Namespace to set
	 * @param value The string with the URI to set
	 */

	void setNamespace(const XMLCh * prefix, const XMLCh * value);

	/**
	 * \brief Get the list of namespaces.
	 *
	 * Returns the DOMNamedNodeMap of the attributes of the XPath transform
	 * node.  
	 *
	 * @note This will also contain the Filter attribute
	 *
	 * @returns A pointer to the NamedNodeMap
	 */

	XERCES_CPP_NAMESPACE_QUALIFIER DOMNamedNodeMap * getNamespaces(void) {
		return mp_NSMap;
	}

	/**
	 * \brief Delete a namespace to the list to be used
	 *
	 * Delete a namespace from the XPath Element.
	 *
	 * @param prefix NCName of the Namespace to delete
	 * @throws XSECException if the NCName does not exist
	 *
	 */

	void deleteNamespace(const XMLCh * prefix);

	//@}
	
private:

	// Just let the TXFM read directly

	friend class TXFMXPathFilter;

	DSIGXPathFilterExpr();
	DSIGXPathFilterExpr(const DSIGXPathFilterExpr& theOther);

	const XSECEnv				* mp_env;
	XERCES_CPP_NAMESPACE_QUALIFIER DOMNode						
								* mp_xpathFilterNode;
	XERCES_CPP_NAMESPACE_QUALIFIER DOMNode						
								* mp_exprTextNode;
	XERCES_CPP_NAMESPACE_QUALIFIER DOMNamedNodeMap				
								* mp_NSMap;
	safeBuffer					m_expr;
	xpathFilterType				m_filterType;
	bool						m_loaded;


};

#endif /* DSIGXPATHFILTEREXPR_INCLUDE */
