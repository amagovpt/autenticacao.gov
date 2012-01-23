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
 * XKMSRSAKeyPair := Interface for RSAKeyPair elements
 *
 * $Id:$
 *
 */

#ifndef XKMSRSAKEYPAIR_INCLUDE
#define XKMSRSAKEYPAIR_INCLUDE

// XSEC Includes

#include <xsec/framework/XSECDefs.hpp>

/**
 * @ingroup xkms
 */

/**
 * @brief Interface definition for the RSAKeyPair
 *
 * The \<RSAKeyPair\> is one of the few XKMS elements that is not
 * directly part of an XKMS Message.  It is sent in RegisterResult
 * and RecoverResult messages as a CipherData.
 *
 * This element allows the service to provide back to the user
 * a complete RSA key - including the private components.  The various
 * components that make up an RSA key are all provided in Base64 format
 * (CryptoBinary type)
 *
 * The schema definition for RSAKeyPair is as follows :
 *
 * \verbatim
   <!-- RSAKeyPair -->
   <element name="RSAKeyPair" type="xkms:RSAKeyPairType"/>
   <complexType name="RSAKeyPairType">
      <sequence>
         <element ref="xkms:Modulus"/>
         <element ref="xkms:Exponent"/>
         <element ref="xkms:P"/>
         <element ref="xkms:Q"/>
         <element ref="xkms:DP"/>
         <element ref="xkms:DQ"/>
         <element ref="xkms:InverseQ"/>
         <element ref="xkms:D"/>
      </sequence>
   </complexType>
   <element name="Modulus" type="ds:CryptoBinary"/>
   <element name="Exponent" type="ds:CryptoBinary"/>
   <element name="P" type="ds:CryptoBinary"/>
   <element name="Q" type="ds:CryptoBinary"/>
   <element name="DP" type="ds:CryptoBinary"/>
   <element name="DQ" type="ds:CryptoBinary"/>
   <element name="InverseQ" type="ds:CryptoBinary"/>
   <element name="D" type="ds:CryptoBinary"/>
   <!-- /RSAKeyPair -->
\endverbatim
 *
 * @note There are no "set" methods for this class.  It is designed
 * to be created "atomically" via the XKMSMessageFactory class and not
 * altered.
 */


class XKMSRSAKeyPair {

public:

	/** @name Constructors and Destructors */
	//@{

protected:

	XKMSRSAKeyPair() {};

public:

	virtual ~XKMSRSAKeyPair() {};

	/** @name Getter Interface Methods */
	//@{

	/**
	 * \brief Get the Modulus of the key pair
	 *
	 * @returns The Modulus in base64 encoding
	 */

	virtual const XMLCh * getModulus(void) const = 0;

	/**
	 * \brief Get the Exponent of the key pair
	 *
	 * @returns The Exponent in base64 encoding
	 */

	virtual const XMLCh * getExponent(void) const = 0;

	/**
	 * \brief Get the P of the key pair
	 *
	 * @returns The P in base64 encoding
	 */

	virtual const XMLCh * getP(void) const = 0;

	/**
	 * \brief Get the Q of the key pair
	 *
	 * @returns The Q in base64 encoding
	 */

	virtual const XMLCh * getQ(void) const = 0;

	/**
	 * \brief Get the DP of the key pair
	 *
	 * @returns The DP in base64 encoding
	 */

	virtual const XMLCh * getDP(void) const = 0;

	/**
	 * \brief Get the DQ of the key pair
	 *
	 * @returns The DQ in base64 encoding
	 */

	virtual const XMLCh * getDQ(void) const = 0;

	/**
	 * \brief Get the InverseQ of the key pair
	 *
	 * @returns The InverseQ in base64 encoding
	 */

	virtual const XMLCh * getInverseQ(void) const = 0;

	/**
	 * \brief Get the D of the key pair
	 *
	 * @returns The D in base64 encoding
	 */

	virtual const XMLCh * getD(void) const = 0;


	//@}

private:

	// Unimplemented
	XKMSRSAKeyPair(const XKMSRSAKeyPair &);
	XKMSRSAKeyPair & operator = (const XKMSRSAKeyPair &);

};

#endif /* XKMSRSAKEYPAIR_INCLUDE */
