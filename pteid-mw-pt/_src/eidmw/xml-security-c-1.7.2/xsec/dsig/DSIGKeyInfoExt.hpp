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
 * DSIGKeyInfoExt := An extension type for anything unknown to the library
 *
 * $Id:$
 *
 */

#ifndef DSIGKEYINFOEXT_INCLUDE
#define DSIGKEYINFOEXT_INCLUDE

#include <xsec/dsig/DSIGKeyInfo.hpp>
#include <xsec/utils/XSECPlatformUtils.hpp>

XSEC_DECLARE_XERCES_CLASS(DOMElement);

/**
 * @ingroup pubsig
 */

/**
 * @brief The class for extension nodes in a KeyInfo list.
 *
 * Class for holding information on an extension node.
 *
 */



class DSIG_EXPORT DSIGKeyInfoExt : public DSIGKeyInfo {

public:

	/** @name Constructors and Destructors */
	//@{

	/**
	 * \brief Constructor used when XML exists.
	 *
	 * This constructor is used by interface classes
	 * an existing DOM structure into the extension element.
	 *
	 * @param env Environment the KeyInfo is operating within
	 * @param extNode DOMNode to load information from
	 */

    DSIGKeyInfoExt(const XSECEnv * env, XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *extNode);

	/**
	 * \brief Destructor
	 */

	virtual ~DSIGKeyInfoExt();

	//@}

	/** @name Load and Get functions */
	//@{

	/**
	 * \brief Load an existing XML structure into this object.
	 */

	virtual void load(void);

	//@}

	//@}

	/** @name Information Functions */
	//@{

	/**
	 * \brief Return type of this KeyInfo element
	 */

	virtual keyInfoType getKeyInfoType(void) const {return DSIGKeyInfo::KEYINFO_EXTENSION;}

	//@}

    virtual const XMLCh * getKeyName(void) const {return NULL;}

private:

	DSIGKeyInfoExt();							// Non-implemented constructor
};



#endif /* #define DSIGKEYNAME_INCLUDE */
