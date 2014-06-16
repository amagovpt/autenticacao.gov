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
 * DSIGKeyInfoX509 := A "Super" key that defines a certificate with a sub-key that defines
 *                the signing key
 *
 * $Id: DSIGKeyInfoX509.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef DSIGKEYINFOX509_INCLUDE
#define DSIGKEYINFOX509_INCLUDE

#include <xsec/utils/XSECSafeBuffer.hpp>
#include <xsec/dsig/DSIGKeyInfo.hpp>
#include <xsec/enc/XSECCryptoX509.hpp>

#include <vector>

XSEC_USING_XERCES(DOMElement);

/**
 * @ingroup pubsig
 */

/**
 * @brief The class for \<X509Data\> nodes in a KeyInfo list.
 *
 * Class for holding information on a X509Data node as well as setting
 * such a node in a signature.
 *
 */


class DSIG_EXPORT DSIGKeyInfoX509 : public DSIGKeyInfo {

public:


	struct X509Holder {

		const XMLCh			* mp_encodedX509;		// Base64 encoding
		XSECCryptoX509		* mp_cryptoX509;		// The certificate

	};

#if defined(XSEC_NO_NAMESPACES)
	typedef vector<X509Holder *>			X509ListType;
    typedef vector<const XMLCh *>           X509CRLListType;
#else
	typedef std::vector<X509Holder *>		X509ListType;
    typedef std::vector<const XMLCh *>      X509CRLListType;
#endif

#if defined(XSEC_SIZE_T_IN_NAMESPACE_STD)
	typedef std::size_t		size_type;
#else
	typedef size_t			size_type;
#endif

	/** @name Constructors and Destructors */
	//@{

	/**
	 * \brief Constructor used when XML structures exist.
	 *
	 * Constructor called by interface class when loading an
	 * X509Data element from DOM nodes.
	 *
	 * @param env Operating environment
	 * @param X509Data DOMNode at start of data
	 */

	DSIGKeyInfoX509(const XSECEnv * env, XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *X509Data);

	/**
	 * \brief Constructor called when building XML structures
	 *
	 * Constructor called by interface class when an XML
	 * structure is being built by a user calling the API
	 * 
	 * @param env Operating environment
	 */

	DSIGKeyInfoX509(const XSECEnv * env);
	
	/**
	 * \brief Destructor
	 */

	virtual ~DSIGKeyInfoX509();

	//@}

	/** @name Load function and get methods */
	//@{

	/**
	 * \brief Function called to load an XML structure
	 *
	 * Function called by intercace class to load an X509Data structure
	 * from DOMNodes.
	 */

	virtual void load();

	/**
	 * \brief Get the name of the certificate
	 *
	 * Get the name stored in the X509SubjectName element (if it
	 * exists).
	 *
	 * @returns A pointer to a buffer containing the name
	 * (NULL if not set.)
	 */

	const XMLCh * getX509SubjectName(void) const;

	/**
	 * \brief Get the name of the certificate (interface function)
	 *
	 * Get the name stored in the X509SubjectName element (if it
	 * exists).
	 *
	 * @returns A pointer to the buffer containing the name (or NULL if not set).
	 * The decoded string is returned.
	 * @see setX509SubjectName(void)
	 */

	const XMLCh * getKeyName(void) const;
	
    /**
     * \brief Get the X509Digest Algorithm
     *
     * Get the Algorithm for the X509Digest.
     *
     * @returns A pointer to the buffer containing the algorithm
     * (0 if not set.)
     */

	const XMLCh* getX509DigestAlgorithm(void) const;

    /**
     * \brief Get the X509Digest value
     *
     * Get the value for the X509Digest.
     *
     * @returns A pointer to the buffer containing the digest
     * (0 if not set.)
     */

	const XMLCh* getX509DigestValue(void) const;

    /**
     * \brief Get the IssuerSerialName
     *
     * Get the name of the Issuer (stored in the X509IssuerSerial element).
     *
     * @returns A pointer to the buffer containing the issuer name.
     * (0 if not set.)
     */

	const XMLCh * getX509IssuerName(void) const;

	/**
	 * \brief Get the IsserSerialNumber
	 *
	 * Get the serial number of the certificate of the issuer of the
	 * signing certificate.
	 * 
	 * @returns A pointer to the string containing the IssuerSerialNumber.
	 * (0 if not set.)  This is the decoded string.
	 * @see setX509IssuerSerial
	 */

	const XMLCh * getX509IssuerSerialNumber(void) const;

	/**
	 * \brief Get the first associated CRL
	 *
	 * Return the string containing the base64 encoded CRL that was held in
	 * the first X509CRL node
	 *
	 * @returns A pointer to the string containing the CRL
	 * (0 if not set)
	 */

	const XMLCh * getX509CRL(void) const;

    /**
     * \brief Find the number of CRLs held
     *
     * Find the number of CRLs held in the X509Data structures.
     *
     * @returns The number of CRLs
     */

    int getX509CRLListSize(void) const;

    /**
     * \brief Get the DER encoded CRL pointed to in the list.
     *
     * Use the index to find the required CRL and return a pointer
     * to the buffer containing the encoded CRL.
     *
     * @returns A pointer to the buffer containing the CRL or 0 if
     * no CRL exists at that point in the list.
     */

    const XMLCh * getX509CRLItem(int item) const;

	/**
	 * \brief Get the SKI value (if set)
	 *
	 * The SKI (Subject Key Identifier) can be used to reference a
	 * required certificate.  If this was set in the KeyInfo element,
	 * this function will return the value
	 *
	 * @returns the base64 encoded (plan - not DER) encoded SKI value
	 */

	const XMLCh * getX509SKI(void) const;

	/**
	 * \brief Return the raw Retrieval method to find this certificate
	 *
	 * In some cases, the KeyInfo RetrievalMethod references a raw certificate
	 * In such cases, the library simply creates a KeyInfoX509 object and sets
	 * this string to allow others to find the certificate
	 */

	const XMLCh * getRawRetrievalURI(void) const;

	/**
	 * \brief Find the number of certificates held
	 *
	 * Find the number of certificates held in the X509Data structures.
	 *
	 * @returns The number of certificates
	 */

	int getCertificateListSize(void) const;

	/**
	 * \brief Get the DER encoded certificate pointed to in the list.
	 *
	 * Use the index to find the required certificate and return a pointer
	 * to the buffer containing the encoded certificate.
	 *
	 * @returns A pointer to the buffer containing the certificate or 0 if
	 * no certificate exists at that point in the list.
	 */

	const XMLCh * getCertificateItem(int item) const;

	/**
	 * \brief Get the Crypto Interface X509 structure version of the certificate
	 *
	 * Use the index to find the required certificate and return a pointer
	 * to the XSECCryptoX509 cert
	 *
	 * @returns A pointer to the XSECCryptoX509 cert structure
	 */

	XSECCryptoX509 * getCertificateCryptoItem(int item);

    /**
	 * \brief Get the Crypto Interface X509 structure version of the certificate
	 *
	 * Use the index to find the required certificate and return a pointer
	 * to the XSECCryptoX509 cert
	 *
	 * @returns A pointer to the XSECCryptoX509 cert structure
	 */

	const XSECCryptoX509 * getCertificateCryptoItem(int item) const;

	/**
	 * \brief Interface function to find the type of this KeyInfo
	 */
	
	virtual keyInfoType getKeyInfoType(void) const {return DSIGKeyInfo::KEYINFO_X509;}

	//@}

	/** @name Create and Set functions */
	//@{

	/**
	 * \brief Create a new X509 data element.
	 *
	 * Create a blank (empty) X509Data element that can then be used
	 * by the application to add X509Data elements.
	 *
	 * @returns A pointer to the new X509Data element.
	 */

	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * 
		createBlankX509Data(void);

	/**
	 * \brief Set the X509SubjectName element in the KeyInfo element.
	 *
	 * If a X509SubjectName element exists, replace the text with the
	 * provided text.  Otherwise create the element and set the text.
	 *
	 * @note XMLDSIG requires Distinguished Names be encoded in a defined
	 * manner (escaping '<' characters etc.).  This method will perform
	 * this encoding prior to creating the DOM nodes.
	 *
	 * @param name The name to set.
	 */

	void setX509SubjectName(const XMLCh * name);

	/**
	 * \brief Set the IssuerSerial element
	 *
	 * If an X509IssuerSerial exists, replace the values with those provided,
	 * otherwise create a new element and set the values appropriately.
	 *
	 * @note XMLDSIG requires Distinguished Names be encoded in a defined
	 * manner (escaping '<' characters etc.).  This method will perform
	 * this encoding prior to creating the DOM nodes.
	 *
	 * @param name The name of the issuer.
	 * @param serial The serial number of the issuer's certificate
	 */

	void setX509IssuerSerial(const XMLCh * name, const XMLCh * serial);

    /**
     * \brief Set the dsig11:X509Digest element
     *
     * If a dsig11:X509Digest exists, replace the values with those provided,
     * otherwise create a new element and set the values appropriately.
     *
     * @param algorithm The algorithm type
     * @param value The digest value
     */

    void setX509Digest(const XMLCh * algorithm, const XMLCh * value);

	/**
     * \brief Add a CRL.
	 *
     * Append an X509CRL element to the list of CRLs
     * stored at the end of this KeyInfo element.
	 *
     * @param crl A pointer to the base64 encoded CRL,
     * exactly as it will appear in the XML structure.
	 */

	void setX509CRL(const XMLCh * crl);

    /**
     * \brief Add a CRL.
     *
     * Append an X509CRL element to the list of CRLs
     * stored at the end of this KeyInfo element.
     *
     * @param crl A pointer to the base64 encoded CRL,
     * exactly as it will appear in the XML structure.
     */

    void appendX509CRL(const XMLCh * crl);
	
	/**
	 * \brief Set the SKI element
	 *
	 * The SKI node provides a reference to the Subject Key Identifier of
	 * a certificate.
	 *
	 * This function takes a base64 encoded ski and sets it into the 
	 * appropriate node
	 *
	 * @param ski The base64 plain (non-DER) encoded SKI value
	 */

	void setX509SKI(const XMLCh * ski);

	/**
	 * \brief set the retrieval URI
	 *
	 * Generally to be used by internal library processes only.
	 * This sets the retrieval URI - but does _not_ manipulate the DOM
	 * in any way.
	 *
	 * @param uri The URI string to use
	 */

	void setRawRetrievalURI(const XMLCh * uri);

	/**
	 * \brief Add a certificate.
	 *
	 * Append an X509Certificate element to the list of certificates
	 * stored at the end of this KeyInfo element.
	 *
	 * @param base64Certificate A pointer to the base64 encoded certificate,
	 * exactly as it will appear in the XML structure.
	 */

	void appendX509Certificate(const XMLCh * base64Certificate);

	//@}


private:

	DSIGKeyInfoX509();

	X509ListType		m_X509List;				// The X509 structures
	X509CRLListType     m_X509CRLList;          // The X509CRL list
	XMLCh 				* mp_X509IssuerName;	// Parameters from KeyInfo (not cert)
	const XMLCh 		* mp_X509SerialNumber;
	XMLCh 				* mp_X509SubjectName;
	const XMLCh			* mp_X509SKI;
	XMLCh				* mp_rawRetrievalURI;

	// Text nodes holding information

	XERCES_CPP_NAMESPACE_QUALIFIER DOMNode	* mp_X509SubjectNameTextNode;
	XERCES_CPP_NAMESPACE_QUALIFIER DOMNode	* mp_X509IssuerNameTextNode;
	XERCES_CPP_NAMESPACE_QUALIFIER DOMNode	* mp_X509SerialNumberTextNode;
	XERCES_CPP_NAMESPACE_QUALIFIER DOMNode	* mp_X509SKITextNode;
	XERCES_CPP_NAMESPACE_QUALIFIER DOMNode  * mp_X509DigestTextNode;

};



#endif /* #define DSIGKEYX509_INCLUDE */
