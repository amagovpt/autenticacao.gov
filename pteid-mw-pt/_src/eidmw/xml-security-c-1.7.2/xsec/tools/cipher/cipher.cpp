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
 * cipher := Tool to handle basic encryption/decryption of XML documents
 *
 * Author(s): Berin Lautenbach
 *
 * $Id: cipher.cpp 1352678 2012-06-21 20:56:28Z scantor $
 *
 */

// XSEC

#include <xsec/utils/XSECPlatformUtils.hpp>
#include <xsec/framework/XSECProvider.hpp>
#include <xsec/framework/XSECException.hpp>
#include <xsec/enc/XSECCryptoException.hpp>
#include <xsec/enc/XSECCryptoUtils.hpp>
#include <xsec/enc/OpenSSL/OpenSSLCryptoSymmetricKey.hpp>
#include <xsec/utils/XSECBinTXFMInputStream.hpp>
#include <xsec/xenc/XENCEncryptedData.hpp>
#include <xsec/xenc/XENCEncryptedKey.hpp>

#include "XencInteropResolver.hpp"

// ugly :<

#if defined(_WIN32)
#	include <xsec/utils/winutils/XSECURIResolverGenericWin32.hpp>
#else
#	include <xsec/utils/unixutils/XSECURIResolverGenericUnix.hpp>
#endif

// General

#include <memory.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>

#if defined(HAVE_UNISTD_H)
# include <unistd.h>
# define _MAX_PATH PATH_MAX
#else
# if defined(HAVE_DIRECT_H)
#  include <direct.h>
# endif
#endif

#if defined (_DEBUG) && defined (_MSC_VER)
#include <crtdbg.h>
#endif


#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/XMLException.hpp>
#include <xercesc/util/XMLUri.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <xercesc/util/Janitor.hpp>
#include <xercesc/util/BinFileInputStream.hpp>
#include <xercesc/framework/XMLFormatter.hpp>
#include <xercesc/framework/StdOutFormatTarget.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>

XERCES_CPP_NAMESPACE_USE

using std::cerr;
using std::cout;
using std::endl;
using std::ostream;

#ifndef XSEC_NO_XALAN

// XALAN

#include <xalanc/XPath/XPathEvaluator.hpp>
#include <xalanc/XalanTransformer/XalanTransformer.hpp>

XALAN_USING_XALAN(XPathEvaluator)
XALAN_USING_XALAN(XalanTransformer)

#endif

#if !defined (XSEC_HAVE_OPENSSL) && !defined(XSEC_HAVE_WINCAPI) && !defined(XSEC_HAVE_NSS)
#	error No available cryptoAPI
#endif

#if defined (XSEC_HAVE_OPENSSL)
// OpenSSL

#	include <xsec/enc/OpenSSL/OpenSSLCryptoKeyHMAC.hpp>
#	include <xsec/enc/OpenSSL/OpenSSLCryptoKeyRSA.hpp>
#	include <openssl/err.h>
#	include <openssl/bio.h>
#	include <openssl/evp.h>
#	include <openssl/pem.h>

#endif

#if defined (XSEC_HAVE_WINCAPI)

#	include <xsec/enc/WinCAPI/WinCAPICryptoProvider.hpp>
#	include <xsec/enc/WinCAPI/WinCAPICryptoSymmetricKey.hpp>
#	include <xsec/enc/WinCAPI/WinCAPICryptoKeyHMAC.hpp>

#endif

#if defined (XSEC_HAVE_NSS)

#	include <xsec/enc/NSS/NSSCryptoProvider.hpp>
#	include <xsec/enc/NSS/NSSCryptoSymmetricKey.hpp>
#	include <xsec/enc/NSS/NSSCryptoKeyHMAC.hpp>

#endif

#include <time.h>

#ifdef XSEC_NO_XALAN

std::ostream& operator<< (std::ostream& target, const XMLCh * s)
{
    char *p = XMLString::transcode(s);
    target << p;
    XSEC_RELEASE_XMLCH(p);
    return target;
}

#endif

// ----------------------------------------------------------------------------
//           Checksig
// ----------------------------------------------------------------------------


void printUsage(void) {

	cerr << "\nUsage: cipher [options] <input file name>\n\n";
	cerr << "     Where options are :\n\n";
	cerr << "     --decrypt/-d\n";
	cerr << "         Operate in decrypt mode (default) - outputs the decrypted octet stream\n";
	cerr << "         Reads in the input file as an XML file, searches for an EncryptedData node\n";
	cerr << "         and decrypts the content\n";
	cerr << "     --decrypt-element/-de\n";
	cerr << "         Operate in decrypt and XML mode.\n";
	cerr << "         This will output the original XML document with the first encrypted\n";
	cerr << "         element decrypted.\n";
	cerr << "     --encrypt-file/-ef\n";
	cerr << "         Encrypt the contents of the input file as raw data and create an\n";
	cerr << "         XML Encrypted Data outpu\n";
	cerr << "     --encrypt-xml/-ex\n";
	cerr << "         Parse the input file and encrypt the doc element down, storing the\n";
	cerr << "         output as a XML Encrypted Data\n";
	cerr << "     --key/-k [kek] <KEY_TYPE> [options]\n";
	cerr << "         Set the key to use.\n";
	cerr << "             If the first parameter is \"kek\", the key arguments will be used\n";
	cerr << "                  as a Key EncryptionKey\n";
	cerr << "             KEY_TYPE defines what the key is.  Can be one of :\n";
	cerr << "                  X509, RSA, AES128, AES192, AES256, AES128-GCM, AES192-GCM, AES256-GCM or 3DES\n";
	cerr << "             options are :\n";
	cerr << "                  <filename> - for X509 PEM files (must be an RSA KEK certificate\n";
	cerr << "                  <filename> <password> - for RSA private key files (MUST be a KEK)\n";
	cerr << "                  <key-string> - For a string to use as the key for AES or DES keys\n";
	cerr << "     --xkms/-x\n";
	cerr << "         The key that follows on the command line is to be interpreted as\n";
	cerr << "         an XKMS RSAKeyPair encryption key\n";
	cerr << "     --interop/-i\n";
	cerr << "         Use the interop resolver for Baltimore interop examples\n";
	cerr << "     --out-file/-o\n";
	cerr << "         Output the result to the indicated file (rather than stdout)\n";
#if defined (XSEC_HAVE_WINCAPI)
	cerr << "     --wincapi/-w\n";
	cerr << "         Force use of Windows Crypto API\n";
#endif
#if defined (XSEC_HAVE_NSS)
	cerr << "     --nss/-n\n";
	cerr << "         Force use of NSS Crypto API\n";
#endif

	cerr << "\n     Exits with codes :\n";
	cerr << "         0 = Decrypt/Encrypt OK\n";
	cerr << "         1 = Decrypt/Encrypt failed\n";
	cerr << "         2 = Processing error\n";

}

int evaluate(int argc, char ** argv) {
	
	char					* filename = NULL;
	char					* outfile = NULL;
	unsigned char			* keyStr = NULL;
	bool					doDecrypt = true;
	bool					errorsOccured = false;
	bool					doDecryptElement = false;
	bool					useInteropResolver = false;
	bool					encryptFileAsData = false;
	bool					parseXMLInput = true;
	bool					doXMLOutput = false;
	bool					isXKMSKey = false;
	XSECCryptoKey			* kek = NULL;
	XSECCryptoKey			* key = NULL;
	int						keyLen = 0;
	encryptionMethod		kekAlg = ENCRYPT_NONE;
	encryptionMethod		keyAlg = ENCRYPT_NONE;
	DOMDocument				*doc;
	unsigned char			keyBuf[24];
	XMLFormatTarget			*formatTarget ;

#if defined(_WIN32) && defined (XSEC_HAVE_WINCAPI)
	HCRYPTPROV				win32DSSCSP = 0;		// Crypto Providers
	HCRYPTPROV				win32RSACSP = 0;

	CryptAcquireContext(&win32DSSCSP, NULL, NULL, PROV_DSS, CRYPT_VERIFYCONTEXT);
	CryptAcquireContext(&win32RSACSP, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT);

#endif

	if (argc < 2) {

		printUsage();
		return 2;
	}

	// Run through parameters
	int paramCount = 1;

	while (paramCount < argc - 1) {

		if (_stricmp(argv[paramCount], "--decrypt-element") == 0 || _stricmp(argv[paramCount], "-de") == 0) {
			paramCount++;
			doDecrypt = true;
			doDecryptElement = true;
			doXMLOutput = true;
			parseXMLInput = true;
		}
		else if (_stricmp(argv[paramCount], "--interop") == 0 || _stricmp(argv[paramCount], "-i") == 0) {
			// Use the interop key resolver
			useInteropResolver = true;
			paramCount++;
		}
		else if (_stricmp(argv[paramCount], "--encrypt-file") == 0 || _stricmp(argv[paramCount], "-ef") == 0) {
			// Use this file as the input
			doDecrypt = false;
			encryptFileAsData = true;
			doXMLOutput = true;
			parseXMLInput = false;
			paramCount++;
		}
		else if (_stricmp(argv[paramCount], "--encrypt-xml") == 0 || _stricmp(argv[paramCount], "-ex") == 0) {
			// Us this file as an XML input file
			doDecrypt = false;
			encryptFileAsData = false;
			doXMLOutput = true;
			parseXMLInput = true;
			paramCount++;
		}
		else if (_stricmp(argv[paramCount], "--out-file") == 0 || _stricmp(argv[paramCount], "-o") == 0) {
			if (paramCount +2 >= argc) {
				printUsage();
				return 1;
			}
			paramCount++;
			outfile = argv[paramCount];
			paramCount++;
		}
		else if (_stricmp(argv[paramCount], "--xkms") == 0 || _stricmp(argv[paramCount], "-x") == 0) {
			paramCount++;
			isXKMSKey = true;
		}

#if defined (XSEC_HAVE_WINCAPI)
		else if (_stricmp(argv[paramCount], "--wincapi") == 0 || _stricmp(argv[paramCount], "-w") == 0) {
			// Use the interop key resolver
			WinCAPICryptoProvider * cp = new WinCAPICryptoProvider();
			XSECPlatformUtils::SetCryptoProvider(cp);
			paramCount++;
		}
#endif
#if defined (XSEC_HAVE_NSS)
		else if (_stricmp(argv[paramCount], "--nss") == 0 || _stricmp(argv[paramCount], "-n") == 0) {
			// NSS Crypto Provider
			NSSCryptoProvider * cp = new NSSCryptoProvider();
			XSECPlatformUtils::SetCryptoProvider(cp);
			paramCount++;
		}
#endif
		else if (_stricmp(argv[paramCount], "--key") == 0 || _stricmp(argv[paramCount], "-k") == 0) {

			// Have a key!
			paramCount++;
			bool isKEK = false;
			XSECCryptoSymmetricKey::SymmetricKeyType loadKeyAs =
				XSECCryptoSymmetricKey::KEY_NONE;

			if (_stricmp(argv[paramCount], "kek") == 0) {
				isKEK = true;
				paramCount++;
				if (paramCount >= argc) {
					printUsage();
					return 2;
				}
			}

			if (_stricmp(argv[paramCount], "3DES") == 0 ||
				_stricmp(argv[paramCount], "AES128") == 0 ||
				_stricmp(argv[paramCount], "AES192") == 0 ||
				_stricmp(argv[paramCount], "AES256") == 0 ||
				_stricmp(argv[paramCount], "AES128-GCM") == 0 ||
				_stricmp(argv[paramCount], "AES192-GCM") == 0 ||
				_stricmp(argv[paramCount], "AES256-GCM") == 0) {
				
				if (paramCount +2 >= argc) {
					printUsage();
					return 2;
				}

				switch(argv[paramCount][4]) {
				case '\0' :
					keyLen = 24;
					loadKeyAs = XSECCryptoSymmetricKey::KEY_3DES_192;
					keyAlg = ENCRYPT_3DES_CBC;
					break;
				case '2' :
					keyLen = 16;
					loadKeyAs = XSECCryptoSymmetricKey::KEY_AES_128;
					if (isKEK) {
						kekAlg = ENCRYPT_KW_AES128;
					}
					else if (strlen(argv[paramCount]) == 6) {
						keyAlg = ENCRYPT_AES128_CBC;
					}
                    else {
                        keyAlg = ENCRYPT_AES128_GCM;
                    }
					break;
				case '9' :
					keyLen = 24;
					loadKeyAs = XSECCryptoSymmetricKey::KEY_AES_192;
					if (isKEK) {
						kekAlg = ENCRYPT_KW_AES192;
					}
					else if (strlen(argv[paramCount]) == 6) {
						keyAlg = ENCRYPT_AES192_CBC;
					}
                    else {
                        keyAlg = ENCRYPT_AES192_GCM;
                    }
					break;
				case '5' :
					keyLen = 32;
					loadKeyAs = XSECCryptoSymmetricKey::KEY_AES_256;
					if (isKEK) {
						kekAlg = ENCRYPT_KW_AES256;
					}
					else if (strlen(argv[paramCount]) == 6) {
						keyAlg = ENCRYPT_AES256_CBC;
					}
                    else {
                        keyAlg = ENCRYPT_AES256_GCM;
                    }
					break;
				}

				paramCount++;
				unsigned char keyStr[64];
				if (strlen(argv[paramCount]) > 64) {
					cerr << "Key string too long\n";
					return 2;
				}
				XSECCryptoSymmetricKey * sk = 
					XSECPlatformUtils::g_cryptoProvider->keySymmetric(loadKeyAs);

				if (isXKMSKey) {
					unsigned char kbuf[XSEC_MAX_HASH_SIZE];
					CalculateXKMSKEK((unsigned char *) argv[paramCount], (int) strlen(argv[paramCount]), kbuf, XSEC_MAX_HASH_SIZE);
					sk->setKey(kbuf, keyLen);
				}
				else {
					memset(keyStr, 0, 64);
					strcpy((char *) keyStr, argv[paramCount]);
					sk->setKey(keyStr, keyLen);
				}
				paramCount++;
				if (isKEK)
					kek = sk;
				else
					key = sk;
			}


#if defined (XSEC_HAVE_OPENSSL)

			else if (_stricmp(argv[paramCount], "RSA") == 0) {
				// RSA private key file

				if (paramCount + 3 >= argc) {

					printUsage();
					return 2;

				}

				if (!isKEK) {
					cerr << "RSA private keys may only be KEKs\n";
					return 2;
				}

				BIO * bioKey;
				if ((bioKey = BIO_new(BIO_s_file())) == NULL) {

					cerr << "Error opening private key file\n\n";
					return 1;

				}

				if (BIO_read_filename(bioKey, argv[paramCount + 1]) <= 0) {

					cerr << "Error opening private key file\n\n";
					return 1;

				}

				EVP_PKEY * pkey;
				pkey = PEM_read_bio_PrivateKey(bioKey,NULL,NULL,argv[paramCount + 2]);

				if (pkey == NULL) {

					cerr << "Error loading private key\n\n";
					return 1;

				}

				kek = new OpenSSLCryptoKeyRSA(pkey);
				kekAlg = ENCRYPT_RSA_15;
				EVP_PKEY_free(pkey);
				BIO_free(bioKey);
				paramCount += 3;
			}

			else if (_stricmp(argv[paramCount], "X509") == 0) {

				// X509 cert used to load an encrypting key

				if (paramCount + 2 >= argc) {

					printUsage();
					exit (1);

				}

				if (!isKEK) {
					cerr << "X509 private keys may only be KEKs\n";
					return 2;
				}

				// Load the encrypting key
				// For now just read a particular file

				BIO * bioX509;

				if ((bioX509 = BIO_new(BIO_s_file())) == NULL) {

					cerr << "Error opening file\n\n";
					exit (1);

				}

				if (BIO_read_filename(bioX509, argv[paramCount + 1]) <= 0) {

					cerr << "Error opening X509 Certificate " << argv[paramCount + 1] << "\n\n";
					exit (1);

				}

				X509 * x
					;
				x = PEM_read_bio_X509_AUX(bioX509,NULL,NULL,NULL);

				if (x == NULL) {

					BIO * bio_err;
					
					if ((bio_err=BIO_new(BIO_s_file())) != NULL)
						BIO_set_fp(bio_err,stderr,BIO_NOCLOSE|BIO_FP_TEXT);

					cerr << "Error loading certificate key\n\n";
					ERR_print_errors(bio_err);
					BIO_free(bio_err);
					exit (1);

				}

				// Now load the key
				EVP_PKEY *pkey;

				pkey = X509_get_pubkey(x);

				if (pkey == NULL || pkey->type != EVP_PKEY_RSA) {
					cerr << "Error extracting RSA key from certificate" << endl;
				}

				kek = new OpenSSLCryptoKeyRSA(pkey);
				kekAlg = ENCRYPT_RSA_15;

				// Clean up

				EVP_PKEY_free (pkey);
				X509_free(x);
				BIO_free(bioX509);

				paramCount += 2;
				
			} /* argv[1] = "--x509cert" */
#endif /* XSEC_HAVE_OPENSSL */
			else {
				printUsage();
				return 2;
			}
		}

		else {
			cerr << "Unknown option: " << argv[paramCount] << endl;
			printUsage();
			return 2;
		}
	}

	if (paramCount >= argc) {
		printUsage();
		return 2;
	}

	if (outfile != NULL) {
		formatTarget = new LocalFileFormatTarget(outfile);
	}
	else {
		formatTarget = new StdOutFormatTarget();
	}

	filename = argv[paramCount];

	if (parseXMLInput) {

		XercesDOMParser * parser = new XercesDOMParser;
		Janitor<XercesDOMParser> j_parser(parser);
		
		parser->setDoNamespaces(true);
		parser->setCreateEntityReferenceNodes(true);

		// Now parse out file

		xsecsize_t errorCount = 0;
		try
		{
    		parser->parse(filename);
			errorCount = parser->getErrorCount();
			if (errorCount > 0)
				errorsOccured = true;
		}

		catch (const XMLException& e)
		{
			cerr << "An error occured during parsing\n   Message: "
				 << e.getMessage() << endl;
			errorsOccured = true;
		}


		catch (const DOMException& e)
		{
		   cerr << "A DOM error occured during parsing\n   DOMException code: "
				 << e.code << endl;
			errorsOccured = true;
		}

		if (errorsOccured) {

			cout << "Errors during parse" << endl;
			return (2);

		}

		/*

			Now that we have the parsed file, get the DOM document and start looking at it

		*/
		
		doc = parser->adoptDocument();
	}

	else {
		// Create an empty document
		XMLCh tempStr[100];
		XMLString::transcode("Core", tempStr, 99);    
		DOMImplementation *impl = DOMImplementationRegistry::getDOMImplementation(tempStr);
		doc = impl->createDocument(
			0,                    // root element namespace URI.
			MAKE_UNICODE_STRING("ADoc"),            // root element name
			NULL);// DOMDocumentType());  // document type object (DTD).
	}


	XSECProvider prov;
	XENCCipher * cipher = prov.newCipher(doc);

	if (kek != NULL)
		cipher->setKEK(kek);
	if (key != NULL)
		cipher->setKey(key);

	try {

		if (doDecrypt) {

			if (useInteropResolver == true) {

				// Map out base path of the file
				char path[_MAX_PATH];
				char baseURI[(_MAX_PATH * 2) + 10];
				getcwd(path, _MAX_PATH);

				strcpy(baseURI, "file:///");		

				// Ugly and nasty but quick
				if (filename[0] != '\\' && filename[0] != '/' && filename[1] != ':') {
					strcat(baseURI, path);
					strcat(baseURI, "/");
				} else if (path[1] == ':') {
					path[2] = '\0';
					strcat(baseURI, path);
				}

				strcat(baseURI, filename);

				// Find any ':' and "\" characters
				int lastSlash = 0;
				for (unsigned int i = 8; i < strlen(baseURI); ++i) {
					if (baseURI[i] == '\\') {
						lastSlash = i;
						baseURI[i] = '/';
					}
					else if (baseURI[i] == '/')
						lastSlash = i;
				}

				// The last "\\" must prefix the filename
				baseURI[lastSlash + 1] = '\0';

				XMLCh * uriT = XMLString::transcode(baseURI);

				XencInteropResolver ires(doc, &(uriT[8]));
				XSEC_RELEASE_XMLCH(uriT);
				cipher->setKeyInfoResolver(&ires);

			}
			// Find the EncryptedData node
			DOMNode * n = findXENCNode(doc, "EncryptedData");

			if (doDecryptElement) {
				while (n != NULL) {

					// decrypt
					cipher->decryptElement(static_cast<DOMElement *>(n));

					// Find the next EncryptedData node
					n = findXENCNode(doc, "EncryptedData");
				}

			}
			else {
				XSECBinTXFMInputStream * bis = cipher->decryptToBinInputStream(static_cast<DOMElement *>(n));
				Janitor<XSECBinTXFMInputStream> j_bis(bis);
	
				XMLByte buf[1024];			
				xsecsize_t read = bis->readBytes(buf, 1023);
				while (read > 0) {
					formatTarget->writeChars(buf, read, NULL);
					read = bis->readBytes(buf, 1023);
				}
			}
		}
		else {

			XENCEncryptedData *xenc = NULL;
			// Encrypting
			if (kek != NULL && key == NULL) {
				XSECPlatformUtils::g_cryptoProvider->getRandom(keyBuf, 24);
				XSECCryptoSymmetricKey * k = 
					XSECPlatformUtils::g_cryptoProvider->keySymmetric(XSECCryptoSymmetricKey::KEY_3DES_192);
				k->setKey(keyBuf, 24);
				cipher->setKey(k);
				keyAlg = ENCRYPT_3DES_CBC;
				keyStr = keyBuf;
				keyLen = 24;
			}

			if (encryptFileAsData) {

				// Create a BinInputStream
#if defined(XSEC_XERCES_REQUIRES_MEMMGR)
				BinFileInputStream * is = new BinFileInputStream(filename, XMLPlatformUtils::fgMemoryManager);
#else
				BinFileInputStream * is = new BinFileInputStream(filename);
#endif
				xenc = cipher->encryptBinInputStream(is, keyAlg);

				// Replace the document element
				DOMElement * elt = doc->getDocumentElement();
				doc->replaceChild(xenc->getElement(), elt);
				elt->release();
			}
			else {
				// Document encryption
				cipher->encryptElement(doc->getDocumentElement(), keyAlg);
			}

			// Do we encrypt a created key?
			if (kek != NULL && xenc != NULL) {
				XENCEncryptedKey *xkey = cipher->encryptKey(keyStr, keyLen, kekAlg);
				// Add to the EncryptedData
				xenc->appendEncryptedKey(xkey);
			}
		}

		if (doXMLOutput) {
			// Output the result

			XMLCh core[] = {
				XERCES_CPP_NAMESPACE_QUALIFIER chLatin_C,
				XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o,
				XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r,
				XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e,
				XERCES_CPP_NAMESPACE_QUALIFIER chNull
			};

            DOMImplementation *impl = DOMImplementationRegistry::getDOMImplementation(core);

#if defined (XSEC_XERCES_DOMLSSERIALIZER)
            // DOM L3 version as per Xerces 3.0 API
            DOMLSSerializer   *theSerializer = ((DOMImplementationLS*)impl)->createLSSerializer();
            Janitor<DOMLSSerializer> j_theSerializer(theSerializer);
            
            // Get the config so we can set up pretty printing
            DOMConfiguration *dc = theSerializer->getDomConfig();
            dc->setParameter(XMLUni::fgDOMWRTFormatPrettyPrint, false);

            // Now create an output object to format to UTF-8
            DOMLSOutput *theOutput = ((DOMImplementationLS*)impl)->createLSOutput();
            Janitor<DOMLSOutput> j_theOutput(theOutput);

            theOutput->setEncoding(MAKE_UNICODE_STRING("UTF-8"));
            theOutput->setByteStream(formatTarget);

            theSerializer->write(doc, theOutput);

#else			
			DOMWriter         *theSerializer = ((DOMImplementationLS*)impl)->createDOMWriter();
			Janitor<DOMWriter> j_theSerializer(theSerializer);

			theSerializer->setEncoding(MAKE_UNICODE_STRING("UTF-8"));
			if (theSerializer->canSetFeature(XMLUni::fgDOMWRTFormatPrettyPrint, false))
				theSerializer->setFeature(XMLUni::fgDOMWRTFormatPrettyPrint, false);

			theSerializer->writeNode(formatTarget, *doc);
#endif	
			cout << endl;

		}
	}

	catch (XSECException &e) {
		char * msg = XMLString::transcode(e.getMsg());
		cerr << "An error occured during encryption/decryption operation\n   Message: "
		<< msg << endl;
		XSEC_RELEASE_XMLCH(msg);
		errorsOccured = true;
		if (formatTarget != NULL)
			delete formatTarget;
		doc->release();
		return 2;
	}
	catch (XSECCryptoException &e) {
		cerr << "An error occured during encryption/decryption operation\n   Message: "
		<< e.getMsg() << endl;
		errorsOccured = true;
		if (formatTarget != NULL)
			delete formatTarget;
		doc->release();

#if defined (XSEC_HAVE_OPENSSL)
		ERR_load_crypto_strings();
		BIO * bio_err;
		if ((bio_err=BIO_new(BIO_s_file())) != NULL)
			BIO_set_fp(bio_err,stderr,BIO_NOCLOSE|BIO_FP_TEXT);

		ERR_print_errors(bio_err);
#endif
		return 2;
	}
	
	if (formatTarget != NULL)
		delete formatTarget;

	doc->release();
	return 0;
}


int main(int argc, char **argv) {

	int retResult;

#if defined (_DEBUG) && defined (_MSC_VER)

	// Do some memory debugging under Visual C++

	_CrtMemState s1, s2, s3;

	// At this point we are about to start really using XSEC, so
	// Take a "before" checkpoing

	_CrtMemCheckpoint( &s1 );

#endif

	// Initialise the XML system

	try {

		XMLPlatformUtils::Initialize();
#ifndef XSEC_NO_XALAN
		XPathEvaluator::initialize();
		XalanTransformer::initialize();
#endif
		XSECPlatformUtils::Initialise();

	}
	catch (const XMLException &e) {

		cerr << "Error during initialisation of Xerces" << endl;
		cerr << "Error Message = : "
		     << e.getMessage() << endl;

	}

	retResult = evaluate(argc, argv);

	XSECPlatformUtils::Terminate();
#ifndef XSEC_NO_XALAN
	XalanTransformer::terminate();
	XPathEvaluator::terminate();
#endif
	XMLPlatformUtils::Terminate();

#if defined (_DEBUG) && defined (_MSC_VER)

	_CrtMemCheckpoint( &s2 );

	if ( _CrtMemDifference( &s3, &s1, &s2 ) && (
		s3.lCounts[0] > 0 ||
		s3.lCounts[1] > 1 ||
		// s3.lCounts[2] > 2 ||  We don't worry about C Runtime
		s3.lCounts[3] > 0 ||
		s3.lCounts[4] > 0)) {

		// Note that there is generally 1 Normal and 1 CRT block
		// still taken.  1 is from Xalan and 1 from stdio

		// Send all reports to STDOUT
		_CrtSetReportMode( _CRT_WARN, _CRTDBG_MODE_FILE );
		_CrtSetReportFile( _CRT_WARN, _CRTDBG_FILE_STDOUT );
		_CrtSetReportMode( _CRT_ERROR, _CRTDBG_MODE_FILE );
		_CrtSetReportFile( _CRT_ERROR, _CRTDBG_FILE_STDOUT );
		_CrtSetReportMode( _CRT_ASSERT, _CRTDBG_MODE_FILE );
		_CrtSetReportFile( _CRT_ASSERT, _CRTDBG_FILE_STDOUT );

		// Dumpy memory stats

 		_CrtMemDumpAllObjectsSince( &s3 );
	    _CrtMemDumpStatistics( &s3 );
	}

	// Now turn off memory leak checking and end as there are some 
	// Globals that are allocated that get seen as leaks (Xalan?)

	int dbgFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
	dbgFlag &= ~(_CRTDBG_LEAK_CHECK_DF);
	_CrtSetDbgFlag( dbgFlag );

#endif

	return retResult;
}
