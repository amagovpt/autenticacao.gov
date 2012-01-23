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
 * DSIGReferenceList := Class for Loading and storing a list of references
 *
 * $Id: DSIGReferenceList.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef DSIGREFERENCELIST_INCLUDE
#define DSIGREFERENCELIST_INCLUDE


// XSEC Includes
#include <xsec/framework/XSECDefs.hpp>
//#include <dsig/DSIGReference.hpp>

// General includes
#include <vector>

class DSIGReference;

/**
 * @ingroup pubsig
 */

/**
 * @brief Holds a list of reference elements.
 *
 * The library holds a Signature's list of references using this class.
 * Manifest lists are similarly held.  Developers can obtain the 
 * list from the Signature library using DSIGSignature::getReferenceList
 * and then iterate through each reference.
 *
 * The library also uses the List as the owner of the memory associated
 * with References.  So when a list is deleted, the references are deleted
 * at the same time.
 *
 */

class DSIG_EXPORT DSIGReferenceList {

public:

#if defined(XSEC_NO_NAMESPACES)
	typedef vector<DSIGReference *>			ReferenceListVectorType;
#else
	typedef std::vector<DSIGReference *>	ReferenceListVectorType;
#endif

#if defined(XSEC_SIZE_T_IN_NAMESPACE_STD)
	typedef std::size_t		size_type;
#else
	typedef size_t			size_type;
#endif

	/** @name Constructors and Destructors */
	//@{

	/**
	 * \brief Construct a list
	 */

	DSIGReferenceList();

	/**
	 * \brief Destroy a list.
	 *
	 * Standard destructor to close down the list.
	 *
	 * @note Destroys the references as well as the list
	 */

	~DSIGReferenceList();

	//@}

	/** @name List manipulation */
	//@{

	/**
	 * \brief Add a reference to the list
	 *
	 * Appends the reference to the end of the list and takes ownership
	 * of associated memory.
	 */
	
	void addReference(DSIGReference * ref);

	/**
	 * \brief Remove a reference from the list
	 *
	 * Removes the reference at the index point and returns a pointer
	 * to the reference removed.
	 *
	 * @note This also releases ownership.  It is the responsibility of
	 * the caller to ensure the reference is deleted.
	 *
	 * @note This does not currently delete the reference from the Signature 
	 *
	 * @param index Point in the list to remove
	 */
	
	DSIGReference * removeReference(size_type index);

	/**
	 * \brief Return a reference from the list
	 *
	 * Gets the reference at point index in the list
	 *
	 * @param index The pointer into the list
	 */

	DSIGReference * item(size_type index);

	/**
	 * \brief Find the number of elements in the list
	 *
	 * @returns The number of elements in the list
	 */

	size_type	getSize();

	/**
	 * \brief Clear out the list
	 *
	 * Simply clears the list.  Does not delete the reference elements themselves.
	 */
	
	bool empty();

	//@}

private:

	ReferenceListVectorType					m_referenceList;

};


#endif /* DSIGREFERENCELIST_INCLUDE */
