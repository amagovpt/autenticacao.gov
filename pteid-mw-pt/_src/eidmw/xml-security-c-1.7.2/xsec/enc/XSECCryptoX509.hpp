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
 * XSECCryptoX509:= A base class for handling X509 (V3) certificates
 *
 * Author(s): Berin Lautenbach
 *
 * $Id: XSECCryptoX509.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef XSECCRYPTOX509_INCLUDE
#define XSECCRYPTOX509_INCLUDE

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/enc/XSECCryptoKey.hpp>
#include <xsec/utils/XSECSafeBuffer.hpp>

/**
 * \brief Interface class for X509 certificates.
 * @ingroup crypto
 *
 * The library uses classes derived from this to process X509 Certificates.
 *
 * Strictly speaking, this class is not required (and is completely 
 * <em>Optional</em>.  However it is used by
 * XSECKeyInfoResolverDefault to extract a key from a certificate in cases
 * where the caller is not worried about the trust level of the certificate.
 *
 */


class DSIG_EXPORT XSECCryptoX509 {

public :

	/** @name Constructors and Destructors */
	//@{

	XSECCryptoX509() {};
	virtual ~XSECCryptoX509() {};

	//@}
	/** @name Key Interface methods */
	//@{

	/**
	 * \brief Return the type of the key stored in the certificate.
	 *
	 * The implementation is expected to extract the key from the 
	 * certificate and determine the type.
	 *
	 */

	virtual XSECCryptoKey::KeyType getPublicKeyType() const = 0;

	/**
	 * \brief Get a copy of the public key.
	 *
	 * The implementation should extract the key from the certificate,
	 * create an instance of the appropriate key type, and return it.
	 *
	 */

	virtual XSECCryptoKey * clonePublicKey() const = 0;

	/**
	 * \brief Returns a string that identifies the crypto owner of this library.
	 */

	virtual const XMLCh * getProviderName() const = 0;

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

	virtual void loadX509Base64Bin(const char * buf, unsigned int len) = 0;

	/**
	 * \brief Load a PEM encoded certificate into the object.
	 *
	 * Take a PEM encoded certificate and load.
	 *
	 * @param buf A buffer containing the PEM encoded certificate
	 * @param len The number of bytes of data in the certificate.
	 * (0 if the string is null terminated.)
	 */

	void loadX509PEM(const char * buf, unsigned int len = 0);

	/**
	 * \brief Get a Base64 DER encoded copy of the certificate
	 *
	 * @returns A safeBuffer containing the DER encoded certificate
	 */

	virtual safeBuffer &getDEREncodingSB(void) = 0;		// Get the DER string

    /**
	 * \brief Get a Base64 DER encoded copy of the certificate
	 *
	 * @returns A safeBuffer containing the DER encoded certificate
	 */

	virtual const safeBuffer &getDEREncodingSB(void) const = 0;		// Get the DER string

	//@}

};


#endif /* XSECCRYPTOX509_INCLUDE */


