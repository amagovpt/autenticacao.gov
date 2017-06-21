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
 * DSIGKeyInfoPGPData := Contains a key pointer to a PGP key
 *
 * Author(s): Berin Lautenbach
 *
 * $Id: DSIGKeyInfoPGPData.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef DSIGKEYINFOPGPDATA_INCLUDE
#define DSIGKEYINFOPGPDATA_INCLUDE

#include <xsec/utils/XSECSafeBuffer.hpp>
#include <xsec/dsig/DSIGKeyInfo.hpp>
#include <xsec/utils/XSECPlatformUtils.hpp>

XSEC_DECLARE_XERCES_CLASS(DOMElement);

/**
 * @ingroup pubsig
 */

/**
 * @brief The class for \<PGPData\> nodes in a KeyInfo list.
 *
 * Class for holding information on a PGP Data node as well as setting
 * such a node in a signature.
 *
 */



class DSIG_EXPORT DSIGKeyInfoPGPData : public DSIGKeyInfo {

public:

	/** @name Constructors and Destructors */
	//@{

	/**
	 * \brief Constructor used when XML exists.
	 *
	 * This constructor is used by interface objects to load
	 * an existing DOM structure into the PGPData element.
	 *
	 * @param env Environment the KeyInfo is operating within
	 * @param pgpDataNode DOMNode to load information from
	 */

	DSIGKeyInfoPGPData(const XSECEnv * env, 
					   XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *pgpDataNode);

	/**
	 * \brief Constructor used when XML needs to be created.
	 *
	 * This constructor is used by interface objects to
	 * create a DSIGKeyInfoPGPData object that can then be used
	 * to create the required XML.
	 *
	 * @param env Operating environment
	 */

	DSIGKeyInfoPGPData(const XSECEnv * env);

	/**
	 * \brief Destructor
	 */

	virtual ~DSIGKeyInfoPGPData();

	//@}

	/** @name Load and Get functions */
	//@{

	/**
	 * \brief Load an existing XML structure into this object.
	 */

	virtual void load(void);

	/**
	 * \brief Get the PGP Key ID held in the structure
	 *
	 * Returns a pointer to the buffer containing the ID of the PGP key.
	 *
	 * @returns A pointer to the char buffer containing the (base 64 encoded)
	 * PGP Key ID
	 */

	virtual const XMLCh * getKeyID(void) const {return mp_keyID;}

	/**
	 * \brief Get the PGP Key Packet
	 *
	 * Returns a pointer to the buffer containing the PGP Key packet.
	 *
	 * @returns A pointer to the XMLCh buffer containing the Key Packet in
	 * base64 encoded format
	 */

	virtual const XMLCh * getKeyPacket(void) const {return mp_keyPacket;}

	/**
	 * \brief Get key name - unimplemented for PGP packets
	 */

	virtual const XMLCh * getKeyName(void) const {return NULL;}

	//@}

	/**@name Create and set functions */
	//@{
	
	/**
	 * \brief Create a new PGPData element in the current document.
	 *
	 * Creates a new PGPData element and sets the KeyPacket and KeyID 
	* with the strings passed in.
	 *
	 * @param id Value to set the KeyID as (NULL if not to be set)
	 * @param packet Value to set the key packet to (NULL if not to be set)
	 * @returns The newly created DOMElement with the structure underneath.
	 */

	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * 
		createBlankPGPData(const XMLCh * id, const XMLCh * packet);

	/**
	 * \brief Set the value of the KeyID to a new string.
	 *
	 * Uses the passed in string to set a new value in the DOM structure.
	 *
	 * @param id The value to set the KeyID to
	 */

	void setKeyID(const XMLCh * id);

	/**
	 * \brief Set the value of the KeyPacket to a new string.
	 *
	 * Uses the passed in string to set a new value in the DOM structure.
	 *
	 * @param packet The value to set the KeyPacket to
	 */

	void setKeyPacket(const XMLCh * packet);

	//@}

	/** @name Information Functions */
	//@{
	
	/**
	 * \brief Return type of this KeyInfo element
	 */
	
	virtual keyInfoType getKeyInfoType(void) const {return DSIGKeyInfo::KEYINFO_PGPDATA;}

	//@}

private:

	DSIGKeyInfoPGPData();						// Non-implemented constructor

	const XMLCh			* mp_keyID;				// Key ID (from DOM)
	const XMLCh			* mp_keyPacket;			// key Packet (from DOM)
	XERCES_CPP_NAMESPACE_QUALIFIER DOMNode				
						* mp_keyIDTextNode;		// Text node containing the ID
	XERCES_CPP_NAMESPACE_QUALIFIER DOMNode				
						* mp_keyPacketTextNode;	// Text node contianing key packet
};



#endif /* #define DSIGKEYINFOPGPDATA_INCLUDE */
