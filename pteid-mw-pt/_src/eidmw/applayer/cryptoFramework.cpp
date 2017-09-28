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
#include "cryptoFramework.h"

#include "MWException.h"
#include "eidErrors.h"
#include "Util.h"
#include "Log.h"
#include "ByteArray.h"
#include "APLConfig.h"
#include "APLCardPteid.h"

#include "MiscUtil.h"
#include "Thread.h"

#ifdef WIN32
#include <wincrypt.h>
#endif

#include <openssl/evp.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/err.h>
#include "xercesc/util/Base64.hpp"
#include "xercesc/util/XMLString.hpp"

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#define CRL_MEMORY_CACHE_SIZE	10

namespace eIDMW
{
/* **********************************
*** Internal class CrlMemoryCache ***
********************************** */
class CrlMemoryCache
{

private:

	class CrlMemoryElement
	{
	public:
		CrlMemoryElement()
		{
			m_baHash=NULL;
			m_crl=NULL;
			m_timeStamp.clear();
		}

		~CrlMemoryElement()
		{
			clear();
		}

		void clear()
		{
			if(m_baHash)
			{
				delete m_baHash;
				m_baHash=NULL;
			}
			if(m_crl)
			{
				X509_CRL_free(m_crl);
				m_crl=NULL;
			}
			m_timeStamp.clear();
		}

		bool getOlderTS(std::string &timeStamp)
		{
			if((timeStamp.empty() && !m_timeStamp.empty()) || m_timeStamp.compare(timeStamp)<0)
			{
				timeStamp=m_timeStamp;
				return true;
			}

			return false;
		}

		bool checkHash(const CByteArray &hash, std::string &timeStamp,bool &bTSChanged)
		{
			bTSChanged=getOlderTS(timeStamp);
			if(m_baHash)
				return m_baHash->Equals(hash);
			else
				return false;
		}

		X509_CRL *getCrl()
		{
			CTimestampUtil::getTimestamp(m_timeStamp,0,"%Y%m%dT%H%M%S"/*YYYYMMDDThhmmss*/);
			return m_crl;
		}

		void setCrl(X509_CRL *crl,const CByteArray &hash)
		{
			clear();

			m_baHash=new CByteArray(hash);
			m_crl=crl;
			CTimestampUtil::getTimestamp(m_timeStamp,0,"%Y%m%dT%H%M%S"/*YYYYMMDDThhmmss*/);
		}

	private:
		CByteArray *m_baHash;
		X509_CRL *m_crl;
		std::string m_timeStamp;
	};

public:
	CrlMemoryCache()
	{
		MWLOG(LEV_INFO, MOD_SSL, L"Create CrlMemoryCache");
		m_CrlMemoryArray=new CrlMemoryElement[CRL_MEMORY_CACHE_SIZE];
	}

	~CrlMemoryCache()
	{
		MWLOG(LEV_INFO, MOD_SSL, L"Delete CrlMemoryCache");
		delete[] m_CrlMemoryArray;
		MWLOG(LEV_INFO, MOD_SSL, L" ---> CrlMemoryCache deleted");
	}

	X509_CRL *getX509CRL(const CByteArray &crl, const CByteArray &hash)
	{
		int i;
		bool bTSChanged=false;
		int iOlder=0;
		std::string timeStamp;
		X509_CRL *pX509CRL=NULL;

		//Check if already in the array
		MWLOG(LEV_DEBUG, MOD_SSL, L"Check for element in CrlMemoryCache hash=%ls",hash.ToWString().c_str());
		for(i=0;i<CRL_MEMORY_CACHE_SIZE;i++)
		{
			if(m_CrlMemoryArray[i].checkHash(hash,timeStamp,bTSChanged))
			{
				MWLOG(LEV_DEBUG, MOD_SSL, L" ---> Element found index= %ld",i);
				return m_CrlMemoryArray[i].getCrl();
			}

			if(bTSChanged)
				iOlder=i;
		}

		MWLOG(LEV_DEBUG, MOD_SSL, L"Add element in CrlMemoryCach");
		//find the index to replace
		for(;i<CRL_MEMORY_CACHE_SIZE;i++)
		{
			if(m_CrlMemoryArray[i].getOlderTS(timeStamp))
				iOlder=i;
		}
		MWLOG(LEV_DEBUG, MOD_SSL, L" ---> Index found = %ld",iOlder);

		const unsigned char *pTempBuffer = crl.GetBytes();

		pX509CRL = d2i_X509_CRL(&pX509CRL,&pTempBuffer,crl.Size());

		MWLOG(LEV_DEBUG, MOD_SSL, L" ---> OpenSSL structure created");

		m_CrlMemoryArray[iOlder].setCrl(pX509CRL,hash);
		MWLOG(LEV_DEBUG, MOD_SSL, L" ---> Element added");

		return pX509CRL;
	}

private:
	CrlMemoryElement *m_CrlMemoryArray;
};

/* ***************
*** APL_CryptoFwk ***
***************** */
APL_CryptoFwk::APL_CryptoFwk()
{
	m_proxy_host.clear();
	m_proxy_port.clear();
	m_proxy_pac.clear();

	resetProxy();

	m_CrlMemoryCache=NULL;
	m_CrlMemoryCache=new CrlMemoryCache();
}

APL_CryptoFwk::~APL_CryptoFwk(void)
{
	if(m_CrlMemoryCache) delete m_CrlMemoryCache;
}

bool d2i_X509_Wrapper(X509** pX509,const unsigned char* pucContent, int iContentSize) {
  *pX509 = d2i_X509(pX509, &pucContent, iContentSize);

  if(*pX509 == NULL) return false;

  return true;
}

unsigned long APL_CryptoFwk::GetCertUniqueID(const CByteArray &cert)
{
	const unsigned char *pucCert=NULL;
	X509 *pX509 = NULL;

	//Convert cert into pX509
	pucCert=cert.GetBytes();
	if ( ! d2i_X509_Wrapper(&pX509, pucCert,cert.Size() ) )
	  throw CMWEXCEPTION(EIDMW_ERR_CHECK);

	//The unique ID is made with a hash of issuer and serial

	unsigned long ret;
	ret=X509_issuer_and_serial_hash(pX509);

	//Free openSSL object
	X509_free(pX509);

	return ret;
}

bool APL_CryptoFwk::VerifyDateValidity(const CByteArray &cert)
{
	bool bOk=false;

	const unsigned char *pucCert=NULL;
	X509 *pX509 = NULL;

	//Convert cert into pX509
	pucCert=cert.GetBytes();
	if ( ! d2i_X509_Wrapper(&pX509, pucCert,cert.Size() ) )
	  throw CMWEXCEPTION(EIDMW_ERR_CHECK);

	bOk=VerifyDateValidity(pX509);

	//Free openSSL object
	if(pX509) X509_free(pX509);

	return bOk;
}
bool APL_CryptoFwk::VerifyDateValidity(const X509 *pX509)
{
	bool bOk=false;

	if (pX509==NULL)
	  throw CMWEXCEPTION(EIDMW_ERR_CHECK);

	//The current time must be between notBefore and notAfter
	if(X509_cmp_current_time(X509_get_notBefore(pX509))<=0
		&& X509_cmp_current_time(X509_get_notAfter(pX509))>=0)

		bOk=true;

	return bOk;
}

bool APL_CryptoFwk::VerifyCrlDateValidity(const CByteArray &crl)
{
	bool bOk=false;

	X509_CRL *pX509_Crl = NULL;

	//Convert cert into pX509
	if ( NULL == (pX509_Crl=getX509CRL(crl)) )
	  throw CMWEXCEPTION(EIDMW_ERR_CHECK);

	bOk=VerifyCrlDateValidity(pX509_Crl);

	return bOk;
}

bool APL_CryptoFwk::VerifyCrlDateValidity(const X509_CRL *pX509_Crl)
{
	bool bOk=false;

	if (pX509_Crl==NULL)
	  throw CMWEXCEPTION(EIDMW_ERR_CHECK);

	//The current time must be between notBefore and notAfter
	if(X509_cmp_current_time(X509_CRL_get_lastUpdate(pX509_Crl))<=0
		&& X509_cmp_current_time(X509_CRL_get_nextUpdate(pX509_Crl))>=0)

		bOk=true;

	return bOk;
}
bool APL_CryptoFwk::isSelfIssuer(const CByteArray &cert)
{
	bool bOk = false;
	const unsigned char *pucCert=NULL;
	X509 *pX509 = NULL;

	//Convert cert into pX509
	pucCert=cert.GetBytes();

	if ( ! d2i_X509_Wrapper(&pX509, pucCert,cert.Size() ) )
	  throw CMWEXCEPTION(EIDMW_ERR_CHECK);


	//FIRST CHECK IF THE ISUER NAME IS THE SAME AS THE OWNER (SUBJECT)
	if(X509_name_cmp(X509_get_issuer_name(pX509),X509_get_subject_name(pX509))==0)
	{
		//THEN CHECK IF THE CERTIFICATE IS SELF-SIGNED
		bOk=VerifyCertSignature(pX509,pX509);
	}

	//Free openSSL object
    X509_free(pX509);

	return bOk;
}

bool APL_CryptoFwk::isCrlValid(const CByteArray &crl,const CByteArray &issuer)
{
	bool bOk = false;
	const unsigned char *pucIssuer=NULL;
	X509_CRL *pX509_Crl = NULL;
	X509 *pX509_Issuer = NULL;

	//Convert crl into pX509_Crl
	if ( NULL == (pX509_Crl=getX509CRL(crl)) )
	  throw CMWEXCEPTION(EIDMW_ERR_CHECK);

	//Convert issuer into pX509_Issuer
	pucIssuer=issuer.GetBytes();

	if ( ! d2i_X509_Wrapper(&pX509_Issuer, pucIssuer,issuer.Size() ) )
	  throw CMWEXCEPTION(EIDMW_ERR_CHECK);

	bOk=isCrlIssuer(pX509_Crl,pX509_Issuer);

	if(bOk)
		bOk=VerifyCrlDateValidity(pX509_Crl);

	//Free openSSL object
	if(pX509_Issuer)  X509_free(pX509_Issuer);

	return bOk;
}

bool APL_CryptoFwk::isCrlIssuer(const CByteArray &crl,const CByteArray &issuer)
{
	bool bOk = false;
	const unsigned char *pucIssuer=NULL;
	X509_CRL *pX509_Crl = NULL;
	X509 *pX509_Issuer = NULL;

	//Convert crl into pX509_Crl
	if ( NULL == (pX509_Crl=getX509CRL(crl)) )
	  throw CMWEXCEPTION(EIDMW_ERR_CHECK);

	//Convert issuer into pX509_Issuer
	pucIssuer=issuer.GetBytes();

	if ( ! d2i_X509_Wrapper(&pX509_Issuer, pucIssuer,issuer.Size() ) )
	  throw CMWEXCEPTION(EIDMW_ERR_CHECK);

	bOk=isCrlIssuer(pX509_Crl,pX509_Issuer);

	//Free openSSL object
	if(pX509_Issuer)  X509_free(pX509_Issuer);

	return bOk;
}

bool APL_CryptoFwk::isCrlIssuer(X509_CRL *pX509_Crl,X509 *pX509_Issuer)
{
	bool bOk = false;

	//Convert crl into pX509_Crl
	if ( pX509_Crl==NULL || pX509_Issuer==NULL )
	  throw CMWEXCEPTION(EIDMW_ERR_CHECK);

	//FIRST CHECK IF THE ISUER NAME IS THE SAME AS THE OWNER (SUBJECT)
	if(X509_name_cmp(X509_CRL_get_issuer(pX509_Crl),X509_get_subject_name(pX509_Issuer))==0)
	{
		//THEN CHECK IF CERT IS SIGNED BY ISSUER
		bOk=VerifyCrlSignature(pX509_Crl,pX509_Issuer);
	}

	return bOk;
}

bool APL_CryptoFwk::isIssuer(const CByteArray &cert,const CByteArray &issuer)
{
	bool bOk = false;
	const unsigned char *pucCert=NULL;
	const unsigned char *pucIssuer=NULL;
	X509 *pX509_Cert = NULL;
	X509 *pX509_Issuer = NULL;

	//Convert cert into pX509_Cert
	pucCert=cert.GetBytes();

	if ( ! d2i_X509_Wrapper(&pX509_Cert, pucCert,cert.Size() ) )
	  throw CMWEXCEPTION(EIDMW_ERR_CHECK);

	//Convert issuer into pX509_Issuer
	pucIssuer=issuer.GetBytes();

	if ( ! d2i_X509_Wrapper(&pX509_Issuer, pucIssuer,issuer.Size() ) )
	  throw CMWEXCEPTION(EIDMW_ERR_CHECK);


	//FIRST CHECK IF THE ISUER NAME IS THE SAME AS THE OWNER (SUBJECT)
	if(X509_name_cmp(X509_get_issuer_name(pX509_Cert),X509_get_subject_name(pX509_Issuer))==0)
	{
		//THEN CHECK IF CERT IS SIGNED BY ISSUER
		bOk=VerifyCertSignature(pX509_Cert,pX509_Issuer);
	}

	//Free openSSL object
	X509_free(pX509_Cert);
    X509_free(pX509_Issuer);

	return bOk;
}

bool APL_CryptoFwk::VerifyCertSignature(X509 *pX509_Cert,X509 *pX509_Issuer)
{
	bool bOk = false;

	if(pX509_Cert==NULL || pX509_Issuer==NULL)
		throw CMWEXCEPTION(EIDMW_ERR_CHECK);

	//Convert pX509_Cert->cert_info into unsigned char * and then into CByteArray
	unsigned char *pucInfo,*pucInfoNext;
	long lLen=i2d_X509_CINF(pX509_Cert->cert_info,NULL); //Get the length for the buffer
	if(lLen > 0)
	{
		OpenSSL_add_all_digests();
		//Convert the signature into CByteArray
		CByteArray signature(pX509_Cert->signature->data,pX509_Cert->signature->length);
		pucInfo = pucInfoNext = (unsigned char *)malloc(lLen);	//Allocate the buffer
		i2d_X509_CINF(pX509_Cert->cert_info,&pucInfoNext);		//Fill the buffer
		CByteArray certinfo(pucInfo,lLen);						//Fill the CByteArray
		free(pucInfo);											//Free buffer

		const EVP_MD *algorithm;
		int i=OBJ_obj2nid(pX509_Cert->sig_alg->algorithm);
		const char *algoName=OBJ_nid2sn(i);
		algorithm=EVP_get_digestbyname(algoName);
		if(algorithm==NULL)
			algorithm=EVP_sha1();

		//Verify if the signature of the certinfo is correct (regarding the issuer certificate)
		bOk=VerifySignature(certinfo,signature,pX509_Issuer,algorithm);
	}

	return bOk;
}

bool APL_CryptoFwk::VerifyCrlSignature(X509_CRL *pX509_Crl,X509 *pX509_Issuer)
{
	if(pX509_Crl==NULL || pX509_Issuer==NULL)
		throw CMWEXCEPTION(EIDMW_ERR_CHECK);

	bool bOk = false;

	//Convert pX509_Crl into unsigned char * and then into CByteArray
	unsigned char *pucCrl,*pucCrlNext;
	long lLen=i2d_X509_CRL_INFO(pX509_Crl->crl,NULL); //Get the length for the buffer
	if(lLen > 0)
	{
		//Convert the signature into CByteArray
		CByteArray signature(pX509_Crl->signature->data,pX509_Crl->signature->length);

		pucCrl = pucCrlNext = (unsigned char *)malloc(lLen);	//Allocate the buffer
		i2d_X509_CRL_INFO(pX509_Crl->crl,&pucCrlNext);		//Fill the buffer
		CByteArray crl(pucCrl,lLen);						//Fill the CByteArray
		free(pucCrl);										//Free buffer

		//Verify if the signature of the certinfo is correct (regarding the issuer certificate)
		bOk=VerifySignature(crl,signature,pX509_Issuer,EVP_sha1());
	}

	return bOk;
}

const EVP_MD *APL_CryptoFwk::ConvertAlgorithm(FWK_HashAlgo algo)
{
	switch(algo)
	{
	case FWK_ALGO_MD5:
		return EVP_md5();
	case FWK_ALGO_SHA1:
		return EVP_sha1();
	default:
		throw CMWEXCEPTION(EIDMW_ERR_CHECK);
	}
}

bool APL_CryptoFwk::VerifyHash(const CByteArray &data, const CByteArray &hash, FWK_HashAlgo algo)
{
	//Check the hash with sha1 algorithm
	return VerifyHash(data,hash,ConvertAlgorithm(algo));
}

bool APL_CryptoFwk::VerifyHashMd5(const CByteArray &data, const CByteArray &hash)
{
	//Check the hash with sha1 algorithm
	return VerifyHash(data,hash,EVP_md5());
}

bool APL_CryptoFwk::VerifyHashSha1(const CByteArray &data, const CByteArray &hash)
{
	//Check the hash with sha1 algorithm
	return VerifyHash(data,hash,EVP_sha1());
}

bool APL_CryptoFwk::VerifyHashSha256(const CByteArray &data, const CByteArray &hash)
{
	//Check the hash with sha256 algorithm
	return VerifyHash(data,hash,EVP_sha256());
}


bool APL_CryptoFwk::VerifyHash(const CByteArray &data, const CByteArray &hash, const EVP_MD *algorithm)
{
	CByteArray baCalculatedHash;

	if(!GetHash(data,algorithm,&baCalculatedHash))
		return false;

	//If the hash calculate is the same as the given hash, it's ok
	return (memcmp(baCalculatedHash.GetBytes(), hash.GetBytes(), hash.Size()) == 0);
}

bool APL_CryptoFwk::GetHash(const CByteArray &data, FWK_HashAlgo algo, CByteArray *hash)
{
	return GetHash(data,ConvertAlgorithm(algo),hash);
}

bool APL_CryptoFwk::GetHashSha1(const CByteArray &data, CByteArray *hash)
{
	//Get the hash with sha1 algorithm
	return GetHash(data,EVP_sha1(),hash);
}

bool APL_CryptoFwk::GetHashMd5(const CByteArray &data, CByteArray *hash)
{
	//Get the hash with md5 algorithm
	return GetHash(data,EVP_md5(),hash);
}

bool APL_CryptoFwk::GetHash(const CByteArray &data, const EVP_MD *algorithm, CByteArray *hash)
{
	if(hash==NULL)
		throw CMWEXCEPTION(EIDMW_ERR_CHECK);

	EVP_MD_CTX cmd_ctx;
	unsigned char md_value[EVP_MAX_MD_SIZE] = {0};
	unsigned int md_len = 0;

	//Calculate the hash from the data
	EVP_DigestInit(&cmd_ctx, algorithm);
    EVP_DigestUpdate(&cmd_ctx, data.GetBytes(), data.Size());
    EVP_DigestFinal(&cmd_ctx, md_value, &md_len);

	//Copy the hash in the ByteArray
	hash->ClearContents();
	hash->Append(md_value,md_len);

	return true;

}
bool APL_CryptoFwk::VerifySignatureSha1(const CByteArray &data, const CByteArray &signature, const CByteArray &cert)
{
	bool bOk=false;

 	const unsigned char *pucCert=NULL;
	X509 *pX509 = NULL;

	//Convert cert into pX509_Cert
	pucCert=cert.GetBytes();
	if ( ! d2i_X509_Wrapper(&pX509, pucCert,cert.Size() ) )
	  throw CMWEXCEPTION(EIDMW_ERR_CHECK);

	//Verify if the signature of the data is correct (regarding the certificate pX509)
	bOk=VerifySignature(data,signature,pX509,EVP_sha1());

 	//Free openSSL object
    X509_free(pX509);

	return bOk;
}

bool APL_CryptoFwk::VerifySignature(const CByteArray &data, const CByteArray &signature, X509 *pX509, const EVP_MD *algorithm)
{
 	if(pX509==NULL)
		throw CMWEXCEPTION(EIDMW_ERR_CHECK);

    EVP_MD_CTX cmd_ctx;
    EVP_PKEY *pKey = NULL;
 	const unsigned char *pucSign=NULL;
	long ret;

	//Get the public key
    if (NULL == (pKey = X509_get_pubkey(pX509)))
        throw CMWEXCEPTION(EIDMW_ERR_CHECK);

	//VERIFY THE DATA-SIGNATURE with public key
	EVP_VerifyInit(&cmd_ctx, algorithm);
    EVP_VerifyUpdate(&cmd_ctx, data.GetBytes(), data.Size());

	pucSign=signature.GetBytes();
	ret = EVP_VerifyFinal(&cmd_ctx, pucSign, signature.Size(), pKey);

	EVP_MD_CTX_cleanup(&cmd_ctx);
 	//Free openSSL object
    EVP_PKEY_free(pKey);

	return (ret==1);
}

bool APL_CryptoFwk::VerifyRoot(const CByteArray &cert, const unsigned char *const *roots)
{
	const unsigned char *const *proot;
	for(proot=roots;*proot!=NULL;proot++)
	{
		if(memcmp(cert.GetBytes(),*proot,cert.Size())==0)
			return true;
	}

	return false;
}

FWK_CertifStatus APL_CryptoFwk::CRLValidation(const CByteArray &cert,const CByteArray &crl)
{
	X509_CRL *pX509Crl = NULL;
	X509 *pX509 = NULL;
    	STACK_OF(X509_REVOKED) *pRevokeds = NULL;
 	const unsigned char *pucCert=NULL;
	pucCert=cert.GetBytes();
	bool bFound=false;
	FWK_CertifStatus eStatus=FWK_CERTIF_STATUS_UNCHECK;

	if ( ! d2i_X509_Wrapper(&pX509, pucCert,cert.Size() ) )
	  goto cleanup;

	//Convert bytearray into X509_CRL
	if (NULL == (pX509Crl=getX509CRL(crl)))
	{
		eStatus=FWK_CERTIF_STATUS_ERROR;
		goto cleanup;
	}

    pRevokeds = X509_CRL_get_REVOKED(pX509Crl);
    if(pRevokeds)
	{
		for(int i = 0; i < sk_X509_REVOKED_num(pRevokeds); i++)
		{
			X509_REVOKED *pRevoked = sk_X509_REVOKED_value(pRevokeds, i);
			if(M_ASN1_INTEGER_cmp(X509_get_serialNumber(pX509),pRevoked->serialNumber)==0)
			{
				bFound=true;
				break;
			}
		}
	}

	if(bFound)
		eStatus = FWK_CERTIF_STATUS_REVOKED;
	else
		eStatus = FWK_CERTIF_STATUS_VALID;

cleanup:
	//Free openSSL object
    if(pX509) X509_free(pX509);

	return eStatus;
}

FWK_CertifStatus APL_CryptoFwk::OCSPValidation(const CByteArray &cert, const CByteArray &issuer, CByteArray *pResponse)
{
	return GetOCSPResponse(cert,issuer,pResponse);
}

FWK_CertifStatus APL_CryptoFwk::GetOCSPResponse(const CByteArray &cert, const CByteArray &issuer, CByteArray *pResponse)
{
	bool bResponseOk = false;
	const unsigned char *pucCert=NULL;
	const unsigned char *pucIssuer=NULL;
	X509 *pX509_Cert = NULL;
	X509 *pX509_Issuer = NULL;
	OCSP_RESPONSE *pOcspResponse=NULL;
	FWK_CertifStatus eStatus=FWK_CERTIF_STATUS_UNCHECK;

	//Convert cert into pX509_Cert
	pucCert=cert.GetBytes();
	if ( ! d2i_X509_Wrapper(&pX509_Cert, pucCert,cert.Size() ) )
	{
		eStatus=FWK_CERTIF_STATUS_ERROR;
		goto cleanup;
	}

	//Convert issuer into pX509_Issuer
	pucIssuer=issuer.GetBytes();
	if ( ! d2i_X509_Wrapper(&pX509_Issuer, pucIssuer,issuer.Size() ) )
	{
		eStatus=FWK_CERTIF_STATUS_ERROR;
		goto cleanup;
	}

	try
	{
		eStatus=GetOCSPResponse(pX509_Cert,pX509_Issuer,&pOcspResponse);
		if(eStatus!=FWK_CERTIF_STATUS_CONNECT && eStatus!=FWK_CERTIF_STATUS_ERROR)
			bResponseOk=true;
	}
	catch(CMWException e)
	{
		eStatus=FWK_CERTIF_STATUS_ERROR;
	}

	if(bResponseOk && pResponse)
	{
		unsigned char *pBuffer=NULL;
		unsigned char *pBufferNext=NULL;

		long lLen = i2d_OCSP_RESPONSE(pOcspResponse, NULL);
		if(lLen > 0)
		{
			pBuffer = pBufferNext = (unsigned char *)malloc(lLen);
			i2d_OCSP_RESPONSE(pOcspResponse, &pBufferNext);
			pResponse->Append(pBuffer,lLen);
			free(pBuffer);
		}
		else
		{
			eStatus=FWK_CERTIF_STATUS_ERROR;
		}
	}

	//Free openSSL object
cleanup:
	if (pOcspResponse) OCSP_RESPONSE_free(pOcspResponse);
	if (pX509_Cert) X509_free(pX509_Cert);
    if (pX509_Issuer) X509_free(pX509_Issuer);

	return eStatus;
}

FWK_CertifStatus APL_CryptoFwk::GetOCSPResponse(const char *pUrlResponder,const tOcspCertID &certid, CByteArray *pResponse,const CByteArray *issuer)
{
	if(certid.issuerNameHash==NULL || certid.issuerKeyHash==NULL || certid.serialNumber==NULL)
		throw CMWEXCEPTION(EIDMW_ERR_CHECK);

	bool bResponseOk = false;
	X509 *pX509_Issuer = NULL;
 	OCSP_CERTID *pCertID=NULL;
	ASN1_TYPE *astype;
	X509_ALGOR *hashAlgorithm=NULL;
	ASN1_OCTET_STRING *issuerNameHash=NULL;
	ASN1_OCTET_STRING *issuerKeyHash=NULL;
	ASN1_INTEGER *serialNumber=NULL;
	FWK_CertifStatus eStatus=FWK_CERTIF_STATUS_UNCHECK;
	OCSP_RESPONSE *pOcspResponse=NULL;
	int nid = 0;

	if(issuer)
	{
		const unsigned char *pucIssuer=NULL;

		//Convert issuer into pX509_Issuer
		pucIssuer=issuer->GetBytes();
		if ( ! d2i_X509_Wrapper(&pX509_Issuer, pucIssuer,issuer->Size() ) )
		{
			eStatus=FWK_CERTIF_STATUS_ERROR;
			goto cleanup;
		}
	}

	//Convert tOcspCertID into OCSP_CERTID

	switch(certid.hashAlgorithm)
	{
	case FWK_ALGO_MD5:
		nid = EVP_MD_type(EVP_md5());
		break;
	case FWK_ALGO_SHA1:
		nid = EVP_MD_type(EVP_sha1());
		break;
	case FWK_ALGO_SHA256:
		nid = EVP_MD_type(EVP_sha256());
		break;	
	}

	if (!(astype = ASN1_TYPE_new()))
	{
		eStatus=FWK_CERTIF_STATUS_ERROR;
		goto cleanup;
	}

	astype->type=V_ASN1_NULL;

	if (!(hashAlgorithm = X509_ALGOR_new()))
	{
		eStatus=FWK_CERTIF_STATUS_ERROR;
		goto cleanup;
	}

	hashAlgorithm->algorithm=OBJ_nid2obj(nid);
	hashAlgorithm->parameter=astype;

	if(!(issuerNameHash=ASN1_OCTET_STRING_new()))
	{
		eStatus=FWK_CERTIF_STATUS_ERROR;
		goto cleanup;
	}

	// Mac's ASN1_OCTET_STRING_set() needs a 'const unsigned char *' instead of an 'unsigned char *'
	unsigned char tucTmp[200];

	memset(tucTmp, 0, sizeof(tucTmp));
	if (certid.issuerNameHash->Size() <= sizeof(tucTmp))
		memcpy(tucTmp, certid.issuerNameHash->GetBytes(), certid.issuerNameHash->Size());
	ASN1_OCTET_STRING_set(issuerNameHash, tucTmp,certid.issuerNameHash->Size());

	if(!(issuerKeyHash=ASN1_OCTET_STRING_new()))
	{
		eStatus=FWK_CERTIF_STATUS_ERROR;
		goto cleanup;
	}

	memset(tucTmp, 0, sizeof(tucTmp));
	if (certid. issuerKeyHash->Size() <= sizeof(tucTmp))
		memcpy(tucTmp, certid. issuerKeyHash->GetBytes(), certid. issuerKeyHash->Size());
	ASN1_OCTET_STRING_set(issuerKeyHash,tucTmp,certid.issuerKeyHash->Size());

	if(!(serialNumber=ASN1_INTEGER_new()))
	{
		eStatus=FWK_CERTIF_STATUS_ERROR;
		goto cleanup;
	}

	memset(tucTmp, 0, sizeof(tucTmp));
	if (certid. serialNumber->Size() <= sizeof(tucTmp))
		memcpy(tucTmp, certid. serialNumber->GetBytes(), certid. serialNumber->Size());
	ASN1_OCTET_STRING_set(serialNumber,tucTmp,certid.serialNumber->Size());

	if (!(pCertID = OCSP_CERTID_new()))
	{
		eStatus=FWK_CERTIF_STATUS_ERROR;
		goto cleanup;
	}

	pCertID->hashAlgorithm=hashAlgorithm;
	pCertID->issuerNameHash=issuerNameHash;
	pCertID->issuerKeyHash=issuerKeyHash;
	pCertID->serialNumber=serialNumber;

	try
	{
		eStatus=GetOCSPResponse(pUrlResponder,pCertID,&pOcspResponse,pX509_Issuer);
		if(eStatus!=FWK_CERTIF_STATUS_CONNECT && eStatus!=FWK_CERTIF_STATUS_ERROR)
			bResponseOk=true;
	}
	catch(CMWException e)
	{
		eStatus=FWK_CERTIF_STATUS_ERROR;
	}

	if(bResponseOk && pResponse)
	{
		unsigned char *pBuffer=NULL;
		unsigned char *pBufferNext=NULL;

		long lLen = i2d_OCSP_RESPONSE(pOcspResponse, NULL);
		if(lLen > 0)
		{
			pBuffer = pBufferNext = (unsigned char *)malloc(lLen);
			i2d_OCSP_RESPONSE(pOcspResponse, &pBufferNext);
			pResponse->Append(pBuffer,lLen);
			free(pBuffer);
		}
		else
		{
			eStatus=FWK_CERTIF_STATUS_ERROR;
		}
	}

cleanup:
	//Free openSSL object
	if (pOcspResponse) OCSP_RESPONSE_free(pOcspResponse);
	//if (pCertID) OCSP_CERTID_free(pCertID);
	//if (hashAlgorithm) X509_ALGOR_free(hashAlgorithm);
	//if (astype) ASN1_TYPE_free(astype);
	//if (issuerNameHash) ASN1_OCTET_STRING_free(issuerNameHash);
	//if (issuerKeyHash) ASN1_OCTET_STRING_free(issuerKeyHash);
	//if (serialNumber) ASN1_INTEGER_free(serialNumber);

	return eStatus;
}

FWK_CertifStatus APL_CryptoFwk::GetOCSPResponse(X509 *pX509_Cert,X509 *pX509_Issuer, OCSP_RESPONSE **pResponse)
{
	if(pX509_Cert==NULL || pX509_Issuer==NULL)
		throw CMWEXCEPTION(EIDMW_ERR_CHECK);

    OCSP_CERTID *pCertID=NULL;
	char *pUrlResponder=NULL;
	FWK_CertifStatus eStatus=FWK_CERTIF_STATUS_UNCHECK;

	//Get the URL of the OCSP responder
	pUrlResponder = GetOCSPUrl(pX509_Cert);

	//If there is no OCSP responder, the certificate is unknown for OCSP
	if(pUrlResponder == NULL || strlen(pUrlResponder)==0)
	{
		eStatus = FWK_CERTIF_STATUS_UNKNOWN;
		goto cleanup;
	}

    pCertID = OCSP_cert_to_id(0, pX509_Cert, pX509_Issuer);
    if (!pCertID)
	{
		eStatus = FWK_CERTIF_STATUS_ERROR;
		goto cleanup;
	}

	eStatus=GetOCSPResponse(pUrlResponder,pCertID,pResponse,pX509_Issuer);

cleanup:
	if (pUrlResponder) free(pUrlResponder);

    return eStatus;
}

FWK_CertifStatus APL_CryptoFwk::GetOCSPResponse(const char *pUrlResponder,OCSP_CERTID *pCertID, OCSP_RESPONSE **pResponse,X509 *pX509_Issuer)
{
	//The pointer should not be NULL
    if(pUrlResponder == NULL)
        throw CMWEXCEPTION(EIDMW_ERR_CHECK);

	//We must have a responder
	if(strlen(pUrlResponder)==0)
        throw CMWEXCEPTION(EIDMW_ERR_CHECK);

    if (!pCertID)
		throw CMWEXCEPTION(EIDMW_ERR_CHECK);

	BIO *pBio = 0;

	//OCSP connection socket, we will use async io on it
	int fd;
	int iSSL = 0;
	int rv = 0;
    char *pszHost = 0;
    char *pszPath = 0;
    char *pszPort = 0;

    const char * proxy_user_value = NULL;
    SSL_CTX  *pSSLCtx = 0;
    OCSP_REQ_CTX *ctx;
    X509_STORE_CTX *verify_ctx = NULL;
    OCSP_REQUEST  *pRequest = 0;
    OCSP_BASICRESP *pBasic = NULL;
    X509_STORE *store = NULL;
    bool  bConnect = false;
    bool useProxy = false;
    ASN1_GENERALIZEDTIME  *producedAt, *thisUpdate, *nextUpdate;
	int iStatus=-1;
	FWK_CertifStatus eStatus=FWK_CERTIF_STATUS_UNCHECK;
	int iReason = 0;

	APL_Config proxy_host(CConfig::EIDMW_CONFIG_PARAM_PROXY_HOST);
	APL_Config proxy_user(CConfig::EIDMW_CONFIG_PARAM_PROXY_USERNAME);
	APL_Config proxy_pwd(CConfig::EIDMW_CONFIG_PARAM_PROXY_PWD);

	if (proxy_host.getString() != NULL && strlen(proxy_host.getString()) > 0)
	{
		useProxy = true;
	}

	//We parse the URL
	char *uri=new char[strlen(pUrlResponder)+1];
#ifdef WIN32
	strcpy_s(uri,strlen(pUrlResponder)+1,pUrlResponder);
#else
	strcpy(uri,pUrlResponder);
#endif
	if (!ParseUrl(uri, &pszHost, &pszPort, &pszPath, &iSSL))
	{
		eStatus=FWK_CERTIF_STATUS_ERROR;
		goto cleanup;
	}

	//We create the request
    if (!(pRequest = OCSP_REQUEST_new()))
	{
		eStatus=FWK_CERTIF_STATUS_ERROR;
		goto cleanup;
	}

	//We add the CertID
    if (!OCSP_request_add0_id(pRequest, pCertID))
	{
		eStatus=FWK_CERTIF_STATUS_ERROR;
		goto cleanup;
	}

	OCSP_request_add1_nonce(pRequest, 0, -1);

	MWLOG(LEV_DEBUG, MOD_APL, "DEBUG: OCSP connecting to host %s port: %s IsSSL? %d",
		pszHost, pszPort, iSSL);

	/* establish a connection to the OCSP responder using proxy according to the Config */
	pBio = Connect(pszHost, atoi(pszPort), iSSL, &pSSLCtx);

	if (pBio == NULL)
	{

		MWLOG(LEV_ERROR, MOD_APL, "GetOCSPResponse: failed to connect to socket!");
		eStatus=FWK_CERTIF_STATUS_CONNECT;
	}
	else
	{

		if (BIO_get_fd(pBio, &fd) < 0) {
			fprintf(stderr, "Can't get connection fd\n");
			goto cleanup;
		}

		/*
    SCOPE(OCSP_REQ_CTX, rctx, OCSP_sendreq_new(connection.get(), const_cast<char*>(url.c_str()), 0, -1));
    if(!rctx)
        THROW_OPENSSLEXCEPTION("Failed to set OCSP request headers.");

    if(!OCSP_REQ_CTX_add1_header(rctx.get(), "Host", const_cast<char*>(hostname.c_str())))
        THROW_OPENSSLEXCEPTION("Failed to set OCSP request headers.");
		    

    if(!OCSP_REQ_CTX_set1_req(rctx.get(), req))
        THROW_OPENSSLEXCEPTION("Failed to set OCSP request headers.");

    if(!OCSP_sendreq_nbio(&resp, rctx.get()))
        THROW_OPENSSLEXCEPTION("Failed to send OCSP request.");
		*/

		//TODO: Test this with and without proxy ...
		ctx = OCSP_sendreq_new(pBio, useProxy ? uri : pszPath, NULL, -1);

		proxy_user_value = proxy_user.getString();

		if (proxy_user_value != NULL && strlen(proxy_user_value) > 0)
		{
			fprintf(stderr, "OCSP: Adding proxy auth header!\n");
			std::string proxy_cleartext = std::string(proxy_user_value) + ":" + proxy_pwd.getString();

	        char *auth_token = Base64Encode((const unsigned char *)proxy_cleartext.c_str(), proxy_cleartext.size());
	        std::string header_value = std::string("basic ") + auth_token;
	        OCSP_REQ_CTX_add1_header(ctx, "Proxy-Authorization", header_value.c_str());
			free(auth_token);
		}

		OCSP_REQ_CTX_add1_header(ctx, "User-Agent", PTEID_USER_AGENT_VALUE);

		OCSP_REQ_CTX_set1_req(ctx, pRequest);

		fd_set confds;

		//Timeout value for the OCSP request
		struct timeval tv;
		for (;;) {
			rv = OCSP_sendreq_nbio(pResponse, ctx);
			if (rv != -1)
				break;
		
			FD_ZERO(&confds);
			FD_SET(fd, &confds);
			tv.tv_usec = 0;
			//Timeout value in seconds
			tv.tv_sec = 10;
			if (BIO_should_read(pBio))
				rv = select(fd + 1, &confds, NULL, NULL, &tv);
			else if (BIO_should_write(pBio))
				rv = select(fd + 1, NULL, &confds, NULL, &tv);
			else {
				MWLOG(LEV_ERROR, MOD_APL, "GetOCSPResponse: Unexpected retry condition");
				goto cleanup;
			}
			if (rv == 0) {
				MWLOG(LEV_ERROR, MOD_APL, "GetOCSPResponse: Timeout on request");
				break;
			}
			if (rv == -1) {
				MWLOG(LEV_ERROR, MOD_APL, "GetOCSPResponse: Select error");
				break;
			}
		}

		/* send the request and get a response */
		if( NULL == *pResponse)
		{
			MWLOG(LEV_ERROR, MOD_APL, "GetOCSPResponse - Error querying OCSP: %s", ERR_error_string(ERR_get_error(), NULL));
			eStatus=FWK_CERTIF_STATUS_ERROR;
			goto cleanup;
		}

		int ResponseStatus = OCSP_response_status(*pResponse);
		if (ResponseStatus != OCSP_RESPONSE_STATUS_SUCCESSFUL)
		{
			eStatus=FWK_CERTIF_STATUS_ERROR;
			goto cleanup;
		}

	    if (!(pBasic = OCSP_response_get1_basic(*pResponse)))
		{
			eStatus=FWK_CERTIF_STATUS_ERROR;
			goto cleanup;
		}

		if (OCSP_check_nonce(pRequest, pBasic) <= 0)
		{
           MWLOG(LEV_ERROR, MOD_APL, "Incorrect Nonce value in OCSP response! Discarding the obtained information");
           eStatus = FWK_CERTIF_STATUS_ERROR;
           goto cleanup;
    	}

    	APL_EIDCard *pcard = dynamic_cast<APL_EIDCard *>(m_card);
    	//Create new store to verify the OCSP responder certificate
    	store = X509_STORE_new();

    	for (int i = 0; i < pcard->getCertificates()->countSODCAs(); i++) {
			APL_Certif * sod_ca = pcard->getCertificates()->getSODCA(i);
			X509 *pX509 = NULL;
			const unsigned char *p = sod_ca->getData().GetBytes();

			pX509 = d2i_X509(&pX509, &p, sod_ca->getData().Size());
			X509_STORE_add_cert(store, pX509);
			//fprintf(stderr, "OCSP: Adding CA %s\n", X509_NAME_oneline(X509_get_subject_name(pX509), 0, 0));
		}

		verify_ctx = X509_STORE_CTX_new();

		if(pX509_Issuer)
		{
			//Get the algorithm
			const EVP_MD *algorithm;
			int i=OBJ_obj2nid(pBasic->signatureAlgorithm->algorithm);
			const char *algoName=OBJ_nid2sn(i);
			algorithm=EVP_get_digestbyname(algoName);
			if(algorithm==NULL)
				algorithm=EVP_sha1();

			//Get the Data of the response
			long lLen = i2d_OCSP_RESPDATA(pBasic->tbsResponseData,NULL); //Get the length for the buffer
			if(lLen <= 0)
			{
				eStatus=FWK_CERTIF_STATUS_ERROR;
				goto cleanup;
			}

			unsigned char *pucBuffer=NULL;
			unsigned char *pucBufferNext=NULL;
			pucBuffer = pucBufferNext = (unsigned char *)malloc(lLen);		//Allocate the buffer
			i2d_OCSP_RESPDATA(pBasic->tbsResponseData,&pucBufferNext);		//Fill the buffer
			CByteArray baData(pucBuffer,lLen);								//Fill the CByteArray
			free(pucBuffer);												//Free buffer

			//Get the signature
			CByteArray baSignature(pBasic->signature->data,pBasic->signature->length);

			//We check the certificate in the response to see if has correctly signed the response
			//and for this certificate we check if the issuer was the one we have
			STACK_OF(X509) *pX509s = pBasic->certs;
			X509 *pX509 = NULL;
			bool bFound = false;

			//Loop through the X509's
			for(int i = 0; i < sk_X509_num(pX509s); i++)
			{
				pX509 = sk_X509_value(pX509s, i);
				if(pX509 != NULL)
				{
					if(VerifySignature(baData,baSignature,pX509,algorithm))
					{
						bFound = true;
						break;
					}
				}
			}
			if (bFound)
			{
				//Init validation context using the SOD CA certificates as trusted
				X509_STORE_CTX_init(verify_ctx, store, pX509, NULL);
				//fprintf(stderr, "OCSP: Next we're going to verify cert: %s\n", X509_NAME_oneline(X509_get_subject_name(pX509), 0, 0));
				int ret_validation = X509_verify_cert(verify_ctx);

				if (ret_validation < 1)
				{

					MWLOG(LEV_DEBUG, MOD_APL, "Couldn't validate OCSP certificate using builtin roots. Trying issuer...");
					//eStatus = FWK_CERTIF_STATUS_ERROR;
				}
				// Old validation method
				if(!VerifyCertSignature(pX509,pX509_Issuer))
					MWLOG(LEV_DEBUG, MOD_APL, "Couldn't validate OCSP certificate using issuer. This may or may not be serious...");
				
			}
			else
			{
				eStatus=FWK_CERTIF_STATUS_ERROR;
			}
		
		}

		if(eStatus!=FWK_CERTIF_STATUS_ERROR)
		{
			if (!OCSP_resp_find_status(pBasic, pCertID, &iStatus, &iReason, &producedAt,&thisUpdate, &nextUpdate))
			{
				eStatus=FWK_CERTIF_STATUS_ERROR;
			}
			else
			{
				switch(iStatus)
				{
				case V_OCSP_CERTSTATUS_GOOD:
					eStatus=FWK_CERTIF_STATUS_VALID;
					break;
				case V_OCSP_CERTSTATUS_REVOKED:
					eStatus=FWK_CERTIF_STATUS_REVOKED;
					break;
				default:
					eStatus=FWK_CERTIF_STATUS_UNKNOWN;
					break;
				}
				//Check specific revocation reason
				if (iReason == OCSP_REVOKED_STATUS_CERTIFICATEHOLD)
				{
					eStatus = FWK_CERTIF_STATUS_SUSPENDED;
				}
			}
		}
	}

cleanup:
	if(uri) delete[] uri;
    if (pBio) BIO_free_all(pBio);
    if (pszHost) OPENSSL_free(pszHost);
    if (pszPort) OPENSSL_free(pszPort);
    if (pszPath) OPENSSL_free(pszPath);
    if (pRequest) OCSP_REQUEST_free(pRequest);
    if (pSSLCtx) SSL_CTX_free(pSSLCtx);
    if (pBasic) OCSP_BASICRESP_free(pBasic);
    if (verify_ctx) {
    	X509_STORE_CTX_cleanup(verify_ctx);
   		X509_STORE_CTX_free(verify_ctx);
   	}
	if (store) X509_STORE_free(store);

    return eStatus;
}

bool APL_CryptoFwk::GetOCSPUrl(const CByteArray &cert, std::string &url)
{
	const unsigned char *pucCert=NULL;
	X509 *pX509 = NULL;
	char *pUrl=NULL;
	bool bOk=false;

	//Convert cert into pX509_Cert
	pucCert=cert.GetBytes();
	if ( ! d2i_X509_Wrapper(&pX509, pucCert,cert.Size() ) )
	  throw CMWEXCEPTION(EIDMW_ERR_CHECK);

	//Get the URL of the OCSP responder
	pUrl = GetOCSPUrl(pX509);

	url.clear();

	if(pUrl && strlen(pUrl)>0)
	{
		bOk=true;
		url+=pUrl;
	}

	if(pUrl) free(pUrl);
	if(pX509) X509_free(pX509);

	return bOk;
}

char *APL_CryptoFwk::GetOCSPUrl(X509 *pX509_Cert)
{
    STACK_OF(ACCESS_DESCRIPTION)*pStack = NULL;
    const char *pData = NULL;
    bool bFound = false;

	pStack = (STACK_OF(ACCESS_DESCRIPTION)*) X509_get_ext_d2i(pX509_Cert, NID_info_access, NULL, NULL);

    if(pStack == NULL)
        return _strdup("");

	for(int j = 0; j < sk_ACCESS_DESCRIPTION_num(pStack); j++)
    {
        ACCESS_DESCRIPTION *pAccess = sk_ACCESS_DESCRIPTION_value(pStack, j);
        if(pAccess != NULL && pAccess->method != NULL && OBJ_obj2nid(pAccess->method) == NID_ad_OCSP)
        {
            GENERAL_NAME *pName = pAccess->location;
            if(pName != NULL && pName->type == GEN_URI)
            {
                pData = (const char *)ASN1_STRING_data(pName->d.uniformResourceIdentifier);
                bFound = true;
				break;
            }
        }
    }
    sk_ACCESS_DESCRIPTION_free(pStack);

	if(!bFound)
		return NULL;

    return _strdup(pData);

}

bool APL_CryptoFwk::GetCDPUrl(const CByteArray &cert, std::string &url)
{
	const unsigned char *pucCert=NULL;
	X509 *pX509 = NULL;
	char *pUrl=NULL;
	bool bOk = false;

	//Convert cert into pX509_Cert
	pucCert=cert.GetBytes();
	if ( ! d2i_X509_Wrapper(&pX509, pucCert,cert.Size() ) )
	  throw CMWEXCEPTION(EIDMW_ERR_CHECK);

	//Get the URL of the OCSP responder
	pUrl = GetCDPUrl(pX509);

	url.clear();

	if(pUrl && strlen(pUrl)>0)
	{
		bOk=true;
		url+=pUrl;
	}

	if(pUrl) free(pUrl);
	if(pX509) X509_free(pX509);

	return bOk;
}

char *APL_CryptoFwk::GetCDPUrl(X509 *pX509_Cert)
{
    STACK_OF(DIST_POINT)* pStack = NULL;
    const char *pData = NULL;
    bool bFound = false;

	pStack = (STACK_OF(DIST_POINT)*) X509_get_ext_d2i(pX509_Cert, NID_crl_distribution_points, NULL, NULL);

    if(pStack == NULL)
       return _strdup("");


    for(int j = 0; j < sk_DIST_POINT_num(pStack); j++)
    {
        DIST_POINT *pRes = (DIST_POINT *)sk_DIST_POINT_value(pStack, j);
        if(pRes != NULL)
        {
            STACK_OF(GENERAL_NAME) *pNames = pRes->distpoint->name.fullname;
            if(pNames)
            {
                for(int i = 0; i < sk_GENERAL_NAME_num(pNames); i++)
                {
                    GENERAL_NAME *pName = sk_GENERAL_NAME_value(pNames, i);
                    if(pName != NULL && pName->type == GEN_URI )
                    {
                        pData = (const char *)ASN1_STRING_data(pName->d.uniformResourceIdentifier);
                        bFound = true;
						break;
                    }
                 }
                sk_GENERAL_NAME_free(pNames);
				if(bFound) break;
            }
        }
    }
    sk_DIST_POINT_free(pStack);

	if(!bFound)
		return NULL;

    return _strdup(pData);

}

bool APL_CryptoFwk::getCertInfo(const CByteArray &cert, tCertifInfo &info, const char *dateFormat)
{
	const unsigned char *pucCert=NULL;
	X509 *pX509 = NULL;

	//Convert cert into pX509_Cert
	pucCert=cert.GetBytes();

	if ( ! d2i_X509_Wrapper(&pX509, pucCert,cert.Size() ) )
	  throw CMWEXCEPTION(EIDMW_ERR_CHECK);

    char szTemp[128] = {0};

	CByteArray baTemp;

	baTemp.ClearContents();
	baTemp.Append(X509_get_serialNumber(pX509)->data,X509_get_serialNumber(pX509)->length);
	info.serialNumber=baTemp.ToString(false);

    memset(szTemp, 0, sizeof(szTemp));
	X509_NAME_get_text_by_NID(X509_get_subject_name(pX509), NID_commonName, szTemp, sizeof(szTemp));
	info.ownerName=szTemp;

    memset(szTemp, 0, sizeof(szTemp));
	X509_NAME_get_text_by_NID(X509_get_issuer_name(pX509), NID_commonName, szTemp, sizeof(szTemp));
	info.issuerName=szTemp;

	TimeToString(X509_get_notBefore(pX509),info.validityNotBefore,dateFormat);
	TimeToString(X509_get_notAfter(pX509),info.validityNotAfter,dateFormat);

    EVP_PKEY *pKey = X509_get_pubkey(pX509);
    if(pKey) info.keyLength=EVP_PKEY_bits(pKey);

	//Free openSSL object
	if(pX509) X509_free(pX509);
    if(pKey) EVP_PKEY_free(pKey);

	return true;
}

bool APL_CryptoFwk::getCrlInfo(const CByteArray &crl,tCrlInfo &info, const char *dateFormat)
{
	bool bDownload=true;

	X509_CRL *pX509CRL = getX509CRL(crl);
	if(!pX509CRL)
		throw CMWEXCEPTION(EIDMW_ERR_CHECK);

	int iCheck = 0;

	iCheck = X509_cmp_time(X509_CRL_get_lastUpdate(pX509CRL), NULL);
	if (iCheck >= 0)
	{
		// X509_V_ERR_CRL_NOT_YET_VALID or X509_V_ERR_ERROR_IN_CRL_LAST_UPDATE_FIELD error
		bDownload = false;
	}

	iCheck = X509_cmp_time(X509_CRL_get_nextUpdate(pX509CRL), NULL);
	if (iCheck <= 0)
	{
		// X509_V_ERR_CRL_HAS_EXPIRED or X509_V_ERR_ERROR_IN_CRL_NEXT_UPDATE_FIELD error
		bDownload = false;
	 }

    char szTemp[128] = {0};

	memset(szTemp, 0, sizeof(szTemp));
	X509_NAME_get_text_by_NID(X509_CRL_get_issuer(pX509CRL), NID_commonName, szTemp, sizeof(szTemp));
	info.issuerName=szTemp;
	//info.issuerName=X509_CRL_get_issuer(pX509CRL);

	TimeToString(X509_CRL_get_lastUpdate(pX509CRL),info.validityLastUpdate,dateFormat);
	TimeToString(X509_CRL_get_nextUpdate(pX509CRL),info.validityNextUpdate,dateFormat);

	return bDownload;
}

X509_CRL *APL_CryptoFwk::getX509CRL(const CByteArray &crl)
{
	CByteArray baHash;
	GetHash(crl,EVP_sha1(),&baHash);

	return m_CrlMemoryCache->getX509CRL(crl,baHash);
}


void loadWindowsRootCertificates(X509_STORE *store) 
{
#ifdef WIN32
    HCERTSTORE hStore;
    PCCERT_CONTEXT pContext = NULL;
    X509 *x509;

    hStore = CertOpenSystemStore(NULL, L"ROOT");

	if (!hStore) {
		MWLOG(LEV_ERROR, MOD_APL, "cryptoframework: Failed to open Windows ROOT cert store!");
		return;
	}

    while (pContext = CertEnumCertificatesInStore(hStore, pContext))
    {

        x509 = NULL;
        x509 = d2i_X509(NULL, (const unsigned char **)&pContext->pbCertEncoded, pContext->cbCertEncoded);
        if (x509)
        {
            int i = X509_STORE_add_cert(store, x509);

            if (i == 1)
				MWLOG(LEV_DEBUG, MOD_APL, "loadWindowsRootCerts: Adding Root cert: %s", X509_NAME_oneline(X509_get_subject_name(x509), 0, 0));
        }
    }
	CertFreeCertificateContext(pContext);
	CertCloseStore(hStore, 0);
#endif
}



BIO *APL_CryptoFwk::Connect(char *pszHost, int iPort, int iSSL, SSL_CTX **ppSSLCtx)
{

	BIO * pConnect = NULL;

	if (iSSL)
	{
		OpenSSL_add_all_algorithms();
		SSL_library_init();
		SSL_load_error_strings();
		ERR_load_BIO_strings();

		SSL_CTX *pSSLCtx = SSL_CTX_new(TLSv1_1_client_method());
		X509_STORE *store = SSL_CTX_get_cert_store(pSSLCtx);

		loadWindowsRootCertificates(store);
		//This will only load root certs for Linux
		SSL_CTX_set_default_verify_paths(pSSLCtx);

		SSL_CTX_set_options(pSSLCtx, SSL_OP_NO_TICKET | SSL_OP_NO_SSLv2);

#ifndef __APPLE__
		SSL_CTX_set_verify(pSSLCtx, SSL_VERIFY_PEER, NULL);
#endif

		if (!(pConnect = BIO_new_ssl_connect(pSSLCtx)))
			return NULL;

		BIO_set_conn_hostname(pConnect, pszHost);
	}
	else
	{
		if (!(pConnect = BIO_new_connect(pszHost)))
		{
			unsigned long sslError = ERR_get_error();
			MWLOG(LEV_ERROR, MOD_APL, "APL_CryptoFwk::Connect() : Error returned by BIO_new_connect() - %s", ERR_error_string(sslError, NULL));
			return NULL;
		}
	}

	BIO_set_conn_int_port(pConnect, &iPort);
		//Set BIO as nonblocking
	BIO_set_nbio(pConnect, 1);

	int rv = BIO_do_connect(pConnect);
	int fd;
	fd_set confds;
	struct timeval tv;

	if ((rv <= 0) && !BIO_should_retry(pConnect)) {
		MWLOG(LEV_ERROR, MOD_APL, "OCSP: BIO_do_connect failed!");
		return NULL;
	}

	if (BIO_get_fd(pConnect, &fd) < 0) {
		MWLOG(LEV_ERROR, MOD_APL, "OCSP: Can't get connection fd!");
		return NULL;
	}


	FD_ZERO(&confds);
	FD_SET(fd, &confds);
	tv.tv_usec = 0;
		//Connect timeout value
	tv.tv_sec = 10;
	rv = select(fd + 1, NULL, &confds, NULL, &tv);

	if (rv == 0) {
		fprintf(stderr, "Timeout on connect\n");
		return NULL;
	}

	return pConnect;
}

void APL_CryptoFwk::resetProxy()
{
	APL_Config conf_pac(CConfig::EIDMW_CONFIG_PARAM_PROXY_PACFILE);
	m_proxy_pac = conf_pac.getString();

	APL_Config conf_host(CConfig::EIDMW_CONFIG_PARAM_PROXY_HOST);
	m_proxy_host = conf_host.getString();

	APL_Config conf_port(CConfig::EIDMW_CONFIG_PARAM_PROXY_PORT);
	char buffer[10];
	sprintf_s(buffer,sizeof(buffer),"%ld",conf_port.getLong());
	m_proxy_port = buffer;

	MWLOG(LEV_INFO, MOD_SSL, L"Proxy settings: pac='%ls', host='%ls' and port=%ls",utilStringWiden(m_proxy_pac).c_str(),utilStringWiden(m_proxy_host).c_str(),utilStringWiden(m_proxy_port).c_str());
}

int APL_CryptoFwk::ParseUrl(char *pszUri, char **pszHost, char **pszPort, char **pszPath, int *iSSL)
{
	std::string proxy_host;
	std::string proxy_port;

	//If there is a pac file we check for the host and port for this uri
	if(!m_proxy_pac.empty())
    {
		if(!GetProxyFromPac(m_proxy_pac.c_str(),pszUri,&proxy_host,&proxy_port))
		{
			proxy_host.clear();
			proxy_port.clear();
		}
	}
	else
	{
		proxy_host=m_proxy_host;
		proxy_port=m_proxy_port;
	}

	if(!proxy_host.empty())
    {
        *pszHost = (char *)OPENSSL_malloc(proxy_host.size() + 1);
        strcpy_s(*pszHost,proxy_host.size() + 1, proxy_host.c_str());
    }
    if(!proxy_port.empty() && strcmp(proxy_port.c_str(),"0")!=0)
    {
        *pszPort = (char *)OPENSSL_malloc(proxy_port.size() + 1);
        strcpy_s(*pszPort,proxy_port.size() + 1, proxy_port.c_str());
    }

     if(*pszHost == NULL || *pszPort == NULL)
    {
        return OCSP_parse_url(pszUri, pszHost, pszPort, pszPath, iSSL);
     }
     else
     {
        *pszPath = (char *)OPENSSL_malloc(strlen(pszUri) + 1);
        strcpy_s(*pszPath,strlen(pszUri) + 1, pszUri);
		return 1;
     }

	 return 0;
}

bool APL_CryptoFwk::b64Encode(const CByteArray &baIn, CByteArray &baOut,bool bWithLineFeed)
{
  	XMLSize_t iLenOut=0;
	XMLByte *pOut=NULL;

	//Encode the baIn
	pOut=XERCES_CPP_NAMESPACE::Base64::encode((XMLByte *)baIn.GetBytes(),baIn.Size(),&iLenOut );

	//2000000000 is huge, but just checking in case of the unlickely truncation
	if(!pOut || iLenOut > 2000000000)
		return false;

	//Put the result in baOut
	baOut.ClearContents();
	baOut.Append(pOut,(unsigned long)iLenOut);//truncation checked above

	//XERCES_CPP_NAMESPACE::XMLString::release((char**)&pOut);
	delete [] pOut;

	//If we don't want linefeed, we have to replace them by space
	if(!bWithLineFeed)
		baOut.Replace(0x0a,' ');

	return true;
}

bool APL_CryptoFwk::b64Decode(const CByteArray &baIn, CByteArray &baOut)
{
	unsigned char *pIn=NULL;
	XMLSize_t iLenOut=0;
	XMLByte *pOut=NULL;

	//The byte array must be zero terminated
	pIn=new unsigned char[baIn.Size()+1];
	memcpy(pIn,baIn.GetBytes(),baIn.Size());
	pIn[baIn.Size()]=0;

	//Decode the pIn
	pOut=XERCES_CPP_NAMESPACE::Base64::decode((XMLByte *)pIn,&iLenOut);
	if(!pOut || iLenOut > 2000000000)
	{
		delete[] pIn;
		return false;
	}

	//Put the result in baOut
	baOut.ClearContents();
	baOut.Append(pOut,(unsigned long)iLenOut);//truncation checked above :
	//XERCES_CPP_NAMESPACE::XMLString::release((char**)&pOut);
	delete [] pOut;
	delete[] pIn;

	return true;
}

void APL_CryptoFwk::TimeToString(ASN1_TIME *asn1Time, std::string &strTime, const char *dateFormat)
{
        struct tm timeinfo = {0};
        char buffer [50];

	try
	{
		if(asn1Time->type == V_ASN1_UTCTIME)
		{
			UtcTimeToString(asn1Time, timeinfo);
			strftime (buffer,sizeof(buffer),dateFormat,&timeinfo);
		}
		if(asn1Time->type == V_ASN1_GENERALIZEDTIME)
		{
			GeneralTimeToBuffer(asn1Time, buffer, sizeof(buffer));
		}
		strTime.assign(buffer);
	}
	catch(...)
	{
		strTime.clear();
	}
}

void APL_CryptoFwk::GeneralTimeToBuffer(ASN1_GENERALIZEDTIME *asn1Time, char* buffer,size_t bufferSize)
{
	char *v;
	int i;

	i=asn1Time->length;
	v=(char *)asn1Time->data;

	if ((bufferSize < 12) || (i < 12))
		return;

	for (i=0; i<12; i++)
	{
		if ((v[i] > '9') || (v[i] < '0'))
			return;
	}
	buffer[0] = v[6];//day
	buffer[1] = v[7];//day
	buffer[2] = '/';
	buffer[3] = v[4];//month
	buffer[4] = v[5];//month
	buffer[5] = '/';
	buffer[6] = v[0];//year
	buffer[7] = v[1];//year
	buffer[8] = v[2];//year
	buffer[9] = v[3];//year
	buffer[10] = 0;

}

void APL_CryptoFwk::GeneralTimeToString(ASN1_GENERALIZEDTIME *asn1Time, struct tm &timeinfo)
{
	char *v;
	int gmt=0;
	int i;

	i=asn1Time->length;
	v=(char *)asn1Time->data;

	if (i < 12)
		return;

	if (v[i-1] == 'Z')
		gmt=1;

	for (i=0; i<12; i++)
	{
		if ((v[i] > '9') || (v[i] < '0'))
			return;
	}

	timeinfo.tm_year= (v[0]-'0')*1000+(v[1]-'0')*100 + (v[2]-'0')*10+(v[3]-'0');
	timeinfo.tm_mon= (v[4]-'0')*10+(v[5]-'0')-1;

	if ((timeinfo.tm_mon > 11) || (timeinfo.tm_mon < 0))
		return;

	timeinfo.tm_mday= (v[6]-'0')*10+(v[7]-'0');
	timeinfo.tm_hour= (v[8]-'0')*10+(v[9]-'0');
	timeinfo.tm_min=  (v[10]-'0')*10+(v[11]-'0');

	if ((v[12] >= '0') && (v[12] <= '9') && (v[13] >= '0') && (v[13] <= '9'))
		timeinfo.tm_sec=  (v[12]-'0')*10+(v[13]-'0');
}

void APL_CryptoFwk::UtcTimeToString(ASN1_UTCTIME *asn1Time, struct tm &timeinfo)
{
	char *v;
	int gmt=0;
	int i;

	i=asn1Time->length;
	v=(char *)asn1Time->data;

	if (i < 10)
		return;

	if (v[i-1] == 'Z')
		gmt=1;

	for (i=0; i<10; i++)
	{
		if ((v[i] > '9') || (v[i] < '0'))
			return;
	}

	timeinfo.tm_year= (v[0]-'0')*10+(v[1]-'0');

	if (timeinfo.tm_year < 50)
		timeinfo.tm_year+=100;

	timeinfo.tm_mon= (v[2]-'0')*10+(v[3]-'0')-1;

	if ((timeinfo.tm_mon > 11) || (timeinfo.tm_mon < 0))
		return;

	timeinfo.tm_mday= (v[4]-'0')*10+(v[5]-'0');
	timeinfo.tm_hour= (v[6]-'0')*10+(v[7]-'0');
	timeinfo.tm_min=  (v[8]-'0')*10+(v[9]-'0');

	if ((v[10] >= '0') && (v[10] <= '9') && (v[11] >= '0') && (v[11] <= '9'))
		timeinfo.tm_sec=  (v[10]-'0')*10+(v[11]-'0');
}

}
