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
 * DSIGKeyInfoMgmtData := MgmtData - used for conveying in-band key data
 *
 * Author(s): Berin Lautenbach
 *
 * $Id: DSIGKeyInfoMgmtData.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef DSIGKEYINFOMGMTDATA_INCLUDE
#define DSIGKEYINFOMGMTDATA_INCLUDE

#include <xsec/dsig/DSIGKeyInfo.hpp>
#include <xsec/utils/XSECPlatformUtils.hpp>

XSEC_DECLARE_XERCES_CLASS(DOMElement);

/**
 * @ingroup pubsig
 */

/**
 * @brief The class for \<MgmtData\> nodes in a KeyInfo list.
 *
 * Class for holding in band key information.
 * @note Use of this class is <em>NOT RECOMMEDED</em> within
 * the DSIG standard.
 *
 */



class DSIG_EXPORT DSIGKeyInfoMgmtData : public DSIGKeyInfo {

public:

	/** @name Constructors and Destructors */
	//@{

	/**
	 * \brief Constructor used when XML exists.
	 *
	 * This constructor is used by other user interface classes to load
	 * an existing DOM structure into the Name element.
	 *
	 * @param env Environment operating within.
	 * @param nameNode DOMNode to load information from
	 */

	DSIGKeyInfoMgmtData(const XSECEnv * env, XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *nameNode);

	/**
	 * \brief Constructor used when XML needs to be created.
	 *
	 * This constructor is used by interface objects to
	 * create a DSIGKeyInfoName object that can then be used
	 * to create the required XML.
	 *
	 * @param env Operating environment
	 */

	DSIGKeyInfoMgmtData(const XSECEnv * env);

	/**
	 * \brief Destructor
	 */

	virtual ~DSIGKeyInfoMgmtData();

	//@}

	/** @name Load and Get functions */
	//@{

	/**
	 * \brief Load an existing XML structure into this object.
	 */

	virtual void load(void);

	/**
	 * \brief Get key name - unimplemented for MgmtData
	 */

	virtual const XMLCh * getKeyName(void) const {return NULL;}

	/**
	 * \brief Get the MgmtData string
	 *
	 * Returns a pointer to the buffer containing the data string.
	 *
	 * @returns A pointer to the XMLCh buffer containing the data
	 */

	virtual const XMLCh * getData(void) const {return mp_data;};

	//@}

	/**@name Create and set functions */
	//@{
	
	/**
	 * \brief Create a new MgmtData element in the current document.
	 *
	 * Creates a new MgmtData element and sets the data with the string
	 * passed in.
	 *
	 * @param data Value to set the MgmtData to
	 * @returns The newly created DOMElement with the structure underneath.
	 */

	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * createBlankMgmtData(const XMLCh * data);

	/**
	 * \brief Set the value of the MgmtData to a new string.
	 *
	 * Uses the passed in string to set a new value in the DOM structure.
	 *
	 * @param data Value to set in MgmtData
	 */

	void setData(const XMLCh * data);

	//@}

	/** @name Information Functions */
	//@{
	
	/**
	 * \brief Return type of this KeyInfo element
	 */
	
	virtual keyInfoType getKeyInfoType(void) const {return DSIGKeyInfo::KEYINFO_MGMTDATA;}

	//@}

private:

	DSIGKeyInfoMgmtData();						// Non-implemented constructor

	const XMLCh			* mp_data;				// The Data stored in the XML file
	XERCES_CPP_NAMESPACE_QUALIFIER DOMNode				
						* mp_dataTextNode;		// Text node containing the name

};



#endif /* #define DSIGKEYINFOMGMTDATA_INCLUDE */

