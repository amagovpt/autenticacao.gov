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
 * OpenSSLCryptoX509:= OpenSSL based class for handling X509 (V3) certificates
 *
 * Author(s): Berin Lautenbach
 *
 * $Id: OpenSSLCryptoX509.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef OPENSSLCRYPTOX509_INCLUDE
#define OPENSSLCRYPTOX509_INCLUDE

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/enc/XSECCryptoX509.hpp>

#if defined (XSEC_HAVE_OPENSSL)
#include <openssl/x509.h>
#include <openssl/bio.h>

/**
 * \brief Implementation class for interface for X509 certificates.
 * @ingroup opensslcrypto
 *
 * The library uses classes derived from this to process X509 Certificates.
 *
 */

class DSIG_EXPORT OpenSSLCryptoX509 : public XSECCryptoX509 {

public :

	/** @name Constructors and Destructors */
	//@{

	OpenSSLCryptoX509();
	virtual ~OpenSSLCryptoX509();

	//@}


	//@}
	/** @name Key Interface methods */
	//@{

	/**
	 * \brief Return the type of the key stored in the certificate.
	 *
	 * Will extract the key from the certificate to return the appropriate
	 * type
	 *
	 */

	virtual XSECCryptoKey::KeyType getPublicKeyType() const;

	/**
	 * \brief Returns a string that identifies the crypto owner of this library.
	 */

	virtual const XMLCh * getProviderName() const;

	/**
	 * \brief Get a copy of the public key.
	 *
	 * Extracts the public key from the certificate and returns the appropriate
	 * OpenSSLCryrptoKey (DSA or RSA) object
	 *
	 */

	virtual XSECCryptoKey * clonePublicKey() const;

	//@}

	/** @name Load and Get the certificate */
	//@{

	/**
	 * \brief Load a certificate into the object.
	 *
	 * Take a base64 DER encoded certificate and load.
	 *
	 * @param buf A buffer containing the Base64 encoded certificate
	 * @param len The number of bytes of data in the certificate.
	 */

	virtual void loadX509Base64Bin(const char * buf, unsigned int len);

	/**
	 * \brief Get a Base64 DER encoded copy of the certificate
	 *
	 * @returns A safeBuffer containing the DER encoded certificate
	 */

	virtual safeBuffer &getDEREncodingSB(void) {return m_DERX509;}

    /**
	 * \brief Get a Base64 DER encoded copy of the certificate
	 *
	 * @returns A safeBuffer containing the DER encoded certificate
	 */

	virtual const safeBuffer &getDEREncodingSB(void) const {return m_DERX509;}

	//@}

	/** @name OpenSSL Library Specific functions */
	//@{

	/**
	 * \brief OpenSSL specific constructor 
	 *
	 * Construct the object around an existing X509 certificate
	 */

	OpenSSLCryptoX509(X509 * x);

	/**
	 * \brief Get OpenSSL certificate structure
	 */

	X509 * getOpenSSLX509(void) {return mp_X509;}

    /**
	 * \brief Get OpenSSL certificate structure
	 */

	const X509 * getOpenSSLX509(void) const {return mp_X509;}

	//@}

private:

	X509			* mp_X509;				// The X509 structure
	safeBuffer		m_DERX509;
};

#endif /* XSEC_HAVE_OPENSSL */
#endif /* OPENSSLCRYPTOX509_INCLUDE */

