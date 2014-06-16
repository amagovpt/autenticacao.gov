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
 * XSECAlgorithmHandler := Interface class to define handling of
 *						   encryption and signature algorithms
 *
 * $Id: XSECAlgorithmHandler.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef XSECALGHANDLER_INCLUDE
#define XSECALGHANDLER_INCLUDE

// XSEC Includes

#include <xsec/framework/XSECDefs.hpp>

class TXFMChain;
class XENCEncryptionMethod;
class XSECCryptoKey;
class safeBuffer;
class XSECBinTXFMInputStream;

XSEC_DECLARE_XERCES_CLASS(DOMDocument);

// Xerces

/**
 * @ingroup xenc
 *\@{*/



/**
 * @brief Interface class to provide handlers for processing different
 * encryption, signature and hashing types.
 *
 * The XENCCipher class allows users and callers to register algorithm
 * handlers for different Type URIs, as used in the various XML Signature
 * or Encryption elements.
 *
 * Default handlers are provided by the
 * library, and are used to process all algorithms defined as mandatory
 * (and many optional) within the standard.
 *
 * Users can extend this class to provide custom algorithm handlers
 * for their own classes.
 *
 * @note The library will use a single clone of any provided object for
 * a given algorithm.  So all implementation classes <b>must</b> be 
 * thread safe!
 */

class XSECAlgorithmHandler {

public:
	
	/** @name Constructors and Destructors */
	//@{
	
	virtual ~XSECAlgorithmHandler() {};

	//@}


	/** @name Encryption Methods */
	//@{

	/**
	 * \brief Encrypt an input Transform chain to a safeBuffer.
	 *
	 * This method takes a TXFMChain that will provide the plain
	 * text data, and places the encrypted and base64 encoded output 
	 * in a safeBuffer.
	 *
	 * The EncryptionMethod object is provided so that any algorithm
	 * specific parameters can be embedded by the processor.  Default
	 * parameters can be set directly (OAEParams and KeySize).  Anything
	 * additional will need to be set within the DOM directly.
	 *
	 * @param plainText Chain that will provide the plain bytes.  Ownership
	 * remains with the caller - do not delete.
	 * @param encryptionMethod Information about the algorithm to use.
	 * Can also be used to set the required encryption parameters
	 * @param key The key that has been provided by the calling 
	 * application to perform the encryption.
	 *
     * @param doc Document in which to operate
     * @param result SafeBuffer object to place result into
	 * @note This is not quite the symmetric opposite of decryptToSafeBuffer
	 * because of the way the library uses transformers.  It is expected
	 * that this method will create a safeBuffer with <b>base64</b> encoded
	 * data.  (It's easier to throw a TXFMBase64 txfmer on the end of the
	 * chain than to do the encryption and then separately base64 encode.)
	 */

	virtual bool encryptToSafeBuffer(
		TXFMChain * plainText,
		XENCEncryptionMethod * encryptionMethod,
		XSECCryptoKey * key,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument * doc,
		safeBuffer & result
	) = 0;

	//@}

	/** @name Decryption Methods */
	//@{

	/**
	 * \brief Decrypt an input Transform chain to a safeBuffer.
	 *
	 * This method takes a TXFMChain that will provide the cipher
	 * text data, and places the output in a safeBuffer.
	 *
	 * The EncryptionMethod object is provided so that any algorithm
	 * specific parameters can be found by the processor.  It also
	 * allows applications to embed multiple algorithms in a single
	 * processing object.  The Type value can then be read from the
	 * EncryptionMethod object to determine what to do.
	 *
	 * @param cipherText Chain that will provide the cipherText.
	 * Ownership remains with the caller - do not delete.
	 * @param encryptionMethod Information about the algorithm to use
	 * @param key The key that has been determined via a resolver or
	 * that has been provided by the calling application.
     * @param doc Document in which to operate
     * @param result SafeBuffer object to place result into
	 * @returns The number of plain bytes placed in the safeBuffer
	 */

	virtual unsigned int decryptToSafeBuffer(
		TXFMChain * cipherText,
		XENCEncryptionMethod * encryptionMethod,
		XSECCryptoKey * key,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument * doc,
		safeBuffer & result
	) = 0;

	/**
	 * \brief Append an appropriate decrypt TXFMer to a cipher txfm chain.
	 *
	 * This method takes a TXFMChain that will provide the cipher
	 * text data, and appends the appropriate cipher transformer to
	 * decrypt the output.
	 *
	 * The EncryptionMethod object is provided so that any algorithm
	 * specific parameters can be found by the processor.  It also
	 * allows applications to embed multiple algorithms in a single
	 * processing object.  The Type value can then be read from the
	 * EncryptionMethod object to determine what to do.
	 *
	 * @param cipherText Chain that will provide the cipherText.
	 * Ownership remains with the caller - do not delete.
	 * @param encryptionMethod Information about the algorithm to use
	 * @param key The key that has been determined via a resolver or
	 * that has been provided by the calling application.
     * @param doc Document from which to create XML Nodes
	 * @returns The resulting BinInputStream
	 */

	virtual bool appendDecryptCipherTXFM(
		TXFMChain * cipherText,
		XENCEncryptionMethod * encryptionMethod,
		XSECCryptoKey * key,
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument * doc
	) = 0;


	//@}

	/** @name SigningMethods */
	//@{

	/**
	 * \brief Sign an input TXFMChain.
	 *
	 * This method takes a TXFMChain that will provide the plain
	 * text data, and places a (null terminated) base64 encoded 
	 * hash into a result safeBuffer.
	 *
	 * Unlike the encryption methods, the URI must also be passed in,
	 * as a single AlgorithmHandler might be used to support multiple
	 * different signing algorithms, but the overhead of passing in the
	 * entire SignedInfo is not required.
	 *
	 * @param plainText Chain that will provide the plain bytes.  Ownership
	 * remains with the caller - do not delete.
	 * @param URI Algorithm URI to use for the signing operation.
	 * @param key The key that has been provided by the calling 
	 * application to perform the signature.
	 * @param outputLength Max length of an HMAC signature to use in output
     * @param result SafeBuffer object to place result into
	 * @returns the number of bytes placed in the output safeBuffer
	 */

	virtual unsigned int signToSafeBuffer(
		TXFMChain * inputBytes,
		const XMLCh * URI,
		XSECCryptoKey * key,
		unsigned int outputLength,
		safeBuffer & result
	) = 0;

	/**
	 * \brief Validate a signature using an input TXFMChain
	 *
	 * Using the input plain text bytes (in the form of a transform chain)
	 * validate the associated base64 encoded signature using the supplied
	 * key.
	 *
	 * The URI value is used to determine the algorithm to be called upon to
	 * validate the signature.
	 *
	 * @param plainText Chain that will provide the plain bytes.  Ownership
	 * remains with the caller - do not delete.
	 * @param URI Algorithm URI to use for the signing operation.
	 * @param sig Base64 encoded signature value
	 * @param key The key that has been provided by the calling 
	 * application to perform the signature.
	 * @returns true if successful validate, false otherwise
	 */
	 
	virtual bool verifyBase64Signature(
		TXFMChain * inputBytes,
		const XMLCh * URI,
		const char * sig,
		unsigned int outputLength,
		XSECCryptoKey * key
	) = 0;

	//@}

	/** @name Hash appending */
	//@{

	/**
	 * \brief Append a signature hash to a TXFM Chain based on URI
	 *
	 * Given a URI string create the appropriate hash TXFM.  NOTE the
	 * input URI should be a SIGNATURE URI - e.g. #hash-sha1
	 *
	 * @param inputBytes the Input TXFMChain to append the hash to
	 * @param uri URI string to match hash against
	 * @param key The key for this signature (in case of an HMAC hash)
	 * @returns true if a match was found and a hash was appended
	 */

	virtual bool appendSignatureHashTxfm(
		TXFMChain * inputBytes,
		const XMLCh * URI,
		XSECCryptoKey * key
	) = 0;

	/**
	 * \brief Append a hash to a TXFM Chain based on URI
	 *
	 * Given a URI string create the appropriate hash TXFM.  NOTE the
	 * input URI should be a "stright" (i.e. non-signature) hash
	 * algorithm URI - e.g. #sha1
	 *
	 * @param inputBytes the Input TXFMChain to append the hash to
	 * @param uri URI string to match hash against
	 * @returns true if a match was found and a hash was appended
	 */

	virtual bool appendHashTxfm(
		TXFMChain * inputBytes,
		const XMLCh * URI
	) = 0;

	//@}

	
	/** @name Key handling */
	//@{

	/**
	 * \brief Create a key that will support a given URI
	 *
	 * Given a URI string and a raw bit string, create the associated key
	 *
	 * @param uri URI string to match key to
	 * @param keyBuffer Raw bits to set in the key
	 * @param keyLen Number of bytes in the key
	 */

	virtual XSECCryptoKey * createKeyForURI(
		const XMLCh * uri,
		const unsigned char * keyBuffer,
		unsigned int keyLen
	) = 0;

	//@}

	/** @name Miscellaneous Functions */
	//@{

	/**
	 * \brief Create a new instance of the handler
	 *
	 * Provides a means for the library to create a new instance
	 * of the object without knowing its type
	 */

	virtual XSECAlgorithmHandler * clone(void) const = 0;

	//@}

};

/*\@}*/

#endif /* XSECALGHANDLER_INCLUDE*/

