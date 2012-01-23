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
 * Author(s): Milan Tomic
 *
 */

#ifndef NSSCRYPTOSYMMETRICKEY_INCLUDE
#define NSSCRYPTOSYMMETRICKEY_INCLUDE

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/enc/XSECCryptoSymmetricKey.hpp>

#if defined (XSEC_HAVE_NSS)

#include <pk11func.h>
#include <nss.h>

#define NSS_MAX_BLOCK_SIZE		32

/**
 * \ingroup nsscrypto
 * @{
 */

/**
 * \brief Base interface definition for symmetric key material.
 *
 * This is the implementation for a wrapper of NSS symmetric
 * crypto functions.
 */

class DSIG_EXPORT NSSCryptoSymmetricKey : public XSECCryptoSymmetricKey {

public :

	/** @name Constructors and Destructors */
	//@{
	
	/**
	 * \brief Constructor
	 *
	 * Can only construct a Symmetric key if we know what type it is
	 *
	 * @param type The type of key (i.e. algorithm) to create
	 **/

	NSSCryptoSymmetricKey(XSECCryptoSymmetricKey::SymmetricKeyType type);

	/**
	 * \brief Destructor 
	 *
	 * Implementations must ensure that the held key is properly destroyed
	 * (overwritten) when key objects are deleted.
	 */

	virtual ~NSSCryptoSymmetricKey();

	//@}

	/** @name Basic CryptoKey Interface methods */
	//@{

	/**
	 * \brief Returns a string that identifies the crypto owner of this library.
	 */

	virtual const XMLCh * getProviderName() const;

	/**
	 * \brief Clone the key
	 *
	 * All keys need to be able to copy themselves and return
	 * a pointer to the copy.  This allows the library to 
	 * duplicate keys.
	 */

	virtual XSECCryptoKey * clone() const;

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

	SymmetricKeyType getSymmetricKeyType(void) const;

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

	void setKey(const unsigned char * key, unsigned int keyLen);

	/**
	 * \brief Initialise an decryption process
	 *
	 * Setup the key to get ready for a decryption session.
	 * Callers can pass in an IV.  If one is not provided, 
	 * but the algorithm requires one (e.g. 3DES_CBC), then 
	 * implementations should assume that the start of the
	 * cipher text stream will in fact be the IV.
	 *
	 * @param doPad By default, we perform padding for last block
	 * @param mode mode selection (Currently ECB or CBC mode only)
	 * @param iv Initialisation Vector to be used.  NULL if one is
	 * not required, or if IV will be set from data stream
	 * @returns true if the initialisation succeeded.
	 */

	virtual bool decryptInit(bool doPad = true,
							 SymmetricKeyMode mode = MODE_CBC,
							 const unsigned char * iv = NULL);

	/**
	 * \brief Continue an decrypt operation using this key.
	 *
	 * Decryption must have been set up using an encryptInit
	 * call.  Takes the inBuf and continues a decryption operation,
	 * writing the output to outBuf.
	 *
	 * This function does not have to guarantee that all input
	 * will be decrypted.  In cases where the input is not a length
	 * of the block size, the implementation will need to hold back
	 * cipher-text to be handles during the next operation.
	 *
	 * @note While maxOutLength is defined, the NSS libraries will
	 * not read the value, so the onus is on the caller to ensure the
	 * buffer is long enough to hold the output!
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
								 unsigned int maxOutLength);

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
	 * @note While maxOutLength is defined, the NSS libraries will
	 * not read the value, so the onus is on the caller to ensure the
	 * buffer is long enough to hold the output!
	 *
	 * @param plainBuf Buffer to place any remaining plain text in
	 * @param maxOutLength Maximum number of bytes to pace in output
	 * @returns Bytes placed in output buffer
	 */

	virtual unsigned int decryptFinish(unsigned char * plainBuf,
									   unsigned int maxOutLength);

	/**
	 * \brief Initialise an encryption process
	 *
	 * Setup the key to get ready for a decryption session.
	 * Callers can pass in an IV.  If one is not provided, 
	 * but the algorithm requires one (e.g. 3DES_CBC), then
	 * implementations are required to generate one.
	 *
	 * @param doPad By default, we perform padding for last block
	 * @param mode What mode to handle blocks (Currently CBC or ECB)
	 * @param iv Initialisation Vector to be used.  NULL if one is
	 * not required, or if IV is to be generated
	 * @returns true if the initialisation succeeded.
	 */

	virtual bool encryptInit(bool doPad = true, 
							 SymmetricKeyMode mode = MODE_CBC,
							 const unsigned char * iv = NULL);

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
								 unsigned int maxOutLength);

	/**
	 * \brief Finish a encryption operation
	 *
	 * Complete a encryption process.  No plain text is passed in,
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
	 * @param cipherBuf Buffer to place final block of cipher text in
	 * @param maxOutLength Maximum number of bytes to pace in output
	 * @returns Bytes placed in output buffer
	 */

	virtual unsigned int encryptFinish(unsigned char * plainBuf,
									   unsigned int maxOutLength);

	//@}

private:

	// Unimplemented constructors
	
	NSSCryptoSymmetricKey();
	NSSCryptoSymmetricKey(const NSSCryptoSymmetricKey &);
	NSSCryptoSymmetricKey & operator= (const NSSCryptoSymmetricKey &);

	int decryptCtxInit(const unsigned char * iv);

	SymmetricKeyType				m_keyType;
	SymmetricKeyMode				m_keyMode;		// ECB or CBC
	bool							  m_initialised;
	bool							m_doPad; //Does we need it?  Yes we do - key wraps are unpadded for 3DES

	unsigned char					m_lastBlock[NSS_MAX_BLOCK_SIZE];
	unsigned int					m_blockSize;
  bool							    m_ivSent;		// Has the IV been put in the stream
	unsigned int					m_ivSize;

  PK11Context *         mp_ctx;
	PK11SymKey *				  mp_k;

};

#endif /* XSEC_HAVE_NSS */
#endif /* NSSCRYPTOSYMMETRICKEY_INCLUDE */
