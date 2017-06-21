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
 * $Id: XSECXPathNodeList.cpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

// XSEC

#include <xsec/utils/XSECXPathNodeList.hpp>
#include <xsec/framework/XSECError.hpp>

#include <string.h>

XERCES_CPP_NAMESPACE_USE

// --------------------------------------------------------------------------------
//           Constructors and Destructors.
// --------------------------------------------------------------------------------

XSECXPathNodeList::XSECXPathNodeList(unsigned int initialSize) {

	m_num = 0;
	mp_tree = NULL;

}

XSECXPathNodeList::XSECXPathNodeList(const XSECXPathNodeList &other) {

	m_num = other.m_num;
	mp_tree = copy_tree(other.mp_tree);
	mp_current = NULL;

}

XSECXPathNodeList::~XSECXPathNodeList() {

	// Delete all the elements in the node list
	delete_tree(mp_tree);

}

XSECXPathNodeList & XSECXPathNodeList::operator= (const XSECXPathNodeList & toCopy) {

	// For now simply delete the old list and set with the new
	// Large overhead as we call other functions, but simplest way to
	// implement for now

	mp_tree = copy_tree(toCopy.mp_tree);
	m_num = toCopy.m_num;
	mp_current = NULL;
	return *this;

}

// --------------------------------------------------------------------------------
//           Utility Functions.
// --------------------------------------------------------------------------------

XSECXPathNodeList::btn * XSECXPathNodeList::findNodeIndex(const DOMNode *n) const {

	btn * t = mp_tree;

	while (t != NULL && t->v != n) {

		if (n > t->v)
			t = t->r;
		else
			t = t->l;
	}

	return t;

}

void XSECXPathNodeList::delete_tree(btn * t) {

	if (t == NULL)
		return;

	btn * parent, * n;

	n = t;
	while (n != NULL) {

		if (n->l)
			n = n->l;
		else if (n->r)
			n = n->r;

		else {
			parent = n->p;
			if (parent != NULL) {
				if (parent->l == n)
					parent->l = NULL;
				else 
					parent->r = NULL;
			}
			// Delete this node
			delete n;
			n = parent;
		}
	}

}

XSECXPathNodeList::btn * XSECXPathNodeList::copy_tree(btn * t) const {

	if (t == NULL)
		return NULL;

	btn * n, *c, *cp, *ret;
    c = cp = NULL;

	n = t;
	bool create = true;
	ret = NULL;
	while (n != NULL) {

		if (create) {
			XSECnew(c, btn);
			c->l = NULL;
			c->r = NULL;
			c->v = n->v;

			// R we at top?
			if (ret == NULL) {
				ret = c;
				c->p = NULL;
				cp = NULL;
			}

			else {
				c->p = cp;
				if (cp != NULL) {
					if (n->p->l == n)
						cp->l = c;
					else
						cp->r = c;
				}
			}
		}

		// Go down!
		if (c->l == NULL && n->l != NULL) {
			cp = c;
			n = n->l;
			create = true;
		}
		else if (c->r == NULL && n->r != NULL) {
			cp = c;
			n = n->r;
			create = true;
		}
		else {

			// Go Back Up!
			n = n->p;
			c = cp;
			if (cp != NULL)
				cp = cp->p;
			create = false;
		}
	}

	return ret;
}


// --------------------------------------------------------------------------------
//           Adding and Deleting Nodes.
// --------------------------------------------------------------------------------

long XSECXPathNodeList::balance_count(btn * t) const {

	if (t == NULL)
		return 0;

	long r = (t->r == NULL ? 0 : t->r->h);
	long l = (t->l == NULL ? 0 : t->l->h);

	return r - l;

}

long XSECXPathNodeList::calc_height(btn * t) {

	if (t == NULL)
		return 0;
	if (t->l == NULL) {
		if (t->r == NULL)
			return 1;
		return t->r->h + 1;
	}
	else {
		if (t->r == NULL)
			return t->l->h + 1;
	}

	return ((t->l->h > t->r->h ? t->l->h : t->r->h) + 1);
}

void XSECXPathNodeList::rotate_right(btn * t) {

	// Rotate me right!
	btn * lc = t->l;

	// First - are we at the root?
	if (t == mp_tree) {
		lc->p = NULL;
		mp_tree = lc;
	}
	else {
		if (t->p->l == t) {
			t->p->l = lc;
		}
		else {
			t->p->r = lc;
		}

		lc->p = t->p;
	}

	// Do the rotate
	t->l = lc->r;
	if (t->l)
		t->l->p = t;
	lc->r = t;
	t->p = lc;

	// Recalculate heights
	lc = t;
	while (lc != NULL) {
		lc->h = calc_height(lc);
		lc = lc->p;
	}

}


void XSECXPathNodeList::rotate_left(btn * t) {

	// Rotate me left!
	btn * rc = t->r;

	// First - are we at the root?
	if (t == mp_tree) {
		rc->p = NULL;
		mp_tree = rc;
	}
	else {
		if (t->p->r == t) {
			t->p->r = rc;
		}
		else {
			t->p->l = rc;
		}

		rc->p = t->p;
	}

	// Do the rotate
	t->r = rc->l;
	if (t->r)
		t->r->p = t;
	rc->l = t;
	t->p = rc;

	// Recalculate heights
	rc = t;
	while (rc != NULL) {
		rc->h = calc_height(rc);
		rc = rc->p;
	}

}

void XSECXPathNodeList::addNode(const DOMNode *n) {

	btn * v;

	if (m_num == 0) {
		XSECnew(mp_tree, btn);
		mp_tree->l = mp_tree->r = NULL;
		mp_tree->v = n;
		mp_tree->p = NULL;
		mp_tree->h = 1;
		m_num = 1;
		return;
	}

	// Find the node
	btn * t = mp_tree;
	btn * last = NULL;

	while (t != NULL && t->v != n) {
		last = t;
		if (n > t->v)
			t = t->r;
		else
			t = t->l;
	}

	if (t != NULL)
		// Node already exists in tree!
		return;

	// Work out the insertion.
	XSECnew(v, btn);
	v->v = n;
	v->r = v->l = NULL;
	v->h = 1;
	v->p = last;

	// Determine on which leg to place the new value
	if (n > last->v)
		last->r = v;
	else
		last->l = v;

	// Recalculate heights
	t = last;
	long newh;
	while (t != NULL) {
		newh = calc_height(t);
		if (newh > t->h) {
			t->h = newh;
			t = t->p;
		}
		else
			// If the height is the same here, then nothing will have changed above
			t = NULL;
	}

	// Rebalance!
	
	t = last;
	while (t != NULL) {		
		long bal = balance_count(t);
		long balrc = balance_count(t->r);
		long ballc = balance_count(t->l);

		// Case 1 - Balance is OK
		if (bal <= 1 && bal >= -1) {
			// Do nothing!
		}

		// Case 2a - Balance is -2 and LC = -1
		else if (bal == -2 && ballc == -1) {

			// Rotate current node right
			rotate_right(t);
		}
		// Or balance is +2 and RC = +1
		else if (bal == 2 && balrc  == 1) {

			// Rotate current node left
			rotate_left(t);
		}
		// Case 2b = Balance is -2 and LC = +1
		else if (bal == -2 && ballc == 1) {

			// Double Right rotation
			rotate_left(t->l);
			rotate_right(t);
		}

		else {
			rotate_right(t->r);
			rotate_left(t);

		}

		t = t->p;

	}

}

void XSECXPathNodeList::removeNode(const DOMNode *n) {

	btn * t, * last;
    last = NULL;
	btn * i = findNodeIndex(n);

	if (i == NULL)
		// Not found!
		return;

	// Delete from tree
	if (i == mp_tree) {
		// Bugger - we are at the top of the tree

		// OK - No children?
		if (i->l == NULL && i->r == NULL) {
			// WooHoo!  Easy!
			delete mp_tree;
			mp_tree = NULL;
		}

		// One Child?
		if (i->l != NULL && i->r == NULL) {

			// WooHoo! Easy!
			mp_tree = i->l;
			mp_tree->p = NULL;
			delete i;
		}
		if (i->r != NULL && i->l == NULL) {

			// WooHoo! Easy!
			mp_tree = i->r;
			mp_tree->p = NULL;
			delete i;
		}

		// Oh dear = we have two children and now some heartache
		if (i->r->l == NULL && i->r->r == NULL) {

			// No subtree on right hand side.
			mp_tree = mp_tree->l;
			mp_tree->p = NULL;
			t = mp_tree->r;
			last = mp_tree;
			while (t != NULL) {
				last = t;
				if (i->r->v < t->v)
					t = t->l;
				else
					t = t->r;
			}

			if (i->r->v < last->v) {
				last->l = i->r;
				i->r->p = last;
			}
			else {
				last->r = i->r;
				i->r->p = last;
			}
		}

		else {

			// Need to find the "in-order" successor
			t = i->r;

			while (t != NULL) {
				last = t;
				t=t->l;
			}

			if (last == i->r) {
				// can't have been a left hand leg on the next node!)
				last->l = i->l;
				if (last->l != NULL)
					last->l->p = last;

				mp_tree = last;
				last->p = NULL;

				delete i;
			}
			else {
			
				// OK - Last is now the next biggest node, and it doesn;t
				// have anything on it's left (otherwise there would be something smaller)

				last->p->l = last->r;
				last->r->p = last->p;
				last->l = i->l;
				last->r = i->r;
				if (last->r != NULL)
					last->r->p = last;
				if (last->l != NULL)
					last->l->p = last;

				mp_tree = last;
				last->p = NULL;
				
				delete i;

			}
		}
	}

	else { 
		
		/* i != mp_tree */
		// OK - No children?
		if (i->l == NULL && i->r == NULL) {
			// WooHoo!  Easy!
			if (i->p->l == i)
				i->p->l = NULL;
			else
				i->p->r = NULL;

			delete i;
		}

		// One Child?
		if (i->l != NULL && i->r == NULL) {

			// WooHoo! Easy!
			if (i->p->l == i) {
				i->p->l = i->l;
				i->l->p = i->p;
			}
			else {
				i->p->r = i->l;
				i->r->p = i->p;
			}
			delete i;
		}
		if (i->r != NULL && i->l == NULL) {

			// WooHoo! Easy!
			if (i->p->l == i) {
				i->p->l = i->r;
				i->r->p = i->p;
			}
			else {
				i->p->r = i->r;
				i->r->p = i->p;
			}
			delete i;
		}

		// Oh dear = we have two children and now some heartache
		if (i->r->l == NULL && i->r->r == NULL) {

			// No subtree on right hand side.
			if (i->p->l == i) {
				// Was left hand child
				i->p->l = i->l;
				i->l->p = i->p;
				t = i->l;

				// Find insertion point for dangling node
				while (t != NULL) {
					last = t;
					t = t->r;
				}

				last->r = i->r;
				i->r->p = last;
			}
			else {
				// Was right hand child
				i->p->r = i->l;
				i->l->p = i->p;
				t = i->l;

				// Find insertion point for dangling node
				while (t != NULL) {
					last = t;
					t = t->r;
				}

				last->r = i->r;
				i->r->p = last;
			}


		}

		else {

			// Subtree - so need to find the "in-order" successor
			t = i->r;

			while (t != NULL) {
				last = t;
				t=t->l;
			}

			// OK - Last is now the next biggest node, and it doesn;t
			// have anything on it's left (otherwise there would be something smaller)

			last->p->l = last->r;
			last->r->p = last->p;
			last->l = i->l;
			last->r = i->r;
			if (last->r != NULL)
				last->r->p = last;
			if (last->l != NULL)
				last->l->p = last;

			mp_tree = last;
			last->p = NULL;
			
			delete i;

		}
	}

	m_num--;
}

void XSECXPathNodeList::clear() {

	m_num = 0;
	delete_tree(mp_tree);
	mp_tree = NULL;

}

// --------------------------------------------------------------------------------
//           Information functions.
// --------------------------------------------------------------------------------


bool XSECXPathNodeList::hasNode(const DOMNode *n) const {

	btn * i = findNodeIndex(n);

	return (i != NULL);

}

const DOMNode *XSECXPathNodeList::getFirstNode(void) const {


	if (mp_tree == NULL) 
		return NULL;

	// Find the smallest node
	mp_current = mp_tree;
	while (mp_current->l != NULL)
		mp_current = mp_current->l;

	return mp_current->v;

}

const DOMNode *XSECXPathNodeList::getNextNode(void) const {

	if (mp_current == NULL)
		return NULL;

	btn * t = mp_current;

	if (t->r != NULL) {
		// Find next biggest node
		t = t->r;
		while (t->l != NULL)
			t = t->l;
		mp_current = t;
	}
	else {
		// Go backwards!
		t = mp_current->p;
		while (t != NULL && t->r == mp_current) {
			mp_current = t;
			t = t->p;
		}

		if (t == NULL) {
			mp_current = NULL;
			return NULL;
		}
		mp_current = t;
	}

	return t->v;

}
	
// --------------------------------------------------------------------------------
//           Intersect with another list
// --------------------------------------------------------------------------------

void XSECXPathNodeList::intersect(const XSECXPathNodeList &toIntersect) {

	// Create a new list
	XSECXPathNodeList ret;

	const DOMNode * n = getFirstNode();
	while (n != NULL) {

		if (toIntersect.hasNode(n))
			ret.addNode(n);

		n = getNextNode();

	}

	// Swap lists
	btn * t = mp_tree;
	mp_tree = ret.mp_tree;
	ret.mp_tree = t;
	m_num = ret.m_num;

	return;

}

