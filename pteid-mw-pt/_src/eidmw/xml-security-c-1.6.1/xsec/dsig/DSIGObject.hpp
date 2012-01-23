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
 * $Id: DSIGObject.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef DSIGOBJECT_INCLUDE
#define DSIGOBJECT_INCLUDE

// XSEC Includes

#include <xsec/framework/XSECDefs.hpp>

XSEC_DECLARE_XERCES_CLASS(DOMNode);

class XSECEnv;

/**
 * @ingroup pubsig
 */

/**
 * @brief Base class for \<Object\> nodes in a \<Signature\> element.
 *
 * The DSIG spec allows for enveloping signatures, in which the signature holds
 * the information it is signing.  For these types of signatures, the data being
 * signed can be held in an \<Object\> container.
 *
 * This class allows callers to and manipulate Object containers.
 *
 */


class DSIG_EXPORT DSIGObject {

public:

	/** @name Constructors and Destructors */
	//@{

	/**
	 * \brief Construct from an owning signature
	 *
	 * Called by the library when an Object needs to be created from an Object
	 * in a DOM tree.
	 *
	 * @param env The environment that the Object is operating within
	 * @param dom The DOM node that will be loaded
	 */

	DSIGObject(const XSECEnv * env, XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *dom);

	/**
	 * \brief Construct a new object
	 *
	 * Called by the library to create an Object from scratch
	 *
	 * @param env The environment the Object is operating within
	 */

	DSIGObject(const XSECEnv * env);


	/**
	 * \brief Destructor
	 */

	~DSIGObject();

	//@}

	/** @name Library functions */
	//@{

	/**
	 * \brief Load the object from DOM
	 *
	 * Called by the library to load a constructed object
	 */

	void load(void);

	/**
	 * \brief Create a new Object
	 *
	 * Create a new Object from scratch (will generate the DOM)
	 */

	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * 
		createBlankObject(void);

	//@}

	/** @name Get functions */
	//@{

	/**
	 * \brief Get the Id for this object
	 *
	 * @returns the URI attribute string for this object
	 */

	const XMLCh * getId(void);

	/**
	 * \brief Returns the MimeType string of this object
	 *
	 * @returns a pointer to the buffer containing the Mime Type string
	 */

	const XMLCh * getMimeType(void);

	/**
	 * \brief Returns the Encoding string of this object
	 *
	 * @returns a pointer to the buffer containing the Encoding string
	 */

	const XMLCh * getEncoding(void);

	/**
	 * \brief Returns the Element node for this object
	 *
	 * @returns the Element node at the head of this object
	 */

	const XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * getElement(void);

	//@}

	/** @name Setter functions */
	//@{
	
	/**
	 * \brief Set the Id attribute for this Object
	 *
	 * @param id String to use for the Id attribute
	 */

	void setId(const XMLCh * id);

	/**
	 * \brief Set the Id attribute for this Object
	 *
	 * @param type String to use for the MimeType attribute
	 */

	void setMimeType(const XMLCh * type);

	/**
	 * \brief Set the Encoding attribute for this Object
	 *
	 * @param encoding String to use for the Encoding attribute
	 */

	void setEncoding(const XMLCh * encoding);

	/**
	 * \brief Add a child node to the Object
	 *
	 * This is a "ease of use" function to allow users to add a DOM structure
	 * that has been built previously into the Object element
	 */

	void appendChild(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode * child);

	//@}


private:

	const XSECEnv		* mp_env;
	XERCES_CPP_NAMESPACE_QUALIFIER DOMNode
						* mp_objectNode;
	XERCES_CPP_NAMESPACE_QUALIFIER DOMNode
						* mp_idAttr;
	XERCES_CPP_NAMESPACE_QUALIFIER DOMNode
						* mp_mimeTypeAttr;
	XERCES_CPP_NAMESPACE_QUALIFIER DOMNode
						* mp_encodingAttr;

};

#endif /* DSIGOBJECT_INCLUDE */

