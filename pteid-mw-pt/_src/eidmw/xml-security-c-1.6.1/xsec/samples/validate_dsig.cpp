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
 * simpleValidate := An application to validate an in-memory signature
 *
 * Author(s): Berin Lautenbach
 *
 * $ID$
 *
 * $LOG$
 *
 */

#include <iostream>
#include <fstream>
#include <string>

// XML-Security-C (XSEC)

#include <xsec/framework/XSECProvider.hpp>
#include <xsec/dsig/DSIGReference.hpp>
#include <xsec/enc/OpenSSL/OpenSSLCryptoKeyHMAC.hpp>
#include <xsec/framework/XSECException.hpp>
#include <xsec/enc/OpenSSL/OpenSSLCryptoX509.hpp>
#include <xsec/enc/OpenSSL/OpenSSLCryptoHash.hpp>
#include <xsec/enc/XSECCryptoException.hpp>


// Xerces

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/framework/LocalFileInputSource.hpp>
#include <xercesc/dom/DOMErrorHandler.hpp>
#include <xercesc/dom/DOMError.hpp>
#include <xercesc/dom/DOMException.hpp>

//OpenSSL
#include <openssl/sha.h>

XERCES_CPP_NAMESPACE_USE
using std::cerr;
using std::cout;
using std::endl;
using std::ostream;
using std::flush;


char docToValidate[4096] = "\
<PurchaseOrder>\n\
<Company>Widgets.Org</Company>\n\
<Product>A large widget</Product>\n\
<Amount>$16.50</Amount>\n\
<Due>16 January 2010</Due>\n\
<ds:Signature xmlns:ds=\"http://www.w3.org/2000/09/xmldsig#\">\n\
<ds:SignedInfo>\n\
<ds:CanonicalizationMethod Algorithm=\"http://www.w3.org/TR/2001/REC-xml-c14n-20010315\"/>\n\
<ds:SignatureMethod Algorithm=\"http://www.w3.org/2000/09/xmldsig#dsa-sha1\"/>\n\
<ds:Reference URI=\"#xpointer(/)\">\n\
<ds:Transforms>\n\
<ds:Transform Algorithm=\"http://www.w3.org/2000/09/xmldsig#enveloped-signature\"/>\n\
<ds:Transform Algorithm=\"http://www.w3.org/TR/2001/REC-xml-c14n-20010315#WithComments\"/>\n\
</ds:Transforms>\n\
<ds:DigestMethod Algorithm=\"http://www.w3.org/2000/09/xmldsig#sha1\"/>\n\
<ds:DigestValue>n+6y945h/SvlVF9qBq+Lb4TrcOI=</ds:DigestValue>\n\
</ds:Reference>\n\
</ds:SignedInfo>\n\
<ds:SignatureValue>OmToLo8uEnK37nCFXDiZwgcsZGJ0aZ4AyECUy78DL91AHRRWdjllSQ==</ds:SignatureValue>\n\
<ds:KeyInfo>\n\
<ds:X509Data>\n\
<ds:X509SubjectName>C=AU, ST=Vic, O=XML-Security-C Project, CN=Samples Demo Certificate</ds:X509SubjectName>\n\
</ds:X509Data>\n\
</ds:KeyInfo>\n\
</ds:Signature>\n\
</PurchaseOrder>\n";

//Various crap util-code that should go in a seperate file
//
struct HexCharStruct
{
	unsigned char c;
	HexCharStruct(unsigned char _c) : c(_c) { }
};

inline std::ostream& operator<<(std::ostream& o, const HexCharStruct& hs)
{
	  return (o << std::hex << (unsigned int)hs.c);
}

inline HexCharStruct hex(unsigned char _c)
{
	  return HexCharStruct(_c);
}

void printByteArray(std::ostream &stream, unsigned char * arr, int len)
{
	unsigned int j = 0;

	while ( j < len )
	{
		if (j % 20 == 0)
			stream << endl;
		stream << hex((unsigned char)arr[j]);
		j++;
	}
	stream << endl;
}


int SHA1HashBytes(unsigned char * in, unsigned int len, unsigned char * out)
{
	int out_bytes = 0;
	//Direct OpenSSL call
	SHA1(in, len, out);

	return out_bytes;
}

//TODO: Refactor/Fix this into something reasonable
void HashFile(const char * path)
{
	//XXX
	std::ifstream file(path, std::ios::binary|std::ios::ate);
	char * in;
	unsigned char out[20];
	std::ifstream::pos_type size = -1;
	if (file.is_open())
	{
		size = file.tellg();
		in = new char [size];
		file.seekg (0, std::ios::beg);
		file.read(in, size);
	}

	SHA1HashBytes ((unsigned char *)in, size, out);
	cout << "SHA1 Sum: ";
	printByteArray (std::cout, out, 20);

}

int main (int argc, char **argv) {


	if (argc < 2)
		return -1;


	try {
		XMLPlatformUtils::Initialize();
		XSECPlatformUtils::Initialise();
	}
	catch (const XMLException &e) {

		cerr << "Error during initialisation of Xerces" << endl;
		cerr << "Error Message = : "
		     << e.getMessage() << endl;
	}


	// Use xerces to parse the document
	XercesDOMParser * parser = new XercesDOMParser();
	//parser->setDoNamespaces(true);
    	//ErrorHandler *errorHandler = (ErrorHandler *)new MyErrorHandler();
    	//parser->setErrorHandler(errorHandler);
	//parser->setCreateEntityReferenceNodes(true);
	//parser->setDoSchema(true);

	// Create an input source

	//LocalFileInputSource * input = new LocalFileInputSource((const XMLCh *)argv[1]);

	xsecsize_t errorCount = 0;

	try
	{

	   cout << "Parsing file " << argv[1] << endl;
	   parser->parse(argv[1]);
	}
	catch (const XMLException& toCatch)
        {   
            XERCES_STD_QUALIFIER cerr << "\nError during parsing: '" << "'\n"
                 << "Exception message is:  \n"
                 << XMLString::transcode(toCatch.getMessage()) << "\n" << XERCES_STD_QUALIFIER endl;
        }   
        catch (const DOMException& toCatch)
        {   
            const unsigned int maxChars = 2047;
            XMLCh errText[maxChars + 1]; 

            XERCES_STD_QUALIFIER cerr << "\nDOM Error during parsing:" << "'\n"
                 << "DOMException code is:  " << toCatch.code << XERCES_STD_QUALIFIER endl;

            if (DOMImplementation::loadDOMExceptionMsg(toCatch.code, errText, maxChars))
                 XERCES_STD_QUALIFIER cerr << "Message is: " << XMLString::transcode(errText) << XERCES_STD_QUALIFIER endl;
        }
	catch (...)
	{
		cout << "Unexpected Exception \n" ;
		return -1;
	}

        DOMDocument *doc = parser->getDocument();

	// Now create a signature object to validate the document

	XSECProvider prov;
	DSIGSignature * sig;
	const char * sig_value;
	const char * cert_value = NULL;


	try {
	
		//sig = prov.newSignatureFromDOM(doc);
		
		// Find the X509Data node
		DOMNode *node = doc->getDocumentElement();
		node->normalize();
		if (node == NULL)
			cerr << "Error on getDocumentElement()\n";

		node = node->getFirstChild();
		node = node->getNextSibling();
		/*node = node->getFirstChild();
		if (node != NULL && node->getNodeType() == DOMNode::TEXT_NODE)
		{
			sig_value = XMLString::transcode(node->getNodeValue()) ;
		}
		else 
			cout << "Error finding signature node!\n" ;
		*/
		//XXX: Ugly (refactor into stuff) 
		node = node->getNextSibling();
		node = node->getFirstChild();
		node = node->getNextSibling();
		//node = node->getNextSibling();
		//cout << "Tag 1: " << XMLString::transcode(node->getTextContent()) << endl;
		node = node->getNextSibling();
		//X509Data element
		node = node->getNextSibling();
		//X509Certificate
		node = node->getFirstChild();
		node = node->getNextSibling();
		node = node->getFirstChild();
		cert_value =  XMLString::transcode(node->getNodeValue()) ;
		cout << "Cert value: " << cert_value << endl;
	        
		if (cert_value == NULL)
			exit(1);

		OpenSSLCryptoX509 * x509 = new OpenSSLCryptoX509();
		x509->loadX509Base64Bin(sig_value, (unsigned int) strlen(sig_value));
		
		//sig->load();
		DSIGKeyInfoList * kinfList = sig->getKeyInfoList();
		
		// See if we can find a Key Name
		const XMLCh * kname;
		DSIGKeyInfoList::size_type size, i;
		size = kinfList->getSize();

		for (i = 0; i < size; ++i) {
			kname = kinfList->item(i)->getKeyName();
			if (kname != NULL) {
				char * n = XMLString::transcode(kname);
				cout << "Key Name = " << n << endl;
				XSEC_RELEASE_XMLCH(n);
			}
		}

		sig->setSigningKey(x509->clonePublicKey());

		if (sig->verify()) {
			cout << "Signature Valid\n";
		}
		else {
			char * err = XMLString::transcode(sig->getErrMsgs());
			cout << "Incorrect Signature\n";
			cout << err << endl;
			XSEC_RELEASE_XMLCH(err);
		}


	}

	catch (XSECException &e)
	{
		cerr << "An error occured during a signature load\n   Message: "
		<< e.getMsg() << endl;
		exit(1);
		
	}
	catch (XSECCryptoException &e) {
		cerr << "An error occured in the XML-Security-C Crypto routines\n   Message: "
		<< e.getMsg() << endl;
		exit(1);
	}
		
		// Clean up

	//delete input;
	delete parser;

	return 0;
}
