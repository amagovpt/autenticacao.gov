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
 * DSIGTransformList := Class for Loading and storing a list of references
 *
 * $Id: DSIGTransformList.cpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

// XSEC Includes
#include <xsec/dsig/DSIGTransformList.hpp>
#include <xsec/dsig/DSIGTransform.hpp>

DSIGTransformList::DSIGTransformList(void){

}

DSIGTransformList::~DSIGTransformList() {

	// Delete all the transforms contained in the list

	DSIGTransformList::TransformListVectorType::iterator it = m_transformList.begin();

	while (it != m_transformList.end()) {
		
		delete *it;
		it++;

	}

}


void DSIGTransformList::addTransform(DSIGTransform * ref) {

	m_transformList.push_back(ref);

}

DSIGTransformList::size_type DSIGTransformList::getSize(void) {

	return m_transformList.size();

}

void DSIGTransformList::removeTransform(size_type index) {

	if (index < m_transformList.size()) {

		TransformListVectorType::iterator i = m_transformList.begin();
		for (size_type j = 0; j < index; ++j)
			i++;
		
		if (i != m_transformList.end()) {
			delete *i;
			m_transformList.erase(i);
		}
	}
}

DSIGTransform * DSIGTransformList::item(DSIGTransformList::size_type index) {

	if (index < m_transformList.size())
		return m_transformList[index];

	return NULL;

}


bool DSIGTransformList::empty() {

	// Clear out the list - note we do NOT delete the transform elements

	return m_transformList.empty();

}
