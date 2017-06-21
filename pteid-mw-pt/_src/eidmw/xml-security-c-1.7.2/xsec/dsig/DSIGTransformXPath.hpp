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
 * DSIGTransformXPath := Class that performs XPath transforms
 *
 * $Id: DSIGTransformXPath.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef DSIGTRANSFORMXPATH_INCLUDE
#define DSIGTRANSFORMXPATH_INCLUDE

#include <xsec/dsig/DSIGTransform.hpp>
#include <xsec/utils/XSECSafeBuffer.hpp>


XSEC_DECLARE_XERCES_CLASS(DOMNamedNodeMap);
XSEC_DECLARE_XERCES_CLASS(DOMNode);

/**
 * @ingroup pubsig
 */

/**
 * @brief Transform holder for XPath transforms.
 *
 * The DSIGTransformXPath class is used to hold XPath \<Transform\> elements
 * within a document.
 *
 * @see TXFMXPath
 * @see DSIGTransform
 *
 */

class DSIG_EXPORT DSIGTransformXPath : public DSIGTransform {

public:

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

	DSIGTransformXPath(const XSECEnv * env, XERCES_CPP_NAMESPACE_QUALIFIER DOMNode * node);

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

	DSIGTransformXPath(const XSECEnv * env);
		  
	/**
	 * \brief Destructor.
	 *
	 * Destroy the DSIGSignature elements.
	 *
	 * Does not destroy any associated DOM Nodes
	 */
		  
	virtual ~DSIGTransformXPath();
	
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
	 * \brief Create the XPath Transformer class.
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

	virtual XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * createBlankTransform(
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument * parentDoc);

	/**
	 * \brief Load a DOM structure
	 *
	 * Load the expression and Namespaces.
	 *
	 */

	virtual void load(void);

	//@}

	/** @name XPath specific methods */

	//@{

	/** 
	 *
	 * \brief Set the XPath expression.
	 *
	 * Takes the provided string and uses it to set the expression in the
	 * Signature and DOM structures.
	 *
	 * If an expression already exists, it is overwritten.
	 *
	 * @param expr The expression to set
	 */

	void setExpression(const char * expr);

	/**
	 * \brief Get the XPath expression
	 *
	 * Returns a character buffer with the expression inside it.
	 *
	 * @note Do not delete the returned pointer - it is owned by the object.
	 *
	 * @returns The expression
	 */

	const char * getExpression(void);

	/**
	 * \brief Add a new namespace to the list to be used
	 *
	 * Add a new namespace to the XPath Element.
	 *
	 * @param prefix NCName of the Namespace to set
	 * @param value The string with the URI to set
	 */

	void setNamespace(const char * prefix, const char * value);

	/**
	 * \brief Get the list of namespaces.
	 *
	 * Returns the DOMNamedNodeMap of the attributes of the XPath transform
	 * node.  This <em>should</em> only contain namespaces.
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

	void deleteNamespace(const char * prefix);

	//@}
	
private:

	DSIGTransformXPath();
	DSIGTransformXPath(const DSIGTransformXPath & theOther);

	safeBuffer					m_expr;
	XERCES_CPP_NAMESPACE_QUALIFIER DOMNode						
								* mp_exprTextNode;
	XERCES_CPP_NAMESPACE_QUALIFIER DOMNode						
								* mp_xpathNode;
	XERCES_CPP_NAMESPACE_QUALIFIER DOMNamedNodeMap		
								* mp_NSMap;


};

#endif /* DSIGTRANSFORMXPATH_INCLUDE */
