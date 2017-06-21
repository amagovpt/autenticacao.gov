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
 * TXFMChain := List class that holds and manipulates a chain of TXFM 
 *              objects.
 *
 * Author(s): Berin Lautenbach
 *
 * $Id: TXFMChain.cpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#include <xsec/transformers/TXFMBase.hpp>
#include <xsec/transformers/TXFMChain.hpp>


// -----------------------------------------------------------------------
//  deleteTransformChain = easy way to delete an entire chain of transforms
// -----------------------------------------------------------------------


void TXFMChain::deleteTXFMChain(TXFMBase * toDelete) {

	if (toDelete != NULL) {
		deleteTXFMChain(toDelete->input);
		delete toDelete;
	}

}

// --------------------------------------------------------------------------------
//           Constructors/Destructors
// --------------------------------------------------------------------------------

TXFMChain::TXFMChain(TXFMBase * baseTxfm, bool deleteChainWhenDone) :
mp_currentTxfm(baseTxfm),
m_deleteChainWhenDone(deleteChainWhenDone) {
}

TXFMChain::~TXFMChain() {

	if (m_deleteChainWhenDone)
		deleteTXFMChain(mp_currentTxfm);

}

// --------------------------------------------------------------------------------
//           Read hash
// --------------------------------------------------------------------------------

void TXFMChain::appendTxfm(TXFMBase * txfm) {

	TXFMBase * oldTxfm = mp_currentTxfm;
	mp_currentTxfm = txfm;

	// This may throw an exception, but if it does each TXFM type 
	// Guarantees that it will have made the input part of the 
	// chain before such an exception.  So the caller can clear out
	// the entire chain - including the new txfm - by deleting
	// *this.

	txfm->setInput(oldTxfm);

}

