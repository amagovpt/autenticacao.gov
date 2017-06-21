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
 * DSIGKeyInfoSPKIData := SPKI Information
 *
 * Author(s): Berin Lautenbach
 *
 * $Id: DSIGKeyInfoSPKIData.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef DSIGKEYINFOSPKIDATA_INCLUDE
#define DSIGKEYINFOSPKIDATA_INCLUDE

#include <xsec/dsig/DSIGKeyInfo.hpp>
#include <xsec/utils/XSECPlatformUtils.hpp>

XSEC_DECLARE_XERCES_CLASS(DOMElement);

#include <vector>

/**
 * @ingroup pubsig
 */

/**
 * @brief The class for \<SPKIData\> nodes in a KeyInfo list.
 *
 * Class for holding information on a SPKIData node as well as setting
 * such a node in a signature.
 *
 */



class DSIG_EXPORT DSIGKeyInfoSPKIData : public DSIGKeyInfo {

public:

	/** @name Constructors and Destructors */
	//@{

	/**
	 * \brief Constructor used when XML exists.
	 *
	 * This constructor is used by interface objects to load
	 * an existing DOM structure into the Name element.
	 *
	 * @param env Environment KeyInfo is operating in
	 * @param nameNode DOMNode to load information from
	 */

	DSIGKeyInfoSPKIData(const XSECEnv * env, 
						XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *nameNode);

	/**
	 * \brief Constructor used when XML needs to be created.
	 *
	 * This constructor is used by interface objects to
	 * create a DSIGKeyInfoSPKIData object that can then be used
	 * to create the required XML.
	 *
	 * @param env Operating environment
	 */

	DSIGKeyInfoSPKIData(const XSECEnv * env);

	/**
	 * \brief Destructor
	 */

	virtual ~DSIGKeyInfoSPKIData();

	//@}

	/** @name Load and Get functions */
	//@{

	/**
	 * \brief Load an existing XML structure into this object.
	 */

	void load(void);

	/**
	 * \brief Get the number of S-expressions 
	 *
	 * Returns the number of S-expressions held for this SPKIData element
	 *
	 * @returns The number of S-expressions
	 */

	unsigned int getSexpSize(void) const;

	/**
	 * \brief returns the indicated SExpression
	 *
	 * Returns a pointer to a XMLCh buffer holding the required SExpression.
	 *
	 * @param index The number of the SExpression to return
	 * @returns A pointer to the char buffer containing the base64 encoded 
	 * S-expression
	 */

	virtual const XMLCh * getSexp(unsigned int index) const;

	/**
	 * \brief Get key name - unimplemented for SPKI packets
	 */

	virtual const XMLCh * getKeyName(void) const {return NULL;}

	//@}

	/**@name Create and set functions */
	//@{
	
	
	/**
	 * \brief Create a new SPKIData element in the document.
	 *
	 * Creates a new SPKIData element and sets the first S-expression
	 * with the string passed in.
	 *
	 * @param Sexp Value (base64 encoded string) to set the first S-expression
	 * @returns The newly created DOMElement with the structure underneath.
	 */

	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * 
		createBlankSPKIData(const XMLCh * Sexp);

	/**
	 * \brief Append a new SPKISexp element to the SPKIData nodes
	 *
	 * Append a new SPKISexp element to the list of S-expressions that
	 * already exists.
	 *
	 * @param Sexp Value (base64 encoded string) to set the new S-expression
	 */

	void appendSexp(const XMLCh * Sexp);

	//@}

	/** @name Information Functions */
	//@{
	
	/**
	 * \brief Return type of this KeyInfo element
	 */
	
	virtual keyInfoType getKeyInfoType(void) const {return DSIGKeyInfo::KEYINFO_SPKIDATA;}

	//@}

private:

	DSIGKeyInfoSPKIData();						// Non-implemented constructor

	struct SexpNode {
		const XMLCh		* mp_expr;
		XERCES_CPP_NAMESPACE_QUALIFIER DOMNode			
						* mp_exprTextNode;
	};

	typedef std::vector<SexpNode *> sexpVectorType;

	sexpVectorType		m_sexpList;

};



#endif /* #define DSIGKEYSPKIDATA_INCLUDE */
