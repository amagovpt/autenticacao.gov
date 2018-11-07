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

#ifndef __CERTIF_H__
#define __CERTIF_H__

#include <string>
#include <map>
#include <vector>

#include "Export.h"
#include "P15Objects.h"
#include "ByteArray.h"
#include "APLCard.h"
#include "APLCardFile.h"
#include "APLCrypto.h"
#include <limits.h>

namespace eIDMW
{

#define ANY_INDEX ULONG_MAX

enum APL_CertifType
{
	APL_CERTIF_TYPE_UNKNOWN,		/**< Type unknown */
	APL_CERTIF_TYPE_ROOT,			/**< Root certificate */
	APL_CERTIF_TYPE_ROOT_AUTH,		/**< CA certificate */
	APL_CERTIF_TYPE_ROOT_SIGN,		/**< CA certificate */
	APL_CERTIF_TYPE_AUTHENTICATION,	/**< Authentication certificate */
	APL_CERTIF_TYPE_SIGNATURE,		/**< Signature certificate */
};

enum APL_CertifStatus
{
	APL_CERTIF_STATUS_UNCHECK,		/**< Validity not check yet */
	APL_CERTIF_STATUS_VALID,		/**< Valid certificate */
	APL_CERTIF_STATUS_VALID_CRL,	/**< Valid certificate through CRL process */
	APL_CERTIF_STATUS_VALID_OCSP,	/**< Valid certificate through OCSP process */
	APL_CERTIF_STATUS_REVOKED,		/**< Revoked certificate */
	APL_CERTIF_STATUS_SUSPENDED,    /**< Suspended certificate */
	APL_CERTIF_STATUS_CONNECT,		/**< Connection problem */
	APL_CERTIF_STATUS_ISSUER,		/**< An issuer is missing in the chain */
	APL_CERTIF_STATUS_ERROR,		/**< Error during validation */
	APL_CERTIF_STATUS_EXPIRED,      /**< Expired Certificate */
	APL_CERTIF_STATUS_UNKNOWN		/**< Validity unknown */
};

enum APL_CrlStatus
{
	APL_CRL_STATUS_UNKNOWN,			/**< Validity unknown */
	APL_CRL_STATUS_VALID,			/**< Valid Crl */
	APL_CRL_STATUS_CONNECT,			/**< Connection problem */
	APL_CRL_STATUS_ERROR			/**< Error during validation */
};

enum APL_ValidationProcess
{
	APL_VALIDATION_PROCESS_NONE=0,
	APL_VALIDATION_PROCESS_CRL,
	APL_VALIDATION_PROCESS_OCSP
};

enum APL_ValidationLevel
{
	APL_VALIDATION_LEVEL_NONE=0,
	APL_VALIDATION_LEVEL_OPTIONAL,
	APL_VALIDATION_LEVEL_MANDATORY
};

enum APL_HashAlgo {
	APL_ALGO_MD5,      // 16-byte hash
	APL_ALGO_SHA1      // 20-byte hash
};

class APL_CryptoFwk;
class APL_CardFile_Certificate;
class APL_Certif;
class APL_OcspResponse;


/******************************************************************************//**
  * Class that represents a certificates store
  * 
  * The store is linked to a smart card
  * It contains the P15 certificates that are on this card
  * and can contain other certificates
  *
  * To get a APL_Certifs object, we have to ask it from the APL_SmartCard 
  *********************************************************************************/
class APL_Certifs : public APL_Crypto
{
public:
	/**
	  * Create an APL_Certifs store without any link to a card
	  * This store is not link to any Card so some methods could not be used
	  * these methods throw EIDMW_ERR_BAD_USAGE exception
	  */
	EIDMW_APL_API APL_Certifs();				

	/**
	  * Destructor
	  *
	  * Destroy all the certificate hold in m_certifsAll
	  */
	EIDMW_APL_API virtual ~APL_Certifs(void);

	EIDMW_APL_API virtual bool isAllowed();							/**< The document is allowed*/


	EIDMW_APL_API virtual CByteArray getXML(bool bNoHeader=false);	
	EIDMW_APL_API virtual CByteArray getCSV();						
	EIDMW_APL_API virtual CByteArray getTLV();						

	/**
	  * Return the number of P15 certificates on the card
	  */
	EIDMW_APL_API unsigned long countFromCard();

	/**
	  * Return the number of certificates in the store (P15 on the card or not)
	  */
	EIDMW_APL_API unsigned long countAll();

	/**
	  * Return the certificate with the number ulIndexCard from the card
	  *
	  * If the APL_Certif object does not yet exist, 
	  * it is create and add to both maps m_certifsOnCard and m_certifsAll
	  */
	EIDMW_APL_API APL_Certif *getCertFromCard(unsigned long ulIndexCard);

	/**
	  * Return the certificate with the number ulIndexAll
	  *
	  * First load all the certificates from the card 
	  * Then return the certificate with index ulIndexAll
	  * ATTENTION ulIndexAll and ulIndexCard are two different index)
	  */
	EIDMW_APL_API APL_Certif *getCert(unsigned long ulIndex);
	
	/**
	  * Add a certificate to the store
	  *
	  * If the unique ID is not yet in the map m_certifsAll 
	  * it is create and add to this map only
	  */
	EIDMW_APL_API APL_Certif *addCert(const CByteArray &cert,APL_CertifType type=APL_CERTIF_TYPE_UNKNOWN,bool bHidden=false);

	/**
	  * Return the certificate from its unique ID
	  *
	  * If it is not yet in the map (m_certifsAll), we throw an exception
	  */
	EIDMW_APL_API APL_Certif *getCertUniqueId(unsigned long ulUniqueId);

	/**
	  * Return the number of certificates by type in the store
	  */
	EIDMW_APL_API unsigned long countCert(APL_CertifType type);

	/**
	  * Return the certificate by type in the store
	  *
	  * If no certificate is found, we throw an exception
	  */
	EIDMW_APL_API APL_Certif *getCert(APL_CertifType type,unsigned long ulIndex=ANY_INDEX);

	/**
	  * Return the number of root certificates in the store
	  */
	EIDMW_APL_API unsigned long countRoot();

	/**
	  * Return the root certificate in the store
	  *
	  * If no root is found, we throw an exception
	  */
	EIDMW_APL_API APL_Certif *getRoot(unsigned long ulIndex=ANY_INDEX);

	/**
	  *	 Methods related to SOD CA certificates
	 
	  */

	EIDMW_APL_API void initSODCAs();

	EIDMW_APL_API void clearSODCAs();

	EIDMW_APL_API void addToSODCAs(const CByteArray &cert);


	unsigned long countSODCAs();

	APL_Certif * getSODCA(int index);

	/**
	  * Return the number of authentication certificates in the store
	  */
	EIDMW_APL_API unsigned long countAuthentication();

	/**
	  * Return the authentication certificate in the store
	  *
	  * If no authentication certificate is found, we throw an exception
	  */
	EIDMW_APL_API APL_Certif *getAuthentication(unsigned long ulIndex=ANY_INDEX);

	/**
	  * Return the number of signature certificates in the store
	  */
	EIDMW_APL_API unsigned long countSignature();

	/**
	  * Return the signature certificate in the store
	  *
	  * If no signature certificate is found, we throw an exception
	  */
	EIDMW_APL_API APL_Certif *getSignature(unsigned long ulIndex=ANY_INDEX);

	/**
	  * Return the number of CA certificates in the store
	  */
	EIDMW_APL_API unsigned long countCA();

	/**
	  * Return the Signature CA certificate in the store
	  *
	  * If no CA certificate is found, we throw an exception
	  */
	EIDMW_APL_API APL_Certif *getSignatureSubCA();

	EIDMW_APL_API APL_Certif *getAuthenticationSubCA();



	/**
	  * Return the number of children for cert in the store
	  */
	EIDMW_APL_API unsigned long countChildren(const APL_Certif *cert);

	/**
	  * Return the ulIndex'th children for cert (first is 0)
	  *
	  * If out of range, we throw an exception
	  */
	EIDMW_APL_API APL_Certif *getChildren(const APL_Certif *cert,unsigned long ulIndex);

	/**
	  * Loop through m_certifs to find the issuer of the certificate
	  *
	  * If no issuer is found, NULL is return
	  */
	EIDMW_APL_API APL_Certif *findIssuer(const APL_Certif *cert);

	/**
	  * Loop through m_certifs to find the issuer of the certificate
	  *
	  * If no issuer is found, NULL is return
	  */
	EIDMW_APL_API APL_Certif *findIssuer(const CByteArray *data);

	/**
	  * Loop through m_certifs to find the issuer of the crl
	  *
	  * If no issuer is found, NULL is return
	  */
	EIDMW_APL_API APL_Certif *findCrlIssuer(const CByteArray &crldata);


	void resetFlags();	/**< Reset flags (issuer, root and test) in the certifactes from the store */

	/**
	  * Add a certificate to the store for internal use (virtual reader, RRn)
	  *
	  * No need to export
	  */
	APL_Certif *addCert(APL_CardFile_Certificate *file,APL_CertifType type,bool bOnCard,bool bHidden,unsigned long ulIndex,const CByteArray *cert,const CByteArray *cert_tlv_struct);

	/**
	  * Return the card from where the store comes (could be NULL)
	  *
	  * No need to export
	  */
	APL_SmartCard *getCard();

protected:
	/**
	  * Constructor
	  *
	  * @param card is the smart card to which the store is linked
	  */
	APL_Certifs(APL_SmartCard *card);

private:
	APL_Certifs(const APL_Certifs& certstore);				/**< Copy not allowed - not implemented */
	APL_Certifs &operator= (const APL_Certifs& certstore);	/**< Copy not allowed - not implemented */

	void loadCard();										/**< Load all the certificates from the card */
	void loadFromFile();									/**< Load all the certificates from the certificates directory */
	void init(APL_SmartCard *card);

	void resetIssuers(); /**< Reset issuer pointer in the certifactes from the store */
	void resetRoots();	/**< Reset root flag in the certifactes from the store */
	void resetTests();	/**< Reset test flag in the certifactes from the store */

	void initMyCerts();

	/**
	 * This is a callback function for the scanDir
	 *
	 * @param SubDir is the sub directory relative the the Dir of scanDir
	 * @param File is the file to threat
	 * @param param must be this pointer
	 */
	static void foundCertificate(const char *SubDir, const char *File, void *param);

	APL_SmartCard *m_card;									/**< The smart card from which some certificates come */
	APL_CryptoFwk *m_cryptoFwk;								/**< Pointer to the crypto framework */

	/**
	  * Hold all the APL_Certif pointers in the store
	  *		coming from the card or not
	  *
	  * The index is the certificate unique ID
	  */
	bool defaultSODCertifs;

	std::vector <APL_Certif *> m_sod_cas;
	std::map<unsigned long,APL_Certif *> m_certifs;
	std::vector<unsigned long> m_certifsOrder;
	std::vector<APL_Certif *> my_certifs;

	std::string m_certExtension;
	std::string m_certs_dir;

friend APL_Certifs *APL_SmartCard::getCertificates();	/**< This method must access protected constructor */
};

class APL_CertStatusCache;
class APL_OcspResponse;
class APL_Crl;
struct tCertifInfo;


/******************************************************************************//**
  * Class that represents one certificate
  * 
  * Can be constructed from a PKSC15 object from a smart card
  * or from a certificates strore and a file.
  *
  * To get APL_Certif object, we have to ask it from APL_Certifs 
  *********************************************************************************/
class APL_Certif : public APL_Crypto
{

public:
	/**
	  * Destructor
	  *
	  * Destroy m_certFile if it has been created in the constructor
	  */
	EIDMW_APL_API virtual ~APL_Certif(void);

	EIDMW_APL_API virtual bool isAllowed();							/**< The document is allowed*/

	EIDMW_APL_API virtual CByteArray getXML(bool bNoHeader=false);	/**< Build the XML document */
	EIDMW_APL_API virtual CByteArray getCSV();			/**< Build the CSV document */
	EIDMW_APL_API virtual CByteArray getTLV();			/**< Build the TLV document */

	EIDMW_APL_API unsigned long getIndexOnCard() const;	/**< Return Index off the certificate on card */
	EIDMW_APL_API const char *getLabel();			/**< Return P15 Label */
	EIDMW_APL_API unsigned long getID() const;		/**< Return P15 ID */
	EIDMW_APL_API unsigned long getUniqueId() const;	/**< Return the unique ID */
	EIDMW_APL_API const CByteArray &getData() const;	/**< Return content of certificate file */
	EIDMW_APL_API void getFormattedData(CByteArray &data) const;	/**< Return content of certificate file without ending zero */
	EIDMW_APL_API APL_CertifType getType();			/**< Return the type of the certificate */

	/**
	  * Return the certificate issuer (NULL if not found)
	  *
	  * The certificate comes from the store
	  */
	EIDMW_APL_API APL_Certif *getIssuer();

	/**
	  * Return the root certificate at the end of the chain
	  */
	EIDMW_APL_API APL_Certif *getRoot();

	/**
	  * Return the number of children for this certificate;
	  */
	EIDMW_APL_API unsigned long countChildren(bool bForceRecount=false);

	/**
	  * Return the children certificate (certificate that has been issued by this one)
	  *
	  * @param ulIndex is the children index (the index for the first child is 0)
	  * @return if ulIndex is out of range, NULL is returned
	  *	Throw an exception EIDMW_ERR_PARAM_RANGE if index out of range
	  */
	EIDMW_APL_API APL_Certif *getChildren(unsigned long ulIndex);

	/**
	  * Return the test status
	  *
	  * @return true if test certificate
	  * @return false if good one
	  */
	EIDMW_APL_API bool isTest();

	/**
	  * @return true if the certificate is of that type
	  * @return false otherwise
	  */
	EIDMW_APL_API bool isType(APL_CertifType type);

	/**
	  * Return the root status
	  *
	  * @return true if it is a root certificate
	  * @return false otherwise
	  */
	EIDMW_APL_API bool isRoot();

	/**
	  * Return the authentication status
	  *
	  * @return true if it is a authentication certificate
	  * @return false otherwise
	  */
	EIDMW_APL_API bool isAuthentication();

	/**
	  * Return the signature status
	  *
	  * @return true if it is a signature certificate
	  * @return false otherwise
	  */
	EIDMW_APL_API bool isSignature();

	/**
	  * Return the CA status
	  *
	  * @return true if it is a CA certificate
	  * @return false otherwise
	  */
	EIDMW_APL_API bool isCA();

	/**
	  * Check if the certificate chain end by the one of pteid root;
	  *
	  * @return true if root ok
	  * @return false otherwise
	  */
	EIDMW_APL_API bool isFromPteidValidChain();

	/**
	  * Return the status of the certificate file
	  */
	EIDMW_APL_API tCardFileStatus getFileStatus();

	/**
	  * Return the status of the certificate
	  *
	  * Get the parameter crl/ocsp from config and call getStatus(bool crl, bool ocsp)
	  */
	EIDMW_APL_API APL_CertifStatus getStatus();

	/**
	  * Return the status of the certificate
	  *
	  * Call the CertStatusCache for finding or calculating this status
	  */
	EIDMW_APL_API APL_CertifStatus getStatus(APL_ValidationLevel crl, APL_ValidationLevel ocsp);

	/**
	  * Get the CRL from this certificate
	  */
	EIDMW_APL_API APL_Crl *getCRL();

	/**
	  * Get the OCSPResponder object from this certificate
	  */
	EIDMW_APL_API APL_OcspResponse *getOcspResponse();

	/**
	  * Make the CRL validation
	  */
	EIDMW_APL_API APL_CertifStatus verifyCRL(bool forceDownload=false);

	/**
	  * Make the OCSP validation
	  */
	EIDMW_APL_API APL_CertifStatus verifyOCSP();

	/**
	  * Return the OCSP Responce
	  */
	EIDMW_APL_API CByteArray getOCSPResponse();

	EIDMW_APL_API const char *getSerialNumber();		/**< Return the serial number */
	EIDMW_APL_API const char *getOwnerName();			/**< Return the name of the owner */
	EIDMW_APL_API const char *getIssuerName();			/**< Return the name of the issuer */
	EIDMW_APL_API const char *getValidityBegin();		/**< Return validity begin date */
	EIDMW_APL_API const char *getValidityEnd();			/**< Return validity end date */
	EIDMW_APL_API unsigned long getKeyLength();			/**< Return the length of public/private key */
	
	EIDMW_APL_API bool isHidden();						/**< This is an hidden certificate */
	EIDMW_APL_API bool isFromCard();						/**< This certificate comes from the card */

	EIDMW_APL_API APL_Certifs *getCertificates();		/**< Return a pointer to the store */

	APL_CertifStatus validationCRL();	/**< Make the CRL validation (not exported, internal used from APL_CertStatusCache::checkCertValidation) */
	APL_CertifStatus validationOCSP();	/**< Make the OCSP validation (not exported, internal used from APL_CertStatusCache::checkCertValidation) */

	void resetIssuer();	/**< reset m_issuer pointer */
	void resetRoot();	/**< reset m_root flag */
	void resetTest();	/**< reset m_test flag */

protected:

	/**
	  * Constructor
	  *
	  * @param card is the smartcard where to read the file
	  * @param ulIndex is the index of the P15 object
	  */
	APL_Certif(APL_SmartCard *card,APL_Certifs *store,unsigned long ulIndex);

	/**
	  * Constructor
	  *
	  * @param store is the store in which the APL_Certif object is hold
	  * @param file is the certificate file
	  */
	APL_Certif(APL_Certifs *store,APL_CardFile_Certificate *file,APL_CertifType type,bool bOnCard,bool bHidden,unsigned long ulIndex,const CByteArray *cert,const CByteArray *cert_tlv_struct);

	/**
	  * Constructor
	  *
	  * @param store is the store in which the APL_Certif object is hold
	  * @param cert is the certificate
	  */
	APL_Certif(APL_Certifs *store,const CByteArray &cert,APL_CertifType type,bool bHidden);

private:
	APL_Certif(const APL_Certif& certif);				/**< Copy not allowed - not implemented */
	APL_Certif &operator= (const APL_Certif& certif);	/**< Copy not allowed - not implemented */

	/**
	  * Init m_info
	  */
	void initInfo();

	CByteArray getP15TLV();						/**< Return the P15 struct in a TLV CByteArray */
	void setP15TLV(const CByteArray *bytearray);/**< Fill the P15 struct with the TLV CByteArray */

	unsigned long m_ulIndex;							/**< Index of the P15 object on the card */

	tCert m_certP15;									/**< P15 structure load at construct time */
	bool m_certP15Ok;									/**< P15 structure load at construct time */
	APL_CardFile_Certificate *m_certFile;				/**< The certificate file */
	bool m_delCertFile;									/**< Mean that m_certFile must be deleted in destructor */

	APL_CryptoFwk *m_cryptoFwk;							/**< Pointer to the crypto framework */
	APL_CertStatusCache *m_statusCache;					/**< Pointer to the status cache */

	APL_Crl *m_crl;										/**< The crl link to the certificate */
	APL_OcspResponse *m_ocsp;							/**< The ocsp link to the certificate */

	bool m_hidden;										/**< This is an hidden certificate */
	bool m_onCard;										/**< This certificate comes from the card */

	int m_test;											/**< Test certificate : -1 = unknown, 1 = test, 0 = good certificate */
	int m_root;											/**< Root certificate : -1 = unknown, 1 = root, 0 = non-root */
	APL_Certif *m_issuer;								/**< Link to the certificate issuer */

	APL_Certifs *m_store;								/**< The store in which the APL_Certif object is hold */

	bool m_initInfo;									/**< True when issuer, root and test has been set */
	tCertifInfo *m_info;								/**< Divers information about the certificate */

	unsigned long m_countChildren;						/**< The number of children */

	APL_CertifType m_type;

friend APL_Certif *APL_Certifs::getCertFromCard(unsigned long ulIndex);				/**< This method must access protected constructor */
friend APL_Certif *APL_Certifs::addCert(APL_CardFile_Certificate *file,APL_CertifType type,bool bOnCard,bool bHidden,unsigned long ulIndex,const CByteArray *cert,const CByteArray *cert_tlv_struct);	/**< This method must access protected constructor */
friend APL_Certif *APL_Certifs::addCert(const CByteArray &cert,APL_CertifType type,bool bHidden);		/**< This method must access protected constructor */
friend void APL_Certifs::addToSODCAs(const CByteArray &cert);
};


class APL_CrlDownloadingCache;
struct tCrlInfo;

/******************************************************************************//**
  * Class that represents one crl
  * 
  * To get APL_Crl object, we have to ask it from APL_Certif 
  *********************************************************************************/
class APL_Crl
{
public:
	/**
	  * Create an APL_Crl object from the URI only
	  * As there is no issuer, this CRL can't be verify and some method are not allowed
	  * (ex. getIssuer)
	  * Theses methods throw EIDMW_ERR_BAD_USAGE exception
	  */
	EIDMW_APL_API APL_Crl(const char *uri);

	EIDMW_APL_API virtual ~APL_Crl(void);		/**< Destructor */

	/**
	  * Verify certificate trough CRL process
	  */
	EIDMW_APL_API APL_CertifStatus verifyCert(bool forceDownload=false);

	/**
	  * Return the CRL 
	  * If it commes from a Certif we verify the signing
	  * If it's created from the URL only the signing is not verify
	  * @return the status of the crl
	  */
	EIDMW_APL_API APL_CrlStatus getData(CByteArray &data,bool forceDownload=false);

	/**
	  * Return the certificate issuer (NULL if not found)
	  *
	  * The certificate comes from the store
	  */
	EIDMW_APL_API APL_Certif *getIssuer();

	EIDMW_APL_API const char *getIssuerName();	/**< Return the name of the issuer */

	EIDMW_APL_API const char *getUri();			/**< Return the uri of the CRL */

private:
	APL_Crl(const char *uri,APL_Certif *certif);/**< Constructor */

	APL_Crl(const APL_Crl& crl);				/**< Copy not allowed - not implemented */
	APL_Crl &operator= (const APL_Crl& crl);	/**< Copy not allowed - not implemented */

	/**
	  * Init m_info,  m_issuer and verify the signature
	  */
	void init();

	std::string m_uri;							/**< The url from where the crl comes */

	bool m_initOk;								/**< True when init has been done */
	APL_Certif *m_certif;						/**< Link to the certificate */
	APL_Certif *m_issuer;						/**< Link to the issuer certificate */
	tCrlInfo *m_info;							/**< Divers information about the crl */

	APL_CrlDownloadingCache *m_cache;
	APL_CryptoFwk *m_cryptoFwk;
	CMutex m_Mutex;									/**< Mutex */

friend APL_Crl *APL_Certif::getCRL();		/**< This method must access protected constructor */
};

struct tOcspCertID;

/******************************************************************************//**
  * Class that represents an OCSP response
  * 
  * To get APL_OcspResponse object, we have to ask it from APL_Certif 
  *********************************************************************************/
class APL_OcspResponse
{
public:
	/**
	  * Create an APL_OcspResponse object from the URI only and CertID
	  * This OCSP Response is not link to any APL_Certif so some methods could not be used
	  * these methods throw EIDMW_ERR_BAD_USAGE exception
	  */
	EIDMW_APL_API APL_OcspResponse(const char *uri,APL_HashAlgo hashAlgorithm,const CByteArray &issuerNameHash,const CByteArray &issuerKeyHash,const CByteArray &serialNumber);

	EIDMW_APL_API virtual ~APL_OcspResponse(void);		/**< Destructor */

	/**
	  * Verify certificate trough OCSP process
	  */
	EIDMW_APL_API APL_CertifStatus verifyCert();

	/**
	  * Get the OCSP Response
	  */
	EIDMW_APL_API APL_CertifStatus getResponse(CByteArray &response);

	EIDMW_APL_API const char *getUri();			/**< Return the uri of the responder */

private:
	APL_OcspResponse(const char *uri,APL_Certif *certif);/**< Constructor */

	APL_OcspResponse(const APL_OcspResponse& ocsp);				/**< Copy not allowed - not implemented */
	APL_OcspResponse &operator= (const APL_OcspResponse& pcsp);	/**< Copy not allowed - not implemented */

	APL_CertifStatus getResponse(CByteArray *response);	/**< Get the OCSP Response */

	std::string m_uri;							/**< The url from where the crl comes */

	APL_Certif *m_certif;						/**< Link to the certificate */

	tOcspCertID	*m_certid;

	CByteArray *m_response;
	APL_CertifStatus m_status;
	std::string m_validity;						/**<  Validity of the response */
	unsigned long m_delay;

	APL_CryptoFwk *m_cryptoFwk;
	CMutex m_Mutex;									/**< Mutex */

friend APL_OcspResponse *APL_Certif::getOcspResponse();		/**< This method must access protected constructor */
};

}

#endif //__CERTIF_H__

