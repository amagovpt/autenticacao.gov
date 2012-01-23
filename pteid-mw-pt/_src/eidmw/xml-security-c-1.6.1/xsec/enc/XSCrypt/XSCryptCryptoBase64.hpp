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
 * XSCryptCryptoBase64 := Internal implementation of a base64 encoder/decoder
 *
 * Author(s): Berin Lautenbach
 *
 * $Id: XSCryptCryptoBase64.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef XSCRYPTCRYPTOBASE64_INCLUDE
#define XSCRYPTCRYPTOBASE64_INCLUDE

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/enc/XSECCryptoBase64.hpp>
#include <xsec/utils/XSECSafeBuffer.hpp>

 /**
 * @defgroup xscryptcrypto Internal Crypto API Interface
 * @ingroup crypto
 * The XSCrypt Interface provides cryptographic functions that are missing
 * from other libraries.
 *
 * Currently this only provides Base64 functionality which is not
 * available in the Windows Crypto API.
 *
 */

class DSIG_EXPORT XSCryptCryptoBase64 : public XSECCryptoBase64 {


public :

	// Constructors/Destructors
	
	XSCryptCryptoBase64() : m_state(B64_UNINITIALISED) {};
	virtual ~XSCryptCryptoBase64() {};

	/** @name Decoding Functions */
	//@{

	/**
	 * \brief Initialise the base64 object.
	 *
	 * Initialises the OpenSSL decode context and gets ready for data
	 * to be decoded.
	 *
	 */

	virtual void		 decodeInit(void);					// Setup

	/**
	 * \brief Decode some passed in data.
	 *
	 * Decode the passed in data and place the data in the outData buffer
	 *
	 * @param inData Pointer to the buffer holding encoded data.
	 * @param inLength Length of the encoded data in the buffer
	 * @param outData Buffer to place decoded data into
	 * @param outLength Maximum amount of data that can be placed in
	 *        the buffer.
	 * @returns The number of bytes placed in the outData buffer.
	 */
	
	virtual unsigned int decode(const unsigned char * inData, 
						 	    unsigned int inLength,
								unsigned char * outData,
								unsigned int outLength);

	/**
	 * \brief Finish off a decode.
	 *
	 * Clean out any extra data from previous decode operations and place
	 * into the outData buffer.
	 *
	 * @param outData Buffer to place any remaining decoded data
	 * @param outLength Max amount of data to be placed in the buffer.
	 * @returns Amount of data placed in the outData buffer
	 */

	virtual unsigned int decodeFinish(unsigned char * outData,
							 	      unsigned int outLength);

	//@}

	/** @name Encoding Functions */
	//@{

	/**
	 * \brief Initialise the base64 object for encoding
	 *
	 * Get the context variable ready for a base64 decode
	 *
	 */

	virtual void		 encodeInit(void);

	/**
	 * \brief Encode some passed in data.
	 *
	 * @param inData Pointer to the buffer holding data to be encoded.
	 * @param inLength Length of the data in the buffer
	 * @param outData Buffer to place encoded data into
	 * @param outLength Maximum amount of data that can be placed in
	 *        the buffer.
	 * @returns The number of bytes placed in the outData buffer.
	 */

	virtual unsigned int encode(const unsigned char * inData, 
						 	    unsigned int inLength,
								unsigned char * outData,
								unsigned int outLength);

	/**
	 * \brief Finish off an encode.
	 *
	 * Take any data from previous encode operations and create the
	 * tail of the base64 encoding.
	 *
	 * @param outData Buffer to place any remaining encoded data
	 * @param outLength Max amount of data to be placed in the buffer.
	 * @returns Amount of data placed in the outData buffer
	 */


	virtual unsigned int encodeFinish(unsigned char * outData,
							 	      unsigned int outLength);

	//@}

private :

	enum b64state {

		B64_UNINITIALISED,
		B64_ENCODE,
		B64_DECODE
	};

	safeBuffer				m_inputBuffer;		// Carry over buffer
	safeBuffer				m_outputBuffer;		// Carry over output

	unsigned int			m_remainingInput;	// Number of bytes in carry input buffer
	unsigned int			m_remainingOutput;	// Number of bytes in carry output buffer

	bool					m_allDone;			// End found (=)

	b64state				m_state;			// What are we currently doing?

	unsigned int			m_charCount;		// How many characters in current line?

	// Private functions
	void canonicaliseInput(const unsigned char *inData, unsigned int inLength);

};

#endif /* XSCRYPTCRYPTOBASE64_INCLUDE */
