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
 * XSECProvider.hpp := Main interface class that applications use to
 *					   get access to Signature and Encryption functions.
 *
 * $Id: XSECProvider.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */
#ifndef XSECPROVIDER_INCLUDE
#define XSECPROVIDER_INCLUDE

#include <xsec/utils/XSECPlatformUtils.hpp>
#include <xsec/dsig/DSIGSignature.hpp>
#include <xsec/xenc/XENCCipher.hpp>
#include <xsec/xkms/XKMSMessageFactory.hpp>

#include <xercesc/util/Mutexes.hpp>

#include <vector>

/**
 * @addtogroup pubsig
 * @{
 */

/**
 * @brief The main provider class for XML Digital Signatures and Encryption objects.
 *
 * <p>The XSECProvider class is used to create and destroy signature objects and
 * encryption objects.  It provides a number of methods to create signature 
 * and encryption objects for a variety of situations - in particular creating an 
 * empty signature or cipher with which to create the DOM structure or creating a 
 * security object based on an already existing DOM structure.</p>
 *
 */


class DSIG_EXPORT XSECProvider {

#if defined(XALAN_NO_NAMESPACES)
	typedef vector<DSIGSignature *>			SignatureListVectorType;
#else
	typedef std::vector<DSIGSignature *>	SignatureListVectorType;
#endif

#if defined(XALAN_NO_NAMESPACES)
	typedef vector<XENCCipher *>			CipherListVectorType;
#else
	typedef std::vector<XENCCipher *>		CipherListVectorType;
#endif

public:

    /** @name Constructors and Destructors */
    //@{
	
    /**
	 * \brief Default constructor.
	 *
	 * <p>The provider class requires no parameters for construction</p>
	 *
	 */


	XSECProvider();
	~XSECProvider();

	//@}

    /** @name Signature Creation Classes */
    //@{
	
    /**
	 * \brief DSIGSignature creator for use with existing XML signatures or templates.
	 *
	 * <p>Create a DSIGSignature object based on an already existing
	 * DSIG Signature XML node.  It is assumed that the underlying
	 * DOM structure is in place and works correctly.</p>
	 *
	 * <p>In this case, the caller can pass in the signature DOM Node for cases
	 * where there may be more than one signature in a document.  The caller
	 * needs to specify which signature tree is to be used.</p>
	 *
	 * @param doc The DOM document node in which the signature is embedded.
	 * @param sigNode The DOM node (within doc) that is to be used as the 
	 *	base of the signature.
	 * @see DSIGSignature#load
	 */

	DSIGSignature * newSignatureFromDOM(
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc, 
		XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *sigNode
	);

    /**
	 * \brief DSIGSignature creator for use with existing XML signatures or templates.
	 *
	 * <p>Create a DSIGSignature object based on an already existing
	 * DSIG Signature XML node.  It is assumed that the underlying
	 * DOM structure is in place and works correctly.</p>
	 *
	 * <p>In this case, the XML-Security libraries will find the signature
	 * node.</p>
	 *
	 * @note The library will <em>only</em> find and use the first signature node
	 * in the document.  If there are more, they will not be validated
	 * @param doc The DOM document node in which the signature is embedded.
	 * @see DSIGSignature#load
	 */
	
	DSIGSignature * newSignatureFromDOM(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc);

    /**
	 * \brief DSIGSignature creator for creating new XML signatures.
	 *
	 * <p>Create an empty DSIGSignature object that can be used to create new
	 * signature values.  The returned signature object needs to be initialised
	 * with a document so a blank signature DOM structure can be created</p>
	 *
	 * @see DSIGSignature#createBlankSignature
	 */
	
	DSIGSignature * newSignature(void);

    /**
	 * \brief Method for destroying DSIGSignature objects created via this provider.
	 *
	 * <p>The provider keeps track of all signature objects created during the lifetime
	 * of the provider.  This method can be called to delete a signature whilst the 
	 * provider is still in scope.  Otherwise the objects will be automatically
	 * deleted when the provider object goes out of scope.</p>
	 * 
	 * <p>In cases where the DSIGSignature has been used to create a new DOM structure,
	 * it can be safely deleted once the signature operations have been completed without
	 * impacting the underlying DOM structure.</p>
	 *
	 * @param toRelease The DSIGSignature object to be deleted.
	 * @todo The DSIGSignature objects are fairly bulky in terms of creation and deletion.
	 * There should be a capability to store "released" objects in a re-use stack.  At the
	 * moment the Provider class simply deletes the objects.
	 * @see DSIGSignature#createBlankSignature
	 */

	void releaseSignature(DSIGSignature * toRelease);

	//@}

	/** @name Encryption Creation Functions */
	//@{

	/**
	 * \brief Create an XENCCipher object based on a particular DOM Document
	 *
	 * XENCCipher is an engine class that is used to wrap encryption/decryption
	 * functions.  Unlike the Signature functions, only a XENCCipher object attached
	 * to a particular document is required.  Arbitrary objects within this document
	 * can then be encrypted/decrypted using this class.
	 *
	 * @param doc Document to attach the XENCCipher to.
	 * @returns An implementation object for XENCCipher
	 */

	XENCCipher * newCipher(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument * doc);

	/**
	 * \brief Method to delete XENCCipher objects created via this provider
	 *
	 * <p>The provider keeps track of all objects by it.  This method can be used
	 * to delete any previously created XENCCipher objects prior to the provider
	 * being deleted.  Any XENCCipher objects not released using this function will
	 * automatically be deleted when the provider goes out of scope (or is itself
	 * deleted).
	 *
	 * @param toRelease The XENCCipher object to be deleted
	 */

	void releaseCipher(XENCCipher * toRelease);

	//@}

	/** @name XKMS Functions */
	//@{

	/**
	 * \brief Obtain a pointer to the XKMSMessageFactory.
	 *
	 * The XKMSMessageFactory is used to create and manipulate XKMS messages.
	 *
	 * @note Unlike other objects created by the provider, only one 
	 * XKMSMessageFactory is ever instantiated for a particular provider.
	 * Applications should <b>never</b> delete the Factory, as it is taken
	 * care of by the provider.
	 */

	XKMSMessageFactory * getXKMSMessageFactory(void);

	/** @name Environmental Options */
	//@{

	/**
	 * \brief Set the default URIResolver.
	 *
	 * DSIGSignature objects require a URIResolver to allow them to de-reference
	 * URIs in reference elements.
	 *
	 * This function sets the resolver that will be used for all
	 * signatures created after this is set.  The resolver is
	 * cloned, so the object passed in can be safely deleted once the
	 * function has been completed.
	 */

	void setDefaultURIResolver(XSECURIResolver * resolver);

	//@}

private:

	// Copy constructor is disabled
	XSECProvider(const XSECProvider &);
	XSECProvider * operator = (const XSECProvider &);

	// Internal functions

	void setup(DSIGSignature *sig);
	void setup(XENCCipher *cipher);

	SignatureListVectorType						m_activeSignatures;
	CipherListVectorType						m_activeCiphers;

	XKMSMessageFactory							* mp_xkmsMessageFactory;

	XSECURIResolver								* mp_URIResolver;
	XERCES_CPP_NAMESPACE_QUALIFIER XMLMutex		m_providerMutex;
};

/** @} */

#endif /* XSECPROVIDER_INCLUDE */
