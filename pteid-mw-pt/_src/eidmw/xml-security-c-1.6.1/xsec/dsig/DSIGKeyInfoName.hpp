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
 * DSIGKeyInfoName := References a key by name.
 *
 * $Id: DSIGKeyInfoName.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef DSIGKEYINFONAME_INCLUDE
#define DSIGKEYINFONAME_INCLUDE

#include <xsec/utils/XSECSafeBuffer.hpp>
#include <xsec/dsig/DSIGKeyInfo.hpp>
#include <xsec/utils/XSECPlatformUtils.hpp>

XSEC_DECLARE_XERCES_CLASS(DOMElement);

/**
 * @ingroup pubsig
 */

/**
 * @brief The class for \<KeyName\> nodes in a KeyInfo list.
 *
 * Class for holding information on a KeyName node as well as setting
 * such a node in a signature.
 *
 */



class DSIG_EXPORT DSIGKeyInfoName : public DSIGKeyInfo {

public:

	/** @name Constructors and Destructors */
	//@{

	/**
	 * \brief Constructor used when XML exists.
	 *
	 * This constructor is used by interface classes
	 * an existing DOM structure into the Name element.
	 *
	 * @param env Environment the KeyInfo is operating within
	 * @param nameNode DOMNode to load information from
	 */

	DSIGKeyInfoName(const XSECEnv * env, XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *nameNode);

	/**
	 * \brief Constructor used when XML needs to be created.
	 *
	 * This constructor is used by interface objects to
	 * create a DSIGKeyInfoName object that can then be used
	 * to create the required XML.
	 *
	 * @param env Operating environment
	 */

	DSIGKeyInfoName(const XSECEnv * env);

	/**
	 * \brief Destructor
	 */

	virtual ~DSIGKeyInfoName();

	//@}

	/** @name Load and Get functions */
	//@{

	/**
	 * \brief Load an existing XML structure into this object.
	 */

	virtual void load(void);

	/**
	 * \brief Get the name held in the structure
	 *
	 * Returns a pointer to the buffer containing the name of the key.
	 *
	 * @returns A pointer to the char buffer containing the key name
	 */

	virtual const XMLCh * getKeyName(void) const {return mp_name;};

	/**
	 * \brief Get the decoded key name (for distinguished names)
	 *
	 * Returns a pointer to the buffer containing the decoded Distinguished
	 * Name.
	 *
	 * @returns A pointer to the char buffer containing the key name
	 */

	virtual const XMLCh * getDecodedKeyName(void);

	//@}

	/**@name Create and set functions */
	//@{

	/**
	 * \brief Create a new KeyName element in the current document.
	 *
	 * Creates a new KeyName element and sets the name with the string
	 * passed in.
	 *
	 * @param name Value to set the KeyName as
	 * @param isDName If set to true, the name will be encoded as a DName
	 * prior to be being placed in the DOM document
	 * @returns The newly created DOMElement with the structure underneath.
	 */

	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * createBlankKeyName(const XMLCh * name, bool isDName = false);

	/**
	 * \brief Set the value of the KeyName to a new string.
	 *
	 * Uses the passed in string to set a new value in the DOM structure.
	 *
	 * @param name Value to set in KeyName
	 * @param isDName If set to true, the name will be encoded as a DName
	 * prior to be being placed in the DOM document
	 */

	void setKeyName(const XMLCh * name, bool isDName = false);

	//@}

	/** @name Information Functions */
	//@{

	/**
	 * \brief Return type of this KeyInfo element
	 */

	virtual keyInfoType getKeyInfoType(void) const {return DSIGKeyInfo::KEYINFO_NAME;}

	//@}

private:

	DSIGKeyInfoName();							// Non-implemented constructor

	const XMLCh			* mp_name;				// The Data stored in the XML file
	XMLCh				* mp_decodedDName;		// When this is a DName that needs decoding
	XERCES_CPP_NAMESPACE_QUALIFIER DOMNode
						* mp_keyNameTextNode;	// Text node containing the name

};



#endif /* #define DSIGKEYNAME_INCLUDE */
