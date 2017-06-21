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
 * XSECXPathNodeList := A structure to hold node lists from XPath 
 * evaluations
 *
 * Author(s): Berin Lautenbach
 *
 * $Id: XSECXPathNodeList.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef XSECXPATHNODELIST_INCLUDE
#define XSECXPATHNODELIST_INCLUDE

// XSEC
#include <xsec/framework/XSECDefs.hpp>

// Xerces

XSEC_DECLARE_XERCES_CLASS(DOMNode)

#define _XSEC_NODELIST_DEFAULT_SIZE	100

/**
 * @ingroup internal
 */

/**
 * \brief Class for holding lists of DOMNodes.
 *
 * This class is used primarily for holding lists of nodes found during XPath
 * processing.  It is also used for xpath-filter which requires multiple list
 * comparisons.
 *
 * It is not implemented using one of the container classes as it has the
 * potential to become a real bottleneck.  It could potentially be implemented
 * as a hash list based on names of nodes (or even pointers).
 *
 */

class DSIG_EXPORT XSECXPathNodeList {

public:

	/** @name Constructors, Destructors and operators */
	//@{

	XSECXPathNodeList(unsigned int initialSize = _XSEC_NODELIST_DEFAULT_SIZE);

	/**
	 * \brief Copy Constructor
	 *
	 * @note The XSEC Library generally passes NodeLists around as pointers.
	 * There is a LARGE overhead associated with re-creating lists, so we
	 * try to avoid doing it unless necessary.
	 */

	XSECXPathNodeList(const XSECXPathNodeList &other);

	~XSECXPathNodeList();

	/**
	 * \brief Assignment Operator.
	 *
	 * Set one node list equal to another.
	 *
	 * @note Do not do this frequently, particularly for large lists
	 * as it will replicate the entire list and is a fairly expensive
	 * operation
	 *
	 * @param toCopy The list to be copied from
	 */

	XSECXPathNodeList & operator= (const XSECXPathNodeList & toCopy);


	//@}


	/** @name Adding and Deleting nodes */
	//@{

	/**
	 *\brief Add a node to the list.
	 *
	 * Checks to see whether the node is already in the list, and if not
	 * adds it.
	 *
	 * @param n The node to add.
	 */

	void addNode(const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *n);

	/**
	 * \brief Remove a node from the list.
	 *
	 * Given a node, find it in the list and (if it exists) delete it from the list.
	 *
	 * @param n The node to be removed.
	 */

	void removeNode(const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *n);

	/**
	 * \brief Clear out the entire list, deleting all entries.
	 *
	 */

	void clear(void);

	//@}

	/** @name Reading List Functions */
	//@{

	/**
	 * \brief Check if a node exists in the list.
	 *
	 * @param n The node to find in the list.
	 */

	bool hasNode(const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *n) const;

	/**
	 * \brief Get the first node in the list.
	 *
	 * Returns the first node in the list of nodes and resets the search list.
	 *
	 * @returns The first node in the list or NULL if none exist
	 */

	const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode * getFirstNode(void) const;

	/**
	 * \brief Get the next node in the list
	 *
	 * Returns the next node in the list.
	 *
	 * @returns The next node in the list of NULL if none exist
	 */

	const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *getNextNode(void) const;

	//@}

	/** @name Manipulating Nodesets */
	//@{

	/**
	 *\brief Intersect with nodeset
	 *
	 * Delete any nodes in my list that are not in the intersect list
	 *
	 * @param toIntersect The list to intersect with.
	 */

	void intersect(const XSECXPathNodeList &toIntersect);

	//@}

private:

	/* Implement an unbalanced binary search tree */
	typedef struct s_btn {
		struct s_btn * l;		// Left
		struct s_btn * r;		// Right
		struct s_btn * p;		// Parent
		const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode
					 * v;		// Value
		long           h;		// Height
	} btn;

	// Internal functions
	btn * findNodeIndex(const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode * n) const;
	void delete_tree(btn * t);
	btn * copy_tree(btn * t) const ;
	long balance_count(btn * t) const;
	void rotate_left(btn * t);
	void rotate_right(btn * t);
	long calc_height(btn * t);

	btn								* mp_tree;			// The tree
	unsigned int					m_num;				// Number of elements in the tree

	mutable btn						* mp_current;		// current point in list for getNextNode
};



#endif /* XSECXPATHNODELIST_INCLUDE */

