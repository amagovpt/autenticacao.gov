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
 * $Id: XSECCryptoKeyHMAC.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef XSECCRYPTOKEYHMAC_INCLUDE
#define XSECCRYPTOKEYHMAC_INCLUDE

#include <xsec/enc/XSECCryptoKey.hpp>
#include <xsec/utils/XSECSafeBuffer.hpp>

/**
 * \ingroup crypto
 */

/**
 * \brief Interface class for HMAC keys.
 *
 * The library uses classes derived from this to process HMAC keys.
 */

class DSIG_EXPORT XSECCryptoKeyHMAC : public XSECCryptoKey {

public :

	/** @name Constructors and Destructors */
	//@{
	
	XSECCryptoKeyHMAC() {};
	virtual ~XSECCryptoKeyHMAC() {};
	
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
	
	virtual XSECCryptoKey * clone() const = 0;

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
	 */

	virtual void setKey(unsigned char * inBuf, unsigned int inLength) = 0;

	/**
	 * \brief Get the key value
	 * 
	 * Copy the key into the safeBuffer and return the number of bytes
	 * copied.
	 *
	 * @param outBuf Buffer to copy key into
	 * @returns number of bytes copied in
	 */

	virtual unsigned int getKey(safeBuffer &outBuf) const = 0;

	//@}
};


#endif /* XSECCRYPTOKEYHMAC_INCLUDE */
