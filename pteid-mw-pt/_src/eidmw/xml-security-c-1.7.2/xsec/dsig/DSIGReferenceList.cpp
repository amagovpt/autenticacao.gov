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
 * $Id: DSIGReferenceList.cpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

// XSEC Includes
#include <xsec/dsig/DSIGReferenceList.hpp>
#include <xsec/dsig/DSIGReference.hpp>

XERCES_CPP_NAMESPACE_USE

DSIGReferenceList::DSIGReferenceList() {}

DSIGReferenceList::~DSIGReferenceList() {

	// Delete all the references contained in the list

	ReferenceListVectorType::iterator iterator = m_referenceList.begin();

	while (iterator != m_referenceList.end()) {
		
		delete *iterator;
		iterator++;

	}

}


void DSIGReferenceList::addReference(DSIGReference * ref) {

	m_referenceList.push_back(ref);

}

DSIGReferenceList::size_type DSIGReferenceList::getSize() {

	return m_referenceList.size();

}

DSIGReference * DSIGReferenceList::removeReference(size_type index) {

	DSIGReference * ret = NULL;
	if (index < m_referenceList.size()) {

		ret = m_referenceList[index];
		m_referenceList.erase(m_referenceList.begin() + index);
	
	}

	return ret;

}

DSIGReference * DSIGReferenceList::item(ReferenceListVectorType::size_type index) {

	if (index < m_referenceList.size())
		return m_referenceList[index];

	return NULL;

}
bool DSIGReferenceList::empty() {

	// Clear out the list - note we do NOT delete the reference elements

	ReferenceListVectorType::iterator retTest;

	retTest = m_referenceList.erase(m_referenceList.begin(), m_referenceList.end());
	return (retTest == m_referenceList.end());

}


