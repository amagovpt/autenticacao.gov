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
 * XENCCipher := Interface definition for main encryption worker class
 *
 * $Id: XENCCipher.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef XENCCIPHER_INCLUDE
#define XENCCIPHER_INCLUDE

// XSEC Includes

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/xenc/XENCCipherData.hpp>
#include <xsec/dsig/DSIGConstants.hpp>

// Xerces

XSEC_DECLARE_XERCES_CLASS(DOMElement);
XSEC_DECLARE_XERCES_CLASS(DOMDocument);
XSEC_DECLARE_XERCES_CLASS(BinInputStream);

class XSECCryptoKey;
class XENCEncryptedData;
class XENCEncryptedKey;
class XSECKeyInfoResolver;
class XSECBinTXFMInputStream;
class TXFMChain;

/**
 * @defgroup xenc XML Encryption Implementation
 *
 * <p>The classes in this group implement the XML Encryption
 * standard.  In most cases, users should interact with these
 * functions via the XENCCipher class</p>
 *
 *\@{*/



/**
 * @brief Main worker class for the XSEC implementation of XML
 * Encryption.
 *
 * The XENCCipher class not something that is directly defined in
 * the XML Encryption standard.  It is a control class used by the
 * library to generate encrypted XML information and to decrypt
 * information held in XML Encryption structures.
 *
 * All encryption and decryption work performed by the library is
 * handled within this class.  The other XENC classes simply
 * handle marshalling and unmarshalling of the DOM data.
 *
 */


class XENCCipher {

public:
	
	/** @name Constructors and Destructors */
	//@{
	
	virtual ~XENCCipher() {};

	//@}

	/** @name Decryption Functions */
	//@{

	/**
	 * \brief Decrypt the nominated element.
	 *
	 * Decrypts the passed in element, which must be the root
	 * node of a \<EncryptedData\> method with a type of "#Element".
	 * If not, the library will throw an XSECException exception.
	 *
	 * This is an "all in one method".  The library will replace
	 * the passed in Element (i.e. the encrypted XML data) with
	 * the resultant plain text, after it has been parsed back into
	 * DOM nodes
	 *
	 * @param element Root of EncryptedData DOM structyre to decrypt
	 * @returns The owning document with the element replaced, or NULL
	 * if the decryption fails for some reason (normally an exception).
	 * @throws XSECException if the decryption fails, or if this is
	 * not a valid EncryptedData DOM structure.
	 */

	virtual XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument * decryptElement(
		XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * element
	) = 0;

	/**
	 * \brief Decrypt the nominated element without replacing it.
	 *
	 * Decrypts the passed in element, which must be the root
	 * node of a \<EncryptedData\> method with a type of "#Element".
	 * If not, the library will throw an XSECException exception.  Rather
	 * than replacing the element with the decrypted content, the
	 * result is passed to the caller as an orphaned document fragment.
	 *
	 * @param element Root of EncryptedData DOM structyre to decrypt
	 * @returns The document fragment containing the decrypted node-set.
	 * @throws XSECException if the decryption fails, or if this is
	 * not a valid EncryptedData DOM structure.
	 */

	virtual XERCES_CPP_NAMESPACE_QUALIFIER DOMNode * decryptElementDetached(
		XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * element
	) = 0;

	/**
	 * \brief Decrypt currently loaded element.
	 *
	 * Decrypts the an element that was previously passed in via 
	 * loadEncryptedData with a type of "#Element".
	 * If not, the library will throw an XSECException exception.
	 *
	 * This is an "all in one method".  The library will replace
	 * the passed in Element (i.e. the encrypted XML data) with
	 * the resultant plain text, after it has been parsed back into
	 * DOM nodes
	 *
	 * @param element Root of EncryptedData DOM structyre to decrypt
	 * @returns The owning document with the element replaced, or NULL
	 * if the decryption fails for some reason (normally an exception).
	 * @throws XSECException if the decryption fails, or if this is
	 * not a valid EncryptedData DOM structure.
	 */

	virtual XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument * decryptElement(void) = 0;

	/**
	 * \brief Decrypt currently loaded element without replacing it.
	 *
	 * Decrypts the an element that was previously passed in via 
	 * loadEncryptedData with a type of "#Element".
	 * If not, the library will throw an XSECException exception.
	 *
	 * This does not replace the currently existing DOM document.  It returns
	 * an "orphaned" document fragment containing the serialised version of the
	 * decrypted data.
	 *
	 * @param element Root of EncryptedData DOM structyre to decrypt
	 * @returns The owning document with the element replaced, or NULL
	 * if the decryption fails for some reason (normally an exception).
	 * @throws XSECException if the decryption fails, or if this is
	 * not a valid EncryptedData DOM structure.
	 */

	virtual XERCES_CPP_NAMESPACE_QUALIFIER DOMNode * decryptElementDetached(void) = 0;
	
	/**
	 * \brief Decrypt the nominated element and put the output to an InputStream.
	 *
	 * Decrypts the passed in element, which must be the root
	 * node of a \<EncryptedData\> method.
	 *
	 * This call does not change the source DOM in any way.  It simply
	 * processes the encrypted data and provides an InputStream that the
	 * caller can read from to read the plain text data.
	 *
	 * @param element Root of EncryptedData DOM structyre to decrypt
	 * @returns A BinInputStream object that the application can use to
	 * read the decrypted data.
	 * @throws XSECException if the decryption fails, or if this is
	 * not a valid EncryptedData DOM structure.
	 */

	virtual XSECBinTXFMInputStream * decryptToBinInputStream(
		XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * element
	) = 0;

	/**
	 * \brief Decrypt a key
	 *
	 * Reads in the passed in KeyInfo structure for an EncryptedKey and 
	 * decrypts the key to a buffer.
	 *
	 * @param encryptedKey the already loaded encryptedKey structure
	 * @param rawKey Buffer to place the decrypted key into
	 * @param maxKeySize Maximum number of bytes to place in the buffer
	 */

	virtual int decryptKey(
		XENCEncryptedKey * encryptedKey,
		XMLByte * rawKey,
		int maxKeySize
	) = 0;

	/**
	 * \brief Decrypt a key directly from DOM
	 *
	 * Loads an EncryptedKey from DOM and then decrypts the key.
	 * If a NULL buffer is passed in, will simply load the key and return
	 *
	 * @param keyNode Node to load from
	 * @param rawKey Buffer to decrypt to
	 * @param maxKeySize Length of rawKey buffer
	 * @returns The number of bytes decrypted
	 */

	virtual int decryptKey(
		XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * keyNode,
		XMLByte * rawKey,
		int maxKeySize
	) = 0;

	//@}

	/** @name Encryption Functions */
	//@{

	/**
	 * \brief Encrypt the nominated element.
	 * 
	 * Encrypts the passed in element and all children.  The element
	 * is replaced with an EncryptedData element
	 *
	 * @param element Element (and children) to encrypt
	 * @param em The encryptionMethod to use for this encryption.  Use
	 * ENCRYPT_NONE if a user defined type is required.
	 * @param algorithmURI If ENCRYPT_NONE is passed in, this will be
	 * used to set the algorithm URI.  If this is also NULL - no
	 * EncryptionMethod will be set.  <b>NULL Value Unsupported if em not
	 * set!  It's use could cause problems!</b>
	 *
	 * @returns The owning document with the element replaced, or NULL
	 * if the decryption fails for some reason (normally an exception).
	 * @throws XSECException if the encryption fails.
	 */

	virtual XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument * encryptElement(
		XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * element,
		encryptionMethod em,
		const XMLCh * algorithmURI = NULL
	) = 0;

	/**
	 * \brief Encrypt the nominated element without affecting the current document.
	 * 
	 * Encrypts the passed in element and all children.  The element
	 * is not replaced - the return node is an "orphaned" subtree from
	 * the passed in document and the original document is untouched.
	 *
	 * @param element Element (and children) to encrypt
	 * @param em The encryptionMethod to use for this encryption.  Use
	 * ENCRYPT_NONE if a user defined type is required.
	 * @param algorithmURI If ENCRYPT_NONE is passed in, this will be
	 * used to set the algorithm URI.  If this is also NULL - no
	 * EncryptionMethod will be set.  <b>NULL Value Unsupported if em not
	 * set!  It's use could cause problems!</b>
	 *
	 * @returns The resulting document fragment containing the encrypted data.
	 * @throws XSECException if the encryption fails.
	 */

	virtual XERCES_CPP_NAMESPACE_QUALIFIER DOMNode * encryptElementDetached(
		XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * element,
		encryptionMethod em,
		const XMLCh * algorithmURI = NULL
	) = 0;
	
	/**
	 * \brief Encrypt the children of the nominated element
	 * 
	 * Encrypts the all children of the passed in element, but
	 * leaves the element itself in place, with one new child - an
	 * EncryptedData node of type #content
	 *
	 * @param element Element whose children are to be encrypted
	 * @param em The encryptionMethod to use for this encryption.  Use
	 * ENCRYPT_NONE if a user defined type is required.
	 * @param algorithmURI If ENCRYPT_NONE is passed in, this will be
	 * used to set the algorithm URI.  If this is also NULL - no
	 * EncryptionMethod will be set.  <b>NULL Value Unsupported if em not
	 * set!  It's use could cause problems!</b>
	 *
	 * @returns The owning document with the element's children replaced, or NULL
	 * if the decryption fails for some reason (normally an exception).
	 * @throws XSECException if the encryption fails.
	 */

	virtual XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument * encryptElementContent(
		XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * element,
		encryptionMethod em,
		const XMLCh * algorithmURI = NULL
	) = 0;

	/**
	 * \brief Encrypt the children of the nominated element
	 * 
	 * Encrypts the all children of the passed in element.  The input
	 * DOM node set is untouched, but the function returns an orphaned
	 * sub-tree owned by the passed in document containing the encrypted
	 * data.
	 *
	 * @param element Element whose children are to be encrypted
	 * @param em The encryptionMethod to use for this encryption.  Use
	 * ENCRYPT_NONE if a user defined type is required.
	 * @param algorithmURI If ENCRYPT_NONE is passed in, this will be
	 * used to set the algorithm URI.  If this is also NULL - no
	 * EncryptionMethod will be set.  <b>NULL Value Unsupported if em not
	 * set!  It's use could cause problems!</b>
	 *
	 * @returns The resulting (orphaned) sub-tree from the passed in document
	 * containing the encrypted data.
	 * @throws XSECException if the encryption fails.
	 */

	virtual XERCES_CPP_NAMESPACE_QUALIFIER DOMNode * encryptElementContentDetached(
		XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * element,
		encryptionMethod em,
		const XMLCh * algorithmURI = NULL
	) = 0;
	
	/**
	 * \brief Encrypt a buffer of data as a key
	 *
	 * Encrypts the passed in data and creates an EncryptedKey element
	 *
	 * @param keyBuffer The key data to encrypt
	 * @param keyLen Bytes to encrypt
	 * @param em The encryptionMethod to use for this encryption.  Use
	 * ENCRYPT_NONE if a user defined type is required.
	 * @param algorithmURI If ENCRYPT_NONE is used for em, this will be
	 * used as the algorithm URI.
	 *
	 * @returns The EncryptedKey element
	 */

	virtual XENCEncryptedKey * encryptKey(
		const unsigned char * keyBuffer,
		unsigned int keyLen,
		encryptionMethod em,
		const XMLCh * algorithmURI = NULL
	) = 0;

	/**
	 * \brief Encrypt an input stream to a CipherValue
	 *
	 * Encrypts the data passed in via a Xerces BinInputStream and places it
	 * directly into a new EncryptedData element that contains a CipherValue
	 *
	 * @param plainText The InputStream to read the plain text from
	 * @param em The encryptionMethod to use for this encryption.  Use
	 * ENCRYPT_NONE if a user defined type is required.
	 * @param algorithmURI if ENCRYPT_NONE is used for em, this will be used
	 * as the algorithm URI
	 *
	 * @returns the EncryptedData element containing the CipherValue of the data
	 */

	virtual XENCEncryptedData * encryptBinInputStream(
		XERCES_CPP_NAMESPACE_QUALIFIER BinInputStream * plainText,
		encryptionMethod em,
		const XMLCh * algorithmURI = NULL
	) = 0;

	/**
	 * \brief Encrypt a TXFMChain to a CipherValue
	 *
	 * Encrypts the data passed in via a TXFMChain and places it
	 * directly into a new EncryptedData element that contains a CipherValue.
	 *
	 * @note This is not really intended for client apps, but is used internally
	 * and is provided for flexibility.  The "formal" method is encryptBinInputStream
	 *
	 * @param plainText The TXFMChain to read the plain text from
	 * @param em The encryptionMethod to use for this encryption.  Use
	 * ENCRYPT_NONE if a user defined type is required.
	 * @param algorithmURI if ENCRYPT_NONE is used for em, this will be used
	 * as the algorithm URI
	 *
	 * @returns the EncryptedData element containing the CipherValue of the data
	 */

	virtual XENCEncryptedData * encryptTXFMChain(
		TXFMChain * plainText,
		encryptionMethod em,
		const XMLCh * algorithmURI = NULL
	) = 0;

	//@}
	/** @name Getter Functions */
	//@{

	/**
	 * \brief Get owning document
	 *
	 * Every Cipher object is associated with an owning document (for generation of
	 * nodes etc.)  This allows callers to retrieve this value.
	 *
	 * @returns The DOMDocument that is used by this object
	 */

	virtual XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument * getDocument(void) = 0;

	/**
	 * \brief Get namespace prefix for XENC nodes
	 *
	 * Find the string being used by the library to prefix nodes in the 
	 * xenc: namespace.
	 *
	 * @returns XENC namespace prefix
	 */

	virtual const XMLCh * getXENCNSPrefix(void) const = 0;

	/**
	 * \brief Get the EncryptedData element
	 *
	 * Allows the user to get the EncryptedData element that was last processed/
	 * created by this XENCCipher object.
	 *
	 * @returns The last used EncryptedData
	 */

	virtual XENCEncryptedData * getEncryptedData(void) = 0;

	/**
	 * \brief Tell caller whether PrettyPrinting is active
	 *
	 * @returns True if Pretty Printing is active, false if not
	 */

	virtual bool getPrettyPrint(void) = 0;

	/**
	 * \brief Tell caller whether the serialisation routines will
	 * use exclusive or normal canonicalisation.
	 *
	 * When serialising an element prior to encryption, the c14n
	 * canonicalisation routines are used.  By default, exclusive
	 * c14n is used, however this can be turned off using the
	 * setExclusiveC14nSerialisation call.  This function returns 
	 * the current state of the associated flag.
	 *
	 * @returns True if Exclusive c14n will be used, false if standard
	 */

	virtual bool getExclusiveC14nSerialisation(void) = 0;

	//@}

	/** @name Setter Functions */
	//@{

	/**
	 * \brief Set decryption key for next operation
	 *
	 * Set the passed in key for the next decryption/encryption
	 * operation.
	 *
	 * @param key Key to use
	 * @note This function will take ownership of the key and delete it when done.
	 */

	virtual void setKey(XSECCryptoKey * key) = 0;

	/**
	 * \brief Set Key Encryption Key for next operation
	 *
	 * Set the passed in key for the next key decryption/encryption
	 * operation.
	 *
	 * @note This key will only be used to decrypt EncryptedKey elements.
	 * To set a key for decrypting an EncryptedData use #setKey instead.
	 *
	 * @param key Key to use
	 * @note This function will take ownership of the key and delete it when done.
	 */

	virtual void setKEK(XSECCryptoKey * key) = 0;

	/**
	 * \brief Register a KeyInfoResolver 
	 *
	 * Registers a KeyInfoResolver to be used by the cipher when 
	 * it needs to find a key to be used to decrypt some ciper text
	 *
	 * @note The library will use the #clone() function from the resolver
	 * to get a copy.  The passed in resolver remains the property of
	 * the calling function
	 *
	 * @param resolver Resolver to clone and use for resolving keys
	 *
	 */

	virtual void setKeyInfoResolver(const XSECKeyInfoResolver * resolver) = 0;

	/**
	 * \brief Set prefix for XENC nodes
	 *
	 * Set the namespace prefix the library will use when creating
	 * nodes in the XENC namespace
	 */

	virtual void setXENCNSPrefix(const XMLCh * prefix) = 0;

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

	virtual void setPrettyPrint(bool flag) = 0;

	/**
	 * \brief Set whether the serialisation routines will
	 * use exclusive or normal canonicalisation.
	 *
	 * When serialising an element prior to encryption, the c14n
	 * canonicalisation routines are used.  By default, exclusive
	 * c14n is used, however this can be turned off using the
	 * setExclusiveC14nSerialisation call.
	 *
	 * @param flag Set for true if Exclusive c14n will be used, 
	 * false otherwise
	 */

	virtual void setExclusiveC14nSerialisation(bool flag) = 0;

	//@}

	/** @name Creation and loading Functions */
	//@{

	/**
	 * \brief Create a new EncryptedData element
	 *
	 * Method for creating a basic Encrypted Data element.  Can be used
	 * in cases where an application needs to build this from scratch.
	 *
	 * In general, applications should use the higher level methods such
	 * as #encryptElement or #encryptElementContent.
	 *
	 * @note The Cipher object will take on this new object as the current
	 * EncryptedData and delete any currently being held.
	 *
	 * @param type Should this set up a CipherReference or a CipherValue
	 * @param algorithm URI string to use for the Algorithm attribute in EncryptionMethod.
	 * Set to NULL for no defined algorithm.
	 * @param value String to set the cipher data to if the type is VALUE_TYPE.
	 * for REFERENCE_TYPE CipherData elements, this should be the URI value.
	 * @returns An XENCEncryptedData object
	 */

	virtual XENCEncryptedData * createEncryptedData(XENCCipherData::XENCCipherDataType type,
													const XMLCh * algorithm,
													const XMLCh * value) = 0;

	/**
	 * \brief Load an EncryptedKey element
	 *
	 * Take a passed in EncryptedKey DOMNode and return a loaded XENCEncryptedKey
	 * object based on the DOMNode from the passed in element.
	 *
	 * @param keyNode Element node to load EncryptedKey from
	 * @returns An XENCEncryptedKey structure (owned by the caller) based on the 
	 * node.
	 */

	virtual XENCEncryptedKey * loadEncryptedKey(
		XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * keyNode
		) = 0;

	/**
	 * \brief Load an EncryptedData element
	 *
	 * Take a passed in EncryptedData DOMNode and return a loaded XENCEncryptedData
	 * object based on the DOMNode from the passed in element.
	 *
	 * @note The Cipher object will take on this new object as the current
	 * EncryptedData and delete any currently being held.
	 *
	 * @param dataNode Element node to load EncryptedData from
	 * @returns An XENCEncryptedData structure (owned by the caller) based on the 
	 * node.
	 */

	virtual XENCEncryptedData * loadEncryptedData(
		XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * dataNode
		) = 0;


	//@}

};

/*\@}*/

#endif /* XENCCIPHER_INCLUDE */

