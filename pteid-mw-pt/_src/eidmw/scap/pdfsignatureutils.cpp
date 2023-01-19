/*-****************************************************************************

 * Copyright (C) 2017-2018 André Guerreiro - <aguerreiro1985@gmail.com>
 * Copyright (C) 2017-2021 Adriano Campos - <adrianoribeirocampos@gmail.com>
 * Copyright (C) 2019 Miguel Figueira - <miguel.figueira@caixamagica.pt>
 * Copyright (C) 2022 José Pinto - <jose.pinto@caixamagica.pt>
 *
 * Licensed under the EUPL V.1.2

****************************************************************************-*/

#include "pdfsignatureutils.h"
#include "eidlib.h"

#include <string.h>

//OpenSSL includes for loadPKCS7Object() and X509_to_PEM
#include <openssl/asn1.h>
#include <openssl/x509.h>
#include <openssl/pkcs7.h>
#include <openssl/pem.h>
#include <openssl/err.h>

#include <QFile>
#include "MiscUtil.h"
#include "Log.h"
#include "cryptoFramework.h" // for SHA256_Wrapper...

#include "scapsettings.h"

namespace eIDMW {

static void signatureDataFromPkcs7Object(QByteArray &sigBinary, SignatureDetails &sigDetails)
{
    STACK_OF(PKCS7_SIGNER_INFO) *sinfos = NULL;
    PKCS7_SIGNER_INFO *si;
    unsigned char *authenticated_attr_buf = NULL;
    int auth_attr_len = 0, num_certificates = 0;
    X509 *cert = NULL;
    //SHA-256 hash size: that's the hash function in use for the SCAP citizen signature
    unsigned char documentHash[32];

    /* Load the human readable error strings for libcrypto */
    ERR_load_crypto_strings();

    /* Load all digest and cipher algorithms */
    OpenSSL_add_all_algorithms();

    memset(documentHash, 0, sizeof(documentHash));
    const char *binaryData = sigBinary.constData();

    PKCS7 *p7 = d2i_PKCS7(NULL, (const unsigned char **)&binaryData, sigBinary.size());

    if (!p7) {
        MWLOG(LEV_ERROR, MOD_SCAP, "%s: Error decoding the PKCS7 object from binary data of size %d", __FUNCTION__, sigBinary.size());
        return;
    }

    STACK_OF(X509) *signers = PKCS7_get0_signers(p7, NULL, 0);

    if (!signers) {
        MWLOG(LEV_ERROR, MOD_SCAP, "%s: Malformed PKCS7 object: does not contain signers!", __FUNCTION__);
        goto cleanup;
    }

    num_certificates = sk_X509_num(signers);
    if (num_certificates < 1) {
        MWLOG(LEV_ERROR, MOD_SCAP, "%s: Malformed PKCS7 object: no certificates in signer_info!", __FUNCTION__);
        goto cleanup;
    }

    cert = sk_X509_value(signers, 0);

    if (cert == NULL) {
        MWLOG(LEV_ERROR, MOD_SCAP, "%s: Malformed PKCS7 object: failed to build signing_certificate object!", __FUNCTION__);
        goto cleanup;
    }
    else {
        sigDetails.signing_certificate = X509_to_PEM(cert);

        sinfos = PKCS7_get_signer_info(p7);
        if (!sinfos || sk_PKCS7_SIGNER_INFO_num(sinfos) != 1) {
            MWLOG(LEV_ERROR, MOD_SCAP, "%s: Malformed PKCS7 object: signer_info %p", __FUNCTION__, sinfos);
            goto cleanup;
        }
        si = sk_PKCS7_SIGNER_INFO_value(sinfos, 0);

        ASN1_OCTET_STRING *enc_digest = si->enc_digest;
        const unsigned char *octet_str_data = enc_digest->data;
        sigDetails.signature = QByteArray((const char *)octet_str_data, enc_digest->length);

        auth_attr_len = ASN1_item_i2d((ASN1_VALUE *)si->auth_attr, &authenticated_attr_buf,
                                      ASN1_ITEM_rptr(PKCS7_ATTR_SIGN));

        if (auth_attr_len > 0) {
            SHA256_Wrapper(authenticated_attr_buf, auth_attr_len, documentHash);
            sigDetails.document_hash = QByteArray((const char *)documentHash, sizeof(documentHash));
        } else {
            MWLOG(LEV_ERROR, MOD_SCAP, "%s: Error loading the authenticated attributes (input for documentHash) from PKCS7 data", __FUNCTION__);
        }
    }

cleanup:
    PKCS7_free(p7);

}

/* Grab Hash, signature and certificate from the CC or CMD signed file */
bool get_citizen_signature_details(const QString &filepath, SignatureDetails &sig_details)
{

    // First we need to find the last instance of the hex-encoded Signature contents string
    static const char *needleValues = "/Type /Sig /Contents <";
    static const char endNeedle = '>';

    // Get PDF File
    QFile file(filepath);

    if (!file.open(QIODevice::ReadOnly)) {
        MWLOG(LEV_ERROR, MOD_SCAP, "%s: Error opening file for reading", __FUNCTION__);
        return false; // Error opening file
    }

    QByteArray needle = QByteArray::fromRawData(needleValues, strlen(needleValues));
    QByteArray fileByteArray = file.readAll();
    int pdfBinaryLen = fileByteArray.size();

    int indexOfNeedle = fileByteArray.lastIndexOf(needle);
    if (indexOfNeedle < 0) {
		MWLOG(LEV_ERROR, MOD_SCAP,
                  "Could not find signature contents string. Len = %d", pdfBinaryLen);
        return false;
    }
    indexOfNeedle += needle.length();

    int i = indexOfNeedle;
    int endOfNeedle = 0;
    while (i != pdfBinaryLen) {
        if (fileByteArray.at(i) == endNeedle) {
            endOfNeedle = i;
            break;
        }
        i++;
    }

    if (endOfNeedle == 0) {
		MWLOG(LEV_ERROR, MOD_SCAP,
                  "Could not find signature contents string (2). Len = %d", pdfBinaryLen);
        return false;
    }
    QByteArray signatureContents = fileByteArray.mid(indexOfNeedle, endOfNeedle - indexOfNeedle);

    QByteArray signatureContentsBinary = QByteArray::fromHex(signatureContents);

    signatureDataFromPkcs7Object(signatureContentsBinary, sig_details);

    if (sig_details.signing_certificate.size() == 0) {
		MWLOG(LEV_ERROR, MOD_SCAP, "Signing certificate size returned zero");
        return false;
    }

    return true;
}

};