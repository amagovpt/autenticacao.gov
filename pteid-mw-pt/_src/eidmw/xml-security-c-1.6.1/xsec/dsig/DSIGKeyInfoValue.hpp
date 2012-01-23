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
 * DSIGKeyInfoValue := A value setting
 *
 * Author(s): Berin Lautenbach
 *
 * $Id: DSIGKeyInfoValue.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef DSIGKEYINFOVALUE_INCLUDE
#define DSIGKEYINFOVALUE_INCLUDE

#include <xsec/utils/XSECSafeBuffer.hpp>
#include <xsec/dsig/DSIGKeyInfo.hpp>
#include <xsec/utils/XSECPlatformUtils.hpp>

XSEC_DECLARE_XERCES_CLASS(DOMElement);

/**
 * @ingroup pubsig
 */

/**
 * @brief The class for \<KeyValue\> nodes in a KeyInfo list.
 *
 * Class for holding information on a KeyValue node as well as setting
 * such a node in a signature.
 *
 * Two types of Value are understood - RSA and DSA.
 *
 * RSA values are fully implemented as per XML-DSig.  They have two parameters
 *
 * <ul>
 * <li><em>Modulus</em> - holds the modulus of this public key; and</li>
 * <li><em>Exponent</em> - holds the exponent.</li>
 * </ul>
 *
 * DSA values have all mandatory parts implemented  - P, Q, G and Y.
 *
 * J, Seed and PgenCounter are not currently implemented.
 */


class DSIG_EXPORT DSIGKeyInfoValue : public DSIGKeyInfo {

public:

	/** @name Constructors and Destructors */
	//@{

	/**
	 * \brief Constructor for an existing KeyInfo  *
	 *
	 * Constructor used when loading a KeyValue node that already exists
	 * in an XML document.
	 *
	 * @param env Operating environment
	 * @param valueNode DOMNode at head of XML structure
	 */

	DSIGKeyInfoValue(
		const XSECEnv * env, 
		XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *valueNode
	);

	/**
	 * \brief Constructor for creating from scratch
	 *
	 * Constructor used when creating a new KeyValue node to append
	 * to a signature structure.
	 *
	 * @param env Operating environment
	 */

	DSIGKeyInfoValue(const XSECEnv * env);

	virtual ~DSIGKeyInfoValue();

	//@}

	/** @name Loading and Get functions */
	//@{

	/**
	 * \brief Load an existing XML structure
	 *
	 * Called by the Signature class when it is reading in
	 * a DOM structure 
	 */

	virtual void load(void);

	/**
	 * \brief Get P value
	 *
	 * @returns a pointer to the DSA P string value.
	 */

	const XMLCh * getDSAP(void) const {return mp_PTextNode->getNodeValue();}

	/**
	 * \brief Get Q value
	 *
	 * @returns a pointer to the DSA Q string value.
	 */

	const XMLCh * getDSAQ(void) const {return mp_QTextNode->getNodeValue();}

	/**
	 * \brief Get G value
	 *
	 * @returns a pointer to the DSA G string value.
	 */

	const XMLCh * getDSAG(void) const {return mp_GTextNode->getNodeValue();}

	/**
	 * \brief Get Y value
	 *
	 * @returns a pointer to the DSA Y string value.
	 */

	const XMLCh * getDSAY(void) const {return mp_YTextNode->getNodeValue();}

	/**
	 * \brief Get Modulus
	 *
	 * @returns A pointer to the RSA Modulus
	 */

	const XMLCh * getRSAModulus(void) const;

	/**
	 * \brief Get Exponent
	 *
	 * @returns A pointer to the buffer containing the RSA Modulus string
	 */

	const XMLCh * getRSAExponent(void) const;

	/**
	 * \brief Get NamedCurve URI
	 *
	 * @returns A pointer to the EC NamedCurve URI
	 */

	const XMLCh * getECNamedCurve(void) const;

	/**
	 * \brief Get EC Public Key
	 *
	 * @returns A pointer to the buffer containing the EC public key
	 */

	const XMLCh * getECPublicKey(void) const;

    //@}

	/** @name DSA Create and set functions */
	//@{

	/**
	 * \brief Create a blank KeyValue (and DOM structure)
	 *
	 * Create a blank KeyValue structure with the passed parameters
	 * and create the required DOM structure as well.
	 *
	 * @param P The P value (base64 encoded in unicode format)
	 * @param Q The Q value (base64 encoded in unicode format)
	 * @param G The G value (base64 encoded in unicode format)
	 * @param Y The Y value (base64 encoded in unicode format)
	 * @returns the DOMElement at the head of the DOM structure
	 */
	
	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * createBlankDSAKeyValue(const XMLCh * P,
		const XMLCh * Q,
		const XMLCh * G,
		const XMLCh * Y
	);

	/**
	 * \brief Set P value.
	 *
	 * Take the provided string and use it to set the P parameter
	 * in the KeyValue
	 */

	void setDSAP(const XMLCh * P);

	/**
	 * \brief Set Q value.
	 *
	 * Take the provided string and use it to set the Q parameter
	 * in the KeyValue
	 */

	void setDSAQ(const XMLCh * Q);

	/**
	 * \brief Set G value.
	 *
	 * Take the provided string and use it to set the G parameter
	 * in the KeyValue
	 */

	void setDSAG(const XMLCh * G);

	/**
	 * \brief Set Y value.
	 *
	 * Take the provided string and use it to set the Y parameter
	 * in the KeyValue
	 */

	void setDSAY(const XMLCh * Y);

	//@}

	/** @name RSA Create and Set functions */
	//@{

	/**
	 * \brief Create a blank RSA KeyValue
	 *
	 * Create a new RSAKeyValue object and associated DOM structures.
	 *
	 * @param modulus Base64 encoded value of Modulus to set
	 * @param exponent Base64 encoded value of the Exponent to set
	 * @returns The DOM structure of the RSAKeyValue that has been created
	 */
	
	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * 
		createBlankRSAKeyValue(const XMLCh * modulus,
			const XMLCh * exponent);

	/**
	 * \brief Set the Modulus
	 *
	 * Set the base64 encoded string of the Modulus value within the element.
	 *
	 * @param modulus Base64 encoded value to set
	 */

	void setRSAModulus(const XMLCh * modulus);

	/**
	 * \brief Set the exponent
	 *
	 * Set the base64 encoded string of the exponent value within the element
	 *
	 * @param exponent Base64 encoded value to set
	 */

	void setRSAExponent(const XMLCh * exponent);

	//@}

	/** @name EC Create and Set functions */
	//@{

	/**
	 * \brief Create a blank EC KeyValue
	 *
	 * Create a new ECKeyValue object and associated DOM structures.
	 *
	 * @param curveName URI of NamedCurve to set
	 * @param publicKey Base64 encoded key value to set
	 * @returns The DOM structure of the ECKeyValue that has been created
	 */
	
	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * 
		createBlankECKeyValue(const XMLCh * curveName,
			const XMLCh * publicKey);

	/**
	 * \brief Set the NamedCurve URI
	 *
	 * Set the URI of the NamedCurve within the element.
	 *
	 * @param curveName URI of NamedCurve to set
	 */

	void setECNamedCurve(const XMLCh * curveName);

	/**
	 * \brief Set the public key
	 *
	 * Set the base64 encoded public key
	 *
	 * @param publicKey Base64 encoded public key to set
	 */

	void setECPublicKey(const XMLCh * publicKey);

	//@}

	/** @name Information Functions */
	//@{

	/**
	 * \brief Interface call to return type
	 *
	 * @returns the type of this keyInfo node
	 */

	virtual keyInfoType getKeyInfoType(void) const;

	/**
	 * \brief Get the name of this key (irrelevant for a KeyValue)
	 *
	 * @returns NULL
	 */

	virtual const XMLCh * getKeyName(void) const;

	//@}
private:

	// Structures to hold ALL the required information
	// Not the most efficient of methods, but simple.

	// DSA

	XERCES_CPP_NAMESPACE_QUALIFIER DOMNode	* mp_PTextNode;	// Nodes where strings are stored
	XERCES_CPP_NAMESPACE_QUALIFIER DOMNode	* mp_QTextNode;
	XERCES_CPP_NAMESPACE_QUALIFIER DOMNode	* mp_GTextNode;
	XERCES_CPP_NAMESPACE_QUALIFIER DOMNode	* mp_YTextNode;

	// RSA
	
	XERCES_CPP_NAMESPACE_QUALIFIER DOMNode	* mp_modulusTextNode;
	XERCES_CPP_NAMESPACE_QUALIFIER DOMNode	* mp_exponentTextNode;

    // EC

	XERCES_CPP_NAMESPACE_QUALIFIER DOMNode	* mp_namedCurveElementNode;
	XERCES_CPP_NAMESPACE_QUALIFIER DOMNode	* mp_ecPublicKeyTextNode;

	// General

	keyInfoType								m_keyInfoType;
};

#endif /* #define DSIGKEYVALUE_INCLUDE */
