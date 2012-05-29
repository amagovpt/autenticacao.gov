/**
******************************************************************************
*
**  PTeID Middleware Project.
**  Copyright (C) 2011-2012
**  Andre Guerreiro <andre.guerreiro@caixamagica.pt>
**
**  XAdES and XAdES-T signature generator and validator 
**
**
**
*/


#include <iostream>
#include <fstream>
#include <cstdio>

#include "APLCard.h"
#include "APLConfig.h"

#include "CardPteidDef.h"
#include "XadesSignature.h"
#include "MWException.h"
#include "eidErrors.h"
#include "Util.h"
#include "static_pteid_certs.h"

#include "MiscUtil.h"


#include "Log.h"
#include "ByteArray.h"

// Xerces
#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/framework/MemBufFormatTarget.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/XMLException.hpp>
#include <xercesc/util/XMLUri.hpp>
#include <xercesc/util/Janitor.hpp>

// XML-Security-C (XSEC)

#include <xsec/framework/XSECProvider.hpp>
#include <xsec/framework/XSECException.hpp>
#include <xsec/dsig/DSIGReference.hpp>
#include <xsec/dsig/DSIGObject.hpp>
#include <xsec/dsig/DSIGConstants.hpp>
#include <xsec/dsig/DSIGKeyInfoX509.hpp>
#include <xsec/enc/OpenSSL/OpenSSLCryptoKeyHMAC.hpp>
#include <xsec/enc/OpenSSL/OpenSSLCryptoX509.hpp>
#include <xsec/enc/XSECKeyInfoResolverDefault.hpp>
#include <xsec/enc/XSECCryptoUtils.hpp>
#include <xsec/enc/XSECCryptoException.hpp>
#include <xsec/utils/XSECBinTXFMInputStream.hpp>
#include <xsec/transformers/TXFMBase.hpp>
#include <xsec/transformers/TXFMChain.hpp>

//cURL for Timestamping
#include <curl/curl.h>

//OpenSSL
#include <openssl/sha.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/bio.h>
#include <openssl/err.h>

//stat
#include <sys/types.h>
#include <sys/stat.h>
#ifdef WIN32
#include <io.h>
#include <Shlwapi.h> //UrlCreateFromPath()
#endif

//Solve stupid differences in Windows standards compliance
#ifndef WIN32
#define _stat stat
#define _read read
#define _fileno fileno
#define _snprintf snprintf
#endif

#define SHA1_LEN 20
#define PTEID_SIGNATURE_LENGTH 128

XERCES_CPP_NAMESPACE_USE

namespace eIDMW
{

	CByteArray XadesSignature::mp_timestamp_data = CByteArray();

	CByteArray XadesSignature::HashFile(const char *file_path)
	{

		std::ifstream file(file_path, std::ios::binary|std::ios::ate);
		char * in; 
		unsigned char out[SHA1_LEN];

		int size = -1; 
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
			in = new char[SHA1_LEN];

		}
		//OpenSSL call
		SHA1 ((unsigned char *)in, size, out);
		return CByteArray((const unsigned char*)out, 20L);

	}
	
#ifdef WIN32

	std::wstring utf8_decode(const char *str)
	{
		int size_needed = MultiByteToWideChar(CP_UTF8, 0, str, strlen(str), NULL, 0);
		std::wstring wstrTo(size_needed, 0);
		MultiByteToWideChar(CP_UTF8, 0, str, strlen(str), &wstrTo[0], size_needed);
		return wstrTo;
	}

	const wchar_t * pathToURI(const std::wstring path)
	{
		DWORD url_size = MAX_PATH*5;
		LPWSTR pszUrl = new WCHAR[url_size]; //We have to account for chars that must be escaped into %XX sequences

		HRESULT ret = UrlCreateFromPath(path.c_str(), pszUrl, &url_size, NULL);
       
		if (ret != S_OK)
		{
	    	MWLOG(LEV_ERROR, MOD_APL, L"XadesSignature: UrlCreateFromPath returned error, \
			URI is probably wrongly encoded");
			return std::wstring(L"file://localhost" + path).c_str();
		}
		
		return pszUrl;

	}
#endif

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
		size_t realsize = size * nmemb;
		mp_timestamp_data.Append((const unsigned char*)ptr, realsize);

		return realsize;

	}
	


	void XadesSignature::generate_asn1_request_struct(unsigned char *sha_1)
	{

		for (unsigned int i=0; i != SHA1_LEN; i++)
		    timestamp_asn1_request[SHA1_OFFSET +i] = sha_1[i];
	}

	std::string XadesSignature::getTS_CAPath()
	{
		APL_Config certs_dir(CConfig::EIDMW_CONFIG_PARAM_GENERAL_CERTS_DIR);
		std::string m_certs_dir = certs_dir.getString();
		return m_certs_dir + "tsa_chain.pem";
	}

	static XMLCh s_Id[] = {

	chLatin_I,
	chLatin_d,
	chNull
        };

	/*Add the following elements to the signature to be compliant with XaDES spec
		 QualifyingProperties -> UnsignedProperties -> UnsignedSignatureProperties 
		   -> SignatureTimeStamp -> EncapsulatedTimeStamp

		   TODO: Add a reference as target of the SignedProperties Element
		    <SignedProperties Target="#SignatureWithSignedAndUnsignedProperties">
			...
			 <ds:Reference URI="#SignedProperties "
			Type=http://uri.etsi.org/01903/v1.1.1#SignedProperties">
			   <ds:DigestMethod Algorithm="http://www.w3.org/2000/09/xmldsig#sha1"/>
			<ds:DigestValue>...</ds:DigestValue> 
			</ds:Reference> 

	*/
	//Returns the newly created timestamp node
	DOMNode *XadesSignature::addSignatureProperties(DSIGSignature *sig)
	{
		XMLCh *prefix = XMLString::transcode("etsi");
		XMLCh *xades_namespace = XMLString::transcode("http://uri.etsi.org/01903/v1.1.1#");
		safeBuffer str; 
		
		XERCES_NS DOMDocument *doc = sig->getParentDocument();
		DSIGObject * obj1 = sig->appendObject();
		
		obj1->setId(MAKE_UNICODE_STRING("Signature_PteID_Timestamp"));
	
		makeQName(str, prefix, "QualifyingProperties");
		DOMNode * n1 = doc->createElementNS(xades_namespace, str.rawXMLChBuffer());
		n1->appendChild(doc->createTextNode(DSIGConstants::s_unicodeStrNL)); //Pretty print
		makeQName(str, prefix, "UnsignedProperties");
		DOMNode * n2 = doc->createElementNS(xades_namespace, str.rawXMLChBuffer());
		n2->appendChild(doc->createTextNode(DSIGConstants::s_unicodeStrNL)); //Pretty print
		makeQName(str, prefix, "UnsignedSignatureProperties");
		DOMNode * n3 = doc->createElementNS(xades_namespace, str.rawXMLChBuffer());
		n3->appendChild(doc->createTextNode(DSIGConstants::s_unicodeStrNL)); //Pretty print
		makeQName(str, prefix, "SignedProperties");
		DOMNode * n4 = doc->createElementNS(xades_namespace, str.rawXMLChBuffer());
		((DOMElement *)n4)->setAttributeNS(NULL, s_Id, XMLString::transcode("SignedProperties"));
		n4->appendChild(doc->createTextNode(DSIGConstants::s_unicodeStrNL)); //Pretty print
		makeQName(str, prefix, "SignatureTimeStamp");
		DOMNode * n5 = doc->createElementNS(xades_namespace, str.rawXMLChBuffer());
		makeQName(str, prefix, "EncapsulatedTimeStamp");
		DOMNode * n6 = doc->createElementNS(xades_namespace, str.rawXMLChBuffer());
		n1->appendChild(n2);
		n1->appendChild(n4);
		n2->appendChild(n3);
		n3->appendChild(n5);
		n5->appendChild(n6);
	
		obj1->appendChild(n1);

		return n6;

	}
	
	/*
	 * timestamp parameter is the base64-encoded string version of the timestamp
	 *
	 */
	void XadesSignature::addTimestampNode (XERCES_NS DOMNode *node, unsigned char *timestamp)
	{
		XERCES_NS DOMDocument *doc = node->getOwnerDocument();

		node->appendChild(doc->createTextNode(
					XMLString::transcode((const char*)timestamp)));

	}

	unsigned char* base64Encode(const unsigned char *array, int len)
	{
		EVP_ENCODE_CTX ectx;
		unsigned char* out = new unsigned char[len*2];
		int outlen = 0;

		EVP_EncodeInit( &ectx );
		EVP_EncodeUpdate( &ectx, out, &outlen, array, len );
		//This null-terminates the out string
		EVP_EncodeFinal( &ectx, out+outlen, &outlen );

		return out;

	}

	void base64Decode(const char *array, unsigned int inlen, unsigned char *&decoded, unsigned int &decoded_len)
	{

		BIO *bio, *b64;
		unsigned int outlen = 0;
		unsigned char *inbuf = new unsigned char[512];
		memset(inbuf, 0, 512);

		b64 = BIO_new(BIO_f_base64());
		BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);

		bio = BIO_new_mem_buf((void *)array, inlen);
		bio = BIO_push(b64, bio);

		outlen = BIO_read(bio, inbuf, 512);
		decoded = inbuf;

		decoded_len = outlen;

		BIO_free_all(bio);
	}




	void XadesSignature::timestamp_data(const unsigned char *input, unsigned int data_len)
	{

		CURL *curl;
		CURLcode res;
		char error_buf[CURL_ERROR_SIZE];

		//Make sure the static array receiving the network reply 
		// is zero'd out before each request
		mp_timestamp_data.Chop(mp_timestamp_data.Size());

		//Get Timestamping server URL from config
		APL_Config tsa_url(CConfig::EIDMW_CONFIG_PARAM_XSIGN_TSAURL);
		const char * TSA_URL = tsa_url.getString();

		curl_global_init(CURL_GLOBAL_ALL);

		curl = curl_easy_init();

		if (curl) 
		{

			struct curl_slist *headers= NULL;

			headers = curl_slist_append(headers, "Content-Type: application/timestamp-request");
			headers = curl_slist_append(headers, "Content-Transfer-Encoding: binary");
			headers = curl_slist_append(headers, "User-Agent: PTeID Middleware v2");

			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

			curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, data_len);

			curl_easy_setopt(curl, CURLOPT_URL, TSA_URL);

			curl_easy_setopt(curl, CURLOPT_TIMEOUT, 15L);

			/* Now specify the POST data */ 
			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, input);

			curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, error_buf);

			//curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp_out);
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &XadesSignature::curl_write_data);

			/* Perform the request, res will get the return code */ 
			res = curl_easy_perform(curl);

			if (res != 0)
			{
				MWLOG(LEV_ERROR, MOD_APL, L"Timestamping error in HTTP POST request. LibcURL returned %ls\n", 
						utilStringWiden(string(error_buf)).c_str());
			}

			curl_slist_free_all(headers);

			/* always cleanup */ 
			curl_easy_cleanup(curl);
			curl_global_cleanup();

		}

	}

	CByteArray *XadesSignature::WriteToByteArray(XERCES_NS DOMDocument * doc)
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
void XadesSignature::initXerces()
{
	try {

		XMLPlatformUtils::Initialize();
		XSECPlatformUtils::Initialise();

	}
	catch (const XMLException &e) {

		MWLOG(LEV_ERROR, MOD_APL, L"Error during initialisation of Xerces. Error Message: %s",
		     e.getMessage()) ;
		throw CMWEXCEPTION(EIDMW_XERCES_INIT_ERROR);
	}
}


X509 * loadCertFromB64Der(const char *base64_string)
{
	X509 *cert = NULL;
	unsigned int bufsize = strlen(base64_string);

	unsigned char *der_buffer = (unsigned char *)malloc(bufsize);

	base64Decode(base64_string, bufsize, der_buffer, bufsize);

	if (der_buffer != NULL)
	{
		cert = d2i_X509(NULL, (const unsigned char**)(&der_buffer), bufsize);
		
		if (cert == NULL)
			MWLOG(LEV_ERROR, MOD_APL, L"Error parsing certificate from DER buffer!\n");

	}
	return cert;

}

void XadesSignature::foundCertificate (const char *SubDir, const char *File, void *param)
{
	X509_STORE *certificate_store =  (X509_STORE *)param;
	APL_Config certs_dir(CConfig::EIDMW_CONFIG_PARAM_GENERAL_CERTS_DIR);
	string path = string(certs_dir.getString());
	FILE *m_stream;
	X509 *pCert = NULL;
	long int bufsize;
	unsigned char *buf;

#ifdef WIN32
	errno_t werr;
	path += "\\";
#endif
	path+=SubDir;
#ifdef WIN32
	path += "\\";
#else
	path+= "/";
#endif
	path+=File;
	
#ifdef WIN32
	if ((werr = fopen_s(&m_stream, path.c_str(), "rb")) != 0)
		goto err;
#else
	if ((m_stream = fopen(path.c_str(), "rb")) == NULL)
		goto err;
#endif

	if (fseek( m_stream, 0L, SEEK_END))
		goto err;

	bufsize = ftell(m_stream);
	buf = (unsigned char *) malloc(bufsize*sizeof(unsigned char));

	if (fseek(m_stream, 0L, SEEK_SET)){
		free(buf);
		goto err;
	}

	if (fread(buf, sizeof( unsigned char ), bufsize, m_stream) != bufsize)
		goto err;

	pCert = d2i_X509(&pCert, (const unsigned char **)&buf, bufsize);
	if (pCert == NULL)
	   goto err;
	
	if(X509_STORE_add_cert(certificate_store, pCert) == 0)
	   goto err;
	
	MWLOG(LEV_DEBUG, MOD_APL, L"XadesSignature::foundCertificate: successfully added cert %ls", 
			utilStringWiden(path).c_str());
	return;


	err:
		MWLOG(LEV_DEBUG, MOD_APL, L"XadesSignature::foundCertificate: problem with file %ls ", 
			utilStringWiden(path).c_str());

}


const XMLCh * locateTimestamp(XERCES_NS DOMDocument *doc)
{
	safeBuffer str;
	//Qualified Tag Name
	makeQName(str, XMLString::transcode("etsi") ,"EncapsulatedTimeStamp");
	DOMNodeList *list = doc->getElementsByTagNameNS(XMLString::transcode("*"),
		       	XMLString::transcode("EncapsulatedTimeStamp"));

	if (list->getLength() == 0)
	{
	   return NULL;
	}


	DOMNode * timestamp = list->item(0);

	return timestamp->getFirstChild()->getNodeValue();
	
}

XMLCh* XadesSignature::createURI(const char *path)
{

#ifdef WIN32
	XMLCh * uni_reference_uri = (XMLCh*)pathToURI(utf8_decode(path));
#else
	string default_uri = string("file://localhost/") + Basename((char *)path);
	//TODO: We also need to URL-encode the path on Unix
	XMLCh * uni_reference_uri = XMLString::transcode(default_uri.c_str());
#endif
	return uni_reference_uri;

}

CByteArray &XadesSignature::SignXades(const char ** paths, unsigned int n_paths, bool do_timestamping)
{

	initXerces();

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
	unsigned char signature_hash[SHA1_LEN];

        DOMImplementation *impl = 
		DOMImplementationRegistry::getDOMImplementation(MAKE_UNICODE_STRING("Core"));
	
	XERCES_NS DOMDocument *doc = impl->createDocument(0, MAKE_UNICODE_STRING("Document"), NULL);
	DOMElement *rootElem = doc->getDocumentElement();


	memset(toFill, 0x2f, 20);
	oidlen = appendOID(toFill);

	try {
		
		// Create a signature object

		sig = prov.newSignature();
		//sig->setDSIGNSPrefix(MAKE_UNICODE_STRING("ds"));

		// Use it to create a blank signature DOM structure from the doc
		sigNode = sig->createBlankSignature(doc, CANON_C14N_COM, SIGNATURE_RSA, HASH_SHA1);

		// Insert the signature DOM nodes into the doc
		rootElem->appendChild(doc->createTextNode(MAKE_UNICODE_STRING("\n")));
		rootElem->appendChild(sigNode);
		rootElem->appendChild(doc->createTextNode(MAKE_UNICODE_STRING("\n")));

		for (unsigned int i = 0; i != n_paths ; i++)
		{
			const char * path = paths[i];
			//Create a reference to the external file
			DSIGReference * ref = sig->createReference(createURI(path));
			MWLOG(LEV_DEBUG, MOD_APL, L"SignXades(): Hashing file %s", path);
			sha1_hash = HashFile(path);

			//Fill the hash value as base64-encoded string
			ref->setExternalHash(sha1_hash.GetBytes());
		}

		try
		{
			// This is a somewhat hackish way of getting the canonicalized hash
			// of the reference
			sig->calculateSignedInfoHash(&toFill[oidlen], 20);
		}
		catch (const XMLException &e)
		{
			MWLOG(LEV_ERROR, MOD_APL, L"Exception in calculateSignedInfoHash(), message: %S - %s", 
				XMLString::transcode(e.getType()), XMLString::transcode(e.getMessage()));
		
		}
		
		CByteArray certData;
	        mp_card->readFile(PTEID_FILE_CERT_SIGNATURE, certData);

		loadCert (certData, pub_key);


		keyInfoX509 = sig->appendX509Data();

		ssl_cert = new OpenSSLCryptoX509(mp_cert);

		keyInfoX509->appendX509Certificate(ssl_cert->getDEREncodingSB().sbStrToXMLCh());
		
		try
		{
			rsa_signature = mp_card->Sign(CByteArray(toFill, SHA1_LEN + oidlen), true);
		}
		catch(...)
		{
			MWLOG(LEV_ERROR, MOD_APL, L"APLCard::Sign() failed, can't generate XADES signature");
			throw;
		}


		sig->signExternal((XMLByte *)(rsa_signature.GetBytes()), PTEID_SIGNATURE_LENGTH); //RSA Signature with modlength=1024 bits
		
		DOMNode * timestamp_node = addSignatureProperties(sig);

		if (do_timestamping)
		{

			//Hash the signature value, generate the ASN.1 encoded request 
			//and then send it to the timestamp server
			SHA1 (rsa_signature.GetBytes(), PTEID_SIGNATURE_LENGTH, signature_hash);

			generate_asn1_request_struct(signature_hash);

			timestamp_data(timestamp_asn1_request, ASN1_LEN);

			CByteArray *timestamp_blob = &XadesSignature::mp_timestamp_data;

			if (timestamp_blob->Size() == 0)
				MWLOG(LEV_ERROR, MOD_APL,
			    L"An error occurred in timestamp_data. It's possible that the timestamp service is down ");
			else
			{
				unsigned char * base64str = base64Encode(timestamp_blob->GetBytes(), timestamp_blob->Size());
				addTimestampNode(timestamp_node, base64str);
			}
		}
	}
	catch (XSECCryptoException &e)
	{
		MWLOG(LEV_ERROR, MOD_APL, L"An error occurred in XSec Crypto Functions. Message: %s\n",
		 e.getMsg());

	}
	catch (XSECException &e)
	{
		MWLOG(LEV_ERROR, MOD_APL, L"An error occured during a signature load. Message: %s\n",
		 e.getMsg());
		
	}

	return *WriteToByteArray(doc);

}


}
