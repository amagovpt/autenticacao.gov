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
 * DSIGTransformXSL := Class that performs XML Stylesheet Language transforms
 *
 * $Id: DSIGTransformXSL.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#include <xsec/dsig/DSIGTransform.hpp>

// Xerces

#include <xercesc/dom/DOM.hpp>

/**
 * @ingroup pubsig
 */

/**
 * @brief Transform holder for XSLT Transforms.
 *
 * The DSIGTransformXSL class is used to hold XSLT \<Transform\> elements
 * within a document.
 *
 *
 * @see TXFMXSL
 * @see DSIGTransform
 *
 */



class DSIG_EXPORT DSIGTransformXSL : public DSIGTransform {

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

	DSIGTransformXSL(const XSECEnv * env, XERCES_CPP_NAMESPACE_QUALIFIER DOMNode * node);

    /**
	 * \brief Contructor used for new signatures.
	 *
	 * The Node structure will have to be created.
	 *
	 * @param env The operating environment
	 * @see createBlankTransform
	 */

	DSIGTransformXSL(const XSECEnv * env);
		  
	/**
	 * \brief Destructor.
	 *
	 * Destroy the DSIGSignature elements.
	 *
	 * Does not destroy any associated DOM Nodes
	 */
		  
	virtual ~DSIGTransformXSL();
	
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
	 * \brief Create the XSLT transformer element.
	 *
	 */

	virtual void appendTransformer(TXFMChain * input);

	/**
	 * \brief Construct blank XSLT Transform element.
	 *
	 * Instruct the implementation to create the required
	 * transform and return the newly constructed DOMNode structure
	 */

	virtual XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * createBlankTransform(
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument * parentDoc);

	/**
	 * \brief Load a DOM structure
	 *
	 * Find the start of the XSLT transform and load.
	 *
	 */

	virtual void load(void);

	//@}

	/** @name XSLT Specific Methods */

	//@{

	/**
	 * \brief Set the DOM tree nodes beneath for the actual Transform
	 *
	 * The caller needs to have already created the DOM tree with the 
	 * XSLT embedded.
	 *
	 * @returns The old transform Element node if it existed.
	 * @param stylesheet The new stylesheet to insert into the document
	 * @note Does not delete the old stylesheet if one existed.  This is returned
	 * to the caller who is expected to delete it.
	 */

	XERCES_CPP_NAMESPACE_QUALIFIER DOMNode * setStylesheet(
		XERCES_CPP_NAMESPACE_QUALIFIER DOMNode * stylesheet
	);

	/**
	 * \brief Get the stylesheet node.
	 *
	 * Obtain the DOMNode at the top of the stylesheet that will be used in this
	 * transform.
	 *
	 * @returns The top stylesheet node
	 */

	XERCES_CPP_NAMESPACE_QUALIFIER DOMNode * getStylesheet(void);

	//@}
	
private:

	DSIGTransformXSL();
	DSIGTransformXSL(const DSIGTransformXSL & theOther);

	XERCES_CPP_NAMESPACE_QUALIFIER DOMNode * mp_stylesheetNode;

};
