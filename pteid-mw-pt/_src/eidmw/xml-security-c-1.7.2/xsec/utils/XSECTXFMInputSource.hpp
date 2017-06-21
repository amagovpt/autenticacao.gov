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
 * XSECTXFMInputSource := Transfomer InputSource for Xerces Parser.
 *
 * $Id: XSECTXFMInputSource.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *					 
 */


#ifndef XSECTXFMINPUTSOURCE_INCLUDE
#define XSECTXFMINPUTSOURCE_INCLUDE

#include <xsec/framework/XSECDefs.hpp>
#include <xercesc/sax/InputSource.hpp>

class TXFMChain;

XSEC_DECLARE_XERCES_CLASS(BinInputStream);

/**
 * @brief InputSource wrapper for a TXFMList.
 * @ingroup interfaces
 *
 * This class provides a wrapper for a TXFMList.  It is used to provide 
 * an input to the Xerces Parser.
 *
 * @note This is a one-off use class.  In the process of providing bytes to
 * the parser, it runs through the transforms, which (currently) cannot be
 * reset.
 *
 */


class DSIG_EXPORT XSECTXFMInputSource : public XERCES_CPP_NAMESPACE_QUALIFIER InputSource
{

public :

	/** @name Constructors and Destructors */
	//@{

	/**
	 * \brief Construct around an existing transform list
	 *
	 * @param lst The final TXFM element in the input chain.
	 * @param deleteWhenDone Flag to instruct the class to delete the chain when
	 * done.  By default set to true.
	 */

    XSECTXFMInputSource(TXFMChain * lst, bool deleteWhenDone = true);

	/**
	 * \brief Destructor
	 *
	 * Delete the object.  If deleteWhenDone was set during construction, will
	 * delete the chain if it has not already been done.
	 */

    virtual ~XSECTXFMInputSource();

	//@}

	/** @name Stream management methods */
	//@{

	/**
	 * \brief Interface method
	 *
	 * Returns an InputStream that can be read by the parser
	 */

	XERCES_CPP_NAMESPACE_QUALIFIER BinInputStream* makeStream() const;

	//@}

private :

	mutable TXFMChain			* mp_chain;			// End point of list
	bool						m_deleteWhenDone;	// Do we delete?

};


#endif /* XSECTXFMINPUTSOURCE_INCLUDE */
