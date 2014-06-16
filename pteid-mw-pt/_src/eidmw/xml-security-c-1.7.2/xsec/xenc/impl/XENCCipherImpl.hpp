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
 * XENCCipherImpl := Implementation of the main encryption worker class
 *
 * $Id: XENCCipherImpl.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef XENCCIPHERIMPL_INCLUDE
#define XENCCIPHERIMPL_INCLUDE

// XSEC Includes

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/xenc/XENCCipher.hpp>

class safeBuffer;
class XSECProvider;
class XENCEncryptedDataImpl;
class TXFMChain;
class XSECEnv;
class XSECKeyInfoResolver;
class XSECPlatformUtils;
class DSIGKeyInfoList;

XSEC_DECLARE_XERCES_CLASS(DOMNode);
XSEC_DECLARE_XERCES_CLASS(DOMDocumentFragment);

class XENCCipherImpl : public  XENCCipher {

public:

	virtual ~XENCCipherImpl();

	// Implementation for decrypting elements

	XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument * 
		decryptElement(XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * element);
	XERCES_CPP_NAMESPACE_QUALIFIER DOMNode * 
		decryptElementDetached(XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * element);
	XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument * 
		decryptElement(void);
	XERCES_CPP_NAMESPACE_QUALIFIER DOMNode * 
		decryptElementDetached(void);
	XSECBinTXFMInputStream * decryptToBinInputStream(
		XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * element
	);

	// Decrypting Keys
	virtual int decryptKey(XENCEncryptedKey * encryptedKey, 
		XMLByte * rawKey,
		int maxKeySize);
	virtual int decryptKey(
		XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * keyNode,
		XMLByte * rawKey,
		int maxKeySize
	);


	// Implementation for encryption Elements
	XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument * encryptElement(
		XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * element,
		encryptionMethod em,
		const XMLCh * uri = NULL);
	virtual XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument * encryptElementContent(
		XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * element,
		encryptionMethod em,
		const XMLCh * algorithmURI = NULL);
	XERCES_CPP_NAMESPACE_QUALIFIER DOMNode * encryptElementDetached(
		XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * element,
		encryptionMethod em,
		const XMLCh * uri = NULL);
	virtual XERCES_CPP_NAMESPACE_QUALIFIER DOMNode * encryptElementContentDetached(
		XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * element,
		encryptionMethod em,
		const XMLCh * algorithmURI = NULL);

	// Encrypt a key
	virtual XENCEncryptedKey * encryptKey(
		const unsigned char * keyBuffer,
		unsigned int keyLen,
		encryptionMethod em,
		const XMLCh * algorithmURI = NULL
	);
	// Encrypt octet streams
	virtual XENCEncryptedData * encryptBinInputStream(
		XERCES_CPP_NAMESPACE_QUALIFIER BinInputStream * plainText,
		encryptionMethod em,
		const XMLCh * algorithmURI = NULL
	);
	virtual XENCEncryptedData * encryptTXFMChain(
		TXFMChain * plainText,
		encryptionMethod em,
		const XMLCh * algorithmURI = NULL
	);

	// Getter methods
	XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument * getDocument(void) 
		{return mp_doc;}
	const XMLCh * getXENCNSPrefix(void) const;
	virtual XENCEncryptedData * getEncryptedData(void);
	bool getPrettyPrint(void);
	bool getExclusiveC14nSerialisation(void);

	// Setter methods
	void setKey(XSECCryptoKey * key);
	void setKEK(XSECCryptoKey * key);
	void setKeyInfoResolver(const XSECKeyInfoResolver * resolver);

	void setXENCNSPrefix(const XMLCh * prefix);
	void setPrettyPrint(bool flag);
	void setExclusiveC14nSerialisation(bool flag);

	// Creation methods
	XENCEncryptedData * createEncryptedData(XENCCipherData::XENCCipherDataType type,
											const XMLCh * algorithm,
											const XMLCh * value);
	virtual XENCEncryptedKey * loadEncryptedKey(
		XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * keyNode
		);
	virtual XENCEncryptedData * loadEncryptedData(
		XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * dataNode
		);

protected:

	// Initialisation
	static void Initialise(void);

protected:

	// Protected to prevent direct creation of objects
	XENCCipherImpl(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument * doc);

private:

	// Internal functions
	XERCES_CPP_NAMESPACE_QUALIFIER DOMDocumentFragment 
							* deSerialise(
								safeBuffer &content, 
								XERCES_CPP_NAMESPACE_QUALIFIER DOMNode * ctx
							);
	XSECCryptoKey * decryptKeyFromKeyInfoList(DSIGKeyInfoList * kil);

	// Unimplemented constructor
	XENCCipherImpl();

	XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument				
							* mp_doc;			// Document against which this will operate

	// Current working object
	XENCEncryptedDataImpl	* mp_encryptedData;

	// Key
	XSECCryptoKey			* mp_key;
	bool					m_keyDerived;		// Was this derived or loaded?

	// KEK
	XSECCryptoKey			* mp_kek;
	bool					m_kekDerived;		// Was this derived or loaded?

	// Environment
	XSECEnv					* mp_env;

	// Resolvers
	XSECKeyInfoResolver		* mp_keyInfoResolver;

	// Use exclusive canonicalisation?
	bool					m_useExcC14nSerialisation;

	friend class XSECProvider;
	friend class XSECPlatformUtils;

};

#endif /* XENCCIPHERIMPL_INCLUDE */

