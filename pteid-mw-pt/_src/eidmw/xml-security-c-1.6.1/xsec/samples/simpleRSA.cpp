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
 * SimpleHMAC := An application to generate an XML document (via Xerces) and sign it
 *
 * Author(s): Berin Lautenbach
 *
 * $ID$
 *
 * $LOG$
 *
 */

#include "IOStreamOutputter.hpp"

// Xerces

#include <xercesc/util/PlatformUtils.hpp>

// XML-Security-C (XSEC)

#include <xsec/framework/XSECProvider.hpp>
#include <xsec/dsig/DSIGReference.hpp>
#include <xsec/enc/OpenSSL/OpenSSLCryptoKeyHMAC.hpp>
#include <xsec/framework/XSECException.hpp>


XERCES_CPP_NAMESPACE_USE

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
		MAKE_UNICODE_STRING("\nTo whom it may concern\n\n...\n")));

	return doc;

}

int main (int argc, char **argv) {

	try {
		XMLPlatformUtils::Initialize();
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
	DOMElement *rootElem = doc->getDocumentElement();

	// The signature

	XSECProvider prov;
	DSIGSignature *sig;
	DOMElement *sigNode;

	try {
		
		// Create a signature object

		sig = prov.newSignature();
		sig->setDSIGNSPrefix(MAKE_UNICODE_STRING("ds"));

		// Use it to create a blank signature DOM structure from the doc

		sigNode = sig->createBlankSignature(doc, CANON_C14N_COM, SIGNATURE_RSA, HASH_SHA1);

		// Inser the signature DOM nodes into the doc

		rootElem->appendChild(doc->createTextNode(MAKE_UNICODE_STRING("\n")));
		rootElem->appendChild(sigNode);
		rootElem->appendChild(doc->createTextNode(MAKE_UNICODE_STRING("\n")));

		// Create an envelope reference for the text to be signed
		DSIGReference * ref = sig->createReference(MAKE_UNICODE_STRING(""));
		ref->appendEnvelopedSignatureTransform();

		// Set the HMAC Key to be the string "secret"

		OpenSSLCryptoKeyHMAC * hmacKey = new OpenSSLCryptoKeyHMAC();
		hmacKey->setKey((unsigned char *) "secret", (unsigned int) strlen("secret"));
		sig->setSigningKey(hmacKey);

		// Add a KeyInfo element
		sig->appendKeyName(MAKE_UNICODE_STRING("The secret key is \"secret\""));

		// Sign

		sig->sign();
		
	}

	catch (XSECException &e)
	{
		cerr << "An error occured during a signature load\n   Message: "
		<< e.getMsg() << endl;
		exit(1);
		
	}

	// Output

	docSetup(doc);
	cout << doc;

	return 0;

}
