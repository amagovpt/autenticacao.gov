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
 * $Id: TXFMChain.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef TXFMCHAIN_INCLUDE
#define TXFMCHAIN_INCLUDE

#include <xsec/framework/XSECDefs.hpp>

class TXFMBase;


/**
 * \brief A holder class to take care of a chain of internal
 * transformer objects.
 * @ingroup internal
 */


class DSIG_EXPORT TXFMChain {

public:

	// Constructors/destructors

	TXFMChain(TXFMBase * baseTxfm, bool deleteChainWhenDone = true);
	~TXFMChain();

	// Manipulate
	void appendTxfm(TXFMBase * txfm);
	TXFMBase * getLastTxfm(void);

private:

	TXFMChain();
	TXFMChain(TXFMChain &toCopy);

	TXFMBase				* mp_currentTxfm;
	bool					m_deleteChainWhenDone;

	void deleteTXFMChain(TXFMBase * toDelete);

};

inline
TXFMBase * TXFMChain::getLastTxfm(void) {return mp_currentTxfm;}



#endif /* TXFMCHAIN_INCLUDE */
