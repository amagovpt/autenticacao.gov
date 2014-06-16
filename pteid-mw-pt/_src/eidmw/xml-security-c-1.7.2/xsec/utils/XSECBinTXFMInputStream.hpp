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
 * XSECBinTXFMInputStream := Implement the BinInputStream around Transforms.
 *
 * Author(s): Berin Lautenbach
 *
 * $ID$
 *
 * $LOG$
 *
 */


#ifndef XSECBINTXFMINPUTSTREAM_INCLUDE
#define XSECBINTXFMINPUTSTREAM_INCLUDE

#include <xsec/framework/XSECDefs.hpp>
#include <xercesc/util/BinInputStream.hpp>

class TXFMChain;
class TXFMBase;

/**
 * @defgroup interfaces Classes that interface applications to XML Security C.
 *
 * These classes and files provide interfaces to allow the XSEC library to link
 * to other libraries or to allow other libraries/applications to interface
 * to XSEC.
 * @{
 */

/**
 * @brief BinInputSource wrapper for a TXFMChain.
 *
 * This class provides a wrapper for a TXFMChain.  It can be used to either provide
 * a nice interface to applications wishing to read the BYTESTREAM output of a
 * TXFM chain, or, as it is derived from BinInputStream, provide an input to the
 * Xerces Parser.
 *
 */


class DSIG_EXPORT XSECBinTXFMInputStream : public XERCES_CPP_NAMESPACE_QUALIFIER BinInputStream
{

public :

	/** @name Constructors and Destructors */
	//@{

	/**
	 * \brief Construct around an existing transform list
	 *
	 * @param lst The input TXFM chain.
	 * @param deleteWhenDone Flag to instruct the class to delete the chain when
	 * done.  By default set to true.
	 */

    XSECBinTXFMInputStream(TXFMChain * lst, bool deleteWhenDone = true);

	/**
	 * \brief Destructor
	 *
	 * Delete the object.  If deleteWhenDone was set during construction, will
	 * delete the chain if it has not already been done.
	 */

    virtual ~XSECBinTXFMInputStream();

	//@}

	/** @name Stream management methods */
	//@{

	/**
	 * \brief Reset the stream.
	 *
	 * @note At this time simply deletes the TXFM chain - many TXFMs do not
	 * have an ability to reset.
	 */

    void reset();

	//@}

	/** @name Interface */
	//@{

	/**
	 * \brief Current position in stream.
	 *
	 * @returns Bytes already returned.
	 */

#ifdef XSEC_XERCES_64BITSAFE
    virtual XMLFilePos curPos() const;
#else
    virtual unsigned int curPos() const;
#endif

	/**
	 * \brief Retrieve the required number of bytes and return
	 *
	 * Retrieve up to the requested number of bytes.  Does not always
	 * retrieve as much as requested, but will always retrieve something
	 * until completed.
	 *
	 * @note When complete, will return 0 and delete the TXFM chain if
	 * requested to do so in the constructor.
	 * @param toFill The buffer that will be read into.
	 * @param maxToRead Maximum number of bytes to return
	 * @returns The number of bytes read or 0 if complete.
	 */

    virtual xsecsize_t readBytes(XMLByte* const  toFill,
		const xsecsize_t maxToRead);


#ifdef XSEC_XERCES_INPUTSTREAM_HAS_CONTENTTYPE
    const XMLCh* getContentType() const;
#endif

private :

	TXFMBase					* mp_txfm;			// End point of list
	TXFMChain					* mp_chain;			// The actual chain
	bool						m_deleteWhenDone;	// Do we delete?
	bool						m_deleted;			// Have we deleted?
	bool						m_done;				// Are we done?
    xsecsize_t				    m_currentIndex;		// How much read?

};



#endif
