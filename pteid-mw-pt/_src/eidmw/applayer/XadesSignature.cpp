/**
 *
 * XAdES and XAdES-T signature generation for PT-Eid Middleware
 *
 * Author: André Guerreiro <andre.guerreiro@caixamagica.pt>
 *
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

#include "MiscUtil.h"

// Timestamp.cc contains the implementation for local
// timestamp validation using OpenSSL 1.0. ATM its disabled because
// it complicates the deployment
//#include "Timestamp.h"

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

	CByteArray XadesSignature::mp_validate_data = CByteArray();
	CByteArray XadesSignature::mp_subject_name = CByteArray();

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
	
	size_t XadesSignature::curl_write_validation_data(char *ptr, size_t size, size_t nmemb, void * stream)
	{
		size_t realsize = size * nmemb;
		mp_validate_data.Append((const unsigned char*)ptr, realsize);

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

	char *strip_newlines(const char *str)
	{
		char *cleaned = new char[strlen(str)];
		int j = 0;
		for (unsigned int i=0; i < strlen(str); i++)
		{
			if (str[i] != '\n')
				cleaned[j++] = str[i]; 
		}
		cleaned[j] = 0;
		return cleaned;
	}


	/* This function validates timestamps using the HTTP Form available
	 at http://ts.cartaodecidadao.pt/tsaclient/validate.html */
	#define TSA_VALIDATE_URL "http://ts.cartaodecidadao.pt/tsaclient/validate.html"

	void XadesSignature::do_post_validate_timestamp(char *input, long input_len, char *sha1_string)
	{

		CURL *curl;
		CURLcode res;
		char error_buf[CURL_ERROR_SIZE];

		//Make sure the static array receiving the network reply 
		// is zero'd out before each request
		mp_validate_data.Chop(mp_validate_data.Size());

		//Get Timestamping server URL from config
		APL_Config tsa_url(CConfig::EIDMW_CONFIG_PARAM_XSIGN_TSAURL);
		const char * TSA_URL = tsa_url.getString();
		static const char expect_header[] = "Expect:";

		curl_global_init(CURL_GLOBAL_ALL);

		curl = curl_easy_init();

		if (curl) 
		{

			struct curl_slist *headers= NULL;
			struct curl_httppost* formpost = NULL;
		      	struct curl_httppost* lastptr = NULL;

			headers = curl_slist_append(headers, "User-Agent: PTeID Middleware v2");
			headers = curl_slist_append(headers, expect_header);

			/* Build the form contents */

			curl_formadd(&formpost,
				     &lastptr,
				    CURLFORM_COPYNAME, "hash",
				    CURLFORM_COPYCONTENTS, sha1_string,
			            CURLFORM_END);

			curl_formadd(&formpost,
				     &lastptr,
				    CURLFORM_COPYNAME, "filename", "filename.xml",
				    CURLFORM_COPYCONTENTS, generateId(20),  //It should have some randomness
			            CURLFORM_END);

			curl_formadd(&formpost,
          			     &lastptr,
			             CURLFORM_COPYNAME, "fileUpload",
			             CURLFORM_BUFFER, "ts_resp",
			             CURLFORM_BUFFERPTR, input,
			             CURLFORM_BUFFERLENGTH, input_len,
			             CURLFORM_END);

			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

			/* Now specify the POST data */ 
			curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);

			curl_easy_setopt(curl, CURLOPT_URL, TSA_VALIDATE_URL);

			curl_easy_setopt(curl, CURLOPT_TIMEOUT, 15L);

			curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, error_buf);

			//curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);

			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &XadesSignature::curl_write_validation_data);

			/* Perform the request, res will get the return code */ 
			res = curl_easy_perform(curl);

			if (res != 0)
			{
				MWLOG(LEV_ERROR, MOD_APL, L"Timestamp Validation error in HTTP POST request. LibcURL returned %ls\n", 
					utilStringWiden(string(error_buf)).c_str());
			}

			/* always cleanup */ 
			curl_easy_cleanup(curl);

			curl_slist_free_all(headers);
			curl_formfree(formpost);

			curl_global_cleanup();
		}


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


bool XadesSignature::checkExternalRefs(DSIGReferenceList *refs, tHashedFile **hashes)
{
        const char * URI;
        XMLByte arr[SHA1_LEN*sizeof(unsigned char)];

	bool res;
	int j;
	tHashedFile *hashed_file=NULL;

	for ( j = 0 ; hashes[j] != NULL; j++)
	{	
		res = false;	
		hashed_file = hashes[j];
		for (int i = 0; i!=refs->getSize() ; i++)
		{	
			DSIGReference * r = refs->item(i);

			r->readHash(arr, 20);

			if (memcmp(arr, hashed_file->hash->GetBytes(), SHA1_LEN) == 0)
			{   
				res = true;
				break;
			}

		}
		// If a single hash reference fails then abort
		if (res == false)
		{
			MWLOG (LEV_ERROR, MOD_APL,
					L" checkExternalRefs(): SHA-1 Hash Value for file %ls doesn't match.",
				       	utilStringWiden(*(hashed_file->URI)).c_str());
			return false;
		}
	}
	
	//If container has no signed files this test is automatically false
	if (j == 0)
	{
		MWLOG (LEV_DEBUG, MOD_APL, L"checkExternalRefs(): Container has no signed files!");
		return false;
	}

	MWLOG (LEV_DEBUG, MOD_APL, L" checkExternalRefs(): All External References matched.");
	return true;
}

/*
 * A little HTML-scraping to get the validation result and timestamp value
 */
bool XadesSignature::grep_validation_result (char *time_and_date)
{
	
	// Accented 'a' encoded in latin-1 as expected
	const char * valid_timestamp_pattern = "Selo Temporal V\xE1lido.";
	const char * invalid_timestamp_pattern = "Selo Temporal n\xE3o corresponde ao ficheiro seleccionado";
	const char * general_error_pattern = "Ocorreu um erro";
	const char * signed_by = getString(16);
	unsigned char *haystack = mp_validate_data.GetBytes();

	if (mp_validate_data.Size() == 0)
	{
		MWLOG(LEV_DEBUG, MOD_APL, L"ValidateTimestamp: grep_validation_result called with empty mp_timestamp_data");
		return false;
	}
	
	const char *ts_str = strstr((const char *)haystack, valid_timestamp_pattern);

	//Warning: magic offsets ahead...
	if (ts_str != NULL)
	{
		//Grab the TimeDate string
		strcat(time_and_date, signed_by);
		strncpy(time_and_date+strlen(signed_by), ts_str+36, 27);
		time_and_date[27+strlen(signed_by)] = '\n';
		time_and_date[28+strlen(signed_by)] = 0;

		return true; 
	}
	else
	{
		if (strstr((const char *)haystack, invalid_timestamp_pattern) == NULL)
		//Unexpected output but invalid nonetheless!
			MWLOG(LEV_ERROR, MOD_APL, 
	L"ValidateTimestamp: Unexpected output in the timestamp validation form, considered invalid");

		return false; 
	}

}

char *getHexString(unsigned char *bytes, unsigned int len)
{
	char *hex = new char[len*2+1];
	memset (hex, 0, len*2+1);
	
	for (unsigned int i = 0; i!=len; i++)
		sprintf(hex+(i*2), "%02x", bytes[i]);

	return hex;
}

bool XadesSignature::ValidateTimestamp (CByteArray signature, CByteArray ts_resp, char *errors,
				unsigned long *error_length)
{

//TODO: The DOMDocument should be already cached by ValidateXades()
	bool errorsOccured = false;

	bool result = false;
	unsigned char *signature_bin;
	MWLOG(LEV_DEBUG, MOD_APL, L"ValidateTimestamp() called with XML content of %d bytes."
		L"Error buffer addr: 0x%x, error_length=%d ", signature.Size(), errors, *error_length);

	if (signature.Size() == 0)
	{
		int err_len = _snprintf(errors, *error_length, "%s", getString(0));
		*error_length = err_len;
		MWLOG(LEV_ERROR, MOD_APL, L"ValidateTimestamp() received empty Signature. This most likely means a corrupt zipfile");
		return false;
	}

	initXerces();

	//Load XML from a MemoryBuffer
	MemBufInputSource * source = new MemBufInputSource(signature.GetBytes(),
			(XMLSize_t)signature.Size(),
			XMLString::transcode(generateId(20)));

	XercesDOMParser * parser = new XercesDOMParser;
	Janitor<XercesDOMParser> j_parser(parser);

	parser->setDoNamespaces(true);
	parser->setCreateEntityReferenceNodes(true);

	// Now parse out file

	xsecsize_t errorCount = 0;
	try
	{
		parser->parse(*source);
	}

	catch (const XMLException& e)
	{
		MWLOG(LEV_ERROR, MOD_APL, L"An error occured during parsing\n   Message: %s",
				XMLString::transcode(e.getMessage()));
		errorsOccured = true;
	}
	catch (const DOMException& e)
	{
		MWLOG(LEV_ERROR, MOD_APL, L"A DOM error occured during parsing\n   DOMException code: %d",
				e.code);
		errorsOccured = true;
	}

	if (errorsOccured)
		return false;

	DOMNode *doc;

	doc = parser->getDocument();

	// Find the signature node
	
	DOMNode *sigNode = findDSIGNode(doc, "Signature");

	XSECProvider prov;
	unsigned char signature_hash[SHA1_LEN];
	char time_and_date[100];
	unsigned int sig_len = 0;
	memset(time_and_date, 0, sizeof(time_and_date));
	XERCES_NS DOMDocument * theDOM = dynamic_cast<XERCES_NS DOMDocument *>(doc);
	DSIGSignature * sig = prov.newSignatureFromDOM(theDOM, sigNode);
	
	sig->load();

	const char* tmp = XMLString::transcode(sig->getSignatureValue());
	char *tmp2 = strip_newlines(tmp);
	base64Decode(tmp2, strlen(tmp2), signature_bin, sig_len);
	SHA1(signature_bin, sig_len, signature_hash);

	char * sha1_string = getHexString(signature_hash, SHA1_LEN);

	//std::cerr << "POST Parameter (hash): " << sha1_string << std::endl; 
	
	do_post_validate_timestamp((char *)ts_resp.GetBytes(), (long)ts_resp.Size(), sha1_string);

	if (mp_validate_data.Size() == 0)
	{
		*error_length = _snprintf(errors, *error_length, "%s", getString(8));
		return false;
	}
	
	result = grep_validation_result(time_and_date);

	if (result)
		*error_length = _snprintf(errors, *error_length, "%s", time_and_date);
	else
		*error_length = _snprintf(errors, *error_length, "%s", getString(2));

	return result;

}


X509 *loadCertFromPEM(const char *pem_buffer)
{

	char * final_pem = (char *)calloc(strlen(pem_buffer)+ 65, sizeof(char));
	strcat(final_pem, "-----BEGIN CERTIFICATE-----\n");
	strcat(final_pem, pem_buffer);
	strcat(final_pem, "-----END CERTIFICATE-----\n");

	BIO *pem_bio = BIO_new_mem_buf((void *)final_pem, -1);
	X509 *cert = NULL;

	if (pem_bio != NULL)
	{
		cert = PEM_read_bio_X509(pem_bio, NULL, NULL, NULL);
		if (cert == NULL)
			MWLOG(LEV_ERROR, MOD_APL, L"Error parsing certificate from PEM string!");

	}
	
	BIO_free_all(pem_bio);	
	return cert;

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
/*
* This method validates the supplied PEM certificate.
* To be used by ValidateXades()
*
*/
bool XadesSignature::ValidateCert(const char *pem_certificate)
{
	if (pem_certificate == NULL || strlen(pem_certificate) == 0)
		return false;

	bool bStopRequest = false;
	bool res = false;

	OpenSSL_add_all_algorithms();

	X509_STORE *store = X509_STORE_new();
	
	X509 *pCert = NULL;

	APL_Config certs_dir(CConfig::EIDMW_CONFIG_PARAM_GENERAL_CERTS_DIR);
	const char * str_certs_dir = certs_dir.getString();
	CPathUtil::scanDir(str_certs_dir, "", "der", bStopRequest, store, &XadesSignature::foundCertificate);

	X509_STORE_CTX *ctx;
	X509 *cert = NULL;

	ctx = X509_STORE_CTX_new();

	if (ctx == NULL)
	{
		/* Bad error */
		return false;
	}
	
	cert = loadCertFromPEM(pem_certificate);

	if (cert == NULL)
	{
		MWLOG(LEV_ERROR, MOD_APL, L"ValidateCert(): Broken or corrupt certificate");
		return false;
	}

	//Load cert chain into store

	X509_STORE_CTX_init(ctx, store, cert, NULL);

	X509_STORE_CTX_set_flags(ctx, X509_V_FLAG_CB_ISSUER_CHECK);
	
	int validate_res = X509_verify_cert(ctx);

	if (validate_res == 1)
	{
		MWLOG(LEV_DEBUG, MOD_APL, L"ValidateCert(): Valid certificate");
		res = true;
	}
	else
	{
		res = false;
		char subject_buf[1024];
		std::string ssl_error = std::string(X509_verify_cert_error_string(ctx->error));
		MWLOG(LEV_DEBUG, MOD_APL, L"ValidateCert(): Invalid certificate! OpenSSL Error: %ls", 
			utilStringWiden(ssl_error).c_str());

		X509* error_cert = X509_STORE_CTX_get_current_cert(ctx);
		X509_NAME_oneline(X509_get_subject_name(ctx->current_cert), subject_buf, sizeof(subject_buf));
		std::string subject_str = std::string(subject_buf);
		MWLOG(LEV_DEBUG, MOD_APL, L"Certificate that caused the error: %ls",
			utilStringWiden(subject_str).c_str());

	}

	X509_STORE_CTX_free(ctx);
	return res;

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

char * XadesSignature::parseSubjectFromCert(const char *cert_buffer)
{

	//Subject name
	X509 *cert;
	const int BUFSIZE = 500;

	if ((cert = loadCertFromPEM(cert_buffer)) == NULL)
		return NULL;

	char *subject = (char *)malloc(BUFSIZE*sizeof(unsigned char));

	X509_NAME_get_text_by_NID(X509_get_subject_name(cert), NID_commonName, subject, BUFSIZE);

	return subject;

}


bool XadesSignature::ValidateXades(CByteArray signature, tHashedFile **hashes, char *errors, unsigned long *error_length)
{
	bool errorsOccured = false;
	
	MWLOG(LEV_DEBUG, MOD_APL, L"ValidateXades() called with XML content of %d bytes."
		L"Error buffer addr: 0x%x, error_length=%d ", signature.Size(), errors, *error_length);

	if (signature.Size() == 0)
	{
		int err_len = _snprintf(errors, *error_length, "Signature Validation error: " 
				"Couldn't extract signature from zip container");
		*error_length = err_len;
		MWLOG(LEV_ERROR, MOD_APL, L"ValidateXades() received empty Signature. This most likely means a corrupt zipfile");
		return false;
	}

	initXerces();

	//Load XML from a MemoryBuffer
	MemBufInputSource * source = new MemBufInputSource(signature.GetBytes(),
			(XMLSize_t)signature.Size(),
			XMLString::transcode(generateId(20)));


	XercesDOMParser * parser = new XercesDOMParser;
	Janitor<XercesDOMParser> j_parser(parser);

	parser->setDoNamespaces(true);
	parser->setCreateEntityReferenceNodes(true);

	// Now parse out file

	xsecsize_t errorCount = 0;
	try
	{
		parser->parse(*source);
	}

	catch (const XMLException& e)
	{
		MWLOG(LEV_ERROR, MOD_APL, L"An error occured during parsing\n   Message: %s",
				XMLString::transcode(e.getMessage()));
		errorsOccured = true;
	}
	catch (const DOMException& e)
	{
		MWLOG(LEV_ERROR, MOD_APL, L"A DOM error occured during parsing\n   DOMException code: %d",
				e.code);
		errorsOccured = true;
	}

	if (errorsOccured) {
		//Write to output report 
		int err_len = _snprintf(errors, *error_length, "%s", getString(4));
		*error_length = err_len;
		MWLOG(LEV_ERROR, MOD_APL, L"Errors parsing XML Signature, bailing out");
		return false;
	}

	/*
	 *   Now that we have the parsed file, get the DOM document and start looking at it
	*/
	
	DOMNode *doc;		// The document that we parsed

	doc = parser->getDocument();
	XERCES_NS DOMDocument *theDOM = parser->getDocument();

	// Find the signature node
	
	DOMNode *sigNode = findDSIGNode(doc, "Signature");

	// Create the signature checker

	if (sigNode == NULL) {

		int err_len = _snprintf(errors, *error_length, "Signature Validation error: XML Signature Node not found");
		*error_length = err_len;

		MWLOG(LEV_ERROR, MOD_APL, L"ValidateXades: Could not find <Signature> node in the signature provided");
		return false;
	}

	MWLOG(LEV_DEBUG, MOD_APL, L"ValidateXades: XML signature correctly parsed.");

	XSECProvider prov;
	XSECKeyInfoResolverDefault theKeyInfoResolver;

	DSIGSignature * sig = prov.newSignatureFromDOM(theDOM, sigNode);

	// The only way we can verify is using keys read directly from the KeyInfo list,
	// so we add a KeyInfoResolverDefault to the Signature.

	sig->setKeyInfoResolver(&theKeyInfoResolver);
	sig->registerIdAttributeName(MAKE_UNICODE_STRING("ID"));

	bool result = false;
	bool extern_result = false; 
	try {

		sig->load();
				
		// Validate signing certificate first
		DSIGKeyInfo *keyinfo = sig->getKeyInfoList()->item(0);
		// If keyinfo is NULL this most certainly means a broken signature
		// just skip certificate checking
		if (keyinfo != NULL)
		{

			//This should always be the case for signatures created by pteid-mw
			if (keyinfo->getKeyInfoType() == DSIGKeyInfo::KEYINFO_X509)
			{
				DSIGKeyInfoX509 *cert_element = dynamic_cast<DSIGKeyInfoX509 *> (keyinfo);
				const XMLCh *pem_cert = cert_element->getCertificateItem(0);
				char * tmp_cert = XMLString::transcode(pem_cert);
				
				//Clear the static array
				mp_subject_name.Chop(mp_subject_name.Size());
				//Parse the signer's name from the certificate
				char * subject_name = utf8_to_latin1(
					   parseSubjectFromCert(tmp_cert));

				if (subject_name != NULL)
				{
					mp_subject_name.Append(CONST_STR getString(14), strlen(getString(14)));
					mp_subject_name.Append(CONST_STR " ", 1);
					mp_subject_name.Append(CONST_STR subject_name, strlen(subject_name));
				}
				bool cert_result = ValidateCert(tmp_cert);

				if (!cert_result)
				{
					int err_len = _snprintf(errors, *error_length, "%s",  getString(12));
					*error_length = err_len;
					return false;
				}
				
					

				
				XMLString::release(&tmp_cert);
			}
		}
		

		DSIGReferenceList *refs = sig->getReferenceList();

		if (refs != NULL)
			extern_result = checkExternalRefs(refs, hashes);
		if (!extern_result)
		{
			int err_len = _snprintf(errors, *error_length, "%s",  getString(6));
			*error_length = err_len;
			return false;
		}
		
		result = sig->verifySignatureOnly();

	}
	catch (XSECException &e) {
		MWLOG(LEV_ERROR, MOD_APL, L"ValidateXades(): XSECException thrown. Detail: %ls",
			e.getMsg());
		result = false;
	}
	catch (XSECCryptoException &e) {
		MWLOG(LEV_ERROR, MOD_APL, L"ValidateXades(): XSECCryptoException thrown. Detail: %ls",
			e.getMsg());
		return false;
	}

	if (result == false)
	{
		int err_len = _snprintf(errors, *error_length, "%s", getString(10));
		*error_length = err_len;
	}

	return result;

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
