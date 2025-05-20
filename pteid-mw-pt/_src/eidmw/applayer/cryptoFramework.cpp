/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2009 FedICT.
 * Copyright (C) 2019 Caixa Magica Software.
 * Copyright (C) 2011 Vasco Silva - <vasco.silva@caixamagica.pt>
 * Copyright (C) 2012 Rui Martinho - <rui.martinho@ama.pt>
 * Copyright (C) 2014, 2016-2017 André Guerreiro - <aguerreiro1985@gmail.com>
 * Copyright (C) 2014 Vasco Dias - <vasco.dias@caixamagica.pt>
 * Copyright (C) 2016 Luiz Lemos - <luiz.lemos@caixamagica.pt>
 * Copyright (C) 2019 Adriano Campos - <adrianoribeirocampos@gmail.com>
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
#include "PKIFetcher.h"

#include "MiscUtil.h"
#include "Thread.h"
#include <openssl/rand.h>

#ifdef WIN32
#include <wincrypt.h>
#endif

#include <openssl/evp.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/err.h>
#include <openssl/ocsp.h>

#include "xercesc/util/Base64.hpp"
#include "xercesc/util/XMLString.hpp"

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#define CRL_MEMORY_CACHE_SIZE 10

namespace eIDMW {
/* **********************************
*** Internal class CrlMemoryCache ***
********************************** */
class CrlMemoryCache {

private:
	class CrlMemoryElement {
	public:
		CrlMemoryElement() {
			m_baHash = NULL;
			m_crl = NULL;
			m_timeStamp.clear();
		}

		~CrlMemoryElement() { clear(); }

		void clear() {
			if (m_baHash) {
				delete m_baHash;
				m_baHash = NULL;
			}
			if (m_crl) {
				X509_CRL_free(m_crl);
				m_crl = NULL;
			}
			m_timeStamp.clear();
		}

		bool getOlderTS(std::string &timeStamp) {
			if ((timeStamp.empty() && !m_timeStamp.empty()) || m_timeStamp.compare(timeStamp) < 0) {
				timeStamp = m_timeStamp;
				return true;
			}

			return false;
		}

		bool checkHash(const CByteArray &hash, std::string &timeStamp, bool &bTSChanged) {
			bTSChanged = getOlderTS(timeStamp);
			if (m_baHash)
				return m_baHash->Equals(hash);
			else
				return false;
		}

		X509_CRL *getCrl() {
			CTimestampUtil::getTimestamp(m_timeStamp, 0, "%Y%m%dT%H%M%S" /*YYYYMMDDThhmmss*/);
			return m_crl;
		}

		void setCrl(X509_CRL *crl, const CByteArray &hash) {
			clear();

			m_baHash = new CByteArray(hash);
			m_crl = crl;
			CTimestampUtil::getTimestamp(m_timeStamp, 0, "%Y%m%dT%H%M%S" /*YYYYMMDDThhmmss*/);
		}

	private:
		CByteArray *m_baHash;
		X509_CRL *m_crl;
		std::string m_timeStamp;
	};

public:
	CrlMemoryCache() {
		MWLOG(LEV_INFO, MOD_SSL, L"Create CrlMemoryCache");
		m_CrlMemoryArray = new CrlMemoryElement[CRL_MEMORY_CACHE_SIZE];
	}

	~CrlMemoryCache() {
		MWLOG(LEV_INFO, MOD_SSL, L"Delete CrlMemoryCache");
		delete[] m_CrlMemoryArray;
		MWLOG(LEV_INFO, MOD_SSL, L" ---> CrlMemoryCache deleted");
	}

	X509_CRL *getX509CRL(const CByteArray &crl, const CByteArray &hash) {
		int i;
		bool bTSChanged = false;
		int iOlder = 0;
		std::string timeStamp;
		X509_CRL *pX509CRL = NULL;

		// Check if already in the array
		MWLOG(LEV_DEBUG, MOD_SSL, L"Check for element in CrlMemoryCache hash=%ls", hash.ToWString().c_str());
		for (i = 0; i < CRL_MEMORY_CACHE_SIZE; i++) {
			if (m_CrlMemoryArray[i].checkHash(hash, timeStamp, bTSChanged)) {
				MWLOG(LEV_DEBUG, MOD_SSL, L" ---> Element found index= %ld", i);
				return m_CrlMemoryArray[i].getCrl();
			}

			if (bTSChanged)
				iOlder = i;
		}

		MWLOG(LEV_DEBUG, MOD_SSL, L"Add element in CrlMemoryCach");
		// find the index to replace
		for (; i < CRL_MEMORY_CACHE_SIZE; i++) {
			if (m_CrlMemoryArray[i].getOlderTS(timeStamp))
				iOlder = i;
		}
		MWLOG(LEV_DEBUG, MOD_SSL, L" ---> Index found = %ld", iOlder);

		const unsigned char *pTempBuffer = crl.GetBytes();

		pX509CRL = d2i_X509_CRL(&pX509CRL, &pTempBuffer, crl.Size());

		MWLOG(LEV_DEBUG, MOD_SSL, L" ---> OpenSSL structure created");

		m_CrlMemoryArray[iOlder].setCrl(pX509CRL, hash);
		MWLOG(LEV_DEBUG, MOD_SSL, L" ---> Element added");

		return pX509CRL;
	}

private:
	CrlMemoryElement *m_CrlMemoryArray;
};

/* ***************
*** APL_CryptoFwk ***
***************** */
APL_CryptoFwk::APL_CryptoFwk() {
	m_proxy_host.clear();
	m_proxy_port.clear();
	m_proxy_pac.clear();

	resetProxy();

	m_CrlMemoryCache = NULL;
	m_CrlMemoryCache = new CrlMemoryCache();
}

APL_CryptoFwk::~APL_CryptoFwk(void) {
	if (m_CrlMemoryCache)
		delete m_CrlMemoryCache;
}

bool d2i_X509_Wrapper(X509 **pX509, const unsigned char *pucContent, int iContentSize) {
	*pX509 = d2i_X509(pX509, &pucContent, iContentSize);

	if (*pX509 == NULL)
		return false;

	return true;
}

unsigned long APL_CryptoFwk::GetCertUniqueID(const CByteArray &cert) {
	const unsigned char *pucCert = NULL;
	X509 *pX509 = NULL;

	// Convert cert into pX509
	pucCert = cert.GetBytes();
	if (!d2i_X509_Wrapper(&pX509, pucCert, cert.Size()))
		throw CMWEXCEPTION(EIDMW_ERR_CHECK);

	// The unique ID is made with a hash of issuer and serial

	unsigned long ret;
	ret = X509_issuer_and_serial_hash(pX509);

	// Free openSSL object
	X509_free(pX509);

	return ret;
}

ASN1_INTEGER *APL_CryptoFwk::getCertSerialNumber(const CByteArray &cert) {
	const unsigned char *pucCert = NULL;
	X509 *pX509 = NULL;

	// Convert cert into pX509_Cert
	pucCert = cert.GetBytes();
	if (!d2i_X509_Wrapper(&pX509, pucCert, cert.Size()))
		throw CMWEXCEPTION(EIDMW_ERR_CHECK);

	// Gets the serial number as ASN1_INTEGER
	ASN1_INTEGER *tmp_serial_number = X509_get_serialNumber(pX509);

	// We need to copy because X509_get_serialNumber returns an internal pointer to X509 object
	ASN1_INTEGER *serial_number = ASN1_INTEGER_dup(tmp_serial_number);

	// Free OpenSSL object
	if (pX509)
		X509_free(pX509);

	return serial_number;
}

bool APL_CryptoFwk::VerifyDateValidity(const CByteArray &cert) {
	bool bOk = false;

	const unsigned char *pucCert = NULL;
	X509 *pX509 = NULL;

	// Convert cert into pX509
	pucCert = cert.GetBytes();
	if (!d2i_X509_Wrapper(&pX509, pucCert, cert.Size()))
		throw CMWEXCEPTION(EIDMW_ERR_CHECK);

	bOk = VerifyDateValidity(pX509);

	// Free openSSL object
	if (pX509)
		X509_free(pX509);

	return bOk;
}
bool APL_CryptoFwk::VerifyDateValidity(const X509 *pX509) {
	bool bOk = false;

	if (pX509 == NULL)
		throw CMWEXCEPTION(EIDMW_ERR_CHECK);

	// The current time must be between notBefore and notAfter
	if (X509_cmp_current_time(X509_get_notBefore(pX509)) <= 0 && X509_cmp_current_time(X509_get_notAfter(pX509)) >= 0)

		bOk = true;

	return bOk;
}

bool APL_CryptoFwk::VerifyCrlDateValidity(const CByteArray &crl) {
	bool bOk = false;

	X509_CRL *pX509_Crl = NULL;

	// Convert cert into pX509
	if (NULL == (pX509_Crl = getX509CRL(crl)))
		throw CMWEXCEPTION(EIDMW_ERR_CHECK);

	bOk = VerifyCrlDateValidity(pX509_Crl);

	return bOk;
}

bool APL_CryptoFwk::VerifyCrlDateValidity(const X509_CRL *pX509_Crl) {
	bool bOk = false;

	if (pX509_Crl == NULL)
		throw CMWEXCEPTION(EIDMW_ERR_CHECK);

	// The current time must be between notBefore and notAfter
	if (X509_cmp_current_time(X509_CRL_get0_lastUpdate(pX509_Crl)) <= 0 &&
		X509_cmp_current_time(X509_CRL_get0_nextUpdate(pX509_Crl)) >= 0)

		bOk = true;

	return bOk;
}

bool APL_CryptoFwk::isSelfIssuer(const CByteArray &cert) {
	bool bOk = false;
	const unsigned char *pucCert = NULL;
	X509 *pX509 = NULL;

	// Convert cert into pX509
	pucCert = cert.GetBytes();

	if (!d2i_X509_Wrapper(&pX509, pucCert, cert.Size()))
		throw CMWEXCEPTION(EIDMW_ERR_CHECK);

	// FIRST CHECK IF THE ISUER NAME IS THE SAME AS THE OWNER (SUBJECT)
	if (X509_name_cmp(X509_get_issuer_name(pX509), X509_get_subject_name(pX509)) == 0) {
		// THEN CHECK IF THE CERTIFICATE IS SELF-SIGNED
		bOk = VerifyCertSignature(pX509, pX509);
	}

	// Free openSSL object
	X509_free(pX509);

	return bOk;
}

bool APL_CryptoFwk::isCrlValid(const CByteArray &crl, const CByteArray &issuer) {
	bool bOk = false;
	const unsigned char *pucIssuer = NULL;
	X509_CRL *pX509_Crl = NULL;
	X509 *pX509_Issuer = NULL;

	// Convert crl into pX509_Crl
	if (NULL == (pX509_Crl = getX509CRL(crl)))
		throw CMWEXCEPTION(EIDMW_ERR_CHECK);

	// Convert issuer into pX509_Issuer
	pucIssuer = issuer.GetBytes();

	if (!d2i_X509_Wrapper(&pX509_Issuer, pucIssuer, issuer.Size()))
		throw CMWEXCEPTION(EIDMW_ERR_CHECK);

	bOk = isCrlIssuer(pX509_Crl, pX509_Issuer);

	if (bOk)
		bOk = VerifyCrlDateValidity(pX509_Crl);

	// Free openSSL object
	if (pX509_Issuer)
		X509_free(pX509_Issuer);

	return bOk;
}

bool APL_CryptoFwk::isCrlIssuer(const CByteArray &crl, const CByteArray &issuer) {
	bool bOk = false;
	const unsigned char *pucIssuer = NULL;
	X509_CRL *pX509_Crl = NULL;
	X509 *pX509_Issuer = NULL;

	// Convert crl into pX509_Crl
	if (NULL == (pX509_Crl = getX509CRL(crl)))
		throw CMWEXCEPTION(EIDMW_ERR_CHECK);

	// Convert issuer into pX509_Issuer
	pucIssuer = issuer.GetBytes();

	if (!d2i_X509_Wrapper(&pX509_Issuer, pucIssuer, issuer.Size()))
		throw CMWEXCEPTION(EIDMW_ERR_CHECK);

	bOk = isCrlIssuer(pX509_Crl, pX509_Issuer);

	// Free openSSL object
	if (pX509_Issuer)
		X509_free(pX509_Issuer);

	return bOk;
}

bool APL_CryptoFwk::isCrlIssuer(X509_CRL *pX509_Crl, X509 *pX509_Issuer) {
	bool bOk = false;

	// Convert crl into pX509_Crl
	if (pX509_Crl == NULL || pX509_Issuer == NULL)
		throw CMWEXCEPTION(EIDMW_ERR_CHECK);

	// FIRST CHECK IF THE ISUER NAME IS THE SAME AS THE OWNER (SUBJECT)
	if (X509_name_cmp(X509_CRL_get_issuer(pX509_Crl), X509_get_subject_name(pX509_Issuer)) == 0) {
		// THEN CHECK IF CERT IS SIGNED BY ISSUER
		bOk = VerifyCrlSignature(pX509_Crl, pX509_Issuer);
	}

	return bOk;
}

bool APL_CryptoFwk::isIssuer(const CByteArray &cert, const CByteArray &issuer) {
	bool bOk = false;
	const unsigned char *pucCert = NULL;
	const unsigned char *pucIssuer = NULL;
	X509 *pX509_Cert = NULL;
	X509 *pX509_Issuer = NULL;

	// Convert cert into pX509_Cert
	pucCert = cert.GetBytes();

	if (!d2i_X509_Wrapper(&pX509_Cert, pucCert, cert.Size()))
		throw CMWEXCEPTION(EIDMW_ERR_CHECK);

	// Convert issuer into pX509_Issuer
	pucIssuer = issuer.GetBytes();

	if (!d2i_X509_Wrapper(&pX509_Issuer, pucIssuer, issuer.Size()))
		throw CMWEXCEPTION(EIDMW_ERR_CHECK);

	// FIRST CHECK IF THE ISUER NAME IS THE SAME AS THE OWNER (SUBJECT)
	if (X509_name_cmp(X509_get_issuer_name(pX509_Cert), X509_get_subject_name(pX509_Issuer)) == 0) {
		// THEN CHECK IF CERT IS SIGNED BY ISSUER
		bOk = VerifyCertSignature(pX509_Cert, pX509_Issuer);
	}

	// Free openSSL object
	X509_free(pX509_Cert);
	X509_free(pX509_Issuer);

	return bOk;
}

bool APL_CryptoFwk::VerifyCertSignature(X509 *pX509_Cert, X509 *pX509_Issuer) {

	EVP_PKEY *pKey = NULL;

	if (pX509_Cert == NULL || pX509_Issuer == NULL)
		throw CMWEXCEPTION(EIDMW_ERR_CHECK);

	// Get the public key
	if (NULL == (pKey = X509_get_pubkey(pX509_Issuer)))
		throw CMWEXCEPTION(EIDMW_ERR_CHECK);

	int verification = X509_verify(pX509_Cert, pKey);

	return verification == 1;
}

bool APL_CryptoFwk::VerifyCrlSignature(X509_CRL *pX509_Crl, X509 *pX509_Issuer) {
	EVP_PKEY *pKey = NULL;

	if (pX509_Crl == NULL || pX509_Issuer == NULL)
		throw CMWEXCEPTION(EIDMW_ERR_CHECK);

	// Get the public key
	if (NULL == (pKey = X509_get_pubkey(pX509_Issuer)))
		throw CMWEXCEPTION(EIDMW_ERR_CHECK);

	int verification = X509_CRL_verify(pX509_Crl, pKey);

	return verification == 1;
}

const EVP_MD *APL_CryptoFwk::ConvertAlgorithm(FWK_HashAlgo algo) {
	switch (algo) {
	case FWK_ALGO_SHA1:
		return EVP_sha1();
	case FWK_ALGO_SHA256:
		return EVP_sha256();
	default:
		throw CMWEXCEPTION(EIDMW_ERR_CHECK);
	}
}

bool APL_CryptoFwk::VerifyHash(const CByteArray &data, const CByteArray &hash, FWK_HashAlgo algo) {
	// Check the hash with sha1 algorithm
	return VerifyHash(data, hash, ConvertAlgorithm(algo));
}

bool APL_CryptoFwk::VerifyHashSha1(const CByteArray &data, const CByteArray &hash) {
	// Check the hash with sha1 algorithm
	return VerifyHash(data, hash, EVP_sha1());
}

bool APL_CryptoFwk::VerifyHashSha256(const CByteArray &data, const CByteArray &hash) {
	// Check the hash with sha256 algorithm
	return VerifyHash(data, hash, EVP_sha256());
}

bool APL_CryptoFwk::VerifyHash(const CByteArray &data, const CByteArray &hash, const EVP_MD *algorithm) {
	CByteArray baCalculatedHash;
	int ret = 0;

	if (!GetHash(data, algorithm, &baCalculatedHash)) {
		return false;
	}

	ret = memcmp(baCalculatedHash.GetBytes(), hash.GetBytes(), hash.Size());

	if (ret) {
		MWLOG(LEV_DEBUG, MOD_APL, "The calculated hash doesn't match the given hash: %s %s",
			  baCalculatedHash.ToString(true, false).c_str(), hash.ToString(true, false).c_str());
	} else {
		MWLOG(LEV_DEBUG, MOD_APL, "The calculated hash is equal to the given hash: %s %s",
			  baCalculatedHash.ToString(true, false).c_str(), hash.ToString(true, false).c_str());
	}

	// If the calculated hash is the same as the given hash, it's OK
	return ret == 0;
}

bool APL_CryptoFwk::GetHash(const CByteArray &data, FWK_HashAlgo algo, CByteArray *hash) {
	return GetHash(data, ConvertAlgorithm(algo), hash);
}

bool APL_CryptoFwk::GetHashSha1(const CByteArray &data, CByteArray *hash) {
	// Get the hash with sha1 algorithm
	return GetHash(data, EVP_sha1(), hash);
}

bool APL_CryptoFwk::GetHash(const CByteArray &data, const EVP_MD *algorithm, CByteArray *hash) {
	if (hash == NULL)
		throw CMWEXCEPTION(EIDMW_ERR_CHECK);

	EVP_MD_CTX *cmd_ctx = EVP_MD_CTX_new();
	unsigned char md_value[EVP_MAX_MD_SIZE] = {0};
	unsigned int md_len = 0;

	// Calculate the hash from the data
	EVP_DigestInit(cmd_ctx, algorithm);
	EVP_DigestUpdate(cmd_ctx, data.GetBytes(), data.Size());
	EVP_DigestFinal(cmd_ctx, md_value, &md_len);

	// Copy the hash in the ByteArray
	hash->ClearContents();
	hash->Append(md_value, md_len);

	EVP_MD_CTX_free(cmd_ctx);

	return true;
}

bool APL_CryptoFwk::VerifyRoot(const CByteArray &cert, const unsigned char *const *roots,
							   const unsigned long *root_lengths) {
	const unsigned char *const *proot;
	unsigned int i = 0;
	for (proot = roots; *proot != NULL; proot++, i++) {
		unsigned long cert_len = root_lengths[i];
		if (cert.Size() <= cert_len && memcmp(cert.GetBytes(), *proot, cert.Size()) == 0)
			return true;
	}

	return false;
}

// Serial Number
FWK_CertifStatus APL_CryptoFwk::CRLValidation(ASN1_INTEGER *serial_number, X509_CRL *pX509Crl) {

	MWLOG(LEV_INFO, MOD_SSL, L"CRL Validation");

	STACK_OF(X509_REVOKED) *pRevokeds = NULL;
	bool bFound = false;
	bool onHold = false;
	FWK_CertifStatus eStatus = FWK_CERTIF_STATUS_UNCHECK;

	// Gets the revoked certificates from the CRL
	pRevokeds = X509_CRL_get_REVOKED(pX509Crl);

	// If the list isn't empty
	if (pRevokeds) {
		// For each revoked certificate
		for (int i = 0; i < sk_X509_REVOKED_num(pRevokeds); i++) {
			// Gets Certificate
			X509_REVOKED *pRevoked = sk_X509_REVOKED_value(pRevokeds, i);
			// If the serial number matches
			if (ASN1_INTEGER_cmp(serial_number, X509_REVOKED_get0_serialNumber(pRevoked)) == 0) {
				// It is declared as found
				bFound = true;

				// Gets revocation reason
				int crit = 0;
				ASN1_ENUMERATED *revocation_reason =
					(ASN1_ENUMERATED *)X509_REVOKED_get_ext_d2i(pRevoked, NID_crl_reason, &crit, NULL);

				// If the revocation reason is hold
				if (revocation_reason &&
					ASN1_ENUMERATED_get(revocation_reason) == OCSP_REVOKED_STATUS_CERTIFICATEHOLD) {
					onHold = true;
				}
				break;
			}
		}
	}
	// Gets status
	if (onHold) {
		MWLOG(LEV_DEBUG, MOD_APL, "DEBUG: CRL Validation: Certificate Suspended.");
		eStatus = FWK_CERTIF_STATUS_SUSPENDED;
	} else if (bFound) {
		MWLOG(LEV_DEBUG, MOD_APL, "DEBUG: CRL Validation: Certificate Revoked.");
		eStatus = FWK_CERTIF_STATUS_REVOKED;
	} else {
		MWLOG(LEV_DEBUG, MOD_APL, "DEBUG: CRL Validation: Certificate Valid.");
		eStatus = FWK_CERTIF_STATUS_VALID;
	}

	return eStatus;
}

FWK_CertifStatus APL_CryptoFwk::OCSPValidation(const CByteArray &cert, const CByteArray &issuer,
											   CByteArray *pResponse) {
	return GetOCSPResponse(cert, issuer, pResponse);
}

FWK_CertifStatus APL_CryptoFwk::GetOCSPResponse(const CByteArray &cert, const CByteArray &issuer, CByteArray *pResponse,
												bool verifyResponse) {
	bool bResponseOk = false;
	const unsigned char *pucCert = NULL;
	const unsigned char *pucIssuer = NULL;
	X509 *pX509_Cert = NULL;
	X509 *pX509_Issuer = NULL;
	OCSP_RESPONSE *pOcspResponse = NULL;
	FWK_CertifStatus eStatus = FWK_CERTIF_STATUS_UNCHECK;

	// Convert cert into pX509_Cert
	pucCert = cert.GetBytes();
	if (!d2i_X509_Wrapper(&pX509_Cert, pucCert, cert.Size())) {
		eStatus = FWK_CERTIF_STATUS_ERROR;
		goto cleanup;
	}

	// Convert issuer into pX509_Issuer
	pucIssuer = issuer.GetBytes();
	if (!d2i_X509_Wrapper(&pX509_Issuer, pucIssuer, issuer.Size())) {
		eStatus = FWK_CERTIF_STATUS_ERROR;
		goto cleanup;
	}

	try {
		eStatus = GetOCSPResponse(pX509_Cert, pX509_Issuer, &pOcspResponse, verifyResponse);
		if (eStatus != FWK_CERTIF_STATUS_CONNECT && eStatus != FWK_CERTIF_STATUS_ERROR)
			bResponseOk = true;
	} catch (CMWException e) {
		eStatus = FWK_CERTIF_STATUS_ERROR;
	}

	if (bResponseOk && pResponse) {
		unsigned char *pBuffer = NULL;
		unsigned char *pBufferNext = NULL;

		long lLen = i2d_OCSP_RESPONSE(pOcspResponse, NULL);
		if (lLen > 0) {
			pBuffer = pBufferNext = (unsigned char *)malloc(lLen);
			i2d_OCSP_RESPONSE(pOcspResponse, &pBufferNext);
			pResponse->Append(pBuffer, lLen);
			free(pBuffer);
		} else {
			eStatus = FWK_CERTIF_STATUS_ERROR;
		}
	}

	// Free openSSL object
cleanup:
	if (pOcspResponse)
		OCSP_RESPONSE_free(pOcspResponse);
	if (pX509_Cert)
		X509_free(pX509_Cert);
	if (pX509_Issuer)
		X509_free(pX509_Issuer);

	return eStatus;
}

inline int getSocketError() {
#ifdef _WIN32
	return WSAGetLastError();
#else
	return errno;
#endif
}

void APL_CryptoFwk::loadCertificatesToOcspStore(X509_STORE *store) {
	auto add_certif_to_store = [store](APL_Certif * cert) {
		X509 * pX509 = NULL;
		const CByteArray &ba = cert->getData();
		const unsigned char *p_data = ba.GetBytes();

		if (d2i_X509_Wrapper(&pX509, p_data, (int)ba.Size()))
			X509_STORE_add_cert(store, pX509);
	};

	if (m_card) {
		APL_EIDCard *pcard = dynamic_cast<APL_EIDCard *>(m_card);
		for (unsigned long i = 0; i < pcard->getCertificates()->countSODCAs(); i++) {
			add_certif_to_store(pcard->getCertificates()->getSODCA(i));
		}
	}
	else {
		//APL_Certifs instance without card, used for CMD certificates
		APL_Certifs eidstore;
		
		for (unsigned long i = 0; i < eidstore.countAll(); i++) {
			add_certif_to_store(eidstore.getCert(i));
		}
	}
}

FWK_CertifStatus APL_CryptoFwk::GetOCSPResponse(const char *pUrlResponder, OCSP_CERTID *pCertID,
												OCSP_RESPONSE **pResponse, X509 *pX509_Issuer) {
	// The pointer should not be NULL
	if (pUrlResponder == NULL)
		throw CMWEXCEPTION(EIDMW_ERR_CHECK);

	// We must have a responder
	if (strlen(pUrlResponder) == 0)
		throw CMWEXCEPTION(EIDMW_ERR_CHECK);

	if (!pCertID)
		throw CMWEXCEPTION(EIDMW_ERR_CHECK);

	BIO *pBio = 0;

	// OCSP connection socket, we will use async I/O on it
	int fd;
	int iSSL = 0;
	int rv = 0;
	char *pszHost = 0;
	char *pszPath = 0;
	char *pszPort = 0;

	const char *proxy_user_value = NULL;
	SSL_CTX *pSSLCtx = 0;
	OCSP_REQ_CTX *ctx;
	X509_STORE_CTX *verify_ctx = NULL;
	OCSP_REQUEST *pRequest = 0;
	OCSP_BASICRESP *pBasic = NULL;
	X509_STORE *store = NULL;
	bool bConnect = false;
	bool useProxy = false;
	ASN1_GENERALIZEDTIME *producedAt, *thisUpdate, *nextUpdate;
	int iStatus = -1;
	FWK_CertifStatus eStatus = FWK_CERTIF_STATUS_UNCHECK;
	int iReason = 0;

	APL_Config proxy_host(CConfig::EIDMW_CONFIG_PARAM_PROXY_HOST);
	APL_Config proxy_user(CConfig::EIDMW_CONFIG_PARAM_PROXY_USERNAME);
	APL_Config proxy_pwd(CConfig::EIDMW_CONFIG_PARAM_PROXY_PWD);

	if (proxy_host.getString() != NULL && strlen(proxy_host.getString()) > 0) {
		useProxy = true;
	}

	// We parse the URL
	char *uri = new char[strlen(pUrlResponder) + 1];
#ifdef WIN32
	strcpy_s(uri, strlen(pUrlResponder) + 1, pUrlResponder);
#else
	strcpy(uri, pUrlResponder);
#endif
	if (!ParseUrl(uri, &pszHost, &pszPort, &pszPath, &iSSL)) {
		eStatus = FWK_CERTIF_STATUS_ERROR;
		goto cleanup;
	}

	// We create the request
	if (!(pRequest = OCSP_REQUEST_new())) {
		eStatus = FWK_CERTIF_STATUS_ERROR;
		goto cleanup;
	}

	// We add the CertID
	if (!OCSP_request_add0_id(pRequest, pCertID)) {
		eStatus = FWK_CERTIF_STATUS_ERROR;
		goto cleanup;
	}

	OCSP_request_add1_nonce(pRequest, 0, -1);

	MWLOG(LEV_DEBUG, MOD_APL, "OCSP connecting to host %s port: %s IsSSL? %d", pszHost, pszPort, iSSL);

	/* establish a connection to the OCSP responder using proxy according to the Config */
	pBio = Connect(pszHost, atoi(pszPort), iSSL, &pSSLCtx);

	if (pBio == NULL) {

		MWLOG(LEV_ERROR, MOD_APL, "GetOCSPResponse: failed to connect to socket!");
		eStatus = FWK_CERTIF_STATUS_CONNECT;
	} else {

		if (BIO_get_fd(pBio, &fd) < 0) {
			fprintf(stderr, "Can't get connection fd\n");
			goto cleanup;
		}

		// TODO: Test this with and without proxy ...
		ctx = OCSP_sendreq_new(pBio, useProxy ? uri : pszPath, NULL, -1);

		proxy_user_value = proxy_user.getString();

		if (proxy_user_value != NULL && strlen(proxy_user_value) > 0) {
			fprintf(stderr, "OCSP: Adding proxy auth header!\n");
			std::string proxy_cleartext = std::string(proxy_user_value) + ":" + proxy_pwd.getString();
			assert(proxy_cleartext.size() <= LONG_MAX);
			char *auth_token = Base64Encode((const unsigned char *)proxy_cleartext.c_str(), (long) proxy_cleartext.size());
			std::string header_value = std::string("basic ") + auth_token;
			OCSP_REQ_CTX_add1_header(ctx, "Proxy-Authorization", header_value.c_str());
			free(auth_token);
		}

		OCSP_REQ_CTX_add1_header(ctx, "User-Agent", PTEID_USER_AGENT_VALUE);
		OCSP_REQ_CTX_add1_header(ctx, "Host", pszHost);

		OCSP_REQ_CTX_set1_req(ctx, pRequest);

		fd_set confds;

		// Timeout value for the OCSP request
		struct timeval tv;
		for (;;) {
			rv = OCSP_sendreq_nbio(pResponse, ctx);
			if (rv != -1)
				break;

			FD_ZERO(&confds);
			FD_SET(fd, &confds);
			tv.tv_usec = 0;
			// Timeout value in seconds
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
		if (NULL == *pResponse) {
			const char *data = NULL;
			int flags = 0;
			long err = ERR_get_error_line_data(NULL, NULL, &data, &flags);
			if (err == 0) {

				MWLOG(LEV_ERROR, MOD_APL, "GetOCSPResponse - Socket error: %d", getSocketError());
			} else {
				// Check if associated data string is returned in flags
				MWLOG(LEV_ERROR, MOD_APL, "GetOCSPResponse - OpenSSL msg: (%s, Additional data: %s) Socket error: %d",
					  ERR_error_string(err, NULL), ((flags & ERR_TXT_STRING) != 0) ? data : "N/A", getSocketError());
			}

			eStatus = FWK_CERTIF_STATUS_ERROR;
			goto cleanup;
		}

		int ResponseStatus = OCSP_response_status(*pResponse);
		if (ResponseStatus != OCSP_RESPONSE_STATUS_SUCCESSFUL) {
			eStatus = FWK_CERTIF_STATUS_ERROR;
			goto cleanup;
		}

		if (!(pBasic = OCSP_response_get1_basic(*pResponse))) {
			eStatus = FWK_CERTIF_STATUS_ERROR;
			goto cleanup;
		}

		if (OCSP_check_nonce(pRequest, pBasic) <= 0) {
			MWLOG(LEV_ERROR, MOD_APL, "Incorrect Nonce value in OCSP response! Discarding the obtained information");
			eStatus = FWK_CERTIF_STATUS_ERROR;
			goto cleanup;
		}

		// Create new store to verify the OCSP responder certificate
		store = X509_STORE_new();

		loadCertificatesToOcspStore(store);

		if (pX509_Issuer) {
			STACK_OF(X509) *intermediate_certs = sk_X509_new_null();
			// Add known issuer of OCSP certificate
			if (!sk_X509_push(intermediate_certs, pX509_Issuer)) {
				MWLOG(LEV_DEBUG, MOD_APL, "Failed to add pX509_Issuer: intermediate_certs stack will be empty!");
			}

			// OCSP_NOCHECKS flag - disables the verification "that the signer certificate meets the OCSP issuer
			// criteria including potential delegation" See openssl docs at:
			// https://www.openssl.org/docs/man1.1.1/man3/OCSP_basic_verify.html This flag is needed for
			// ocsp.ecee.gov.pt which is using a single OCSP responder cert issued by the new root CA "ECRaizEstado 002"
			// even for OCSP responses of certs issued by the old root ECRaizEstado
			unsigned long verify_flags = OCSP_NOCHECKS;
			int rc = OCSP_basic_verify(pBasic, intermediate_certs, store, verify_flags);

			if (rc <= 0) {
				unsigned long osslError = ERR_get_error();

				MWLOG(LEV_ERROR, MOD_APL, "Failed to validate OCSP_BASICRESP object! openssl error: %s",
					  ERR_error_string(osslError, NULL));
				eStatus = FWK_CERTIF_STATUS_ERROR;
			}
		}

		if (eStatus != FWK_CERTIF_STATUS_ERROR) {
			if (!OCSP_resp_find_status(pBasic, pCertID, &iStatus, &iReason, &producedAt, &thisUpdate, &nextUpdate)) {
				MWLOG(LEV_DEBUG, MOD_APL, "Returning OCSP STATUS_ERROR for URL: %s", pUrlResponder);
				eStatus = FWK_CERTIF_STATUS_ERROR;
			} else {
				switch (iStatus) {
				case V_OCSP_CERTSTATUS_GOOD:
					eStatus = FWK_CERTIF_STATUS_VALID;
					break;
				case V_OCSP_CERTSTATUS_REVOKED:
					eStatus = FWK_CERTIF_STATUS_REVOKED;
					break;
				default:
					eStatus = FWK_CERTIF_STATUS_UNKNOWN;
					break;
				}
				// Check specific revocation reason
				if (iReason == OCSP_REVOKED_STATUS_CERTIFICATEHOLD) {
					eStatus = FWK_CERTIF_STATUS_SUSPENDED;
				}
			}
		}
	}

cleanup:
	if (uri)
		delete[] uri;
	if (pBio)
		BIO_free_all(pBio);
	if (pszHost)
		OPENSSL_free(pszHost);
	if (pszPort)
		OPENSSL_free(pszPort);
	if (pszPath)
		OPENSSL_free(pszPath);
	if (pRequest)
		OCSP_REQUEST_free(pRequest);
	if (pSSLCtx)
		SSL_CTX_free(pSSLCtx);
	if (pBasic)
		OCSP_BASICRESP_free(pBasic);
	if (verify_ctx) {
		X509_STORE_CTX_cleanup(verify_ctx);
		X509_STORE_CTX_free(verify_ctx);
	}
	if (store)
		X509_STORE_free(store);

	MWLOG(LEV_DEBUG, MOD_APL, "GetOCSPResponse() Returning OCSP eStatus: %d for URL: %s", eStatus, pUrlResponder);

	return eStatus;
}

FWK_CertifStatus APL_CryptoFwk::GetOCSPResponse(X509 *pX509_Cert, X509 *pX509_Issuer, OCSP_RESPONSE **pResponse,
												bool verifyResponse) {
	if (pX509_Cert == NULL || pX509_Issuer == NULL)
		throw CMWEXCEPTION(EIDMW_ERR_CHECK);

	OCSP_CERTID *pCertID = NULL;
	char *pUrlResponder = NULL;
	FWK_CertifStatus eStatus = FWK_CERTIF_STATUS_UNCHECK;

	// Get the URL of the OCSP responder
	pUrlResponder = GetOCSPUrl(pX509_Cert);

	// If there is no OCSP responder, the certificate is unknown for OCSP
	if (pUrlResponder == NULL || strlen(pUrlResponder) == 0) {
		eStatus = FWK_CERTIF_STATUS_UNKNOWN;
		goto cleanup;
	}

	pCertID = OCSP_cert_to_id(EVP_sha1(), pX509_Cert, pX509_Issuer);
	if (!pCertID) {
		eStatus = FWK_CERTIF_STATUS_ERROR;
		goto cleanup;
	}

	if (!verifyResponse)
		pX509_Issuer = NULL;

	eStatus = GetOCSPResponse(pUrlResponder, pCertID, pResponse, pX509_Issuer);

cleanup:
	if (pUrlResponder)
		free(pUrlResponder);

	return eStatus;
}

bool APL_CryptoFwk::GetOCSPUrl(const CByteArray &cert, std::string &url) {
	const unsigned char *pucCert = NULL;
	X509 *pX509 = NULL;
	char *pUrl = NULL;
	bool bOk = false;

	// Convert cert into pX509_Cert
	pucCert = cert.GetBytes();
	if (!d2i_X509_Wrapper(&pX509, pucCert, cert.Size()))
		throw CMWEXCEPTION(EIDMW_ERR_CHECK);

	// Get the URL of the OCSP responder
	pUrl = GetOCSPUrl(pX509);

	url.clear();

	if (pUrl && strlen(pUrl) > 0) {
		bOk = true;
		url += pUrl;
	}

	if (pUrl)
		free(pUrl);
	if (pX509)
		X509_free(pX509);

	return bOk;
}

bool APL_CryptoFwk::GetCAIssuerUrl(const CByteArray &cert, std::string &url) {
	const unsigned char *pucCert = NULL;
	X509 *pX509 = NULL;
	char *pUrl = NULL;
	bool bOk = false;

	// Convert cert into pX509_Cert
	pucCert = cert.GetBytes();
	if (!d2i_X509_Wrapper(&pX509, pucCert, cert.Size()))
		throw CMWEXCEPTION(EIDMW_ERR_CHECK);

	// Get the URL of the OCSP responder
	pUrl = GetCAIssuersUrl(pX509);

	url.clear();

	if (pUrl && strlen(pUrl) > 0) {
		bOk = true;
		url += pUrl;
	}

	if (pUrl)
		free(pUrl);
	if (pX509)
		X509_free(pX509);

	return bOk;
}

char *APL_CryptoFwk::GetOCSPUrl(X509 *pX509_Cert) {
	STACK_OF(ACCESS_DESCRIPTION) *pStack = NULL;
	const char *pData = NULL;
	bool bFound = false;

	pStack = (STACK_OF(ACCESS_DESCRIPTION) *)X509_get_ext_d2i(pX509_Cert, NID_info_access, NULL, NULL);

	if (pStack == NULL)
		return _strdup("");

	for (int j = 0; j < sk_ACCESS_DESCRIPTION_num(pStack); j++) {
		ACCESS_DESCRIPTION *pAccess = sk_ACCESS_DESCRIPTION_value(pStack, j);
		if (pAccess != NULL && pAccess->method != NULL && OBJ_obj2nid(pAccess->method) == NID_ad_OCSP) {
			GENERAL_NAME *pName = pAccess->location;
			if (pName != NULL && pName->type == GEN_URI) {
				pData = (const char *)ASN1_STRING_get0_data(pName->d.uniformResourceIdentifier);
				bFound = true;
				break;
			}
		}
	}
	sk_ACCESS_DESCRIPTION_free(pStack);

	if (!bFound)
		return NULL;

	return _strdup(pData);
}

char *APL_CryptoFwk::GetCAIssuersUrl(X509 *pX509_Cert) {
	STACK_OF(ACCESS_DESCRIPTION) *pStack = NULL;
	const char *pData = NULL;
	bool bFound = false;

	pStack = (STACK_OF(ACCESS_DESCRIPTION) *)X509_get_ext_d2i(pX509_Cert, NID_info_access, NULL, NULL);

	if (pStack == NULL)
		return _strdup("");

	for (int j = 0; j < sk_ACCESS_DESCRIPTION_num(pStack); j++) {
		ACCESS_DESCRIPTION *pAccess = sk_ACCESS_DESCRIPTION_value(pStack, j);
		if (pAccess != NULL && pAccess->method != NULL && OBJ_obj2nid(pAccess->method) == NID_ad_ca_issuers) {
			GENERAL_NAME *pName = pAccess->location;
			if (pName != NULL && pName->type == GEN_URI) {
				pData = (const char *)ASN1_STRING_get0_data(pName->d.uniformResourceIdentifier);
				bFound = true;
				break;
			}
		}
	}
	sk_ACCESS_DESCRIPTION_free(pStack);

	if (!bFound)
		return NULL;

	return _strdup(pData);
}

bool APL_CryptoFwk::GetCDPUrl(const CByteArray &cert, std::string &url, int ext_nid) {
	const unsigned char *pucCert = NULL;
	X509 *pX509 = NULL;
	char *pUrl = NULL;
	bool bOk = false;

	// Convert cert into pX509_Cert
	pucCert = cert.GetBytes();
	if (!d2i_X509_Wrapper(&pX509, pucCert, cert.Size()))
		throw CMWEXCEPTION(EIDMW_ERR_CHECK);

	// Get the URL of the OCSP responder
	pUrl = GetCDPUrl(pX509, ext_nid);

	url.clear();

	if (pUrl && strlen(pUrl) > 0) {
		bOk = true;
		url += pUrl;
	}

	if (pUrl)
		free(pUrl);
	if (pX509)
		X509_free(pX509);

	return bOk;
}

char *APL_CryptoFwk::GetCDPUrl(X509 *pX509_Cert, int ext_nid) {
	STACK_OF(DIST_POINT) *pStack = NULL;
	const char *pData = NULL;
	bool bFound = false;

	// Gets extension
	// int ext_nid = delta ? NID_freshest_crl : NID_crl_distribution_points;
	pStack = (STACK_OF(DIST_POINT) *)X509_get_ext_d2i(pX509_Cert, ext_nid, NULL, NULL);
	if (pStack == NULL)
		return _strdup("");

	// For each CDP
	for (int j = 0; j < sk_DIST_POINT_num(pStack); j++) {
		// Loads a distribution point
		DIST_POINT *pRes = (DIST_POINT *)sk_DIST_POINT_value(pStack, j);
		// If the distribution
		if (pRes != NULL) {
			// Gets a stack of names
			STACK_OF(GENERAL_NAME) *pNames = pRes->distpoint->name.fullname;
			if (pNames) {
				for (int i = 0; i < sk_GENERAL_NAME_num(pNames); i++) {
					GENERAL_NAME *pName = sk_GENERAL_NAME_value(pNames, i);
					if (pName != NULL && pName->type == GEN_URI) {
						pData = (const char *)ASN1_STRING_get0_data(pName->d.uniformResourceIdentifier);
						bFound = true;
						break;
					}
				}
				sk_GENERAL_NAME_free(pNames);
				if (bFound)
					break;
			}
		}
	}
	sk_DIST_POINT_free(pStack);

	if (!bFound)
		return NULL;

	return _strdup(pData);
}

bool APL_CryptoFwk::GetCrlData(const CByteArray &cert, CByteArray &outCrl) {
	// Gets the CDP URL so that it can fetch the URL
	//  The Cert will be obtained after the getCDPUrl
	//  The URL will be obtained after the GetCDPUrl
	std::string url;
	if (!GetCDPUrl(cert, url, NID_crl_distribution_points)) {
		MWLOG(LEV_ERROR, MOD_APL, "Couldn't parse CRL URL from certificate");
		return false;
	}

	PKIFetcher crlFetcher;
	outCrl = crlFetcher.fetch_PKI_file(url.c_str());

	if (outCrl.Size() == 0) {
		MWLOG(LEV_ERROR, MOD_APL,
			  "Network error fetching CRL from %s or empty response. "
			  "Revocation info is incomplete!",
			  url.c_str());
		return false;
	}

	// The outCrl is used as a return

	return true;
}

void APL_CryptoFwk::updateCRL(X509_CRL *crl, X509_CRL *delta_crl) {

	// Gets the revoked certificates in the CRL
	STACK_OF(X509_REVOKED) *pRevokeds = NULL;
	pRevokeds = X509_CRL_get_REVOKED(crl);

	// Gets the revoked certificates in the delta CRL
	STACK_OF(X509_REVOKED) *deltaRevokeds = NULL;
	deltaRevokeds = X509_CRL_get_REVOKED(delta_crl);

	// Crit param in reason code function
	int crit = 0;

	// If both the CRL and DeltaCRL have contents
	if (deltaRevokeds && pRevokeds) {

		// For each revoked certificate in the delta_crl
		for (int i = 0; i < sk_X509_REVOKED_num(deltaRevokeds); i++) {

			// Loads the current revoked certificate in the delta_crl
			X509_REVOKED *deltaRevoked = sk_X509_REVOKED_value(deltaRevokeds, i);

			// Puts crit to 0
			crit = 0;

			// Sees if the revoked CRL should be removed
			ASN1_ENUMERATED *revocation_reason =
				(ASN1_ENUMERATED *)X509_REVOKED_get_ext_d2i(deltaRevoked, NID_crl_reason, &crit, NULL);

			// Gets the revoked CRL serial number
			ASN1_INTEGER *serial_number = (ASN1_INTEGER *)X509_REVOKED_get0_serialNumber(deltaRevoked);

			uint64_t *serial_number_uint64 = new uint64_t();

			ASN1_INTEGER_get_uint64(serial_number_uint64, serial_number);

			// Gets a pointer to the X509_REVOKED so that it can be removed
			X509_REVOKED *pRevoked = X509_REVOKED_new();

			// Variable that represents if it exists in the original CRL
			int exists = X509_CRL_get0_by_serial(crl, &pRevoked, serial_number);

			// If it exists in the CRL it needs to be removed or updated
			if (exists != 0) {
				// If it exists in the CRL, but needs to be removed from CRL
				if (revocation_reason && ASN1_ENUMERATED_get(revocation_reason) == OCSP_REVOKED_STATUS_REMOVEFROMCRL) {
					// Remove contract
					X509_REVOKED *deleted_X509 = sk_X509_REVOKED_delete_ptr(pRevokeds, pRevoked);
					// Frees the deleted X509
					X509_REVOKED_free(deleted_X509);
				} else {
					// Changes the revocation reason
					X509_REVOKED_add1_ext_i2d(pRevoked, NID_crl_reason, (void *)revocation_reason, crit,
											  X509V3_ADD_REPLACE);
				}
			} else {
				// If it doesn't exist already then removes it
				// X509_CRL_add0_revoked(crl, deltaRevoked);
				X509_CRL_add0_revoked(crl, X509_REVOKED_dup(deltaRevoked));
			}
		}
	}
}

X509_CRL *APL_CryptoFwk::updateCRL(const CByteArray &crl, const CByteArray &delta_crl) {

	// Parse CRL data from ByteArray
	X509_CRL *CRL = getX509CRL(crl);

	// Parse DeltaCRL data from ByteArray
	X509_CRL *DeltaCRL = getX509CRL(delta_crl);

	if (DeltaCRL != NULL) {
		// Merge the latest changes of deltaCRL
		updateCRL(CRL, DeltaCRL);
	}

	return CRL;
}

void APL_CryptoFwk::performActiveAuthentication(const CByteArray &oid, const CByteArray &pubkey, APL_SmartCard* card) {
	MWLOG(LEV_DEBUG, MOD_APL, L"Performing Active Authentication");

	if (card == nullptr) {
		card = m_card;
	}

	bool failed = 0;
	ECDSA_SIG *ec_sig;
	EVP_PKEY_CTX *ctx;
	EVP_MD_CTX *mdctx;
	EVP_MD *evp_md;
	EVP_PKEY *pkey;
	unsigned char *der_signature = nullptr;
	int der_signature_len;
	BIGNUM *r, *s;
	size_t sig_point_size;

	const unsigned char *oid_buff = oid.GetBytes();
	const unsigned char* pubkey_buff = pubkey.GetBytes();

	// get digest by previously created NID
	ASN1_OBJECT *oid_obj = d2i_ASN1_OBJECT(nullptr, &oid_buff, oid.Size());
	auto nid = OBJ_obj2nid(oid_obj);
	evp_md = (EVP_MD *)EVP_get_digestbynid(nid);
	if (evp_md == nullptr) {
		MWLOG(LEV_INFO, MOD_APL, L"Failed to get digest by NID. Fallback to SHA256");
		evp_md = (EVP_MD *)EVP_sha256();
	}

	// construct active authentication command
	unsigned char data_to_sign[8];
	RAND_bytes(data_to_sign, 8);
	size_t data_size = sizeof(data_to_sign);
	unsigned char apdu[] = {0x00, 0x88, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	memcpy(apdu + 5, data_to_sign, data_size);

	// raw r,s point signature
	auto signature = card->sendAPDU({apdu, sizeof(apdu)});
	auto sw12 = signature.GetBytes(signature.Size() - 2);
	if (sw12.GetByte(0) != 0x90 || sw12.GetByte(1) != 0x00) {
		MWLOG(LEV_ERROR, MOD_APL, L"Failed to perform active authentication");
		failed = true;
		goto cleanup;
	}

	// convert to DER signature object
	sig_point_size = (signature.Size() - 2) / 2;
	ec_sig = ECDSA_SIG_new();
	r = BN_bin2bn(signature.GetBytes(), sig_point_size, nullptr);
	s = BN_bin2bn(signature.GetBytes() + sig_point_size, sig_point_size, nullptr);
	ECDSA_SIG_set0(ec_sig, r, s);
	der_signature_len = i2d_ECDSA_SIG(ec_sig, &der_signature);

	pkey = d2i_PUBKEY(nullptr, (const unsigned char **)&pubkey_buff, pubkey.Size());
	if (pkey == nullptr) {
		MWLOG(LEV_ERROR, MOD_APL, L"Failed to read public key from card");
		failed = true;
		goto cleanup;
	}

	// hash the `to_sign` data
	unsigned char hash[EVP_MAX_MD_SIZE];
	unsigned int hash_len;
	mdctx = EVP_MD_CTX_new();
	if (!mdctx || EVP_DigestInit_ex(mdctx, evp_md, nullptr) <= 0 ||
		EVP_DigestUpdate(mdctx, data_to_sign, data_size) <= 0 || EVP_DigestFinal_ex(mdctx, hash, &hash_len) <= 0) {
		MWLOG(LEV_ERROR, MOD_APL, L"Failed to hash verification data");
		failed = true;
		goto cleanup;
	}

	// Create and initialize the verification context based on the public key
	ctx = EVP_PKEY_CTX_new(pkey, NULL);
	if (!ctx || EVP_PKEY_verify_init(ctx) <= 0 || EVP_PKEY_CTX_set_signature_md(ctx, evp_md) <= 0) {
		MWLOG(LEV_ERROR, MOD_APL, L"Failed to create verification context");
		failed = true;
		goto cleanup;
	}

	// Perform the verification
	if (EVP_PKEY_verify(ctx, der_signature, der_signature_len, (unsigned char *)&hash[0], hash_len) != 1) {
		MWLOG(LEV_ERROR, MOD_APL, L"Failed to verify active authentication signature!");
		failed = true;
		goto cleanup;
	}

cleanup:
	if (pkey)
		EVP_PKEY_free(pkey);
	if (ec_sig)
		ECDSA_SIG_free(ec_sig);
	if (der_signature)
		OPENSSL_free(der_signature);
	if (ctx)
		EVP_PKEY_CTX_free(ctx);
	if (mdctx)
		EVP_MD_CTX_free(mdctx);
	if (evp_md)
		EVP_MD_free(evp_md);
	if (oid_obj)
		ASN1_OBJECT_free(oid_obj);

	// if should throw
	if (failed) {
		throw CMWEXCEPTION(EIDMW_SOD_ERR_ACTIVE_AUTHENTICATION);
	}
}

bool APL_CryptoFwk::GetOCSPCert(const CByteArray &ocspResponse, CByteArray &outCert) {
	bool noCheckExtension = false;
	const unsigned char *p = ocspResponse.GetBytes();
	OCSP_RESPONSE *resp = d2i_OCSP_RESPONSE(NULL, &p, ocspResponse.Size());

	if (resp != NULL) {
		OCSP_BASICRESP *basicResp = OCSP_response_get1_basic(resp);
		if (basicResp != NULL) {
			const STACK_OF(X509) *certs = OCSP_resp_get0_certs(basicResp);
			for (int i = 0; certs && i < sk_X509_num(certs); i++) {
				X509 *cert = sk_X509_value(certs, i);

				// Check for presence of the "OCSP No-Check" extension from the beginning
				int index = X509_get_ext_by_NID(cert, NID_id_pkix_OCSP_noCheck, -1);
				noCheckExtension = (index != -1);

				unsigned char *data = NULL;
				int len = X509_to_DER(cert, &data);

				outCert.Append(data, len);
			}
		}
		OCSP_RESPONSE_free(resp);

	} else {
		MWLOG(LEV_WARN, MOD_APL, "%s: Failed to decode OCSP response!", __FUNCTION__);
	}

	return noCheckExtension;
}

bool APL_CryptoFwk::getCertInfo(const CByteArray &cert, tCertifInfo &info, const char *dateFormat) {
	const unsigned char *pucCert = NULL;
	X509 *pX509 = NULL;

	// Convert cert into pX509_Cert
	pucCert = cert.GetBytes();

	if (!d2i_X509_Wrapper(&pX509, pucCert, cert.Size()))
		throw CMWEXCEPTION(EIDMW_ERR_CHECK);

	char szTemp[128] = {0};

	CByteArray baTemp;

	baTemp.ClearContents();
	baTemp.Append(X509_get_serialNumber(pX509)->data, X509_get_serialNumber(pX509)->length);
	info.serialNumber = byteArrayToHexString(baTemp.GetBytes(), baTemp.Size());

	memset(szTemp, 0, sizeof(szTemp));
	X509_NAME_get_text_by_NID(X509_get_subject_name(pX509), NID_commonName, szTemp, sizeof(szTemp));
	info.ownerName = szTemp;

	memset(szTemp, 0, sizeof(szTemp));
	X509_NAME_get_text_by_NID(X509_get_subject_name(pX509), NID_serialNumber, szTemp, sizeof(szTemp));
	info.subjectSerialNumber = szTemp;

	memset(szTemp, 0, sizeof(szTemp));
	X509_NAME_get_text_by_NID(X509_get_issuer_name(pX509), NID_commonName, szTemp, sizeof(szTemp));
	info.issuerName = szTemp;

	TimeToString(X509_get_notBefore(pX509), info.validityNotBefore, dateFormat);
	TimeToString(X509_get_notAfter(pX509), info.validityNotAfter, dateFormat);

	EVP_PKEY *pKey = X509_get_pubkey(pX509);
	if (pKey)
		info.keyLength = EVP_PKEY_bits(pKey);

	// Free openSSL object
	if (pX509)
		X509_free(pX509);
	if (pKey)
		EVP_PKEY_free(pKey);

	return true;
}

bool APL_CryptoFwk::getCrlInfo(const CByteArray &crl, tCrlInfo &info, const char *dateFormat) {
	bool bDownload = true;

	X509_CRL *pX509CRL = getX509CRL(crl);
	if (!pX509CRL)
		throw CMWEXCEPTION(EIDMW_ERR_CHECK);

	int iCheck = 0;

	iCheck = X509_cmp_time(X509_CRL_get0_lastUpdate(pX509CRL), NULL);
	if (iCheck >= 0) {
		// X509_V_ERR_CRL_NOT_YET_VALID or X509_V_ERR_ERROR_IN_CRL_LAST_UPDATE_FIELD error
		bDownload = false;
	}

	iCheck = X509_cmp_time(X509_CRL_get0_nextUpdate(pX509CRL), NULL);
	if (iCheck <= 0) {
		// X509_V_ERR_CRL_HAS_EXPIRED or X509_V_ERR_ERROR_IN_CRL_NEXT_UPDATE_FIELD error
		bDownload = false;
	}

	char szTemp[128] = {0};

	memset(szTemp, 0, sizeof(szTemp));
	X509_NAME_get_text_by_NID(X509_CRL_get_issuer(pX509CRL), NID_commonName, szTemp, sizeof(szTemp));
	info.issuerName = szTemp;
	// info.issuerName=X509_CRL_get_issuer(pX509CRL);

	TimeToString(X509_CRL_get0_lastUpdate(pX509CRL), info.validityLastUpdate, dateFormat);
	TimeToString(X509_CRL_get0_nextUpdate(pX509CRL), info.validityNextUpdate, dateFormat);

	return bDownload;
}

X509_CRL *APL_CryptoFwk::getX509CRL(const CByteArray &crl) {
	CByteArray baHash;
	GetHash(crl, EVP_sha1(), &baHash);

	return m_CrlMemoryCache->getX509CRL(crl, baHash);
}

void loadWindowsRootCertificates(X509_STORE *store) {
#ifdef WIN32
	HCERTSTORE hStore;
	PCCERT_CONTEXT pContext = NULL;
	X509 *x509;

	hStore = CertOpenSystemStore(NULL, L"ROOT");

	if (!hStore) {
		MWLOG(LEV_ERROR, MOD_APL, "cryptoframework: Failed to open Windows ROOT cert store!");
		return;
	}

	while (pContext = CertEnumCertificatesInStore(hStore, pContext)) {

		x509 = NULL;
		x509 = d2i_X509(NULL, (const unsigned char **)&pContext->pbCertEncoded, pContext->cbCertEncoded);
		if (x509) {
			int i = X509_STORE_add_cert(store, x509);

			if (i == 1)
				MWLOG(LEV_DEBUG, MOD_APL, "loadWindowsRootCerts: Adding Root cert: %s",
					  X509_NAME_oneline(X509_get_subject_name(x509), 0, 0));
		}
	}
	CertFreeCertificateContext(pContext);
	CertCloseStore(hStore, 0);
#endif
}

BIO *APL_CryptoFwk::Connect(char *pszHost, int iPort, int iSSL, SSL_CTX **ppSSLCtx) {

	BIO *pConnect = NULL;

	if (iSSL) {
		OpenSSL_add_all_algorithms();
		SSL_library_init();
		SSL_load_error_strings();
		ERR_load_BIO_strings();

		SSL_CTX *pSSLCtx = SSL_CTX_new(TLS_client_method());
		X509_STORE *store = SSL_CTX_get_cert_store(pSSLCtx);

		loadWindowsRootCertificates(store);
		// This will only load root certs for Linux
		SSL_CTX_set_default_verify_paths(pSSLCtx);

		SSL_CTX_set_options(pSSLCtx, SSL_OP_NO_TICKET | SSL_OP_NO_SSLv2);

#ifndef __APPLE__
		SSL_CTX_set_verify(pSSLCtx, SSL_VERIFY_PEER, NULL);
#endif

		if (!(pConnect = BIO_new_ssl_connect(pSSLCtx)))
			return NULL;

		BIO_set_conn_hostname(pConnect, pszHost);
	} else {
		if (!(pConnect = BIO_new_connect(pszHost))) {
			unsigned long sslError = ERR_get_error();
			MWLOG(LEV_ERROR, MOD_APL, "APL_CryptoFwk::Connect() : Error returned by BIO_new_connect() - %s",
				  ERR_error_string(sslError, NULL));
			return NULL;
		}
	}

	char port_buffer[10];
	sprintf(port_buffer, "%d", iPort);
	BIO_set_conn_port(pConnect, port_buffer);

	// Set BIO as nonblocking
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
	// Connect timeout value
	tv.tv_sec = 10;
	rv = select(fd + 1, NULL, &confds, NULL, &tv);

	if (rv == 0) {
		fprintf(stderr, "Timeout on connect\n");
		return NULL;
	}

	return pConnect;
}

void APL_CryptoFwk::resetProxy() {
	APL_Config conf_pac(CConfig::EIDMW_CONFIG_PARAM_PROXY_PACFILE);
	m_proxy_pac = conf_pac.getString();

	APL_Config conf_host(CConfig::EIDMW_CONFIG_PARAM_PROXY_HOST);
	m_proxy_host = conf_host.getString();

	APL_Config conf_port(CConfig::EIDMW_CONFIG_PARAM_PROXY_PORT);
	char buffer[10];
	sprintf_s(buffer, sizeof(buffer), "%ld", conf_port.getLong());
	m_proxy_port = buffer;

	MWLOG(LEV_INFO, MOD_SSL, L"Proxy settings: pac='%ls', host='%ls' and port=%ls",
		  utilStringWiden(m_proxy_pac).c_str(), utilStringWiden(m_proxy_host).c_str(),
		  utilStringWiden(m_proxy_port).c_str());
}

int APL_CryptoFwk::ParseUrl(char *pszUri, char **pszHost, char **pszPort, char **pszPath, int *iSSL) {
	std::string proxy_host;
	std::string proxy_port;

	// If there is a pac file we check for the host and port for this uri
	if (!m_proxy_pac.empty()) {
		if (!GetProxyFromPac(m_proxy_pac.c_str(), pszUri, &proxy_host, &proxy_port)) {
			proxy_host.clear();
			proxy_port.clear();
		}
	} else {
		proxy_host = m_proxy_host;
		proxy_port = m_proxy_port;
	}

	if (!proxy_host.empty()) {
		*pszHost = (char *)OPENSSL_malloc(proxy_host.size() + 1);
		strcpy_s(*pszHost, proxy_host.size() + 1, proxy_host.c_str());
	}
	if (!proxy_port.empty() && strcmp(proxy_port.c_str(), "0") != 0) {
		*pszPort = (char *)OPENSSL_malloc(proxy_port.size() + 1);
		strcpy_s(*pszPort, proxy_port.size() + 1, proxy_port.c_str());
	}

	if (*pszHost == NULL || *pszPort == NULL) {
		return OCSP_parse_url(pszUri, pszHost, pszPort, pszPath, iSSL);
	} else {
		*pszPath = (char *)OPENSSL_malloc(strlen(pszUri) + 1);
		strcpy_s(*pszPath, strlen(pszUri) + 1, pszUri);
		return 1;
	}

	return 0;
}

bool APL_CryptoFwk::b64Encode(const CByteArray &baIn, CByteArray &baOut, bool bWithLineFeed) {
	XMLSize_t iLenOut = 0;
	XMLByte *pOut = NULL;

	// Encode the baIn
	pOut = XERCES_CPP_NAMESPACE::Base64::encode((XMLByte *)baIn.GetBytes(), baIn.Size(), &iLenOut);

	// 2000000000 is huge, but just checking in case of the unlickely truncation
	if (!pOut || iLenOut > 2000000000)
		return false;

	// Put the result in baOut
	baOut.ClearContents();
	baOut.Append(pOut, (unsigned long)iLenOut); // truncation checked above

	// XERCES_CPP_NAMESPACE::XMLString::release((char**)&pOut);
	delete[] pOut;

	// If we don't want linefeed, we have to replace them by space
	if (!bWithLineFeed)
		baOut.Replace(0x0a, ' ');

	return true;
}

bool APL_CryptoFwk::b64Decode(const CByteArray &baIn, CByteArray &baOut) {
	unsigned char *pIn = NULL;
	XMLSize_t iLenOut = 0;
	XMLByte *pOut = NULL;

	// The byte array must be zero terminated
	pIn = new unsigned char[baIn.Size() + 1];
	memcpy(pIn, baIn.GetBytes(), baIn.Size());
	pIn[baIn.Size()] = 0;

	// Decode the pIn
	pOut = XERCES_CPP_NAMESPACE::Base64::decode((XMLByte *)pIn, &iLenOut);
	if (!pOut || iLenOut > 2000000000) {
		delete[] pIn;
		return false;
	}

	// Put the result in baOut
	baOut.ClearContents();
	baOut.Append(pOut, (unsigned long)iLenOut); // truncation checked above :
	// XERCES_CPP_NAMESPACE::XMLString::release((char**)&pOut);
	delete[] pOut;
	delete[] pIn;

	return true;
}

void APL_CryptoFwk::TimeToString(const ASN1_TIME *asn1Time, std::string &strTime, const char *dateFormat) {
	struct tm timeinfo = {0};
	char buffer[50];

	try {
		if (asn1Time->type == V_ASN1_UTCTIME) {
			UtcTimeToString(asn1Time, timeinfo);
			strftime(buffer, sizeof(buffer), dateFormat, &timeinfo);
		}
		if (asn1Time->type == V_ASN1_GENERALIZEDTIME) {
			GeneralTimeToBuffer(asn1Time, buffer, sizeof(buffer));
		}
		strTime.assign(buffer);
	} catch (...) {
		strTime.clear();
	}
}

void APL_CryptoFwk::GeneralTimeToBuffer(const ASN1_GENERALIZEDTIME *asn1Time, char *buffer, size_t bufferSize) {
	char *v;
	int i;

	i = asn1Time->length;
	v = (char *)asn1Time->data;

	if ((bufferSize < 12) || (i < 12))
		return;

	for (i = 0; i < 12; i++) {
		if ((v[i] > '9') || (v[i] < '0'))
			return;
	}
	buffer[0] = v[6]; // day
	buffer[1] = v[7]; // day
	buffer[2] = '/';
	buffer[3] = v[4]; // month
	buffer[4] = v[5]; // month
	buffer[5] = '/';
	buffer[6] = v[0]; // year
	buffer[7] = v[1]; // year
	buffer[8] = v[2]; // year
	buffer[9] = v[3]; // year
	buffer[10] = 0;
}

void APL_CryptoFwk::GeneralTimeToString(ASN1_GENERALIZEDTIME *asn1Time, struct tm &timeinfo) {
	char *v;
	int gmt = 0;
	int i;

	i = asn1Time->length;
	v = (char *)asn1Time->data;

	if (i < 12)
		return;

	if (v[i - 1] == 'Z')
		gmt = 1;

	for (i = 0; i < 12; i++) {
		if ((v[i] > '9') || (v[i] < '0'))
			return;
	}

	timeinfo.tm_year = (v[0] - '0') * 1000 + (v[1] - '0') * 100 + (v[2] - '0') * 10 + (v[3] - '0');
	timeinfo.tm_mon = (v[4] - '0') * 10 + (v[5] - '0') - 1;

	if ((timeinfo.tm_mon > 11) || (timeinfo.tm_mon < 0))
		return;

	timeinfo.tm_mday = (v[6] - '0') * 10 + (v[7] - '0');
	timeinfo.tm_hour = (v[8] - '0') * 10 + (v[9] - '0');
	timeinfo.tm_min = (v[10] - '0') * 10 + (v[11] - '0');

	if ((v[12] >= '0') && (v[12] <= '9') && (v[13] >= '0') && (v[13] <= '9'))
		timeinfo.tm_sec = (v[12] - '0') * 10 + (v[13] - '0');
}

void APL_CryptoFwk::UtcTimeToString(const ASN1_UTCTIME *asn1Time, struct tm &timeinfo) {
	char *v;
	int gmt = 0;
	int i;

	i = asn1Time->length;
	v = (char *)asn1Time->data;

	if (i < 10)
		return;

	if (v[i - 1] == 'Z')
		gmt = 1;

	for (i = 0; i < 10; i++) {
		if ((v[i] > '9') || (v[i] < '0'))
			return;
	}

	timeinfo.tm_year = (v[0] - '0') * 10 + (v[1] - '0');

	if (timeinfo.tm_year < 50)
		timeinfo.tm_year += 100;

	timeinfo.tm_mon = (v[2] - '0') * 10 + (v[3] - '0') - 1;

	if ((timeinfo.tm_mon > 11) || (timeinfo.tm_mon < 0))
		return;

	timeinfo.tm_mday = (v[4] - '0') * 10 + (v[5] - '0');
	timeinfo.tm_hour = (v[6] - '0') * 10 + (v[7] - '0');
	timeinfo.tm_min = (v[8] - '0') * 10 + (v[9] - '0');

	if ((v[10] >= '0') && (v[10] <= '9') && (v[11] >= '0') && (v[11] <= '9'))
		timeinfo.tm_sec = (v[10] - '0') * 10 + (v[11] - '0');
}

} // namespace eIDMW
