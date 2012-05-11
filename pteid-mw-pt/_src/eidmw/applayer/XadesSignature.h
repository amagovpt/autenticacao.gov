#ifndef XADESSIG_H
#define XADESSIG_H
#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XercesDefs.hpp>
#include <xsec/dsig/DSIGSignature.hpp>

#include <string>
#include <openssl/evp.h>
#include <openssl/x509.h>

#include "Export.h" 
#include "APLCard.h"
#include "MiscUtil.h"

#define ASN1_LEN 43
#define SHA1_OFFSET 20

#define XERCES_NS XERCES_CPP_NAMESPACE_QUALIFIER

#define CONST_STR (const unsigned char *)

#ifndef WIN32
#define _strdup strdup
#endif


XERCES_CPP_NAMESPACE_USE

namespace eIDMW
{

	class CByteArray;
	static unsigned char timestamp_asn1_request[ASN1_LEN] =
	{
	0x30, 0x29, 0x02, 0x01, 0x01, 0x30, 0x21, 0x30, 0x09, 0x06, 0x05, 0x2b, 0x0e, 0x03, 0x02, 0x1a,
	0x05, 0x00, 0x04, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0xff
	};

	static char *XADES_STRINGS[] = 
	{
		"Signature Validation error: Couldn't extract signature from zip container", 
		"Erro de validação da assinatura: Não foi possível extrair a assinatura",
		"Timestamp Validation: The timestamp does not match the signed content", 
		"Validação de Selo Temporal: O selo temporal não corresponde ao conteúdo assinado",
		"Signature Validation error: Malformed XML Document",
		"Erro de validação da assinatura: Erro de sintaxe na assinatura XML",
		"Signature Validation error: At least one of the signed file(s) was changed or is missing",
		"Erro de validação da assinatura: Pelo menos um dos ficheiros assinados foi alterado ou está em falta",
		"Timestamp Validation: Internal Error, couldn't validate timestamp", 
		"Validação de Selo Temporal: Erro Interno, não foi possível validar o selo temporal", 
		"Validation error: RSA Signature of referenced content is invalid",
		"Erro de validação da assinatura: A assinatura criptográfica do conteúdo está inválida",
		"Validation Error: The certificate used to sign this data is not trusted",
		"Erro de validação da assinatura: O certificado contido na assinatura não provém de uma fonte confiável",
		"Signed by:",
		"Assinado por:",
		"Timestamp: ",
		"Selo temporal: "
	};

	class XadesSignature
	{
		public:

		XadesSignature(APL_Card *card): mp_card(card), mp_cert(NULL)
		{

		};
		
		~XadesSignature()
		{
		   X509_free(mp_cert);
		};

		CByteArray &SignXades(CByteArray ba, const char *URL);
		CByteArray &SignXades(const char ** paths, unsigned int n_paths, bool do_timestamp);
		
		static bool ValidateCert(const char *pem_certificate);
		static void foundCertificate (const char *SubDir, const char *File, void *param);
		static bool checkExternalRefs(DSIGReferenceList *refs, tHashedFile **hashes);
		static bool ValidateXades(CByteArray signature, tHashedFile **hashes, char *errors, unsigned long *error_length);
		
		static bool ValidateTimestamp (CByteArray signature, CByteArray ts_resp, char *errors, unsigned long *error_length);
		static bool grep_validation_result (char *time_and_date);

		static CByteArray mp_timestamp_data;
		static CByteArray mp_validate_data;
		static CByteArray mp_subject_name;
		static void do_post_validate_timestamp(char *input, long input_len, char *sha1_string);

		private:
		
		std::string getTS_CAPath();
		
		CByteArray HashFile(const char *file_path);
		static char * parseSubjectFromCert(const char *cert);
		DOMNode * addSignatureProperties(DSIGSignature *sig);
		CByteArray *WriteToByteArray(XERCES_NS DOMDocument *doc); 
		//Utility methods for signature
		void loadCert(CByteArray &ba, EVP_PKEY *pub_key);


		//Locate the text subnode of an EncapsulatedTimestamp
		static XMLCh * locateTimestamp(XERCES_NS DOMDocument *doc);
		
		int appendOID(XMLByte *toFill);

		void addTimestampNode(XERCES_NS DOMNode *node, unsigned char *timestamp);
		XMLCh* createURI(const char *path);


		//Utility methods for timestamping
		
		//Curl write_function callback: it writes the data to the static CByteArray mp_timestamp_data
		static size_t curl_write_data(char *ptr, size_t size, size_t nmemb, void * stream); 

		//Curl write_function callback: it writes the data to the static CByteArray mp_validate_data
		static size_t curl_write_validation_data(char *ptr, size_t size, size_t nmemb, void * stream); 

		void timestamp_data(const unsigned char *input, unsigned int data_len);

	
		void generate_asn1_request_struct(unsigned char *sha_1);

		static void initXerces();

		static const char * getString(unsigned int id)
		{
			
			std::wstring language = CConfig::GetString(CConfig::EIDMW_CONFIG_PARAM_GENERAL_LANGUAGE);
			if(wcscmp(L"nl", language.c_str())==0)
			{
				return XADES_STRINGS[id+1];
			}
			else
				return XADES_STRINGS[id];
		}
		
		X509 * mp_cert;
		APL_Card *mp_card;


	};
}

#endif 

