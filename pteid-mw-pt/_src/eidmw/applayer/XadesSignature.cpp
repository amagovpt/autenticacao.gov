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
#include "MWException.h"
#include "eidErrors.h"

#include "Log.h"
#include "ByteArray.h"

// Xerces
#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/framework/MemBufFormatTarget.hpp>

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

#include <curl/curl.h>

#include <openssl/sha.h>

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

#define SHA1_LEN 20
#define PTEID_SIGNATURE_LENGTH 128

XERCES_CPP_NAMESPACE_USE

namespace eIDMW
{

	CByteArray XadesSignature::HashFile(const char *file_path)
	{

		std::ifstream file(file_path, std::ios::binary|std::ios::ate);
		char * in; 
		unsigned char out[SHA1_LEN];

		std::ifstream::pos_type size = -1; 
		if (file.is_open())
		{   
			size = file.tellg();
			in = new char [size];
			file.seekg (0, std::ios::beg);
			file.read (in, size);
		}
		file.close();

		//Special-case empty files
		if (size == 0)
		{   
			in = new char[20];

		}
		//cryptoframework.h
		SHA1 ((unsigned char *)in, size, out);
		return CByteArray((const unsigned char*)out, 20L);
		//return mp_crypto->GetHashSha1(CByteArray(in, size));

	}
	int XadesSignature::appendOID(XMLByte *toFill)
	{

		int oidLen;
		unsigned char * oid = getRSASigOID(HASH_SHA1, oidLen);
		memcpy(toFill, oid, oidLen);
		return oidLen;

	}

	/** Timestamping stuff */
	size_t XadesSignature::curl_write_data(char *ptr, size_t size, size_t nmemb, void * stream)
	{
		mp_timestamp_data.Append((const unsigned char*)ptr, (unsigned long)size*nmemb);


	}
	
	void XadesSignature::generate_asn1_request_struct(char *sha_1)
	{

		for (unsigned int i=0; i != 20; i++)
		    timestamp_asn1_request[SHA1_OFFSET +i] = sha_1[i];
	}

	CByteArray XadesSignature::timestamp_data(const unsigned char *input, unsigned int data_len)
	{

		CURL *curl;
		CURLcode res;
		char error_buf[CURL_ERROR_SIZE];

		curl = curl_easy_init();

		if (curl) 
		{

			struct curl_slist *headers= NULL;

			headers = curl_slist_append(headers, "Content-Type: application/timestamp-request");
			headers = curl_slist_append(headers, "Content-Transfer-Encoding: binary");
			headers = curl_slist_append(headers, "User-Agent: PTeID Middleware v2");

			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

			curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, data_len);

			curl_easy_setopt(curl, CURLOPT_URL, TIMESTAMPING_HOST);

			/* Now specify the POST data */ 
			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, input);

			curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, error_buf);

			/*TODO: Using WRITEDATA to pass a fp to write response data breaks on Win32
			 *      according to curl documentation
			 */
			//curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp_out);
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &XadesSignature::curl_write_data);

			/* Perform the request, res will get the return code */ 
			res = curl_easy_perform(curl);

			if (res != 0)
			{
				MWLOG(LEV_ERROR, MOD_APL, L"Timestamping error in HTTP POST request. LibcURL returned %s\n", 
						(char *)error_buf);
				throw CMWEXCEPTION(EIDMW_ERR_CHECK);
			}

			curl_slist_free_all(headers);

			/* always cleanup */ 
			curl_easy_cleanup(curl);

		}

		return CByteArray(mp_timestamp_data);
	}

	CByteArray *XadesSignature::WriteToByteArray(DOMDocument * doc)
	{
		CByteArray * ba_out = new CByteArray();
		XMLCh tempStr[3] = {chLatin_L, chLatin_S, chNull};
		DOMImplementation *impl = 
			            DOMImplementationRegistry::getDOMImplementation(tempStr); 

		// construct the DOMWriter
	        DOMLSSerializer   *theSerializer = ((DOMImplementationLS*)impl)->createLSSerializer();
		DOMLSOutput       *theOutputDesc = ((DOMImplementationLS*)impl)->createLSOutput();


		 // construct the MemBufFormatTarget
		 MemBufFormatTarget * myFormatTarget = new MemBufFormatTarget();
		 theOutputDesc->setByteStream(myFormatTarget);
		 theOutputDesc->setEncoding(XMLString::transcode("UTF-8"));
		 theSerializer->write(doc, theOutputDesc);

		// serialize the DOMNode to a UTF-8 string
		 const XMLByte* utf8_string = myFormatTarget->getRawBuffer();
		 XMLSize_t size = myFormatTarget->getLen();
		 MWLOG(LEV_DEBUG, MOD_APL, L"XadesSignature::WriteToByteArray: Returning XML byte array, size=%d", size);

		 ba_out->Append((const unsigned char *)utf8_string, size);
		 return ba_out;

	}


void XadesSignature::loadCert(CByteArray &data, EVP_PKEY *pub_key)
{

	unsigned char * data_cert = data.GetBytes();
	//OpenSSL call	
	mp_cert = d2i_X509(NULL, (const unsigned char**)(&data_cert), data.Size());

	if (mp_cert == NULL)
	{
		MWLOG(LEV_ERROR, MOD_APL, L"loadCert() Error decoding certificate data!");
	}

}

//TODO: Needs some refactoring and testing
CByteArray &XadesSignature::SignXades(const char * path, unsigned int n_paths)
{

	try {
		XMLPlatformUtils::Initialize();
		XSECPlatformUtils::Initialise();
	}
	catch (const XMLException &e) {

		MWLOG(LEV_ERROR, MOD_APL, L"Error during initialisation of Xerces. Error Message: %s",
		     e.getMessage()) ;

	}

	XSECProvider prov;
	DSIGSignature *sig;
	DOMElement *sigNode;
	int oidlen;
	OpenSSLCryptoX509 *ssl_cert = NULL;
	DSIGKeyInfoX509 * keyInfoX509 = NULL;
	EVP_PKEY *pub_key = NULL;

	CByteArray sha1_hash;
	CByteArray rsa_signature;
	XMLByte toFill[35 * sizeof(XMLByte)]; //SHA-1 Hash prepended with Algorithm ID as by PKCS#1 standard

	const std::string &reference_uri = string("file://localhost") + path;

	XMLCh * uni_reference_uri = XMLString::transcode(reference_uri.c_str());
	//XMLCh * uni_reference_uri = XMLString::transcode("#hello");

        DOMImplementation *impl = 
		DOMImplementationRegistry::getDOMImplementation(MAKE_UNICODE_STRING("Core"));
	
	DOMDocument *doc = impl->createDocument(0, MAKE_UNICODE_STRING("Document"), NULL);
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
		sha1_hash = HashFile(path);
		//cerr << "DEBUG: Hash of external file: " << endl;
		//printByteArray(cerr, sha1_hash, 20);

		ref->setExternalHash(sha1_hash.GetBytes());
		try
		{
			//This is a somewhat hackish way of getting the canonicalized hash
			//of the reference
			sig->calculateSignedInfoHash(&toFill[oidlen], 20);
			//ref->makeBinInputStream()->readBytes(toFill, 20); 	
		}
		catch (const XMLException &e)
		{
			MWLOG(LEV_ERROR, MOD_APL, L"Exception in calculateSignedInfoHash(), message: %s - %s", 
				XMLString::transcode(e.getType()), XMLString::transcode(e.getMessage()));
		
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
			rsa_signature = mp_card.Sign(CByteArray(toFill, SHA1_LEN + oidlen));
		}
		catch(...)
		{
			MWLOG(LEV_ERROR, MOD_APL, L"APLCard::Sign() failed, can't generate XADES signature");
			throw;
		}


		sig->signExternal((XMLByte *)(rsa_signature.GetBytes()), PTEID_SIGNATURE_LENGTH); //RSA Signature with modlength=1024 bits

		//addTimestampNode(sig);
		
	}
	catch (XSECCryptoException &e)
	{
		MWLOG(LEV_ERROR, MOD_APL, L"An error occurred in XSec Crypto Functions. Message: %s\n",
		 e.getMsg());

	}
	catch (XSECException &e)
	{
		MWLOG(LEV_ERROR, MOD_APL, L"An error occured during a signature load. Message: %s\n",
		 e.getMsg());;
		
	}

	return *WriteToByteArray(doc);

}


}
