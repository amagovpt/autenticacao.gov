#ifndef XADESSIG_H
#define XADESSIG_H

#include <xercesc/util/PlatformUtils.hpp>
#include <xsec/dsig/DSIGSignature.hpp>

#include <string>
#include <openssl/evp.h>
#include <openssl/x509.h>

#include "Export.h" 
#include "APLCard.h"
//#include "cryptoFwkPteid.h"

namespace eIDMW
{
	class CByteArray;
	//class APL_CryptoFwkPteid;
	class XadesSignature
	{
		public:

		EIDMW_APL_API XadesSignature(APL_Card &card): mp_card(card), mp_cert(NULL)
		{
//			mp_crypto = new APL_CryptoFwkPteid();
		};
		
		EIDMW_APL_API ~XadesSignature()
		{
		X509_free(mp_cert);

		}

		EIDMW_APL_API std::string& SignXades(CByteArray ba, const char *URL);
		EIDMW_APL_API std::string& SignXadesT(CByteArray ba, const char *URL);
		EIDMW_APL_API std::string& SignXades(char ** List_of_paths, unsigned int n_paths);
		EIDMW_APL_API std::string& SignXadesT(char ** List_of_paths, unsigned int n_paths);
		

		private:

		CByteArray HashFile(const char *file_path);
		
		//Utility methods	
		void loadCert(CByteArray &ba, EVP_PKEY *pub_key);

		int appendOID(XMLByte *toFill);

		void addTimestampNode(DSIGSignature *sig);
		
		X509 * mp_cert;
		//APL_CryptoFwk *mp_crypto;
		APL_Card & mp_card;

	};
}

#endif 

