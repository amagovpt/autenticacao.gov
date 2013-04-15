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

	class XadesSignature
	{
		public:

		XadesSignature(APL_Card *card): mp_card(card), mp_cert(NULL) {};
		
		~XadesSignature()
		{
			if (mp_cert != NULL)
				X509_free(mp_cert);
		};

		CByteArray &SignXades(CByteArray ba, const char *URL);
		CByteArray &SignXades(const char ** paths, unsigned int n_paths, bool do_timestamp);

		CByteArray mp_timestamp_data;
		private:

		static void initXMLUtils();
		//static void terminateXMLUtils();

		CByteArray HashFile(const char *file_path);
		DOMNode * addSignatureProperties(DSIGSignature *sig);
		CByteArray *WriteToByteArray(XERCES_NS DOMDocument *doc); 

		//Utility methods for signature
		void loadCert(CByteArray &ba, EVP_PKEY *pub_key);
		int appendOID(XMLByte *toFill);
		void addTimestampNode(XERCES_NS DOMNode *node, unsigned char *timestamp);
		XMLCh* createURI(const char *path);

		X509 * mp_cert;
		APL_Card *mp_card;
	};
}

#endif 

