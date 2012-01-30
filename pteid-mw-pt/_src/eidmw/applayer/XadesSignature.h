#ifndef XADESSIG_H
#define XADESSIG_H
#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xsec/dsig/DSIGSignature.hpp>

#include <string>
#include <openssl/evp.h>
#include <openssl/x509.h>

#include "Export.h" 
#include "APLCard.h"

#define ASN1_LEN 43
#define SHA1_OFFSET 20

#define TIMESTAMPING_HOST "http://ts.cartaodecidadao.pt/tsa/server"

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
		
		/* Add methods for Sign multiple files */
		CByteArray &SignXades(CByteArray ba, const char *URL);
		CByteArray &SignXadesT(CByteArray ba, const char *URL);
		CByteArray &SignXades(const char * path, unsigned int n_paths);
		CByteArray &SignXadesT(const char * path, unsigned int n_paths);

		bool ValidateXades(CByteArray signature);
		

		private:
		
		CByteArray HashFile(const char *file_path);
		void addSignatureProperties(DSIGSignature *sig);
		CByteArray *WriteToByteArray(xercesc_3_1::DOMDocument *doc); 
		//Utility methods	
		void loadCert(CByteArray &ba, EVP_PKEY *pub_key);
		
		int appendOID(XMLByte *toFill);

		void addTimestampNode(DSIGSignature *sig);

		//Utility methods for timestamping
		size_t curl_write_data(char *ptr, size_t size, size_t nmemb, void * stream); 
		CByteArray timestamp_data(const unsigned char *input, unsigned int data_len);
		void generate_asn1_request_struct(char *sha_1);

		void initXerces();

		
		X509 * mp_cert;
		//APL_CryptoFwk *mp_crypto;
		APL_Card *mp_card;
		CByteArray mp_timestamp_data;


	};
}

#endif 

