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
 * XSECCryptoBase64 := Base virtual class to define a base64 encoder/decoder
 *
 * Author(s): Berin Lautenbach
 *
 * $Id: XSECCryptoBase64.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef XSECCRYPTOBASE64_INCLUDE
#define XSECCRYPTOBASE64_INCLUDE

#include <xsec/framework/XSECDefs.hpp>

/**
 * @ingroup crypto
 */
 /*\@{*/

/**
 * \brief Base64 encode/decode handler interface class.
 *
 * <p>The XSEC library will use implementations of this interface 
 * for translating bytes to/from base64 encoding.</p>
 *
 * <p>There are many places where XML DSIG uses Base64 encoding for 
 * embedding data in the \<Signature\> structure.  In some cases this object
 * is used.  In other cases, the library passes base64 code directly to
 * the cryptographic handler.</p>
 *
 * @note The library may re-use Base64 objects.  However it will always
 * call the ??Init function prior to re-use.  In addtion, the object does
 * not need to be able to handle concurrent encode/decode operations.
 */

class DSIG_EXPORT XSECCryptoBase64 {


public :

	// Constructors/Destructors
	
	XSECCryptoBase64() {};
	virtual ~XSECCryptoBase64() {};

	/** @name Decoding Functions */
	//@{

	/**
	 * \brief Initialise the base64 object.
	 *
	 * The XSEC library will <em>always</em> call this function prior
	 * to decoding any data.  This function will also be called when
	 * one decode (or encode) has been completed and the library wishes
	 * to re-use the object for another decode operation.
	 *
	 */

	virtual void		 decodeInit(void) = 0;

	/**
	 * \brief Decode some passed in data.
	 *
	 * <p>The XSEC library will pass a block of data into the decoder
	 * and request that as much as possible be decoded into the outData
	 * buffer.</p>
	 *
	 * <p>Due to the nature of base64, there may be data that cannot be
	 * fully decoded (not enough encoding through yet).  The implementation
	 * is expected to keep this in memory until another call to #decode()
	 * or a call to #decodeFinish().</p>
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
								unsigned int outLength) = 0;
	/**
	 * \brief Finish off a decode.
	 *
	 * <p>The library will call this when there is no more base64 data for
	 * the current decode.</p>
	 *
	 * @param outData Buffer to place any remaining decoded data
	 * @param outLength Max amount of data to be placed in the buffer.
	 * @returns Amount of data placed in the outData buffer
	 */

	virtual unsigned int decodeFinish(unsigned char * outData,
							 	      unsigned int outLength) = 0;

	//@}

	/** @name Encoding Functions */
	//@{

	/**
	 * \brief Initialise the base64 object for encoding
	 *
	 * The XSEC library will <em>always</em> call this function prior
	 * to encoding any data.  This function will also be called when
	 * one encode (or decode) has been completed and the library wishes
	 * to re-use the object for another encode operation.
	 *
	 */
	
	virtual void		 encodeInit(void) = 0;

	/**
	 * \brief Encode some passed in data.
	 *
	 * <p>The XSEC library will pass a block of data into the Encoder
	 * and request that as much as possible be encoded into the outData
	 * buffer.</p>
	 *
	 * <p>Due to the nature of the implementation, there may be data 
	 * that cannot be
	 * fully encoded (not enough data through yet).  The implementation
	 * is expected to keep this in memory until another call to #encode()
	 * or a call to #encodeFinish().</p>
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
								unsigned int outLength) = 0;
	/**
	 * \brief Finish off an encode.
	 *
	 * <p>The library will call this when there is no more data for
	 * the current encode operation.</p>
	 *
	 * @param outData Buffer to place any remaining encoded data
	 * @param outLength Max amount of data to be placed in the buffer.
	 * @returns Amount of data placed in the outData buffer
	 */


	virtual unsigned int encodeFinish(unsigned char * outData,
							 	      unsigned int outLength) = 0;

	//@}

	/** @name Utility functions */
	//@{

	/**
	 * \brief Clean a buffer of base64 text
	 *
	 * Creates a new copy of the passed in buffer with new lines in
	 * accord with the MIME standard.
	 *
	 * @param buffer The buffer of base64 text to clean
	 * @param bufLen The number of characters to clean in the buffer (0 for strlen)
	 * @param retBufLen The number of characters placed in the cleaned buffer
	 * @returns A clean version of the buffer
	 * @note This is NOT re-implemented in any of the crypto interfaces.  It is 
	 * a Base64 utility function only.
	 */

	static char * cleanBuffer(const char * buffer, unsigned int bufLen, unsigned int &retBufLen);

	//@}

};
/*\@}*/
#endif /* XSECCRYPTOBASE64_INCLUDE */
