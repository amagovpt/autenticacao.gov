/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2009 FedICT.
 *
 * This is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License version
 * 3.0 as published by the Free Software Foundation.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this software; if not, see
 * http://www.gnu.org/licenses/.

**************************************************************************** */
#pragma once

#ifndef __CRYPTOFRAMEWORK_H__
#define __CRYPTOFRAMEWORK_H__

#include <memory>
#include <string>
#include <vector>
#include "Mutex.h"


// Mutex.h includes windows.h and that conflicts with the openssl defines
#if defined(__WINCRYPT_H__)
#undef X509_NAME
#undef X509_EXTENSIONS
#undef X509_CERT_PAIR
#undef PKCS7_ISSUER_AND_SERIAL
#undef OCSP_REQUEST
#undef OCSP_RESPONSE
#endif
#include "APLCard.h"

#include "openssl/evp.h"
#include "openssl/ocsp.h"
#include "openssl/ssl.h"
#include "openssl/bio.h"

namespace eIDMW
{

	class APL_Certif;

enum FWK_CertifStatus
{
	FWK_CERTIF_STATUS_UNCHECK,		/**< Validity uncheck yet */
	FWK_CERTIF_STATUS_VALID,		/**< Valid certificate */
	FWK_CERTIF_STATUS_REVOKED,		/**< Revoked certificate */
	FWK_CERTIF_STATUS_CONNECT,		/**< Connection problem */
	FWK_CERTIF_STATUS_ERROR,		/**< Error during validation */
	FWK_CERTIF_STATUS_UNKNOWN,		/**< Certificate unknown by responder */
	FWK_CERTIF_STATUS_SUSPENDED     /* Certificate on hold */

};

struct tCertifInfo
{
	std::string serialNumber;			/**< Serial number of the certificate */
	std::string ownerName;				/**< Name of the certificate owner */
	std::string issuerName;				/**< Name of the certificate issuer */
	std::string validityNotBefore;		/**< Begin date validity */
	std::string validityNotAfter;		/**< End date validity */
	unsigned long keyLength;			/**< Length of the key */
};

struct tCrlInfo
{
	std::string issuerName;				/**< Name of the certificate issuer */
	std::string validityLastUpdate;		/**< Last update date */
	std::string validityNextUpdate;		/**< Next update date */
};

enum FWK_HashAlgo {
	FWK_ALGO_MD5,      // 16-byte hash
	FWK_ALGO_SHA1,     // 20-byte hash
	FWK_ALGO_SHA256
};

class CByteArray;

struct tOcspCertID
{
	FWK_HashAlgo hashAlgorithm;
	const CByteArray *issuerNameHash;
	const CByteArray *issuerKeyHash;
	const CByteArray *serialNumber;
};

class CrlMemoryCache;

void loadWindowsRootCertificates(X509_STORE *store);

/******************************************************************************//**
  * Abstract class for cryptographic features 
  *
  * The goal of this class is to provide facilities to openSSL usage
  * This is only for internal use, no export is foreseen. 
  *
  *********************************************************************************/
class APL_CryptoFwk
{
public:
	/**
	  * Destructor
	  */
	virtual ~APL_CryptoFwk(void)=0;

	/**
	  * Verify if the cert is the correct Pteid root
	  */
	virtual bool VerifyRoot(const CByteArray &cert)=0;

	/**
	  * Return a unique ID from a certificate
	  */
	unsigned long GetCertUniqueID(const CByteArray &cert);

	/**
	  * Verify the validity date of the certificate
	  */
	bool VerifyDateValidity(const CByteArray &cert);

	/**
	  * Verify the validity date of the crl
	  */
	bool VerifyCrlDateValidity(const CByteArray &crl);

	/**
	  * Check if the certificate is self-issuer (Root)
	  * 
	  * - First check the issuer name
	  * - Then verify the signature (must be self-signed)
	  */
	bool isSelfIssuer(const CByteArray &cert);

	/**
	  * Check if the certificate (cert) has the issuer (issuer)
	  * 
	  * - First check if issuer name correspond
	  * - Then verify the signature
	  */
	bool isIssuer(const CByteArray &cert,const CByteArray &issuer);

	/**
	  * Check if the crl has the issuer (issuer)
	  * 
	  * - First check if issuer name correspond
	  * - Then verify the signature
	  */
	bool isCrlIssuer(const CByteArray &crl,const CByteArray &issuer);

	/**
	  * Check if the crl is valid
	  * 
	  * - First check if the isssuer is ok
	  * - Then verify the validity date
	  */
	bool isCrlValid(const CByteArray &crl,const CByteArray &issuer);

	/**
	  * Verify if the data has the correct hash
	  */
	bool VerifyHash(const CByteArray &data, const CByteArray &hash, FWK_HashAlgo algorithm);

	/**
	  * Verify if the data has the correct hash (md5 algorithm)
	  */
	bool VerifyHashMd5(const CByteArray &data, const CByteArray &hash);

	/**
	  * Verify if the data has the correct hash (sha1 algorithm)
	  */
	bool VerifyHashSha1(const CByteArray &data, const CByteArray &hash);

	/**
	 * Verify if the data has the correct hash (sha256 algorithm)
	 */
	bool VerifyHashSha256(const CByteArray &data, const CByteArray &hash);

	/**
	  * Get the hash of the data
	  */
	bool GetHash(const CByteArray &data, FWK_HashAlgo algorithm, CByteArray *hash);

	/**
	  * Get the hash of the data (md5 algorithm)
	  */
	bool GetHashMd5(const CByteArray &data, CByteArray *hash);

	/**
	  * Get the hash of the data (sha1 algorithm)
	  */
	bool GetHashSha1(const CByteArray &data, CByteArray *hash);

	/**
	  * Verify if the signature of the data (sha1 algorithm)
	  */
	bool VerifySignatureSha1(const CByteArray &data, const CByteArray &signature, const CByteArray &cert);

	/**
	  * Validate the certificate through CRL process
	  */
	FWK_CertifStatus CRLValidation(const CByteArray &cert,const CByteArray &crl);

	/**
	  * Validate the certificate through OCSP process
	  *
	  * @return true if Validation OK
	  * @return false if Revoked or Unknown
	  */
	FWK_CertifStatus OCSPValidation(const CByteArray &cert, const CByteArray &issuer, CByteArray *response=NULL);

	/**
	  * Send a OCSP request and get the response
	  *
	  * Throw exception if something unwanted append (no connection...)
	  *
	  * @return The status
	  */
	FWK_CertifStatus GetOCSPResponse(const CByteArray &cert, const CByteArray &issuer, CByteArray *response);

	/**
	  * Send a OCSP request and get the response
	  * If issuer is not NULL, the verification of the response is done
	  * Throw exception if something unwanted append (no connection...)
	  * @return The status
	  */
	FWK_CertifStatus GetOCSPResponse(const char *pUrlResponder,const tOcspCertID &certid, CByteArray *response,const CByteArray *issuer=NULL);

	/**
	  * Return the Url of the OCSP responder
	  * @return true if OCSP responder found
	  */
	bool GetOCSPUrl(const CByteArray &cert, std::string &url);

	/**
	  * Return the url of CRL distribution point
	  * @return true if CDP found
	  */
	bool GetCDPUrl(const CByteArray &cert, std::string &url);

	/**
	  * Return the Url of the OCSP responder
	  * The char * buffer is created and must be destroy by the caller
	  */
	char *GetOCSPUrl(X509 *pX509_Cert);

	/**
	  * Return the CRL distribution point Url
	  * The char * buffer is created and must be destroy by the caller
	  */
	char *GetCDPUrl(X509 *pX509_Cert);


	/**
	  * Fill the info structure with value from certificate
	  *
	  * @return true if everything ok, false otherwise
	  */
	bool getCertInfo(const CByteArray &cert, tCertifInfo &info, const char *dateFormat="%d/%m/%Y");

	/**
	  * Return the validitydate of a CRL
	  *
	  * @return true if the file is up to date
	  */
	bool getCrlInfo(const CByteArray &crl,tCrlInfo &info, const char *dateFormat);

	/**
	  * Encode the byte array in b64
	  *
	  * @return true if the operation succeed
	  */
	bool b64Encode(const CByteArray &baIn, CByteArray &baOut,bool bWithLineFeed=true);

	/**
	  * Decode the byte array in b64
	  *
	  * @return true if the operation succeed
	  */
	bool b64Decode(const CByteArray &baIn, CByteArray &baOut);

	/**
	  * Reset the proxy parameters
	  */
	void resetProxy();


	void setActiveCard(APL_SmartCard *card) 
	{
		m_card = card;
	}

protected:
	/**
	  * Constructor - used within "instance"
	  */
	APL_CryptoFwk();

	/**
	  * Parse url to take proxy parameter into account
	  */
	int ParseUrl(char *url, char **phost, char **pport, char **ppath, int *pssl);

	/**
	  * Convert digest algorithm
	  */
	const EVP_MD *ConvertAlgorithm(FWK_HashAlgo algo);

	/**
	  * Verify if the data has the correct hash
	  */
	bool VerifyHash(const CByteArray &data, const CByteArray &hash, const EVP_MD *algorithm);

	/**
	  * Get the hash of the data
	  */
	bool GetHash(const CByteArray &data, const EVP_MD *algorithm, CByteArray *hash);

	/**
	  * Verify if the certificate is signed by the issuer
	  *
	  * @param pX509_Cert : The certificate to check
	  * @param pX509_Issuer : The certificate issuer
	  */
	bool VerifyCertSignature(X509 *pX509_Cert,X509 *pX509_Issuer);

	/**
	  * Verify if the crl is signed by the issuer
	  *
	  * @param pX509_Crl : The CRL to check
	  * @param pX509_Issuer : The certificate issuer
	  */
	bool VerifyCrlSignature(X509_CRL *pX509_Crl,X509 *pX509_Issuer);

	/**
	  * Verify if the signature of the data
	  */
	bool VerifySignature(const CByteArray &data, const CByteArray &signature, X509 *pX509, const EVP_MD *algorithm);

	/**
	  * Verify if the cert is the correct root
	  */
	bool VerifyRoot(const CByteArray &cert, const unsigned char *const *roots);

	/**
	  * Send a OCSP request and get the response
	  *
	  * @return The status
	  */
	FWK_CertifStatus GetOCSPResponse(X509 *pX509_Cert,X509 *pX509_Issuer, OCSP_RESPONSE **pResponse); 

	/**
	  * Send a OCSP request and get the response
	  * If pX509_Issuer is not null the signature of the response is verify
	  * @return The status
	  */
	FWK_CertifStatus GetOCSPResponse(const char *pUrlResponder,OCSP_CERTID *pCertID, OCSP_RESPONSE **pResponse,X509 *pX509_Issuer=NULL);

	/**
	  * Make a connection to an internet service
	  */
	BIO *Connect(char *pszHost, int iPort, int iSSL, SSL_CTX **ppSSLCtx); 

	/**
	  * Convert ASN1_TIME into string
	  */
	void TimeToString(ASN1_TIME *asn1Time, std::string &strTime, const char *format);

	/**
	  * Convert ASN1_GENERALIZEDTIME into struct tm
	  */
	void GeneralTimeToString(ASN1_GENERALIZEDTIME *asn1Time, struct tm &timeinfo);

	/**
	  * Convert ASN1_GENERALIZEDTIME into char*
	  */
	void GeneralTimeToBuffer(ASN1_GENERALIZEDTIME *asn1Time, char* buffer,size_t bufferSize);

	/**
	  * Convert ASN1_UTCTIME into struct tm
	  */
	void UtcTimeToString(ASN1_UTCTIME *asn1Time, struct tm &timeinfo);


	/**
	  * Convert the bytearray into X509_CRL
	  * Create the X509_CRL object that MUST BE destroyed by the caller
	  */
	X509_CRL *getX509CRL(const CByteArray &crl);

	/**
	  * Verify the validity date of the certificate
	  */
	bool VerifyDateValidity(const X509 *pX509);

	/**
	  * Verify the validity date of the crl
	  */
	bool VerifyCrlDateValidity(const X509_CRL *pX509_Crl);

	/**
	  * Check if the crl has the issuer (issuer)
	  * 
	  * - First check if issuer name correspond
	  * - Then verify the signature
	  */
	bool isCrlIssuer(X509_CRL *pX509_Crl,X509 *pX509_issuer);

	APL_SmartCard *m_card;

	std::string m_proxy_host;	/**< proxy host */
	std::string m_proxy_port;	/**< proxy port */
	std::string m_proxy_pac;	/**< proxy pac file */

private:
	APL_CryptoFwk(const APL_CryptoFwk& cryptofwk);				/**< Copy not allowed - not implemented */
	APL_CryptoFwk& operator= (const APL_CryptoFwk& cryptofwk);	/**< Copy not allowed - not implemented */

	CrlMemoryCache *m_CrlMemoryCache;
};

}

#endif // __CRYPTOFRAMEWORK_H__
