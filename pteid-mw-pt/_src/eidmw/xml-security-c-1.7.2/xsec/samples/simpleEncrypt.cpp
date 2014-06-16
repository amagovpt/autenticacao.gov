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
 * SimpleEncrypt := An application to generate an XML document (via Xerces) and encrypt
 *					a portion of it
 *
 * $Id: simpleEncrypt.cpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#include "IOStreamOutputter.hpp"

// Xerces

#include <xercesc/util/PlatformUtils.hpp>

// XML-Security-C (XSEC)

#include <xsec/framework/XSECProvider.hpp>
#include <xsec/framework/XSECException.hpp>
#include <xsec/xenc/XENCCipher.hpp>
#include <xsec/xenc/XENCEncryptedData.hpp>
#include <xsec/xenc/XENCEncryptedKey.hpp>

#include <xsec/enc/OpenSSL/OpenSSLCryptoSymmetricKey.hpp>
#include <xsec/enc/OpenSSL/OpenSSLCryptoX509.hpp>

// Xalan

#ifndef XSEC_NO_XALAN
#include <xalanc/XalanTransformer/XalanTransformer.hpp>
XALAN_USING_XALAN(XalanTransformer)
#endif

// OpenSSL

#include <openssl/rand.h>

XERCES_CPP_NAMESPACE_USE

DOMElement * g_toEncrypt;

char cert[] = "\n\
MIIC7jCCAq6gAwIBAgICEAMwCQYHKoZIzjgEAzB5MQswCQYDVQQGEwJBVTEMMAoG\n\
A1UECBMDVmljMRIwEAYDVQQHEwlNZWxib3VybmUxHzAdBgNVBAoTFlhNTC1TZWN1\n\
cml0eS1DIFByb2plY3QxEDAOBgNVBAsTB1hTRUMtQ0ExFTATBgNVBAMTDFhTRUMt\n\
Q0EgUm9vdDAeFw0wMjEyMTIxMDEzMTlaFw0xMjEyMDkxMDEzMTlaMFYxCzAJBgNV\n\
BAYTAkFVMQwwCgYDVQQIEwNWaWMxHzAdBgNVBAoTFlhNTC1TZWN1cml0eS1DIFBy\n\
b2plY3QxGDAWBgNVBAMTD1JTQSBTYW1wbGUgQ2VydDCBnzANBgkqhkiG9w0BAQEF\n\
AAOBjQAwgYkCgYEA0I96ZLWXJAM8LIUZ37y4c93WjVOsaQM6B6N6own7cQ8B9Ucp\n\
zwOXsnCVZFfJsB9gtTxZLaY7UE2dgrz47iplFecxL5mM7iKOklmGlWTfzyY87BGT\n\
GlQPlPBoX19WBf67Lhc1wovK+hVXdyzf/6VxxMKAxnSVHZaXVRLl9YhSpTUCAwEA\n\
AaOCAQEwgf4wCQYDVR0TBAIwADAsBglghkgBhvhCAQ0EHxYdT3BlblNTTCBHZW5l\n\
cmF0ZWQgQ2VydGlmaWNhdGUwHQYDVR0OBBYEFGq6U1SwYNRyTJGAwARirEdavfXB\n\
MIGjBgNVHSMEgZswgZiAFBKNX9CsAIsjUIFmVq4wE4wlOGC5oX2kezB5MQswCQYD\n\
VQQGEwJBVTEMMAoGA1UECBMDVmljMRIwEAYDVQQHEwlNZWxib3VybmUxHzAdBgNV\n\
BAoTFlhNTC1TZWN1cml0eS1DIFByb2plY3QxEDAOBgNVBAsTB1hTRUMtQ0ExFTAT\n\
BgNVBAMTDFhTRUMtQ0EgUm9vdIIBADAJBgcqhkjOOAQDAy8AMCwCFGoKhVPnDeg9\n\
nbEFo2KDDlG/NiUqAhRJxQPLXDhehQjn6eqQWOUlkFtA9A==";


DOMDocument *createLetter(DOMImplementation *impl) {

	DOMDocument *doc = impl->createDocument(
                0,
                MAKE_UNICODE_STRING("Letter"),             
                NULL);  

    DOMElement *rootElem = doc->getDocumentElement();

	// Add the ToAddress

	DOMElement *addressElem = doc->createElement(MAKE_UNICODE_STRING("ToAddress"));
	rootElem->appendChild(doc->createTextNode(MAKE_UNICODE_STRING("\n")));
	rootElem->appendChild(addressElem);
	addressElem->appendChild(doc->createTextNode(
		MAKE_UNICODE_STRING("The address of the Recipient")));

	// Add the FromAddress
	addressElem = doc->createElement(MAKE_UNICODE_STRING("FromAddress"));
	rootElem->appendChild(doc->createTextNode(MAKE_UNICODE_STRING("\n")));
	rootElem->appendChild(addressElem);
	addressElem->appendChild(doc->createTextNode(
		MAKE_UNICODE_STRING("The address of the Sender")));

	// Add some text
	DOMElement *textElem = doc->createElement(MAKE_UNICODE_STRING("Text"));
	rootElem->appendChild(doc->createTextNode(MAKE_UNICODE_STRING("\n")));
	rootElem->appendChild(textElem);
	textElem->appendChild(doc->createTextNode(
		MAKE_UNICODE_STRING("\nTo whom it may concern, my secret \
credit card number is : \n  0123 4567 89ab cdef\n\n...\n")));

	g_toEncrypt = textElem;
	return doc;

}

int main (int argc, char **argv) {

	try {
		XMLPlatformUtils::Initialize();
#ifndef XSEC_NO_XALAN
		XalanTransformer::initialize();
#endif
		XSECPlatformUtils::Initialise();
	}
	catch (const XMLException &e) {

		cerr << "Error during initialisation of Xerces" << endl;
		cerr << "Error Message = : "
		     << e.getMessage() << endl;

	}

	// Create a blank Document

    DOMImplementation *impl = 
		DOMImplementationRegistry::getDOMImplementation(MAKE_UNICODE_STRING("Core"));
	
	// Create a letter
	DOMDocument *doc = createLetter(impl);

	try {
		
		/* Create the cipher object that we need */

		XSECProvider prov;
		XENCCipher *cipher;

		cipher = prov.newCipher(doc);

		/* Now generate a random key that we can use to encrypt the element
		 *
		 * First check the status of the random generation in OpenSSL
		 */

		if (RAND_status() != 1) {

			cerr << "OpenSSL random generation not properly initialised" << endl;
			exit(1);

		}

		unsigned char keyBuf[24];
		if (RAND_bytes(keyBuf, 24) == 0) {

			cerr << "Error obtaining 24 bytes of random from OpenSSL" << endl;
			exit(1);

		}

		/* Wrap this in a Symmetric 3DES key */

		OpenSSLCryptoSymmetricKey * key = 
			new OpenSSLCryptoSymmetricKey(XSECCryptoSymmetricKey::KEY_3DES_192);
		key->setKey(keyBuf, 24);
		cipher->setKey(key);

		/* Encrypt the element that needs to be hidden */
		cipher->encryptElement(g_toEncrypt, ENCRYPT_3DES_CBC);

		/* Now lets create an EncryptedKey element to hold the generated key */

		/* First lets load the public key in the certificate */
		OpenSSLCryptoX509 * x509 = new OpenSSLCryptoX509();
		x509->loadX509Base64Bin(cert, (unsigned int) strlen(cert));
	
		/* Now set the Key Encrypting Key (NOTE: Not the normal key) */
		cipher->setKEK(x509->clonePublicKey());
		

		/* Now do the encrypt, using RSA with PKCS 1.5 padding */

		XENCEncryptedKey * encryptedKey = 
			cipher->encryptKey(keyBuf, 24, ENCRYPT_RSA_15);

		/*
		 * Add the encrypted Key to the previously created EncryptedData, which
		 * we first retrieve from the cipher object.  This will automatically create
		 * the appropriate <KeyInfo> element within the EncryptedData
		 */

		XENCEncryptedData * encryptedData = cipher->getEncryptedData();
		encryptedData->appendEncryptedKey(encryptedKey);
		

	}

	catch (XSECException &e)
	{
		char * msg = XMLString::transcode(e.getMsg());
		cerr << "An error occurred during an encryption operation\n   Message: "
		<< msg << endl;
		exit(1);
		
	}

	/* Output */
	docSetup(doc);
	cout << doc;

	return 0;

}
