/*-****************************************************************************

 * Copyright (C) 2019 Miguel Figueira - <miguelblcfigueira@gmail.com>
 *
 * Licensed under the EUPL V.1.1

****************************************************************************-*/

#include "PAdESExtender.h"

#include "cryptoFwkPteid.h"
#include "Log.h"
#include "Util.h"
#include "APLCertif.h"
#include "TsaClient.h"
#include "MiscUtil.h"
#include "sign-pkcs7.h"
#include "CRLFetcher.h"
#include "poppler/PDFDoc.h"

#include <openssl/pkcs7.h>
#include <openssl/x509.h>
#include <openssl/ocsp.h>

#include <unordered_map> 

namespace eIDMW
{
    PAdESExtender::PAdESExtender(PDFSignature *signedPdfDoc) 
    {
        m_signedPdfDoc = signedPdfDoc;
    }

    bool PAdESExtender::addT()
    {
        // TODO: verify if B first. Return error if not
        bool success = true;

        m_signedPdfDoc->m_incrementalMode = true;

        PDFDoc *doc = m_signedPdfDoc->m_doc;
        doc->prepareTimestamp();
        unsigned char *to_sign = NULL;
        const char *hexToken = NULL;
        unsigned long len = doc->getSigByteArray(&to_sign, true);

        TSAClient tsp;

        //Compute SHA-256 of the signed digest
        CByteArray data(to_sign, len);
        CByteArray hashToBeSigned;
        APL_CryptoFwkPteid *cryptoFwk = AppLayer.getCryptoFwk();
        cryptoFwk->GetHash(data, FWK_ALGO_SHA256, &hashToBeSigned);

        tsp.timestamp_data(hashToBeSigned.GetBytes(), hashToBeSigned.Size());
        CByteArray tsresp = tsp.getResponse();

        unsigned char *timestamp_token = NULL;
        int tsp_token_len = 0;

        if (tsresp.Size() == 0) {
            MWLOG(LEV_ERROR, MOD_APL, "LTV: Timestamp Error - response is empty");
            success = false;
            goto cleanup;
        }

        timestamp_token = tsresp.GetBytes();
        tsp_token_len = tsresp.Size();

        unsigned char *tsToken = NULL;
        int tsTokenLen = 0;
        if (!getTokenFromTsResponse(timestamp_token, tsp_token_len, &tsToken, &tsTokenLen)) {
            MWLOG(LEV_ERROR, MOD_APL, "LTV: Error getting TimeStampToken from respnse.");
            success = false;
            goto cleanup;
        }

        hexToken = bin2AsciiHex(tsToken, tsTokenLen);

        doc->closeSignature((const char *)hexToken);
        m_signedPdfDoc->save();

    cleanup:
        if (hexToken)
            delete hexToken;

        if (to_sign)
            delete to_sign;

        return success;
    }

	bool PAdESExtender::findIssuerInEidStore(APL_CryptoFwkPteid * cryptoFwk, CByteArray &certif_ba, CByteArray &issuer_ba) {
		bool issuer_found = false;
		APL_Certifs eidstore;

		for (unsigned long i = 0; i != eidstore.countAll(); i++) {
			const CByteArray & candidate_issuer = eidstore.getCert(i)->getData();
			if (cryptoFwk->isIssuer(certif_ba, candidate_issuer)) {
				issuer_found = true;
				issuer_ba.ClearContents();
				issuer_ba.Append(candidate_issuer);
				MWLOG(LEV_INFO, MOD_APL, "Found issuer in eidstore: %s", eidstore.getCert(i)->getOwnerName());
				break;
			}

		}

		return issuer_found;
	}

	void PAdESExtender::addOCSPCertToValidationData(CByteArray &ocsp_response_ba) {
		MWLOG(LEV_DEBUG, MOD_APL, "Entering new function %s", __FUNCTION__);
		const unsigned char *p = ocsp_response_ba.GetBytes();
		ValidationDataElement *vde = NULL; 
		OCSP_RESPONSE *resp = d2i_OCSP_RESPONSE(NULL, &p, ocsp_response_ba.Size());
		

		if (resp != NULL) {
			OCSP_BASICRESP * basic_resp = OCSP_response_get1_basic(resp);
			if (basic_resp != NULL) {
				const STACK_OF(X509) * certs = OCSP_resp_get0_certs(basic_resp);
				for (size_t i = 0; certs && i < sk_X509_num(certs); i++) {
					MWLOG(LEV_DEBUG, MOD_APL, "Adding one certificate to DSS from OCSP response");
					X509 *cert = sk_X509_value(certs, i);
					unsigned char * der_data = NULL;
					int data_len = X509_to_DER(cert, &der_data);
					vde = new ValidationDataElement(der_data, data_len, ValidationDataElement::CERT);
					m_validationData.push_back(vde);

				}

			}

		}
		else {
			MWLOG(LEV_WARN, MOD_APL, "%s: Failed to decode OCSP response!", __FUNCTION__);
		}
		

	}

    bool PAdESExtender::addLT()
    {
        // TODO: verify if T first. Extend if not
        bool success = true; 

        m_signedPdfDoc->m_incrementalMode = true;

        PDFDoc *doc = m_signedPdfDoc->m_doc;
        APL_CryptoFwkPteid *cryptoFwk = AppLayer.getCryptoFwk();

        std::unordered_map<unsigned long, ValidationDataElement *> certsInDoc;

        const char *hexHash = NULL;
        unsigned char *signatureContents = NULL;
        std::unordered_set<int> sigIndexes = doc->getSignaturesIndexesUntilLastTimestamp();
        
        for (auto const& idx : sigIndexes)
        {
            /* Compute SHA1 of signature contents to add as key of VRI dict. */
            int length = doc->getSignatureContents(&signatureContents, idx);
            if (length <= 0)
            {
                MWLOG(LEV_ERROR, MOD_APL,
                    "addLT(): Error getting signature content.");
                continue;
            }

            /* Compute SHA1 hash of signature to be added as VRI key. */
            CByteArray contentBytes(signatureContents, length), hash;
            cryptoFwk->GetHashSha1(contentBytes, &hash);
            hexHash = bin2AsciiHex(hash.GetBytes(), hash.Size());

            PKCS7 *p7 = NULL;
            p7 = d2i_PKCS7(NULL, (const unsigned char**)&signatureContents, length);
            if (p7 == NULL)
            {
                MWLOG(LEV_ERROR, MOD_APL,
                    "addLT(): Error decoding signature content.");
                continue;
            }

            STACK_OF(X509) *certs = NULL;

            int type = OBJ_obj2nid(p7->type);
            if (type == NID_pkcs7_signed) {
                certs = p7->d.sign->cert;
            }
            else if (type == NID_pkcs7_signedAndEnveloped) {
                certs = p7->d.signed_and_enveloped->cert;
            }

            /* Add validation data for signature timestamp */
            ASN1_TYPE *asn1TokenType = NULL;
            STACK_OF(PKCS7_SIGNER_INFO) * signer_info = PKCS7_get_signer_info(p7);
            asn1TokenType = PKCS7_get_attribute(sk_PKCS7_SIGNER_INFO_value(signer_info, 0), NID_id_smime_aa_timeStampToken);
            if (asn1TokenType != NULL && asn1TokenType->type == V_ASN1_SEQUENCE)
            {
                PKCS7 *ts_p7 = NULL;
                const unsigned char *token_der = asn1TokenType->value.asn1_string->data;
                int token_len = asn1TokenType->value.asn1_string->length;
                ts_p7 = d2i_PKCS7(NULL, &token_der, token_len);

                if (ts_p7 != NULL)
                {
                    STACK_OF(X509) *ts_certs = NULL;
                    int type = OBJ_obj2nid(ts_p7->type);
                    if (type == NID_pkcs7_signed) {
                        ts_certs = ts_p7->d.sign->cert;
                    }
                    else if (type == NID_pkcs7_signedAndEnveloped) {
                        ts_certs = ts_p7->d.signed_and_enveloped->cert;
                    }

                    for (size_t i = 0; i < sk_X509_num(ts_certs); i++)
                    {
                        sk_X509_push(certs, sk_X509_value(ts_certs, i));
                    }
                }
            }

            /* Iterate over the certificates in this signature and add them to DSS.*/
            for (size_t i = 0; certs && i < sk_X509_num(certs); i++) {
                unsigned char *certBytes = NULL;
                X509 *cert = sk_X509_value(certs, i);

                unsigned int len = i2d_X509(cert, &certBytes);
                if (len < 0)
                {
                    MWLOG(LEV_ERROR, MOD_APL, "addLT(): Failed to parse certificate in signature.");
                    success = false;
                    goto cleanup;
                }

                ValidationDataElement *certElem;
                unsigned long uniqueCertId = X509_issuer_and_serial_hash(cert);
                if (certsInDoc.find(uniqueCertId) == certsInDoc.end())
                {
                    /* If the cert has not been added to validation data, create new element. */
                    certElem = new ValidationDataElement(certBytes, len, ValidationDataElement::CERT);
                    certsInDoc.insert(std::pair<unsigned long, ValidationDataElement *>(uniqueCertId, certElem));
                }
                else
                {
                    certElem = certsInDoc.at(uniqueCertId);
                }
                certElem->addVriKey(hexHash);
            }
        }

        /* Copy map to validationData vector */
        for (auto const& cert : certsInDoc)
            m_validationData.push_back(cert.second);

        /* For the certificates to validate, look for the issuer certificate (such that the subject name is
        equal to the subject name of the current certificate) and add the revocation data. */
        size_t validationDataSize = m_validationData.size();
        for (size_t i = 0; i <  validationDataSize; i++)
        {
            size_t subjLen = m_validationData[i]->getSize();
            CByteArray certDataByteArray(m_validationData[i]->getData(), subjLen);
            
            size_t j;
            CByteArray issuerCertDataByteArray;
            size_t issuerLen;

			MWLOG(LEV_DEBUG, MOD_APL, "### %s: adding revocation info for certificate: %s", __FUNCTION__, certificate_subject_from_der(certDataByteArray));
            
            bool foundIssuer = false;
            for (j = 0; j < validationDataSize; j++)
            {
                issuerLen = m_validationData[j]->getSize();
                issuerCertDataByteArray.ClearContents();
                issuerCertDataByteArray.Append(m_validationData[j]->getData(), issuerLen);

                if (cryptoFwk->isIssuer(certDataByteArray, issuerCertDataByteArray)) {
                    foundIssuer = true;
                    break;
                }
            }

            /*Root CA.*/
            if (i == j)
                continue;

			if (!foundIssuer) {
				//Try to find issuer in eidstore certificates
				foundIssuer = findIssuerInEidStore(cryptoFwk, certDataByteArray, issuerCertDataByteArray);
				if (foundIssuer) {
					auto *vde = 
						new ValidationDataElement(issuerCertDataByteArray.GetBytes(), issuerCertDataByteArray.Size(), ValidationDataElement::CERT, m_validationData[i]->getVriHashKeys());
					m_validationData.push_back(vde);
				}

			}

            CByteArray response;
            FWK_CertifStatus status;
            if (foundIssuer)
            {
                status = cryptoFwk->GetOCSPResponse(certDataByteArray, issuerCertDataByteArray, &response, false);
            }
			else {
				MWLOG(LEV_WARN, MOD_APL, "Couldn't find issuer for cert # %lu. Revocation info is going to be fetched from CRL", 
					i);
			}

            if (status == FWK_CERTIF_STATUS_REVOKED || status == FWK_CERTIF_STATUS_SUSPENDED)
            {
                MWLOG(LEV_WARN, MOD_APL, "addLT(): OCSP validation: revoked certificate.");
                success = false;
                goto cleanup;
            }
			else if (status == FWK_CERTIF_STATUS_UNKNOWN) {
				MWLOG(LEV_WARN, MOD_APL, "addLT(): OCSP server returned unknown status so it's either a server error or the request is buggy or uses unsupported algorithm/feature");
				//TODO: do we add this OCSP or fallback to CRL ??

			}
            else if (!foundIssuer || status == FWK_CERTIF_STATUS_ERROR)
            {
                /* Use CRL if there is no OCSP response. */
                CRLFetcher crlFetcher;
                std::string crlUrl;
                if (!cryptoFwk->GetCDPUrl(certDataByteArray, crlUrl)) 
                {
                    MWLOG(LEV_WARN, MOD_APL, "addLT(): Couldn't parse CRL URL from certificate.");
                    continue;
                }
                CByteArray crl = crlFetcher.fetch_CRL_file(crlUrl.c_str());
                ValidationDataElement *crlElem = new ValidationDataElement(crl.GetBytes(), crl.Size(), ValidationDataElement::CRL, m_validationData[i]->getVriHashKeys());
                m_validationData.push_back(crlElem);
                continue;
            }

			addOCSPCertToValidationData(response);

            ValidationDataElement *ocspResponseElem = new ValidationDataElement(response.GetBytes(), response.Size(), ValidationDataElement::OCSP, m_validationData[i]->getVriHashKeys());
            m_validationData.push_back(ocspResponseElem);
        
		}  //End of outer loop

        doc->addDSS(m_validationData);
        m_signedPdfDoc->save();

    cleanup:
        if (hexHash)
            delete hexHash;

        if (!m_calledFromLtaMethod)
        {
            for (auto const& validationElem : m_validationData)
                delete validationElem;
        }
        
        return success;
    }

    bool PAdESExtender::addLTA()
    {
        // TODO: verify if LT first. Extend if not
        m_calledFromLtaMethod = true;

        bool success = true;
        if (!addLT())
        {
            success = false;
            goto cleanup;
        }
        
        if (!addT())
        {
            success = false;
            goto cleanup;
        }

    cleanup:
        for (auto const& validationElem : m_validationData)
            delete validationElem;

        m_calledFromLtaMethod = false;

        return success;
    }
}