/**
 *
 * XAdES signature generation for PTEid Middleware
 *
 * Author: Andre Guerreiro <andre.guerreiro@caixamagica.pt>
 *
 */

#include <iostream>
#include <fstream>
#include <cstdio>

#include "APLCard.h"
#include "CardPteidDef.h"
#include "XadesSignature.h"

//For SHA-1
#include "cryptoFramework.h"
#include "Log.h"

#include "ByteArray.h"

// Xerces

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>

// XML-Security-C (XSEC)

#include <xsec/framework/XSECProvider.hpp>
#include <xsec/dsig/DSIGReference.hpp>
#include <xsec/dsig/DSIGObject.hpp>
#include <xsec/enc/OpenSSL/OpenSSLCryptoKeyHMAC.hpp>
#include <xsec/enc/OpenSSL/OpenSSLCryptoX509.hpp>
#include <xsec/enc/XSECCryptoUtils.hpp>
#include <xsec/enc/XSECCryptoException.hpp>
#include <xsec/framework/XSECException.hpp>
#include <xsec/utils/XSECBinTXFMInputStream.hpp>
#include <xsec/dsig/DSIGConstants.hpp>
#include <xsec/dsig/DSIGKeyInfoX509.hpp>
#include <xsec/transformers/TXFMBase.hpp>
#include <xsec/transformers/TXFMChain.hpp>

//TODO: Replace all console output with MWLOG calls
//stat
#include <sys/types.h>
#include <sys/stat.h>
//
#ifdef WIN32
#include <io.h>
#endif
//
//Solve stupid differences in Windows standards compliance
#ifndef WIN32
#define _stat stat
#define _read read
#define _fileno fileno
#endif

XERCES_CPP_NAMESPACE_USE

namespace eIDMW
{

	CByteArray XadesSignature::HashFile(const char *file_path)
	{

		std::ifstream file(file_path, std::ios::binary|std::ios::ate);
		char * in; 

		std::ifstream::pos_type size = -1; 
		if (file.is_open())
		{   
			size = file.tellg();
			in = new char [size];
			file.seekg (0, std::ios::beg);
			file.read(in, size);
		}   
		file.close();

		//Special-case empty files
		if (size == 0)
		{   
			in = new char[20];

		}   
		//cryptoframework.h
		return CByteArray();
		//return mp_crypto->GetHashSha1(CByteArray(in, size));

	}
int XadesSignature::appendOID(XMLByte *toFill)
{

	int oidLen;
	unsigned char * oid = getRSASigOID(HASH_SHA1, oidLen);
	memcpy(toFill, oid, oidLen);
	return oidLen;

}


void XadesSignature::loadCert(CByteArray &data, EVP_PKEY *pub_key)
{
	FILE *fp;
	int ret = 0;
	char *in;
	unsigned char * data_cert = data.GetBytes();
	//OpenSSL call	
	mp_cert = d2i_X509(NULL, (const unsigned char**)(&data_cert), data.Size());

	if (mp_cert == NULL)
	{
		MWLOG(LEV_ERROR, MOD_APL, L"loadCert() Error decoding certificate data!");
	}

}

//TODO: Needs some refactoring and testing
std::string& XadesSignature::SignXades(char ** List_of_paths, unsigned int n_paths)
{

	try {
		XMLPlatformUtils::Initialize();
		XSECPlatformUtils::Initialise();
	}
	catch (const XMLException &e) {

		cerr << "Error during initialisation of Xerces" << endl;
		cerr << "Error Message = : "
		     << e.getMessage() << endl;

	}

	XSECProvider prov;
	DSIGSignature *sig;
	DOMElement *sigNode;
	int oidlen;
	OpenSSLCryptoX509 *ssl_cert = NULL;
	DSIGKeyInfoX509 * keyInfoX509 = NULL;
	EVP_PKEY *pub_key = NULL;

	CByteArray *sha1_hash;
	unsigned char * rsa_signature;
	XMLByte toFill[35 * sizeof(XMLByte)]; //SHA-1 Hash prepended with Algorithm ID as by PKCS#1 standard

	const std::string &reference_uri = string("file://localhost") + List_of_paths[0];

	XMLCh * uni_reference_uri = XMLString::transcode(reference_uri.c_str());
	//XMLCh * uni_reference_uri = XMLString::transcode("#hello");

        DOMImplementation *impl = 
		DOMImplementationRegistry::getDOMImplementation(MAKE_UNICODE_STRING("Core"));
	
	DOMDocument *doc = impl->createDocument(
                0,
                MAKE_UNICODE_STRING("Document"),
                NULL);
	DOMElement *rootElem = doc->getDocumentElement();


	memset(toFill, 0x2f, 20);
	oidlen = appendOID(toFill);

	try {
		
		// Create a signature object

		sig = prov.newSignature();
		//sig->setDSIGNSPrefix(MAKE_UNICODE_STRING("ds"));

		// Use it to create a blank signature DOM structure from the doc

		sigNode = sig->createBlankSignature(doc, CANON_C14N_COM, SIGNATURE_RSA, HASH_SHA1);

		// Inser the signature DOM nodes into the doc

		rootElem->appendChild(doc->createTextNode(MAKE_UNICODE_STRING("\n")));
		rootElem->appendChild(sigNode);
		rootElem->appendChild(doc->createTextNode(MAKE_UNICODE_STRING("\n")));

		//Create a reference to the external file
		DSIGReference * ref = sig->createReference(uni_reference_uri);

		//TODO: iterate over arguments, eventually
		*sha1_hash = HashFile(List_of_paths[0]);
		//cerr << "DEBUG: Hash of external file: " << endl;
		//printByteArray(cerr, sha1_hash, 20);

		if (sha1_hash != NULL)
			ref->setExternalHash(sha1_hash->GetBytes());
		try
		{
			//This is a somewhat hackish way of getting the canonicalized hash
			//of the reference
			sig->calculateSignedInfoHash(&toFill[oidlen], 20);
			//ref->makeBinInputStream()->readBytes(toFill, 20); 	
		}
		catch (const XMLException &e)
		{
			cerr << "Exception in calculateSignedInfoHash(), message: " <<
				XMLString::transcode(e.getType()) <<  " - " 
				  << XMLString::transcode(e.getMessage()) << endl;
		
		}

		//cerr << "DEBUG: SignedInfoHash: " << endl;
		//printByteArray (cerr, toFill , 35);
		
		//TODO: Load Certificate from the right applayer object
		CByteArray certData;
	       	mp_card.readFile(PTEID_FILE_CERT_SIGNATURE, certData);
		loadCert (certData, pub_key);

		//Access pteid card to sign the XML Data
		//This code will be eventually integrated in applayer
		// so this connection to the card will be completely different
		//XXX: We should make the signing key configurable,
		//for now it is the Authentication key

		// Create KeyInfo element

		keyInfoX509 = sig->appendX509Data();

		ssl_cert = new OpenSSLCryptoX509(mp_cert);

		keyInfoX509->appendX509Certificate(ssl_cert->getDEREncodingSB().sbStrToXMLCh());
		
		try
		{
			CByteArray rsa_signature = mp_card.Sign(CByteArray(toFill, 20+oidlen));
		}
		catch(...)
		{
			MWLOG(LEV_ERROR, MOD_APL, L"APLCard::Sign() failed, can't generate XADES signature");
			throw;
		}


		sig->signExternal((XMLByte *)rsa_signature, 128); //RSA Signature with modlength=1024 bits

		//addTimestampNode(sig);
		
		
	}
	catch (XSECCryptoException &e)
	{

		cerr << "An error occurred in XSec Crypto Functions.\n   Message: "
		<< e.getMsg() << endl;
		exit(1);

	}
	catch (XSECException &e)
	{
		cerr << "An error occured during a signature load.\n   Message: "
		<< e.getMsg() << endl;
		exit(1);
		
	}

	//WriteToByteArray


}


}
