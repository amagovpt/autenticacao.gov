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
 * DSIG_Reference := Class for checking and setting up reference nodes in a DSIG signature
 *
 * $Id: DSIGReference.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *					 
 */

// High level include
#include <xsec/framework/XSECDefs.hpp>

// Xerces INcludes

#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMNamedNodeMap.hpp>

// XSEC Includes
#include <xsec/utils/XSECSafeBufferFormatter.hpp>
#include <xsec/dsig/DSIGTransform.hpp>
#include <xsec/dsig/DSIGReferenceList.hpp>
#include <xsec/dsig/DSIGConstants.hpp>

class DSIGTransformList;
class DSIGTransformBase64;
class DSIGTransformC14n;
class DSIGTransformEnvelope;
class DSIGTransformXPath;
class DSIGTransformXPathFilter;
class DSIGTransformXSL;
class DSIGSignature;
class DSIGSignedInfo;

class TXFMBase;
class TXFMChain;
class XSECBinTXFMInputStream;
class XSECURIResolver;
class XSECEnv;

/**
 * @ingroup pubsig
 */

/**
 * @brief The class used for manipulating Reference Elements within a signature.
 *
 * <p>The DSIGReference class creates and manipulates (including hashing and validating)
 * \<Reference\> elements.</p>
 *
 */

class DSIG_EXPORT DSIGReference {

public:

    /** @name Constructors and Destructors */
    //@{
	
    /**
	 * \brief Contructor for use with existing XML signatures or templates.
	 *
	 * <p>Create a DSIGReference object based on an already existing
	 * DSIG Reference XML node.  It is assumed that the underlying
	 * DOM structure is in place and works correctly.</p>
	 *
	 * @note DSIGReference structures should only ever be created via calls to a
	 * DSIGSignature object.
	 *
	 * @param env The operating environment in which the Reference is operating
	 * @param dom The DOM node (within doc) that is to be used as the base of the reference.
	 * @see #load
	 * @see DSIGSignature#createReference
	 */

	DSIGReference(const XSECEnv * env, XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *dom);

    /**
	 * \brief Contructor for use when creating new Reference structures.
	 *
	 * <p>Create a DSIGReference object that can later be used to create
	 * a new Reference structure in the DOM document.</p>
	 *
	 * @note DSIGReference structures should only ever be created via calls to a
	 * DSIGSignature object.
	 *
	 * @param env The environment object for this reference.
	 * @see #load
	 * @see DSIGSignature#createReference
	 */
	
	DSIGReference(const XSECEnv * env);

	/**
	 * \brief Destructor.
	 *
	 * @note Does not impact any created DOM structures when destroyed.
	 *
	 * @note DSIGReferences should <em>never</em> be destroyed/deleted by
	 * applications.  They are owned and managed by DSIGSignature structures.
	 */

	~DSIGReference();

	//@}

    /** @name Reference Construction and Manipulation */
    //@{

	/**
	 * \brief Load a DSIGReference from an existing DOM structure.
	 *
	 * <p>This function will load a Reference structure from the owner
	 * document.</p>
	 *
	 */
	
	void load();

	/**
	 * \brief Create a Reference structure in the document.
	 *
	 * <p>This function will create a Reference structure in the owner
	 * document.  In some cases, a call to this function will be sufficient
	 * to put the required Reference in place.  In other cases, calls will
	 * also need to be made to the various append*Transform methods.</p>
	 *
	 * @note The XSEC Library currently makes very little use of <em>type</em>
	 * attributes in \<Reference\> Elements.  However this may of use to calling
	 * applications.
	 *
	 * @param URI The URI (data source) for this reference.  Set to NULL for
	 * an anonymous reference.
	 * @param hm The type of Digest to be used (generally SHA-1)
	 * @param type A type string (as defined by XML Signature).
	 * @returns The root Reference element of the newly created DOM structure.
	 * @deprecated Use the URI based creation method instead
	 */

	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * 
		createBlankReference(const XMLCh * URI, hashMethod hm, char * type);

	/**
	 * \brief Create a Reference structure in the document.
	 *
	 * <p>This function will create a Reference structure in the owner
	 * document.  In some cases, a call to this function will be sufficient
	 * to put the required Reference in place.  In other cases, calls will
	 * also need to be made to the various append*Transform methods.</p>
	 *
	 * @note The XSEC Library currently makes very little use of <em>type</em>
	 * attributes in \<Reference\> Elements.  However this may of use to calling
	 * applications.
	 *
	 * @param URI The URI (data source) for this reference.  Set to NULL for
	 * an anonymous reference.
	 * @param hashAlgorithmURI The type of Digest to be used (generally SHA-1)
	 * @param type A type string (as defined by XML Signature).
	 * @returns The root Reference element of the newly created DOM structure.
	 */

	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * 
		createBlankReference(const XMLCh * URI, 
			const XMLCh * hashAlgorithmURI, 
			const XMLCh * type);

	/**
	 * \brief Append an Enveloped Signature Transform to the Reference.
	 *
	 * Appends a simple enveloped-signature transform to the list of transforms
	 * in this element.
	 *
	 * @returns The newly created envelope transform.
	 *
	 */

	DSIGTransformEnvelope *  appendEnvelopedSignatureTransform();
	
	/**
	 * \brief Append a Base64 Transform to the Reference.
	 *
	 * @returns The newly created Base64 transform.
	 */

	DSIGTransformBase64 * appendBase64Transform();
	
	/**
	 * \brief Append an XPath Transform to the Reference.
	 *
	 * <p> Append an XPath transform.  Namespaces can be added to the 
	 * transform directly using the returned <em>DSIGTransformXPath</em>
	 * structure</p>
	 *
	 * @param expr The XPath expression to be placed in the transform.
	 * @returns The newly created XPath transform
	 */

	DSIGTransformXPath * appendXPathTransform(const char * expr);
	
	/**
	 * \brief Append an XPath-Filter2 Transform to the Reference.
	 *
	 * The returned DSIGTransformXPathFilter will have no actual filter
	 * expressions loaded, but calls can be made to
	 * DSIGTransformXPathFilter::appendTransform to add them.
	 *
	 * @returns The newly created XPath Filter transform
	 */

	DSIGTransformXPathFilter * appendXPathFilterTransform(void);

	/**
	 * \brief Append an XSLT Transform to the Reference.
	 *
	 * <p>The caller must have already create the stylesheet and turned it into
	 * a DOM structure that is passed in as the stylesheet parameter.</p>
	 *
	 * @param stylesheet The stylesheet DOM structure to be placed in the reference.
	 * @returns The newly create XSLT transform
	 */

	DSIGTransformXSL * appendXSLTransform(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *stylesheet);
	
	/**
	 * \brief Append a Canonicalization Transform to the Reference.
	 *
	 * @param canonicalizationAlgorithmURI The type of canonicalisation to be added.
	 * @returns The newly create canonicalisation transform
	 */

	DSIGTransformC14n * appendCanonicalizationTransform(
		const XMLCh * canonicalizationAlgorithmURI
	);

	/**
	 * \brief Append a Canonicalization Transform to the Reference.
	 *
	 * @param cm The type of canonicalisation to be added.
	 * @returns The newly create canonicalisation transform
	 * @deprecated Use the AlgorithmURI based method instead
	 */

	DSIGTransformC14n * appendCanonicalizationTransform(canonicalizationMethod cm);

	/**
	 * \brief Append a "debug" transformer.
	 *
	 * This method allows applications to provide a TXFM that will be appended
	 * to the transform chain just prior to the application of the hash
	 * algorithm.
	 *
	 * @note This is primarily for debugging.  It should not be used to modify the
	 * contents of the byte stream.
	 *
	 * @param t The TXFM element to insert.
	 */
	
	void setPreHashTXFM(TXFMBase * t);

   /**
    * \brief Set the Id attribute of the DSIGReference
    *
    * This method allows applications to set the Id attribute of the DSIGReference
    * as described in http://www.w3.org/TR/xmldsig-core/#sec-Reference.
    *
    *
    * @param id The value for this reference.
    */
   void setId(const XMLCh *id);

   /**
    * \brief Set the Type attribute of the DSIGReference
    *
    * This method allows applications to set the Type attribute of the DSIGReference
    * as described in http://www.w3.org/TR/xmldsig-core/#sec-Reference.
    *
    *
    * @param type The value for this reference.
    */
   void setType(const XMLCh *type);
	//@}

	/** @name Getting Information */
	//@{

	/**
	 * \brief Create an input stream based on the digested byte stream.
	 *
	 * This method allows applications to read the fully canonicalised
	 * byte stream that is hashed for a reference.
	 *
	 * All transforms are performed up to the point where they would
	 * normally be fed into the Digest function.
	 *
	 * @returns A BinInputSource of the canonicalised SignedInfo
	 */

	XSECBinTXFMInputStream * makeBinInputStream(void) const;

	/**
	 * \brief Return the URI string of the Reference.
	 *
	 * @returns A pointer to the buffer (owned by the Reference) containing 
	 * the value of the URI stored inthe reference
	 */

	const XMLCh * getURI();

	/**
	 * \brief Get the Digest method
	 *
	 * Obtain the digest method used to find a hash for this reference
	 *
	 * @returns the hashMethod
	 */

	hashMethod getHashMethod(void) {
		return me_hashMethod;
	}

    /**
     * \brief Get the Digest Algorithm URI
     *
     * @returns the URI associated with the Algorithm used to generate
     * the digest
     */

    const XMLCh * getAlgorithmURI() const {
        return mp_algorithmURI;
    }

	/**
	 * \brief Obtain the transforms for this reference
	 *
	 * Get the DSIGTransformList object for this reference.  Can be used to
	 * obtain information about the transforms and also change the the transforms
	 */

	DSIGTransformList * getTransforms(void) {
		return mp_transformList;
	}

	/**
	 * \brief Determine whether the reference is a manifest
	 *
	 * @returns true iff the Reference element is a Manifest reference
	 */
	
	bool isManifest();

	/**
	 * \brief Get the Manifest
	 *
	 * @returns The ReferenceList containing the references in the Manifest
	 * list of this reference element.
	 */

	DSIGReferenceList * getManifestReferenceList();		// Return list of references for a manifest object


	//@}
	
	/** @name Message Digest/Hash manipulation */
	//@{

	/**
	 * \brief Calculate the Hash  value of a reference
	 *
	 * Takes the Reference URI, performs all the transforms and finally
	 * calculates the Hash value of the data using the Digest algorithm
	 * indicated in the reference
	 *
	 * @param toFill A Buffer that the raw hash will be copied into.
	 * @param maxToFill Maximum number of bytes to place in the buffer
	 * @returns The number of bytes copied into the buffer
	 */

	unsigned int calculateHash(XMLByte * toFill, 
							unsigned int maxToFill);
	void setExternalHash(XMLByte *hash);

	/**
	 * \brief Read the hash from the Reference element.
	 *
	 * Reads the Base64 encoded element from the Reference element.
	 * The hash is then translated from Base64 back into raw form and
	 * written into the indicated buffer.
	 *
	 * @param toFill Pointer to the buffer where the raw hash will be written
	 * @param maxToFill Maximum bytes to write to the buffer
	 * @returns Number of bytes written
	 */
	
	unsigned int readHash(XMLByte *toFill,			
							unsigned int maxToFill);

	/**
	 * \brief Validate the Reference element
	 *
	 * Performs a #calculateHash() and a #readHash() and then compares the
	 * results.
	 *
	 * @returns true iff the hash of the data matches the hash stored 
	 * in the reference.
	 */

	bool checkHash();

	/**
	 * \brief Set the value of the hash in the Reference
	 *
	 * Hashes the data referenced by the element and then writes
	 * the Base64 encoded hash value into the Reference.
	 *
	 */

	void setHash();

	//@}

	/** @name Helper (static) Functions */	
	//@{

	/**
	 * \brief Create a Transformer chain
	 *
	 * Given a TransformList create the corresponding TXFM chain to allow
	 * the caller to read the reference byte stream
	 *
	 * @note This method is primarily for use within the XSEC library.
	 * Users wishing to get the byte stream should use the #makeBinInputStream
	 * method instead.
	 *
	 * @param input The input transformer to which the TXFMs will be applied to
	 * This is generally created from the URI attribute of the reference.
	 * @param lst The list of Transform elements from which to build the 
	 * transformer list.
	 * @returns The <B>end</B> of the newly build TXFM chain.  This can be 
	 * read from using TXFMBase::readBytes() to give the end result of the
	 * transforms.
	 */

	static TXFMChain * createTXFMChainFromList(TXFMBase * input, 
							DSIGTransformList * lst);

	/**
	 * \brief Load a Transforms list from the \<Transforms\> DOMNode.
	 *
	 * Reads the data from the XML data stored in the DOM and create
	 * the associated DSIGTrasnformList.
	 *
	 * @param transformsNode Starting node in the DOM
	 * @param formatter The formatter to be used to move from XMLCh to strings
	 * @param env Environment in which to operate
	 * @returns A pointer to the created list.
	 */

	static DSIGTransformList * loadTransforms(
							XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *transformsNode,
							XSECSafeBufferFormatter * formatter,
							const XSECEnv * env);

	/**
	 * \brief Create a starting point for a TXFM Chain.
	 *
	 * Uses the provided URI to find the base data that the Transformer chain
	 * will be built upon.
	 *
	 * @param doc The document that the signature is based on (used for local URIs)
	 * @param URI The URI to build the base from
	 * @param env The environment the signature is operating in
	 * @returns A base TXFM element.
	 */

	static TXFMBase * getURIBaseTXFM(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument * doc, 
									const XMLCh * URI, 
									const XSECEnv * env);

	/**
	 * \brief Load a series of references.
	 *
	 * Takes a series of \<Reference\> elements in a DOM structure
	 * and creates the corresponding ReferenceList object.
	 *
	 * @note Internal function - meant for use by the library
	 *
	 * @param env The environment in which this reference resides
	 * @param firstReference First reference in DOM structure
	 * @returns the created list.
	 */

	static DSIGReferenceList *loadReferenceListFromXML(const XSECEnv * env, 
													   XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *firstReference);

	/**
	 * \brief Validate a list of references.
	 *
	 * Runs through a reference list, calling verify() on each and 
	 * setting the ErrroStrings for any errors found
	 *
	 * @param lst The list to verify
	 * @param errorStr The string to append any errors found to
	 * @returns true iff all the references validate successfully.
	 */

	static bool verifyReferenceList(DSIGReferenceList * lst, safeBuffer &errorStr);
	
	/**
	 * \brief Hash a reference list
	 * 
	 * Run through a list of references and calculate the hash value of each
	 * element.  Finally set the Base64 encoded string according to the newly
	 * calcuated hash.
	 *
	 * @note This is an internal library function and should not be called directly.
	 *
	 * @param list The list of references
	 * @param interlocking If set to false, the library will assume there
	 * are no inter-related references.  The algorithm for determining this
	 * internally is very primitive and CPU intensive, so this is a method to 
	 * bypass the checks.
	 */
	static void hashReferenceList(DSIGReferenceList * list, bool interlocking = true);

	//@}

private:

	// Internal functions
	void createTransformList(void);
	void addTransform(
		DSIGTransform * txfm, 
		XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * txfmElt
	);


	XSECSafeBufferFormatter		* mp_formatter;
	bool formatterLocal;
	XERCES_CPP_NAMESPACE_QUALIFIER DOMNode						
								* mp_referenceNode;		// Points to start of document where reference node is
	TXFMBase					* mp_preHash;			// To be used pre-hash
	DSIGReferenceList			* mp_manifestList;		// The list of references in a manifest
	const XMLCh					* mp_URI;				// The URI String
	bool						m_isManifest;			// Does this reference a manifest?
	XERCES_CPP_NAMESPACE_QUALIFIER DOMNode						
								* mp_transformsNode;
	hashMethod					me_hashMethod;			// What form of digest?
	XERCES_CPP_NAMESPACE_QUALIFIER DOMNode						
								* mp_hashValueNode;		// Node where the Hash value is stored
	const XSECEnv				* mp_env;
	DSIGTransformList			* mp_transformList;		// List of transforms
	const XMLCh					* mp_algorithmURI;		// Hash algorithm for this reference
	
	bool                        m_loaded;

	DSIGReference();

	/*\@}*/

	friend class DSIGSignedInfo;
};




