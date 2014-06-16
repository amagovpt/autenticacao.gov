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
 * $Id: simpleDecrypt.cpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#include "IOStreamOutputter.hpp"

// Xerces

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>

// XML-Security-C (XSEC)

#include <xsec/framework/XSECProvider.hpp>
#include <xsec/framework/XSECException.hpp>
#include <xsec/xenc/XENCCipher.hpp>

#include <xsec/enc/OpenSSL/OpenSSLCryptoKeyRSA.hpp>

// Xalan

#ifndef XSEC_NO_XALAN
#include <xalanc/XalanTransformer/XalanTransformer.hpp>
XALAN_USING_XALAN(XalanTransformer)
#endif

// OpenSSL

#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/pem.h>

XERCES_CPP_NAMESPACE_USE

char letter[] = "\n\
<Letter>\n\
<ToAddress>The address of the Recipient</ToAddress>\n\
<FromAddress>The address of the Sender</FromAddress>\n\
<xenc:EncryptedData Type=\"http://www.w3.org/2001/04/xmlenc#Element\" xmlns:xenc=\"http://www.w3.org/2001/04/xmlenc#\">\n\
<xenc:EncryptionMethod Algorithm=\"http://www.w3.org/2001/04/xmlenc#tripledes-cbc\"/>\n\
<ds:KeyInfo xmlns:ds=\"http://www.w3.org/2000/09/xmldsig#\">\n\
<xenc:EncryptedKey xmlns:xenc=\"http://www.w3.org/2001/04/xmlenc#\">\n\
<xenc:EncryptionMethod Algorithm=\"http://www.w3.org/2001/04/xmlenc#rsa-1_5\"/>\n\
<xenc:CipherData>\n\
<xenc:CipherValue>UDH9I9k6dRvlIS7fHLMONtlP24mqEDPzsjSYUg4X5SSzUFskiVaOIzhYPeF21ZeG\n\
MCmdo6wsu6TQB0XebmvJR2wgvYqDd2L2IbXZ1zj2/G7FbVsB/GSGxuQTFw22aMFd\n\
l47P8bKjxX8cUwwYFzbGAUfkgeQp5DglmnJYJPO1cTw=\n\
</xenc:CipherValue>\n\
</xenc:CipherData>\n\
</xenc:EncryptedKey>\n\
</ds:KeyInfo>\n\
<xenc:CipherData>\n\
<xenc:CipherValue>Q7M6QhZOqjyYXdUDOw809dGEJWkyAygRv7r3EJ8FlcY7dYpn78ON79N/ZB5wJxQt\n\
koyHvWimPWkNRlKGP10DJ42PIGDvuTnTBFHCKbvaMrEjkll8PUd8zHQ0OJIaWIYi\n\
AKydplJy2rCzppwn0bI0YQ==\n\
</xenc:CipherValue>\n\
</xenc:CipherData>\n\
</xenc:EncryptedData></Letter>";


// PKCS8 PEM encoded PrivateKey structure (not Encrypted)

char s_privateKey[] = "\n\
-----BEGIN RSA PRIVATE KEY-----\n\
MIICXAIBAAKBgQDQj3pktZckAzwshRnfvLhz3daNU6xpAzoHo3qjCftxDwH1RynP\n\
A5eycJVkV8mwH2C1PFktpjtQTZ2CvPjuKmUV5zEvmYzuIo6SWYaVZN/PJjzsEZMa\n\
VA+U8GhfX1YF/rsuFzXCi8r6FVd3LN//pXHEwoDGdJUdlpdVEuX1iFKlNQIDAQAB\n\
AoGAYQ7Uc7e6Xa0PvNw4XVHzOSC870pISxqQT+u5b9R+anAEhkQW5dsTJpyUOX1N\n\
RCRmGhG6oq7gnY9xRN1yr0uVfJNtc9/HnzJL7L1jeJC8Ub+zbEBvNuPDL2P21ArW\n\
tcXRycUlfRCRBLop7rfOYPXsjtboAGnQY/6hK4rOF4XGrQUCQQD3Euj+0mZqRRZ4\n\
M1yN2wVP0mKOMg2i/HZXaNeVd9X/wyBgK6b7BxHf6onf/mIBWnJnRBlvdCrSdhuT\n\
lPKEoSgvAkEA2BhfWwQihqD4qJcV65nfosjzOZG41rHX69nIqHI7Ejx5ZgeQByH9\n\
Ym96yXoSpZj9ZlFsJYNogTBBnUBjs+jL2wJAFjpVS9eR7y2X/+hfA0QZDj1XMIPA\n\
RlGANAzymDfXwNLFLuG+fAb+zK5FCSnRl12TvUabIzPIRnbptDVKPDRjcQJBALn8\n\
0CVv+59P8HR6BR3QRBDBT8Xey+3NB4Aw42lHV9wsPHg6ThY1hPYx6MZ70IzCjmZ/\n\
8cqfvVRjijWj86wm0z0CQFKfRfBRraOZqfmOiAB4+ILhbJwKBBO6avX9TPgMYkyN\n\
mWKCxS+9fPiy1iI+G+B9xkw2gJ9i8P81t7fsOvdTDFA=\n\
-----END RSA PRIVATE KEY-----";


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

	// Use xerces to parse the document
	XercesDOMParser * parser = new XercesDOMParser;
	parser->setDoNamespaces(true);
	parser->setCreateEntityReferenceNodes(true);
	parser->setDoSchema(true);

	// Create an input source

	MemBufInputSource* memIS = new MemBufInputSource ((const XMLByte*) letter, (unsigned int) strlen(letter), "XSECMem");

	parser->parse(*memIS);
    xsecsize_t errorCount = parser->getErrorCount();
    if (errorCount > 0) {
		cerr << "Error parsing input document\n";
		exit (1);
	}

    DOMDocument *doc = parser->getDocument();

	/* Decrypt Operations */

	try {

		/* Create the cipher object that we need */

		XSECProvider prov;
		XENCCipher *cipher;

		cipher = prov.newCipher(doc);

		/* Load the private key via OpenSSL and then wrap in an OpenSSLCrypto construct */
		BIO * bioMem = BIO_new(BIO_s_mem());
		BIO_puts(bioMem, s_privateKey);
		EVP_PKEY * pk = PEM_read_bio_PrivateKey(bioMem, NULL, NULL, NULL);

		/* NOTE : For simplicity - no error checking here */

		OpenSSLCryptoKeyRSA * k = new OpenSSLCryptoKeyRSA(pk);
		cipher->setKEK(k);

		/* Find the EncryptedData node */
		DOMNode * encryptedNode = findXENCNode(doc, "EncryptedData");

		/* Do the decrypt */
		cipher->decryptElement((DOMElement *) encryptedNode);

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
