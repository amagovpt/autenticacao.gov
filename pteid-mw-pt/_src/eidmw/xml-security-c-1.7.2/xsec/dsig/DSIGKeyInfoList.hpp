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
 * DSIGKeyInfoList := Class for Loading and storing a list of KeyInfo elements
 *					 
 * $Id: DSIGKeyInfoList.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef DSIGKEYINFOLIST_INCLUDE
#define DSIGKEYINFOLIST_INCLUDE


// XSEC Includes
#include <xsec/framework/XSECDefs.hpp>
#include <xsec/dsig/DSIGKeyInfo.hpp>
#include <xsec/utils/XSECSafeBufferFormatter.hpp>
#include <xsec/enc/XSECCryptoKey.hpp>

// Xerces
#include <xercesc/dom/DOM.hpp>

// General includes
#include <vector>

// Forward definitions
class DSIGKeyInfoValue;
class DSIGKeyInfoDEREncoded;
class DSIGKeyInfoX509;
class DSIGKeyInfoName;
class DSIGKeyInfoPGPData;
class DSIGKeyInfoSPKIData;
class DSIGKeyInfoMgmtData;
class DSIGSignature;

/**
 * @ingroup pubsig
 */

/**
 * \brief Container class for KeyInfo elements.
 *
 * The library stores KeyInfo lists using this class.
 */

class DSIG_EXPORT DSIGKeyInfoList {

public:

#if defined(XSEC_NO_NAMESPACES)
	typedef vector<DSIGKeyInfo *>			KeyInfoListVectorType;
#else
	typedef std::vector<DSIGKeyInfo *>		KeyInfoListVectorType;
#endif

#if defined(XSEC_SIZE_T_IN_NAMESPACE_STD)
	typedef std::size_t		size_type;
#else
	typedef size_t			size_type;
#endif

	/** @name Constructors and Destructors */
	//@{

	/**
	 * \brief Main constructor
	 *
	 * Main constructor called by DSIGSignature
	 *
	 * @note Should only ever be created by a Signature or Cipher class.
	 *
	 * @param env The environment the KeyInfo is operating within
	 */

	DSIGKeyInfoList(const XSECEnv * env);

	/**
	 * \brief Destructor
	 */

	~DSIGKeyInfoList();

	//@}

	/** @name Public (API) functions */
	//@{

	/**
	 * \brief Get size of list
	 *
	 * @returns the number of elements in the list
	 */

	size_t getSize() const;

	/*
	 * \brief Get an item
	 *
	 * Returns the item at index point in the list
	 *
	 * @note This is an internal function and should not be called directly
	 *
	 * @param index Pointer into the list
	 * @returns The indicated element or 0 if it does not exist.
	 */

	DSIGKeyInfo * item(size_type index);

	/*
	 * \brief Get an item
	 *
	 * Returns the item at index point in the list
	 *
	 * @note This is an internal function and should not be called directly
	 *
	 * @param index Pointer into the list
	 * @returns The indicated element or 0 if it does not exist.
	 */

	const DSIGKeyInfo * item(size_type index) const;

	//@}

	/** @name Manipulate the List */
	//@{

	/**
	 * \brief Add an already created KeyInfo 
	 *
	 * Adds a KeyInfo element that has already been built
	 * into the list.
	 *
	 * @note This method does <b>not</b> insert the KeyInfo into the DOM
	 * structure - it assumes it is already there and just adds to the list.
	 * @param ref The KeyInfo to add
	 */

	void addKeyInfo(DSIGKeyInfo * ref);

	/**
	 * \brief Add an already created KeyInfo 
	 *
	 * Adds a KeyInfo element that has already been built
	 * into the list.
	 *
	 * @note This method finds the DOM node of the KeyInfo and inserts it
	 * into the <KeyInfo/> list in the DOM.
	 * @param ref The KeyInfo to add
	 */

	void addAndInsertKeyInfo(DSIGKeyInfo * ref);	
	
	/**
	 * \brief Read from DOM and create.
	 *
	 * Uses a DOMNode pointing to the start of the KeyInfo element
	 * to build a new KeyInfo and then add it to the list
	 *
	 * @note This is an internal function and should not be called directly
	 *
	 * @param ki Head of DOM structure with the KeyInfo
	 * @returns true if successfully loaded
	 */

	bool addXMLKeyInfo(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *ki);

	/**
	 * \brief Read full list from DOM
	 *
	 * Will take the starting node of a KeyInfo list and read into the
	 * list structure.  This is a bit different from other "load"
	 * functions, in that it takes a node as a parameter.
	 *
	 * @note This is an internal functions and should not be called directly
	 *
	 * @param node The \<KeyInfo\> element node to read from
	 */

	bool loadListFromXML(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode * node);

	/*
	 * \brief Remove a KeyInfo from the list
	 *
	 * Removes the KeyInfo element at the indicated index point in the list
	 *
	 * @param index Pointer in the list to remove element from
	 * @returns The removed KeyInfo element
	 */

	DSIGKeyInfo * removeKeyInfo(size_type index);

	/**
	 * \brief Set the overarching environment
	 *
	 * Sets the environment this list is operating within
	 *
	 * @param env Operating environment
	 */

	void setEnvironment(const XSECEnv * env) {mp_env = env;}

	/**
	 * \brief Clear out the list
	 *
	 * Removes all elements from the list
	 *
	 * @note Deletes the items themselves as well as clearing the list.
	 */

	void empty();
	
	/**
	 * \brief Is the list empty?
	 *
	 * @returns true Iff there are no elements in the list
	 */

	bool isEmpty() const;

	//@}

	/** @name Create new KeyInfo elements */
	//@{

	/**
	 * \brief Create basic KeyInfo element.
	 *
	 * Creates the basic KeyInfo node that can then be used to
	 * embed specific KeyInfo types
	 */

	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * 
		createKeyInfo(void);

	/**
	 * \brief Append a DSA KeyValue element 
	 *
	 * Add a new KeyInfo element for a DSA Value
	 *
	 * @param P Base64 encoded value of P
	 * @param Q Base64 encoded value of Q
	 * @param G Base64 encoded value of G
	 * @param Y Base64 encoded value of Y
	 * @returns A pointer to the created object.
	 */

	DSIGKeyInfoValue * appendDSAKeyValue(const XMLCh * P, 
						   const XMLCh * Q, 
						   const XMLCh * G, 
						   const XMLCh * Y);

	/**
	 * \brief Append a RSA KeyValue element 
	 *
	 * Add a new KeyInfo element for a RSA Value
	 *
	 * @param modulus Base64 encoded value of the modulus
	 * @param exponent Base64 encoded value of exponent
	 * @returns A pointer to the created object.
	 */

	DSIGKeyInfoValue * appendRSAKeyValue(const XMLCh * modulus, 
						   const XMLCh * exponent);

	/**
	 * \brief Append an EC KeyValue element 
	 *
	 * Add a new KeyInfo element for an EC Value
	 *
	 * @param curveName URI (generally urn:oid:...) that identifies the curve
	 * @param keyValue Base64 encoded public key
	 * @returns A pointer to the created object.
	 */

	DSIGKeyInfoValue * appendECKeyValue(const XMLCh * curveName, 
						   const XMLCh * keyValue);

	/**
	 * \brief Append a X509Data element.
	 *
	 * Add a new KeyInfo element for X509 data.
	 *
	 * @note The added element is empty.  The caller must make use of the
	 * returned object to set the required values.
	 *
	 * @returns A pointer to the created object.
	 */

	DSIGKeyInfoX509 * appendX509Data(void);

	/**
	 * \brief Append a KeyName element.
	 *
	 * Add a new KeyInfo element for a key name.
	 *
	 * @param name The name of the key to set in the XML
	 * @param isDName Treat the name as a Distinguished name and encode accordingly
	 * @returns A pointer to the created object
	 */

	DSIGKeyInfoName * appendKeyName(const XMLCh * name, bool isDName = false);

	/**
	 * \brief Append a PGPData element.
	 *
	 * Add a new KeyInfo element for a PGP key.
	 *
	 * @param id The ID of the key to set in the XML (base64 encoded - NULL if none)
	 * @param packet The Packet information to set in the XML (base64 encoded -
	 * NULL if none)
	 * @returns A pointer to the created object
	 */

	DSIGKeyInfoPGPData * appendPGPData(const XMLCh * id, const XMLCh * packet);

	/**
	 * \brief Append a SPKIData element
	 *
	 * Add a new KeyInfo element for a set of SPKI S-expressions
	 *
	 * @param sexp The initial S-expression to set in the SPKIData element
	 * @returns A pointer to the created object
	 */

	DSIGKeyInfoSPKIData * appendSPKIData(const XMLCh * sexp);

	/**
	 * \brief Append a MgmtData element
	 *
	 * Add a new KeyInfo element for Management Data
	 *
	 * @param data The string to set in the MgmtData element
	 * @returns A pointer to the created object
	 */

	DSIGKeyInfoMgmtData * appendMgmtData(const XMLCh * data);

	/**
	 * \brief Append a DEREncodedKeyValue element
	 *
	 * Add a new KeyInfo element for a DEREncodedKeyValue
	 *
	 * @param data The data to set in the DEREncodedKeyValue element
	 * @returns A pointer to the created object
	 */

	DSIGKeyInfoDEREncoded * appendDEREncoded(const XMLCh * data);

	//@}

private:

	DSIGKeyInfoList();

	KeyInfoListVectorType					m_keyInfoList;
	const XSECEnv							* mp_env;
	XERCES_CPP_NAMESPACE_QUALIFIER DOMNode	* mp_keyInfoNode;
	// KeyInfoListVectorType::iterator			m_iterator;
};


#endif /* DSIGKEYINFOLIST_INCLUDE */
