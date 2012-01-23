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
 * XSECCryptoProvider := Base virtual class to define a crpyto module
 *
 * Author(s): Berin Lautenbach
 *
 * $Id: XSECCryptoProvider.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef XSECCRYPTOPROVIDER_INCLUDE
#define XSECCRYPTOPROVIDER_INCLUDE

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/enc/XSECCryptoHash.hpp>
#include <xsec/enc/XSECCryptoKeyHMAC.hpp>
#include <xsec/enc/XSECCryptoBase64.hpp>
#include <xsec/enc/XSECCryptoX509.hpp>
#include <xsec/enc/XSECCryptoKeyDSA.hpp>
#include <xsec/enc/XSECCryptoKeyEC.hpp>
#include <xsec/enc/XSECCryptoKeyRSA.hpp>
#include <xsec/enc/XSECCryptoSymmetricKey.hpp>

/**
 * @defgroup crypto Cryptographic Abstraction Layer
 * <p>The interface layer between the cryptographic modules and the XML 
 * Security library.  It has been created to allow users to easily 
 * integrate other cryptographic libraries into the XML-Security 
 * library.</p>
 *
 * <p>The XML-Security-C library itself makes calls to this interface 
 * layer to perform all cryptographic procedures.  In order to 
 * instantiate the correct object (i.e. the object
 * that belongs to the correct crypto library), XSEC makes calls to 
 * the virtual class XSECCryptoProvider, which returns pointers to 
 * particular virtual class objects.</p>
 *
 * <p>The cryptographic interface has been kept as thin as possible.
 * The classes are not meant to provide a complete
 * wrapper for the cryptographic libraries involved.  The application
 * program is expected to deal directly with the chosen library.  This
 * ensures that the xml-security library can perform the functions it
 * needs to, but does not constrain the application in what it can do.</p>
 *
 * <p>Therefore, three type of methods are available on any cryptographic
 * class.</p>
 *
 * <ul>
 * <li><em>Required</em> methods are those absolutely necessary for
 * the library to operate.  For example, these include the methods 
 * necessary for the library to decode a base64 encoded signature 
 * and validate it against a defined key.</li>
 * <li><em>Optional</em> methods are used by the ancillary classes
 * in the library.  For example, the default KeyInfoResolver can
 * use an optional method to extract a public key from a certificate.
 * This is not strictly necessary, as the calling application could
 * provide a resolver that does this work directly rather than using
 * the XSECCryptoX509 class.</li>
 * <li><em>Library Specific</em> methods are those methods that are
 * unique to a particular library.  For example, the OpenSSLCryptoX509
 * class has a Library Specific constructor that takes an OpenSSL
 * X509 structure as its argument.</li>
 * </ul>
 *
 * <p>Unless marked otherwise, all methods defined in the XSECCrypto*
 * classes are <em>Required</em>.</p>
 *
 * <p>The particular instantiation of XSECCryptoProvider that is to 
 * be used is set via the XSECPlatformUtils#Initialise() function 
 * call.  If no provider is passed in, the Initialise function 
 * generates an OpenSSLCryptoProvider class for use.  If OpenSSL
 * is not available under windows, the library will use the Windows
 * CAPI instead.</p>
 *
 * <p>The provider is kept in a global variable, and is used by 
 * all signature objects created by a particular application.  At 
 * this time there is no way to have different signature
 * objects use different CryptoProviders</p>
 *
 * @todo Add an ability to better handle "optional" functions.  The library 
 * should make a call to the
 * provider to see whether an optional function (e.g. extract key from 
 * X509) has been
 * provided.
 *
 *
 *\@{*/

// Some constants

/**
 *\brief Maximum length (in bytes) of any expected Digest results.
 *
 * This constant defines the maximum length (in bytes) of HASH returns.
 * We have defined it far and above the size we would normally expect.
 *
 * @todo This should really come from each of the providers and should
 * be implemented as a function call to *CryptoProvider
 */

#define CRYPTO_MAX_HASH_SIZE		128

/**
 *\brief OID For DSA
 */

#define CRYPTO_OID_DSA				"1.2.840.10040.4.1"

/**
 *\brief The base class that all *CryptoProviders need to implement.
 *
 * The instatiations of this class are used by the core library to
 * create cryptographic objects necessary for the library to do its work
 * without actually knowing any details at all about the provider library
 *
 * @note Subject to change
 * @see OpenSSLCryptoProvider
 */

class DSIG_EXPORT XSECCryptoProvider {


public :

	/** @name Constructors and Destructors */
	//@{
	
	XSECCryptoProvider() {};
	virtual ~XSECCryptoProvider() {};
	//@}


	/** @name Hashing (Digest) Functions */
	//@{

	/**
	 * \brief Return a SHA1 implementation.
	 *
	 * Call used by the library to obtain a SHA1 object from the 
	 * provider.
	 *
	 * @returns A pointer to a Hash object that implements SHA1
	 * @see XSECCryptoHash
	 * @deprecated Use hashSHA instead
	 */
	 
	virtual XSECCryptoHash			* hashSHA1() const = 0;

	/**
	 * \brief Return a SHA implementation.
	 *
	 * Call used by the library to obtain a SHA object from the 
	 * provider.  Size of hash determined by length argument (160 = SHA1)
	 *
	 * @returns A pointer to a Hash object that implements SHA1
	 * @param length - length of hash.  E.g. 160 for SHA1 or 256 for SHA256
	 * @see XSECCryptoHash
	 */
	 
	virtual XSECCryptoHash			* hashSHA(int length = 160) const = 0;

	/**
	 * \brief Return a HMAC SHA1 implementation.
	 *
	 * Call used by the library to obtain a HMAC SHA1 object from the 
	 * provider.  The caller will need to set the key in the hash
	 * object with an XSECCryptoKeyHMAC using XSECCryptoHash::setKey()
	 *
	 * @returns A pointer to a Hash object that implements HMAC-SHA1
	 * @see XSECCryptoHash
	 * @deprecated use hashHMACSHA instead
	 */

	virtual XSECCryptoHash			* hashHMACSHA1() const = 0;

	/**
	 * \brief Return a HMAC SHA(1-512) implementation.
	 *
	 * Call used by the library to obtain a HMAC SHA object from the 
	 * provider.  The caller will need to set the key in the hash
	 * object with an XSECCryptoKeyHMAC using XSECCryptoHash::setKey()
	 *
	 * @returns A pointer to a Hash object that implements HMAC-SHA1
	 * @param length Length of hash output (160 = SHA1, 256, 512 etc)
	 * @see XSECCryptoHash
	 */

	virtual XSECCryptoHash			* hashHMACSHA(int length = 160) const = 0;

	/**
	 * \brief Return a MD5 implementation.
	 *
	 * Call used by the library to obtain a MD5 object from the 
	 * provider.
	 *
	 * @returns A pointer to a Hash object that implements MD5
	 * @see XSECCryptoHash
	 */
	 
	virtual XSECCryptoHash			* hashMD5() const = 0;

	/**
	 * \brief Return a HMAC MD5 implementation.
	 *
	 * Call used by the library to obtain a HMAC MD5 object from the 
	 * provider.  The caller will need to set the key in the hash
	 * object with an XSECCryptoKeyHMAC using XSECCryptoHash::setKey()
	 *
	 * @note The use of MD5 is explicitly marked as <b>not recommended</b> 
	 * in the XML Digital Signature standard due to recent advances in
	 * cryptography indicating there <em>may</em> be weaknesses in the 
	 * algorithm.
	 *
	 * @returns A pointer to a Hash object that implements HMAC-MD5
	 * @see XSECCryptoHash
	 */

	virtual XSECCryptoHash			* hashHMACMD5() const = 0;

	/**
	 * \brief Return a HMAC key
	 *
	 * Sometimes the library needs to create an HMAC key (notably within
	 * the XKMS utilities.
	 *
	 * This function allows the library to obtain a key that can then have
	 * a value set within it.
	 */

	virtual XSECCryptoKeyHMAC		* keyHMAC() const = 0;

	//@}

	/** @name Encoding functions */
	//@{

	/**
	 * \brief Return a Base64 encoder/decoder implementation.
	 *
	 * Call used by the library to obtain a Base64 encoder/decoder.
	 *
	 * @returns Pointer to the new Base64 encoder.
	 * @see XSECCryptoBase64
	 */

	virtual XSECCryptoBase64		* base64() const = 0;

	//@}

	/** @name Keys and Certificates */
	//@{

	/**
	 * \brief Return a DSA key implementation object.
	 * 
	 * Call used by the library to obtain a DSA key object.
	 *
	 * @returns Pointer to the new DSA key
	 * @see XSECCryptoKeyDSA
	 */

	virtual XSECCryptoKeyDSA		* keyDSA() const = 0;

	/**
	 * \brief Return an RSA key implementation object.
	 * 
	 * Call used by the library to obtain an RSA key object.
	 *
	 * @returns Pointer to the new RSA key
	 * @see XSECCryptoKeyRSA
	 */

	virtual XSECCryptoKeyRSA		* keyRSA() const = 0;

	/**
	 * \brief Return an EC key implementation object.
	 * 
	 * Call used by the library to obtain an EC key object.
	 *
	 * @returns Pointer to the new EC key
	 * @see XSECCryptoKeyEC
	 */

	virtual XSECCryptoKeyEC         * keyEC() const;

	/**
	 * \brief Return a key implementation object based on DER-encoded input.
	 * 
	 * Call used by the library to obtain a key object from a DER-encoded key.
	 *
     * @param buf       DER-encoded data
     * @param buflen    length of data
     * @param base64    true iff data is base64-encoded
	 * @returns Pointer to the new key
	 * @see XSECCryptoKey
	 */

	virtual XSECCryptoKey         * keyDER(const char* buf, unsigned long buflen, bool base64) const;

    /**
	 * \brief Return an X509 implementation object.
	 * 
	 * Call used by the library to obtain an object that can work
	 * with X509 certificates.
	 *
	 * @returns Pointer to the new X509 object
	 * @see XSECCryptoX509
	 */

	virtual XSECCryptoX509			* X509() const = 0;

	/**
	 * \brief Determine whether a given algorithm is supported
	 *
	 * A call that can be used to determine whether a given 
	 * symmetric algorithm is supported
	 */

	virtual bool algorithmSupported(XSECCryptoSymmetricKey::SymmetricKeyType alg) const = 0;

	/**
	 * \brief Determine whether a given algorithm is supported
	 *
	 * A call that can be used to determine whether a given 
	 * digest algorithm is supported
	 */

	virtual bool algorithmSupported(XSECCryptoHash::HashType alg) const = 0;
	
	/**
	 * \brief Return a Symmetric Key implementation object.
	 *
	 * Call used by the library to obtain a bulk encryption
	 * object.
	 *
	 * @returns Pointer to the new SymmetricKey object
	 * @see XSECCryptoSymmetricKey
	 */

	virtual XSECCryptoSymmetricKey	* keySymmetric(XSECCryptoSymmetricKey::SymmetricKeyType alg) const = 0;

	/**
	 * \brief Obtain some random octets
	 *
	 * For generation of IVs and the like, the library needs to be able
	 * to obtain "random" octets.  The library uses this call to the 
	 * crypto provider to obtain what it needs.
	 *
	 * @param buffer The buffer to place the random data in
	 * @param numOctets Number of bytes required
	 * @returns Number of bytes obtained.
	 */

	virtual unsigned int getRandom(unsigned char * buffer, unsigned int numOctets) const = 0;

	//@}

	/** @name Information Functions */
	//@{

	/**
	 * \brief Returns a string that identifies the Crypto Provider
	 */

	virtual const XMLCh * getProviderName() const = 0;

	//@}

	/*\@}*/
};


#endif /* XSECCRYPTOPROVIDER_INCLUDE */
