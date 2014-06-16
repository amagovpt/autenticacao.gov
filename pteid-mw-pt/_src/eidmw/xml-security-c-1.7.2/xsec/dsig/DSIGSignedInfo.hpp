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
 * DSIGSignature := Class for checking and setting up signature nodes in a DSIG signature
 *
 * Author(s): Berin Lautenbach
 *
 * $Id: DSIGSignedInfo.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef DSIGSIGNEDINFO_INCLUDE
#define DSIGSIGNEDINFO_INCLUDE

// XSEC Includes
#include <xsec/framework/XSECDefs.hpp>
#include <xsec/utils/XSECSafeBufferFormatter.hpp>
#include <xsec/dsig/DSIGConstants.hpp>
#include <xsec/dsig/DSIGReferenceList.hpp>

// Xerces Includes

XSEC_DECLARE_XERCES_CLASS(DOMDocument);
XSEC_DECLARE_XERCES_CLASS(DOMNode);
XSEC_DECLARE_XERCES_CLASS(DOMElement);

// General includes

#include <vector>

class XSECEnv;

/**
 * @ingroup pubsig
 */

/**
 * @brief Constructs and holds a SignedInfo.
 *
 * The \<SignedInfo\> node is the container for all the information
 * that is signed.  It contains the ReferenceList and information
 * on the signature and canonicalisation method for the signature.
 *
 * Generally this class should not be manipulated directly.
 *
 */

class DSIG_EXPORT DSIGSignedInfo {

public:

	/** @name Constructors and Destructors */
	//@{

	/**
	 * \brief Constructor for existing nodes
	 *
	 * Called by the library to construct a SignedInfo in cases
	 * where the DOM Nodes exist and need to be loaded
	 *
	 * @param doc The document containing the structure to be loaded
	 * @param pFormatter A safeBuffer formatter that will translate to UTF-8
	 * @param signedInfoNode The node at the top of the SignedInfo tree fragment
	 * @param env Operating environment
	 */

	DSIGSignedInfo(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc, 
		XSECSafeBufferFormatter * pFormatter, 
		XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *signedInfoNode,
		const XSECEnv * env);


	/**
	 * \brief Constructor for building from scratch
	 *
	 * Will set up the class in preparation for building the 
	 * DOM structure 
	 *
	 * @param doc The document to use to construct
	 * @param pFormatter Formatter to use to translate to UTF-8
	 * @param env Operating environment
	 */

	DSIGSignedInfo(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc,
				XSECSafeBufferFormatter * pFormatter, 
				const XSECEnv * env);

	/**
	 * \brief Destructor
	 * 
	 * Delete - but does not destroy the DOM Nodes
	 *
	 */

	~DSIGSignedInfo();

	//@}

	/** @name Create and Set */
	//@{

	/**
	 * \brief Load from DOM
	 *
	 * Load the SignedInfo from the DOM Document
	 *
	 * Does not do any verification of signatures or references - 
	 * simply loads the values
	 */

	void load(void);

	/**
	 * \brief Verify the SignedInfo
	 *
	 * Validates each reference contained in the SignedInfo.  Does not
	 * validate the signature itself - this is done by DSIGSignature
	 *
	 * @param errStr The safeBuffer that error messages should be written to.
	 */

	bool verify(safeBuffer &errStr);

	/**
	 * \brief Hash the reference list
	 *
	 * Goes through each reference in the SignedInfo (including referenced
	 * manifests), performs the digest operation and adds the digest
	 * to the reference element
	 *
	 * @param interlockingReferences Set to true if any references depend on other
	 * references
	 */

	void hash(bool interlockingReferences);

	/**
	 * \brief Create an empty SignedInfo
	 *
	 * Creates the DOM structure for a SignedInfo
	 *
	 * Builds the DOM structures and sets the control
	 * structures of the SignedInfo
	 *
	 * @param canonicalizationAlgorithmURI The canonicalisation method to set the SignedInfo as
	 * @param signatureAlgorithmURI Signature Method to use
	 * @param hm Hash method to use (for the SignedInfo, not the references)
	 */

	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *
		createBlankSignedInfo(
			const XMLCh * canonicalizationAlgorithmURI,
			const XMLCh * signatureAlgorithmURI
		);

	/**
	 * \brief Create an empty SignedInfo
	 *
	 * Creates the DOM structure for a SignedInfo
	 *
	 * Builds the DOM structures and sets the control
	 * structures of the SignedInfo
	 *
	 * @param cm The canonicalisation method to set the SignedInfo as
	 * @param sm Signature Method to use
	 * @param hm Hash method to use (for the SignedInfo, not the references)
	 * @deprecated Use URI based creation method instead
	 */

	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *
		createBlankSignedInfo(canonicalizationMethod cm,
			signatureMethod	sm,
			hashMethod hm
		);

	/**
	 * \brief Create a reference to add to the SignedInfo
	 *
	 * Called by DSIGSignature to create and enter a new reference element
	 *
	 * @param URI What the reference references
	 * @param hm Digest method to use for the reference
	 * @param type Reference type
	 * @deprecated Use the algorithmURI based call instead
	 */

	DSIGReference * createReference(const XMLCh * URI,
		hashMethod hm, char * type);


	/**
	 * \brief Create a reference to add to the SignedInfo
	 *
	 * Called by DSIGSignature to create and enter a new reference element
	 *
	 * @param URI What the reference references
	 * @param hashAlgorithmURI Digest method to use for the reference
	 * @param type Reference type
	 */

	DSIGReference * createReference(
		const XMLCh * URI,
		const XMLCh * hashAlgorithmURI, 
		const XMLCh * type
	);

    /**
     * \brief Remove a reference from the list
     *
     * Removes the reference at the index point and returns a pointer
     * to the reference removed.
     *
     * @note This also releases ownership.  It is the responsibility of
     * the caller to ensure the reference is deleted.
     *
     * @note This removes the reference from the Signature
     *
     * @param index Point in the list to remove
     */

    DSIGReference * removeReference(DSIGReferenceList::size_type index);

	//@}

	/** @name Getter functions */
	//@{

	/**
	 * \brief Get the node pointing to the top of the DOM fragment
	 *
	 * @returns the SignedInfo node
	 */

	XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *getDOMNode(void);

	/**
	 * \brief Get the Algorithm URI
	 *
	 * @returns the URI associated with the Algorithm used to generate
	 * the signature
	 */

	const XMLCh * getAlgorithmURI() {return mp_algorithmURI;}

	/**
	 * \brief Get the canonicalisation method 
	 * 
	 * @returns Canonicalisation method
	 */

	canonicalizationMethod getCanonicalizationMethod(void);

	/**
	 * \brief Get the hash method
	 *
	 * @returns the Hash (digest) Method
	 */

	hashMethod getHashMethod(void);

	/**
	 * \brief Get the signature method
	 *
	 * @returns the Signature method
	 */

	signatureMethod getSignatureMethod(void);

	/**
	 * \brief Get HMAC length
	 * 
	 * HMAC signatures can be truncated to a nominated length.
	 * This returns the length used.
	 */

	int getHMACOutputLength(void);

	/**
	 * \brief Return the list of references
	 *
	 * Returns a pointer to the object holding the references
	 * contained in the SignedInfo
	 */

	DSIGReferenceList *getReferenceList (void) {return mp_referenceList;}

	//@}


private:

	XSECSafeBufferFormatter		* mp_formatter;
	bool						m_loaded;				// Have we already loaded?
	XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument					
								* mp_doc;
	XERCES_CPP_NAMESPACE_QUALIFIER DOMNode						
								* mp_signedInfoNode;
	canonicalizationMethod		m_canonicalizationMethod;
	signatureMethod				m_signatureMethod;
	hashMethod					m_hashMethod;
	DSIGReferenceList			* mp_referenceList;
	int							m_HMACOutputLength;
	const XMLCh					* mp_algorithmURI;
	const XSECEnv				* mp_env;

	// Not implemented constructors

	DSIGSignedInfo();
	// DSIGSignedInfo & operator= (const DSIGSignedInfo &);

};


#endif /* DSIGSIGNEDINFO_INCLUDE */
