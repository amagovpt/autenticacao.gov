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


XERCES_CPP_NAMESPACE_USE

namespace eIDMW
{

	class CByteArray;

	class XadesSignature
	{
		public:

		XadesSignature(APL_Card *card): m_do_timestamping(false), m_do_long_term_validation(false), mp_cert(NULL), m_digest_state(NULL), mp_card(card)
		{ };

		~XadesSignature()
		{

			for (int i = 0; i != m_certs.size(); i++)
				X509_free(m_certs[i]);
		};

		CByteArray &SignXades(const char ** paths, unsigned int n_paths);
		void enableTimestamp() { m_do_timestamping = true; }
		void enableLongTermValidation() { m_do_long_term_validation = true; };

		CByteArray mp_timestamp_data;

		private:

		static void initXMLUtils();
		static void terminateXMLUtils();

		bool m_do_timestamping;
		bool m_do_long_term_validation;

		CByteArray &Sign(const char ** paths, unsigned int n_paths);
		CByteArray HashFile(const char *file_path);

		void addCardSignature(unsigned char *signature, unsigned int siglen, XERCES_NS DOMDocument *doc);
		void setReferenceHash(XMLByte *hash, unsigned int hash_len, int ref_index, XERCES_NS DOMDocument *doc);

		int HashSignedInfoNode(XERCES_NS DOMDocument *doc, XMLByte *outbuf);
		int HashSignedPropertiesNode(XERCES_NS DOMDocument *doc, XMLByte *outbuf);
		DOMNode * addSignatureProperties(DSIGSignature *sig, XMLCh *sig_id, CByteArray &cert_data);
		bool AddSignatureTimestamp(XERCES_NS DOMDocument *dom);
		bool appendTimestamp(XERCES_NS DOMDocument * doc, DOMNode *parent, const char * tag_name, std::string to_timestamp);
		bool AddRevocationInfo(XERCES_NS DOMDocument * dom);
		bool AddArchiveTimestamp(XERCES_NS DOMDocument *dom);
		bool AddSigAndRefsTimestamp(XERCES_NS DOMDocument *dom);
		bool addCompleteCertificateRefs(XERCES_NS DOMDocument *dom);

		CByteArray *WriteToByteArray(XERCES_NS DOMDocument *doc);

		//Utility methods for signature
		void addCertificateChain(DSIGKeyInfoX509 *keyInfo);
		void addCertificateToKeyInfo(const CByteArray &cert, DSIGKeyInfoX509 *keyInfo);
		void loadSignerCert(CByteArray &ba, EVP_PKEY *pub_key);
		int appendOID(XMLByte *toFill);
		void addTimestampNode(XERCES_NS DOMNode *node, unsigned char *timestamp);
		XMLCh* createURI(const char *path);

		X509 * mp_cert;
		std::vector<X509 *> m_certs;
		std::vector<CByteArray> m_cert_bas;

		//Intermediate hash containing the signedinfo external file
		EVP_MD_CTX *m_digest_state;
		APL_Card *mp_card;
	};
}

#endif

