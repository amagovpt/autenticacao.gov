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
 * WinCAPICryptoProvider := Base class to handle Windows Crypto API
 *
 * Author(s): Berin Lautenbach
 *
 * $Id: WinCAPICryptoProvider.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef WINCAPICRYPTOPROVIDER_INCLUDE
#define WINCAPICRYPTOPROVIDER_INCLUDE

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/enc/XSECCryptoProvider.hpp>

#if defined (XSEC_HAVE_WINCAPI)

#if defined (_WIN32_WINNT)
#	undef _WIN32_WINNT
#endif
#define _WIN32_WINNT 0x0400
#include <wincrypt.h>


// For older versions of wincrypt.h

#if !defined (PROV_RSA_AES)
#	define PROV_RSA_AES      24
#	define ALG_SID_AES_128   14
#	define ALG_SID_AES_192   15
#	define ALG_SID_AES_256   16
#	define ALG_SID_AES       17
#	define CALG_AES_128      (ALG_CLASS_DATA_ENCRYPT|ALG_TYPE_BLOCK|ALG_SID_AES_128)
#	define CALG_AES_192      (ALG_CLASS_DATA_ENCRYPT|ALG_TYPE_BLOCK|ALG_SID_AES_192)
#	define CALG_AES_256      (ALG_CLASS_DATA_ENCRYPT|ALG_TYPE_BLOCK|ALG_SID_AES_256)
#endif

#define WINCAPI_BLOBHEADERLEN   0x08
#define WINCAPI_DSSPUBKEYLEN    0x08
#define WINCAPI_DSSSEEDLEN      0x18
#define WINCAPI_RSAPUBKEYLEN    0x0C

/**
 * @defgroup wincapicrypto Windows Crypto API Interface
 * @ingroup crypto
 * The WinCAPI crypto provides an experimental inerface to
 * the Windows Cryptographic API.
 *
 * All initialisation of the Windows providers needs to be done
 * by the calling application.  The interface will call the provided
 * DSS (PROV_DSS) provider and RSA (PROV_RSA_FULL) provider to perform
 * cryptographic functions.
 *
 * The tools use the default providers, but the calling application
 * can use any providers that implement PROV_DSS and PROV_FULL_RSA.
 *
 * Note that, unlike the OpenSSL classes, the various implementation 
 * classes all require their owner provider class to be passed into
 * the constructor.  This allows them to access the RSA and DSS CAPI
 * providers being used for the implementation.
 *
 * @todo Need to allow the various classes to over-ride the PROV
 * objects to allow specific private key instances rather than one
 * instance across the library instance.
 */
 /*\@{*/

class DSIG_EXPORT WinCAPICryptoProvider : public XSECCryptoProvider {


public :

	/** @name Constructors and Destructors */
	//@{
	/**
	 * \brief Create a Windows CAPI interface layer
	 *
	 * Windows CSPs work under a provider model.  The user should specify
	 * which CSP to use.
	 *
	 * @param provDSSName Name of DSS provider - must be of type PROV_DSS.
	 * Will use the default Windows DSS provider if nothing passed in.
	 * @param provRSAName RSA provider - must be of type PROV_RSA_FULL.
	 * Will use the default RSA_FULL provider if nothing passed in
         * @param dwFlags If you are running XSEC as service you should specify
         * CRYPT_MACHINE_KEYSET here
	 */

	WinCAPICryptoProvider(LPCSTR provDSSName = NULL, LPCSTR provRSAName = NULL, DWORD dwFlags = 0);

	virtual ~WinCAPICryptoProvider();

	//@}

	/** @name Hashing (Digest) Functions */
	//@{

	/**
	 * \brief Return a SHA1 implementation.
	 *
	 * Call used by the library to obtain a SHA1 object from the 
	 * provider.
	 *
	 * @returns A pointer to an WinCAPI Hash object that implements SHA1
	 * @see WinCAPICryptoHash
	 */

	virtual XSECCryptoHash			* hashSHA1() const;

	/**
	 * \brief Return a SHA implementation.
	 *
	 * Call used by the library to obtain a SHA object from the 
	 * provider.  Size of hash determined by length argument (160 = SHA1)
	 *
	 * @returns A pointer to a Hash object that implements SHA1
	 * @param length - length of hash.  E.g. 160 for SHA1 or 256 for SHA256
	 * @see WinCAPICryptoHash
	 */
	 
	virtual XSECCryptoHash			* hashSHA(int length = 160) const;
	
	/**
	 * \brief Return a HMAC SHA1 implementation.
	 *
	 * Call used by the library to obtain a HMAC SHA1 object from the 
	 * provider.  The caller will need to set the key in the hash
	 * object with an XSECCryptoKeyHMAC using WinCAPICryptoHash::setKey()
	 *
	 * @returns A pointer to a Hash object that implements HMAC-SHA1
	 * @see WinCAPICryptoHash
	 */

	virtual XSECCryptoHash			* hashHMACSHA1() const;

	/**
	 * \brief Return a HMAC SHA(1-512) implementation.
	 *
	 * Call used by the library to obtain a HMAC SHA object from the 
	 * provider.  The caller will need to set the key in the hash
	 * object with an XSECCryptoKeyHMAC using XSECCryptoHash::setKey()
	 *
	 * @returns A pointer to a Hash object that implements HMAC-SHA1
	 * @param length Length of hash output (160 = SHA1, 256, 512 etc)
	 * @see WinCAPICryptoHash
	 */

	virtual XSECCryptoHash			* hashHMACSHA(int length = 160) const;
	
	/**
	 * \brief Return a MD5 implementation.
	 *
	 * Call used by the library to obtain a MD5 object from the 
	 * WiNCAPI provider.
	 *
	 * @returns A pointer to a Hash object that implements MD5
	 * @see WinCAPICryptoHash
	 */

	virtual XSECCryptoHash			* hashMD5() const;

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
	 * @see WinCAPICryptoHash
	 */

	virtual XSECCryptoHash			* hashHMACMD5() const;

	/**
	 * \brief Return a HMAC key
	 *
	 * Sometimes the library needs to create an HMAC key (notably within
	 * the XKMS utilities.
	 *
	 * This function allows the library to obtain a key that can then have
	 * a value set within it.
	 */

	virtual XSECCryptoKeyHMAC		* keyHMAC(void) const;

	//@}

	/** @name Encoding functions */
	//@{

	/**
	 * \brief Return a Base64 encoder/decoder implementation.
	 *
	 * Call used by the library to obtain a Base64 
	 * encoder/decoder.
	 *
	 * @note Windows providers do not implement Base64, so the internal
	 * implementation (XSCrypt) is used instead.
	 * 
	 *
	 * @returns Pointer to the new Base64 encoder.
	 * @see XSCryptCryptoBase64
	 */

	virtual XSECCryptoBase64		* base64() const;

	//@}

	/** @name Keys and Certificates */
	//@{

	/**
	 * \brief Return a DSA key implementation object.
	 * 
	 * Call used by the library to obtain a DSA key object.
	 *
	 * @returns Pointer to the new DSA key
	 * @see WinCAPICryptoKeyDSA
	 */

	virtual XSECCryptoKeyDSA		* keyDSA() const;

	/**
	 * \brief Return an RSA key implementation object.
	 * 
	 * Call used by the library to obtain an WinCAPI RSA key object.
	 *
	 * @returns Pointer to the new RSA key
	 * @see WinCAPICryptoKeyRSA
	 */

	virtual XSECCryptoKeyRSA		* keyRSA() const;

	/**
	 * \brief Return an X509 implementation object.
	 * 
	 * Call used by the library to obtain an object that can work
	 * with X509 certificates.
	 *
	 * @returns Pointer to the new X509 object
	 * @see WinCAPICryptoX509
	 */

	virtual XSECCryptoX509			* X509() const;

	//@}

	/** @name Windows CAPI Specific methods */
	//@{

	/**
	 * \brief Returns the Crypto Provider being used for DSS
	 */

	HCRYPTPROV getProviderDSS(void) {return m_provDSS;}

	/**
	 * \brief Returns the Provider being used for RSA functions
	 */

	HCRYPTPROV getProviderRSA(void) {return m_provRSA;}

	/**
	 * \brief Return the internal key store provider
	 */

	HCRYPTPROV getApacheKeyStore(void) {return m_provApacheKeyStore;}

	/**
	 * \brief Translate B64 I2OS integer to a WinCAPI int.
	 *
	 * Decodes a Base64 (ds:CryptoBinary) integer and reverses the order to 
	 * allow loading into a Windows CAPI function.  (CAPI uses Little Endian 
	 * storage of integers).
	 *
	 * @param b64 Base 64 string
	 * @param b64Len Length of base64 string
	 * @param retLen Parameter to hold length of return integer
	 */

	static BYTE * b642WinBN(const char * b64, unsigned int b64Len, unsigned int &retLen);

	/**
	 * \brief Translate a WinCAPI int to a B64 I2OS integer .
	 *
	 * Encodes a Windows integer in I2OSP base64 encoded format.
	 *
	 * @param n Buffer holding the Windows Integer
	 * @param nLen Length of data in buffer
	 * @param retLen Parameter to hold length of return integer
	 * @returns A pointer to a buffer holding the encoded data 
	 * (transfers ownership)
	 */

	static unsigned char * WinBN2b64(BYTE * n, DWORD nLen, unsigned int &retLen);

	/**
	 * \brief Determine whether a given algorithm is supported
	 *
	 * A call that can be used to determine whether a given 
	 * symmetric algorithm is supported
	 */

	virtual bool algorithmSupported(XSECCryptoSymmetricKey::SymmetricKeyType alg) const;

	/**
	 * \brief Determine whether a given algorithm is supported
	 *
	 * A call that can be used to determine whether a given 
	 * digest algorithm is supported
	 */

	virtual bool algorithmSupported(XSECCryptoHash::HashType alg) const;
	
	/**
	 * \brief Return a Symmetric Key implementation object.
	 *
	 * Call used by the library to obtain a bulk encryption
	 * object.
	 *
	 * @returns Pointer to the new SymmetricKey object
	 * @see XSECCryptoSymmetricKey
	 */

	virtual XSECCryptoSymmetricKey	* keySymmetric(XSECCryptoSymmetricKey::SymmetricKeyType alg) const;

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

	virtual unsigned int getRandom(unsigned char * buffer, unsigned int numOctets) const;


	//@}

	/** @name Information Functions */
	//@{

	/**
	 * \brief Returns a string that identifies the Crypto Provider
	 */

	virtual const XMLCh * getProviderName() const;

	//@}


private:

	HCRYPTPROV		m_provDSS;
	HCRYPTPROV		m_provRSA;
	HCRYPTPROV		m_provApacheKeyStore;
	LPCSTR			m_provDSSName;
	LPCSTR			m_provRSAName;
	bool			m_haveAES;
	DWORD			m_provRSAType;

};

/*\@}*/

#endif /* XSEC_HAVE_WINCAPI */
#endif /* WINCAPICRYPTOPROVIDER_INCLUDE */

