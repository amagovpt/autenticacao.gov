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
 * DSIGTransformEnvelope := Class that holds an Envelope transform
 *
 * $Id: DSIGTransformEnvelope.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#include <xsec/dsig/DSIGTransform.hpp>
 
/**
 * @ingroup pubsig
 */

/**
 * @brief Transform holder for Base 64.
 *
 * The DSIGTransformEnvelope class is used to hold base64 \<Transform\> elements
 * within a document.
 *
 * This is a very simple transform with no real options.
 *
 * @see TXFMBase
 * @see DSIGTransform
 *
 */



class DSIG_EXPORT DSIGTransformEnvelope : public DSIGTransform {

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

	DSIGTransformEnvelope(const XSECEnv * env, XERCES_CPP_NAMESPACE_QUALIFIER DOMNode * node);

    /**
	 * \brief Contructor used for new signatures.
	 *
	 * The Node structure will have to be created.
	 *
	 * @param env The operating environment
	 * @see createBlankTransform
	 */

	DSIGTransformEnvelope(const XSECEnv * env);
		  
	/**
	 * \brief Destructor.
	 *
	 * Destroy the DSIGSignature elements.
	 *
	 * Does not destroy any associated DOM Nodes
	 */
		  
	virtual ~DSIGTransformEnvelope();
	
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
	 * \brief Create the Enevelope transformer element.
	 *
	 * Implemented by each Transform class and used by the DSIGSignature
	 * to construct a complete Transform list.
	 */

	virtual void appendTransformer(TXFMChain * input);

	/**
	 * \brief Construct blank Envelope Transform element.
	 *
	 * Instruct the implementation to create the required
	 * transform and return the newly constructed DOMNode structure
	 */

	virtual XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * 
		createBlankTransform(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument * parentDoc);

	/**
	 * \brief Load a DOM structure
	 *
	 * For a Envelope transform this does almost nothing.
	 *
	 */

	virtual void load(void);

	//@}

	
private:

	DSIGTransformEnvelope();
	DSIGTransformEnvelope(const DSIGTransformEnvelope & theOther);

};
