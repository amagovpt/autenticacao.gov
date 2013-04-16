#include <cstring>
#include "MWException.h"
#include "ByteArray.h"
#include "Log.h"
#include "Util.h"
#include "MiscUtil.h"

// Xerces

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/XMLException.hpp>
#include <xercesc/util/Janitor.hpp>

//XML-Security
#include <xsec/framework/XSECProvider.hpp>
#include <xsec/framework/XSECException.hpp>
#include <xsec/dsig/DSIGReference.hpp>
#include <xsec/dsig/DSIGSignature.hpp>
#include <xsec/dsig/DSIGObject.hpp>
#include <xsec/dsig/DSIGReferenceList.hpp>
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

//OpenSSL
#include <openssl/sha.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/bio.h>
#include <openssl/err.h>

#include <curl/curl.h>

#include "APLConfig.h"

#include "static_pteid_certs.h"
#include "SigVerifier.h"

//Make up for the absence of strptime on Windows CRT
#ifdef _WIN32
#include "strptime.c"
#endif

#define XERCES_NS XERCES_CPP_NAMESPACE_QUALIFIER
#define SHA1_LEN 20

XERCES_CPP_NAMESPACE_USE

namespace eIDMW
{

	//PIMPL Idiom, just to avoid exposing private details in the header file
	class SignatureImpl
	{
	public:
		bool checkExternalRefs(DSIGReferenceList *refs, tHashedFile **hashes);
		XERCES_NS DOMNode *m_signature_dom;
	};


	SignatureVerifier::SignatureVerifier(const char *sig_container_path)
	{
		m_sigcontainer_path = (char *)malloc(strlen(sig_container_path)+1);
		strcpy((char *)m_sigcontainer_path, sig_container_path);
		m_time_and_date = (char *)calloc(50, sizeof(char));
		
		pimpl = new SignatureImpl();
		pimpl->m_signature_dom = NULL;
		m_cert = NULL;

	}

	SignatureVerifier::~SignatureVerifier()
	{
		free(m_sigcontainer_path);
		free(m_time_and_date);
		delete pimpl;
	}

	size_t curl_write_validation_data(char *ptr, size_t size, size_t nmemb, void * userdata)
	{
		size_t realsize = size * nmemb;
		CByteArray * buffer = (CByteArray *)userdata;
		buffer->Append((const unsigned char*)ptr, realsize);

		return realsize;

	}

	char *getHexString(unsigned char *bytes, unsigned int len)
	{
		char *hex = new char[len*2+1];
		memset (hex, 0, len*2+1);

		for (unsigned int i = 0; i!=len; i++)
			sprintf(hex+(i*2), "%02x", bytes[i]);

		return hex;
	}


	void SignatureVerifier::base64Decode(const char *array, unsigned int inlen, unsigned char *&decoded, unsigned int &decoded_len)
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

	CByteArray SignatureVerifier::do_post_validate_timestamp(char *input, long input_len, char *sha1_string)
	{

		CURL *curl;
		CURLcode res;
		char error_buf[CURL_ERROR_SIZE];

		CByteArray *validate_data = new CByteArray();

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

			//This is the ByteArray to where curl_write_validation_data will write
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, validate_data);
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &curl_write_validation_data);

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
		return *validate_data;

	}

SigVerifyErrorCode SignatureVerifier::ValidateTimestamp (CByteArray signature, CByteArray ts_resp)
{

	bool errorsOccured = false;

	bool result = false;
	unsigned char * signature_bin;

	if (signature.Size() == 0)
	{
		MWLOG(LEV_ERROR, MOD_APL, L"ValidateTimestamp() received empty Signature. This most likely means a corrupt zipfile");
		return XADES_ERROR_INVALID_TIMESTAMP;
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
		MWLOG(LEV_ERROR, MOD_APL, L"Errors parsing XML Signature, bailing out");
		return XADES_ERROR_BROKENSIG;
	}

	
	DOMNode *doc;		// The document that we parsed

	doc = parser->getDocument();

	//XERCES_NS DOMNode * doc = pimpl->m_signature_dom;

	// Find the signature node
	
	XERCES_NS DOMNode *sigNode = findDSIGNode(doc, "Signature");

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
	
	CByteArray validate_data = do_post_validate_timestamp((char *)ts_resp.GetBytes(), (long)ts_resp.Size(), sha1_string);

	if (validate_data.Size() == 0)
	{
		MWLOG(LEV_ERROR, MOD_APL, L"Timestamp Validation form returned nothing!");
		return XADES_ERROR_INVALID_TIMESTAMP;
	}
	
	result = grep_validation_result(validate_data);

	return result? XADES_ERROR_OK : XADES_ERROR_INVALID_TIMESTAMP;


}
/*
 * A little HTML-scraping to get the validation result and timestamp value
 */
bool SignatureVerifier::grep_validation_result (CByteArray validate_data)
{
	
	// Accented 'a' encoded in latin-1 as expected
	const char * valid_timestamp_pattern = "Selo Temporal V\xE1lido.";
	const char * invalid_timestamp_pattern = "Selo Temporal n\xE3o corresponde ao ficheiro seleccionado";
	const char * general_error_pattern = "Ocorreu um erro";
	// const char * signed_by = getString(16);
	unsigned char *haystack = validate_data.GetBytes();

	if (validate_data.Size() == 0)
	{
		MWLOG(LEV_DEBUG, MOD_APL, L"ValidateTimestamp: grep_validation_result called with empty mp_timestamp_data");
		return false;
	}
	
	const char *ts_str = strstr((const char *)haystack, valid_timestamp_pattern);

	//Warning: magic offsets ahead...
	if (ts_str != NULL)
	{
		//Grab the TimeDate string
		strncpy(m_time_and_date, ts_str+36, 27);
		m_time_and_date[27] = 0;

		return true; 
	}
	else
	{
		if (strstr((const char *)haystack, invalid_timestamp_pattern) == NULL)
		//Unexpected output but invalid nonetheless!
			MWLOG(LEV_ERROR, MOD_APL, 
        	L"ValidateTimestamp: Unexpected output in the timestamp validation form, considered invalid but "
			L"probably the webform we're using is broken...");

		return false; 
	}

}

	SigVerifyErrorCode SignatureVerifier::Verify()
	{

		if (strlen(m_sigcontainer_path) == 0)
			throw CMWEXCEPTION(EIDMW_ERR_CHECK);

		if (!CPathUtil::existFile(m_sigcontainer_path))
			throw CMWEXCEPTION(EIDMW_ERR_CHECK);

		tHashedFile **hashes = NULL;
		int i = 0;
		SigVerifyErrorCode result;
		Container *container = new Container(m_sigcontainer_path);
		int n_files = 0;

		hashes = container->getHashes(&n_files);

		CByteArray sig_content = container->ExtractSignature();
		CByteArray timestamp = container->ExtractTimestamp();

		delete container;

		result = ValidateXades(sig_content, hashes);
		
		//Dont check the Timestamp if the signature is invalid
		if (result == eIDMW::XADES_ERROR_OK)
		{
			if (timestamp.Size() > 0)
			{

				result = ValidateTimestamp(sig_content, timestamp);

			}
		}

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

char * SignatureVerifier::GetSigner()
{

	if (m_cert == NULL)
		throw CMWEXCEPTION(EIDMW_ERR_CHECK);

	return parseSubjectFromCert();

}


//TODO: Add some more exception-handling
char * SignatureVerifier::GetTimestampString()
{
	if (strlen(m_time_and_date) == 0)
		return NULL;

	return m_time_and_date;

}

time_t SignatureVerifier::GetUnixTimestamp()
{
	if (strlen(m_time_and_date) == 0)
		throw CMWEXCEPTION(EIDMW_ERR_CHECK);

	struct tm time;
	//This will ignore the milliseconds and timezone components...
	char * ret = strptime(m_time_and_date, "%d-%m-%Y %H:%M:%S", &time);
	if (ret == NULL)
		printf("strptime failed!\n");

	return mktime(&time);
}


char * SignatureVerifier::parseSubjectFromCert()
{
 
       //Subject name
       X509 *cert;

       X509_NAME * subject_struct = X509_get_subject_name(m_cert);	

       int space_needed = X509_NAME_get_text_by_NID(subject_struct, NID_commonName, NULL, 0) +1;
       char *subject = (char *)malloc(space_needed);

       X509_NAME_get_text_by_NID(subject_struct, NID_commonName, subject, space_needed);

       return subject;
}


bool SignatureVerifier::ValidateCert(const char *pem_certificate)
{
	if (pem_certificate == NULL || strlen(pem_certificate) == 0)
		return false;

	bool bStopRequest = false;
	bool res = false;

	OpenSSL_add_all_algorithms();

	X509_STORE *store = X509_STORE_new();
	
	X509 *pCert = NULL;
	unsigned char * cert_data = NULL;
	char *parsing_error = NULL;


	for (unsigned int i = 0; i != CERTS_N;
		i++)
	{
		pCert = NULL;
		cert_data = PTEID_CERTS[i].cert_data;
	    pCert = d2i_X509(&pCert, (const unsigned char **)&cert_data, 
			PTEID_CERTS[i].cert_len);

		if (pCert == NULL)
		{
			parsing_error = ERR_error_string(ERR_get_error(), NULL);
		MWLOG(LEV_ERROR, MOD_APL, L"XadesSignature::ValidateCert: Error parsing certificate #%d. Details: %s",
				i, parsing_error);
		}
		else
		{
			if(X509_STORE_add_cert(store, pCert) == 0)
				printf("XadesSignature::ValidateCert: error adding certificate #%d\n",  i);
		}
	
	}
	
	X509_STORE_CTX *ctx;
	X509 *cert = NULL;

	ctx = X509_STORE_CTX_new();

	if (ctx == NULL)
	{
		/* Bad error */
		return false;
	}
	
	cert = loadCertFromPEM(pem_certificate);
	m_cert = cert;

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

void SignatureVerifier::initXerces()
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



bool SignatureImpl::checkExternalRefs(DSIGReferenceList *refs, tHashedFile **hashes)
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


SigVerifyErrorCode SignatureVerifier::ValidateXades(CByteArray signature, tHashedFile **hashes)
{
	bool errorsOccured = false;
	
	MWLOG(LEV_DEBUG, MOD_APL, L"ValidateXades() called with XML content of %d bytes."
		, signature.Size());

	if (signature.Size() == 0)
	{
		MWLOG(LEV_ERROR, MOD_APL, L"ValidateXades() received empty Signature. This most likely means a corrupt zipfile");
		return XADES_ERROR_NOSIG;
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
		MWLOG(LEV_ERROR, MOD_APL, L"Errors parsing XML Signature, bailing out");
		return XADES_ERROR_BROKENSIG;
	}

	
	DOMNode *doc;		// The document that we parsed

	doc = parser->getDocument();
	/*
	//Saving the DOM Node as an instance variable to avoid reparsing
	// We need to clone the Node because its memory is owned by the parser object
	try
	{
	pimpl->m_signature_dom = doc->cloneNode(true);
	}
	catch (const DOMException& e)
	{
	
		MWLOG(LEV_ERROR, MOD_APL, L"An error occured during cloneNode(). Message: %s",
				e.getMessage());
	}
	*/

	XERCES_NS DOMDocument *theDOM = parser->getDocument();


	// Find the signature node
	
	DOMNode *sigNode = findDSIGNode(doc, "Signature");

	// Create the signature checker

	if (sigNode == NULL) {

		MWLOG(LEV_ERROR, MOD_APL, L"ValidateXades: Could not find <Signature> node in the signature provided");
		return XADES_ERROR_BROKENSIG;
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
			
				bool cert_result = ValidateCert(tmp_cert);

				if (!cert_result)
				{
					return XADES_ERROR_INVALID_CERTIFICATE;
				}

				
				XMLString::release(&tmp_cert);
			}
		}
		

		DSIGReferenceList *refs = sig->getReferenceList();

		if (refs != NULL)
			extern_result = pimpl->checkExternalRefs(refs, hashes);
		if (!extern_result)
		{
			return XADES_ERROR_EXTERNAL_REFS;
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
		result = false;
	}

	XSECPlatformUtils::Terminate();
	XMLPlatformUtils::Terminate();

	if (result == false)
	{
		return XADES_ERROR_INVALID_RSA;
	}

	return XADES_ERROR_OK;

}

}
