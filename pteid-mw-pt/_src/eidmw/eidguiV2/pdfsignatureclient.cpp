/*-****************************************************************************

 * Copyright (C) 2017-2018 André Guerreiro - <aguerreiro1985@gmail.com>
 * Copyright (C) 2017-2021 Adriano Campos - <adrianoribeirocampos@gmail.com>
 * Copyright (C) 2019 Miguel Figueira - <miguel.figueira@caixamagica.pt>
 *
 * Licensed under the EUPL V.1.2

****************************************************************************-*/

#include "pdfsignatureclient.h"
#include "scapsignature.h"
#include "treeiteminfo.h"
#include "qfileinfo.h"
#include "qfile.h"
#include <string.h>
#include <codecvt>

#include "ScapSettings.h"
#include "gapi.h"
#include "eidlibdefines.h"
#include "Util.h"
#include "Config.h"
#include "totp_gen.h"

//applayer and common headers
#include "MiscUtil.h"
#include "PDFSignature.h"
#include "ByteArray.h"

//OpenSSL includes for loadPKCS7Object() and X509_to_PEM
#include <openssl/asn1.h>
#include <openssl/x509.h>
#include <openssl/bio.h>
#include <openssl/pkcs7.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/err.h>

#include "SCAP-services-v3/SCAPH.h"
#include "SCAP-services-v3/SCAPAuthorizationServiceSoapBindingProxy.h"
#include "SCAP-services-v3/SCAPSignatureServiceSoapBindingProxy.h"

#include "eidlib.h"
#include "MWException.h"

PDFSignatureClient::PDFSignatureClient()
{
    {
        eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_GENERAL_SCAP_APPID);

        m_appID = config.getString();
        is_last_signature = false;

        /*qDebug() << "m_appID = " << m_appID;*/
    }

}

/*
TODO:
OpenSSL utility functions: maybe move this into seperate file ??
*/

ns1__AttributeType *convertAttributeType(ns3__AttributeType *, soap *);

unsigned int SHA256_Wrapper(unsigned char *data, unsigned long data_len, unsigned char *digest)
{

    EVP_MD_CTX *cmd_ctx = EVP_MD_CTX_new();
    unsigned int md_len = 0;
 
    //Calculate the hash from the data
    EVP_DigestInit(cmd_ctx, EVP_sha256());
    EVP_DigestUpdate(cmd_ctx, data, data_len);
    EVP_DigestFinal(cmd_ctx, digest, &md_len);
 
    EVP_MD_CTX_free(cmd_ctx);

    return md_len;

}

void loadPkcs7Object(QByteArray &sigBinary, SignatureDetails &sigDetails)
{

    STACK_OF(PKCS7_SIGNER_INFO) *sinfos = NULL;
    PKCS7_SIGNER_INFO *si;
    unsigned char *authenticated_attr_buf = NULL;
    int auth_attr_len = 0;
    unsigned char documentHash[32];

    /* Load the human readable error strings for libcrypto */
    ERR_load_crypto_strings();

    /* Load all digest and cipher algorithms */
    OpenSSL_add_all_algorithms();

    memset(documentHash, 0, sizeof(documentHash));
    const char *binaryData = sigBinary.constData();

    PKCS7 *p7 = d2i_PKCS7(NULL, (const unsigned char **)&binaryData, sigBinary.size());

    if (!p7) {
        qDebug() << "Error loading the PKCS7 object from binary data";
        return;
    }

    STACK_OF(X509) *signers = PKCS7_get0_signers(p7, NULL, 0);

    if (!signers) {
        //TODO
        qDebug() << "Error loading the PKCS7 object from binary data";
        return;
    }

    int num_certificates = sk_X509_num(signers);

    //TODO: signers always contains 1 cert ??
    X509 *cert = sk_X509_value(signers, 0);

    if (cert == NULL) {

    } else {
        sigDetails.signing_certificate = X509_to_PEM(cert);

        sinfos = PKCS7_get_signer_info(p7);
        if (!sinfos || sk_PKCS7_SIGNER_INFO_num(sinfos) != 1) {

            //TODO
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
            qDebug() << "Error loading the authenticated attributes (input for documentHash) from PKCS7 data";
            return;
        }
    }

}

/* Grab Hash, signature and certificate from the CC or CMD signed file */
int getCitizenSignatureDetails(QString filepath, SignatureDetails &sigDetails)
{

    // First we need to find the last instance of the hex-encoded Signature contents string
    static const char *needleValues = "/Type /Sig /Contents <";
    static const char endNeedle = '>';

    // Get PDF File
    QFile file(filepath);

    if (!file.open(QIODevice::ReadOnly)){
        PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "ScapSignature",
                  "Error opening file");
        return GAPI::ScapGenericError; // Error opening file
    }

    QByteArray needle = QByteArray::fromRawData(needleValues, strlen(needleValues));
    QByteArray fileByteArray = file.readAll();
    int pdfBinaryLen = fileByteArray.size();

    int indexOfNeedle = fileByteArray.lastIndexOf(needle);
    if (indexOfNeedle < 0) {
        qDebug() << "Could not find signature contents string. Len = " << pdfBinaryLen;
        PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "ScapSignature",
                  "Could not find signature contents string. Len = %d",pdfBinaryLen);
        return GAPI::ScapGenericError;
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
        qDebug() << "Could not find contents string (2). Len = %d" << pdfBinaryLen;
        PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "ScapSignature",
                  "Could not find signature contents string (2). Len = %d",pdfBinaryLen);
        return GAPI::ScapGenericError;
    }
    QByteArray signatureContents = fileByteArray.mid(indexOfNeedle, endOfNeedle - indexOfNeedle);

    QByteArray signatureContentsBinary = QByteArray::fromHex(signatureContents);

    loadPkcs7Object(signatureContentsBinary, sigDetails);

    if (sigDetails.signing_certificate.size() == 0){
        PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "ScapSignature",
                  "Signing certificate size returned zero");
        return GAPI::ScapGenericError;
    }
    else
        return GAPI::ScapSucess;
}

ns1__AttributeType *convertAttributeType(ns3__AttributeType *attributeType, soap *sp)
{
    ns1__AttributeType *convertedAttribute = soap_new_ns1__AttributeType(sp);

    // Attribute suppliers
    ns1__AttributeSupplierType *attrSuppl = soap_new_ns1__AttributeSupplierType(sp);
    attrSuppl->Id = attributeType->AttributeSupplier->Id;
    attrSuppl->Name = attributeType->AttributeSupplier->Name;

    // Main Attribute
    ns1__MainAttributeType *main_attrib = soap_new_ns1__MainAttributeType(sp);
    main_attrib->AttributeID = attributeType->MainAttribute->AttributeID;
    main_attrib->Description = attributeType->MainAttribute->Description;

    //Main LegalAct List
    ns1__LegalActListType *legalActList = soap_new_ns1__LegalActListType(sp);

    if (attributeType->MainAttribute->LegalActList != NULL) {
        legalActList->LegalAct = attributeType->MainAttribute->LegalActList->LegalAct;
        main_attrib->LegalActList = legalActList;
    }

    if (attributeType->MainAttribute->SubAttributeList != NULL) {

        //Main Sub AttributeList
        ns1__SubAttributeListType *subAttrList = soap_new_ns1__SubAttributeListType(sp);
        for (uint i = 0; i < attributeType->MainAttribute->SubAttributeList->SubAttribute.size(); i++) {
            ns3__SubAttributeType *acSubAttr = attributeType->MainAttribute->SubAttributeList->SubAttribute.at(
                                                   i);
            ns1__SubAttributeType *subAttr = soap_new_ns1__SubAttributeType(sp);
            subAttr->AttributeID = acSubAttr->AttributeID;
            subAttr->Description = acSubAttr->Description;
            subAttr->Value = acSubAttr->Value;
            subAttrList->SubAttribute.push_back(subAttr);
        }

        main_attrib->SubAttributeList = subAttrList;
    }

    // Final
    convertedAttribute->AttributeSupplier = attrSuppl;
    convertedAttribute->MainAttribute = main_attrib;

    return convertedAttribute;
}

QByteArray PDFSignatureClient::openSCAPSignature(const char *inputFile, const char *outputPath,
                                                 std::string certChain, QString citizenName, QString citizenId,
                                                 QString attributeSupplier,  QString attribute,
                                                 PDFSignatureInfo signatureInfo, bool isVisible, bool isTimestamp, bool isCC,
                                                 bool useCustomImage, QByteArray &m_jpeg_scaled_data)
{
    qDebug() << "openSCAPSignature inputFile: " << inputFile << " outputPath: " << outputPath;

    local_pdf_handler = new PTEID_PDFSignature(inputFile);

    PDFSignature *sig_handler = local_pdf_handler->getPdfSignature();
        
    if (useCustomImage && isVisible) {
        const PTEID_ByteArray imageData(reinterpret_cast<const unsigned char *>(
            m_jpeg_scaled_data.data()), static_cast<unsigned long>(m_jpeg_scaled_data.size()));
        sig_handler->setCustomImage(
            const_cast<unsigned char *>(imageData.GetBytes()), imageData.Size());
    }

    std::vector<std::string> certs = toPEM((char *)certChain.c_str(), certChain.size());

    std::vector<CByteArray> certificatesData;

    for (size_t i = 0; i != certs.size(); i++) {
        CByteArray ba;
        unsigned char *der = NULL;
        int derLen = PEM_to_DER((char *)certs.at(i).c_str(), &der);

        if ( derLen < 0 ) {
            eIDMW::PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_ERROR, "ScapSignature",
                             "PEM -> DER conversion failed - len: %d", derLen );
            return QByteArray();

        }
        ba.Append((const unsigned char *)der, (unsigned long)derLen);
        certificatesData.push_back(ba);
    }

    if (isVisible) {
        if (signatureInfo.getX() >= 0 && signatureInfo.getY() >= 0) {
            sig_handler->setVisibleCoordinates(
                        signatureInfo.getSelectedPage() > 0 ? signatureInfo.getSelectedPage() : sig_handler->getPageCount(),
                        signatureInfo.getX(), signatureInfo.getY());
            sig_handler->setCustomSealSize(signatureInfo.getSealWidth(),signatureInfo.getSealHeight());
        }
    }
    sig_handler->setSCAPAttributes(strdup(citizenName.toUtf8().constData()),
                                   strdup(citizenId.toUtf8().constData()),
                                   strdup(attributeSupplier.toUtf8().constData()), strdup(attribute.toUtf8().constData()));

    sig_handler->setExternCertificate(certificatesData.at(0));
    APL_SignatureLevel level = isTimestamp ? APL_SignatureLevel::LEVEL_TIMESTAMP : APL_SignatureLevel::LEVEL_BASIC;

    if (signatureInfo.isLtv()) {
        if (is_last_signature)
            level = APL_SignatureLevel::LEVEL_LTV;
        else
            level = APL_SignatureLevel::LEVEL_LT;
    }

    sig_handler->setSignatureLevel(level);

    std::vector<CByteArray> caCerts(certificatesData.begin() + 1, certificatesData.end());

    sig_handler->setExternCertificateCA(caCerts);

    // Set the correct image logo, CC, CMD or custom
    isCC ? sig_handler->setIsCC(true) : sig_handler->setIsCC(false);

    //Add PDF signature objects right before we call getHash()
    try{
        sig_handler->signFiles(signatureInfo.getLocation(), signatureInfo.getReason(), outputPath, false);
    }
    catch (eIDMW::CMWException &e) {
        e.GetError();
        throw PTEID_Exception(e.GetError());
    }

    /* Calculate hash */
    CByteArray hashByteArray = sig_handler->getHash();
    if ( hashByteArray.Size() == 0 ) {
        eIDMW::PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_ERROR, "ScapSignature", "PDFSignature::getHash() failed" );
    }

    unsigned char sha256SigPrefix[] = {
        0x30, 0x31, 0x30, 0x0d, 0x06, 0x09,
        0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x01,
        0x05, 0x00, 0x04, 0x20
    };

    CByteArray signatureInput(sha256SigPrefix, sizeof(sha256SigPrefix));
    signatureInput.Append(hashByteArray);

    return QByteArray((const char *)signatureInput.GetBytes(), signatureInput.Size());
}

void WriteToFile(const char *path, unsigned char *content, unsigned int content_len)
{
    FILE *f=NULL;

    f = fopen(path, "wb");

    if(f) {
        fwrite(content, content_len, 1, f);
        fclose(f);
    }

}


const int PDFSignatureClient::totp_digits = 6;
const int PDFSignatureClient::totp_step_time = 60;

const char *AUTHORIZATION_ENDPOINT = "/SCAPSignature/AuthorizationService";
const char *SIGNATURE_ENDPOINT = "/SCAPSignature/SignatureService";

unsigned char * PDFSignatureClient::callSCAPSignatureService(soap* sp, QByteArray documentHash,
                                                 ns1__TransactionType *transaction, unsigned int &signatureLen,
                                                 QString citizenId, int &error)
{
    ScapSettings settings;
   
    SignatureServiceSoapBindingProxy proxy(sp);

    _ns1__SignatureRequest sigRequest;
    _ns1__SignatureResponse sigResponse;

    sigRequest.AppId = m_appID.toStdString();
    sigRequest.ProcessId = *(this->processId);
    sigRequest.Transaction = transaction;

    m_secretKey = settings.getSecretKey(citizenId);

    /*qDebug() << "m_secretKey = " << m_secretKey.data();*/
    std::string new_totp = generateTOTP(m_secretKey, totp_digits, totp_step_time, time(NULL));

    if (new_totp.length() != totp_digits) {
        eIDMW::PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_ERROR, "ScapSignature", 
            "Error generating TOTP: possibly invalid key");
        return NULL;
    }


    sigRequest.TOTP = &new_totp;

    xsd__base64Binary *base64DocumentHash =
        soap_new_set_xsd__base64Binary(sp, (unsigned char *)documentHash.data(),
                                       documentHash.size(), NULL, NULL, NULL);
    sigRequest.Hash = *base64DocumentHash;

    std::string s_endpoint = QString("https://" + settings.getScapServerHost() + ":" +
                                     settings.getScapServerPort().append(SIGNATURE_ENDPOINT)).toStdString();

    proxy.soap_endpoint = s_endpoint.c_str();

    int rc = proxy.Signature(&sigRequest, sigResponse);

    if (rc != SOAP_OK) {
        qDebug() << "Error returned by Signature in SoapBindingProxy(). Error code: " << rc;
        error = handleError(rc, proxy.soap, __FUNCTION__);
        return NULL;
    }
    else {
         //Check for Success Status
        int status_code = std::stoi(sigResponse.Status->Code);
        if (status_code == 0 && sigResponse.DocumentSignature != NULL) {
            PTEID_LOG(PTEID_LOG_LEVEL_DEBUG, "ScapSignature",
                      "SignatureService returned with status code: %s and message: %s",
                      sigResponse.Status->Code.c_str(),
                      sigResponse.Status->Message.c_str());

            unsigned int sig_len = sigResponse.DocumentSignature->__size;

            qDebug() << "SignatureService returned signature size: " << sig_len;

            unsigned char * outSignature = (unsigned char*)malloc(sig_len);

            if ( outSignature == NULL ) {
                qDebug() << "Malloc fail!";
                return NULL;
            }

            memcpy(outSignature , sigResponse.DocumentSignature->__ptr, sig_len);

            signatureLen = sig_len;
            return outSignature;
        }
        else {
            eIDMW::PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_ERROR, "ScapSignature", 
                "SCAPSignatureService returned functional error code: %s and message: %s", sigResponse.Status->Code.c_str(),
                sigResponse.Status->Message.c_str());
            error = handleError(status_code, NULL, __FUNCTION__);
            return NULL;
        }
    }
    
}

int PDFSignatureClient::closeSCAPSignature(unsigned char * scap_signature, unsigned int sig_len) {

    qDebug() << "closeSCAPSignature called with signature size = " << sig_len * 8 << " bits";

    PDFSignature *sig_handler = local_pdf_handler->getPdfSignature();

    CByteArray baSCAPSignature(scap_signature, sig_len);
    return sig_handler->signClose(baSCAPSignature);
}

typedef int (*ParseHeaderFn)(struct soap*, const char*, const char*);

static ParseHeaderFn pParseHeader = nullptr;
static QString httpDate = "";

static int ParseHeader(struct soap * soap, const char * key, const char * val)
{
    if (!soap_tag_cmp(key, "Date"))
    {
          httpDate = soap_strdup(soap, val);
    }
    return pParseHeader(soap, key, val);
}

int PDFSignatureClient::signPDF(ProxyInfo proxyInfo, QString finalfilepath, QString filepath,
                                QString citizenName, QString citizenId, bool isTimestamp, bool isCC,
                                PDFSignatureInfo signatureInfo, std::vector<ns3__AttributeType *> &attributeTypeList,
                                bool useCustomImage, QByteArray &m_jpeg_scaled_data)
{
    // Get endpoint from settings
    ScapSettings settings;

    soap *sp = soap_new2(SOAP_C_UTFSTRING, SOAP_C_UTFSTRING);
    //soap * sp = soap_new2(SOAP_ENC_MTOM, SOAP_ENC_MTOM);

    //Define appropriate network timeouts
    sp->recv_timeout = RECV_TIMEOUT;
    sp->send_timeout = SEND_TIMEOUT;
    sp->connect_timeout = CONNECT_TIMEOUT;

    char * ca_path = NULL;
    std::string cacerts_file;

#ifdef __linux__
    ca_path = "/etc/ssl/certs";
    //Load CA certificates from file provided with pteid-mw
#else
    cacerts_file = utilStringNarrow(CConfig::GetString(CConfig::EIDMW_CONFIG_PARAM_GENERAL_CERTS_DIR)) + "/cacerts.pem";
#endif

    int ret = soap_ssl_client_context(sp, SOAP_SSL_DEFAULT,
        NULL,
        NULL,
        cacerts_file.size() > 0 ? cacerts_file.c_str() : NULL, /* cacert file to store trusted certificates (needed to verify server) */
        ca_path,
        NULL);

    if (ret != SOAP_OK) {
        eIDMW::PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_ERROR
            , "ScapSignature"
            , "Error in signPDF: Gsoap returned %d "
            , ret);
        qDebug() << "signPDF() returned error!" << ret;
        return GAPI::ScapGenericError;
    }

    AuthorizationServiceSoapBindingProxy proxy(sp);

    std::string proxy_host;
    long proxy_port;

    std::string s_endpoint = QString("https://" + settings.getScapServerHost() + ":" +
        settings.getScapServerPort().append(AUTHORIZATION_ENDPOINT)).toStdString();

    proxy.soap_endpoint = s_endpoint.c_str();

    if (proxyInfo.isAutoConfig()) 
    {
        proxyInfo.getProxyForHost(s_endpoint, &proxy_host, &proxy_port);
        if (proxy_host.size() > 0) {
            sp->proxy_host = proxy_host.c_str();
            sp->proxy_port = proxy_port;
         }
    }
    else if (proxyInfo.isManualConfig()) {
        sp->proxy_host = strdup(proxyInfo.getProxyHost().c_str());
        try {
            proxy_port = std::stol(proxyInfo.getProxyPort());
         }
        catch (...) {
            eIDMW::PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_ERROR, "ScapSignature", "Error parsing proxy port to number value.");
        }
        sp->proxy_port = proxy_port;
        if (proxyInfo.getProxyUser().size() > 0) {
            sp->proxy_userid = strdup(proxyInfo.getProxyUser().c_str());
            sp->proxy_passwd = strdup(proxyInfo.getProxyPwd().c_str());
        }

    }

    _ns1__AuthorizationRequest authorizationRequest;
    _ns1__AuthorizationResponse authorizationResponse;

    authorizationRequest.AppId = m_appID.toStdString();

    m_secretKey = settings.getSecretKey(citizenId);

    /*qDebug() << "m_secretKey = " << m_secretKey.data();*/
    std::string new_totp = generateTOTP(m_secretKey, totp_digits, totp_step_time, time(NULL));

    if (new_totp.length() != totp_digits) {
        eIDMW::PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_ERROR, "ScapSignature", 
            "Error generating TOTP: possibly invalid key");
        return GAPI::ScapSecretKeyError;
    }


    authorizationRequest.TOTP = new_totp;

    SignatureDetails sigDetails;

    //Get the details of the first citizen signature, performed via CC or CMD
    ret = getCitizenSignatureDetails(filepath, sigDetails);
    if (ret == GAPI::ScapSucess) {
        qDebug() << "getCitizenSignatureDetails() returned success!";
    } else {
        qDebug() << "getCitizenSignatureDetails() returned error!";
        PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_ERROR, "ScapSignature",
                  "Get Citizen signature details returned error: %d",ret);
        return GAPI::ScapGenericError;
    }

    xsd__base64Binary *base64DocumentSignature =
        soap_new_set_xsd__base64Binary(sp, (unsigned char *)sigDetails.signature.data(),
                                       sigDetails.signature.size(), NULL, NULL, NULL);

    xsd__base64Binary *base64DocumentHash =
        soap_new_set_xsd__base64Binary(sp, (unsigned char *)sigDetails.document_hash.data(),
                                       sigDetails.document_hash.size(), NULL, NULL, NULL);

    authorizationRequest.DocumentSignature = *base64DocumentSignature;
    authorizationRequest.DocumentHash = *base64DocumentHash;

    //Certificate is string in PEM format
    authorizationRequest.SignatureCertificate = std::string(sigDetails.signing_certificate);
    ns1__PersonalData *personalData = soap_new_req_ns1__PersonalData(sp, citizenName.toStdString(), citizenId.toStdString());
    
    authorizationRequest.PersonalData = personalData;

    std::vector<ns1__AttributeType *> attributes;
    for (uint pos = 0; pos < attributeTypeList.size(); pos++) {
        ns3__AttributeType *acAttributeType = attributeTypeList.at(pos);

        ns1__AttributeType *convertedAttribute = convertAttributeType(acAttributeType, sp);
        attributes.push_back(convertedAttribute);
    }

    // Create Attribute list type
    ns1__AttributeListType *attributeList = soap_new_req_ns1__AttributeListType(sp, attributes);

    authorizationRequest.AttributeList = attributeList;

    proxy.soap_endpoint = s_endpoint.c_str();

    qDebug() << "SCAP PDFSignatureClient:: Authorization endpoint: " << proxy.soap_endpoint;
            
    pParseHeader = proxy.soap->fparsehdr;

    proxy.soap->fparsehdr = ParseHeader;

    int rc = proxy.Authorization(&authorizationRequest, authorizationResponse);

    if (rc != SOAP_OK) {
        qDebug() << "Error returned by calling Authorization in SoapBindingProxy(). Error code: " << rc;
        return handleError(rc, proxy.soap, __FUNCTION__);
    }
    else {
        qDebug() << "Authorization service returned SOAP_OK";
        //Check for Success Status
        int status_code = std::stoi(authorizationResponse.Status->Code);
        if (status_code != 0 || authorizationResponse.TransactionList == NULL) {
            qDebug() << "authorizationService returned error";
            PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_ERROR, "ScapSignature",
                      "AuthorizationService returned error. Error code: %s, message: %s, ProcessID: %s",
                      authorizationResponse.Status->Code.c_str(),
                      authorizationResponse.Status->Message.c_str(),
				      authorizationResponse.ProcessId != NULL ? authorizationResponse.ProcessId->c_str() : "(null)");
            return handleError(status_code, NULL, __FUNCTION__);
        }

        PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_DEBUG, "ScapSignature",
                      "AuthorizationService returned with status code: %s, message: %s, ProcessID for current SCAP signature: %s",
                      authorizationResponse.Status->Code.c_str(),
                      authorizationResponse.Status->Message.c_str(),
			     authorizationResponse.ProcessId != NULL ? authorizationResponse.ProcessId->c_str() : "(null)");

        std::vector<ns1__TransactionType *> transactionList =
            authorizationResponse.TransactionList->Transaction;

        this->processId = authorizationResponse.ProcessId;

        const char * outputPath = NULL;
        const char * inputPath = strdup(filepath.toUtf8().constData());

        QTemporaryFile *tempFile = NULL;

        QString attributeListString = "";
        QString attributeSupplierListString = "";
        QString lastAttrSupplierString = "";
        QString lastAttrSupplierType = "";
        QString signOriginalReason = signatureInfo.getReason();
        QString signOriginalLocation = signatureInfo.getLocation();
        bool moreThanOneNext = false;
        std::vector <QTemporaryFile *> tempFiles;
        bool throwTimestampError = false;
        bool throwLTVError = false;

        for (unsigned int i = 0; i != transactionList.size(); i++) {
            bool isVisible = false;
            QByteArray signatureHash;
            ns1__MainAttributeType *mainAttribute = NULL;

            ns1__TransactionType *transaction = transactionList.at(i);

            mainAttribute = transaction->MainAttribute;

            //In this case we are adding a visible signature
            if (i == transactionList.size() - 1) {
                outputPath = strdup(finalfilepath.toUtf8().constData());
                attributeListString.append(QString::fromStdString("."));
                attributeSupplierListString.append(QString::fromStdString("."));
                isVisible = true;
            }
            else {
                // Creates a temporary file for every iteration except the last
                isVisible = false;
                ns1__TransactionType *transactionNext = transactionList.at(i+1);


                // A new attribute for a NEW attribute supplier;
                if(QString::fromStdString(transaction->AttributeSupplier->Name.c_str()) != lastAttrSupplierString){
                        qDebug() << "A new attribute for a NEW attribute supplier";

                        // Check if have more than one attribute from this attribute supplier
                        if(moreThanOneNext){
                            attributeListString.append(QString::fromStdString(" } "));
                        }
                        if(QString::fromStdString(transaction->AttributeSupplier->Name.c_str())
                                == QString::fromStdString(transactionNext->AttributeSupplier->Name.c_str())){
                            moreThanOneNext = true;
                        }else{
                            moreThanOneNext = false;
                        }
                        // Check if the first time
                        if(i == 0){
                            qDebug() << "The first time";
                            if(moreThanOneNext) attributeListString.append(QString::fromStdString(" { "));
                            attributeListString.append(QString::fromStdString(mainAttribute->Description->data()));
                        }else{
                            qDebug() << "Not the first time";
                            if(lastAttrSupplierType == "ENTERPRISE"){
                                attributeListString.append(QString::fromStdString(" de "));
                                attributeListString.append(lastAttrSupplierString);
                            }

                            attributeListString.append(QString::fromStdString(" e "));
                            if(moreThanOneNext) attributeListString.append(QString::fromStdString(" { "));
                            attributeListString.append(QString::fromStdString(mainAttribute->Description->data()));

                            // The LAST attributeSupplier
                            if(lastAttrSupplierType != "ENTERPRISE"){
                                if(i == transactionList.size() - 2){
                                    attributeSupplierListString.append(QString::fromStdString(" e "));
                                }else{
                                    if(QString::fromStdString(transactionNext->AttributeSupplier->Type->c_str()) == "ENTERPRISE"){
                                        attributeSupplierListString.append(QString::fromStdString(" e "));
                                    }else{
                                        attributeSupplierListString.append(QString::fromStdString(" , "));
                                    }
                                }
                            }
                        }
                        if(lastAttrSupplierType != "ENTERPRISE"){
                            if(QString::fromStdString(transaction->AttributeSupplier->Type->c_str()) == "INSTITUTION"
                                || QString::fromStdString(transaction->AttributeSupplier->Type->c_str()) == "EMPLOYEE"){
                                attributeSupplierListString.append(QString::fromStdString(transaction->AttributeSupplier->Name.c_str()));
                            }else{
                                attributeSupplierListString.append(QString::fromStdString("SCAP"));
                            }
                        }
                }

                //  A new attribute for the SAME attribute supplier
                if(QString::fromStdString(transaction->AttributeSupplier->Name.c_str()) == lastAttrSupplierString){
                        qDebug() << "A new attribute for the same attribute supplier";
                        if(QString::fromStdString(transaction->AttributeSupplier->Name.c_str())
                                == QString::fromStdString(transactionNext->AttributeSupplier->Name.c_str())){
                            attributeListString.append(QString::fromStdString(" , "));
                        }else{
                            attributeListString.append(QString::fromStdString(" e "));
                        }

                        attributeListString.append(QString::fromStdString(mainAttribute->Description->data()));
                }

                // The LAST attribute visible
                if(i == transactionList.size() - 2){
                        qDebug() << "The LAST attribute visible";
                        if(moreThanOneNext) attributeListString.append(QString::fromStdString(" } "));

                        if(QString::fromStdString(transaction->AttributeSupplier->Type->c_str()) == "ENTERPRISE")
                        {
                            attributeListString.append(QString::fromStdString(" de "));
                            attributeListString.append(QString::fromStdString(transaction->AttributeSupplier->Name.c_str()));
                        }
                }
                lastAttrSupplierString = QString::fromStdString(transaction->AttributeSupplier->Name.c_str());
                lastAttrSupplierType = QString::fromStdString(transaction->AttributeSupplier->Type->c_str());

                tempFile = new QTemporaryFile();
                tempFiles.push_back(tempFile);


                if (!tempFile->open()) {
                    PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_ERROR, "ScapSignature", "PDF Signature error: Error creating temporary file");
                    return GAPI::ScapGenericError;
                }
                outputPath = strdup(tempFile->fileName().toStdString().c_str());
            }

            // If signature is not visible then add attribute details in the reason field of the signature details
            // Else add original reason and location
            if(!isVisible)
            {
                QString signDetailsReason = "";
                QString signDetailsLocation = "";
                signDetailsReason.append("Entidade: " + QString::fromStdString(transaction->AttributeSupplier->Name.c_str()));
                signDetailsReason.append(". Na qualidade de: ");
                signDetailsReason.append(mainAttribute->Description->c_str());
                signDetailsReason.append(". Subatributos: ");

                if (mainAttribute->SubAttributeList != NULL) {
                    for (uint ii = 0; ii < mainAttribute->SubAttributeList->SubAttribute.size(); ii++) {
                        ns1__SubAttributeType *acSubAttr = mainAttribute->SubAttributeList->SubAttribute.at(ii);
                        if(ii != 0) signDetailsReason.append("; ");
                        if (acSubAttr->Description != NULL) {
                            signDetailsReason.append(QString::fromStdString(acSubAttr->Description->c_str()));
                        }
                        signDetailsReason.append(": ");
                        if (acSubAttr->Value != NULL) {
                            signDetailsReason.append(QString::fromStdString(acSubAttr->Value->c_str()));
                        }
                    }
                }
                signatureInfo.setReason(strdup(signDetailsReason.toStdString().c_str()));
                signatureInfo.setLocation(strdup(signDetailsLocation.toStdString().c_str()));
            }else{
                signatureInfo.setReason(strdup(signOriginalReason.toStdString().c_str()));
                signatureInfo.setLocation(strdup(signOriginalLocation.toStdString().c_str()));
            }

            // Only the last signature should be timestamped
            is_last_signature = (i == transactionList.size() - 1);
            signatureHash = openSCAPSignature(inputPath, outputPath,
                            transaction->AttributeSupplierCertificateChain, citizenName, citizenId,
                            attributeSupplierListString, attributeListString, signatureInfo, isVisible, isTimestamp && is_last_signature, isCC,
                            useCustomImage, m_jpeg_scaled_data);

            if (signatureHash.size() == 0) {
                PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_ERROR, "ScapSignature", "Open SCAP Signature failed! signatureHash invalid!");
                return GAPI::ScapGenericError;
            }

#ifdef DEBUG
            std::string filename = std::string("/tmp/transaction_")+ std::to_string(i) + "_hash.bin";

            WriteToFile(filename.c_str(), (unsigned char *)signatureHash.data(), signatureHash.size());
#endif

            int error = 0;
            unsigned int sig_len = 0;
            unsigned char * scap_signature = callSCAPSignatureService(sp, signatureHash,
                                                                        transaction, sig_len, citizenId, error);

            if (sig_len > 0) {
#ifdef DEBUG
                std::string filename = std::string("/tmp/transaction_")+ std::to_string(i) + "_signature.bin";
                WriteToFile(filename.c_str(), (unsigned char *)scap_signature, sig_len);
#endif
                try{
                    closeSCAPSignature(scap_signature, sig_len);
                }
                catch (eIDMW::CMWException &e) {
                    if (e.GetError() != EIDMW_TIMESTAMP_ERROR && e.GetError() != EIDMW_LTV_ERROR){
                        throw PTEID_Exception(e.GetError());
                    }
                    if (e.GetError() == EIDMW_TIMESTAMP_ERROR)
                        throwTimestampError = true;
                    else
                        throwLTVError = true;
                }
            }else{
                if (scap_signature == NULL && error != 0) {
                    return error;
                }
                PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_ERROR, "ScapSignature", "Call SCAP Signature Service failed! signature len invalid");
                return GAPI::ScapGenericError;
            }
            // Apply the next signature over the current one's output file
            inputPath = outputPath;
        }
        // Delete tempFiles the tempFiles
        for (auto& tempFile : tempFiles){
            delete tempFile;
        }
        if (throwLTVError)
            throw PTEID_Exception(EIDMW_LTV_ERROR);
            
        if (throwTimestampError)
            throw PTEID_Exception(EIDMW_TIMESTAMP_ERROR);
    }

    return GAPI::ScapSucess;
}

int handleError(int status_code, soap *sp, const char *call){
    if (sp != NULL) { // SOAP error
        if (status_code == SOAP_FAULT) {
            if (sp->fault != NULL && sp->fault->faultstring != NULL)
                PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "ScapSignature",
                            "Error returned by calling %s in SoapBindingProxy() returned SOAP Fault: %s", call,
                            sp->fault->faultstring);
            return GAPI::ScapGenericError;
        }

        PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "ScapSignature",
                        "Error returned by calling %s in SoapBindingProxy(). Error code: %d", call, status_code);

        if (status_code == SOAP_EOF || status_code == SOAP_TCP_ERROR) {
            return GAPI::ScapTimeOutError;
        }
    }
    else { // SCAP error
        if (status_code == SCAP_TOTP_FAILED_ERROR_CODE) {
            QDateTime serverTime = QDateTime::fromString(httpDate, Qt::RFC2822Date);
            long local = time(nullptr);
            long server = serverTime.toSecsSinceEpoch();
            qDebug() << "local: " << local << "server: " << server;

                PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_ERROR, "ScapSignature",
                            "%s returned tLocal: %ld tServer: %ld", call, local, server);

            if (abs(difftime(local,server)) > SCAP_MAX_CLOCK_DIFF) {
                return GAPI::ScapClockError;
            }
            else {
                return GAPI::ScapSecretKeyError;
            }
        }
        if (status_code == SCAP_ACCOUNT_MATCH_ERROR_CODE
                || status_code == SCAP_REQUEST_ERROR_CODE) {
            return GAPI::ScapNotValidAttributesError;
        }
        if (status_code == SCAP_ATTRIBUTES_EXPIRED) {
            return GAPI::ScapAttributesExpiredError;
        }
        if (status_code == SCAP_ZERO_ATTRIBUTES) {
            return GAPI::ScapZeroAttributesError;
        }
        if (status_code == SCAP_ATTRIBUTES_NOT_VALID) {
            return GAPI::ScapNotValidAttributesError;
        }
    }
    return GAPI::ScapGenericError;
}
