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
 * DSIGKeyInfo := Base (virtual) class that defines an XSEC KeyInfo node
 *
 * Author(s): Berin Lautenbach
 *
 * $Id: DSIGKeyInfo.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef DSIGKEYINFO_INCLUDE
#define DSIGKEYINFO_INCLUDE

// XSEC Includes

#include <xsec/utils/XSECDOMUtils.hpp>
#include <xsec/utils/XSECSafeBufferFormatter.hpp>
#include <xsec/enc/XSECCryptoKey.hpp>

#include <xercesc/dom/DOM.hpp>

class DSIGSignature;
class XSECEnv;

/**
 * @ingroup pubsig
 */

/**
 * @brief Base class for <Key*> nodes in a KeyInfo list.
 *
 * Digital signatures can have a number of KeyInfo elements that are
 * used to communicate information about what key to use between the
 * signer and the validator.
 *
 * In the XML-Security-C libary, KeyInfo elements are only used for
 * holding information about keys.  They do not in themselves perform
 * any cryptographic function.
 *
 */


class DSIG_EXPORT DSIGKeyInfo {

public:

	/**
	 * \brief List of potential KeyInfo types
	 *
	 * The keyIntoType enumerated type defines the KeyInfo types known by
	 * the XML-Security-C library.
	 *
	 */


	enum keyInfoType {
        KEYINFO_EXTENSION       = 0,            // Extension type unknown to library
		KEYINFO_NOTSET			= 1,			// Empty key type
		KEYINFO_X509			= 2,			// X.509 Certificate-Related Data
		KEYINFO_VALUE_DSA		= 3,			// DSA Key
		KEYINFO_VALUE_RSA		= 4,            // RSA Key
		KEYINFO_NAME			= 5,			// Name of a key (application dependant)
		KEYINFO_PGPDATA			= 6,			// PGP Key
		KEYINFO_SPKIDATA		= 7,            // SPKI Key
		KEYINFO_MGMTDATA		= 8,			// Management data
		KEYINFO_ENCRYPTEDKEY	= 9, 			// XML Encryption - Encrypted Key
        KEYINFO_VALUE_EC        = 10,           // ECC Key
        KEYINFO_DERENCODED      = 11            // DER-Encoded Key
	};

public:

	/** @name Constructors and Destructors */
	//@{

	/**
	 * \brief Construct from an owning signature
	 *
	 * All KeyInfo types take a constructor that provides the controlling environment.
	 *
	 * @param env The environment that the KeyInfo is operating within
	 */

	DSIGKeyInfo(const XSECEnv * env) {mp_keyInfoDOMNode = NULL; mp_env = env;}

	/**
	 * \brief The Destructor
	 */

	virtual ~DSIGKeyInfo() {};

	//@}

	/** @name Get functions */
	//@{

	/**
	 * \brief Return type
	 *
	 * Can be used to find what type of KeyInfo this is
	 */

	virtual keyInfoType getKeyInfoType(void) const = 0;

	/**
	 * \brief Return the DOMNode that heads up this KeyInfo child
	 */

	virtual XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *getKeyInfoDOMNode()
		{return mp_keyInfoDOMNode;}

	/**
	 * \brief Return the name of this key
	 *
	 * For those KeyInfo types that have a keyname, this function should return
	 * it.  For certificates, this may be the DN.
	 *
	 * @returns A pointer to a buffer containing the name
	 */

	virtual const XMLCh * getKeyName(void) const = 0;

	//@}

	/** @name Load and Set */
	//@{

	/**
	 * \brief Load the DOM structures.
	 *
	 * Used by the library to instruct the object to load information from
	 * the DOM nodes
	 */

	virtual void load() = 0;

	//@}

protected:

	XERCES_CPP_NAMESPACE_QUALIFIER DOMNode			* mp_keyInfoDOMNode;
	const XSECEnv									* mp_env;

private:
	DSIGKeyInfo();

};




#endif /* #define XSECKEYINFO_INCLUDE */
