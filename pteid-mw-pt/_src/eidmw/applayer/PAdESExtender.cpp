/*-****************************************************************************

 * Copyright (C) 2020 Miguel Figueira - <miguelblcfigueira@gmail.com>
 * Copyright (C) 2020 André Guerreiro - <aguerreiro1985@gmail.com>
 *
 * Licensed under the EUPL V.1.2

****************************************************************************-*/

#include "PAdESExtender.h"

#include "cryptoFwkPteid.h"
#include "Log.h"
#include "Util.h"
#include "MWException.h"
#include "eidErrors.h"
#include "APLCertif.h"
#include "TsaClient.h"
#include "MiscUtil.h"
#include "sign-pkcs7.h"
#include "CRLFetcher.h"
#include "poppler/PDFDoc.h"

#include <openssl/pkcs7.h>
#include <openssl/x509.h>
#include <openssl/ocsp.h>

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
        int tsTokenLen = 0;
        unsigned char *tsToken = NULL;

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
            delete[] hexToken;

        if (to_sign)
            free(to_sign);

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
                MWLOG(LEV_INFO, MOD_APL, "%s: Found issuer: %s", eidstore.getCert(i)->getOwnerName(), __FUNCTION__);
                break;
            }

        }

        return issuer_found;
    }

    /* 
       Extract the OCSP signing certificate from the response and add it to m_validationData
       The return value means if we need to add revocation info (CRL) for the OCSP certificate itself
    */
    bool PAdESExtender::addOCSPCertToValidationData(CByteArray &ocsp_response_ba, CByteArray &out_ocsp_cert) {

        bool check_revocation = false;
        const unsigned char *p = ocsp_response_ba.GetBytes();
        OCSP_RESPONSE *resp = d2i_OCSP_RESPONSE(NULL, &p, ocsp_response_ba.Size());

        if (resp != NULL) {
            OCSP_BASICRESP * basic_resp = OCSP_response_get1_basic(resp);
            if (basic_resp != NULL) {
                const STACK_OF(X509) * certs = OCSP_resp_get0_certs(basic_resp);
                for (int i = 0; certs && i < sk_X509_num(certs); i++) {
                    MWLOG(LEV_DEBUG, MOD_APL, "Adding one certificate to DSS from OCSP response");
                    X509 *cert = sk_X509_value(certs, i);
                    //Check for presence of the "OCSP No-Check" extension from the beginning
                    int index = X509_get_ext_by_NID(cert, NID_id_pkix_OCSP_noCheck, -1);
                    check_revocation = (index == -1);

                    unsigned char * der_data = NULL;
                    int data_len = X509_to_DER(cert, &der_data);
                    ValidationDataElement vde(der_data, data_len, ValidationDataElement::CERT);
                    addValidationElement(vde);
                    //Return OCSP cert to caller
                    out_ocsp_cert.Append(der_data, data_len);
                }

            }
            OCSP_RESPONSE_free(resp);

        }
        else {
            MWLOG(LEV_WARN, MOD_APL, "%s: Failed to decode OCSP response!", __FUNCTION__);
        }
        
        return check_revocation;
    }

    unsigned long PAdESExtender::getCertUniqueId(const unsigned char * data, int dataSize)
    {
        X509 *pX509 = NULL;
        pX509 = d2i_X509(&pX509, &data, dataSize);
        long cert_unique_id = X509_issuer_and_serial_hash(pX509);

        X509_free(pX509);

        return cert_unique_id;
    }

    ValidationDataElement* PAdESExtender::addValidationElement(ValidationDataElement &elem)
    {
   
        if (elem.getType() == ValidationDataElement::CERT)
        {
            signed long uniqueCertId = getCertUniqueId(elem.getData(), elem.getSize());
            if (m_certsInDoc.find(uniqueCertId) != m_certsInDoc.end())
            {
                return m_certsInDoc.at(uniqueCertId);
            }

            /* If the cert has not been added to validation data, create new element. */
            ValidationDataElement *newElem = new ValidationDataElement(elem);
            m_certsInDoc.insert(std::pair<unsigned long, ValidationDataElement *>(uniqueCertId, newElem));
            m_validationData.push_back(newElem);
            return newElem;
        }
        ValidationDataElement *newElem = new ValidationDataElement(elem);
        m_validationData.push_back(newElem);
        return newElem;
    }

    void PAdESExtender::removeValidationElement(ValidationDataElement *elem)
    {

        if (elem->getType() == ValidationDataElement::CERT)
        {
            signed long docCertId = getCertUniqueId(elem->getData(), elem->getSize());
            vector<ValidationDataElement *>::iterator it = m_validationData.begin();
            while(it != m_validationData.end())
            {
                ValidationDataElement *newVde = *it;
                if (newVde->getType() == ValidationDataElement::CERT)
                {
                    signed long newVdeId = getCertUniqueId(newVde->getData(), newVde->getSize());
                    if (newVdeId == docCertId)
                    {
                        it = m_validationData.erase(it);
                        continue;
                    }
                }
                it++;
            }
        }
    }

    bool PAdESExtender::addCRLRevocationInfo(CByteArray & cert, std::unordered_set<string> vri_keys) {

        CRLFetcher crlFetcher;
        std::string crlUrl;
        APL_CryptoFwkPteid *cryptoFwk = AppLayer.getCryptoFwk();
        if (!cryptoFwk->GetCDPUrl(cert, crlUrl))
        {
            MWLOG(LEV_ERROR, MOD_APL, "addCRLRevocationInfo(): Couldn't parse CRL URL from certificate");
            return false;
        }
        CByteArray crl = crlFetcher.fetch_CRL_file(crlUrl.c_str());

        if (crl.Size() == 0) {
            MWLOG(LEV_ERROR, MOD_APL, "Network error fetching CRL from %s or empty response. Revocation info is incomplete!", crlUrl.c_str());
            return false;
        }

        ValidationDataElement crlElem(crl.GetBytes(), crl.Size(), ValidationDataElement::CRL, vri_keys);
        addValidationElement(crlElem);

        return true;
    }

    bool isSignerCertificate(X509 * cert) {
        uint32_t ext_key_usage = X509_get_extended_key_usage(cert);

        //Exclude OCSP signing and TSA certificates
        if (ext_key_usage != UINT32_MAX) {
            if (XKU_OCSP_SIGN & ext_key_usage)
                return false;
            else if (XKU_TIMESTAMP & ext_key_usage)
                return false;
        }
        //Exclude CA certificates
        uint32_t key_usage = X509_get_key_usage(cert);
        if (key_usage != UINT32_MAX) {
            if (KU_KEY_CERT_SIGN & key_usage)
                return false;
        }

        return true;
    }

    using PKCS7_ptr = std::unique_ptr<PKCS7, decltype(&::PKCS7_free)>;

    bool PAdESExtender::addLT()
    {
        // TODO: verify if T first. Extend if not
        bool success = true;

        CByteArray issuerCertDataByteArray;
        size_t issuerLen;
        FWK_CertifStatus status = FWK_CERTIF_STATUS_UNCHECK;

        m_signedPdfDoc->m_incrementalMode = true;

        PDFDoc *doc = m_signedPdfDoc->m_doc;
        APL_CryptoFwkPteid *cryptoFwk = AppLayer.getCryptoFwk();

        const char *hexHash = NULL;
        unsigned char *signatureContents = NULL;

        std::vector<size_t> signerCerts_idx;
        std::unordered_set<int> sigIndexes = doc->getSignaturesIndexesUntilLastTimestamp();

        if (sigIndexes.empty()) {
            MWLOG(LEV_ERROR, MOD_APL, "addLT(): No signatures found in the document. Pre-condition for addLT() is broken!");
            success = false;
            goto cleanup;
        }
        
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

            unsigned char * ptr = signatureContents;
            PKCS7_ptr p7(d2i_PKCS7(NULL, (const unsigned char**)&signatureContents, length), ::PKCS7_free);
            free(ptr);

            if (p7.get() == NULL)
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
            STACK_OF(PKCS7_SIGNER_INFO) * signer_info = PKCS7_get_signer_info(p7.get());
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

                    for (int i = 0; i < sk_X509_num(ts_certs); i++)
                    {
                        sk_X509_push(certs, sk_X509_value(ts_certs, i));
                    }
                }
            }
            
            /* Iterate over the certificates in this signature and add them to DSS.*/
            for (int i = 0; certs && i < sk_X509_num(certs); i++) {
                unsigned char *certBytes = NULL;
                X509 *cert = sk_X509_value(certs, i);

                unsigned int len = i2d_X509(cert, &certBytes);
                if (len < 0)
                {
                    MWLOG(LEV_ERROR, MOD_APL, "addLT(): Failed to parse certificate in signature.");
                    success = false;
                    goto cleanup;
                }
                size_t vd_size = m_validationData.size();

                ValidationDataElement certElem(certBytes, len, ValidationDataElement::CERT);
                ValidationDataElement *addedElem = addValidationElement(certElem);
                addedElem->addVriKey(hexHash);
                //Add to signerCerts_idx if it's a signing cert and ignore duplicates
                if (isSignerCertificate(cert) && m_validationData.size() > vd_size) {

                    signerCerts_idx.push_back(m_validationData.size()-1);
                }
            }
        }

        //Add revocation info for all signer certificates preferably using OCSP
        for (auto signer_cert_idx: signerCerts_idx)
        {
            CByteArray ocsp_response;
            bool ocsp_check_revocation = false;
            bool foundIssuer = false;
            ValidationDataElement *vd_elem = m_validationData[signer_cert_idx];
            CByteArray signer_cert(vd_elem->getData(), vd_elem->getSize());
			//Replaced full subject by "issuer - serial" for privacy reasons
            //MWLOG(LEV_DEBUG, MOD_APL, "%s: adding revocation info for certificate: %s", __FUNCTION__, certificate_subject_from_der(signer_cert));

			MWLOG(LEV_DEBUG, MOD_APL, "%s: adding revocation info for cert issued by: %s", __FUNCTION__, certificate_issuer_serial_from_der(signer_cert).c_str());
            
            //Find issuer for each signing cert
            for (size_t j = 0; j < m_validationData.size(); j++)
            {
                if (m_validationData[j]->getType() != ValidationDataElement::CERT)
                    continue;

                issuerLen = m_validationData[j]->getSize();
                issuerCertDataByteArray.ClearContents();
                issuerCertDataByteArray.Append(m_validationData[j]->getData(), issuerLen);

                if (cryptoFwk->isIssuer(signer_cert, issuerCertDataByteArray)) {
                    foundIssuer = true;
                    break;
                }
            }

            /* Searching in eidstore may be useful to support more CA certificates in PADES-LT other than CC and CMD */
            if (!foundIssuer) {
                //Try to find issuer in eidstore certificates
                foundIssuer = findIssuerInEidStore(cryptoFwk, signer_cert, issuerCertDataByteArray);
                if (foundIssuer) {
                    ValidationDataElement vde(issuerCertDataByteArray.GetBytes(), 
                        issuerCertDataByteArray.Size(), ValidationDataElement::CERT, vd_elem->getVriHashKeys());
                    addValidationElement(vde);
                }

            }

            if (foundIssuer)
            {
                status = cryptoFwk->GetOCSPResponse(signer_cert, issuerCertDataByteArray, &ocsp_response, false);
                
            }
            else {
                MWLOG(LEV_WARN, MOD_APL, "%s: Couldn't find issuer for signing cert. Revocation info is going to be fetched from CRL", __FUNCTION__);
            }

            if (status == FWK_CERTIF_STATUS_VALID) {
                CByteArray ocsp_responder_cert;
                ocsp_check_revocation = addOCSPCertToValidationData(ocsp_response, ocsp_responder_cert);

                ValidationDataElement ocspResponseElem(ocsp_response.GetBytes(), ocsp_response.Size(), ValidationDataElement::OCSP, vd_elem->getVriHashKeys());
                addValidationElement(ocspResponseElem);

                if (ocsp_check_revocation) {
                    addCRLRevocationInfo(ocsp_responder_cert, vd_elem->getVriHashKeys());
                }
            }
            if (status == FWK_CERTIF_STATUS_REVOKED || status == FWK_CERTIF_STATUS_SUSPENDED)
            {
                MWLOG(LEV_ERROR, MOD_APL, "%s: OCSP validation: revoked certificate", __FUNCTION__);
                success = false;
                goto cleanup;
            }
            else if (status == FWK_CERTIF_STATUS_UNKNOWN) {
                MWLOG(LEV_WARN, MOD_APL, "%s: OCSP server returned unknown status so it's either a server error or the request is buggy or uses unsupported algorithm/feature", __FUNCTION__);
                success = false;
                goto cleanup;
            }
            else if (!foundIssuer || status == FWK_CERTIF_STATUS_ERROR) {
                /* Use CRL if there is no OCSP response or we couldn't find the issuer (it should never happen with CC or CMD signatures) */
                success = addCRLRevocationInfo(signer_cert, vd_elem->getVriHashKeys());
                if (!success) {
                    goto cleanup;
                }
            }
        
        }  //End of outer loop

        /* Remove from m_validationData the certs already present in document DSS. */
        {
            std::vector<ValidationDataElement*> certsAlreadyAdded;
            doc->getCertsInDSS(&certsAlreadyAdded);

            for (auto cert : certsAlreadyAdded)
            {
                removeValidationElement(cert);
                delete cert;
            }
        }

        doc->addDSS(m_validationData);
        m_signedPdfDoc->save();

    cleanup:
        if (hexHash)
            delete[] hexHash;

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
