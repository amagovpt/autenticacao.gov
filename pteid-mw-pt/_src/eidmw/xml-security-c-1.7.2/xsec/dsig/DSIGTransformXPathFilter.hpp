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
 * DSIGTransformXPathFilter := Class that performs XPath Filter 
 *                             transforms
 *
 * $Id: DSIGTransformXPathFilter.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef DSIGTRANSFORMXPATHFILTER_INCLUDE
#define DSIGTRANSFORMXPATHFILTER_INCLUDE

#include <xsec/dsig/DSIGTransform.hpp>
#include <xsec/utils/XSECSafeBuffer.hpp>


XSEC_DECLARE_XERCES_CLASS(DOMNamedNodeMap);
XSEC_DECLARE_XERCES_CLASS(DOMNode);

#include <vector>

class DSIGXPathFilterExpr;

/**
 * @ingroup pubsig
 */

/**
 * @brief Transform holder for XPath Filter transforms.
 *
 * The DSIGTransformXPath class is used to hold XPath \<Transform\> elements
 * within a document.
 *
 * @see TXFMXPath
 * @see DSIGTransform
 *
 */

class DSIG_EXPORT DSIGTransformXPathFilter : public DSIGTransform {


public:

	/**
	 * \brief Vector used to pass the actual expressions into the transformer
	 */

	typedef std::vector<DSIGXPathFilterExpr *> exprVectorType;

	/** @name Constructors and Destructors */
    //@{
	
    /**
	 * \brief Contructor used for existing XML signatures.
	 *
	 * The Node structure already exists, so read the nodes in.
	 *
	 * @param env The operating environment
	 * @param node The DOM node (within doc) that is to be used as the base of the Transform.
	 * @see #load
	 */

	DSIGTransformXPathFilter(const XSECEnv * env, XERCES_CPP_NAMESPACE_QUALIFIER DOMNode * node);

    /**
	 * \brief Contructor used for new signatures.
	 *
	 * The Node structure will have to be created.
	 *
	 * @note DSIGTransform structures should only ever be created via calls to a
	 * DSIGTransformList object.
	 *
	 * @param env The operating environment
	 * @see createBlankTransform
	 */

	DSIGTransformXPathFilter(const XSECEnv * env);
		  
	/**
	 * \brief Destructor.
	 *
	 * Destroy the DSIGSignature elements.
	 *
	 * Does not destroy any associated DOM Nodes
	 */
		  
	virtual ~DSIGTransformXPathFilter();
	
	//@}

	/** @name Interface Methods */

	//@{

	/**
	 * \brief Determine the transform type.
	 *
	 * Used to determine what the type of the transform is.
	 *
	 */

	virtual transformType getTransformType();

	/**
	 * \brief Create the XPath Filter Transformer class.
	 *
	 * Create the transformer associated with this XPath transform.
	 * Will set the expression and Namespaces as appropriate
	 *
	 * @returns The TXFMXPath transformer associated with this Transform
	 */

	virtual void appendTransformer(TXFMChain * input);

	/**
	 * \brief Construct blank XPath Transform element.
	 *
	 * Instruct the implementation to create the required
	 * transform and return the newly constructed DOMNode structure
	 */

	virtual XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * 
		createBlankTransform(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument * parentDoc);

	/**
	 * \brief Load a DOM structure
	 *
	 * Load the expression and Namespaces.
	 *
	 */

	virtual void load(void);

	//@}

	/** @name XPathFilter specific methods */

	//@{

	/**
	 * \brief Append a new filter to the transform
	 *
	 * Appends the indicated filter (of given type) to the list of 
	 * filters in the transform.
	 *
	 * @param filterType The type of this filter
	 * @param filterExpr The XPath expression to use
	 * @returns The filter expression
	 */

	DSIGXPathFilterExpr * appendFilter(xpathFilterType filterType,
										const XMLCh * filterExpr);

	/**
	 * \brief Retrieve number of expressions
	 *
	 * Allows callers to determine how many expressions there are in
	 * this filter
	 *
	 * @returns The number of expressions
	 */

	unsigned int getExprNum(void);

	/**
	 * \brief Get expression.
	 *
	 * Allows the caller to retrieve the specified expression from the
	 * XPath Filter object
	 *
	 * @param n The index of the expression to retrieve
	 * @returns The indicated expression
	 */

	DSIGXPathFilterExpr * expr(unsigned int n);

	//@}
	
private:

	DSIGTransformXPathFilter();
	DSIGTransformXPathFilter(const DSIGTransformXPathFilter & theOther);

	bool						m_loaded;

	// The expressions
	exprVectorType				m_exprs;


};

#endif /* DSIGTRANSFORMXPATHFILTER_INCLUDE */
