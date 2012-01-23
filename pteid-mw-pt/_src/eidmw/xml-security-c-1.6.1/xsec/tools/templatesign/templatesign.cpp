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
 * templatesign := tool to sign a template XML signature file
 *
 * Author(s): Berin Lautenbach
 *
 * $Id: templatesign.cpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

//XSEC includes
// XSEC

#include <xsec/utils/XSECPlatformUtils.hpp>
#include <xsec/framework/XSECProvider.hpp>
#include <xsec/canon/XSECC14n20010315.hpp>
#include <xsec/dsig/DSIGSignature.hpp>
#include <xsec/dsig/DSIGKeyInfoX509.hpp>
#include <xsec/framework/XSECException.hpp>
#include <xsec/utils/XSECDOMUtils.hpp>
#include <xsec/enc/XSECCryptoException.hpp>

#if defined(_WIN32)
#	include <xsec/utils/winutils/XSECURIResolverGenericWin32.hpp>
#else
#	include <xsec/utils/unixutils/XSECURIResolverGenericUnix.hpp>
#endif

#if defined (XSEC_HAVE_OPENSSL)
#	include <xsec/enc/OpenSSL/OpenSSLCryptoKeyDSA.hpp>
#	include <xsec/enc/OpenSSL/OpenSSLCryptoKeyEC.hpp>
#	include <xsec/enc/OpenSSL/OpenSSLCryptoKeyRSA.hpp>
#	include <xsec/enc/OpenSSL/OpenSSLCryptoKeyHMAC.hpp>
#	include <xsec/enc/OpenSSL/OpenSSLCryptoX509.hpp>

#	include <openssl/bio.h>
#	include <openssl/dsa.h>
#	include <openssl/err.h>
#	include <openssl/evp.h>
#	include <openssl/pem.h>
#endif

#if defined(XSEC_HAVE_WINCAPI)
#	include <xsec/enc/WinCAPI/WinCAPICryptoProvider.hpp>
#	include <xsec/enc/WinCAPI/WinCAPICryptoKeyDSA.hpp>
#	include <xsec/enc/WinCAPI/WinCAPICryptoKeyRSA.hpp>
#	include <xsec/enc/WinCAPI/WinCAPICryptoKeyHMAC.hpp>
#endif

#include <memory.h>
#include <string.h>
#include <iostream>
#include <stdlib.h>

#if defined(HAVE_UNISTD_H)
# include <unistd.h>
# define _MAX_PATH PATH_MAX
#else
# if defined(HAVE_DIRECT_H)
#  include <direct.h>
# endif
#endif

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/XMLException.hpp>
#include <xercesc/util/XMLUniDefs.hpp>
#include <xercesc/util/XMLNetAccessor.hpp>
#include <xercesc/util/XMLUri.hpp>

#ifndef XSEC_NO_XALAN

// XALAN

#include <xalanc/XPath/XPathEvaluator.hpp>
#include <xalanc/XalanTransformer/XalanTransformer.hpp>

XALAN_USING_XALAN(XPathEvaluator)
XALAN_USING_XALAN(XalanTransformer)

#endif

using std::ostream;
using std::cout;
using std::cerr;
using std::endl;
using std::flush;

// Uplift entire program into Xerces namespace

XERCES_CPP_NAMESPACE_USE

// --------------------------------------------------------------------------------
//           Much code taken from the DOMPrint Xerces example
// --------------------------------------------------------------------------------

static XMLFormatter*            gFormatter             = 0;
static XMLCh*                   gEncodingName          = 0;
static XMLFormatter::UnRepFlags gUnRepFlags            = XMLFormatter::UnRep_CharRef;




static const XMLCh  gEndElement[] = { chOpenAngle, chForwardSlash, chNull };
static const XMLCh  gEndPI[] = { chQuestion, chCloseAngle, chNull};
static const XMLCh  gStartPI[] = { chOpenAngle, chQuestion, chNull };
static const XMLCh  gXMLDecl1[] =
{
        chOpenAngle, chQuestion, chLatin_x, chLatin_m, chLatin_l
    ,   chSpace, chLatin_v, chLatin_e, chLatin_r, chLatin_s, chLatin_i
    ,   chLatin_o, chLatin_n, chEqual, chDoubleQuote, chNull
};
static const XMLCh  gXMLDecl2[] =
{
        chDoubleQuote, chSpace, chLatin_e, chLatin_n, chLatin_c
    ,   chLatin_o, chLatin_d, chLatin_i, chLatin_n, chLatin_g, chEqual
    ,   chDoubleQuote, chNull
};
static const XMLCh  gXMLDecl3[] =
{
        chDoubleQuote, chSpace, chLatin_s, chLatin_t, chLatin_a
    ,   chLatin_n, chLatin_d, chLatin_a, chLatin_l, chLatin_o
    ,   chLatin_n, chLatin_e, chEqual, chDoubleQuote, chNull
};
static const XMLCh  gXMLDecl4[] =
{
        chDoubleQuote, chQuestion, chCloseAngle
    ,   chLF, chNull
};

static const XMLCh  gStartCDATA[] =
{
        chOpenAngle, chBang, chOpenSquare, chLatin_C, chLatin_D,
        chLatin_A, chLatin_T, chLatin_A, chOpenSquare, chNull
};

static const XMLCh  gEndCDATA[] =
{
    chCloseSquare, chCloseSquare, chCloseAngle, chNull
};
static const XMLCh  gStartComment[] =
{
    chOpenAngle, chBang, chDash, chDash, chNull
};

static const XMLCh  gEndComment[] =
{
    chDash, chDash, chCloseAngle, chNull
};

static const XMLCh  gStartDoctype[] =
{
    chOpenAngle, chBang, chLatin_D, chLatin_O, chLatin_C, chLatin_T,
    chLatin_Y, chLatin_P, chLatin_E, chSpace, chNull
};
static const XMLCh  gPublic[] =
{
    chLatin_P, chLatin_U, chLatin_B, chLatin_L, chLatin_I,
    chLatin_C, chSpace, chDoubleQuote, chNull
};
static const XMLCh  gSystem[] =
{
    chLatin_S, chLatin_Y, chLatin_S, chLatin_T, chLatin_E,
    chLatin_M, chSpace, chDoubleQuote, chNull
};
static const XMLCh  gStartEntity[] =
{
    chOpenAngle, chBang, chLatin_E, chLatin_N, chLatin_T, chLatin_I,
    chLatin_T, chLatin_Y, chSpace, chNull
};
static const XMLCh  gNotation[] =
{
    chLatin_N, chLatin_D, chLatin_A, chLatin_T, chLatin_A,
    chSpace, chDoubleQuote, chNull
};



// ---------------------------------------------------------------------------
//  Local classes
// ---------------------------------------------------------------------------

class DOMPrintFormatTarget : public XMLFormatTarget
{
public:
    DOMPrintFormatTarget()  {};
    ~DOMPrintFormatTarget() {};

    // -----------------------------------------------------------------------
    //  Implementations of the format target interface
    // -----------------------------------------------------------------------

    void writeChars(const   XMLByte* const  toWrite,
                    const   xsecsize_t    count,
                            XMLFormatter * const formatter)
    {
        // Surprisingly, Solaris was the only platform on which
        // required the char* cast to print out the string correctly.
        // Without the cast, it was printing the pointer value in hex.
        // Quite annoying, considering every other platform printed
        // the string with the explicit cast to char* below.
        cout.write((char *) toWrite, (int) count);
    };

private:
    // -----------------------------------------------------------------------
    //  Unimplemented methods.
    // -----------------------------------------------------------------------
    DOMPrintFormatTarget(const DOMPrintFormatTarget& other);
    void operator=(const DOMPrintFormatTarget& rhs);
};


// ---------------------------------------------------------------------------
//  ostream << DOMNode
//
//  Stream out a DOM node, and, recursively, all of its children. This
//  function is the heart of writing a DOM tree out as XML source. Give it
//  a document node and it will do the whole thing.
// ---------------------------------------------------------------------------
ostream& operator<<(ostream& target, DOMNode* toWrite)
{
    // Get the name and value out for convenience
    const XMLCh*   nodeName = toWrite->getNodeName();
    const XMLCh*   nodeValue = toWrite->getNodeValue();
    xsecsize_t lent = XMLString::stringLen(nodeValue);

    switch (toWrite->getNodeType())
    {
        case DOMNode::TEXT_NODE:
        {
            gFormatter->formatBuf(nodeValue,
                                  lent, XMLFormatter::CharEscapes);
            break;
        }


        case DOMNode::PROCESSING_INSTRUCTION_NODE :
        {
            *gFormatter << XMLFormatter::NoEscapes << gStartPI  << nodeName;
            if (lent > 0)
            {
                *gFormatter << chSpace << nodeValue;
            }
            *gFormatter << XMLFormatter::NoEscapes << gEndPI;
            break;
        }


        case DOMNode::DOCUMENT_NODE :
        {

            DOMNode *child = toWrite->getFirstChild();
            while( child != 0)
            {
                target << child;
                // add linefeed in requested output encoding
                *gFormatter << chLF;
                target << flush;
                child = child->getNextSibling();
            }
            break;
        }


        case DOMNode::ELEMENT_NODE :
        {
            // The name has to be representable without any escapes
            *gFormatter  << XMLFormatter::NoEscapes
                         << chOpenAngle << nodeName;

            // Output the element start tag.

            // Output any attributes on this element
            DOMNamedNodeMap *attributes = toWrite->getAttributes();
            XMLSize_t attrCount = attributes->getLength();
            for (XMLSize_t i = 0; i < attrCount; i++)
            {
                DOMNode  *attribute = attributes->item(i);

                //
                //  Again the name has to be completely representable. But the
                //  attribute can have refs and requires the attribute style
                //  escaping.
                //
                *gFormatter  << XMLFormatter::NoEscapes
                             << chSpace << attribute->getNodeName()
                             << chEqual << chDoubleQuote
                             << XMLFormatter::AttrEscapes
                             << attribute->getNodeValue()
                             << XMLFormatter::NoEscapes
                             << chDoubleQuote;
            }

            //
            //  Test for the presence of children, which includes both
            //  text content and nested elements.
            //
            DOMNode *child = toWrite->getFirstChild();
            if (child != 0)
            {
                // There are children. Close start-tag, and output children.
                // No escapes are legal here
                *gFormatter << XMLFormatter::NoEscapes << chCloseAngle;

                while( child != 0)
                {
                    target << child;
                    child = child->getNextSibling();
                }

                //
                // Done with children.  Output the end tag.
                //
                *gFormatter << XMLFormatter::NoEscapes << gEndElement
                            << nodeName << chCloseAngle;
            }
            else
            {
                //
                //  There were no children. Output the short form close of
                //  the element start tag, making it an empty-element tag.
                //
                *gFormatter << XMLFormatter::NoEscapes << chForwardSlash << chCloseAngle;
            }
            break;
        }


        case DOMNode::ENTITY_REFERENCE_NODE:
            {
                //DOMNode *child;
#if 0
                for (child = toWrite.getFirstChild();
                child != 0;
                child = child.getNextSibling())
                {
                    target << child;
                }
#else
                //
                // Instead of printing the refernece tree
                // we'd output the actual text as it appeared in the xml file.
                // This would be the case when -e option was chosen
                //
                    *gFormatter << XMLFormatter::NoEscapes << chAmpersand
                        << nodeName << chSemiColon;
#endif
                break;
            }


        case DOMNode::CDATA_SECTION_NODE:
            {
            *gFormatter << XMLFormatter::NoEscapes << gStartCDATA
                        << nodeValue << gEndCDATA;
            break;
        }


        case DOMNode::COMMENT_NODE:
        {
            *gFormatter << XMLFormatter::NoEscapes << gStartComment
                        << nodeValue << gEndComment;
            break;
        }


        case DOMNode::DOCUMENT_TYPE_NODE:
        {
            DOMDocumentType *doctype = (DOMDocumentType *)toWrite;;

            *gFormatter << XMLFormatter::NoEscapes  << gStartDoctype
                        << nodeName;

            const XMLCh* id = doctype->getPublicId();
            if (id != 0)
            {
                *gFormatter << XMLFormatter::NoEscapes << chSpace << gPublic
                    << id << chDoubleQuote;
                id = doctype->getSystemId();
                if (id != 0)
                {
                    *gFormatter << XMLFormatter::NoEscapes << chSpace
                       << chDoubleQuote << id << chDoubleQuote;
                }
            }
            else
            {
                id = doctype->getSystemId();
                if (id != 0)
                {
                    *gFormatter << XMLFormatter::NoEscapes << chSpace << gSystem
                        << id << chDoubleQuote;
                }
            }

            id = doctype->getInternalSubset();
            if (id !=0)
                *gFormatter << XMLFormatter::NoEscapes << chOpenSquare
                            << id << chCloseSquare;

            *gFormatter << XMLFormatter::NoEscapes << chCloseAngle;
            break;
        }


        case DOMNode::ENTITY_NODE:
        {
            *gFormatter << XMLFormatter::NoEscapes << gStartEntity
                        << nodeName;

            const XMLCh * id = ((DOMEntity *)toWrite)->getPublicId();
            if (id != 0)
                *gFormatter << XMLFormatter::NoEscapes << gPublic
                            << id << chDoubleQuote;

            id = ((DOMEntity *)toWrite)->getSystemId();
            if (id != 0)
                *gFormatter << XMLFormatter::NoEscapes << gSystem
                            << id << chDoubleQuote;

            id = ((DOMEntity *)toWrite)->getNotationName();
            if (id != 0)
                *gFormatter << XMLFormatter::NoEscapes << gNotation
                            << id << chDoubleQuote;

            *gFormatter << XMLFormatter::NoEscapes << chCloseAngle << chLF;

            break;
        }

/*
        case DOMNode::NOTATION_NODE:
        {
            const XMLCh *  str;

            *gFormatter << gXMLDecl1 << ((DOMXMLDecl *)toWrite)->getVersion();

            *gFormatter << gXMLDecl2 << gEncodingName;

            str = ((DOMXMLDecl *)toWrite)->getStandalone();
            if (str != 0)
                *gFormatter << gXMLDecl3 << str;

            *gFormatter << gXMLDecl4;

            break;
        }

*/
        default:
            cerr << "Unrecognized node type = "
                 << (long)toWrite->getNodeType() << endl;
    }
    return target;
}


// --------------------------------------------------------------------------------
//           End of outputter
// --------------------------------------------------------------------------------

class DOMMemFormatTarget : public XMLFormatTarget
{
public:
    
	unsigned char * buffer;		// Buffer to write to

	DOMMemFormatTarget()  {};
    ~DOMMemFormatTarget() {};

	void setBuffer (unsigned char * toSet) {buffer = toSet;};


    // -----------------------------------------------------------------------
    //  Implementations of the format target interface
    // -----------------------------------------------------------------------

    void writeChars(const   XMLByte* const  toWrite,
                    const   unsigned int    count,
                            XMLFormatter * const formatter)
    {
        // Surprisingly, Solaris was the only platform on which
        // required the char* cast to print out the string correctly.
        // Without the cast, it was printing the pointer value in hex.
        // Quite annoying, considering every other platform printed
        // the string with the explicit cast to char* below.
        memcpy(buffer, (char *) toWrite, (int) count);
		buffer[count] = '\0';
    };

private:
    // -----------------------------------------------------------------------
    //  Unimplemented methods.
    // -----------------------------------------------------------------------
    DOMMemFormatTarget(const DOMMemFormatTarget& other);
    void operator=(const DOMMemFormatTarget& rhs);

	
};

// ---------------------------------------------------------------------------
//  ostream << DOMString
//
//  Stream out a DOM string. Doing this requires that we first transcode
//  to char * form in the default code page for the system
// ---------------------------------------------------------------------------


DOMPrintFormatTarget *DOMtarget;
DOMMemFormatTarget *MEMtarget;
XMLFormatter *formatter, *MEMformatter;
unsigned char *charBuffer;

void printUsage(void) {

	cerr << "\nUsage: templatesign <key options> <file to sign>\n\n";
#if defined (XSEC_HAVE_OPENSSL)
	cerr << "    Where <key options> are one of :\n\n";
	cerr << "        --x509subjectname/-s <distinguished name>\n";
	cerr << "                     <distinguished name> will be set as SubjectName in x509\n";
	cerr << "        --dsakey/-d  <dsa private key file> <password>\n";
	cerr << "                     <dsa private key file> contains a PEM encoded private key\n";
	cerr << "                     <password> is the password used to decrypt the key file\n";
#	if defined (XSEC_HAVE_WINCAPI)
	cerr << "                     NOTE: Not usable if --wincapi previously set\n";
#	endif
#   if defined(XSEC_OPENSSL_HAVE_EC)
	cerr << "        --eckey/-e  <ec private key file> <password>\n";
	cerr << "                     <ec private key file> contains a PEM encoded private key\n";
	cerr << "                     <password> is the password used to decrypt the key file\n";
#   endif
#	if defined (XSEC_HAVE_WINCAPI)
	cerr << "                     NOTE: Not usable if --wincapi previously set\n";
#	endif
	cerr << "        --rsakey/-r <rsa private key file> <password>\n";
	cerr << "                     <rsa privatekey file> contains a PEM encoded private key\n";
	cerr << "                     <password> is the password used to decrypt the key file\n";
	cerr << "        --x509cert/-x <filename>\n";
	cerr << "                      <filename> contains a PEM certificate to be added as a KeyInfo\n";
#endif
	cerr << "        --hmackey/-h <string>\n";
	cerr << "                     <string> is the hmac key to set\n";
	cerr << "        --clearkeys/-c\n";
	cerr << "                      Clears out any current KeyInfo elements in the file\n";
#if defined(XSEC_HAVE_WINCAPI)
	cerr << "        --windss/-wd\n";
	cerr << "                      Use the default user AT_SIGNATURE key from default\n";
	cerr << "                      Windows DSS CSP\n";
	cerr << "        --winrsa/-wr\n";
	cerr << "                      Use the default user AT_SIGNATURE key from default\n";
	cerr << "                      Windows RSA CSP\n";
	cerr << "        --winhmac/-wh <string>\n";
	cerr << "                      Create a windows HMAC key using <string> as the password.\n";
	cerr << "                      Uses a SHA-1 hash of the password to derive a key\n";
#if defined (CRYPT_ACQUIRE_CACHE_FLAG)
	cerr << "        --wincer/-wc <Subject Name>\n";
	cerr << "                      Use the private key associated with the named certificate in the Windows certificate store\n";
#endif /* CRYPT_ACQUIRE_CACHE_FLAG */
	cerr << "        --windsskeyinfo/-wdi\n";
	cerr << "                      Clear KeyInfo elements and insert DSS parameters from windows key\n";
	cerr << "        --winrsakeyinfo/-wri\n";
	cerr << "                      Clear KeyInfo elements and insert RSA parameters from windows key\n";
#endif


}

int main(int argc, char **argv) {

	XSECCryptoKey				* key = NULL;
	DSIGKeyInfoX509				* keyInfoX509 = NULL;
	const char					* x509SubjectName = NULL;
#if defined (XSEC_HAVE_OPENSSL)
	OpenSSLCryptoX509			* certs[128];
#endif
	int							certCount = 0;
	int							paramCount;
	bool						clearKeyInfo = false;
#if defined(XSEC_HAVE_WINCAPI)
	HCRYPTPROV					win32DSSCSP = 0;		// Crypto Provider
	HCRYPTPROV					win32RSACSP = 0;		// Crypto Provider
	bool						winDssKeyInfo = false;
	bool						winRsaKeyInfo = false;
	WinCAPICryptoKeyDSA			* winKeyDSA = NULL;
	WinCAPICryptoKeyRSA			* winKeyRSA = NULL;
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

#if defined (XSEC_HAVE_OPENSSL)
	
	// Initialise OpenSSL
	ERR_load_crypto_strings();
	BIO * bio_err;
	
	if ((bio_err=BIO_new(BIO_s_file())) != NULL)
		BIO_set_fp(bio_err,stderr,BIO_NOCLOSE|BIO_FP_TEXT);

#endif

	if (argc < 2) {

		printUsage();
		exit (1);
	}
	
	paramCount = 1;

	while (paramCount < argc - 1) {

		// Run through all parameters

		if (strcasecmp(argv[paramCount], "--x509subjectname") == 0 || strcasecmp(argv[paramCount], "-s") == 0) {

			if (paramCount +2 >= argc) {

				printUsage();
				exit(1);
			}

			// Get the subject name

			x509SubjectName = argv[paramCount + 1];
			paramCount += 2;
		}

#if defined (XSEC_HAVE_OPENSSL)

		else if (strcasecmp(argv[paramCount], "--dsakey") == 0 || strcasecmp(argv[paramCount], "-d") == 0
			|| strcasecmp(argv[paramCount], "--rsakey") == 0 || strcasecmp(argv[paramCount], "-r") == 0
#   if defined(XSEC_OPENSSL_HAVE_EC)
			|| strcasecmp(argv[paramCount], "--eckey") == 0 || strcasecmp(argv[paramCount], "-e") == 0
#   endif
            ) {

			// OpenSSL Key

			if (paramCount + 3 >= argc) {

				printUsage();
				exit (1);

			}

			if (key != 0) {

				cerr << "\nError loading private key - another key already loaded\n\n";
				printUsage();
				exit(1);

			}

			// Load the signing key
			// For now just read a particular file

			BIO * bioKey;
			if ((bioKey = BIO_new(BIO_s_file())) == NULL) {

				cerr << "Error opening private key file\n\n";
				exit (1);

			}

			if (BIO_read_filename(bioKey, argv[paramCount + 1]) <= 0) {

				cerr << "Error opening private key file\n\n";
				exit (1);

			}

			EVP_PKEY * pkey;
			pkey = PEM_read_bio_PrivateKey(bioKey,NULL,NULL,argv[paramCount + 2]);

			if (pkey == NULL) {

				cerr << "Error loading private key\n\n";
				ERR_print_errors(bio_err);
				exit (1);

			}

			if (strcasecmp(argv[paramCount], "--dsakey") == 0 || strcasecmp(argv[paramCount], "-d") == 0) {

				// Check type is correct

				if (pkey->type != EVP_PKEY_DSA) {
					cerr << "DSA Key requested, but OpenSSL loaded something else\n";
					exit (1);
				}

				// Create the XSEC OpenSSL interface
				key = new OpenSSLCryptoKeyDSA(pkey);
			}
#   if defined(XSEC_OPENSSL_HAVE_EC)
			else if (strcasecmp(argv[paramCount], "--eckey") == 0 || strcasecmp(argv[paramCount], "-e") == 0) {

				// Check type is correct

				if (pkey->type != EVP_PKEY_EC) {
					cerr << "EC Key requested, but OpenSSL loaded something else\n";
					exit (1);
				}

				// Create the XSEC OpenSSL interface
				key = new OpenSSLCryptoKeyEC(pkey);
			}
#   endif
            else {
				if (pkey->type != EVP_PKEY_RSA) {
					cerr << "RSA Key requested, but OpenSSL loaded something else\n";
					exit (1);
				}
				key = new OpenSSLCryptoKeyRSA(pkey);
			}

			EVP_PKEY_free(pkey);
			BIO_free(bioKey);

			paramCount += 3;
			
		} /* argv[1] = "dsa/rsa/ec" */


		else if (strcasecmp(argv[paramCount], "--x509cert") == 0 || strcasecmp(argv[paramCount], "-x") == 0) {

			// X509Data keyInfo

			if (paramCount + 2 >= argc) {

				printUsage();
				exit (1);

			}

			// Load the cert.
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

				cerr << "Error loading certificate\n\n";
				ERR_print_errors(bio_err);
				exit (1);

			}

			// Create the XSEC OpenSSL interface - used only to translate to Base64

			certs[certCount++] = new OpenSSLCryptoX509(x);
			X509_free(x);
			BIO_free(bioX509);

			paramCount += 2;
			
		} /* argv[1] = "--x509cert" */
		
		else 
#endif
		if (strcasecmp(argv[paramCount], "--hmackey") == 0 || strcasecmp(argv[paramCount], "-h") == 0) {

#if defined (XSEC_HAVE_OPENSSL)
			OpenSSLCryptoKeyHMAC * hmacKey = new OpenSSLCryptoKeyHMAC();
#else
#	if defined (XSEC_HAVE_WINCAPI)
			WinCAPICryptoKeyHMAC * hmacKey = new WinCAPICryptoKeyHMAC(0);
#	endif
#endif
			hmacKey->setKey((unsigned char *) argv[paramCount + 1], (unsigned int) strlen(argv[paramCount + 1]));
			key = hmacKey;
			paramCount += 2;

		}

		else if (strcasecmp(argv[paramCount], "--clearkeys") == 0 || strcasecmp(argv[paramCount], "-c") == 0) {

			clearKeyInfo = true;
			paramCount += 1;

		}

#if defined (XSEC_HAVE_WINCAPI)
		else if (strcasecmp(argv[paramCount], "--windss") == 0 || strcasecmp(argv[paramCount], "-wd") == 0) {

			WinCAPICryptoProvider * cp;
			// First set windows as the crypto provider
			cp = new WinCAPICryptoProvider();
			XSECPlatformUtils::SetCryptoProvider(cp);
			
			// Now set the key
			if (!CryptAcquireContext(&win32DSSCSP,
				NULL,
				NULL,
				PROV_DSS,
				0)) {

				cerr << "Error acquiring Crypto context - Attempting to generate new key pair" << endl;
				
				// Attempt to gen a new keyset
				if (!CryptAcquireContext(&win32DSSCSP,
					NULL,
					NULL,
					PROV_DSS,
					CRYPT_NEWKEYSET)) {
						cerr << "Error acquiring DSS Crypto Service Provider with new keyset" << endl;
						return 2;
				}
				else {
					HCRYPTKEY k;
					if (!CryptGenKey(win32DSSCSP, AT_SIGNATURE, CRYPT_EXPORTABLE, &k)) {
						cerr << "Error generating DSS keyset" << endl;
						return 2;
					}
					CryptDestroyKey(k);
				}
			}
			
			winKeyDSA = new WinCAPICryptoKeyDSA(win32DSSCSP, AT_SIGNATURE, true);
			key = winKeyDSA;
			paramCount++;
		}

		else if (strcasecmp(argv[paramCount], "--winrsa") == 0 || strcasecmp(argv[paramCount], "-wr") == 0) {
			WinCAPICryptoProvider * cp;
			cp = new WinCAPICryptoProvider();
			XSECPlatformUtils::SetCryptoProvider(cp);

			if (!CryptAcquireContext(&win32RSACSP,
				NULL,
				NULL,
				PROV_RSA_FULL,
				0)) {

				cerr << "Error acquiring Crypto context - Attempting to generate new RSA key pair" << endl;
				
				// Attempt to gen a new keyset
				if (!CryptAcquireContext(&win32RSACSP,
					NULL,
					NULL,
					PROV_RSA_FULL,
					CRYPT_NEWKEYSET)) {
						cerr << "Error acquiring RSA Crypto Service Provider with new keyset" << endl;
						return 2;
				}
				else {
					HCRYPTKEY k;
					if (!CryptGenKey(win32RSACSP, AT_SIGNATURE, CRYPT_EXPORTABLE, &k)) {
						cerr << "Error generating RSA keyset" << endl;
						return 2;
					}
					CryptDestroyKey(k);
				}
			}

			winKeyRSA = new WinCAPICryptoKeyRSA(win32RSACSP, AT_SIGNATURE, true);
			key = winKeyRSA;
			paramCount++;
		}

		else if (strcasecmp(argv[paramCount], "--winhmac") == 0 || strcasecmp(argv[paramCount], "-wh") == 0) {

			WinCAPICryptoProvider * cp;
			// Obtain default PROV_RSA, with default user key container
			// Note we open in VERIFYCONTEXT as we do not require a assymetric key pair
			if (!CryptAcquireContext(&win32RSACSP,
				NULL,
				NULL,
				PROV_RSA_FULL,
				CRYPT_VERIFYCONTEXT)) {
					cerr << "Error acquiring RSA Crypto Service Provider" << endl;
					return 2;
			}
			cp = new WinCAPICryptoProvider();
			XSECPlatformUtils::SetCryptoProvider(cp);

			paramCount++;
			HCRYPTKEY k;
			HCRYPTHASH h;
			BOOL fResult = CryptCreateHash(
				win32RSACSP,
				CALG_SHA,
				0,
				0,
				&h);

			if (fResult == 0) {
				cerr << "Error creating hash to create windows hmac key from password" << endl;
				return 2;
			}
			fResult = CryptHashData(
				h,
				(unsigned char *) argv[paramCount],
				(DWORD) strlen(argv[paramCount]),
				0);
			
			if (fResult == 0) {
				cerr << "Error hashing password to create windows hmac key" << endl;
				return 2;
			}

			// Now create a key
			fResult = CryptDeriveKey(
				win32RSACSP,
				CALG_RC2,
				h,
				CRYPT_EXPORTABLE,
				&k);

			if (fResult == 0) {
				cerr << "Error deriving key from hash value" << endl;
				return 2;
			}

			// Wrap in a WinCAPI object
			WinCAPICryptoKeyHMAC * hk;
			hk = new WinCAPICryptoKeyHMAC(win32RSACSP);
			hk->setWinKey(k); 

			key = hk;

			CryptDestroyHash(h);
			paramCount++;

		}

		else if (strcasecmp(argv[paramCount], "--windsskeyinfo") == 0 || strcasecmp(argv[paramCount], "-wdi") == 0) {
			winDssKeyInfo = true;
			paramCount++;
		}

		else if (strcasecmp(argv[paramCount], "--winrsakeyinfo") == 0 || strcasecmp(argv[paramCount], "-wri") == 0) {
			winRsaKeyInfo = true;
			paramCount++;
		}

		// Need to find a better way to check this
		// If CryptAcquireCertificatePrivateKey is not defined in the included
		// version of wincapi.h, CRYPT_ACQUIRE_CACHE_FLAG will not be set

#if defined (CRYPT_ACQUIRE_CACHE_FLAG)
		
		// Code provided by Milan Tomic

		//Please note that this example below use CryptAcquireCertificatePrivateKey() function
		//which is not declared in wincrypt.h that ships with VC++ 6. If you would like to run
		//this example you'll need to replace your old wincrypt.h and crypt32.lib with new versions.
		//This example below is compatible with Windows 98/IE 5 and above OS/IE versions.

		else if (strcasecmp(argv[paramCount], "--wincer") == 0 || strcasecmp(argv[paramCount], "-wc") == 0) {
			WinCAPICryptoProvider * cp;
			PCCERT_CONTEXT          pSignerCert = NULL;
			DWORD                   dwKeySpec;
			HCERTSTORE				hStoreHandle;
			#define MY_ENCODING_TYPE  (PKCS_7_ASN_ENCODING | X509_ASN_ENCODING)

			// Obtain default PROV_DSS with default user key container
			if (!CryptAcquireContext(&win32DSSCSP,
				NULL,
				NULL,
				PROV_DSS,
				0)) {
					cerr << "Error acquiring DSS Crypto Service Provider" << endl;
					return 2;
			}//*/

			// Open 'Personal' certificate store 
			if (!(hStoreHandle = CertOpenStore(CERT_STORE_PROV_SYSTEM,
				0,
				NULL,
				CERT_SYSTEM_STORE_CURRENT_USER,
				L"MY"))) {
					cerr << "Error opening 'Personal' store." << endl;
					return 2;
			}

			// Find desired cerificate
			if (!(pSignerCert = CertFindCertificateInStore(hStoreHandle,
				MY_ENCODING_TYPE,
				0,
				CERT_FIND_SUBJECT_STR_A,
				argv[paramCount+1],
				NULL))) {
					cerr << "Can't find '" << argv[paramCount+1] << "' certificate in 'Personal' store." << endl;
					return 2;
			}

			// Now get certificate's private key
			if (!CryptAcquireCertificatePrivateKey(pSignerCert,
				0,
				NULL,
				&win32RSACSP,
				&dwKeySpec,
				NULL)) {
					cerr << "Can't acquire private key of '" << argv[paramCount+1] << "' certificate." << endl;
					exit(1);
			}

			cp = new WinCAPICryptoProvider();
			XSECPlatformUtils::SetCryptoProvider(cp);

			HCRYPTKEY k;
			BOOL fResult = CryptGetUserKey(
				win32RSACSP,
				dwKeySpec,
				&k);

			if (!fResult || k == 0) {
				cerr << "Error obtaining default user (AT_SIGNATURE or AT_KEYEXCHANGE) key from windows RSA provider.\n";
				exit(1);
			};

			winKeyRSA = new WinCAPICryptoKeyRSA(win32RSACSP, k);
			key = winKeyRSA;
			paramCount += 2;

			CertFreeCertificateContext(pSignerCert);
			CertCloseStore(hStoreHandle, 0);
		}

#endif /* CRYPT_ACQUIRE_CACHE_FLAG */

#endif /* XSEC_HAVE_WINCAPI */

		else {

			printUsage();
			exit(1);

		}

	}

	// Create and set up the parser

	XercesDOMParser * parser = new XercesDOMParser;
	
	parser->setDoNamespaces(true);
	parser->setCreateEntityReferenceNodes(true);

	// Now parse out file

	bool errorsOccured = false;
	xsecsize_t errorCount = 0;
    try
    {
    	parser->parse(argv[argc - 1]);
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
		exit (1);

	}

	/*

		Now that we have the parsed file, get the DOM document and start looking at it

	*/
	
	DOMNode *doc;		// The document that we parsed

	doc = parser->getDocument();
	DOMDocument *theDOM = parser->getDocument();

	// Find the signature node
	
	DOMNode *sigNode = findDSIGNode(doc, "Signature");

	// Create the signature checker

	if (sigNode == 0) {

		cerr << "Could not find <Signature> node in " << argv[argc-1] << endl;
		exit(1);
	}


	XSECProvider * prov = new XSECProvider;
	DSIGSignature * sig = prov->newSignatureFromDOM(theDOM, sigNode);

	// Use the internal URI resolver
#if defined(_WIN32)
	XSECURIResolverGenericWin32* theResolver = new XSECURIResolverGenericWin32();
#else
	XSECURIResolverGenericUnix* theResolver = new XSECURIResolverGenericUnix();
#endif 
     
	// Map out base path of the file
	char * filename=argv[argc-1];
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

	theResolver->setBaseURI(MAKE_UNICODE_STRING(baseURI));
	sig->setURIResolver(theResolver);

	try {
		sig->load();
		if (clearKeyInfo == true)
			sig->clearKeyInfo();
		if (key != NULL)
			sig->setSigningKey(key);
		sig->sign();

		// Add any KeyInfo elements

#if defined(XSEC_HAVE_WINCAPI)

		if (winDssKeyInfo == true && winKeyDSA != NULL) {
			char pBuf[1024];
			char qBuf[1024];
			char gBuf[1024];
			char yBuf[1024];

			unsigned int i;
			i = winKeyDSA->getPBase64BigNums((char *) pBuf, 1024);
			pBuf[i] = '\0';
			i = winKeyDSA->getQBase64BigNums((char *) qBuf, 1024);
			qBuf[i] = '\0';
			i = winKeyDSA->getGBase64BigNums((char *) gBuf, 1024);
			gBuf[i] = '\0';
			i = winKeyDSA->getYBase64BigNums((char *) yBuf, 1024);
			yBuf[i] = '\0';

			sig->clearKeyInfo();
			sig->appendDSAKeyValue(
				MAKE_UNICODE_STRING(pBuf),
				MAKE_UNICODE_STRING(qBuf),
				MAKE_UNICODE_STRING(gBuf),
				MAKE_UNICODE_STRING(yBuf));
		}

		if (winRsaKeyInfo == true && winKeyRSA != NULL) {
			char eBuf[1024];
			char mBuf[1024];

			unsigned int i;
			i = winKeyRSA->getExponentBase64BigNums((char *) eBuf, 1024);
			eBuf[i] = '\0';
			i = winKeyRSA->getModulusBase64BigNums((char *) mBuf, 1024);
			mBuf[i] = '\0';

			sig->clearKeyInfo();
			sig->appendRSAKeyValue(
				MAKE_UNICODE_STRING(mBuf),
				MAKE_UNICODE_STRING(eBuf));
		}

#endif
#if defined (XSEC_HAVE_OPENSSL)
		if (certCount > 0) {

			int i;
			// Have some certificates - see if there is already an X509 list
			DSIGKeyInfoList * kiList = sig->getKeyInfoList();
			int kiSize = (int) kiList->getSize();

			for (i = 0; i < kiSize; ++i) {

				if (kiList->item(i)->getKeyInfoType() == DSIGKeyInfo::KEYINFO_X509) {
					keyInfoX509 = (DSIGKeyInfoX509 *) kiList->item(i);
					break;
				}
			}

			if (keyInfoX509 == 0) {

				// Not found - need to create
				keyInfoX509 = sig->appendX509Data();

			}

			for (i = 0; i < certCount; ++i) {

				keyInfoX509->appendX509Certificate(certs[i]->getDEREncodingSB().sbStrToXMLCh());

			}

		} /* certCount > 0 */
#endif
		if (x509SubjectName != NULL) {

			int i;
			// Have some certificates - see if there is already an X509 list
			DSIGKeyInfoList * kiList = sig->getKeyInfoList();
			int kiSize = (int) kiList->getSize();

			for (i = 0; i < kiSize; ++i) {

				if (kiList->item(i)->getKeyInfoType() == DSIGKeyInfo::KEYINFO_X509) {
					keyInfoX509 = (DSIGKeyInfoX509 *) kiList->item(i);
					break;
				}
			}

			if (keyInfoX509 == 0) {

				// Not found - need to create
				keyInfoX509 = sig->appendX509Data();

			}

			keyInfoX509->setX509SubjectName(MAKE_UNICODE_STRING(x509SubjectName));

		} /* certCount > 0 */
	}

	catch (XSECException &e) {
		char * m = XMLString::transcode(e.getMsg());
		cerr << "An error occured during signing operation\n   Message: "
		<< m << endl;
		XSEC_RELEASE_XMLCH(m);
		errorsOccured = true;
		exit (1);
	}

	catch (XSECCryptoException &e) {
		cerr << "A cryptographic error occured during signature operation\n   Message: "
		<< e.getMsg() << endl;
		errorsOccured = true;
		exit(1);
	}

	catch (NetAccessorException) {
		cerr << "A network error occurred during signing operation\n" << endl;
		errorsOccured = true;
		exit(1);
	}

	// Print out the result

	DOMPrintFormatTarget* formatTarget = new DOMPrintFormatTarget();
	
    const XMLCh* encNameStr = XMLString::transcode("UTF-8");
    DOMNode *aNode = doc->getFirstChild();
    if (aNode->getNodeType() == DOMNode::ENTITY_NODE)
    {
        const XMLCh* aStr = 
#if defined XSEC_XERCES_DOMENTITYINPUTENCODING
            ((DOMEntity *)aNode)->getInputEncoding();
#else
            ((DOMEntity *)aNode)->getEncoding();
#endif
        if (!strEquals(aStr, ""))
        {
            encNameStr = aStr;
        }
    }
    xsecsize_t lent = XMLString::stringLen(encNameStr);
    gEncodingName = new XMLCh[lent + 1];
    XMLString::copyNString(gEncodingName, encNameStr, lent);
    gEncodingName[lent] = 0;

	
	
#if defined(XSEC_XERCES_FORMATTER_REQUIRES_VERSION)
	gFormatter = new XMLFormatter("UTF-8", 0, formatTarget,
                                          XMLFormatter::NoEscapes, gUnRepFlags);
#else
	gFormatter = new XMLFormatter("UTF-8", formatTarget,
                                          XMLFormatter::NoEscapes, gUnRepFlags);
#endif

	cout << doc;

	delete [] gEncodingName;
	XMLCh * toRelease = (XMLCh *) encNameStr;
	XSEC_RELEASE_XMLCH(toRelease);
	delete gFormatter;
	delete formatTarget;

#if defined (_WIN32) && defined (XSEC_HAVE_WINCAPI)
	if (win32DSSCSP != 0)
		CryptReleaseContext(win32DSSCSP,0);
	if (win32RSACSP != 0)
		CryptReleaseContext(win32RSACSP,0);
#endif

	prov->releaseSignature(sig);
	delete parser;
	delete prov;
    delete theResolver;

	XSECPlatformUtils::Terminate();
#ifndef XSEC_NO_XALAN
	XalanTransformer::terminate();
	XPathEvaluator::terminate();
#endif
	XMLPlatformUtils::Terminate();

	
	return 0;
}
