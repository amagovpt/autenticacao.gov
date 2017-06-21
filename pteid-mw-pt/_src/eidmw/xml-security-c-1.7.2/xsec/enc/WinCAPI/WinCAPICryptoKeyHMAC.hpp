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
 * WinCAPICryptoKeyHMAC := HMAC Keys
 *
 * Author(s): Berin Lautenbach
 *
 * $Id: WinCAPICryptoKeyHMAC.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef WINCAPICRYPTOKEYHMAC_INCLUDE
#define WINCAPICRYPTOKEYHMAC_INCLUDE

#include <xsec/enc/XSECCryptoKeyHMAC.hpp>

#if defined (XSEC_HAVE_WINCAPI)

#if !defined(_WIN32_WINNT)
#	define _WIN32_WINNT 0x0400
#endif

#include <wincrypt.h>

/**
 * \ingroup wincapicrypto
 */

/**
 * \brief Windows Crypto API implementation for HMAC keys.
 *
 * Used to provide HMAC keys to WinCAPI CryptoHashHMAC
 *
 * Provides two types of key.
 *
 * A <b>Windows Key</b> (via setWinKey) is a direct key that can be used
 * by the Windows HMAC implementation.
 *
 * A <b>byte</b> key (via setKey) is a string of bytes that will be used 
 * as a key.  This requires an internal implementation of an HMAC using the
 * Windows Digest functions, as the Windows API does not allow direct
 * loading of these keys.
 */

class DSIG_EXPORT WinCAPICryptoKeyHMAC : public XSECCryptoKeyHMAC {

public :

	/** @name Constructors and Destructors */
	//@{
	/**
	 * \brief Constructor
	 *
	 * @param prov The handle to the provider context that was used to
	 * create any Windows keys (later set via setKey).  If this is not
	 * to be used for a windows key (i.e. will be used for a "normal"
	 * buffer of bytes as a key, then this value can be set to 0
	 */

	WinCAPICryptoKeyHMAC(HCRYPTPROV prov);
	
	virtual ~WinCAPICryptoKeyHMAC() {};

	//@}

	/** @name Key Interface methods */
	//@{

	/**
	 * \brief Return the type of this key.
	 *
	 * For DSA keys, this allows people to determine whether this is a 
	 * public key, private key or a key pair
	 */

	virtual XSECCryptoKey::KeyType getKeyType() const {return KEY_HMAC;}
	
	/**
	 * \brief Replicate key
	 */

	virtual XSECCryptoKey * clone() const ;

	/**
	 * \brief Return the WinCAPI string identifier
	 */

	virtual const XMLCh * getProviderName() const {return DSIGConstants::s_unicodeStrPROVWinCAPI;}

	//@}

	/** @name Optional Interface methods */
	//@{

	/**
	 * \brief Set the key
	 *
	 * Set the key from the buffer
	 *
	 * @param inBuf Buffer containing the direct bitwise representation of the key
	 * @param inLength Number of bytes of key in the buffer
	 *
	 * @note isSensitive() should have been called on the inbound buffer
	 * to ensure the contents is overwritten when the safeBuffer is deleted
	 */

	virtual void setKey(unsigned char * inBuf, unsigned int inLength);

	/**
	 * \brief Get the key value
	 * 
	 * Copy the key into the safeBuffer and return the number of bytes
	 * copied.
	 *
	 * @param outBuf Buffer to copy key into
	 * @returns number of bytes copied in
	 */

	virtual unsigned int getKey(safeBuffer &outBuf) const;

	//@}

	/** @name Windows specific keys */
	//@{

	/**
	 * \brief Set a Windows key
	 *
	 * Set a Windows Crypto key that has been either derived via the
	 * various Crypt functions or has been loaded from an encrypted BLOB
	 *
	 * @param k Windows CAPI key to load
	 * Note that the library now owns this key (and will destroy it).
	 */

	void setWinKey(HCRYPTKEY k);

	/**
	 * \brief Get a windows key
	 *
	 * Used by WinCAPICryptoHashHMAC to retrieve the key in order to
	 * load it into the HMAC function.
	 *
	 * @returns The key to use or 0 if this object does not hold one
	 */

	HCRYPTKEY getWinKey(void) const;

	/**
	 * \brief Get a windows key provider
	 *
	 * Used by WinCAPICryptoHashHMAC to retrieve the provider handle associated
	 * with an HMAC key in order to load it into the HMAC function.
	 *
	 * @returns The key to use or 0 if this object does not hold one
	 */

	HCRYPTPROV getWinKeyProv(void) const;

	//@}

private:

	safeBuffer			m_keyBuf;
	unsigned int		m_keyLen;

	HCRYPTKEY			m_k;
	HCRYPTPROV			m_p;
};

#endif /* XSEC_HAVE_WINCAPI */
#endif /* WINCAPICRYPTOKEYHMAC_INCLUDE */
