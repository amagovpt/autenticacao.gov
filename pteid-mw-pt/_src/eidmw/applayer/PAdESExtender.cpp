/*-****************************************************************************

 * Copyright (C) 2019 Miguel Figueira - <miguelblcfigueira@gmail.com>
 *
 * Licensed under the EUPL V.1.1

****************************************************************************-*/

#include "PAdESExtender.h"

#include "cryptoFwkPteid.h"
#include "poppler/PDFDoc.h"
#include "Log.h"
#include "Util.h"
#include "TsaClient.h"
#include "sign-pkcs7.h"

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

        const char *hexToken = NULL;
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

    bool PAdESExtender::addLT()
    {
        // TODO: verify if T first. Extend if not
        bool success = true; 

        m_signedPdfDoc->m_incrementalMode = true;

        PDFDoc *doc = m_signedPdfDoc->m_doc;

        /* Compute SHA1 of signature contents to add as key of VRI dict. */
        unsigned char *signatureContents = NULL;
        int length = doc->getSignatureContents(&signatureContents); // TODO: iterate over signatures
        APL_CryptoFwkPteid *cryptoFwk = AppLayer.getCryptoFwk();
        CByteArray contentBytes(signatureContents, length), hash;
        cryptoFwk->GetHashSha1(contentBytes, &hash);
        std::vector<const char *> vriKeyVector;
        const char *hexHash = NULL;
        hexHash = bin2AsciiHex(hash.GetBytes(), hash.Size());
        vriKeyVector.push_back(hexHash);

        std::vector<ValidationDataElement *> validationData;

        PKCS7 *p7 = NULL;
        p7 = d2i_PKCS7(NULL, (const unsigned char**)&signatureContents, length);
        if (p7 == NULL)
        {
            MWLOG(LEV_ERROR, MOD_APL,
                L"Error decoding signature content.");
            success = false;
            goto cleanup;
        }

        STACK_OF(X509) *certs = NULL;

        int i = OBJ_obj2nid(p7->type);
        if (i == NID_pkcs7_signed) {
            certs = p7->d.sign->cert;
        }
        else if (i == NID_pkcs7_signedAndEnveloped) {
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
                int i = OBJ_obj2nid(ts_p7->type);
                if (i == NID_pkcs7_signed) {
                    ts_certs = ts_p7->d.sign->cert;
                }
                else if (i == NID_pkcs7_signedAndEnveloped) {
                    ts_certs = ts_p7->d.signed_and_enveloped->cert;
                }

                for (size_t i = 0; i < sk_X509_num(ts_certs); i++)
                {
                    sk_X509_push(certs, sk_X509_value(ts_certs, i));
                }
            }
        }

        /* Iterate over the certificates in this signature and add them to DSS.*/
        for (i = 0; certs && i < sk_X509_num(certs); i++) {
            unsigned char *certBytes = NULL;
            X509 *cert = sk_X509_value(certs, i);
            //X509_print_fp(stdout, cert); //DEBUG
            unsigned int len = i2d_X509(cert, &certBytes);
            if (len < 0)
            {
                MWLOG(LEV_ERROR, MOD_APL, "Failed to parse certificate in signature.");
                return false;
            }

            ValidationDataElement *certElem = new ValidationDataElement(certBytes, len, ValidationDataElement::CERT, vriKeyVector);

            validationData.push_back(certElem);

            /* If not Root CA, check OCSP response for the current certificate and, if OK, add
            response to validation data. */
            if (i < sk_X509_num(certs) - 1)
            {
                unsigned char *issuerCertBytes = NULL;
                X509 *issuerCert = sk_X509_value(certs, i + 1); // Warning: this assumes chain order. Is it always true?
                unsigned int issuerLen = i2d_X509(issuerCert, &issuerCertBytes);
                if (len < 0)
                {
                    MWLOG(LEV_ERROR, MOD_APL, "Failed to parse an issuer certificate in signature.");
                    success = false;
                    goto cleanup;
                }

                CByteArray certDataByteArray(certBytes, len);
                CByteArray issuerCertDataByteArray(issuerCertBytes, issuerLen);
                CByteArray response;
                FWK_CertifStatus status = cryptoFwk->OCSPValidation(certDataByteArray, issuerCertDataByteArray, &response);
                if (status == FWK_CERTIF_STATUS_REVOKED)
                {
                    MWLOG(LEV_WARN, MOD_APL, "OCSP validation of certificate is false.");
                    success = false;
                    goto cleanup;
                }
                else if (status != FWK_CERTIF_STATUS_VALID)
                {
                    // TODO: CRL in case there is OCSP error
                    continue;
                }

                //const char *hexOCSP = bin2AsciiHex(response.GetBytes(), response.Size()); //DEBUG
                //printf("\n\n OCSP RESPONSE:\n\n%s\n\n", hexOCSP);

                ValidationDataElement *ocspResponseElem = new ValidationDataElement(response.GetBytes(), response.Size(), ValidationDataElement::OCSP, vriKeyVector);
                validationData.push_back(ocspResponseElem);
            }

        }

        doc->addDSS(validationData);
        m_signedPdfDoc->save();

    cleanup:
        if (hexHash)
            delete hexHash;
        
        return success;
    }

    bool PAdESExtender::addLTA()
    {
        // TODO: verify if LT first. Extend if not
        if (!addLT())
            return false;
        
        if (!addT())
            return false;

        return true;
    }
}