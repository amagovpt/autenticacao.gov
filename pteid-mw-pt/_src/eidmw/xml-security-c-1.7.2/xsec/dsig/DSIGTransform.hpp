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
 * DSIGTransform := Base (virtual) class that defines a DSIG Transform
 *
 * Author(s): Berin Lautenbach
 *
 * $Id: DSIGTransform.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef DSIGTRANSFORM_INCLUDE
#define DSIGTRANSFORM_INCLUDE

#include <xsec/dsig/DSIGConstants.hpp>
#include <xsec/framework/XSECDefs.hpp>

XSEC_DECLARE_XERCES_CLASS(DOMNode);
XSEC_DECLARE_XERCES_CLASS(DOMElement);
XSEC_DECLARE_XERCES_CLASS(DOMDocument);

#include <stdio.h>

class XSECEnv;
class TXFMChain;

/**
 * @ingroup pubsig
 */

/**
 * @brief The base class for transforms.
 *
 * The DSIGTransform class is the base class used to hold \<Transform\> elements
 * within a document.
 *
 * It does not in actually perform any transformations - only hold the information
 * about a transform in a \<Signature\> structure.
 *
 * @see TXFMBase
 *
 */


class DSIG_EXPORT DSIGTransform {

public:

	/** @name Constructors and Destructors */
    //@{
	
    /**
	 * \brief Contructor used for existing XML signatures.
	 *
	 * The Node structure already exists, so this type of Transform constructor
	 * will generally read the nodes in.
	 *
	 * @note DSIGTransform structures should only ever be created via calls to a
	 * DSIGTransformList object.
	 *
	 * @param env The environment in which this transform operates
	 * @param node The DOM node (within doc) that is to be used as the base of the Transform.
	 * @see #load
	 */

	DSIGTransform(const XSECEnv * env, XERCES_CPP_NAMESPACE_QUALIFIER DOMNode * node) : 
		  mp_txfmNode(node),
		  mp_env(env) {};

    /**
	 * \brief Contructor used for new signatures.
	 *
	 * The Node structure will have to be created by the implementation class
	 *
	 * @note DSIGTransform structures should only ever be created via calls to a
	 * DSIGTransformList object.
	 *
	 * @param env The environment descriptor for the object
	 *
	 */

	DSIGTransform(const XSECEnv * env) : 
		  mp_txfmNode(NULL),
		  mp_env(env) {};

		  
	/**
	 * \brief Destructor.
	 *
	 * Destroy the DSIGSignature elements.
	 *
	 * Does not destroy any associated DOM Nodes
	 */
		  
	virtual ~DSIGTransform() {};
	
	//@}

	/** @name Interface Methods */

	//@{

	/**
	 * \brief Determine the transform type.
	 *
	 * Used to determine what the type of the transform is.
	 *
	 */

	virtual transformType getTransformType() = 0;
	
	/**
	 * \brief Create the transformer element and append to an existing Chain.
	 *
	 * Implemented by each Transform class and used by the DSIGSignature
	 * to construct a complete Transform chain (TXFMChain).
	 */

	virtual void appendTransformer(TXFMChain * input) = 0;

	/**
	 * \brief Construct a new transform.
	 *
	 * Instruct the implementation to create the required
	 * transform and return the newly constructed DOMNode structure
	 */

	virtual XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * 
		createBlankTransform(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument * parentDoc) = 0;

	/**
	 * \brief Load a DOM structure
	 *
	 * Take the original node and load any sub nodes in the transform
	 * (if necessary)
	 */

	virtual void load(void) = 0;

	//@}

protected:

	/** @name Utility Functions */

	//@{

	/**
	 * \brief Create the basic node structure of a transform
	 *
	 */

	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * createTransformNode();


	XERCES_CPP_NAMESPACE_QUALIFIER DOMNode					
							* mp_txfmNode;			// The node that we read from
	const XSECEnv			* mp_env;	// Owning signature

private:

	DSIGTransform();
	DSIGTransform(const DSIGTransform &theOther);

};



#endif /* #define DSIGTRANSFORM_INCLUDE */
