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
 * DSIGTransformC14n := Class that holds information about C14n transforms
 *
 * $Id: DSIGTransformC14n.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#include <xsec/dsig/DSIGTransform.hpp>

/**
 * @ingroup pubsig
 */

/**
 * @brief Transform holder for C14n based transforms.
 *
 * The DSIGTransformC14n class is used to hold C14n \<Transform\> elements
 * within a document.  This includes Exclusive and Comment options
 *
 *
 * @see TXFMC14n
 * @see DSIGTransform
 *
 */

class DSIG_EXPORT DSIGTransformC14n : public DSIGTransform {

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

	DSIGTransformC14n(const XSECEnv * env, XERCES_CPP_NAMESPACE_QUALIFIER DOMNode * node);

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

	DSIGTransformC14n(const XSECEnv * env);
		  
	/**
	 * \brief Destructor.
	 *
	 * Destroy the DSIGSignature elements.
	 *
	 * Does not destroy any associated DOM Nodes
	 */
		  
	virtual ~DSIGTransformC14n();
	
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
	 * \brief Create the Canonicalising transformer element.
	 *
	 * Implemented by each Transform class and used by the DSIGSignature
	 * when consructing a TXFM List that includes canonicalisation (nearly always)
	 */

	virtual void appendTransformer(TXFMChain * input);

	/**
	 * \brief Construct blank Canonicalisation Transform element.
	 *
	 * Instruct the implementation to create the required
	 * transform and return the newly constructed DOMNode structure
	 *
	 * @note By default creates a non-comment non-exclusive C14n transform.
	 */

	virtual XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * createBlankTransform(
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument * parentDoc);

	/**
	 * \brief Load a DOM structure
	 *
	 * (Re)read the DOM structure into the Signature structures
	 *
	 */

	virtual void load(void);

	//@}

	/** @name C14n Specific Methods */

	//@{
	
	/**
	 * \brief Change canonicalisation method.
	 *
	 * Set the canonicalisation method to the type indicated.  If this changes
	 * the transform from Exclusive to Standard C14n, any associated
	 * InclusiveNamespaces children will be removed.
	 *
	 * If this is moving from one form of Exclusive to another, any InclusiveNamespace
	 * children will remain.
	 *
	 * @param method Type of canicaliser required.
	 *
	 * @see canonicalizationMethod
	 */

	void setCanonicalizationMethod(canonicalizationMethod method);

	/**
	 * \brief Get canonicalisation type.
	 *
	 * Return the type of canonicalisation to the caller.
	 *
	 * @returns Canonicalisation type.
	 * @see canonicalizationMethod
	 */

	canonicalizationMethod getCanonicalizationMethod(void);

	/**
	 * \brief Add a namespace prefix to the InclusiveNamespaces list
	 *
	 * Exclusive canonicalisation includes the ability to define a PrefixList of
	 * namespace prefixes that will not be treated exclusively, rather they will
	 * be handled as per normal C14n.
	 *
	 * This function allows a caller to add a prefix to this list
	 * @param ns The prefix to add.
	 */

	void addInclusiveNamespace(const char * ns);

	/**
	 * \brief Set the namespace list
	 *
	 * Deletes current PrefixList (if any) and sets the list to the space
	 * separated list of namespace prefixes provided in ns.
	 *
	 * @note No checking is done on the string passed in.
	 *
	 * @param ns The (space separated) list of prefixes to set.
	 */

	void setInclusiveNamespaces(const XMLCh * ns);
	
	/**
	 * \brief Get the string containing the inclusive namespaces.
	 *
	 * Get the string containing a list of (space separated) prefixes that will
	 * be handled non-exclusively in an exclusive C14n transform.
	 *
	 * @returns A pointer to the string held in the node (NULL if no namespaces defined).
	 * @note The pointer returned is owned by the Transform structure - do not delete.
	 */

	 const XMLCh * getPrefixList(void);

	 /**
	  * \brief Delete all inclusive namespaces.
	  *
	  * Deletes the structures (including the DOM nodes) that hold the inclusive
	  * namespaces list.
	  */

	 void clearInclusiveNamespaces(void);

	//@}

	
private:

	DSIGTransformC14n();
	DSIGTransformC14n(const DSIGTransformC14n & theOther);

	void createInclusiveNamespaceNode(void);


	canonicalizationMethod			m_cMethod;			// The method
	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement						
									* mp_inclNSNode;	// Node holding the inclusive Namespaces
	const XMLCh						* mp_inclNSStr;		// String holding the namespaces							
};
