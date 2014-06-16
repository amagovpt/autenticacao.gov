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
 * XSECCryptoKeyHMAC := HMAC Keys
 *
 * Author(s): Berin Lautenbach
 *
 * $Id: OpenSSLCryptoKeyHMAC.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef OPENSSLCRYPTOKEYHMAC_INCLUDE
#define OPENSSLCRYPTOKEYHMAC_INCLUDE

#include <xsec/enc/XSECCryptoKeyHMAC.hpp>

#if defined (XSEC_HAVE_OPENSSL)

/**
 * \ingroup opensslcrypto
 */

/**
 * \brief OpenSSL implementation for HMAC keys.
 *
 * Used to provide HMAC keys to OpenSSLCryptoHashHMAC
 */

class DSIG_EXPORT OpenSSLCryptoKeyHMAC : public XSECCryptoKeyHMAC {

public :

	/** @name Constructors and Destructors */
	//@{

	OpenSSLCryptoKeyHMAC();
	virtual ~OpenSSLCryptoKeyHMAC() {};

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

	virtual XSECCryptoKey * clone() const;

	/**
	 * \brief Return the OpenSSL string identifier
	 */

	virtual const XMLCh * getProviderName() const {return DSIGConstants::s_unicodeStrPROVOpenSSL;}
	
	//@}

	/** @name Optional Interface methods
	 * 
	 * These functions do not necessarily have to be implmented.  They
	 * are used by XSECKeyInfoResolverDefault to try to create a key from
	 * KeyInfo elements without knowing anything else.
	 *
	 * If an interface class does not implement these functions, a simple
	 * stub that does nothing should be used.
	 */
	//@{

	/**
	 * \brief Set the key
	 *
	 * Set the key from the buffer
	 *
	 * @param inBuf Buffer containing the key
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

private:

	safeBuffer			m_keyBuf;
	unsigned int		m_keyLen;
};

#endif /* XSEC_HAVE_OPENSSL */

#endif /* OPENSSLCRYPTOKEYHMAC_INCLUDE */
