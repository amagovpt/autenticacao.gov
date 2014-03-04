#ifndef XADESSIG_H
#define XADESSIG_H
#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XercesDefs.hpp>
#include <xsec/dsig/DSIGSignature.hpp>

#include <string>
#include <vector>
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

	void base64Decode(const char *array, unsigned int inlen, unsigned char *&decoded, unsigned int &decoded_len);

	class XadesSignature
	{
		public:

		XadesSignature(APL_Card *card): mp_card(card), mp_cert(NULL), m_do_timestamping(false), m_do_long_term_validation(false) 
		{ };
		
		~XadesSignature()
		{
			if (mp_cert != NULL)
				X509_free(mp_cert);
		};

		CByteArray &SignXades(const char ** paths, unsigned int n_paths);
		bool enableTimestamp() { m_do_timestamping = true; }
		bool enableLongTermValidation() { m_do_long_term_validation = true; };

		CByteArray mp_timestamp_data;

		private:

		static void initXMLUtils();
		static void terminateXMLUtils();

		bool m_do_timestamping;
		bool m_do_long_term_validation;

		CByteArray &Sign(const char ** paths, unsigned int n_paths);
		CByteArray HashFile(const char *file_path);
		
		int HashSignedInfoNode(DOMDocument *doc, XMLByte *outbuf);
		int HashSignedPropertiesNode(DOMDocument *doc, XMLByte *outbuf);
		DOMNode * addSignatureProperties(DSIGSignature *sig, XMLCh *sig_id, CByteArray &cert_data);
		bool AddSignatureTimestamp(DOMDocument *dom);
		bool appendTimestamp(DOMDocument * doc, DOMNode *parent, const char * tag_name, std::string to_timestamp);
		bool AddRevocationInfo(DOMDocument * dom);
		bool AddArchiveTimestamp(DOMDocument *dom);
		bool AddSigAndRefsTimestamp(DOMDocument *dom);
		bool addCompleteCertificateRefs(DOMDocument *dom);

		CByteArray *WriteToByteArray(XERCES_NS DOMDocument *doc); 

		//Utility methods for signature
		void addCertificateChain(DSIGKeyInfoX509 *keyInfo);
		X509 * addCertificateToKeyInfo(CByteArray &cert, DSIGKeyInfoX509 *keyInfo);
		void loadSignerCert(CByteArray &ba, EVP_PKEY *pub_key);
		int appendOID(XMLByte *toFill);
		void addTimestampNode(XERCES_NS DOMNode *node, unsigned char *timestamp);
		XMLCh* createURI(const char *path);

		X509 * mp_cert, *mp_signature_ca_cert;
		std::vector<X509 *> m_certs;
		std::vector<CByteArray> m_cert_bas;

		std::string m_referenced_data;
		APL_Card *mp_card;
	};
}

#endif 

