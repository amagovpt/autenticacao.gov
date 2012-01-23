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
 * TXFMBase := Base (virtual) class that defines a DSIG Transformer
 * Given this is a pure virtual class, this file only implements friend functions
 *
 * Author(s): Berin Lautenbach
 *
 * $Id: TXFMBase.cpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#include <xsec/transformers/TXFMBase.hpp>
#include <xsec/framework/XSECError.hpp>

// -----------------------------------------------------------------------
//  Ensure name spaces are reset when this is destroyed
// -----------------------------------------------------------------------

TXFMBase::~TXFMBase() {

	if (mp_nse != NULL) {

		mp_nse->deleteAddedNamespaces();
		delete mp_nse;
		mp_nse = NULL;

	}

}


// -----------------------------------------------------------------------
//  For expanding name spaces when necessary
// -----------------------------------------------------------------------

bool TXFMBase::nameSpacesExpanded(void) {

	if (mp_nse != NULL)
		return true;

	if (input != NULL)
		return input->nameSpacesExpanded();

	return false;

}

void TXFMBase::expandNameSpaces(void) {

	if (mp_nse != NULL || (input != NULL && input->nameSpacesExpanded()))
		return;		// Already done
	
	XSECnew(mp_nse, XSECNameSpaceExpander(mp_expansionDoc));

	mp_nse->expandNameSpaces();

}

void TXFMBase::deleteExpandedNameSpaces(void) {

	if (mp_nse != NULL) {

		mp_nse->deleteAddedNamespaces();
		delete mp_nse;
		mp_nse = NULL;

	}

	if (input != NULL)
		input->deleteExpandedNameSpaces();

}		

void TXFMBase::activateComments(void) {

	if (input  != NULL) {

		// Only activate them if our input had them

		keepComments = input->keepComments;

	}

	else 
		
		keepComments = true;

}

