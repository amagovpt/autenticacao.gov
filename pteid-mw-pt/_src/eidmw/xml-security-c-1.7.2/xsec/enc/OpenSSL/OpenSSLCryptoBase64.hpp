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
 * OpenSSLCryptoBase64 := Base virtual class to define a base64 encoder/decoder
 *
 * Author(s): Berin Lautenbach
 *
 * $Id: OpenSSLCryptoBase64.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef OPENSSLCRYPTOBASE64_INCLUDE
#define OPENSSLCRYPTOBASE64_INCLUDE

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/enc/XSECCryptoBase64.hpp>

// OpenSSL
#if defined (XSEC_HAVE_OPENSSL)
#	include <openssl/evp.h>

/**
 * @ingroup opensslcrypto
 */
 /*\@{*/

/**
 * \brief Base64 encode/decode handler interface class.
 *
 * The XSEC library will use implementations of this interface 
 * for translating bytes to/from base64 encoding.
 *
 * Uses the EVP decode/encode routines in OpenSSL to perform the 
 * work.
 * 
 * 
 * 
 * @note Requires implementation of OpenSSL > 0.9.6e as there was a bug
 * in the Base64 decoding routines in this version and prior.
 *
 */


class DSIG_EXPORT OpenSSLCryptoBase64 : public XSECCryptoBase64 {


public :

	
	OpenSSLCryptoBase64() {};
	virtual ~OpenSSLCryptoBase64() {};

	/** @name Decoding Functions */
	//@{

	/**
	 * \brief Initialise the base64 object.
	 *
	 * Initialises the OpenSSL decode context and gets ready for data
	 * to be decoded.
	 *
	 */

	virtual void decodeInit(void);

	/**
	 * \brief Decode some passed in data.
	 *
	 * Pass the encoded data through the OpenSSL base64 decode function
	 * and place the data in the outData buffer.
	 *
	 * @note The OpenSSL library is very unkind if the output buffer is
	 * not large enough.  It is the responsibility of the caller to ensure
	 * the buffer will take the data.
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
	 * Clean out any extra data in the OpenSSL decode context 
	 * variable into the outData buffer.
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
	 * Pass the data through the OpenSSL Base64 encoder and place
	 * the output in the outData buffer.  Will keep any "overhang"
	 * data in the context buffer ready for the next pass of input 
	 * data.
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
	 * Take any data left in the context variable, and create the
	 * tail of the base64 encoding.
	 *
	 * @param outData Buffer to place any remaining encoded data
	 * @param outLength Max amount of data to be placed in the buffer.
	 * @returns Amount of data placed in the outData buffer
	 */

	virtual unsigned int encodeFinish(unsigned char * outData,
							 	      unsigned int outLength);	// Finish


	//@}

	/** @name Library Specific Functions */
	//@{

	/**
	 * \brief Translate a base64 encoded BN to a bignum
	 *
	 * Take a ds:CryptoBinary number and translate to an OpenSSL
	 * representation of a "big number" BIGNUM.
	 *
	 */
	
	static BIGNUM * b642BN(char * b64in, unsigned int len);

	/**
	 * \brief Get OpenSSL encode context structure
	 */

	EVP_ENCODE_CTX * getOpenSSLEncodeEVP_ENCODE_CTX(void) {return &m_ectx;}

	/**
	 * \brief Get OpenSSL encode context structure
	 */

	EVP_ENCODE_CTX * getOpenSSLDecodeEVP_ENCODE_CTX(void) {return &m_dctx;}

	//@}

private :

	EVP_ENCODE_CTX m_ectx;				// Encode context
	EVP_ENCODE_CTX m_dctx;				// Decode context

};

/*\@}*/

#endif /* XSEC_HAVE_OPENSSL */
#endif /* OPENSSLCRYPTOBASE64_INCLUDE */
