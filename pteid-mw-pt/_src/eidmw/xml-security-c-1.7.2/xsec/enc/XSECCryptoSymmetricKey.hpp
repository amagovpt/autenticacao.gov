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
 * XSECCryptoSymmetricKey := Bulk encryption algorithms should all be
 *							implemented via this interface
 *
 * Author(s): Berin Lautenbach
 *
 * $Id: XSECCryptoSymmetricKey.hpp 1352680 2012-06-21 20:58:07Z scantor $
 *
 */



#ifndef XSECCRYPTOSYMMETRICKEY_INCLUDE
#define XSECCRYPTOSYMMETRICKEY_INCLUDE

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/dsig/DSIGConstants.hpp>
#include <xsec/enc/XSECCryptoKey.hpp>

/**
 * \ingroup crypto
 */

/**
 * \brief Base interface definition for symmetric key material.
 *
 * All symmetric algorithms are implemented via this interface.
 * Unlike the asymmetric key definitions, this is not further
 * extended for particular algorithms.  Rather it defines
 * encrypt/decrypt functions that are implemented within particular
 * providers for a particular algorithm.
 */

class DSIG_EXPORT XSECCryptoSymmetricKey : public XSECCryptoKey {

public :

	/**
	 * \brief Symmetric Key types understood by the library
	 *
	 * This type defines the list of symmetric key types that the library
	 * understands.
	 */

	enum SymmetricKeyType {

		KEY_NONE,
		KEY_3DES_192,			/** 192 bit (3-Key) 3DES */
		KEY_AES_128,			/** 128 bit AES */
		KEY_AES_192,			/** 192 bit AES */
		KEY_AES_256				/** 256 bit AES */

	};

	enum SymmetricKeyMode {

		MODE_NONE,					/** An error condition */
		MODE_ECB,					/** Electronic Code Book */
		MODE_CBC,					/** Cipher Block Chaining */
        MODE_GCM                    /** Galois-Counter Mode */

	};


	/** @name Constructors and Destructors */
	//@{
	
	/**
	 * \brief Constructor
	 **/

	XSECCryptoSymmetricKey() {};

	/**
	 * \brief Destructor 
	 *
	 * Implementations must ensure that the held key is properly destroyed
	 * (overwritten) when key objects are deleted.
	 */

	virtual ~XSECCryptoSymmetricKey() {};

	//@}

	/** @name Basic CryptoKey Interface methods */
	//@{

	/**
	 * \brief Returns the type of this key.
	 */

	virtual KeyType getKeyType() const {return KEY_SYMMETRIC;}

	/**
	 * \brief Returns a string that identifies the crypto owner of this library.
	 */

	virtual const XMLCh * getProviderName() const = 0;

	/**
	 * \brief Clone the key
	 *
	 * All keys need to be able to copy themselves and return
	 * a pointer to the copy.  This allows the library to 
	 * duplicate keys.
	 */

	virtual XSECCryptoKey * clone() const = 0;

	//@}

	/** @name Symmetric key interface methods */
	//@{

	/**
	 * \brief What type of symmetric key is this?
	 *
	 * There are a number of different types of symmetric key.
	 * This method allows callers to determine the type of this
	 * particular key
	 */

	virtual SymmetricKeyType getSymmetricKeyType(void) const = 0;

	/**
	 * \brief Set the key from the provided bytes
	 *
	 * Symmetric keys can all be loaded from a buffer containing a series
	 * of bytes.
	 *
	 * @param key The buffer containing the key bytes
	 * @param keyLen The number of key bytes in the buffer
	 *
	 */

	virtual void setKey(const unsigned char * key, unsigned int keyLen) = 0;

   	/**
	 * \brief Initialise an decryption process
	 *
	 * Setup the key to get ready for a decryption session.
     *
	 * Callers can pass in an IV.  If one is not provided, 
	 * but the algorithm requires one (e.g. 3DES_CBC), then 
	 * implementations should assume that the start of the
	 * cipher text stream will in fact be the IV.
     *
     * Callers may need to pass a tag to fully initialise an
     * authenticated encryption algorithm. If a tag is not
     * supplied, the caller can obtain the length of the required
     * tag instead, and call this method again once the tag is
     * obtained.
	 *
	 * @param doPad By default, we perform padding for last block
	 * @param mode mode selection, default is CBC
	 * @param iv Initialisation Vector to be used.  NULL if one is
	 * not required, or if IV will be set from data stream
     * @param tag Authentication tag to be used for AEAD ciphers
     * @param taglen length of Authentication Tag
	 * @returns true if the initialisation succeeded.
	 */

	virtual bool decryptInit(bool doPad = true,
							 SymmetricKeyMode mode = MODE_CBC,
							 const unsigned char* iv = NULL,
                             const unsigned char* tag = NULL,
                             unsigned int taglen = 0) = 0;

	/**
	 * \brief Continue a decrypt operation using this key.
	 *
	 * Decryption must have been set up using a decryptInit
	 * call.  Takes the inBuf and continues a decryption operation,
	 * writing the output to outBuf.
	 *
	 * This function does not have to guarantee that all input
	 * will be decrypted.  In cases where the input is not a length
	 * of the block size, the implementation will need to hold back
	 * cipher-text to be handles during the next operation.
	 *
	 * @param inBuf Octets to be decrypted
	 * @param plainBuf Buffer to place output in
	 * @param inLength Number of bytes to decrypt
	 * @param maxOutLength Maximum number of bytes to place in output 
	 * buffer
	 * @returns Bytes placed in output Buffer
	 */

	virtual unsigned int decrypt(const unsigned char * inBuf, 
								 unsigned char * plainBuf, 
								 unsigned int inLength,
								 unsigned int maxOutLength) = 0;

	/**
	 * \brief Finish a decryption operation
	 *
	 * Complete a decryption process.  No cipher text is passed in,
	 * as this should simply be removing any remaining text from
	 * the plain storage buffer.
	 *
	 * May throw an exception if there is some stored cipher text
	 * that is not the length of the block size for block algorithms.
	 *
	 * @param plainBuf Buffer to place any remaining plain text in
	 * @param maxOutLength Maximum number of bytes to pace in output
	 * @returns Bytes placed in output buffer
	 */

	virtual unsigned int decryptFinish(unsigned char * plainBuf,
									   unsigned int maxOutLength) = 0;

	/**
	 * \brief Initialise an encryption process
	 *
	 * Setup the key to get ready for a decryption session.
	 * Callers can pass in an IV.  If one is not provided, 
	 * but the algorithm requires one (e.g. 3DES_CBC), then
	 * implementations are required to generate one.
	 *
	 * @param doPad By default, we perform padding for last block
	 * @param mode What mode to handle blocks. default is CBC
	 * @param iv Initialisation Vector to be used.  NULL if one is
	 * not required, or if IV is to be generated
	 * @returns true if the initialisation succeeded.
	 */

	virtual bool encryptInit(bool doPad = true, 
							 SymmetricKeyMode mode = MODE_CBC,
							 const unsigned char * iv = NULL) = 0;

	/**
	 * \brief Continue an encryption operation using this key.
	 *
	 * Encryption must have been set up using an encryptInit
	 * call.  Takes the inBuf and continues a encryption operation,
	 * writing the output to outBuf.
	 *
	 * This function does not have to guarantee that all input
	 * will be encrypted.  In cases where the input is not a length
	 * of the block size, the implementation will need to hold back
	 * plain-text to be handled during the next operation.
	 *
	 * @param inBuf Octets to be encrypted
	 * @param cipherBuf Buffer to place output in
	 * @param inLength Number of bytes to encrypt
	 * @param maxOutLength Maximum number of bytes to place in output 
	 * buffer
	 * @returns Bytes placed in output Buffer
	 */

	virtual unsigned int encrypt(const unsigned char * inBuf, 
								 unsigned char * cipherBuf, 
								 unsigned int inLength,
								 unsigned int maxOutLength) = 0;

	/**
	 * \brief Finish an encryption operation
	 *
	 * Complete an encryption process.  No plain text is passed in,
	 * as this should simply be removing any remaining text from
	 * the plain storage buffer and creating a final padded block.
	 *
	 * Padding is performed by taking the remaining block, and
	 * setting the last byte to equal the number of bytes of
	 * padding.  If the plain was an exact multiple of the block size,
	 * then an extra block of padding will be used.  For example, if 
	 * the block size is 8 bytes, and there were three remaining plain
	 * text bytes (0x01, 0x02 and 0x03), the final block will be :
	 *
	 * 0x010203????????05
	 *
	 * @param plainBuf Buffer to place final block of cipher text in
	 * @param maxOutLength Maximum number of bytes to pace in output
     * @param taglen length of Authentication Tag
	 * @returns Bytes placed in output buffer
	 */

	virtual unsigned int encryptFinish(unsigned char * plainBuf,
									   unsigned int maxOutLength,
                                       unsigned int taglen = 0) = 0;

	//@}

};


#endif /* XSECCRYPTOSYMMETRICKEY_INCLUDE */
