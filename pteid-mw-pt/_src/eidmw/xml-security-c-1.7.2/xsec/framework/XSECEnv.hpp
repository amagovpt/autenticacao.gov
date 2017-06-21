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
 * XSECEnv := Configuration class - used by the other classes to retrieve
 *            information on the environment they are working under
 *
 * $Id: XSECEnv.hpp 1350043 2012-06-13 22:31:04Z scantor $
 *
 */

#ifndef XSECENV_INCLUDE
#define XSECENV_INCLUDE

// XSEC Includes
#include <xsec/framework/XSECDefs.hpp>
#include <xsec/utils/XSECSafeBufferFormatter.hpp>

#include <vector>

// Xerces Includes

#include <xercesc/dom/DOM.hpp>

class XSECURIResolver;

/**
 * @ingroup internal
 */
/*\@{*/

/**
 * @brief Holds environmental information
 *
 * The various XSEC classes need to be able to retrieve information
 * about the environment they are operating in (namespace prefixes,
 * owning document etc.) - this class is used to provide and hold
 * this info.
 *
 */

class DSIG_EXPORT XSECEnv {

public:
    
	/** @name Constructors and Destructors */
    //@{
	
    /**
	 * \brief Contructor.
	 *
	 */

	XSECEnv(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc);
	XSECEnv(const XSECEnv & theOther);
	virtual ~XSECEnv();

    //@}
	
	/** @name Prefix handling. */
	//@{

	/**
	  * \brief Set the prefix be used for the DSIG namespace.
	  *
	  * <p>When the XSEC library creates XML Element nodes, it uses the prefix here
	  * for all nodes created.  By default, the library assumes that the default
	  * namespace is used.</p>
	  *
	  * <p>The #createBlankSignature function will use this prefix to setup the
	  * dsig namespace.  E.g. (assuming a call has been made to set the prefix to "ds")
	  * the \<Signature\> element will have a namespace attribute added of</p>
	  *
	  * <p>xmlns:ds="http://www.w3.org/2000/09/xmldsig#"</p>
	  *
	  * <p>If no prefix has been set, this attribute will be set as the default namespace</p>
	  *
	  * @see #createBlankSignature
	  * @param prefix The UTF-16 encoded NS prefix to use for the XML 
	  * Digital Signature nodes
	  */

	void setDSIGNSPrefix(const XMLCh * prefix);

    /**
	  * \brief Set the prefix be used for the DSIG 1.1 namespace.
	  *
	  * <p>When the XSEC library creates XML Element nodes, it uses the prefix here
	  * for all nodes created.  By default, the library assumes that the default
	  * namespace is used.</p>
	  *
	  * @param prefix The UTF-16 encoded NS prefix to use for the XML 
	  * Digital Signature 1.1 nodes
	  */

	void setDSIG11NSPrefix(const XMLCh * prefix);

	/**
	  * \brief Set the prefix be used for the Exclusive Canonicalisation namespace.
	  *
	  * The Exclusive Canonicalisation specification defines a new namespace for the
	  * InclusiveNamespaces node.  This function can be used to set the prefix
	  * that the library will use when creating nodes within this namespace.
	  *
	  * <p>xmlns:ds="http://www.w3.org/2001/10/xml-exc-c14n#"</p>
	  *
	  * If no prefix is set, the default namespace will be used
	  *
	  * @see #createBlankSignature
	  * @param prefix The UTF-16 encoded NS prefix to use for the XML 
	  * Exclusive Canonicalisation nodes
	  */

	void setECNSPrefix(const XMLCh * prefix);

	/**
	  * \brief Set the prefix be used for the XPath-Filter2 namespace.
	  *
	  * The XPathFilter definition uses its own namespace.  This
	  * method can be used to set the prefix that the library will use
	  * when creating elements in this namespace
	  *
	  * <p>xmlns:ds="http://www.w3.org/2002/06/xmldsig-filter2"</p>
	  *
	  * If no prefix is set, the default namespace will be used
	  *
	  * @see #createBlankSignature
	  * @param prefix The UTF-16 encoded NS prefix to use for the XPath
	  * filter nodes
	  */

	void setXPFNSPrefix(const XMLCh * prefix);

	/**
	 * \brief Set prefix for XENC nodes
	 *
	 * Set the namespace prefix the library will use when creating
	 * nodes in the XENC namespace
	 */

	void setXENCNSPrefix(const XMLCh * prefix);

	/**
	 * \brief Set prefix for XENC 1.1 nodes
	 *
	 * Set the namespace prefix the library will use when creating
	 * nodes in the XENC 1.1 namespace
	 */

	void setXENC11NSPrefix(const XMLCh * prefix);

	/**
	 * \brief Set prefix for XKMS nodes
	 *
	 * Set the namespace prefix the library will use when creating
	 * nodes in the XKMS namespace
	 */

	void setXKMSNSPrefix(const XMLCh * prefix);

	/**
	 * \brief Get the NS Prefix being used for DSIG elements.
	 *
	 * @returns A pointer to the buffer holding the prefix
	 * @see #setDSIGNSPrefix
	 *
	 */

	const XMLCh * getDSIGNSPrefix() const {return mp_prefixNS;}

	/**
	 * \brief Get the NS Prefix being used for DSIG 1.1 elements.
	 *
	 * @returns A pointer to the buffer holding the prefix
	 * @see #setDSIG11NSPrefix
	 *
	 */

	const XMLCh * getDSIG11NSPrefix() const {return mp_11PrefixNS;}

    /**
	 * \brief Get the NS being used for EC nodes
	 *
	 * @returns A pointer to the buffer holding the prefix
	 * @see #setECNSPrefix
	 */

	const XMLCh * getECNSPrefix() const {return mp_ecPrefixNS;}

	/**
	 * \brief Get the NS being used for XPath Filter2 nodes
	 *
	 * @returns A pointer to the buffer holding the prefix
	 * @see #setXPFNSPrefix
	 */

	const XMLCh * getXPFNSPrefix() const {return mp_xpfPrefixNS;}

	/**
	 * \brief Get namespace prefix for XENC nodes
	 *
	 * Find the string being used by the library to prefix nodes in the 
	 * xenc: namespace.
	 *
	 * @returns XENC namespace prefix
	 */

	const XMLCh * getXENCNSPrefix(void) const {return mp_xencPrefixNS;}

	/**
	 * \brief Get the NS Prefix being used for XENC 1.1 elements.
	 *
	 * @returns A pointer to the buffer holding the prefix
	 * @see #setXENC11NSPrefix
	 *
	 */

	const XMLCh * getXENC11NSPrefix() const {return mp_xenc11PrefixNS;}

	/**
	 * \brief Get namespace prefix for XKMS nodes
	 *
	 * Find the string being used by the library to prefix nodes in the 
	 * xkms: namespace.
	 *
	 * @returns XKMS namespace prefix
	 */

	const XMLCh * getXKMSNSPrefix(void) const {return mp_xkmsPrefixNS;}
	
	//@}

	/** @name Pretty Printing Functions */
	//@{

	/**
	 * \brief Set Pretty Print flag 
	 *
	 * The pretty print flag controls whether the library will output
	 * CR/LF after the elements it adds to a document
	 *
	 * By default the library will do pretty printing (flag is true)
	 *
	 * @param flag Value to set the flag (true = do pretty printing)
	 */

	void setPrettyPrintFlag(bool flag) {m_prettyPrintFlag = flag;}

	/**
	 * \brief Return the current value of the PrettyPrint flag
	 *
	 * @returns The value of the pretty print flag
	 */

	bool getPrettyPrintFlag(void) const {return m_prettyPrintFlag;}

	/**
	 * \brief Do a pretty print output
	 *
	 * The library calls this function to perform CR/LF outputting
	 *
	 * At the moment htis is really redundant, but it is more a holder
	 * so that we can set up something in the library to allow users
	 * to install a pretty print function.
	 * 
	 * @param node Node to append pretty print content to
	 */

	void doPrettyPrint(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode * node) const;

	//@}
	
	/** @name General information functions */
	//@{

	/**
	 * \brief
	 *
	 * Get the DOMDocument that the super class is operating within.
	 *
	 * Mainly used by the library itself.
	 *
	 * @returns The DOM_Document node.
	 */

	XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument * getParentDocument() const
		{return mp_doc;}

	/**
	 * \brief
	 *
	 * Set the DOMDocument that the super class is operating within.
	 *
	 * Mainly used by the library itself.
	 *
	 * @param doc The Document node.
	 */

	void setParentDocument(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument * doc)
		{mp_doc = doc;}

	//@}

	/** @name Resolver manipulation */
	//@{

	/**
	 * \brief Register a URIResolver 
	 *
	 * Registers a URIResolver to be used by the Signature when dereferencing
	 * a URI in a Reference element
	 *
	 */

	void setURIResolver(XSECURIResolver * resolver);

	/**
	 * \brief Return a pointer to the resolver being used
	 *
	 * @returns A pointer to the URIResolver registered in this signature
	 */

	XSECURIResolver * getURIResolver(void) const;


	//@}

	/** @name ID handling */
	
	//@{

	/**
	 * \brief Set Id finding behaviour
	 *
	 * The library de-references "#obj" URI references to ID attributes within
	 * a DOM document.  Currently, the library first uses DOM calls to find if
	 * the Id has been properly set within the document via the parser or one
	 * of the DOM Level 3 calls to set an Id.
	 *
	 * If no Id is found of the correct name, the library then starts searching
	 * for attributes of a given name with the required value.  This list defaults
	 * to "id" and "Id", but can be modified via a call to addIdAttributeName()
	 *
	 * The setIdByAttributeName call enables or disables the second part of the Id
	 * search.  I.e. when the Id doesn't exist as an attribute of Type=ID, whether or
	 * not to search for an attribute of a name in the list of names.  By default
	 * this behaviour is enabled.
	 *
	 * @warning This is currently enabled by default for backwards compatibility
	 * reasons only.  Future version may reverse this and ship disabled by default, as
	 * this behaviour is a potential security risk.
	 *
	 * @param flag Enable (true) or Disable (false) searching for Id attributes by name
	 */

	void setIdByAttributeName(bool flag);

	/**
	 * \brief Determine Id finding behaviour
	 *
	 * Allows a caller to determine whether the library is currently searching for
	 * Id attributes by name
	 *
	 * @returns The value of the IdByAttributeName flag
	 */

	bool getIdByAttributeName(void) const;

	/**
	 * \brief Add an attribute name to be searched for when looking for Id attributes
	 *
	 * This allows a user to add an attribute name to be used to identify Id attributes
	 * when they are not set to be of Type=ID in the DOM
	 *
	 * @note Two names are registered by default - "Id" and "id".  These can be
	 * removed by calling deregisterIdAttributeName
	 *
	 * @param name Name to append to the list of those used to find Id attributes
	 */

	void registerIdAttributeName(const XMLCh * name);

	/**
	 * \brief Remove an attribute name to be searched for when looking for Id attributes
	 *
	 * This allows a user to de-register a particular name to be used to identify Id
	 * attributes.
	 *
	 * @param name Name to remove from the list of those used to find Id attributes
	 * @returns true if found and removed, false if was not in the list
	 */

	bool deregisterIdAttributeName(const XMLCh * name);

	/**
	 * \brief Determine if an attribute name is registered as an Id name
	 *
	 * @param name String to check in the idAttributeName list
	 * @returns true if the passed in name is registered as an Attribute name
	 */

	bool isRegisteredIdAttributeName(const XMLCh * name) const;

	/**
	 * \brief Add an attribute name and namespace to be searched for when looking for 
	 * Id attributes
	 *
	 * This allows a user to add an attribute name in a parcicular namespace to 
	 * be used to identify Id attributes
	 * when they are not set to be of Type=ID in the DOM
	 *
	 * @note Two names are registered by default - "Id" and "id".  These can be
	 * removed by calling deregisterIdAttributeName
	 *
	 * @param ns Namespace URI in which attribute appears
	 * @param name Name to append to the list of those used to find Id attributes
	 */

	void registerIdAttributeNameNS(const XMLCh * ns, const XMLCh * name);

	/**
	 * \brief Remove an attribute name and namespace to be searched for when 
	 * looking for Id attributes
	 *
	 * This allows a user to de-register a particular name to be used to identify Id
	 * attributes.
	 *
	 * @param ns Namespace in which attribute resides
	 * @param name Name to remove from the list of those used to find Id attributes
	 * @returns true if found and removed, false if was not in the list
	 */

	bool deregisterIdAttributeNameNS(const XMLCh *ns, const XMLCh * name);

	/**
	 * \brief Determine if an attribute name and namespace is registered 
	 * as an Id name
	 *
	 * @param ns Namespace in which attribute resides
	 * @param name String to check in the idAttributeName list
	 * @returns true if the passed in name is registered as an Attribute name
	 */

	bool isRegisteredIdAttributeNameNS(const XMLCh * ns, const XMLCh * name) const;

	/**
	 * \brief Get number of Attribute Names registered as Id attributes
	 *
	 * @returns the number of elements in the list
	 */

	int getIdAttributeNameListSize() const;

	/*
	 * \brief Get an indexed attribute name to use as an Id
	 *
	 * Returns the item at index point in the list
	 *
	 * @note This is an internal function and should not be called directly
	 *
	 * @param index Pointer into the list
	 * @returns The indicated element or NULL if it does not exist.
	 */

	const XMLCh * getIdAttributeNameListItem(int index) const;

	/*
	 * \brief Get an indexed attribute Namespace to use as an Id
	 *
	 * Returns the item at index point in the list
	 *
	 * @note This is an internal function and should not be called directly
	 *
	 * @param index Pointer into the list
	 * @returns The indicated element or NULL if it does not exist.
	 */

	const XMLCh * getIdAttributeNameListItemNS(int index) const;

	/*
	 * \brief Determine whether the indexed item is namespace aware
	 *
	 * Returns the item at index point in the list
	 *
	 * @note This is an internal function and should not be called directly
	 *
	 * @param index Pointer into the list
	 * @returns The indicated element or NULL if it does not exist.
	 */

	bool getIdAttributeNameListItemIsNS(int index) const;

	//@}
	
	/** @name Formatters */
	//@{

	/** 
	 * \brief Get a safeBufferFormatter
	 *
	 * Return a UTF-8 safeBuffer formatter
	 *
	 * @returns A pointer to a safeBuffer formatter
	 */

	XSECSafeBufferFormatter * getSBFormatter(void) const {return mp_formatter;}

	//@}

private:

	struct IdAttributeStruct;
	typedef struct IdAttributeStruct IdAttributeType;


#if defined(XSEC_NO_NAMESPACES)
	typedef vector<IdAttributeType *>		IdNameVectorType;
#else
	typedef std::vector<IdAttributeType *>	IdNameVectorType;
#endif

	// Internal functions

	XSECSafeBufferFormatter		* mp_formatter;
	XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument					
								* mp_doc;
	// For creating functions
	XMLCh						* mp_prefixNS;
	XMLCh						* mp_11PrefixNS;
	XMLCh						* mp_ecPrefixNS;
	XMLCh						* mp_xpfPrefixNS;
	XMLCh						* mp_xencPrefixNS;
	XMLCh						* mp_xenc11PrefixNS;
	XMLCh						* mp_xkmsPrefixNS;

	// Resolvers
	XSECURIResolver				* mp_URIResolver;

	// Flags
	bool						m_prettyPrintFlag;
	bool						m_idByAttributeNameFlag;

	// Id handling
	IdNameVectorType			m_idAttributeNameList;	

	XSECEnv();

	/*\@}*/
};

#endif /* XSECENV_INCLUDE */
