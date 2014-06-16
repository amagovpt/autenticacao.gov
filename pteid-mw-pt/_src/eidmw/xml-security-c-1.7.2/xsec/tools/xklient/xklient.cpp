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
 * xklient := Act as a client for an XKMS service
 *
 * $Id: xklient.cpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

// XSEC

#include <xsec/utils/XSECPlatformUtils.hpp>
#include <xsec/framework/XSECProvider.hpp>
#include <xsec/framework/XSECError.hpp>
#include <xsec/canon/XSECC14n20010315.hpp>
#include <xsec/dsig/DSIGSignature.hpp>
#include <xsec/dsig/DSIGKeyInfoX509.hpp>
#include <xsec/dsig/DSIGKeyInfoValue.hpp>
#include <xsec/dsig/DSIGKeyInfoName.hpp>
#include <xsec/framework/XSECException.hpp>
#include <xsec/enc/XSECCryptoException.hpp>
#include <xsec/enc/XSCrypt/XSCryptCryptoBase64.hpp>
#include <xsec/enc/XSECCryptoUtils.hpp>
#include <xsec/utils/XSECDOMUtils.hpp>
#include <xsec/enc/XSECKeyInfoResolverDefault.hpp>

#include <xsec/xkms/XKMSCompoundRequest.hpp>
#include <xsec/xkms/XKMSCompoundResult.hpp>
#include <xsec/xkms/XKMSPendingRequest.hpp>
#include <xsec/xkms/XKMSMessageAbstractType.hpp>
#include <xsec/xkms/XKMSLocateRequest.hpp>
#include <xsec/xkms/XKMSLocateResult.hpp>
#include <xsec/xkms/XKMSStatusRequest.hpp>
#include <xsec/xkms/XKMSStatusResult.hpp>
#include <xsec/xkms/XKMSResult.hpp>
#include <xsec/xkms/XKMSQueryKeyBinding.hpp>
#include <xsec/xkms/XKMSKeyBinding.hpp>
#include <xsec/xkms/XKMSUseKeyWith.hpp>
#include <xsec/xkms/XKMSConstants.hpp>
#include <xsec/xkms/XKMSValidateRequest.hpp>
#include <xsec/xkms/XKMSValidateResult.hpp>
#include <xsec/xkms/XKMSRegisterRequest.hpp>
#include <xsec/xkms/XKMSRegisterResult.hpp>
#include <xsec/xkms/XKMSAuthentication.hpp>
#include <xsec/xkms/XKMSPrototypeKeyBinding.hpp>
#include <xsec/xkms/XKMSRevokeRequest.hpp>
#include <xsec/xkms/XKMSRevokeResult.hpp>
#include <xsec/xkms/XKMSRecoverRequest.hpp>
#include <xsec/xkms/XKMSRecoverResult.hpp>
#include <xsec/xkms/XKMSReissueRequest.hpp>
#include <xsec/xkms/XKMSReissueResult.hpp>
#include <xsec/xkms/XKMSRevokeKeyBinding.hpp>
#include <xsec/xkms/XKMSRecoverKeyBinding.hpp>
#include <xsec/xkms/XKMSReissueKeyBinding.hpp>
#include <xsec/xkms/XKMSRSAKeyPair.hpp>

#include <xsec/utils/XSECSOAPRequestorSimple.hpp>

// General

#include <memory.h>
#include <string.h>
#include <iostream>
#include <stdlib.h>

#if !defined(_WIN32)
#	include <sys/time.h>
#	include <time.h>
#endif

#if defined (_DEBUG) && defined (_MSC_VER)
#include <crtdbg.h>
#endif


#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/framework/StdOutFormatTarget.hpp>
#include <xercesc/util/XMLException.hpp>
#include <xercesc/framework/LocalFileInputSource.hpp>
#include <xercesc/util/XMLUri.hpp>
#include <xercesc/util/Janitor.hpp>
#include <xercesc/sax/ErrorHandler.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <xercesc/sax/EntityResolver.hpp>
#include <xercesc/sax/InputSource.hpp>

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

#if defined (XSEC_HAVE_OPENSSL)
// OpenSSL

#	include <openssl/err.h>
#	include <xsec/enc/OpenSSL/OpenSSLCryptoKeyDSA.hpp>
#	include <xsec/enc/OpenSSL/OpenSSLCryptoBase64.hpp>
#	include <xsec/enc/OpenSSL/OpenSSLCryptoKeyRSA.hpp>
#	include <xsec/enc/OpenSSL/OpenSSLCryptoKeyHMAC.hpp>
#	include <xsec/enc/OpenSSL/OpenSSLCryptoX509.hpp>

#	include <openssl/bio.h>
#	include <openssl/dsa.h>
#	include <openssl/err.h>
#	include <openssl/evp.h>
#	include <openssl/pem.h>

#endif

// --------------------------------------------------------------------------------
//           Global definitions
// --------------------------------------------------------------------------------

bool g_txtOut = false;
char * g_authPassPhrase = NULL;
char * g_privateKeyFile = NULL;
char * g_privateKeyPassPhrase = NULL;

int doParsedMsgDump(DOMDocument * doc);

// --------------------------------------------------------------------------------
//           General functions
// --------------------------------------------------------------------------------


#ifdef XSEC_NO_XALAN

ostream& operator<< (ostream& target, const XMLCh * s)
{
    char *p = XMLString::transcode(s);
    target << p;
    XSEC_RELEASE_XMLCH(p);
    return target;
}

#endif

class X2C {

public:

	X2C(const XMLCh * in) {
		mp_cStr = XMLString::transcode(in);
	}
	~X2C() {
		XSEC_RELEASE_XMLCH(mp_cStr);
	}

	char * str(void) {
		return mp_cStr;
	}

private :

	char * mp_cStr;

};

ostream & operator<<(ostream& target, X2C &x) {
	target << x.str();
	return target;
}

inline
void levelSet(int level) {

	for (int i = 0; i < level; ++i)
		cout << "    ";

}

void outputDoc(DOMDocument * doc) {

	XMLCh tempStr[100];
	XMLString::transcode("Core", tempStr, 99);    
	DOMImplementation *impl = DOMImplementationRegistry::getDOMImplementation(tempStr);
	XMLFormatTarget *formatTarget = new StdOutFormatTarget();

	cerr << endl;

#if defined (XSEC_XERCES_DOMLSSERIALIZER)

    // DOM L3 version as per Xerces 3.0 API
    DOMLSSerializer   *theSerializer = ((DOMImplementationLS*)impl)->createLSSerializer();

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

	theSerializer->setEncoding(MAKE_UNICODE_STRING("UTF-8"));
	if (theSerializer->canSetFeature(XMLUni::fgDOMWRTFormatPrettyPrint, false))
		theSerializer->setFeature(XMLUni::fgDOMWRTFormatPrettyPrint, false);

	theSerializer->writeNode(formatTarget, *doc);
	
#endif

	cout << endl;

	cerr << endl;

	delete theSerializer;
	delete formatTarget;

}

XSECCryptoX509 * loadX509(const char * infile) {

	FILE * f = fopen(infile, "r");
	if (f == NULL)
		return NULL;

	safeBuffer sb;
	char buf[1024];

	int i = (int) fread(buf, 1, 1024, f);
	int j = 0;
	while (i != 0) {
		sb.sbMemcpyIn(j, buf, i);
		j += i;
		i = (int) fread(buf, 1, 1024, f);
	}

	sb[j] = '\0';

	XSECCryptoX509 * ret = 
		XSECPlatformUtils::g_cryptoProvider->X509();

	ret->loadX509PEM(sb.rawCharBuffer());

	return ret;

}

#if defined (XSEC_HAVE_OPENSSL)

XMLCh * BN2b64(BIGNUM * bn) {

	int bytes = BN_num_bytes(bn);
	unsigned char * binbuf = new unsigned char[bytes + 1];
	ArrayJanitor<unsigned char> j_binbuf(binbuf);

	bytes = BN_bn2bin(bn, binbuf);


	int bufLen = bytes * 4;
	int len = bufLen;
	unsigned char * buf;
	XSECnew(buf, unsigned char[bufLen]);
	ArrayJanitor<unsigned char> j_buf(buf);

	XSCryptCryptoBase64 *b64;
	XSECnew(b64, XSCryptCryptoBase64);
	Janitor<XSCryptCryptoBase64> j_b64(b64);

	b64->encodeInit();
	bufLen = b64->encode(binbuf, bytes, buf, bufLen);
	bufLen += b64->encodeFinish(&buf[bufLen], len-bufLen);
	buf[bufLen] = '\0';

	// Now translate to a bignum
	return XMLString::transcode((char *) buf);

}

#endif

DSIGKeyInfoX509 * findX509Data(DSIGKeyInfoList * lst) {

	if (lst == NULL)
		return NULL;

	int sz = (int) lst->getSize();
	for (int i = 0; i < sz; ++i) {

		DSIGKeyInfo *ki = lst->item(i);
		if (ki->getKeyInfoType() == DSIGKeyInfo::KEYINFO_X509)
			return (DSIGKeyInfoX509*) ki;

	}

	return NULL;

}

// --------------------------------------------------------------------------------
//           ErrorHandler
// --------------------------------------------------------------------------------

class xkmsErrorHandler : public ErrorHandler {

public:
	
	xkmsErrorHandler() {}
	~xkmsErrorHandler() {}

	// Interface
	virtual void 	warning (const SAXParseException &exc);
	virtual void 	error (const SAXParseException &exc);
	virtual void 	fatalError (const SAXParseException &exc);
	virtual void 	resetErrors ();
	
private:

	void outputError(const SAXParseException &exc);

};

void xkmsErrorHandler::outputError(const SAXParseException &exc) {

	char * systemId = XMLString::transcode(exc.getSystemId());
	char * msg = XMLString::transcode(exc.getMessage());
	if (exc.getLineNumber() > 0 || exc.getColumnNumber() > 0) {
		cerr << "File: " << systemId << " Line : " << exc.getLineNumber() << " Column : " 
			<< exc.getColumnNumber() << ". " << msg << endl;
	}
	else {
		cerr << msg << endl;
	}
	XSEC_RELEASE_XMLCH(msg);
	XSEC_RELEASE_XMLCH(systemId);

}

void xkmsErrorHandler::warning(const SAXParseException &exc) {

	cerr << "Parser warning - ";
	outputError(exc);

}

void xkmsErrorHandler::error (const SAXParseException &exc) {

	cerr << "Parser error - ";
	outputError(exc);

}

void xkmsErrorHandler::fatalError (const SAXParseException &exc) {

	cerr << "Parser fatal error - ";
	outputError(exc);

}

void xkmsErrorHandler::resetErrors () {

}


// --------------------------------------------------------------------------------
//           Create a LocateRequest
// --------------------------------------------------------------------------------

void printLocateRequestUsage(void) {

	cerr << "\nUsage LocateRequest [--help|-h] <service URI> [options]\n";
	cerr << "   --help/-h                : print this screen and exit\n\n";
	cerr << "   --add-cert/-a <filename> : add cert in filename as a KeyInfo\n";
	cerr << "   --add-name/-n <name>     : Add name as a KeyInfoName\n";
	cerr << "   --add-opaque/-o <data>   : Add an opaque data string\n";
	cerr << "   --add-usage-sig/-us      : Add Signature Key Usage\n";
	cerr << "   --add-usage-exc/-ux      : Add Excange Key Usage\n";
	cerr << "   --add-usage-enc/-ue      : Add Encryption Key Usage\n";
	cerr << "   --add-responselimit/-l <limit>\n";
	cerr << "                            : Set <limit> for ResponseLimit\n";
	cerr << "   --add-usekeywith/-u <Application URI> <Identifier>\n";
	cerr << "                            : Add a UseKeyWith element\n";
	cerr << "   --add-respondwith/-r <Identifier>\n";
	cerr << "                            : Add a RespondWith element\n";
	cerr << "   --add-responsemechanism/-m <Identifier>\n";
	cerr << "                            : Add a ResponseMechanism element\n";
	cerr << "   --sign-rsa/-sr <filename> <passphrase>\n";
	cerr << "           : Sign using the RSA key in file protected by passphrase\n";
	cerr << "   --sign-dsa/-sd <filename> <passphrase>\n";
	cerr << "           : Sign using the DSA key in file protected by passphrase\n\n";

}

XKMSMessageAbstractType * createLocateRequest(XSECProvider &prov, DOMDocument **doc, int argc, char ** argv, int &paramCount, XKMSCompoundRequest * cr = NULL) {

	if (paramCount >= argc || 
		(_stricmp(argv[paramCount], "--help") == 0) ||
		(_stricmp(argv[paramCount], "-h") == 0)) {

		printLocateRequestUsage();
		return NULL;
	}

	/* First create the basic request */
	XKMSMessageFactory * factory = 
		prov.getXKMSMessageFactory();
	XKMSLocateRequest * lr; 
	if (cr == NULL)
		lr = factory->createLocateRequest(MAKE_UNICODE_STRING(argv[paramCount++]), doc);
	else
		lr = cr->createLocateRequest(MAKE_UNICODE_STRING(argv[paramCount++]));

	while (paramCount < argc && _stricmp(argv[paramCount], "--") != 0) {

		if (_stricmp(argv[paramCount], "--add-cert") == 0 || _stricmp(argv[paramCount], "-a") == 0) {
			if (++paramCount >= argc) {
				printLocateRequestUsage();
				delete lr;
				return NULL;
			}
			XSECCryptoX509 * x = loadX509(argv[paramCount]);
			if (x == NULL) {
				delete lr;
				(*doc)->release();
				cerr << "Error opening Certificate file : " << 
					argv[paramCount] << endl;
				return NULL;
			}

			Janitor<XSECCryptoX509> j_x(x);

			XKMSQueryKeyBinding * qkb = lr->getQueryKeyBinding();
			if (qkb == NULL) {
				qkb = lr->addQueryKeyBinding();
			}
			// See if there is already an X.509 element
			DSIGKeyInfoX509 * kix;
			if ((kix = findX509Data(qkb->getKeyInfoList())) == NULL)
				kix = qkb->appendX509Data();
			safeBuffer sb = x->getDEREncodingSB();
			kix->appendX509Certificate(sb.sbStrToXMLCh());
			paramCount++;
		}

		else if (_stricmp(argv[paramCount], "--add-name") == 0 || _stricmp(argv[paramCount], "-n") == 0) {
			if (++paramCount >= argc) {
				printLocateRequestUsage();
				delete lr;
				return NULL;
			}
			XKMSQueryKeyBinding * qkb = lr->addQueryKeyBinding();
			qkb->appendKeyName(MAKE_UNICODE_STRING(argv[paramCount]));
			paramCount++;
		}
		else if (_stricmp(argv[paramCount], "--add-opaque") == 0 || _stricmp(argv[paramCount], "-o") == 0) {
			if (++paramCount >= argc) {
				printLocateRequestUsage();
				delete lr;
				return NULL;
			}
			lr->appendOpaqueClientDataItem(MAKE_UNICODE_STRING(argv[paramCount]));
			paramCount++;
		}
		else if (_stricmp(argv[paramCount], "--add-usage-sig") == 0 || _stricmp(argv[paramCount], "-us") == 0) {
			XKMSQueryKeyBinding * qkb = lr->getQueryKeyBinding();
			if (qkb == NULL)
				qkb = lr->addQueryKeyBinding();
			qkb->setSignatureKeyUsage();
			paramCount++;
		}
		else if (_stricmp(argv[paramCount], "--add-usage-exc") == 0 || _stricmp(argv[paramCount], "-ux") == 0) {
			XKMSQueryKeyBinding * qkb = lr->getQueryKeyBinding();
			if (qkb == NULL)
				qkb = lr->addQueryKeyBinding();
			qkb->setExchangeKeyUsage();
			paramCount++;
		}
		else if (_stricmp(argv[paramCount], "--add-usage-enc") == 0 || _stricmp(argv[paramCount], "-ue") == 0) {
			XKMSQueryKeyBinding * qkb = lr->getQueryKeyBinding();
			if (qkb == NULL)
				qkb = lr->addQueryKeyBinding();
			qkb->setEncryptionKeyUsage();
			paramCount++;
		}
		else if (_stricmp(argv[paramCount], "--add-responselimit") == 0 || _stricmp(argv[paramCount], "-l") == 0) {
			if (paramCount >= argc+1) {
				printLocateRequestUsage();
				delete lr;
				return NULL;
			}
			paramCount++;
			lr->setResponseLimit(atoi(argv[paramCount]));
			paramCount++;
		}
		else if (_stricmp(argv[paramCount], "--add-usekeywith") == 0 || _stricmp(argv[paramCount], "-u") == 0) {
			if (++paramCount >= argc + 1) {
				printLocateRequestUsage();
				delete lr;
				return NULL;
			}
			XKMSQueryKeyBinding *qkb = lr->getQueryKeyBinding();
			if (qkb == NULL)
				qkb = lr->addQueryKeyBinding();

			qkb->appendUseKeyWithItem(MAKE_UNICODE_STRING(argv[paramCount]), MAKE_UNICODE_STRING(argv[paramCount + 1]));
			paramCount += 2;
		}
		else if (_stricmp(argv[paramCount], "--add-respondwith") == 0 || _stricmp(argv[paramCount], "-r") == 0) {
			if (++paramCount >= argc) {
				printLocateRequestUsage();
				delete lr;
				return NULL;
			}
			lr->appendRespondWithItem(MAKE_UNICODE_STRING(argv[paramCount]));
			paramCount++;
		}
		else if (_stricmp(argv[paramCount], "--add-responsemechanism") == 0 || _stricmp(argv[paramCount], "-m") == 0) {
			if (++paramCount >= argc) {
				printLocateRequestUsage();
				delete lr;
				return NULL;
			}
			lr->appendResponseMechanismItem(MAKE_UNICODE_STRING(argv[paramCount]));
			paramCount++;
		}
#if defined (XSEC_HAVE_OPENSSL)
		else if (_stricmp(argv[paramCount], "--sign-dsa") == 0 || _stricmp(argv[paramCount], "-sd") == 0 ||
				_stricmp(argv[paramCount], "--sign-rsa") == 0 || _stricmp(argv[paramCount], "-sr") == 0) {
			if (paramCount >= argc + 2) {
				printLocateRequestUsage();
				delete lr;
				return NULL;
			}

			// DSA or RSA OpenSSL Key
			// For now just read a particular file

			BIO * bioKey;
			if ((bioKey = BIO_new(BIO_s_file())) == NULL) {

				cerr << "Error opening private key file\n\n";
				return NULL;

			}

			if (BIO_read_filename(bioKey, argv[paramCount+1]) <= 0) {

				cerr << "Error opening private key file : " << argv[paramCount+1] << endl;
				return NULL;

			}

			EVP_PKEY * pkey;
			pkey = PEM_read_bio_PrivateKey(bioKey,NULL,NULL,argv[paramCount + 2]);

			if (pkey == NULL) {

				BIO * bio_err;
	
				if ((bio_err=BIO_new(BIO_s_file())) != NULL)
					BIO_set_fp(bio_err,stderr,BIO_NOCLOSE|BIO_FP_TEXT);
				cerr << "Error loading private key\n\n";
				ERR_print_errors(bio_err);
				return NULL;

			}
			XSECCryptoKey *key;
			DSIGSignature * sig;
			if (_stricmp(argv[paramCount], "--sign-dsa") == 0 || _stricmp(argv[paramCount], "-sd") == 0) {

				// Check type is correct

				if (pkey->type != EVP_PKEY_DSA) {
					cerr << "DSA Key requested, but OpenSSL loaded something else\n";
					return NULL;
				}

				sig = lr->addSignature(CANON_C14N_NOC, SIGNATURE_DSA, HASH_SHA1);
				// Create the XSEC OpenSSL interface
				key = new OpenSSLCryptoKeyDSA(pkey);

				XMLCh * P = BN2b64(pkey->pkey.dsa->p);
				XMLCh * Q = BN2b64(pkey->pkey.dsa->q);
				XMLCh * G = BN2b64(pkey->pkey.dsa->g);
				XMLCh * Y = BN2b64(pkey->pkey.dsa->pub_key);

				sig->appendDSAKeyValue(P,Q,G,Y);

				XSEC_RELEASE_XMLCH(P);
				XSEC_RELEASE_XMLCH(Q);
				XSEC_RELEASE_XMLCH(G);
				XSEC_RELEASE_XMLCH(Y);
			}
			else {
				if (pkey->type != EVP_PKEY_RSA) {
					cerr << "RSA Key requested, but OpenSSL loaded something else\n";
					exit (1);
				}
				sig = lr->addSignature(CANON_C14N_NOC, SIGNATURE_RSA, HASH_SHA1);
				key = new OpenSSLCryptoKeyRSA(pkey);

				XMLCh * mod = BN2b64(pkey->pkey.rsa->n);
				XMLCh * exp = BN2b64(pkey->pkey.rsa->e);
				sig->appendRSAKeyValue(mod, exp);
				XSEC_RELEASE_XMLCH(mod);
				XSEC_RELEASE_XMLCH(exp);

			}

			sig->setSigningKey(key);
			sig->sign();

			EVP_PKEY_free(pkey);
			BIO_free(bioKey);

			paramCount += 3;

			
		} /* argv[1] = "dsa/rsa" */

#endif
		else {
			printLocateRequestUsage();
			delete lr;
			(*doc)->release();
			return NULL;
		}
	}

	return lr;
}

// --------------------------------------------------------------------------------
//           Create a ValidateRequest
// --------------------------------------------------------------------------------

void printValidateRequestUsage(void) {

	cerr << "\nUsage ValidateRequest [--help|-h] <service URI> [options]\n";
	cerr << "   --help/-h                : print this screen and exit\n\n";
	cerr << "   --add-cert/-a <filename> : add cert in filename as a KeyInfo\n";
	cerr << "   --add-name/-n <name>     : Add name as a KeyInfoName\n";
	cerr << "   --add-opaque/-o <data>   : Add an opaque data string\n";
	cerr << "   --add-usage-sig/-us      : Add Signature Key Usage\n";
	cerr << "   --add-usage-exc/-ux      : Add Excange Key Usage\n";
	cerr << "   --add-usage-enc/-ue      : Add Encryption Key Usage\n";
	cerr << "   --add-responselimit/-l <limit>\n";
	cerr << "                            : Set <limit> for ResponseLimit\n";
	cerr << "   --add-usekeywith/-u <Application URI> <Identifier>\n";
	cerr << "                            : Add a UseKeyWith element\n";
	cerr << "   --add-respondwith/-r <Identifier>\n";
	cerr << "                            : Add a RespondWith element\n";
	cerr << "   --add-responsemechanism/-m <Identifier>\n";
	cerr << "                            : Add a ResponseMechanism element\n";
	cerr << "   --sign-rsa/-sr <filename> <passphrase>\n";
	cerr << "           : Sign using the RSA key in file protected by passphrase\n";
	cerr << "   --sign-dsa/-sd <filename> <passphrase>\n";
	cerr << "           : Sign using the DSA key in file protected by passphrase\n";
	cerr << "   --sign-cert/-sc <filename>\n";
	cerr << "           : Add the indicated certificate to the signature KeyInfo\n\n";

}

XKMSMessageAbstractType * createValidateRequest(XSECProvider &prov, DOMDocument **doc, int argc, char ** argv, int &paramCount, XKMSCompoundRequest * cr = NULL) {

	if (paramCount >= argc || 
		(_stricmp(argv[paramCount], "--help") == 0) ||
		(_stricmp(argv[paramCount], "-h") == 0)) {

		printValidateRequestUsage();
		return NULL;
	}

	/* First create the basic request */
	XKMSMessageFactory * factory = 
		prov.getXKMSMessageFactory();
	XKMSValidateRequest * vr;

	if (cr == NULL)
		vr = factory->createValidateRequest(MAKE_UNICODE_STRING(argv[paramCount++]), doc);
	else
		vr = cr->createValidateRequest(MAKE_UNICODE_STRING(argv[paramCount++]));

	while (paramCount < argc && _stricmp(argv[paramCount], "--") != 0) {

		if (_stricmp(argv[paramCount], "--add-cert") == 0 || _stricmp(argv[paramCount], "-a") == 0) {
			if (++paramCount >= argc) {
				printValidateRequestUsage();
				delete vr;
				return NULL;
			}
			XSECCryptoX509 * x = loadX509(argv[paramCount]);
			if (x == NULL) {
				delete vr;
				(*doc)->release();
				cerr << "Error opening Certificate file : " << 
					argv[paramCount] << endl;
				return NULL;
			}

			Janitor<XSECCryptoX509> j_x(x);

			XKMSQueryKeyBinding * qkb = vr->getQueryKeyBinding();
			if (qkb == NULL) {
				qkb = vr->addQueryKeyBinding();
			}
			// See if there is already an X.509 element
			DSIGKeyInfoX509 * kix;
			if ((kix = findX509Data(qkb->getKeyInfoList())) == NULL)
				kix = qkb->appendX509Data();
			safeBuffer sb = x->getDEREncodingSB();
			kix->appendX509Certificate(sb.sbStrToXMLCh());
			paramCount++;

		}

		else if (_stricmp(argv[paramCount], "--add-name") == 0 || _stricmp(argv[paramCount], "-n") == 0) {
			if (++paramCount >= argc) {
				printValidateRequestUsage();
				delete vr;
				return NULL;
			}
			XKMSQueryKeyBinding * qkb = vr->addQueryKeyBinding();
			qkb->appendKeyName(MAKE_UNICODE_STRING(argv[paramCount]));
			paramCount++;
		}
		else if (_stricmp(argv[paramCount], "--add-opaque") == 0 || _stricmp(argv[paramCount], "-o") == 0) {
			if (++paramCount >= argc) {
				printValidateRequestUsage();
				delete vr;
				return NULL;
			}
			vr->appendOpaqueClientDataItem(MAKE_UNICODE_STRING(argv[paramCount]));
			paramCount++;
		}
		else if (_stricmp(argv[paramCount], "--add-respondwith") == 0 || _stricmp(argv[paramCount], "-r") == 0) {
			if (++paramCount >= argc) {
				printValidateRequestUsage();
				delete vr;
				return NULL;
			}
			vr->appendRespondWithItem(MAKE_UNICODE_STRING(argv[paramCount]));
			paramCount++;
		}
		else if (_stricmp(argv[paramCount], "--add-responsemechanism") == 0 || _stricmp(argv[paramCount], "-m") == 0) {
			if (++paramCount >= argc) {
				printLocateRequestUsage();
				delete vr;
				return NULL;
			}
			vr->appendResponseMechanismItem(MAKE_UNICODE_STRING(argv[paramCount]));
			paramCount++;
		}
		else if (_stricmp(argv[paramCount], "--add-responselimit") == 0 || _stricmp(argv[paramCount], "-l") == 0) {
			if (paramCount >= argc+1) {
				printValidateRequestUsage();
				delete vr;
				return NULL;
			}
			paramCount++;
			vr->setResponseLimit(atoi(argv[paramCount]));
			paramCount++;
		}
		else if (_stricmp(argv[paramCount], "--add-usage-sig") == 0 || _stricmp(argv[paramCount], "-us") == 0) {
			XKMSQueryKeyBinding * qkb = vr->getQueryKeyBinding();
			if (qkb == NULL)
				qkb = vr->addQueryKeyBinding();
			qkb->setSignatureKeyUsage();
			paramCount++;
		}
		else if (_stricmp(argv[paramCount], "--add-usage-exc") == 0 || _stricmp(argv[paramCount], "-ux") == 0) {
			XKMSQueryKeyBinding * qkb = vr->getQueryKeyBinding();
			if (qkb == NULL)
				qkb = vr->addQueryKeyBinding();
			qkb->setExchangeKeyUsage();
			paramCount++;
		}
		else if (_stricmp(argv[paramCount], "--add-usage-enc") == 0 || _stricmp(argv[paramCount], "-ue") == 0) {
			XKMSQueryKeyBinding * qkb = vr->getQueryKeyBinding();
			if (qkb == NULL)
				qkb = vr->addQueryKeyBinding();
			qkb->setEncryptionKeyUsage();
			paramCount++;
		}
		else if (_stricmp(argv[paramCount], "--add-usekeywith") == 0 || _stricmp(argv[paramCount], "-u") == 0) {
			if (++paramCount >= argc + 1) {
				printValidateRequestUsage();
				delete vr;
				return NULL;
			}
			XKMSQueryKeyBinding *qkb = vr->getQueryKeyBinding();
			if (qkb == NULL)
				qkb = vr->addQueryKeyBinding();

			qkb->appendUseKeyWithItem(MAKE_UNICODE_STRING(argv[paramCount]), MAKE_UNICODE_STRING(argv[paramCount + 1]));
			paramCount += 2;
		}
#if defined (XSEC_HAVE_OPENSSL)
		else if (_stricmp(argv[paramCount], "--sign-dsa") == 0 || _stricmp(argv[paramCount], "-sd") == 0 ||
				_stricmp(argv[paramCount], "--sign-rsa") == 0 || _stricmp(argv[paramCount], "-sr") == 0) {
			if (paramCount >= argc + 2) {
				printValidateRequestUsage();
				delete vr;
				return NULL;
			}

			// DSA or RSA OpenSSL Key
			// For now just read a particular file

			BIO * bioKey;
			if ((bioKey = BIO_new(BIO_s_file())) == NULL) {

				cerr << "Error opening private key file\n\n";
				return NULL;

			}

			if (BIO_read_filename(bioKey, argv[paramCount+1]) <= 0) {

				cerr << "Error opening private key file : " << argv[paramCount+1] << endl;
				return NULL;

			}

			EVP_PKEY * pkey;
			pkey = PEM_read_bio_PrivateKey(bioKey,NULL,NULL,argv[paramCount + 2]);

			if (pkey == NULL) {

				BIO * bio_err;
	
				if ((bio_err=BIO_new(BIO_s_file())) != NULL)
					BIO_set_fp(bio_err,stderr,BIO_NOCLOSE|BIO_FP_TEXT);
				cerr << "Error loading private key\n\n";
				ERR_print_errors(bio_err);
				return NULL;

			}
			XSECCryptoKey *key;
			DSIGSignature * sig;
			if (_stricmp(argv[paramCount], "--sign-dsa") == 0 || _stricmp(argv[paramCount], "-sd") == 0) {

				// Check type is correct

				if (pkey->type != EVP_PKEY_DSA) {
					cerr << "DSA Key requested, but OpenSSL loaded something else\n";
					return NULL;
				}

				sig = vr->addSignature(CANON_C14N_NOC, SIGNATURE_DSA, HASH_SHA1);
				// Create the XSEC OpenSSL interface
				key = new OpenSSLCryptoKeyDSA(pkey);

				XMLCh * P = BN2b64(pkey->pkey.dsa->p);
				XMLCh * Q = BN2b64(pkey->pkey.dsa->q);
				XMLCh * G = BN2b64(pkey->pkey.dsa->g);
				XMLCh * Y = BN2b64(pkey->pkey.dsa->pub_key);

				sig->appendDSAKeyValue(P,Q,G,Y);

				XSEC_RELEASE_XMLCH(P);
				XSEC_RELEASE_XMLCH(Q);
				XSEC_RELEASE_XMLCH(G);
				XSEC_RELEASE_XMLCH(Y);
			}
			else {
				if (pkey->type != EVP_PKEY_RSA) {
					cerr << "RSA Key requested, but OpenSSL loaded something else\n";
					exit (1);
				}
				sig = vr->addSignature(CANON_C14N_NOC, SIGNATURE_RSA, HASH_SHA1);
				key = new OpenSSLCryptoKeyRSA(pkey);

				XMLCh * mod = BN2b64(pkey->pkey.rsa->n);
				XMLCh * exp = BN2b64(pkey->pkey.rsa->e);
				sig->appendRSAKeyValue(mod, exp);
				XSEC_RELEASE_XMLCH(mod);
				XSEC_RELEASE_XMLCH(exp);

			}

			sig->setSigningKey(key);
			sig->sign();

			EVP_PKEY_free(pkey);
			BIO_free(bioKey);

			paramCount += 3;

			
		} /* argv[1] = "dsa/rsa" */
		else if (_stricmp(argv[paramCount], "--sign-cert") == 0 || _stricmp(argv[paramCount], "-sc") == 0) {
			if (++paramCount >= argc) {
				printValidateRequestUsage();
				delete vr;
				return NULL;
			}
			XSECCryptoX509 * x = loadX509(argv[paramCount]);
			if (x == NULL) {
				delete vr;
				(*doc)->release();
				cerr << "Error opening Certificate file : " << 
					argv[paramCount] << endl;
				return NULL;
			}

			Janitor<XSECCryptoX509> j_x(x);

			DSIGSignature * sig = vr->getSignature();
			if (sig == NULL) {
				cerr << "Can only add Certificates to signature after signing\n";
				return NULL;
			}

			// See if there is already an X.509 element
			DSIGKeyInfoX509 * kix;
			if ((kix = findX509Data(sig->getKeyInfoList())) == NULL)
				kix = sig->appendX509Data();
			safeBuffer sb = x->getDEREncodingSB();
			kix->appendX509Certificate(sb.sbStrToXMLCh());
			paramCount++;
		}

#endif
		else {
			printValidateRequestUsage();
			delete vr;
			(*doc)->release();
			return NULL;
		}
	}

	return vr;
}

// --------------------------------------------------------------------------------
//           Create a RegisterRequest
// --------------------------------------------------------------------------------

void printRegisterRequestUsage(void) {

	cerr << "\nUsage RegisterRequest [--help|-h] <service URI> [options]\n";
	cerr << "   --help/-h                : print this screen and exit\n\n";
	cerr << "   --add-name/-n <name>     : Add name as a KeyInfoName\n";
	cerr << "   --add-opaque/-o <data>   : Add an opaque data string\n";
	cerr << "   --add-usage-sig/-us      : Add Signature Key Usage\n";
	cerr << "   --add-usage-exc/-ux      : Add Exchange Key Usage\n";
	cerr << "   --add-usage-enc/-ue      : Add Encryption Key Usage\n";
	cerr << "   --add-usekeywith/-u <Application URI> <Identifier>\n";
	cerr << "                            : Add a UseKeyWith element\n";
	cerr << "   --add-respondwith/-r <Identifier>\n";
	cerr << "                            : Add a RespondWith element\n";
	cerr << "   --add-responsemechanism/-m <Identifier>\n";
	cerr << "                            : Add a ResponseMechanism element\n";
	cerr << "   --sign-dsa/-sd <filename> <passphrase>\n";
	cerr << "           : Sign using the DSA key in file protected by passphrase\n";
	cerr << "   --add-value-dsa/-vd <filename> <passphrase> (and do proof-of-possession sig)\n";
	cerr << "           : Add the DSA key as a keyvalue\n";
	cerr << "   --add-value-rsa/-vr <filename> <passphrase> (and do proof-of-possession sig)\n";
	cerr << "           : Add the RSA key as a keyvalue\n";
	cerr << "   --revocation/-v <phrase> : Set <phrase> as revocation code\n";
	cerr << "   --kek/-k <phrase>        : Key phrase to use for PrivateKey decryption\n";
#if defined (XSEC_HAVE_OPENSSL)
	cerr << "   --output-private-key/-p <file> <pass phrase>\n";
	cerr << "                            : Write PEM encoded private key to file\n";
#endif
	cerr << "   --authenticate/-a <phrase>\n";
	cerr << "           : Use <phrase> as the authentication key for the request\n";
	cerr << "             NOTE - This must come *after* adding of KeyInfo elements\n\n";

}

XKMSMessageAbstractType * createRegisterRequest(XSECProvider &prov, DOMDocument **doc, int argc, char ** argv, int &paramCount, XKMSCompoundRequest * cr = NULL) {

	XSECCryptoKey *proofOfPossessionKey = NULL;
	signatureMethod proofOfPossessionSm = SIGNATURE_DSA;

	if (paramCount >= argc || 
		(_stricmp(argv[paramCount], "--help") == 0) ||
		(_stricmp(argv[paramCount], "-h") == 0)) {

		printRegisterRequestUsage();
		return NULL;
	}

	/* First create the basic request */
	XKMSMessageFactory * factory = 
		prov.getXKMSMessageFactory();
	XKMSRegisterRequest * rr;

	if (cr == NULL)
		rr = factory->createRegisterRequest(MAKE_UNICODE_STRING(argv[paramCount++]), doc);
	else
		rr = cr->createRegisterRequest(MAKE_UNICODE_STRING(argv[paramCount++]));

	while (paramCount < argc && _stricmp(argv[paramCount], "--") != 0) {

		if (_stricmp(argv[paramCount], "--add-name") == 0 || _stricmp(argv[paramCount], "-n") == 0) {
			if (++paramCount >= argc) {
				printRegisterRequestUsage();
				delete rr;
				return NULL;
			}

			XKMSPrototypeKeyBinding * pkb = rr->getPrototypeKeyBinding();
			if (pkb == NULL)
				pkb = rr->addPrototypeKeyBinding();
			pkb->appendKeyName(MAKE_UNICODE_STRING(argv[paramCount]));
			paramCount++;
		}
		else if (_stricmp(argv[paramCount], "--add-opaque") == 0 || _stricmp(argv[paramCount], "-o") == 0) {
			if (++paramCount >= argc) {
				printRegisterRequestUsage();
				delete rr;
				return NULL;
			}
			rr->appendOpaqueClientDataItem(MAKE_UNICODE_STRING(argv[paramCount]));
			paramCount++;
		}
		else if (_stricmp(argv[paramCount], "--kek") == 0 || _stricmp(argv[paramCount], "-k") == 0) {
			if (++paramCount >= argc) {
				printRegisterRequestUsage();
				delete rr;
				return NULL;
			}
			g_authPassPhrase = argv[paramCount++];
		}
		else if (_stricmp(argv[paramCount], "--add-respondwith") == 0 || _stricmp(argv[paramCount], "-r") == 0) {
			if (++paramCount >= argc) {
				printRegisterRequestUsage();
				delete rr;
				return NULL;
			}
			rr->appendRespondWithItem(MAKE_UNICODE_STRING(argv[paramCount]));
			paramCount++;
		}
		else if (_stricmp(argv[paramCount], "--add-responsemechanism") == 0 || _stricmp(argv[paramCount], "-m") == 0) {
			if (++paramCount >= argc) {
				printRegisterRequestUsage();
				delete rr;
				return NULL;
			}
			rr->appendResponseMechanismItem(MAKE_UNICODE_STRING(argv[paramCount]));
			paramCount++;
		}
		else if (_stricmp(argv[paramCount], "--add-usage-sig") == 0 || _stricmp(argv[paramCount], "-us") == 0) {
			XKMSPrototypeKeyBinding * pkb = rr->getPrototypeKeyBinding();
			if (pkb == NULL)
				pkb = rr->addPrototypeKeyBinding();
			pkb->setSignatureKeyUsage();
			paramCount++;
		}
		else if (_stricmp(argv[paramCount], "--add-usage-exc") == 0 || _stricmp(argv[paramCount], "-ux") == 0) {
			XKMSPrototypeKeyBinding * pkb = rr->getPrototypeKeyBinding();
			if (pkb == NULL)
				pkb = rr->addPrototypeKeyBinding();
			pkb->setExchangeKeyUsage();
			paramCount++;
		}
		else if (_stricmp(argv[paramCount], "--add-usage-enc") == 0 || _stricmp(argv[paramCount], "-ue") == 0) {
			XKMSPrototypeKeyBinding * pkb = rr->getPrototypeKeyBinding();
			if (pkb == NULL)
				pkb = rr->addPrototypeKeyBinding();
			pkb->setEncryptionKeyUsage();
			paramCount++;
		}
		else if (_stricmp(argv[paramCount], "--add-usekeywith") == 0 || _stricmp(argv[paramCount], "-u") == 0) {
			if (++paramCount >= argc + 1) {
				printRegisterRequestUsage();
				delete rr;
				return NULL;
			}
			XKMSPrototypeKeyBinding *pkb = rr->getPrototypeKeyBinding();
			if (pkb == NULL)
				pkb = rr->addPrototypeKeyBinding();

			pkb->appendUseKeyWithItem(MAKE_UNICODE_STRING(argv[paramCount]), MAKE_UNICODE_STRING(argv[paramCount + 1]));
			paramCount += 2;
		}
#if defined (XSEC_HAVE_OPENSSL)
		else if (_stricmp(argv[paramCount], "--output-private-key") == 0 || _stricmp(argv[paramCount], "-p") == 0) {
			if (paramCount >= argc + 2) {
				printRegisterRequestUsage();
				delete rr;
				return NULL;
			}
			++paramCount;
			g_privateKeyFile = argv[paramCount++];
			g_privateKeyPassPhrase = argv[paramCount++];
		}
#endif
		else if (_stricmp(argv[paramCount], "--revocation") == 0 || _stricmp(argv[paramCount], "-v") == 0) {
			if (++paramCount >= argc) {
				printRegisterRequestUsage();
				delete rr;
				return NULL;
			}
			XKMSPrototypeKeyBinding *pkb = rr->getPrototypeKeyBinding();
			if (pkb == NULL)
				pkb = rr->addPrototypeKeyBinding();

			// Create the RevocationCodeIdentifier
			unsigned char rciBuf[XSEC_MAX_HASH_SIZE];
			int len = CalculateXKMSRevocationCodeIdentifierEncoding2((unsigned char *) argv[paramCount], (int) strlen(argv[paramCount]), rciBuf, XSEC_MAX_HASH_SIZE);

			if (len <= 0) {
				cerr << "Error creating revocation code!\n";
				delete rr;
				return NULL;
			}

			// Convert to base64
			XMLCh * str = EncodeToBase64XMLCh(rciBuf, len);
			pkb->setRevocationCodeIdentifier(str);
			XSEC_RELEASE_XMLCH(str);

			paramCount++;;
		}		
		else if (_stricmp(argv[paramCount], "--authenticate") == 0 || _stricmp(argv[paramCount], "-a") == 0) {
			if (++paramCount >= argc + 1) {
				printRegisterRequestUsage();
				delete rr;
				return NULL;
			}

			// Create the signature

			unsigned char keyBuf[XSEC_MAX_HASH_SIZE];
			int len = CalculateXKMSAuthenticationKey((unsigned char *) argv[paramCount], (int) strlen(argv[paramCount]), keyBuf, XSEC_MAX_HASH_SIZE);
			if (len <= 0) {
				cout << "Error creating key from pass phrase" << endl;
				delete rr;
				return NULL;
			}

			XSECCryptoKeyHMAC * k = XSECPlatformUtils::g_cryptoProvider->keyHMAC();
			k->setKey(keyBuf, len);

			// Set key and validate
			XKMSAuthentication * a = rr->addAuthentication();
			DSIGSignature * sig = a->addKeyBindingAuthenticationSignature();

			sig->setSigningKey(k);
			sig->sign();

			paramCount++;

		}
#if defined (XSEC_HAVE_OPENSSL)
		else if (_stricmp(argv[paramCount], "--sign-dsa") == 0 || _stricmp(argv[paramCount], "-sd") == 0 ||
				_stricmp(argv[paramCount], "--sign-rsa") == 0 || _stricmp(argv[paramCount], "-sr") == 0) {
			if (paramCount >= argc + 2) {
				printRegisterRequestUsage();
				delete rr;
				return NULL;
			}

			// DSA or RSA OpenSSL Key
			// For now just read a particular file

			BIO * bioKey;
			if ((bioKey = BIO_new(BIO_s_file())) == NULL) {

				cerr << "Error opening private key file\n\n";
				return NULL;

			}

			if (BIO_read_filename(bioKey, argv[paramCount+1]) <= 0) {

				cerr << "Error opening private key file : " << argv[paramCount+1] << endl;
				return NULL;

			}

			EVP_PKEY * pkey;
			pkey = PEM_read_bio_PrivateKey(bioKey,NULL,NULL,argv[paramCount + 2]);

			if (pkey == NULL) {

				BIO * bio_err;
	
				if ((bio_err=BIO_new(BIO_s_file())) != NULL)
					BIO_set_fp(bio_err,stderr,BIO_NOCLOSE|BIO_FP_TEXT);
				cerr << "Error loading private key\n\n";
				ERR_print_errors(bio_err);
				return NULL;

			}
			XSECCryptoKey *key;
			DSIGSignature * sig;
			if (_stricmp(argv[paramCount], "--sign-dsa") == 0 || _stricmp(argv[paramCount], "-sd") == 0) {

				// Check type is correct

				if (pkey->type != EVP_PKEY_DSA) {
					cerr << "DSA Key requested, but OpenSSL loaded something else\n";
					return NULL;
				}

				sig = rr->addSignature(CANON_C14N_NOC, SIGNATURE_DSA, HASH_SHA1);
				// Create the XSEC OpenSSL interface
				key = new OpenSSLCryptoKeyDSA(pkey);

				XMLCh * P = BN2b64(pkey->pkey.dsa->p);
				XMLCh * Q = BN2b64(pkey->pkey.dsa->q);
				XMLCh * G = BN2b64(pkey->pkey.dsa->g);
				XMLCh * Y = BN2b64(pkey->pkey.dsa->pub_key);

				sig->appendDSAKeyValue(P,Q,G,Y);

				XSEC_RELEASE_XMLCH(P);
				XSEC_RELEASE_XMLCH(Q);
				XSEC_RELEASE_XMLCH(G);
				XSEC_RELEASE_XMLCH(Y);
			}
			else {
				if (pkey->type != EVP_PKEY_RSA) {
					cerr << "RSA Key requested, but OpenSSL loaded something else\n";
					exit (1);
				}
				sig = rr->addSignature(CANON_C14N_NOC, SIGNATURE_RSA, HASH_SHA1);
				key = new OpenSSLCryptoKeyRSA(pkey);

				XMLCh * mod = BN2b64(pkey->pkey.rsa->n);
				XMLCh * exp = BN2b64(pkey->pkey.rsa->e);
				sig->appendRSAKeyValue(mod, exp);
				XSEC_RELEASE_XMLCH(mod);
				XSEC_RELEASE_XMLCH(exp);

			}

			sig->setSigningKey(key);
			sig->sign();

			EVP_PKEY_free(pkey);
			BIO_free(bioKey);

			paramCount += 3;

			
		} /* argv[1] = "sign dsa/rsa" */
		else if (_stricmp(argv[paramCount], "--add-value-dsa") == 0 || _stricmp(argv[paramCount], "-vd") == 0 ||
				_stricmp(argv[paramCount], "--add-value-rsa") == 0 || _stricmp(argv[paramCount], "-vr") == 0) {
			if (paramCount >= argc + 2) {
				printRegisterRequestUsage();
				delete rr;
				return NULL;
			}

			// DSA or RSA OpenSSL Key
			// For now just read a particular file

			BIO * bioKey;
			if ((bioKey = BIO_new(BIO_s_file())) == NULL) {

				cerr << "Error opening private key file\n\n";
				return NULL;

			}

			if (BIO_read_filename(bioKey, argv[paramCount+1]) <= 0) {

				cerr << "Error opening private key file : " << argv[paramCount+1] << endl;
				return NULL;

			}

			EVP_PKEY * pkey;
			pkey = PEM_read_bio_PrivateKey(bioKey,NULL,NULL,argv[paramCount + 2]);

			if (pkey == NULL) {

				BIO * bio_err;
	
				if ((bio_err=BIO_new(BIO_s_file())) != NULL)
					BIO_set_fp(bio_err,stderr,BIO_NOCLOSE|BIO_FP_TEXT);
				cerr << "Error loading private key\n\n";
				ERR_print_errors(bio_err);
				return NULL;

			}

			XKMSPrototypeKeyBinding * pkb = rr->getPrototypeKeyBinding();
			if (pkb == NULL)
				pkb = rr->addPrototypeKeyBinding();


			if (_stricmp(argv[paramCount], "--add-value-dsa") == 0 || _stricmp(argv[paramCount], "-vd") == 0) {

				// Check type is correct

				if (pkey->type != EVP_PKEY_DSA) {
					cerr << "DSA Key requested, but OpenSSL loaded something else\n";
					return NULL;
				}

				proofOfPossessionKey = new OpenSSLCryptoKeyDSA(pkey);
				proofOfPossessionSm = SIGNATURE_DSA;

				XMLCh * P = BN2b64(pkey->pkey.dsa->p);
				XMLCh * Q = BN2b64(pkey->pkey.dsa->q);
				XMLCh * G = BN2b64(pkey->pkey.dsa->g);
				XMLCh * Y = BN2b64(pkey->pkey.dsa->pub_key);

				pkb->appendDSAKeyValue(P,Q,G,Y);

				XSEC_RELEASE_XMLCH(P);
				XSEC_RELEASE_XMLCH(Q);
				XSEC_RELEASE_XMLCH(G);
				XSEC_RELEASE_XMLCH(Y);
			}
			else {
				if (pkey->type != EVP_PKEY_RSA) {
					cerr << "RSA Key requested, but OpenSSL loaded something else\n";
					exit (1);
				}

				proofOfPossessionKey = new OpenSSLCryptoKeyRSA(pkey);
				proofOfPossessionSm = SIGNATURE_RSA;

				XMLCh * mod = BN2b64(pkey->pkey.rsa->n);
				XMLCh * exp = BN2b64(pkey->pkey.rsa->e);
				pkb->appendRSAKeyValue(mod, exp);
				XSEC_RELEASE_XMLCH(mod);
				XSEC_RELEASE_XMLCH(exp);

			}

			EVP_PKEY_free(pkey);
			BIO_free(bioKey);

			paramCount += 3;

			
		} /* argv[1] = "value dsa/rsa" */

#endif
		else {
			printRegisterRequestUsage();
			delete rr;
			(*doc)->release();
			return NULL;
		}
	}

	if (proofOfPossessionKey != NULL) {

		// Set up the proof of possession
		DSIGSignature * s = 
			rr->addProofOfPossessionSignature(CANON_C14NE_NOC, proofOfPossessionSm);

		s->setSigningKey(proofOfPossessionKey);
		s->sign();

	}

	return rr;
}

// --------------------------------------------------------------------------------
//           Create a RevokeRequest
// --------------------------------------------------------------------------------

void printRevokeRequestUsage(void) {

	cerr << "\nUsage RevokeRequest [--help|-h] <service URI> [options]\n";
	cerr << "   --help/-h                : print this screen and exit\n\n";
	cerr << "   --add-name/-n <name>     : Add name as a KeyInfoName\n";
	cerr << "   --add-opaque/-o <data>   : Add an opaque data string\n";
	cerr << "   --add-usage-sig/-us      : Add Signature Key Usage\n";
	cerr << "   --add-usage-exc/-ux      : Add Exchange Key Usage\n";
	cerr << "   --add-usage-enc/-ue      : Add Encryption Key Usage\n";
	cerr << "   --add-usekeywith/-u <Application URI> <Identifier>\n";
	cerr << "                            : Add a UseKeyWith element\n";
	cerr << "   --add-respondwith/-r <Identifier>\n";
	cerr << "                            : Add a RespondWith element\n";
	cerr << "   --add-responsemechanism/-m <Identifier>\n";
	cerr << "                            : Add a ResponseMechanism element\n";
	cerr << "   --sign-dsa/-sd <filename> <passphrase>\n";
	cerr << "           : Sign using the DSA key in file protected by passphrase\n";
	cerr << "   --add-value-dsa/-vd <filename> <passphrase>\n";
	cerr << "           : Add the DSA key as a keyvalue\n";
	cerr << "   --add-value-rsa/-vr <filename> <passphrase>\n";
	cerr << "           : Add the RSA key as a keyvalue\n";
	cerr << "   --revocation/-v <phrase> : Set <phrase> as revocation code\n";
	cerr << "   --authenticate/-a <phrase>\n";
	cerr << "           : Use <phrase> as the authentication key for the request\n";
	cerr << "             NOTE - This must come *after* adding of KeyInfo elements\n\n";

}

XKMSMessageAbstractType * createRevokeRequest(XSECProvider &prov, DOMDocument **doc, int argc, char ** argv, int &paramCount, XKMSCompoundRequest * cr = NULL) {

	if (paramCount >= argc || 
		(_stricmp(argv[paramCount], "--help") == 0) ||
		(_stricmp(argv[paramCount], "-h") == 0)) {

		printRegisterRequestUsage();
		return NULL;
	}

	/* First create the basic request */
	XKMSMessageFactory * factory = 
		prov.getXKMSMessageFactory();
	XKMSRevokeRequest * rr;

	if (cr == NULL)
		rr = factory->createRevokeRequest(MAKE_UNICODE_STRING(argv[paramCount++]), doc);
	else
		rr = cr->createRevokeRequest(MAKE_UNICODE_STRING(argv[paramCount++]));

	while (paramCount < argc && _stricmp(argv[paramCount], "--") != 0) {

		if (_stricmp(argv[paramCount], "--add-name") == 0 || _stricmp(argv[paramCount], "-n") == 0) {
			if (++paramCount >= argc) {
				printRevokeRequestUsage();
				delete rr;
				return NULL;
			}

			XKMSRevokeKeyBinding * rkb = rr->getRevokeKeyBinding();
			if (rkb == NULL)
				rkb = rr->addRevokeKeyBinding(XKMSStatus::Indeterminate);
			rkb->appendKeyName(MAKE_UNICODE_STRING(argv[paramCount]));
			paramCount++;
		}
		else if (_stricmp(argv[paramCount], "--add-opaque") == 0 || _stricmp(argv[paramCount], "-o") == 0) {
			if (++paramCount >= argc) {
				printRevokeRequestUsage();
				delete rr;
				return NULL;
			}
			rr->appendOpaqueClientDataItem(MAKE_UNICODE_STRING(argv[paramCount]));
			paramCount++;
		}
		else if (_stricmp(argv[paramCount], "--add-respondwith") == 0 || _stricmp(argv[paramCount], "-r") == 0) {
			if (++paramCount >= argc) {
				printRevokeRequestUsage();
				delete rr;
				return NULL;
			}
			rr->appendRespondWithItem(MAKE_UNICODE_STRING(argv[paramCount]));
			paramCount++;
		}
		else if (_stricmp(argv[paramCount], "--add-responsemechanism") == 0 || _stricmp(argv[paramCount], "-m") == 0) {
			if (++paramCount >= argc) {
				printRevokeRequestUsage();
				delete rr;
				return NULL;
			}
			rr->appendResponseMechanismItem(MAKE_UNICODE_STRING(argv[paramCount]));
			paramCount++;
		}
		else if (_stricmp(argv[paramCount], "--add-usage-sig") == 0 || _stricmp(argv[paramCount], "-us") == 0) {
			XKMSRevokeKeyBinding * rkb = rr->getRevokeKeyBinding();
			if (rkb == NULL)
				rkb = rr->addRevokeKeyBinding(XKMSStatus::Indeterminate);
			rkb->setSignatureKeyUsage();
			paramCount++;
		}
		else if (_stricmp(argv[paramCount], "--add-usage-exc") == 0 || _stricmp(argv[paramCount], "-ux") == 0) {
			XKMSRevokeKeyBinding * rkb = rr->getRevokeKeyBinding();
			if (rkb == NULL)
				rkb = rr->addRevokeKeyBinding(XKMSStatus::Indeterminate);
			rkb->setExchangeKeyUsage();
			paramCount++;
		}
		else if (_stricmp(argv[paramCount], "--add-usage-enc") == 0 || _stricmp(argv[paramCount], "-ue") == 0) {
			XKMSRevokeKeyBinding * rkb = rr->getRevokeKeyBinding();
			if (rkb == NULL)
				rkb = rr->addRevokeKeyBinding(XKMSStatus::Indeterminate);
			rkb->setEncryptionKeyUsage();
			paramCount++;
		}
		else if (_stricmp(argv[paramCount], "--add-usekeywith") == 0 || _stricmp(argv[paramCount], "-u") == 0) {
			if (++paramCount >= argc + 1) {
				printRevokeRequestUsage();
				delete rr;
				return NULL;
			}
			XKMSRevokeKeyBinding *rkb = rr->getRevokeKeyBinding();
			if (rkb == NULL)
				rkb = rr->addRevokeKeyBinding(XKMSStatus::Indeterminate);

			rkb->appendUseKeyWithItem(MAKE_UNICODE_STRING(argv[paramCount]), MAKE_UNICODE_STRING(argv[paramCount + 1]));
			paramCount += 2;
		}
		else if (_stricmp(argv[paramCount], "--revocation") == 0 || _stricmp(argv[paramCount], "-v") == 0) {
			if (++paramCount >= argc) {
				printRevokeRequestUsage();
				delete rr;
				return NULL;
			}

			// Create the RevocationCode value
			unsigned char rciBuf[XSEC_MAX_HASH_SIZE];
			int len = CalculateXKMSRevocationCodeIdentifierEncoding1((unsigned char *) argv[paramCount], (int) strlen(argv[paramCount]), rciBuf, XSEC_MAX_HASH_SIZE);

			if (len <= 0) {
				cerr << "Error creating revocation code!\n";
				delete rr;
				return NULL;
			}

			// Convert to base64
			XMLCh * str = EncodeToBase64XMLCh(rciBuf, len);
			rr->addRevocationCode(str);
			XSEC_RELEASE_XMLCH(str);

			paramCount++;;
		}		else if (_stricmp(argv[paramCount], "--authenticate") == 0 || _stricmp(argv[paramCount], "-a") == 0) {
			if (++paramCount >= argc + 1) {
				printRevokeRequestUsage();
				delete rr;
				return NULL;
			}

			// Create the signature

			unsigned char keyBuf[XSEC_MAX_HASH_SIZE];
			int len = CalculateXKMSAuthenticationKey((unsigned char *) argv[paramCount], (int) strlen(argv[paramCount]), keyBuf, XSEC_MAX_HASH_SIZE);
			if (len <= 0) {
				cout << "Error creating key from pass phrase" << endl;
				delete rr;
				return NULL;
			}

			XSECCryptoKeyHMAC * k = XSECPlatformUtils::g_cryptoProvider->keyHMAC();
			k->setKey(keyBuf, len);

			// Set key and validate
			XKMSAuthentication * a = rr->addAuthentication();
			DSIGSignature * sig = a->addKeyBindingAuthenticationSignature();

			sig->setSigningKey(k);
			sig->sign();

			paramCount++;

		}
#if defined (XSEC_HAVE_OPENSSL)
		else if (_stricmp(argv[paramCount], "--sign-dsa") == 0 || _stricmp(argv[paramCount], "-sd") == 0 ||
				_stricmp(argv[paramCount], "--sign-rsa") == 0 || _stricmp(argv[paramCount], "-sr") == 0) {
			if (paramCount >= argc + 2) {
				printRegisterRequestUsage();
				delete rr;
				return NULL;
			}

			// DSA or RSA OpenSSL Key
			// For now just read a particular file

			BIO * bioKey;
			if ((bioKey = BIO_new(BIO_s_file())) == NULL) {

				cerr << "Error opening private key file\n\n";
				return NULL;

			}

			if (BIO_read_filename(bioKey, argv[paramCount+1]) <= 0) {

				cerr << "Error opening private key file : " << argv[paramCount+1] << endl;
				return NULL;

			}

			EVP_PKEY * pkey;
			pkey = PEM_read_bio_PrivateKey(bioKey,NULL,NULL,argv[paramCount + 2]);

			if (pkey == NULL) {

				BIO * bio_err;
	
				if ((bio_err=BIO_new(BIO_s_file())) != NULL)
					BIO_set_fp(bio_err,stderr,BIO_NOCLOSE|BIO_FP_TEXT);
				cerr << "Error loading private key\n\n";
				ERR_print_errors(bio_err);
				return NULL;

			}
			XSECCryptoKey *key;
			DSIGSignature * sig;
			if (_stricmp(argv[paramCount], "--sign-dsa") == 0 || _stricmp(argv[paramCount], "-sd") == 0) {

				// Check type is correct

				if (pkey->type != EVP_PKEY_DSA) {
					cerr << "DSA Key requested, but OpenSSL loaded something else\n";
					return NULL;
				}

				sig = rr->addSignature(CANON_C14N_NOC, SIGNATURE_DSA, HASH_SHA1);
				// Create the XSEC OpenSSL interface
				key = new OpenSSLCryptoKeyDSA(pkey);

				XMLCh * P = BN2b64(pkey->pkey.dsa->p);
				XMLCh * Q = BN2b64(pkey->pkey.dsa->q);
				XMLCh * G = BN2b64(pkey->pkey.dsa->g);
				XMLCh * Y = BN2b64(pkey->pkey.dsa->pub_key);

				sig->appendDSAKeyValue(P,Q,G,Y);

				XSEC_RELEASE_XMLCH(P);
				XSEC_RELEASE_XMLCH(Q);
				XSEC_RELEASE_XMLCH(G);
				XSEC_RELEASE_XMLCH(Y);
			}
			else {
				if (pkey->type != EVP_PKEY_RSA) {
					cerr << "RSA Key requested, but OpenSSL loaded something else\n";
					exit (1);
				}
				sig = rr->addSignature(CANON_C14N_NOC, SIGNATURE_RSA, HASH_SHA1);
				key = new OpenSSLCryptoKeyRSA(pkey);

				XMLCh * mod = BN2b64(pkey->pkey.rsa->n);
				XMLCh * exp = BN2b64(pkey->pkey.rsa->e);
				sig->appendRSAKeyValue(mod, exp);
				XSEC_RELEASE_XMLCH(mod);
				XSEC_RELEASE_XMLCH(exp);

			}

			sig->setSigningKey(key);
			sig->sign();

			EVP_PKEY_free(pkey);
			BIO_free(bioKey);

			paramCount += 3;

			
		} /* argv[1] = "sign dsa/rsa" */
		else if (_stricmp(argv[paramCount], "--add-value-dsa") == 0 || _stricmp(argv[paramCount], "-vd") == 0 ||
				_stricmp(argv[paramCount], "--add-value-rsa") == 0 || _stricmp(argv[paramCount], "-vr") == 0) {
			if (paramCount >= argc + 2) {
				printRegisterRequestUsage();
				delete rr;
				return NULL;
			}

			// DSA or RSA OpenSSL Key
			// For now just read a particular file

			BIO * bioKey;
			if ((bioKey = BIO_new(BIO_s_file())) == NULL) {

				cerr << "Error opening private key file\n\n";
				return NULL;

			}

			if (BIO_read_filename(bioKey, argv[paramCount+1]) <= 0) {

				cerr << "Error opening private key file : " << argv[paramCount+1] << endl;
				return NULL;

			}

			EVP_PKEY * pkey;
			pkey = PEM_read_bio_PrivateKey(bioKey,NULL,NULL,argv[paramCount + 2]);

			if (pkey == NULL) {

				BIO * bio_err;
	
				if ((bio_err=BIO_new(BIO_s_file())) != NULL)
					BIO_set_fp(bio_err,stderr,BIO_NOCLOSE|BIO_FP_TEXT);
				cerr << "Error loading private key\n\n";
				ERR_print_errors(bio_err);
				return NULL;

			}

			XKMSRevokeKeyBinding * rkb = rr->getRevokeKeyBinding();
			if (rkb == NULL)
				rkb = rr->addRevokeKeyBinding(XKMSStatus::Indeterminate);


			if (_stricmp(argv[paramCount], "--add-value-dsa") == 0 || _stricmp(argv[paramCount], "-vd") == 0) {

				// Check type is correct

				if (pkey->type != EVP_PKEY_DSA) {
					cerr << "DSA Key requested, but OpenSSL loaded something else\n";
					return NULL;
				}

				XMLCh * P = BN2b64(pkey->pkey.dsa->p);
				XMLCh * Q = BN2b64(pkey->pkey.dsa->q);
				XMLCh * G = BN2b64(pkey->pkey.dsa->g);
				XMLCh * Y = BN2b64(pkey->pkey.dsa->pub_key);

				rkb->appendDSAKeyValue(P,Q,G,Y);

				XSEC_RELEASE_XMLCH(P);
				XSEC_RELEASE_XMLCH(Q);
				XSEC_RELEASE_XMLCH(G);
				XSEC_RELEASE_XMLCH(Y);
			}
			else {
				if (pkey->type != EVP_PKEY_RSA) {
					cerr << "RSA Key requested, but OpenSSL loaded something else\n";
					exit (1);
				}

				XMLCh * mod = BN2b64(pkey->pkey.rsa->n);
				XMLCh * exp = BN2b64(pkey->pkey.rsa->e);
				rkb->appendRSAKeyValue(mod, exp);
				XSEC_RELEASE_XMLCH(mod);
				XSEC_RELEASE_XMLCH(exp);

			}

			EVP_PKEY_free(pkey);
			BIO_free(bioKey);

			paramCount += 3;

			
		} /* argv[1] = "value dsa/rsa" */

#endif
		else {
			printRevokeRequestUsage();
			delete rr;
			(*doc)->release();
			return NULL;
		}
	}

	return rr;
}

// --------------------------------------------------------------------------------
//           Create a ReissueRequest
// --------------------------------------------------------------------------------

void printReissueRequestUsage(void) {

	cerr << "\nUsage ReissueRequest [--help|-h] <service URI> [options]\n";
	cerr << "   --help/-h                : print this screen and exit\n\n";
	cerr << "   --add-name/-n <name>     : Add name as a KeyInfoName\n";
	cerr << "   --add-opaque/-o <data>   : Add an opaque data string\n";
	cerr << "   --add-usage-sig/-us      : Add Signature Key Usage\n";
	cerr << "   --add-usage-exc/-ux      : Add Exchange Key Usage\n";
	cerr << "   --add-usage-enc/-ue      : Add Encryption Key Usage\n";
	cerr << "   --add-usekeywith/-u <Application URI> <Identifier>\n";
	cerr << "                            : Add a UseKeyWith element\n";
	cerr << "   --add-respondwith/-r <Identifier>\n";
	cerr << "                            : Add a RespondWith element\n";
	cerr << "   --add-responsemechanism/-m <Identifier>\n";
	cerr << "                            : Add a ResponseMechanism element\n";
	cerr << "   --sign-dsa/-sd <filename> <passphrase>\n";
	cerr << "           : Sign using the DSA key in file protected by passphrase\n";
	cerr << "   --add-value-dsa/-vd <filename> <passphrase>\n";
	cerr << "           : Add the DSA key as a keyvalue (and do proof-of-possession sig)\n";
	cerr << "   --add-value-rsa/-vr <filename> <passphrase>\n";
	cerr << "           : Add the RSA key as a keyvalue (and do proof-of-possession sig)\n";
	cerr << "   --authenticate/-a <phrase>\n";
	cerr << "           : Use <phrase> as the authentication key for the request\n";
	cerr << "             NOTE - This must come *after* adding of KeyInfo elements\n\n";

}

XKMSMessageAbstractType * createReissueRequest(XSECProvider &prov, DOMDocument **doc, int argc, char ** argv, int &paramCount, XKMSCompoundRequest * cr = NULL) {

	XSECCryptoKey *proofOfPossessionKey = NULL;
	signatureMethod proofOfPossessionSm = SIGNATURE_DSA;

	if (paramCount >= argc || 
		(_stricmp(argv[paramCount], "--help") == 0) ||
		(_stricmp(argv[paramCount], "-h") == 0)) {

		printReissueRequestUsage();
		return NULL;
	}

	/* First create the basic request */
	XKMSMessageFactory * factory = 
		prov.getXKMSMessageFactory();
	XKMSReissueRequest * rr;

	if (cr == NULL)
		rr = factory->createReissueRequest(MAKE_UNICODE_STRING(argv[paramCount++]), doc);
	else
		rr = cr->createReissueRequest(MAKE_UNICODE_STRING(argv[paramCount++]));

	while (paramCount < argc && _stricmp(argv[paramCount], "--") != 0) {

		if (_stricmp(argv[paramCount], "--add-name") == 0 || _stricmp(argv[paramCount], "-n") == 0) {
			if (++paramCount >= argc) {
				printReissueRequestUsage();
				delete rr;
				return NULL;
			}

			XKMSReissueKeyBinding * pkb = rr->getReissueKeyBinding();
			if (pkb == NULL)
				pkb = rr->addReissueKeyBinding(XKMSStatus::Indeterminate);
			pkb->appendKeyName(MAKE_UNICODE_STRING(argv[paramCount]));
			paramCount++;
		}
		else if (_stricmp(argv[paramCount], "--add-opaque") == 0 || _stricmp(argv[paramCount], "-o") == 0) {
			if (++paramCount >= argc) {
				printReissueRequestUsage();
				delete rr;
				return NULL;
			}
			rr->appendOpaqueClientDataItem(MAKE_UNICODE_STRING(argv[paramCount]));
			paramCount++;
		}
		else if (_stricmp(argv[paramCount], "--add-respondwith") == 0 || _stricmp(argv[paramCount], "-r") == 0) {
			if (++paramCount >= argc) {
				printReissueRequestUsage();
				delete rr;
				return NULL;
			}
			rr->appendRespondWithItem(MAKE_UNICODE_STRING(argv[paramCount]));
			paramCount++;
		}
		else if (_stricmp(argv[paramCount], "--add-responsemechanism") == 0 || _stricmp(argv[paramCount], "-m") == 0) {
			if (++paramCount >= argc) {
				printReissueRequestUsage();
				delete rr;
				return NULL;
			}
			rr->appendResponseMechanismItem(MAKE_UNICODE_STRING(argv[paramCount]));
			paramCount++;
		}
		else if (_stricmp(argv[paramCount], "--add-usage-sig") == 0 || _stricmp(argv[paramCount], "-us") == 0) {
			XKMSReissueKeyBinding * pkb = rr->getReissueKeyBinding();
			if (pkb == NULL)
				pkb = rr->addReissueKeyBinding(XKMSStatus::Indeterminate);
			pkb->setSignatureKeyUsage();
			paramCount++;
		}
		else if (_stricmp(argv[paramCount], "--add-usage-exc") == 0 || _stricmp(argv[paramCount], "-ux") == 0) {
			XKMSReissueKeyBinding * pkb = rr->getReissueKeyBinding();
			if (pkb == NULL)
				pkb = rr->addReissueKeyBinding(XKMSStatus::Indeterminate);
			pkb->setExchangeKeyUsage();
			paramCount++;
		}
		else if (_stricmp(argv[paramCount], "--add-usage-enc") == 0 || _stricmp(argv[paramCount], "-ue") == 0) {
			XKMSReissueKeyBinding * pkb = rr->getReissueKeyBinding();
			if (pkb == NULL)
				pkb = rr->addReissueKeyBinding(XKMSStatus::Indeterminate);
			pkb->setEncryptionKeyUsage();
			paramCount++;
		}
		else if (_stricmp(argv[paramCount], "--add-usekeywith") == 0 || _stricmp(argv[paramCount], "-u") == 0) {
			if (++paramCount >= argc + 1) {
				printReissueRequestUsage();
				delete rr;
				return NULL;
			}
			XKMSReissueKeyBinding *pkb = rr->getReissueKeyBinding();
			if (pkb == NULL)
				pkb = rr->addReissueKeyBinding(XKMSStatus::Indeterminate);

			pkb->appendUseKeyWithItem(MAKE_UNICODE_STRING(argv[paramCount]), MAKE_UNICODE_STRING(argv[paramCount + 1]));
			paramCount += 2;
		}
		else if (_stricmp(argv[paramCount], "--authenticate") == 0 || _stricmp(argv[paramCount], "-a") == 0) {
			if (++paramCount >= argc + 1) {
				printReissueRequestUsage();
				delete rr;
				return NULL;
			}

			// Create the signature

			unsigned char keyBuf[XSEC_MAX_HASH_SIZE];
			int len = CalculateXKMSAuthenticationKey((unsigned char *) argv[paramCount], (int) strlen(argv[paramCount]), keyBuf, XSEC_MAX_HASH_SIZE);
			if (len <= 0) {
				cout << "Error creating key from pass phrase" << endl;
				delete rr;
				return NULL;
			}

			XSECCryptoKeyHMAC * k = XSECPlatformUtils::g_cryptoProvider->keyHMAC();
			k->setKey(keyBuf, len);

			// Set key and validate
			XKMSAuthentication * a = rr->addAuthentication();
			DSIGSignature * sig = a->addKeyBindingAuthenticationSignature();

			sig->setSigningKey(k);
			sig->sign();

			paramCount++;

		}
#if defined (XSEC_HAVE_OPENSSL)
		else if (_stricmp(argv[paramCount], "--sign-dsa") == 0 || _stricmp(argv[paramCount], "-sd") == 0 ||
				_stricmp(argv[paramCount], "--sign-rsa") == 0 || _stricmp(argv[paramCount], "-sr") == 0) {
			if (paramCount >= argc + 2) {
				printReissueRequestUsage();
				delete rr;
				return NULL;
			}

			// DSA or RSA OpenSSL Key
			// For now just read a particular file

			BIO * bioKey;
			if ((bioKey = BIO_new(BIO_s_file())) == NULL) {

				cerr << "Error opening private key file\n\n";
				return NULL;

			}

			if (BIO_read_filename(bioKey, argv[paramCount+1]) <= 0) {

				cerr << "Error opening private key file : " << argv[paramCount+1] << endl;
				return NULL;

			}

			EVP_PKEY * pkey;
			pkey = PEM_read_bio_PrivateKey(bioKey,NULL,NULL,argv[paramCount + 2]);

			if (pkey == NULL) {

				BIO * bio_err;
	
				if ((bio_err=BIO_new(BIO_s_file())) != NULL)
					BIO_set_fp(bio_err,stderr,BIO_NOCLOSE|BIO_FP_TEXT);
				cerr << "Error loading private key\n\n";
				ERR_print_errors(bio_err);
				return NULL;

			}
			XSECCryptoKey *key;
			DSIGSignature * sig;
			if (_stricmp(argv[paramCount], "--sign-dsa") == 0 || _stricmp(argv[paramCount], "-sd") == 0) {

				// Check type is correct

				if (pkey->type != EVP_PKEY_DSA) {
					cerr << "DSA Key requested, but OpenSSL loaded something else\n";
					return NULL;
				}

				sig = rr->addSignature(CANON_C14N_NOC, SIGNATURE_DSA, HASH_SHA1);
				// Create the XSEC OpenSSL interface
				key = new OpenSSLCryptoKeyDSA(pkey);

				XMLCh * P = BN2b64(pkey->pkey.dsa->p);
				XMLCh * Q = BN2b64(pkey->pkey.dsa->q);
				XMLCh * G = BN2b64(pkey->pkey.dsa->g);
				XMLCh * Y = BN2b64(pkey->pkey.dsa->pub_key);

				sig->appendDSAKeyValue(P,Q,G,Y);

				XSEC_RELEASE_XMLCH(P);
				XSEC_RELEASE_XMLCH(Q);
				XSEC_RELEASE_XMLCH(G);
				XSEC_RELEASE_XMLCH(Y);
			}
			else {
				if (pkey->type != EVP_PKEY_RSA) {
					cerr << "RSA Key requested, but OpenSSL loaded something else\n";
					exit (1);
				}
				sig = rr->addSignature(CANON_C14N_NOC, SIGNATURE_RSA, HASH_SHA1);
				key = new OpenSSLCryptoKeyRSA(pkey);

				XMLCh * mod = BN2b64(pkey->pkey.rsa->n);
				XMLCh * exp = BN2b64(pkey->pkey.rsa->e);
				sig->appendRSAKeyValue(mod, exp);
				XSEC_RELEASE_XMLCH(mod);
				XSEC_RELEASE_XMLCH(exp);

			}

			sig->setSigningKey(key);
			sig->sign();

			EVP_PKEY_free(pkey);
			BIO_free(bioKey);

			paramCount += 3;

			
		} /* argv[1] = "sign dsa/rsa" */
		else if (_stricmp(argv[paramCount], "--add-value-dsa") == 0 || _stricmp(argv[paramCount], "-vd") == 0 ||
				_stricmp(argv[paramCount], "--add-value-rsa") == 0 || _stricmp(argv[paramCount], "-vr") == 0) {
			if (paramCount >= argc + 2) {
				printReissueRequestUsage();
				delete rr;
				return NULL;
			}

			// DSA or RSA OpenSSL Key
			// For now just read a particular file

			BIO * bioKey;
			if ((bioKey = BIO_new(BIO_s_file())) == NULL) {

				cerr << "Error opening private key file\n\n";
				return NULL;

			}

			if (BIO_read_filename(bioKey, argv[paramCount+1]) <= 0) {

				cerr << "Error opening private key file : " << argv[paramCount+1] << endl;
				return NULL;

			}

			EVP_PKEY * pkey;
			pkey = PEM_read_bio_PrivateKey(bioKey,NULL,NULL,argv[paramCount + 2]);

			if (pkey == NULL) {

				BIO * bio_err;
	
				if ((bio_err=BIO_new(BIO_s_file())) != NULL)
					BIO_set_fp(bio_err,stderr,BIO_NOCLOSE|BIO_FP_TEXT);
				cerr << "Error loading private key\n\n";
				ERR_print_errors(bio_err);
				return NULL;

			}

			XKMSReissueKeyBinding * pkb = rr->getReissueKeyBinding();
			if (pkb == NULL)
				pkb = rr->addReissueKeyBinding(XKMSStatus::Indeterminate);


			if (_stricmp(argv[paramCount], "--add-value-dsa") == 0 || _stricmp(argv[paramCount], "-vd") == 0) {

				// Check type is correct

				if (pkey->type != EVP_PKEY_DSA) {
					cerr << "DSA Key requested, but OpenSSL loaded something else\n";
					return NULL;
				}

				proofOfPossessionKey = new OpenSSLCryptoKeyDSA(pkey);
				proofOfPossessionSm = SIGNATURE_DSA;

				XMLCh * P = BN2b64(pkey->pkey.dsa->p);
				XMLCh * Q = BN2b64(pkey->pkey.dsa->q);
				XMLCh * G = BN2b64(pkey->pkey.dsa->g);
				XMLCh * Y = BN2b64(pkey->pkey.dsa->pub_key);

				pkb->appendDSAKeyValue(P,Q,G,Y);

				XSEC_RELEASE_XMLCH(P);
				XSEC_RELEASE_XMLCH(Q);
				XSEC_RELEASE_XMLCH(G);
				XSEC_RELEASE_XMLCH(Y);
			}
			else {
				if (pkey->type != EVP_PKEY_RSA) {
					cerr << "RSA Key requested, but OpenSSL loaded something else\n";
					exit (1);
				}

				proofOfPossessionKey = new OpenSSLCryptoKeyRSA(pkey);
				proofOfPossessionSm = SIGNATURE_RSA;

				XMLCh * mod = BN2b64(pkey->pkey.rsa->n);
				XMLCh * exp = BN2b64(pkey->pkey.rsa->e);
				pkb->appendRSAKeyValue(mod, exp);
				XSEC_RELEASE_XMLCH(mod);
				XSEC_RELEASE_XMLCH(exp);

			}

			EVP_PKEY_free(pkey);
			BIO_free(bioKey);

			paramCount += 3;

			
		} /* argv[1] = "value dsa/rsa" */

#endif
		else {
			printReissueRequestUsage();
			delete rr;
			(*doc)->release();
			return NULL;
		}
	}

	if (proofOfPossessionKey != NULL) {

		// Set up the proof of possession
		DSIGSignature * s = 
			rr->addProofOfPossessionSignature(CANON_C14NE_NOC, proofOfPossessionSm);

		s->setSigningKey(proofOfPossessionKey);
		s->sign();

	}

	return rr;
}

// --------------------------------------------------------------------------------
//           Create a RecoverRequest
// --------------------------------------------------------------------------------

void printRecoverRequestUsage(void) {

	cerr << "\nUsage RecoverRequest [--help|-h] <service URI> [options]\n";
	cerr << "   --help/-h                : print this screen and exit\n\n";
	cerr << "   --add-name/-n <name>     : Add name as a KeyInfoName\n";
	cerr << "   --add-opaque/-o <data>   : Add an opaque data string\n";
	cerr << "   --add-usage-sig/-us      : Add Signature Key Usage\n";
	cerr << "   --add-usage-exc/-ux      : Add Exchange Key Usage\n";
	cerr << "   --add-usage-enc/-ue      : Add Encryption Key Usage\n";
	cerr << "   --add-usekeywith/-u <Application URI> <Identifier>\n";
	cerr << "                            : Add a UseKeyWith element\n";
	cerr << "   --add-respondwith/-r <Identifier>\n";
	cerr << "                            : Add a RespondWith element\n";
	cerr << "   --add-responsemechanism/-m <Identifier>\n";
	cerr << "                            : Add a ResponseMechanism element\n";
	cerr << "   --sign-dsa/-sd <filename> <passphrase>\n";
	cerr << "           : Sign using the DSA key in file protected by passphrase\n";
	cerr << "   --add-value-dsa/-vd <filename> <passphrase>\n";
	cerr << "           : Add the DSA key as a keyvalue\n";
	cerr << "   --add-value-rsa/-vr <filename> <passphrase>\n";
	cerr << "           : Add the RSA key as a keyvalue\n";
	cerr << "   --kek/-k <phrase>        : Key phrase to use for PrivateKey decryption\n";
#if defined (XSEC_HAVE_OPENSSL)
	cerr << "   --output-private-key/-p <file> <pass phrase>\n";
	cerr << "                            : Write PEM encoded private key to file\n";
#endif	
	cerr << "   --authenticate/-a <phrase>\n";
	cerr << "           : Use <phrase> as the authentication key for the request\n";
	cerr << "             NOTE - This must come *after* adding of KeyInfo elements\n\n";

}

XKMSMessageAbstractType * createRecoverRequest(XSECProvider &prov, DOMDocument **doc, int argc, char ** argv, int &paramCount, XKMSCompoundRequest * cr = NULL) {

	if (paramCount >= argc || 
		(_stricmp(argv[paramCount], "--help") == 0) ||
		(_stricmp(argv[paramCount], "-h") == 0)) {

		printRegisterRequestUsage();
		return NULL;
	}

	/* First create the basic request */
	XKMSMessageFactory * factory = 
		prov.getXKMSMessageFactory();
	XKMSRecoverRequest * rr;

	if (cr == NULL)
		rr = factory->createRecoverRequest(MAKE_UNICODE_STRING(argv[paramCount++]), doc);
	else
		rr = cr->createRecoverRequest(MAKE_UNICODE_STRING(argv[paramCount++]));

	while (paramCount < argc && _stricmp(argv[paramCount], "--") != 0) {

		if (_stricmp(argv[paramCount], "--add-name") == 0 || _stricmp(argv[paramCount], "-n") == 0) {
			if (++paramCount >= argc) {
				printRecoverRequestUsage();
				delete rr;
				return NULL;
			}

			XKMSRecoverKeyBinding * rkb = rr->getRecoverKeyBinding();
			if (rkb == NULL)
				rkb = rr->addRecoverKeyBinding(XKMSStatus::Indeterminate);
			rkb->appendKeyName(MAKE_UNICODE_STRING(argv[paramCount]));
			paramCount++;
		}
		else if (_stricmp(argv[paramCount], "--add-opaque") == 0 || _stricmp(argv[paramCount], "-o") == 0) {
			if (++paramCount >= argc) {
				printRecoverRequestUsage();
				delete rr;
				return NULL;
			}
			rr->appendOpaqueClientDataItem(MAKE_UNICODE_STRING(argv[paramCount]));
			paramCount++;
		}
		else if (_stricmp(argv[paramCount], "--add-respondwith") == 0 || _stricmp(argv[paramCount], "-r") == 0) {
			if (++paramCount >= argc) {
				printRecoverRequestUsage();
				delete rr;
				return NULL;
			}
			rr->appendRespondWithItem(MAKE_UNICODE_STRING(argv[paramCount]));
			paramCount++;
		}
		else if (_stricmp(argv[paramCount], "--add-responsemechanism") == 0 || _stricmp(argv[paramCount], "-m") == 0) {
			if (++paramCount >= argc) {
				printRecoverRequestUsage();
				delete rr;
				return NULL;
			}
			rr->appendResponseMechanismItem(MAKE_UNICODE_STRING(argv[paramCount]));
			paramCount++;
		}
		else if (_stricmp(argv[paramCount], "--add-usage-sig") == 0 || _stricmp(argv[paramCount], "-us") == 0) {
			XKMSRecoverKeyBinding * rkb = rr->getRecoverKeyBinding();
			if (rkb == NULL)
				rkb = rr->addRecoverKeyBinding(XKMSStatus::Indeterminate);
			rkb->setSignatureKeyUsage();
			paramCount++;
		}
		else if (_stricmp(argv[paramCount], "--add-usage-exc") == 0 || _stricmp(argv[paramCount], "-ux") == 0) {
			XKMSRecoverKeyBinding * rkb = rr->getRecoverKeyBinding();
			if (rkb == NULL)
				rkb = rr->addRecoverKeyBinding(XKMSStatus::Indeterminate);
			rkb->setExchangeKeyUsage();
			paramCount++;
		}
		else if (_stricmp(argv[paramCount], "--add-usage-enc") == 0 || _stricmp(argv[paramCount], "-ue") == 0) {
			XKMSRecoverKeyBinding * rkb = rr->getRecoverKeyBinding();
			if (rkb == NULL)
				rkb = rr->addRecoverKeyBinding(XKMSStatus::Indeterminate);
			rkb->setEncryptionKeyUsage();
			paramCount++;
		}
		else if (_stricmp(argv[paramCount], "--add-usekeywith") == 0 || _stricmp(argv[paramCount], "-u") == 0) {
			if (++paramCount >= argc + 1) {
				printRecoverRequestUsage();
				delete rr;
				return NULL;
			}
			XKMSRecoverKeyBinding *rkb = rr->getRecoverKeyBinding();
			if (rkb == NULL)
				rkb = rr->addRecoverKeyBinding(XKMSStatus::Indeterminate);

			rkb->appendUseKeyWithItem(MAKE_UNICODE_STRING(argv[paramCount]), MAKE_UNICODE_STRING(argv[paramCount + 1]));
			paramCount += 2;
		}
		else if (_stricmp(argv[paramCount], "--kek") == 0 || _stricmp(argv[paramCount], "-k") == 0) {
			if (++paramCount >= argc) {
				printRecoverRequestUsage();
				delete rr;
				return NULL;
			}
			g_authPassPhrase = argv[paramCount++];
		}
#if defined (XSEC_HAVE_OPENSSL)
		else if (_stricmp(argv[paramCount], "--output-private-key") == 0 || _stricmp(argv[paramCount], "-p") == 0) {
			if (paramCount >= argc + 2) {
				printRecoverRequestUsage();
				delete rr;
				return NULL;
			}
			++paramCount;
			g_privateKeyFile = argv[paramCount++];
			g_privateKeyPassPhrase = argv[paramCount++];
		}
#endif		
		else if (_stricmp(argv[paramCount], "--authenticate") == 0 || _stricmp(argv[paramCount], "-a") == 0) {
			if (++paramCount >= argc + 1) {
				printRecoverRequestUsage();
				delete rr;
				return NULL;
			}

			// Create the signature

			unsigned char keyBuf[XSEC_MAX_HASH_SIZE];
			int len = CalculateXKMSAuthenticationKey((unsigned char *) argv[paramCount], (int) strlen(argv[paramCount]), keyBuf, XSEC_MAX_HASH_SIZE);
			if (len <= 0) {
				cout << "Error creating key from pass phrase" << endl;
				delete rr;
				return NULL;
			}

			XSECCryptoKeyHMAC * k = XSECPlatformUtils::g_cryptoProvider->keyHMAC();
			k->setKey(keyBuf, len);

			// Set key and validate
			XKMSAuthentication * a = rr->addAuthentication();
			DSIGSignature * sig = a->addKeyBindingAuthenticationSignature();

			sig->setSigningKey(k);
			sig->sign();

			paramCount++;

		}
#if defined (XSEC_HAVE_OPENSSL)
		else if (_stricmp(argv[paramCount], "--sign-dsa") == 0 || _stricmp(argv[paramCount], "-sd") == 0 ||
				_stricmp(argv[paramCount], "--sign-rsa") == 0 || _stricmp(argv[paramCount], "-sr") == 0) {
			if (paramCount >= argc + 2) {
				printRegisterRequestUsage();
				delete rr;
				return NULL;
			}

			// DSA or RSA OpenSSL Key
			// For now just read a particular file

			BIO * bioKey;
			if ((bioKey = BIO_new(BIO_s_file())) == NULL) {

				cerr << "Error opening private key file\n\n";
				return NULL;

			}

			if (BIO_read_filename(bioKey, argv[paramCount+1]) <= 0) {

				cerr << "Error opening private key file : " << argv[paramCount+1] << endl;
				return NULL;

			}

			EVP_PKEY * pkey;
			pkey = PEM_read_bio_PrivateKey(bioKey,NULL,NULL,argv[paramCount + 2]);

			if (pkey == NULL) {

				BIO * bio_err;
	
				if ((bio_err=BIO_new(BIO_s_file())) != NULL)
					BIO_set_fp(bio_err,stderr,BIO_NOCLOSE|BIO_FP_TEXT);
				cerr << "Error loading private key\n\n";
				ERR_print_errors(bio_err);
				return NULL;

			}
			XSECCryptoKey *key;
			DSIGSignature * sig;
			if (_stricmp(argv[paramCount], "--sign-dsa") == 0 || _stricmp(argv[paramCount], "-sd") == 0) {

				// Check type is correct

				if (pkey->type != EVP_PKEY_DSA) {
					cerr << "DSA Key requested, but OpenSSL loaded something else\n";
					return NULL;
				}

				sig = rr->addSignature(CANON_C14N_NOC, SIGNATURE_DSA, HASH_SHA1);
				// Create the XSEC OpenSSL interface
				key = new OpenSSLCryptoKeyDSA(pkey);

				XMLCh * P = BN2b64(pkey->pkey.dsa->p);
				XMLCh * Q = BN2b64(pkey->pkey.dsa->q);
				XMLCh * G = BN2b64(pkey->pkey.dsa->g);
				XMLCh * Y = BN2b64(pkey->pkey.dsa->pub_key);

				sig->appendDSAKeyValue(P,Q,G,Y);

				XSEC_RELEASE_XMLCH(P);
				XSEC_RELEASE_XMLCH(Q);
				XSEC_RELEASE_XMLCH(G);
				XSEC_RELEASE_XMLCH(Y);
			}
			else {
				if (pkey->type != EVP_PKEY_RSA) {
					cerr << "RSA Key requested, but OpenSSL loaded something else\n";
					exit (1);
				}
				sig = rr->addSignature(CANON_C14N_NOC, SIGNATURE_RSA, HASH_SHA1);
				key = new OpenSSLCryptoKeyRSA(pkey);

				XMLCh * mod = BN2b64(pkey->pkey.rsa->n);
				XMLCh * exp = BN2b64(pkey->pkey.rsa->e);
				sig->appendRSAKeyValue(mod, exp);
				XSEC_RELEASE_XMLCH(mod);
				XSEC_RELEASE_XMLCH(exp);

			}

			sig->setSigningKey(key);
			sig->sign();

			EVP_PKEY_free(pkey);
			BIO_free(bioKey);

			paramCount += 3;

			
		} /* argv[1] = "sign dsa/rsa" */
		else if (_stricmp(argv[paramCount], "--add-value-dsa") == 0 || _stricmp(argv[paramCount], "-vd") == 0 ||
				_stricmp(argv[paramCount], "--add-value-rsa") == 0 || _stricmp(argv[paramCount], "-vr") == 0) {
			if (paramCount >= argc + 2) {
				printRegisterRequestUsage();
				delete rr;
				return NULL;
			}

			// DSA or RSA OpenSSL Key
			// For now just read a particular file

			BIO * bioKey;
			if ((bioKey = BIO_new(BIO_s_file())) == NULL) {

				cerr << "Error opening private key file\n\n";
				return NULL;

			}

			if (BIO_read_filename(bioKey, argv[paramCount+1]) <= 0) {

				cerr << "Error opening private key file : " << argv[paramCount+1] << endl;
				return NULL;

			}

			EVP_PKEY * pkey;
			pkey = PEM_read_bio_PrivateKey(bioKey,NULL,NULL,argv[paramCount + 2]);

			if (pkey == NULL) {

				BIO * bio_err;
	
				if ((bio_err=BIO_new(BIO_s_file())) != NULL)
					BIO_set_fp(bio_err,stderr,BIO_NOCLOSE|BIO_FP_TEXT);
				cerr << "Error loading private key\n\n";
				ERR_print_errors(bio_err);
				return NULL;

			}

			XKMSRecoverKeyBinding * rkb = rr->getRecoverKeyBinding();
			if (rkb == NULL)
				rkb = rr->addRecoverKeyBinding(XKMSStatus::Indeterminate);


			if (_stricmp(argv[paramCount], "--add-value-dsa") == 0 || _stricmp(argv[paramCount], "-vd") == 0) {

				// Check type is correct

				if (pkey->type != EVP_PKEY_DSA) {
					cerr << "DSA Key requested, but OpenSSL loaded something else\n";
					return NULL;
				}

				XMLCh * P = BN2b64(pkey->pkey.dsa->p);
				XMLCh * Q = BN2b64(pkey->pkey.dsa->q);
				XMLCh * G = BN2b64(pkey->pkey.dsa->g);
				XMLCh * Y = BN2b64(pkey->pkey.dsa->pub_key);

				rkb->appendDSAKeyValue(P,Q,G,Y);

				XSEC_RELEASE_XMLCH(P);
				XSEC_RELEASE_XMLCH(Q);
				XSEC_RELEASE_XMLCH(G);
				XSEC_RELEASE_XMLCH(Y);
			}
			else {
				if (pkey->type != EVP_PKEY_RSA) {
					cerr << "RSA Key requested, but OpenSSL loaded something else\n";
					exit (1);
				}

				XMLCh * mod = BN2b64(pkey->pkey.rsa->n);
				XMLCh * exp = BN2b64(pkey->pkey.rsa->e);
				rkb->appendRSAKeyValue(mod, exp);
				XSEC_RELEASE_XMLCH(mod);
				XSEC_RELEASE_XMLCH(exp);

			}

			EVP_PKEY_free(pkey);
			BIO_free(bioKey);

			paramCount += 3;

			
		} /* argv[1] = "value dsa/rsa" */

#endif
		else {
			printRecoverRequestUsage();
			delete rr;
			(*doc)->release();
			return NULL;
		}
	}

	return rr;
}

// --------------------------------------------------------------------------------
//           Create a PendingRequest
// --------------------------------------------------------------------------------

void printPendingRequestUsage(void) {

	cerr << "\nUsage PendingRequest [--help|-h] <service URI> [options]\n";
	cerr << "   --help/-h                : print this screen and exit\n";
	cerr << "   --original-request-id/-o <id>\n";
	cerr << "                            : Set original request ID\n";
	cerr << "   --response-id/-r <id>\n";
	cerr << "                            : Set Response ID\n\n";

}

XKMSMessageAbstractType * createPendingRequest(XSECProvider &prov, DOMDocument **doc, int argc, char ** argv, int paramCount) {

	if (paramCount  >= argc || 
		(_stricmp(argv[paramCount], "--help") == 0) ||
		(_stricmp(argv[paramCount], "-h") == 0)) {

		printPendingRequestUsage();
		return NULL;
	}

	/* First create the basic request */
	XKMSMessageFactory * factory = 
		prov.getXKMSMessageFactory();
	XKMSPendingRequest * pr = 
		factory->createPendingRequest(MAKE_UNICODE_STRING(argv[paramCount++]), doc);

	while (paramCount < argc && _stricmp(argv[paramCount], "--") != 0) {

		if (_stricmp(argv[paramCount], "--original-request-id") == 0 || _stricmp(argv[paramCount], "-o") == 0) {
			if (++paramCount >= argc) {
				printPendingRequestUsage();
				delete pr;
				return NULL;
			}

			pr->setOriginalRequestId(MAKE_UNICODE_STRING(argv[paramCount++]));
		}
		else if (_stricmp(argv[paramCount], "--response-id") == 0 || _stricmp(argv[paramCount], "-r") == 0) {
			if (++paramCount >= argc) {
				printPendingRequestUsage();
				delete pr;
				return NULL;
			}
			pr->setResponseId(MAKE_UNICODE_STRING(argv[paramCount++]));
		}
		else {
			printPendingRequestUsage();
			delete pr;
			return NULL;
		}
	}

	return pr;
}

// --------------------------------------------------------------------------------
//           Create a StatusRequest
// --------------------------------------------------------------------------------

void printStatusRequestUsage(void) {

	cerr << "\nUsage StatusRequest [--help|-h] <service URI> [options]\n";
	cerr << "   --help/-h                : print this screen and exit\n";
	cerr << "   --original-request-id/-o <id>\n";
	cerr << "                            : Set original request ID\n";
	cerr << "   --response-id/-r <id>\n";
	cerr << "                            : Set Response ID\n\n";

}

XKMSMessageAbstractType * createStatusRequest(XSECProvider &prov, DOMDocument **doc, int argc, char ** argv, int paramCount) {

	if (paramCount  >= argc || 
		(_stricmp(argv[paramCount], "--help") == 0) ||
		(_stricmp(argv[paramCount], "-h") == 0)) {

		printStatusRequestUsage();
		return NULL;
	}

	/* First create the basic request */
	XKMSMessageFactory * factory = 
		prov.getXKMSMessageFactory();
	XKMSStatusRequest * sr = 
		factory->createStatusRequest(MAKE_UNICODE_STRING(argv[paramCount++]), doc);

	while (paramCount < argc && _stricmp(argv[paramCount], "--") != 0) {

		if (_stricmp(argv[paramCount], "--original-request-id") == 0 || _stricmp(argv[paramCount], "-o") == 0) {
			if (++paramCount >= argc) {
				printStatusRequestUsage();
				delete sr;
				return NULL;
			}

			sr->setOriginalRequestId(MAKE_UNICODE_STRING(argv[paramCount++]));
		}
		else if (_stricmp(argv[paramCount], "--response-id") == 0 || _stricmp(argv[paramCount], "-r") == 0) {
			if (++paramCount >= argc) {
				printStatusRequestUsage();
				delete sr;
				return NULL;
			}
			sr->setResponseId(MAKE_UNICODE_STRING(argv[paramCount++]));
		}
		else {
			printStatusRequestUsage();
			delete sr;
			return NULL;
		}
	}

	return sr;
}

// --------------------------------------------------------------------------------
//           Create a CompoundRequest
// --------------------------------------------------------------------------------

void printCompoundRequestUsage(void) {

	cerr << "\nUsage CompoundRequest [--help|-h] <service URI> <LocateRequest|ValidateRequest> .... [-- LocateRequest|ValidateRequest|PendingRequest]*\n";
	cerr << "   --help/-h                : print this screen and exit\n\n";

}

XKMSMessageAbstractType * createCompoundRequest(XSECProvider &prov, DOMDocument **doc, int argc, char ** argv, int paramCount) {

	if (paramCount >= argc || 
		(_stricmp(argv[paramCount], "--help") == 0) ||
		(_stricmp(argv[paramCount], "-h") == 0)) {

		printCompoundRequestUsage();
		return NULL;
	}

	/* First create the basic request */
	XKMSMessageFactory * factory = 
		prov.getXKMSMessageFactory();
	XKMSCompoundRequest * cr = 
		factory->createCompoundRequest(MAKE_UNICODE_STRING(argv[paramCount++]), doc);

	while (paramCount < argc) {

		if ((_stricmp(argv[paramCount], "LocateRequest") == 0) ||
			(_stricmp(argv[paramCount], "lr") == 0)) {

			paramCount++;
			XKMSLocateRequest * r = 
				(XKMSLocateRequest *) (createLocateRequest(prov, NULL, argc, argv, paramCount, cr));

			if (r == NULL) {
				delete cr;
				return NULL;
			}

		}
		else if ((_stricmp(argv[paramCount], "ValidateRequest") == 0) ||
			(_stricmp(argv[paramCount], "vr") == 0)) {

			paramCount++;
			XKMSValidateRequest * r = 
				(XKMSValidateRequest *) (createValidateRequest(prov, NULL, argc, argv, paramCount, cr));

			if (r == NULL) {
				delete cr;
				return NULL;
			}
		}

		else {
			printCompoundRequestUsage();
			delete cr;
			(*doc)->release();
			return NULL;
		}

		if (paramCount < argc && _stricmp(argv[paramCount], "--") == 0)
			paramCount++;
	}

	return cr;
}

// --------------------------------------------------------------------------------
//           MsgDump
// --------------------------------------------------------------------------------

void doMessageAbstractTypeDump(XKMSMessageAbstractType *msg, int level) {

	cout << endl;
	levelSet(level);
	cout << "Base message information : " << endl << endl;

	char * s = XMLString::transcode(msg->getId());
	levelSet(level);
	cout << "Id = " << s << endl;
	XSEC_RELEASE_XMLCH(s);

	s = XMLString::transcode(msg->getService());
	levelSet(level);
	cout << "Service URI = " << s << endl;
	XSEC_RELEASE_XMLCH(s);

	s = XMLString::transcode(msg->getNonce());
	levelSet(level);
	if (s != NULL) {
		cout << "Nonce = " << s << endl;
		XSEC_RELEASE_XMLCH(s);
	}
	else
		cout << "Nonce = <NONE SET>" << endl;

	/* Check for OpaqueClientData */
	if (msg->getOpaqueClientDataSize() > 0) {
		levelSet(level);
		cout << "Opaque Client Data found : " << endl;
		for (int i = 0; i < msg->getOpaqueClientDataSize(); ++i) {
			s = XMLString::transcode(msg->getOpaqueClientDataItemStr(i));
			if (s != NULL) {
				levelSet(level + 1);
				cout << i << " : " << s << endl;
				XSEC_RELEASE_XMLCH(s);
			}
		}
	}
}

void doRequestAbstractTypeDump(XKMSRequestAbstractType *msg, int level) {

	levelSet(level);
	int i = msg->getRespondWithSize();

	cout << "Request message has " << i << " RespondWith elements" << endl << endl;

	for (int j = 0; j < i; ++j) {
		levelSet(level +1);
		char * s = XMLString::transcode(msg->getRespondWithItemStr(j));
		cout << "Item " << j+1 << " : " << s << endl;
		XSEC_RELEASE_XMLCH(s);
	}
	
}

void doResultTypeDump(XKMSResultType *msg, int level) {

	const XMLCh * rid = msg->getRequestId();
	char * s;

	if (rid != NULL) {
		levelSet(level);
		cout << "Result is in response to MsgID : ";
		s = XMLString::transcode(rid);
		cout << s << endl;
		XSEC_RELEASE_XMLCH(s);
	}

	levelSet(level);
	cout << "Result Major code = ";
	s = XMLString::transcode(XKMSConstants::s_tagResultMajorCodes[msg->getResultMajor()]);
	cout << s << endl;
	XSEC_RELEASE_XMLCH(s);

	XKMSResultType::ResultMinor rm = msg->getResultMinor();
	if (rm != XKMSResultType::NoneMinor) {
		levelSet(level);
		cout << "Result Minor code = ";
		char * s = XMLString::transcode(XKMSConstants::s_tagResultMinorCodes[rm]);
		cout << s << endl;
		XSEC_RELEASE_XMLCH(s);
	}

	rid = msg->getRequestSignatureValue();
	if (rid != NULL) {
		levelSet(level);
		cout << "RequestSignatureValue = ";
		s = XMLString::transcode(rid);
		cout << s << endl;
		XSEC_RELEASE_XMLCH(s);
	}
}

void doKeyInfoDump(DSIGKeyInfoList * l, int level) {


	int size = (int) l->getSize();

	for (int i = 0 ; i < size ; ++ i) {

		DSIGKeyInfoValue * kiv;
		DSIGKeyInfoName * kn;
		char * b;

		DSIGKeyInfo * ki = l->item(i);

		switch (ki->getKeyInfoType()) {

		case DSIGKeyInfo::KEYINFO_VALUE_RSA :

			kiv = (DSIGKeyInfoValue *) ki;
			levelSet(level);
			cout << "RSA Key Value" << endl;

			levelSet(level+1);
			b = XMLString::transcode(kiv->getRSAExponent());
			cout << "Base64 encoded exponent = " << b << endl;
			delete[] b;

			levelSet(level+1);
			b = XMLString::transcode(kiv->getRSAModulus());
			cout << "Base64 encoded modulus  = " << b << endl;
			delete[] b;

			break;

		case DSIGKeyInfo::KEYINFO_VALUE_DSA :

			kiv = (DSIGKeyInfoValue *) ki;
			levelSet(level);
			cout << "DSA Key Value" << endl;

			levelSet(level+1);
			b = XMLString::transcode(kiv->getDSAG());
			cout << "G = " << b << endl;
			delete[] b;

			levelSet(level+1);
			b = XMLString::transcode(kiv->getDSAP());
			cout << "P = " << b << endl;
			delete[] b;

			levelSet(level+1);
			b = XMLString::transcode(kiv->getDSAQ());
			cout << "Q = " << b << endl;
			delete[] b;

			levelSet(level+1);
			b = XMLString::transcode(kiv->getDSAY());
			cout << "Y = " << b << endl;
			delete[] b;

			break;

		case DSIGKeyInfo::KEYINFO_NAME :

			kn = (DSIGKeyInfoName *) ki;
			levelSet(level);
			cout << "Key Name" << endl;

			levelSet(level+1);
			b = XMLString::transcode(kn->getKeyName());
			cout << "Name = " << b << endl;
			delete[] b;

			break;

		default:

			levelSet(level);
			cout << "Unknown KeyInfo type" << endl;

		}

	}
}


void doKeyBindingAbstractDump(XKMSKeyBindingAbstractType * msg, int level) {

	levelSet(level);
	cout << "Key Binding found." << endl;

	levelSet(level+1);
	cout << "KeyUsage Encryption : ";
	if (msg->getEncryptionKeyUsage())
		cout << "yes" << endl;
	else
		cout << "no" << endl;

	levelSet(level+1);
	cout << "KeyUsage Exchange   : ";
	if (msg->getExchangeKeyUsage())
		cout << "yes" << endl;
	else
		cout << "no" << endl;

	levelSet(level+1);
	cout << "KeyUsage Signature  : ";
	if (msg->getSignatureKeyUsage())
		cout << "yes" << endl;
	else
		cout << "no" << endl;

	int n = msg->getUseKeyWithSize();
	levelSet(level+1);
	if (n == 0) {
		cout << "No UseKeyWith items found" << endl;
	}
	else {
		cout << "UseKeyWith items : \n";
	}

	for (int i = 0; i < msg->getUseKeyWithSize() ; ++i) {

		XKMSUseKeyWith * ukw = msg->getUseKeyWithItem(i);
		levelSet(level+2);
		char * a = XMLString::transcode(ukw->getApplication());
		char * id = XMLString::transcode(ukw->getIdentifier());
		cout << "Application : \"" << a << "\"\n";
		levelSet(level+2);
		cout << "Identifier  : \"" << id << "\"" << endl;
		XSEC_RELEASE_XMLCH(a);
		XSEC_RELEASE_XMLCH(id);

	}

	// Now dump any KeyInfo
	levelSet(level+1);
	cout << "KeyInfo information:" << endl << endl;

	doKeyInfoDump(msg->getKeyInfoList(), level + 2);

}

void doUnverifiedKeyBindingDump(XKMSUnverifiedKeyBinding * ukb, int level) {

	doKeyBindingAbstractDump((XKMSKeyBindingAbstractType *) ukb, level);

}

void doStatusReasonDump(XKMSStatus::StatusValue v, XKMSStatus *s, int level) {

	char * sr = XMLString::transcode(XKMSConstants::s_tagStatusValueCodes[v]);
	for (XKMSStatus::StatusReason i = XKMSStatus::Signature; i > XKMSStatus::ReasonUndefined; i = (XKMSStatus::StatusReason) (i-1)) {

		if (s->getStatusReason(v, i)) {
			levelSet(level);
			char * rc = XMLString::transcode(XKMSConstants::s_tagStatusReasonCodes[i]);
			cout << sr << "Reason = " << rc << endl;
			XSEC_RELEASE_XMLCH(rc);
		}
	}
	XSEC_RELEASE_XMLCH(sr);

}

void doKeyBindingDump(XKMSKeyBinding * kb, int level) {

	/* Dump the status */

	XKMSStatus * s = kb->getStatus();
	if (s == NULL)
		return;

	char * sr = XMLString::transcode(XKMSConstants::s_tagStatusValueCodes[s->getStatusValue()]);
	levelSet(level);
	cout << "Status = " << sr << endl;
	XSEC_RELEASE_XMLCH(sr);

	/* Dump the status reasons */
	doStatusReasonDump(XKMSStatus::Valid, s, level+1);
	doStatusReasonDump(XKMSStatus::Invalid, s, level+1);
	doStatusReasonDump(XKMSStatus::Indeterminate, s, level+1);

	/* Now the actual key */
	doKeyBindingAbstractDump((XKMSKeyBindingAbstractType *) kb, level);

}

void doRevokeKeyBindingDump(XKMSRevokeKeyBinding * kb, int level) {

	/* Dump the status */

	XKMSStatus * s = kb->getStatus();
	if (s == NULL)
		return;

	char * sr = XMLString::transcode(XKMSConstants::s_tagStatusValueCodes[s->getStatusValue()]);
	levelSet(level);
	cout << "Status = " << sr << endl;
	XSEC_RELEASE_XMLCH(sr);

	/* Dump the status reasons */
	doStatusReasonDump(XKMSStatus::Valid, s, level+1);
	doStatusReasonDump(XKMSStatus::Invalid, s, level+1);
	doStatusReasonDump(XKMSStatus::Indeterminate, s, level+1);

	/* Now the actual key */
	doKeyBindingAbstractDump((XKMSKeyBindingAbstractType *) kb, level);

}

void doRecoverKeyBindingDump(XKMSRecoverKeyBinding * kb, int level) {

	/* Dump the status */

	XKMSStatus * s = kb->getStatus();
	if (s == NULL)
		return;

	char * sr = XMLString::transcode(XKMSConstants::s_tagStatusValueCodes[s->getStatusValue()]);
	levelSet(level);
	cout << "Status = " << sr << endl;
	XSEC_RELEASE_XMLCH(sr);

	/* Dump the status reasons */
	doStatusReasonDump(XKMSStatus::Valid, s, level+1);
	doStatusReasonDump(XKMSStatus::Invalid, s, level+1);
	doStatusReasonDump(XKMSStatus::Indeterminate, s, level+1);

	/* Now the actual key */
	doKeyBindingAbstractDump((XKMSKeyBindingAbstractType *) kb, level);

}

void doReissueKeyBindingDump(XKMSReissueKeyBinding * kb, int level) {

	/* Dump the status */

	XKMSStatus * s = kb->getStatus();
	if (s == NULL)
		return;

	char * sr = XMLString::transcode(XKMSConstants::s_tagStatusValueCodes[s->getStatusValue()]);
	levelSet(level);
	cout << "Status = " << sr << endl;
	XSEC_RELEASE_XMLCH(sr);

	/* Dump the status reasons */
	doStatusReasonDump(XKMSStatus::Valid, s, level+1);
	doStatusReasonDump(XKMSStatus::Invalid, s, level+1);
	doStatusReasonDump(XKMSStatus::Indeterminate, s, level+1);

	/* Now the actual key */
	doKeyBindingAbstractDump((XKMSKeyBindingAbstractType *) kb, level);

}

void doAuthenticationDump(XKMSAuthentication *a, int level) {


	if (a == NULL)
		return;

	DSIGSignature * sig = a->getKeyBindingAuthenticationSignature();
	if (sig != NULL) {

		levelSet(level);
		cout << "KeyBindingAuthentication Signature found.  ";
		if (g_authPassPhrase == NULL) {
			cout << "Cannot check - no pass phrase set" << endl;
			return;
		}
		
		cout << "Checking.... ";

		// Create the key
		unsigned char keyBuf[XSEC_MAX_HASH_SIZE];
		int len = CalculateXKMSAuthenticationKey((unsigned char *) g_authPassPhrase, (int) strlen(g_authPassPhrase), keyBuf, XSEC_MAX_HASH_SIZE);
		if (len <= 0) {
			cout << "Error creating key from pass phrase" << endl;
			return;
		}

		XSECCryptoKeyHMAC * k = XSECPlatformUtils::g_cryptoProvider->keyHMAC();
		k->setKey(keyBuf, len);
		//k->setKey((unsigned char *) g_authPassPhrase, strlen(g_authPassPhrase));

		// Set key and validate
		sig->setSigningKey(k);
		if (sig->verify())
			cout << "OK!" << endl;
		else 
			cout << "Signature BAD!" << endl;

	}

	return;

}

int doLocateRequestDump(XKMSLocateRequest *msg) {

	cout << endl << "This is a LocateRequest Message" << endl;
	int level = 1;
	
	doMessageAbstractTypeDump(msg, level);
	doRequestAbstractTypeDump(msg, level);

	XKMSQueryKeyBinding *qkb = msg->getQueryKeyBinding();
	if (qkb != NULL)
		doKeyBindingAbstractDump(qkb, level);

	return 0;
}

int doStatusRequestDump(XKMSStatusRequest *msg) {

	cout << endl << "This is a StatusRequest Message" << endl;
	int level = 1;
	
	doMessageAbstractTypeDump(msg, level);
	doRequestAbstractTypeDump(msg, level);

	return 0;
}

int doValidateRequestDump(XKMSValidateRequest *msg) {

	cout << endl << "This is a ValidateRequest Message" << endl;
	int level = 1;
	
	doMessageAbstractTypeDump(msg, level);
	doRequestAbstractTypeDump(msg, level);

	XKMSQueryKeyBinding *qkb = msg->getQueryKeyBinding();
	if (qkb != NULL)
		doKeyBindingAbstractDump(qkb, level);

	return 0;
}

int doLocateResultDump(XKMSLocateResult *msg) {

	cout << endl << "This is a LocateResult Message" << endl;
	int level = 1;
	
	doMessageAbstractTypeDump(msg, level);
	doResultTypeDump(msg, level);

	int j;

	if ((j = msg->getUnverifiedKeyBindingSize()) > 0) {

		cout << endl;
		levelSet(level);
		cout << "Unverified Key Bindings" << endl << endl;

		for (int i = 0; i < j ; ++i) {

			doUnverifiedKeyBindingDump(msg->getUnverifiedKeyBindingItem(i), level + 1);

		}

	}

	return 0;
}

int doValidateResultDump(XKMSValidateResult *msg) {

	cout << endl << "This is a ValidateResult Message" << endl;
	int level = 1;
	
	doMessageAbstractTypeDump(msg, level);
	doResultTypeDump(msg, level);

	int j;

	if ((j = msg->getKeyBindingSize()) > 0) {

		cout << endl;
		levelSet(level);
		cout << "Key Bindings" << endl << endl;

		for (int i = 0; i < j ; ++i) {

			doKeyBindingDump(msg->getKeyBindingItem(i), level + 1);

		}

	}

	return 0;
}

int doRegisterResultDump(XKMSRegisterResult *msg) {

	cout << endl << "This is a RegisterResult Message" << endl;
	int level = 1;
	
	doMessageAbstractTypeDump(msg, level);
	doResultTypeDump(msg, level);

	int j;

	if ((j = msg->getKeyBindingSize()) > 0) {

		cout << endl;
		levelSet(level);
		cout << "Key Bindings" << endl << endl;

		for (int i = 0; i < j ; ++i) {

			doKeyBindingDump(msg->getKeyBindingItem(i), level + 1);

		}

	}

	// Check if there is a private key
	if (g_authPassPhrase) {
		XKMSRSAKeyPair * kp = msg->getRSAKeyPair(g_authPassPhrase);
		if (kp != NULL) {
			cout << endl;
			levelSet(level);
			cout << "RSAKeyPair found" << endl << endl;
			level += 1;

			// Translate the parameters to char strings
			char * sModulus = XMLString::transcode(kp->getModulus());
			char * sExponent = XMLString::transcode(kp->getExponent());
			char * sP = XMLString::transcode(kp->getP());
			char * sQ = XMLString::transcode(kp->getQ());
			char * sDP = XMLString::transcode(kp->getDP());
			char * sDQ = XMLString::transcode(kp->getDQ());
			char * sInverseQ = XMLString::transcode(kp->getInverseQ());
			char * sD = XMLString::transcode(kp->getD());

#if defined (XSEC_HAVE_OPENSSL)

			if (g_privateKeyFile != NULL) {
				levelSet(level);
				cout << "Writing private key to file " << g_privateKeyFile;

				// Create the RSA key file
				RSA * rsa = RSA_new();
				rsa->n = OpenSSLCryptoBase64::b642BN(sModulus, (unsigned int) strlen(sModulus));
				rsa->e = OpenSSLCryptoBase64::b642BN(sExponent, (unsigned int) strlen(sExponent));
				rsa->d = OpenSSLCryptoBase64::b642BN(sD, (unsigned int) strlen(sD));
				rsa->p = OpenSSLCryptoBase64::b642BN(sP, (unsigned int) strlen(sP));
				rsa->q = OpenSSLCryptoBase64::b642BN(sQ, (unsigned int) strlen(sQ));
				rsa->dmp1 = OpenSSLCryptoBase64::b642BN(sDP, (unsigned int) strlen(sDP));
				rsa->dmq1 = OpenSSLCryptoBase64::b642BN(sDQ, (unsigned int) strlen(sDQ));
				rsa->iqmp = OpenSSLCryptoBase64::b642BN(sInverseQ, (unsigned int) strlen(sInverseQ));

				// Write it to disk
				BIO *out;
				out = BIO_new_file(g_privateKeyFile, "w");
				if(!out) cout << "Error occurred in opening file!" << endl << endl;
				if (!PEM_write_bio_RSAPrivateKey(out, rsa, EVP_des_ede3_cbc(), NULL, 0, 0, g_privateKeyPassPhrase)) {
					cout << "Error creating PEM output" << endl << endl;
				}
				BIO_free(out);
				RSA_free(rsa);

				cout << " done" << endl << endl;

			}
#endif
			// Now output
			levelSet(level);
			cout << "Modulus = " << sModulus << endl;
			XSEC_RELEASE_XMLCH(sModulus);
			
			levelSet(level);
			cout << "Exponent = " << sExponent << endl;
			XSEC_RELEASE_XMLCH(sExponent);

			levelSet(level);
			cout << "P = " << sP << endl;
			XSEC_RELEASE_XMLCH(sP);

			levelSet(level);
			cout << "Q = " << sQ << endl;
			XSEC_RELEASE_XMLCH(sQ);

			levelSet(level);
			cout << "DP = " << sDP << endl;
			XSEC_RELEASE_XMLCH(sDP);

			levelSet(level);
			cout << "DQ = " << sDQ << endl;
			XSEC_RELEASE_XMLCH(sDQ);

			levelSet(level);
			cout << "Inverse Q = " << sInverseQ << endl;
			XSEC_RELEASE_XMLCH(sInverseQ);

			levelSet(level);
			cout << "D = " << sD << endl;
			XSEC_RELEASE_XMLCH(sD);
		}
	}
	else {
		levelSet(level);
		cout << "Not checking for private key as no decryption phrase set";
	}

	return 0;
}

int doRecoverResultDump(XKMSRecoverResult *msg) {

	cout << endl << "This is a RecoverResult Message" << endl;
	int level = 1;
	
	doMessageAbstractTypeDump(msg, level);
	doResultTypeDump(msg, level);

	int j;

	if ((j = msg->getKeyBindingSize()) > 0) {

		cout << endl;
		levelSet(level);
		cout << "Key Bindings" << endl << endl;

		for (int i = 0; i < j ; ++i) {

			doKeyBindingDump(msg->getKeyBindingItem(i), level + 1);

		}

	}

	// Check if there is a private key
	if (g_authPassPhrase) {
		XKMSRSAKeyPair * kp = msg->getRSAKeyPair(g_authPassPhrase);
		if (kp != NULL) {
			cout << endl;
			levelSet(level);
			cout << "RSAKeyPair found" << endl << endl;
			level += 1;

			// Translate the parameters to char strings
			char * sModulus = XMLString::transcode(kp->getModulus());
			char * sExponent = XMLString::transcode(kp->getExponent());
			char * sP = XMLString::transcode(kp->getP());
			char * sQ = XMLString::transcode(kp->getQ());
			char * sDP = XMLString::transcode(kp->getDP());
			char * sDQ = XMLString::transcode(kp->getDQ());
			char * sInverseQ = XMLString::transcode(kp->getInverseQ());
			char * sD = XMLString::transcode(kp->getD());

#if defined (XSEC_HAVE_OPENSSL)

			if (g_privateKeyFile != NULL) {
				levelSet(level);
				cout << "Writing private key to file " << g_privateKeyFile;

				// Create the RSA key file
				RSA * rsa = RSA_new();
				rsa->n = OpenSSLCryptoBase64::b642BN(sModulus, (unsigned int) strlen(sModulus));
				rsa->e = OpenSSLCryptoBase64::b642BN(sExponent, (unsigned int) strlen(sExponent));
				rsa->d = OpenSSLCryptoBase64::b642BN(sD, (unsigned int) strlen(sD));
				rsa->p = OpenSSLCryptoBase64::b642BN(sP, (unsigned int) strlen(sP));
				rsa->q = OpenSSLCryptoBase64::b642BN(sQ, (unsigned int) strlen(sQ));
				rsa->dmp1 = OpenSSLCryptoBase64::b642BN(sDP, (unsigned int) strlen(sDP));
				rsa->dmq1 = OpenSSLCryptoBase64::b642BN(sDQ, (unsigned int) strlen(sDQ));
				rsa->iqmp = OpenSSLCryptoBase64::b642BN(sInverseQ, (unsigned int) strlen(sInverseQ));

				// Write it to disk
				BIO *out;
				out = BIO_new_file(g_privateKeyFile, "w");
				if(!out) cout << "Error occurred in opening file!" << endl << endl;
				if (!PEM_write_bio_RSAPrivateKey(out, rsa, EVP_des_ede3_cbc(), NULL, 0, 0, g_privateKeyPassPhrase)) {
					cout << "Error creating PEM output" << endl << endl;
				}
				BIO_free(out);
				RSA_free(rsa);

				cout << " done" << endl << endl;

			}
#endif
			// Now output
			levelSet(level);
			cout << "Modulus = " << sModulus << endl;
			XSEC_RELEASE_XMLCH(sModulus);
			
			levelSet(level);
			cout << "Exponent = " << sExponent << endl;
			XSEC_RELEASE_XMLCH(sExponent);

			levelSet(level);
			cout << "P = " << sP << endl;
			XSEC_RELEASE_XMLCH(sP);

			levelSet(level);
			cout << "Q = " << sQ << endl;
			XSEC_RELEASE_XMLCH(sQ);

			levelSet(level);
			cout << "DP = " << sDP << endl;
			XSEC_RELEASE_XMLCH(sDP);

			levelSet(level);
			cout << "DQ = " << sDQ << endl;
			XSEC_RELEASE_XMLCH(sDQ);

			levelSet(level);
			cout << "Inverse Q = " << sInverseQ << endl;
			XSEC_RELEASE_XMLCH(sInverseQ);

			levelSet(level);
			cout << "D = " << sD << endl;
			XSEC_RELEASE_XMLCH(sD);
		}
	}
	else {
		levelSet(level);
		cout << "Not checking for private key as no decryption phrase set";
	}

	return 0;
}

int doRevokeResultDump(XKMSRevokeResult *msg) {

	cout << endl << "This is a RevokeResult Message" << endl;
	int level = 1;
	
	doMessageAbstractTypeDump(msg, level);
	doResultTypeDump(msg, level);

	int j;

	if ((j = msg->getKeyBindingSize()) > 0) {

		cout << endl;
		levelSet(level);
		cout << "Key Bindings" << endl << endl;

		for (int i = 0; i < j ; ++i) {

			doKeyBindingDump(msg->getKeyBindingItem(i), level + 1);

		}

	}

	return 0;
}

int doReissueResultDump(XKMSReissueResult *msg) {

	cout << endl << "This is a ReissueResult Message" << endl;
	int level = 1;
	
	doMessageAbstractTypeDump(msg, level);
	doResultTypeDump(msg, level);

	int j;

	if ((j = msg->getKeyBindingSize()) > 0) {

		cout << endl;
		levelSet(level);
		cout << "Key Bindings" << endl << endl;

		for (int i = 0; i < j ; ++i) {

			doKeyBindingDump(msg->getKeyBindingItem(i), level + 1);

		}

	}

	return 0;
}

int doStatusResultDump(XKMSStatusResult *msg) {

	cout << endl << "This is a StatusResult Message" << endl;
	int level = 1;
	
	doMessageAbstractTypeDump(msg, level);
	doResultTypeDump(msg, level);

	return 0;
}

int doResultDump(XKMSResult *msg) {

	cout << endl << "This is a Result Message" << endl;
	int level = 1;
	
	doMessageAbstractTypeDump(msg, level);
	doResultTypeDump(msg, level);

	return 0;
}

int doRegisterRequestDump(XKMSRegisterRequest *msg) {

	cout << endl << "This is a RegisterRequest Message" << endl;
	int level = 1;
	
	doMessageAbstractTypeDump(msg, level);
	doRequestAbstractTypeDump(msg, level);

	XKMSPrototypeKeyBinding *pkb = msg->getPrototypeKeyBinding();
	if (pkb != NULL) {
		const XMLCh * rci = pkb->getRevocationCodeIdentifier();
		if (rci != NULL) {
			levelSet(1);
			char * sr = XMLString::transcode(rci);
			cout << "Revocation Code found = " << sr << endl;
			XSEC_RELEASE_XMLCH(sr);
		}

		doKeyBindingAbstractDump(pkb, level);
	}

	// Check authentication
	doAuthenticationDump(msg->getAuthentication(), level);

	// Check ProofOfPossession
	levelSet(1);
	DSIGSignature * sig = msg->getProofOfPossessionSignature();
	if (sig != NULL) {

		cout << "Proof of PossessionSignature signature found. Checking.... ";

		// Try to find the key
		XSECKeyInfoResolverDefault kir;
		XSECCryptoKey * k = kir.resolveKey(pkb->getKeyInfoList());

		if (k != NULL) {

			sig->setSigningKey(k);
			if (sig->verify())
				cout << "OK!" << endl;
			else 
				cout << "Signature Bad!" << endl;
		}
		else
			cout << "Cannot obtain key from PrototypeKeyBinding" << endl;

	}
	else {

		cout << "No ProofOfPossession Signature found" << endl;

	}

	return 0;

}

int doRevokeRequestDump(XKMSRevokeRequest *msg) {

	cout << endl << "This is a RevokeRequest Message" << endl;
	int level = 1;
	
	doMessageAbstractTypeDump(msg, level);
	doRequestAbstractTypeDump(msg, level);

	XKMSRevokeKeyBinding *rkb = msg->getRevokeKeyBinding();
	if (rkb != NULL) {
		doRevokeKeyBindingDump(rkb, level);
	}

	// Check authentication
	if (msg->getAuthentication())
		doAuthenticationDump(msg->getAuthentication(), level);

	if (msg->getRevocationCode()) {

		levelSet(1);
		cout << "RevocationCode found = ";
		char * sr = XMLString::transcode(msg->getRevocationCode());
		cout << sr << endl;
		XSEC_RELEASE_XMLCH(sr);
	}

	return 0;

}

int doRecoverRequestDump(XKMSRecoverRequest *msg) {

	cout << endl << "This is a RecoverRequest Message" << endl;
	int level = 1;
	
	doMessageAbstractTypeDump(msg, level);
	doRequestAbstractTypeDump(msg, level);

	XKMSRecoverKeyBinding *rkb = msg->getRecoverKeyBinding();
	if (rkb != NULL) {
		doRecoverKeyBindingDump(rkb, level);
	}

	// Check authentication
	if (msg->getAuthentication())
		doAuthenticationDump(msg->getAuthentication(), level);

	return 0;

}

int doReissueRequestDump(XKMSReissueRequest *msg) {

	cout << endl << "This is a ReiussueRequest Message" << endl;
	int level = 1;
	
	doMessageAbstractTypeDump(msg, level);
	doRequestAbstractTypeDump(msg, level);

	XKMSReissueKeyBinding *rkb = msg->getReissueKeyBinding();
	if (rkb != NULL) {
		doReissueKeyBindingDump(rkb, level);
	}

	// Check authentication
	doAuthenticationDump(msg->getAuthentication(), level);

	// Check ProofOfPossession
	levelSet(1);
	DSIGSignature * sig = msg->getProofOfPossessionSignature();
	if (sig != NULL) {

		cout << "Proof of PossessionSignature signature found. Checking.... ";

		// Try to find the key
		XSECKeyInfoResolverDefault kir;
		XSECCryptoKey * k = kir.resolveKey(rkb->getKeyInfoList());

		if (k != NULL) {

			sig->setSigningKey(k);
			if (sig->verify())
				cout << "OK!" << endl;
			else 
				cout << "Signature Bad!" << endl;
		}
		else
			cout << "Cannot obtain key from PrototypeKeyBinding" << endl;

	}
	else {

		cout << "No ProofOfPossession Signature found" << endl;

	}

	return 0;

}

int doMsgDump(XKMSMessageAbstractType * msg) {

	if (msg->isSigned()) {

		cout << "Message is signed.  Checking signature ... ";
		try {

			XSECKeyInfoResolverDefault theKeyInfoResolver;
			DSIGSignature * sig = msg->getSignature();

			// The only way we can verify is using keys read directly from the KeyInfo list,
			// so we add a KeyInfoResolverDefault to the Signature.

			sig->setKeyInfoResolver(&theKeyInfoResolver);

			if (sig->verify())
				cout << "OK!" << endl;
			else
				cout << "Bad!" << endl;

		}
	
		catch (XSECException &e) {
			cout << "Bad!.  Caught exception : " << endl;
			char * msg = XMLString::transcode(e.getMsg());
			cout << msg << endl;
			XSEC_RELEASE_XMLCH(msg);
		}
	}

	int i;

	switch (msg->getMessageType()) {

	case XKMSMessageAbstractType::CompoundRequest :

		cout << "Compound Request\n\n";

		for (i = 0 ; i < ((XKMSCompoundRequest *) (msg))->getRequestListSize(); ++i) {

			cout << "Message " << i << endl;
			doMsgDump(((XKMSCompoundRequest *) msg)->getRequestListItem(i));

		}
		break;
	
	case XKMSMessageAbstractType::CompoundResult :

		cout << "Compound Result\n\n";

		for (i = 0 ; i < ((XKMSCompoundResult *)(msg))->getResultListSize(); ++i) {

			cout << "Message " << i << endl;
			doMsgDump(((XKMSCompoundResult *) msg)->getResultListItem(i));

		}
		break;
	
	case XKMSMessageAbstractType::LocateRequest :

		doLocateRequestDump((XKMSLocateRequest *) msg);
		break;

	case XKMSMessageAbstractType::LocateResult :

		doLocateResultDump((XKMSLocateResult *) msg);
		break;

	case XKMSMessageAbstractType::StatusRequest :

		doStatusRequestDump((XKMSStatusRequest *) msg);
		break;

	case XKMSMessageAbstractType::StatusResult :

		doStatusResultDump((XKMSStatusResult *) msg);
		break;

	case XKMSMessageAbstractType::Result :

		doResultDump((XKMSResult *) msg);
		break;

	case XKMSMessageAbstractType::ValidateRequest :

		doValidateRequestDump((XKMSValidateRequest *) msg);
		break;

	case XKMSMessageAbstractType::ValidateResult :

		doValidateResultDump((XKMSValidateResult *) msg);
		break;

	case XKMSMessageAbstractType::RegisterRequest :

		doRegisterRequestDump((XKMSRegisterRequest *) msg);
		break;

	case XKMSMessageAbstractType::RegisterResult :

		doRegisterResultDump((XKMSRegisterResult *) msg);
		break;

	case XKMSMessageAbstractType::RevokeRequest :

		doRevokeRequestDump((XKMSRevokeRequest *) msg);
		break;

	case XKMSMessageAbstractType::RevokeResult :

		doRevokeResultDump((XKMSRevokeResult *) msg);
		break;

	case XKMSMessageAbstractType::RecoverRequest :

		doRecoverRequestDump((XKMSRecoverRequest *) msg);
		break;

	case XKMSMessageAbstractType::RecoverResult :

		doRecoverResultDump((XKMSRecoverResult *) msg);
		break;

	case XKMSMessageAbstractType::ReissueRequest :

		doReissueRequestDump((XKMSReissueRequest *) msg);
		break;

	case XKMSMessageAbstractType::ReissueResult :

		doReissueResultDump((XKMSReissueResult *) msg);
		break;

	default :

		cout << "Unknown message type!" << endl;

	}

	return 0;
}

int doParsedMsgDump(DOMDocument * doc) {

	// Get an XKMS Message Factory
	XSECProvider prov;
	XKMSMessageFactory * factory = prov.getXKMSMessageFactory();
	int errorsOccured;

	try {

		XKMSMessageAbstractType * msg =
			factory->newMessageFromDOM(doc->getDocumentElement());

		if (msg == NULL) {
			cerr << "Unable to create XKMS msg from parsed DOM\n" << endl;
			return 2;
		}

		Janitor <XKMSMessageAbstractType> j_msg(msg);

		return doMsgDump(msg);

	}

	catch (XSECException &e) {
		char * msg = XMLString::transcode(e.getMsg());
		cerr << "An error occured during message loading\n   Message: "
		<< msg << endl;
		XSEC_RELEASE_XMLCH(msg);
		errorsOccured = true;
		return 2;
	}
	catch (XSECCryptoException &e) {
		cerr << "An error occured during encryption/signature processing\n   Message: "
		<< e.getMsg() << endl;
		errorsOccured = true;

#if defined (XSEC_HAVE_OPENSSL)
		ERR_load_crypto_strings();
		BIO * bio_err;
		if ((bio_err=BIO_new(BIO_s_file())) != NULL)
			BIO_set_fp(bio_err,stderr,BIO_NOCLOSE|BIO_FP_TEXT);

		ERR_print_errors(bio_err);
#endif

		return 2;
	}
	catch (...) {

		cerr << "Unknown Exception type occured.  Cleaning up and exitting\n" << endl;
		return 2;

	}

	// Clean up

	return 0;
}

// --------------------------------------------------------------------------------
//           Base MessageCreate module
// --------------------------------------------------------------------------------

void printMsgCreateUsage(void) {

	cerr << "\nUsage messagecreate [options] {LocateRequest} [msg specific options]\n";
	cerr << "   --help/-h    : print this screen and exit\n\n";

}

int doMsgCreate(int argc, char ** argv, int paramCount) {

	XSECProvider prov;
	DOMDocument * doc;
	XKMSMessageAbstractType *msg;

	if (paramCount >= argc || 
		(_stricmp(argv[paramCount], "--help") == 0) ||
		(_stricmp(argv[paramCount], "-h") == 0)) {
		printMsgCreateUsage();
		return -1;
	}

	if ((_stricmp(argv[paramCount], "LocateRequest") == 0) ||
		(_stricmp(argv[paramCount], "lr") == 0)) {

		paramCount++;
		msg = createLocateRequest(prov, &doc, argc, argv, paramCount);
		if (msg == NULL) {
			return -1;
		}

	}
	else {

		printMsgCreateUsage();
		return -1;

	}

	outputDoc(doc);
	
	// Cleanup message stuff
	delete msg;
	doc->release();

	return 0;

}


// --------------------------------------------------------------------------------
//           Base request module
// --------------------------------------------------------------------------------

void printDoRequestUsage(void) {

	cerr << "\nUsage request [options] {RequestType} [msg specific options]\n";
	cerr << "   --help/-h       : Print this screen and exit\n";
	cerr << "   --two-phase/-t  : Indicate Two-Phase support in the request message\n";
	cerr << "   --nonce/-n [nonce]\n";
	cerr << "                   : Set two phase nonce value\n";
	cerr << "   --original-requestid/-o [id]\n";
	cerr << "                   : set OriginalRequestId attribute in request\n";
	cerr << "   --envelope-type/-e [NONE|SOAP11|SOAP12]\n";
    cerr << "                   : Set envelope wrapper for request\n";
	cerr << "                         NONE   = No wrapper - straight HTTP request\n";
	cerr << "                         SOAP11 = Use a SOAP 1.1 envelope\n";
	cerr << "                         SOAP12 = Use a SOAP 1.2 envelope\n\n";
	cerr << "                     Where RequestType = one of :\n";
	cerr << "                         CompoundRequest (cr)\n";
	cerr << "                         LocateRequest   (lr)\n";
	cerr << "                         ValidateRequest (vr)\n";
	cerr << "                         PendingRequest  (pr)\n";
	cerr << "                         RegisterRequest (rr)\n";
	cerr << "                         ReissueRequest  (ir)\n";
	cerr << "                         RecoverRequest  (or)\n";
	cerr << "                         RevokeRequest   (er)\n\n";

}

int doRequest(int argc, char ** argv, int paramCount) {

	XSECProvider prov;
	DOMDocument * doc;
	XKMSMessageAbstractType *msg = NULL;
	XSECSOAPRequestorSimple::envelopeType et = XSECSOAPRequestorSimple::ENVELOPE_SOAP11;

	bool twoPhase = false;
	bool parmsDone = false;

	char * nonce = NULL;
	char * originalRequestId = NULL;

	if (paramCount >= argc || 
		(_stricmp(argv[paramCount], "--help") == 0) ||
		(_stricmp(argv[paramCount], "-h") == 0)) {
		printDoRequestUsage();
		return -1;
	}

	while (!parmsDone) {
		if ((_stricmp(argv[paramCount], "--two-phase") == 0) ||
			(_stricmp(argv[paramCount], "-t") == 0)) {

			twoPhase = true;
			paramCount++;

		}
		else if ((_stricmp(argv[paramCount], "--nonce") == 0) ||
			(_stricmp(argv[paramCount], "-n") == 0)) {

			paramCount++;
			if (paramCount == argc) {
				printDoRequestUsage();
				return -1;
			}

			nonce=argv[paramCount++];
		}
		else if ((_stricmp(argv[paramCount], "--original-requestid") == 0) ||
			(_stricmp(argv[paramCount], "-o") == 0)) {

			paramCount++;
			if (paramCount == argc) {
				printDoRequestUsage();
				return -1;
			}

			originalRequestId=argv[paramCount++];
		}		
		else if ((_stricmp(argv[paramCount], "--envelope") == 0) ||
			(_stricmp(argv[paramCount], "-e") == 0)) {

			// Set the wrapper envelope type

			paramCount++;
			if (paramCount == argc) {
				printDoRequestUsage();
				return -1;
			}

			if (_stricmp(argv[paramCount], "NONE") == 0) {
				et = XSECSOAPRequestorSimple::ENVELOPE_NONE;
			}
			else if (_stricmp(argv[paramCount], "SOAP11") == 0) {
				et = XSECSOAPRequestorSimple::ENVELOPE_SOAP11;
			}
			else if (_stricmp(argv[paramCount], "SOAP12") == 0) {
				et = XSECSOAPRequestorSimple::ENVELOPE_SOAP12;
			}
			else {
				printDoRequestUsage();
				return -1;
			}
			paramCount++;
		}
		else if ((_stricmp(argv[paramCount], "LocateRequest") == 0) ||
			(_stricmp(argv[paramCount], "lr") == 0)) {

			paramCount++;
			XKMSLocateRequest * r = 
				(XKMSLocateRequest *) (createLocateRequest(prov, &doc, argc, argv, paramCount));

			if (r == NULL) {
				return -1;
			}

			if (twoPhase)
				r->appendResponseMechanismItem(XKMSConstants::s_tagRepresent);

			msg = r;
			parmsDone = true;

		}
		else if ((_stricmp(argv[paramCount], "ValidateRequest") == 0) ||
			(_stricmp(argv[paramCount], "vr") == 0)) {

			paramCount++;
			XKMSValidateRequest * r = 
				(XKMSValidateRequest *) (createValidateRequest(prov, &doc, argc, argv, paramCount));

			if (r == NULL) {
				return -1;
			}
			if (twoPhase)
				r->appendResponseMechanismItem(XKMSConstants::s_tagRepresent);

			msg = r;
			parmsDone = true;

		}
		else if ((_stricmp(argv[paramCount], "RegisterRequest") == 0) ||
			(_stricmp(argv[paramCount], "rr") == 0)) {

			paramCount++;
			XKMSRegisterRequest * r = 
				(XKMSRegisterRequest *) (createRegisterRequest(prov, &doc, argc, argv, paramCount));

			if (r == NULL) {
				return -1;
			}
			if (twoPhase)
				r->appendResponseMechanismItem(XKMSConstants::s_tagRepresent);

			msg = r;
			parmsDone = true;

		}
		else if ((_stricmp(argv[paramCount], "RevokeRequest") == 0) ||
			(_stricmp(argv[paramCount], "er") == 0)) {

			paramCount++;
			XKMSRevokeRequest * r = 
				(XKMSRevokeRequest *) (createRevokeRequest(prov, &doc, argc, argv, paramCount));

			if (r == NULL) {
				return -1;
			}
			if (twoPhase)
				r->appendResponseMechanismItem(XKMSConstants::s_tagRepresent);

			msg = r;
			parmsDone = true;

		}		
		else if ((_stricmp(argv[paramCount], "RecoverRequest") == 0) ||
			(_stricmp(argv[paramCount], "or") == 0)) {

			paramCount++;
			XKMSRecoverRequest * r = 
				(XKMSRecoverRequest *) (createRecoverRequest(prov, &doc, argc, argv, paramCount));

			if (r == NULL) {
				return -1;
			}
			if (twoPhase)
				r->appendResponseMechanismItem(XKMSConstants::s_tagRepresent);

			msg = r;
			parmsDone = true;

		}
		else if ((_stricmp(argv[paramCount], "ReissueRequest") == 0) ||
			(_stricmp(argv[paramCount], "ir") == 0)) {

			paramCount++;
			XKMSReissueRequest * r = 
				(XKMSReissueRequest *) (createReissueRequest(prov, &doc, argc, argv, paramCount));

			if (r == NULL) {
				return -1;
			}
			if (twoPhase)
				r->appendResponseMechanismItem(XKMSConstants::s_tagRepresent);

			msg = r;
			parmsDone = true;

		}
		else if ((_stricmp(argv[paramCount], "PendingRequest") == 0) ||
			(_stricmp(argv[paramCount], "pr") == 0)) {

			paramCount++;
			XKMSPendingRequest * r = 
				(XKMSPendingRequest *) (createPendingRequest(prov, &doc, argc, argv, paramCount));

			if (r == NULL) {
				return -1;
			}
			if (twoPhase)
				r->appendResponseMechanismItem(XKMSConstants::s_tagRepresent);

			msg = r;
			parmsDone = true;

		}
		else if ((_stricmp(argv[paramCount], "StatusRequest") == 0) ||
			(_stricmp(argv[paramCount], "sr") == 0)) {

			paramCount++;
			XKMSStatusRequest * r = 
				(XKMSStatusRequest *) (createStatusRequest(prov, &doc, argc, argv, paramCount));

			if (r == NULL) {
				return -1;
			}
			if (twoPhase)
				r->appendResponseMechanismItem(XKMSConstants::s_tagRepresent);

			msg = r;
			parmsDone = true;

		}
		else if ((_stricmp(argv[paramCount], "CompoundRequest") == 0) ||
			(_stricmp(argv[paramCount], "cr") == 0)) {

			XKMSCompoundRequest * r = 
				(XKMSCompoundRequest *) (createCompoundRequest(prov, &doc, argc, argv, paramCount + 1));

			if (r == NULL) {
				return -1;
			}
			if (twoPhase)
				r->appendResponseMechanismItem(XKMSConstants::s_tagRepresent);

			msg = r;
			parmsDone = true;

		}
		else {

			printDoRequestUsage();
			return -1;

		}
	}

	XKMSMessageFactory * f = prov.getXKMSMessageFactory();
	XKMSRequestAbstractType * request = f->toRequestAbstractType(msg);

	if (nonce != NULL)
		request->setNonce(MAKE_UNICODE_STRING(nonce));
	if (originalRequestId != NULL)
		request->setOriginalRequestId(MAKE_UNICODE_STRING(originalRequestId));

	try {
		if (g_txtOut) {
			outputDoc(doc);
		}
	}
	catch (...) {
		delete msg;
		doc->release();
		throw;
	}

	DOMDocument * responseDoc;

	try {
		XSECSOAPRequestorSimple req(msg->getService());
#if !defined(_WIN32)
		struct timeval tv1, tv2;
		gettimeofday(&tv1, NULL);
#endif
		req.setEnvelopeType(et);

		responseDoc = req.doRequest(doc);
#if !defined(_WIN32)
		gettimeofday(&tv2, NULL);
		long seconds = tv2.tv_sec - tv1.tv_sec;
		long useconds;
		if (seconds != 0) {
			useconds = 1000000 - tv1.tv_usec + tv2.tv_usec;
			seconds--;
		}
		else {
			useconds = tv2.tv_usec - tv1.tv_usec;
		}
		if (useconds >= 1000000) {
			useconds -= 1000000;
			seconds++;
		}

		cout << "Time taken for request = " << seconds << " seconds, " << useconds << " useconds" << endl;
#endif
		/* If two-phase - re-do the request */
		if (twoPhase) {

			XKMSResultType * r = f->toResultType(f->newMessageFromDOM(responseDoc->getDocumentElement()));
			if (r->getResultMajor() == XKMSResultType::Represent) {

				cerr << "Intermediate response of a two phase sequence received\n\n";

				if (g_txtOut) {
					outputDoc(responseDoc);
				}
				doParsedMsgDump(responseDoc);

				//XKMSRequestAbstractType * request = f->toRequestAbstractType(msg);
				for (int k = 0; k < request->getResponseMechanismSize(); ++k) {
					if (strEquals(request->getResponseMechanismItemStr(k),
								  XKMSConstants::s_tagRepresent)) {
						request->removeResponseMechanismItem(k);
						break;
					}
				}

				request->setNonce(r->getNonce());
				request->setOriginalRequestId(request->getId());
				XMLCh * myId = generateId();

				request->setId(myId);
				XSEC_RELEASE_XMLCH(myId);

				responseDoc->release();
				responseDoc = req.doRequest(doc);

			}
			delete r;
		}

	}
	catch (XSECException &e) {

		char * m = XMLString::transcode(e.getMsg());
		cerr << "Error sending request: " << m << endl;
		XSEC_RELEASE_XMLCH(m);

		delete msg;
		doc->release();

		return -1;

	}
	catch (...) {
		delete msg;
		doc->release();

		throw;

	}

	// Cleanup request stuff
	delete msg;
	doc->release();

	// Now lets process the result

	int ret;
	
	try {
		if (g_txtOut) {
			outputDoc(responseDoc);
		}
		ret = doParsedMsgDump(responseDoc);
	}
	catch (...) {
		responseDoc->release();
		throw;
	}
	
	responseDoc->release();
	return ret;

}


// --------------------------------------------------------------------------------
//           Base msgdump module
// --------------------------------------------------------------------------------


#if 0
class XMLSchemaDTDResolver : public EntityResolver { 

public: 

	XMLSchemaDTDResolver() {}
	~XMLSchemaDTDResolver() {}
	
	InputSource * resolveEntity (const XMLCh* const publicId, const XMLCh* const systemId);
	
};

InputSource * XMLSchemaDTDResolver::resolveEntity (const XMLCh* const publicId, 
												   const XMLCh* const systemId) { 
	

	
	if (strEquals(systemId, "http://www.w3.org/2001/XMLSchema")) { 	
		return new LocalFileInputSource(MAKE_UNICODE_STRING("C:\\prog\\SRC\\xml-security\\c\\Build\\Win32\\VC6\\Debug\\XMLSchema.dtd")); 
	} 
	else { 
		return NULL; 
	}

}
#endif
void printMsgDumpUsage(void) {

	cerr << "\nUsage msgdump [options] <filename>\n";
	cerr << "   --help/-h      : print this screen and exit\n";
	cerr << "   --validate/-v  : validate the input messages\n";
	cerr << "   --auth-phrase/-a <phrase>\n";
	cerr << "                  : use <phrase> for authentication/private key in X-KRSS messages\n";
#if defined (XSEC_HAVE_OPENSSL)
	cerr << "   --output-private-key/-p <filename> <passphrase>\n";
	cerr << "                  : Write private keys from Register or Recover requests to the file\n\n";
#endif
    cerr << "   filename = name of file containing XKMS msg to dump\n\n";

}

int doMsgDump(int argc, char ** argv, int paramCount) {

	char * inputFile = NULL;
	bool doValidate = false;

	if (paramCount >= argc || 
		(_stricmp(argv[paramCount], "--help") == 0) ||
		(_stricmp(argv[paramCount], "-h") == 0)) {
		printMsgDumpUsage();
		return -1;
	}

	while (paramCount < argc-1) {
		if ((_stricmp(argv[paramCount], "--validate") == 0) ||
			(_stricmp(argv[paramCount], "-v") == 0)) {

			doValidate = true;
			paramCount++;

		}
		else if ((_stricmp(argv[paramCount], "--auth-phrase") == 0) ||
			(_stricmp(argv[paramCount], "-a") == 0)) {

			paramCount++;
			g_authPassPhrase = argv[paramCount];
			paramCount++;
		}
#if defined (XSEC_HAVE_OPENSSL)
		else if (_stricmp(argv[paramCount], "--output-private-key") == 0 || _stricmp(argv[paramCount], "-p") == 0) {
			if (paramCount >= argc + 2) {
				printMsgDumpUsage();
				return -1;
			}
			++paramCount;
			g_privateKeyFile = argv[paramCount++];
			g_privateKeyPassPhrase = argv[paramCount++];
		}
#endif
		else {

			printMsgDumpUsage();
			return -1;
		}
	}

	if (paramCount >= argc) {
		printMsgDumpUsage();
		return -1;
	}

	inputFile = argv[paramCount];

	// Dump the details of an XKMS message to the console
	cout << "Decoding XKMS Message contained in " << inputFile << endl;
	
	// Create and set up the parser

	XercesDOMParser * parser = new XercesDOMParser;
	Janitor<XercesDOMParser> j_parser(parser);

	parser->setDoNamespaces(true);
	parser->setCreateEntityReferenceNodes(true);

	// Error handling
	xkmsErrorHandler xeh;
	parser->setErrorHandler(&xeh);

#if 0
	// Local load of XMLSchema.dtd
	XMLSchemaDTDResolver sdr;
	parser->setEntityResolver(&sdr);
#endif

	// Schema handling
	if (doValidate) {
		parser->setDoSchema(true);
		parser->setExternalSchemaLocation("http://www.w3.org/2002/03/xkms# http://www.w3.org/TR/xkms2/Schemas/xkms.xsd");
	}

	bool errorsOccured = false;
	xsecsize_t errorCount = 0;
    try
    {
    	parser->parse(inputFile);
        errorCount = parser->getErrorCount();
    }

    catch (const XMLException& e)
    {
		char * msg = XMLString::transcode(e.getMessage());
        cerr << "An error occured during parsing\n   Message: "
             << msg << endl;
		XSEC_RELEASE_XMLCH(msg);
        errorsOccured = true;
    }


    catch (const DOMException& e)
    {
       cerr << "A DOM error occured during parsing\n   DOMException code: "
             << e.code << endl;
        errorsOccured = true;
    }

	if (errorCount > 0 || errorsOccured) {

		cout << "Errors during parse" << endl;
		return (2);

	}

	/*

		Now that we have the parsed file, get the DOM document and start looking at it

	*/
	
	DOMDocument *doc = parser->getDocument();

	return doParsedMsgDump(doc);
}


// --------------------------------------------------------------------------------
//           Startup and main
// --------------------------------------------------------------------------------

void printUsage(void) {

	cerr << "\nUsage: xklient [base options] {msgdump|msgcreate|dorequest} [command specific options]\n\n";
	cerr << "     msgdump   : Read an XKMS message and print details\n";
	cerr << "     msgcreate : Create a message of type :\n";
	cerr << "                 LocateRequest\n";
	cerr << "                 ValidateRequest\n";
	cerr << "                 PendingRequest\n";
	cerr << "                 RegisterRequest\n";
	cerr << "                 RecoverRequest\n";
	cerr << "                 ReissueRequest\n";
	cerr << "                 RevokeRequest\n";
	cerr << "                 PendingRequest\n";
	cerr << "                 send to service URI and output result\n\n";
	cerr << "     Where options are :\n\n";
	cerr << "     --text/-t\n";
	cerr << "         Print any created XML to screen\n";

}

int evaluate(int argc, char ** argv) {
	
	if (argc < 2) {

		printUsage();
		return 2;
	}

	int paramCount = 1;

	// Run through parameters

	while (paramCount < argc) {

		if (_stricmp(argv[paramCount], "--text") == 0 || _stricmp(argv[paramCount], "-t") == 0) {
			g_txtOut = true;
			paramCount++;
		}
		else if (_stricmp(argv[paramCount], "MsgDump") == 0 || _stricmp(argv[paramCount], "md") == 0) {
			
			// Perform a MsgDump operation
			return doMsgDump(argc, argv, paramCount +1);

		}
		else if (_stricmp(argv[paramCount], "MsgCreate") == 0 || _stricmp(argv[paramCount], "mc") == 0) {
			
			// Perform a MsgDump operation
			return doMsgCreate(argc, argv, paramCount +1);

		}
		else if (_stricmp(argv[paramCount], "Request") == 0 || _stricmp(argv[paramCount], "req") == 0) {
			
			// Perform a MsgDump operation
			return doRequest(argc, argv, paramCount +1);

		}
		else {
			printUsage();
			return 2;
		}
	}

	return 1;

}
	


int main(int argc, char **argv) {

	int retResult;

	/* We output a version number to overcome a "feature" in Microsoft's memory
	   leak detection */

	cout << "XKMS Client (Using Apache XML-Security-C Library v" << XSEC_VERSION_MAJOR <<
		"." << XSEC_VERSION_MEDIUM << "." << XSEC_VERSION_MINOR << ")\n";

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

	if ( _CrtMemDifference( &s3, &s1, &s2 ) && s3.lCounts[1] > 1) {

		std::cerr << "Total count = " << (unsigned int) s3.lTotalCount << endl;

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
