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
 * DSIGTransformList := List that holds all the transforms in the Signature.
 *
 * Author(s): Berin Lautenbach
 *
 * $Id: DSIGTransformList.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef DSIGTRANSFORMLIST_INCLUDE
#define DSIGTRANSFORMLIST_INCLUDE


// XSEC Includes
#include <xsec/framework/XSECDefs.hpp>

// General includes
#include <vector>

class DSIGTransform;

/**
 * @ingroup internal
 */

/**
 * @brief The class used for holding Transform Elements within a signature.
 *
 * This class is the container for the \<Transforms\> list in a Reference or
 * KeyInfo list.  It holds a list of Transform elements that can be
 * manipulated by the caller, or asked to provide the appropriate
 * TXFM* class to actually perform a transform.
 *
 */


class DSIG_EXPORT DSIGTransformList {

public:

#if defined(XSEC_NO_NAMESPACES)
	typedef vector<DSIGTransform *>			TransformListVectorType;
#else
	typedef std::vector<DSIGTransform *>	TransformListVectorType;
#endif

#if defined(XSEC_SIZE_T_IN_NAMESPACE_STD)
	typedef std::size_t		size_type;
#else
	typedef size_t			size_type;
#endif

	/** @name Constructors and Destructors */
	//@{

	/**
	 * \brief Construct the list
	 *
	 */

	DSIGTransformList();

	/**
	 * \brief Destroy all Transform resources
	 *
	 * Destroys the list - including the contained DSIGTransform* elements.
	 * Does not destroy the underlying DOM structure.
	 *
	 */

	~DSIGTransformList();

	//@}

	/** @name Manipulate existing structures */
	//@{

	/**
	 * \brief Add a transform to the list
	 *
	 * Should never be called directly - will add a pre-built
	 * transform to the list.
	 *
	 * @note Will not add any DOM structures
	 * @param ref The transform structure to add
	 */

	void addTransform(DSIGTransform * ref);

	/**
	 * \brief Remove a transform from the list.
	 *
	 * Should never be called directly - will simply remove the element
	 * without deleting
	 */

	void removeTransform(size_type index);

	/**
	 * \brief Delete the transform at the indicated position.
	 *
	 * @param index The position to delete from.
	 */

	DSIGTransform * item(size_type index);

	/** 
	 * \brief Get the number of items.
	 *
	 */

	size_type	getSize();

	/**
	 * \brief Remove all elements - but delete none.
	 */

	bool empty();


	// Get information

private:

	TransformListVectorType					m_transformList;
};


#endif /* DSIGTRANSFORMLIST_INCLUDE */
