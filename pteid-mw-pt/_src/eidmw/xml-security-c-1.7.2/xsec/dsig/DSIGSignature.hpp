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
 * $Id: DSIGSignature.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef DSIGSIGNATURE_INCLUDE
#define DSIGSIGNATURE_INCLUDE

// XSEC Includes
#include <xsec/utils/XSECSafeBufferFormatter.hpp>
#include <xsec/dsig/DSIGTransform.hpp>
#include <xsec/dsig/DSIGKeyInfoList.hpp>
#include <xsec/dsig/DSIGConstants.hpp>
#include <xsec/dsig/DSIGSignedInfo.hpp>

// Xerces Includes

#include <xercesc/dom/DOM.hpp>

class XSECEnv;
class XSECBinTXFMInputStream;
class XSECURIResolver;
class XSECKeyInfoResolver;
class DSIGKeyInfoValue;
class DSIGKeyInfoX509;
class DSIGKeyInfoName;
class DSIGKeyInfoPGPData;
class DSIGKeyInfoSPKIData;
class DSIGKeyInfoMgmtData;
class DSIGObject;

/**
 * @ingroup pubsig
 */
/*\@{*/

/**
 * @brief The main class used for manipulating XML Digital Signatures.
 *
 * <p>The DSIGSignature class is used to manipulate and verify
 * \<signature\> blocks.  It should only ever be created via the 
 * XSECProvider class.</p>
 *
 */

class DSIG_EXPORT DSIGSignature {

protected:

    /** @name Constructors and Destructors */
    //@{
	
    /**
	 * \brief Contructor for use with existing XML signatures or templates.
	 *
	 * <p>Create a DSIGSignature object based on an already existing
	 * DSIG Signature XML node.  It is assumed that the underlying
	 * DOM structure is in place and works correctly.</p>
	 *
	 * <p>It is required that the caller pass in the signature DOM Node
	 * as there may be more than one signature in a document.  The caller
	 * needs to specify which signature tree is to be used.</p>
	 *
	 * @param doc The DOM document node in which the signature is embedded.
	 * @param sigNode The DOM node (within doc) that is to be used as the base of the signature.
	 * @see #load
	 */

	DSIGSignature(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc, 
				  XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *sigNode);

	~DSIGSignature();
    //@}
	
public:

	/** @name Load and Setup Functions */
	//@{

	/**
	  * \brief Load the signature information from the DOM source.
	  *
	  * Used to tell the DSIGSignature object to read from the DOM tree
	  * into local structures.  Will throw various exceptions if it finds that
	  * the DOM structure is not in line with the XML Signature standard.
	  *
	  */

	void load(void);

	/**
	  * \brief Externally set the signing/verification key
	  *
	  * Used prior to a verify or sign operation to set the signature key (public or
	  * private respectively) to be used for the operation.
	  *
	  * @note Once passed in via this call, the key is owned by the Signature.  It will
	  * deleted when a new key is loaded or the signature is released.
	  *
	  * @see #verify
	  * @see #sign
	  */

	void setSigningKey(XSECCryptoKey *k);

	//@}

	/** @name Signature Operations */
	//@{

	/**
	  * \brief Verify that a signature is valid.
	  *
	  * <p>The <I>verify</I> function will validate the signature of an XML document
	  * previously loaded into the DSIGSignature structure via a <I>load</I>.</p>
	  *
	  * <p>It performs the following operations : </p>
	  * <ul>
	  *		<li>Iterate through each reference and validate the hash;
	  *		<li>Iterate through references contained in \<manifest\> elements;
	  *		<li>Calculate the hash of the \<SignedInfo\> element; and
	  *		<li>Validate the signature of the hash previously calculated.
	  * </ul>
	  * 
	  * @returns true/false
	  *		<ul>
	  *        <li><b>true</b> = Signature (and all references) validated correctly.
	  *		   <li><b>false</b> = Signature validation failed.  An error list can be found via a
	  *          call to #getErrMsgs.
	  *		</ul>
	  * @see #load
	  * @see #getErrMsgs
	  */

	bool verify(void);

	/**
	  * \brief Verify a signature is valid (skip references).
	  *
	  * <p>This function is almost the same as #verify except it will skip the
	  * reference checks.
	  *
	  * @see #load
	  * @see #verify
	  */

	bool verifySignatureOnly(void);

	void signExternal(XMLByte *signature, unsigned int siglen);

	/**
	  * \brief Sign a DSIGSignature DOM structure.
	  *
	  * <p>The #sign function will create the reference hash values and signature
	  * value in a DOM structure previously created via a #load or #createBlankSignature
	  * call </p>
	  *
	  * <p>It performs the following operations : </p>
	  * <ul>
	  *		<li>Iterate through each reference, calculate and set the hash value;
	  *		<li>Iterate through references contained in \<manifest\> elements and set their values;
	  *		<li>Calculate the hash of the \<SignedInfo\> element; and
	  *		<li>Calculate (and set) the signature value given the hash previously calculated.
	  * </ul>
	  * 
	  * @note The key to use for signing must have been set prior to call to sign using #setSigningKey
	  *
	  * @throws XSECException (for errors during the XML formatting and loading)
	  * @throws XSECCryptoException (for errors during the cryptographic operations)
	  *
	  * @see #setSigningKey
	  * @see #load
	  * @see #getErrMsgs
	  */

	void sign(void);
	//@}

	/** @name Functions to create and manipulate signature elements. */
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
	  * @param prefix The UTF-16 encoided NS prefix to use for the XML 
	  * Digital Signature nodes
	  */

	void setDSIGNSPrefix(const XMLCh * prefix);

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
	  * @param prefix The UTF-16 encoided NS prefix to use for the XML 
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
	  * @param prefix The UTF-16 encoided NS prefix to use for the XPath
	  * filter nodes
	  */

	void setXPFNSPrefix(const XMLCh * prefix);

	/**
	 * \brief Set Pretty Print
	 *
	 * The pretty print functions controls whether the library will output
	 * CR/LF after the elements it adds to a document
	 *
	 * By default the library will do pretty printing (flag is true)
	 *
	 * @param flag Value to set for Pretty Printing (true = do pretty printing)
	 */

	void setPrettyPrint(bool flag);

	/**
	 * \brief Tell caller whether PrettyPrinting is active
	 *
	 * @returns True if Pretty Printing is active, false if not
	 */

	bool getPrettyPrint(void) const;

	/**
	 * \brief Create a \<Signature\> DOM structure.
	 *
	 * <p>The DOM structure created is still divorced from the document.  The callee
	 * needs to take the returned \<Signature\> Element node and insert it at the
	 * appropriate place in their document.</p>
	 *
	 * <p>The signature is a skeleton only.  There are no references or KeyInfo
	 * elements inserted.  However the DSIGSignature structures are set up with the
	 * new information, so once an element has been created and a signing key has been
	 * set, a call to #sign will sign appropriately.
	 *
	 * @note The digest method (hash method) set here is for the signing function only.
	 * Different hash methods can be used for reference elements.
	 *
	 * @param doc The document the Signature DOM structure will be inserted into.
	 * @param canonicalizationAlgorithmURI The canonicalisation method to use.
	 * @param signatureAlgorithmURI The Signature algorithm (defined by URI) to use
	 * @returns The newly created \<Signature\> element that the caller should insert in
	 * the document.
	 */

	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *createBlankSignature(
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc,
		const XMLCh * canonicalizationAlgorithmURI,
		const XMLCh * signatureAlgorithmURI
	);

	/**
	 * \brief Create a \<Signature\> DOM structure.
	 *
	 * <p>The DOM structure created is still divorced from the document.  The callee
	 * needs to take the returned \<Signature\> Element node and insert it at the
	 * appropriate place in their document.</p>
	 *
	 * <p>The signature is a skeleton only.  There are no references or KeyInfo
	 * elements inserted.  However the DSIGSignature structures are set up with the
	 * new information, so once an element has been created and a signing key has been
	 * set, a call to #sign will sign appropriately.
	 *
	 * @note The digest method (hash method) set here is for the signing function only.
	 * Different hash methods can be used for reference elements.
	 *
	 * @param doc The document the Signature DOM structure will be inserted into.
	 * @param cm The canonicalisation method to use.
	 * @param sm The signature algorithm to be used.
	 * @param hm The Digest function to be used for the actual signatures.
	 * @returns The newly created \<Signature\> element that the caller should insert in
	 * the document.
	 * @deprecated Use the URI based creation method instead, as this supports 
	 * signature algorithms that are not known to the library directly.
	 */

	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *createBlankSignature(
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc,
		canonicalizationMethod cm = CANON_C14N_NOC,
		signatureMethod	sm = SIGNATURE_DSA,
		hashMethod hm = HASH_SHA1
	);

	/**
	 * \brief Add a new reference to the end of the list of \<Reference\> nodes.
	 *
	 * <p>Creates a new DSIGReference, adds it to the list of references handled
	 * by the owning DSIGSignature and also creates the skeleton DOM structure into
	 * the document.</p>
	 *
	 * @note The XSEC Library currently makes very little use of <em>type</em>
	 * attributes in \<Reference\> Elements.  However this may of use to calling
	 * applications.
	 *
	 * @see DSIGReference
	 * @param URI The Data that this Reference node refers to.
	 * @param hm The hashing (digest) method to be used for this reference
	 * @param type A "type" string (as defined in XML Signature).
	 * @returns The newly created DSIGReference element.
	 * @deprecated Use the URI based creation method instead
	 *
	 */

	
	DSIGReference * createReference(
		const XMLCh * URI,
		hashMethod hm = HASH_SHA1, 
		char * type = NULL
	);

	/**
	 * \brief Add a new reference to the end of the list of \<Reference\> nodes.
	 *
	 * <p>Creates a new DSIGReference, adds it to the list of references handled
	 * by the owning DSIGSignature and also creates the skeleton DOM structure into
	 * the document.</p>
	 *
	 * @note The XSEC Library currently makes very little use of <em>type</em>
	 * attributes in \<Reference\> Elements.  However this may of use to calling
	 * applications.
	 *
	 * @see DSIGReference
	 * @param URI The Data that this Reference node refers to.
	 * @param hm The hashing (digest) method to be used for this reference
	 * @param type A "type" string (as defined in XML Signature).
	 * @returns The newly created DSIGReference element.
	 * @deprecated Use the URI based creation method instead
	 *
	 */

	DSIGReference * createReference(
		const XMLCh * URI,
		const XMLCh * hashAlgorithmURI, 
		const XMLCh * type = NULL
	);

    /**
     * \brief Remove a reference from the signature
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

	/** @name General and Information functions. */
	//@{

	/**
	 * \brief Get the hash of the Signed Value
	 * 
	 * Function to calculate and return the hash of the \<SignedInfo\>
	 * structures (after the canonicalization defined by
	 * \<CanonicalizationMethod\> and the reference hashes have been performed).
	 *
	 * @param hashBuf Buffer to place the raw hash in.
	 * @param hashBufLen The length of the buffer
	 * @returns The length of the hash that was placed in hashBuf
	 *
	 */

	unsigned int calculateSignedInfoAndReferenceHash(unsigned char * hashBuf, 
													unsigned int hashBufLen);

	/**
	 * \brief Get the hash of the Signed Value
	 * 
	 * Function to calculate and return the hash of the \<SignedInfo\>
	 * structures (after the canonicalization defined by
	 * \<CanonicalizationMethod\> has been performed).
	 *
	 * @param hashBuf Buffer to place the raw hash in.
	 * @param hashBufLen The length of the buffer
	 * @returns The length of the hash that was placed in hashBuf
	 *
	 */

	unsigned int calculateSignedInfoHash(unsigned char * hashBuf, 
										unsigned int hashBufLen);

	/**
	 * \brief Return the reference list for outside use.
	 *
	 * Returns a pointer to the list of references which can
	 * then be read by the caller.
	 *
	 * @returns The referenceList
	 */

	DSIGReferenceList * getReferenceList(void);

	/**
	 * \brief Return the reference list for outside use.
	 *
	 * Returns a pointer to the list of references which can
	 * then be read by the caller.
	 *
	 * @returns The referenceList
	 */

	const DSIGReferenceList * getReferenceList(void) const;

	/**
	 * \brief Create an input stream from SignedInfo.
	 *
	 * This method allows applications to read the fully canonicalised
	 * byte stream that is hashed and signed.
	 *
	 * All transforms are performed up to the point where they would
	 * normally be fed into the Digest function.
	 *
	 * @returns A BinInputSource of the canonicalised SignedInfo
	 */


	XSECBinTXFMInputStream * makeBinInputStream(void) const;


	/**
	 * \brief Get the Error messages from the last #verify.
	 *
	 * Returns a list of text error messages from the last Signature
	 * operation.  Each error that occurred is in the buffer, separated
	 * by new-lines.
	 *
	 * @note The buffer is owned by the DSIGSignature object - do not delete it
	 *
	 * @returns A pointer to the buffer containing the error strings.
	 *
	 */

	const XMLCh * getErrMsgs(void) const;

	/**
	 * \brief Get the NS Prefix being used for DSIG elements.
	 *
	 * @returns A pointer to the buffer holding the prefix
	 * @see #setDSIGNSPrefix
	 *
	 */

	const XMLCh * getDSIGNSPrefix() const;

	/**
	 * \brief Get the NS being used for EC nodes
	 *
	 * @returns A pointer to the buffer holding the prefix
	 * @see #setECNSPrefix
	 */

	const XMLCh * getECNSPrefix() const;

	/**
	 * \brief Get the NS being used for XPath Filter2 nodes
	 *
	 * @returns A pointer to the buffer holding the prefix
	 * @see #setXPFNSPrefix
	 */

	const XMLCh * getXPFNSPrefix() const;

	/**
	 * \brief
	 *
	 * Get the DOM_Document that this Signature is operating within.
	 *
	 * Mainly used by the library itself.
	 *
	 * @returns The DOM_Document node.
	 */

	XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument * getParentDocument() const
		{return mp_doc;}

	/**
	 * \brief Get canonicalisation algorithm
	 *
	 * Returns the canonicalisation algorithm that will be/is used
	 * to canonicalise the \<SignedInfo\> element prior to hash/sign
	 *
	 * @returns The canonicalisation method
	 */

	canonicalizationMethod getCanonicalizationMethod(void) const
	{return (mp_signedInfo != NULL ? 
			 mp_signedInfo->getCanonicalizationMethod() : CANON_NONE);}


    /**
     * \brief Get the Algorithm URI
     *
     * @returns the URI associated with the Algorithm used to generate
     * the signature
     */

    const XMLCh * getAlgorithmURI() const;

	/**
	 * \brief Get the hash method
	 *
	 * Obtain the hash (digest) algorithm that is used to generate a hash
	 * of the canonicalised \<SignedInfo\> element.
	 *
	 * @returns the Hash (digest) Method
	 */

	hashMethod getHashMethod(void) const
	{return (mp_signedInfo != NULL ? 
			 mp_signedInfo->getHashMethod() : HASH_NONE);}

	/**
	 * \brief Get the signature method
	 *
	 * Obtain the algorithm that will be used to generate/check the signature
	 * of the canonicalised and hashed \<SignedInfo\> element.
	 *
	 * @returns the Signature method
	 */

	signatureMethod getSignatureMethod(void) const
	{return (mp_signedInfo != NULL ? 
			 mp_signedInfo->getSignatureMethod() : SIGNATURE_NONE);}

	/**
	 * \brief Get the Signature Value
	 *
	 * Returns the base64 string holding the signature value for this signature
	 * 
	 * @returns the signature value string
	 */

	const XMLCh * getSignatureValue(void) const;
		 
	/**
	 * \brief Helper function for sub Classes.
	 *
	 * Returns the pointer to the formatter being used within the Signature
	 *
	 */

	XSECSafeBufferFormatter * getSBFormatter(void) {return mp_formatter;}

	/**
	 * \brief Set the interlocking references flag
	 *
	 * This flag controls whether the library will cater for "interlocking"
	 * Reference nodes - where the hash of one reference depends on the
	 * hash of another.
	 *
	 * As of Version 1.3, this is, by default, set to false, as it is 
	 * fairly unusual to find a Reference node that has as an input the
	 * digest value of another reference node.  By setting this value to
	 * true, the library will first calculate all hash values, and then
	 * verify the hash values.  If any are incorrect (caused by one 
	 * the value of one reference being changed *after* it was used as 
	 * the input to another), then the hash values are recalculated.  This
	 * is repeated until the reference list "settles".
	 *
	 * @param flag true for interlocking references, false for standalone
	 */

	void setInterlockingReferences(bool flag) {m_interlockingReferences = flag;}


	/**
	 * \brief Get the interlocking references flag
	 *
	 * This flag controls whether the library will cater for "interlocking"
	 * Reference nodes - where the hash of one reference depends on the
	 * hash of another.
	 *
	 * As of Version 1.3, this is, by default, set to false, as it is 
	 * fairly unusual to find a Reference node that has as an input the
	 * digest value of another reference node.  By setting this value to
	 * true, the library will first calculate all hash values, and then
	 * verify the hash values.  If any are incorrect (caused by one 
	 * the value of one reference being changed *after* it was used as 
	 * the input to another), then the hash values are recalculated.  This
	 * is repeated until the reference list "settles".
	 *
	 * @return Value of flag - true for interlocking references, false for standalone
	 */

	bool getInterlockingReferences(void) const {return m_interlockingReferences;}

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

	/**
	 * \brief Register a KeyInfoResolver 
	 *
	 * Registers a KeyInfoResolver to be used by the Signature when 
	 * it needs to find a key to be used to validate a signature
	 *
	 */

	void setKeyInfoResolver(XSECKeyInfoResolver * resolver);

	/**
	 * \brief Return a pointer to the resolver being used
	 *
	 * @returns A pointer to the KeyInfoResolver registered in this signature
	 */

	XSECKeyInfoResolver * getKeyInfoResolver(void) const;

	//@}

	/** @name KeyInfo Element Manipulation */
	
	//@{

	/**
	 * \brief Get the list of \<KeyInfo\> elements.
	 *
	 * <p>This function recovers list that contains the KeyInfo elements
	 * read in from the DOM document.</p>
	 *
	 * <p>This list should be used by calling applications to determine what key
	 * is appropriate for validating (or even signing) the Signature.</p>
	 *
	 * @todo The KeyInfo process is very primitive.  An interface needs to be
	 * created to allow application developers to install an object into the Signature
	 * that the Signature can call on to translate KeyInfo lists into a Key.
	 * @returns A pointer to the DSIGKeyInfoList object held by the DSIGSignature
	 */
	
	DSIGKeyInfoList * getKeyInfoList() {return &m_keyInfoList;}

	/**
	 * \brief Get the list of \<KeyInfo\> elements.
	 *
	 * <p>This function recovers list that contains the KeyInfo elements
	 * read in from the DOM document.</p>
	 *
	 * <p>This list should be used by calling applications to determine what key
	 * is appropriate for validating (or even signing) the Signature.</p>
	 *
	 * @todo The KeyInfo process is very primitive.  An interface needs to be
	 * created to allow application developers to install an object into the Signature
	 * that the Signature can call on to translate KeyInfo lists into a Key.
	 * @returns A pointer to the DSIGKeyInfoList object held by the DSIGSignature
	 */
	
	const DSIGKeyInfoList * getKeyInfoList() const {return &m_keyInfoList;}

	/**
	 * \brief Clear out all KeyInfo elements in the signature.
	 *
	 * This function will delete all KeyInfo elements from both the DSIGSignature
	 * object <em>and the associated DOM</em>.
	 *
	 */

	void clearKeyInfo(void);

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

	//@}

	/** @name Object handling */
	//@{

	/**
	 * \brief Append an object container
	 *
	 * Create a new Object (i.e. a Signature \<Object\> which is a container
	 * element used to hold information that needs to be signed within the
	 * signature - i.e. in enveloping mode
	 *
	 * @returns the newly created DSIGObject
	 */

	DSIGObject * appendObject(void);

	/**
	 * \brief Find the number of ds:Object nodes within the Signature
	 *
	 * @returns the number of ds:Object nodes held in the Signature, 0 if none
	 */

	int getObjectLength(void) const;

	/**
	 * \brief Get a particular ds:Object from within the Signature
	 *
	 * @returns the ith Object from the list of ds:Object nodes in the signature.
	 * Items are ordered in tree order.
	 */

	DSIGObject * getObjectItem(int i);

	/**
	 * \brief Get a particular ds:Object from within the Signature
	 *
	 * @returns the ith Object from the list of ds:Object nodes in the signature.
	 * Items are ordered in tree order.
	 */

	const DSIGObject * getObjectItem(int i) const;

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
	 * \brief Add an attribute name within a particular Namespace
	 * to be searched for when looking for Id attributes
	 *
	 * This allows a user to add an attribute name to be used to identify Id attributes
	 * when they are not set to be of Type=ID in the DOM
	 *
	 * @note Two names are registered by default - "Id" and "id".  These can be
	 * removed by calling deregisterIdAttributeName
	 *
	 * @param ns Namespace in which attribute will reside
	 * @param name Name to append to the list of those used to find Id attributes
	 */

	void registerIdAttributeNameNS(const XMLCh * ns, const XMLCh * name);

	/**
	 * \brief Remove an attribute name and ns to be searched for when looking for Id attributes
	 *
	 * This allows a user to de-register a particular name to be used to identify Id
	 * attributes.
	 *
	 * @param ns Namespace in which attribute will reside
	 * @param name Name to remove from the list of those used to find Id attributes
	 * @returns true if found and removed, false if was not in the list
	 */

	bool deregisterIdAttributeNameNS(const XMLCh * ns, const XMLCh * name);

	//@}

	friend class XSECProvider;

private:

	// For holding DSIGObject nodes
#if defined(XSEC_NO_NAMESPACES)
	typedef vector<DSIGObject *>			ObjectVectorType;
#else
	typedef std::vector<DSIGObject *>		ObjectVectorType;
#endif


	// Internal functions
	void createKeyInfoElement(void);
	bool verifySignatureOnlyInternal(void);
	TXFMChain * getSignedInfoInput(void);

	// Initialisation
	static void Initialise(void);

	XSECSafeBufferFormatter		* mp_formatter;
	bool						m_loaded;				// Have we already loaded?
	XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument					
								* mp_doc;
	XERCES_CPP_NAMESPACE_QUALIFIER DOMNode						
								* mp_sigNode;
	DSIGSignedInfo				* mp_signedInfo;
	XERCES_CPP_NAMESPACE_QUALIFIER DOMNode						
								* mp_signatureValueNode;
	safeBuffer					m_signatureValueSB;
	DSIGKeyInfoList				m_keyInfoList;
	XERCES_CPP_NAMESPACE_QUALIFIER DOMNode						
								* mp_KeyInfoNode;
	safeBuffer			        m_errStr;

	// Environment
	XSECEnv						* mp_env;
	
	// The signing/verifying key
	mutable XSECCryptoKey		* mp_signingKey;

	// Resolvers
	XSECKeyInfoResolver			* mp_KeyInfoResolver;

	// Objects

	ObjectVectorType			m_objects;

	// Interlocking references
	bool						m_interlockingReferences;

	// Not implemented constructors

	DSIGSignature();
	
	friend class XSECPlatformUtils;

	/*\@}*/
};

#endif /* DSIGSIGNATURE_INCLUDE */
