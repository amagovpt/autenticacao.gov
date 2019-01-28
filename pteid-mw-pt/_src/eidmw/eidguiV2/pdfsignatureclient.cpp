#include "pdfsignatureclient.h"
#include "scapsignature.h"
#include "treeiteminfo.h"
#include "qfileinfo.h"
#include "qfile.h"
#include <string.h>

#include "ScapSettings.h"
#include "gapi.h"
#include "eidlibdefines.h"

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

PDFSignatureClient::PDFSignatureClient()
{
    {
        eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_GENERAL_SCAP_APPID);

        m_appID = config.getString();

        qDebug() << "m_appID = " << m_appID;
    }

}

//Implemented in totp_gen.cpp
std::string generateTOTP(std::string secretKey);

/*
TODO:
OpenSSL utility functions: maybe move this into seperate file ??
*/

ns1__AttributeType *convertAttributeType(ns3__AttributeType *, soap *);

unsigned int SHA256_Wrapper(unsigned char *data, unsigned long data_len, unsigned char *digest)
{

    EVP_MD_CTX cmd_ctx;
    unsigned int md_len = 0;

    //Calculate the hash from the data
    EVP_DigestInit(&cmd_ctx, EVP_sha256());
    EVP_DigestUpdate(&cmd_ctx, data, data_len);
    EVP_DigestFinal(&cmd_ctx, digest, &md_len);

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
        fprintf(stderr, "Error loading the PKCS7 object from binary data\n");
        return;
    }

    STACK_OF(X509) *signers = PKCS7_get0_signers(p7, NULL, 0);

    if (!signers) {
        //TODO
        fprintf(stderr, "Error loading the PKCS7 object from binary data\n");
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
            fprintf(stderr,
                    "Error loading the authenticated attributes (input for documentHash) from PKCS7 data\n");
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

    if (!file.open(QIODevice::ReadOnly))
        return GAPI::ScapGenericError; // Error opening file

    QByteArray needle = QByteArray::fromRawData(needleValues, strlen(needleValues));
    QByteArray fileByteArray = file.readAll();
    int pdfBinaryLen = fileByteArray.size();

    int indexOfNeedle = fileByteArray.lastIndexOf(needle);
    if (indexOfNeedle < 0) {
        qDebug() << "Could not find signature contents string: " << pdfBinaryLen;
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
        qDebug() << "Could not find contents string (2): " << pdfBinaryLen;
        return GAPI::ScapGenericError;
    }
    QByteArray signatureContents = fileByteArray.mid(indexOfNeedle, endOfNeedle - indexOfNeedle);

    QByteArray signatureContentsBinary = QByteArray::fromHex(signatureContents);

    loadPkcs7Object(signatureContentsBinary, sigDetails);

    if (sigDetails.signing_certificate.size() == 0)
        return GAPI::ScapGenericError;
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
                                                 PDFSignatureInfo signatureInfo, bool isVisible, bool isCC)
{
    qDebug() << "openSCAPSignature inputFile: " << inputFile << " outputPath: " << outputPath;

    local_pdf_handler = new PTEID_PDFSignature(inputFile);

    PDFSignature *sig_handler = local_pdf_handler->getPdfSignature();

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
            sig_handler->setVisibleCoordinates(signatureInfo.getSelectedPage(), signatureInfo.getX(), signatureInfo.getY());
        } 
    }
    sig_handler->setSCAPAttributes(strdup(citizenName.toUtf8().constData()),
                                   strdup(citizenId.toUtf8().constData()),
                                   strdup(attributeSupplier.toUtf8().constData()), strdup(attribute.toUtf8().constData()));

    sig_handler->setExternCertificate(certificatesData.at(0));

    std::vector<CByteArray> caCerts(certificatesData.begin() + 1, certificatesData.end());

    sig_handler->setExternCertificateCA(caCerts);

    // Set the correct image logo, CC, CMD or custom
    isCC ? sig_handler->setIsCC(true) : sig_handler->setIsCC(false);

    //Add PDF signature objects right before we call getHash()
    sig_handler->signFiles(signatureInfo.getLocation(), signatureInfo.getReason(), outputPath);

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


const char *AUTHORIZATION_ENDPOINT = "/SCAPSignature/AuthorizationService";
const char *SIGNATURE_ENDPOINT = "/SCAPSignature/SignatureService";

unsigned char * PDFSignatureClient::callSCAPSignatureService(soap* sp, QByteArray documentHash,
                                                 ns1__TransactionType *transaction, unsigned int &signatureLen,
                                                             QString citizenId)
{
    ScapSettings settings;
   
    SignatureServiceSoapBindingProxy proxy(sp);

    _ns1__SignatureRequest sigRequest;
    _ns1__SignatureResponse sigResponse;

    sigRequest.AppId = m_appID.toStdString();
    sigRequest.ProcessId = *(this->processId);
    sigRequest.Transaction = transaction;

    m_secretKey = settings.getSecretKey(citizenId);

    qDebug() << "m_secretKey = " << m_secretKey.data();

    std::string new_totp = generateTOTP(m_secretKey);
    eIDMW::PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_DEBUG, "ScapSignature", "Generated TOTP: %s", new_totp.c_str());
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
        if (rc == SOAP_FAULT) {

            qDebug() << "SOAP Fault returned: TODO print fault message";
        }
        return NULL;
    }
    else {
         //Check for Success Status
        if (sigResponse.Status->Code == "00" && sigResponse.DocumentSignature != NULL) {
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


int PDFSignatureClient::signPDF(ProxyInfo proxyInfo, QString finalfilepath, QString filepath,
                                QString citizenName, QString citizenId,int ltv, bool isCC,
                                PDFSignatureInfo signatureInfo, std::vector<ns3__AttributeType *> &attributeTypeList)
{
    // Get endpoint from settings
    ScapSettings settings;

    soap *sp = soap_new2(SOAP_C_UTFSTRING, SOAP_C_UTFSTRING);
    //soap * sp = soap_new2(SOAP_ENC_MTOM, SOAP_ENC_MTOM);

    //Define appropriate network timeouts
    sp->recv_timeout = RECV_TIMEOUT;
    sp->send_timeout = SEND_TIMEOUT;
    sp->connect_timeout = CONNECT_TIMEOUT;

    //TODO: this disables server certificate verification !!
    soap_ssl_client_context(sp, SOAP_SSL_NO_AUTHENTICATION, NULL, NULL, NULL, NULL, NULL);

    AuthorizationServiceSoapBindingProxy proxy(sp);

    _ns1__AuthorizationRequest authorizationRequest;
    _ns1__AuthorizationResponse authorizationResponse;

    //TODO: change this
    authorizationRequest.AppId = m_appID.toStdString();

    m_secretKey = settings.getSecretKey(citizenId);

    qDebug() << "m_secretKey = " << m_secretKey.data();

    std::string new_totp = generateTOTP(m_secretKey);
    eIDMW::PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_DEBUG, "ScapSignature", "Generated TOTP: %s", new_totp.c_str());
    authorizationRequest.TOTP = new_totp;

    SignatureDetails sigDetails;

    //Get the details of the first citizen signature, performed via CC or CMD
    int ret = getCitizenSignatureDetails(filepath, sigDetails);
    if (ret == GAPI::ScapSucess) {
        qDebug() << "getCitizenSignatureDetails() returned success!";
    } else {
        qDebug() << "getCitizenSignatureDetails() returned error!";
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

    std::string s_endpoint = QString("https://" + settings.getScapServerHost() + ":" +
                                     settings.getScapServerPort().append(AUTHORIZATION_ENDPOINT)).toStdString();

    proxy.soap_endpoint = s_endpoint.c_str();

    qDebug() << "SCAP PDFSignatureClient:: Authorization endpoint: " << proxy.soap_endpoint;
            
    int rc = proxy.Authorization(&authorizationRequest, authorizationResponse);

    if (rc != SOAP_OK) {
        qDebug() << "Error returned by calling Authorization in SoapBindingProxy(). Error code: " << rc;
        if (rc == SOAP_FAULT) {

            qDebug() << "SOAP Fault returned: TODO print fault message";
        }
        return GAPI::ScapGenericError;
    }
    else {
        qDebug() << "Authorization service returned SOAP_OK";

        //Check for Success Status
        if (authorizationResponse.Status->Code != "00" || authorizationResponse.TransactionList == NULL) {
            qDebug() << "authorizationService returned error";
            return GAPI::ScapGenericError;
        }

        else {

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
                                if(QString::fromStdString(transaction->AttributeSupplier->Type->c_str()) == "INSTITUTION"){
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
                        PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_DEBUG, "ScapSignature", "PDF Signature error: Error creating temporary file");
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
                            if(ii != 0)signDetailsReason.append("; ");
                            signDetailsReason.append(QString::fromStdString(acSubAttr->Description->c_str()));
                            signDetailsReason.append(": ");
                            signDetailsReason.append(QString::fromStdString(acSubAttr->Value->c_str()));
                        }
                    }
                    signatureInfo.setReason(strdup(signDetailsReason.toStdString().c_str()));
                    signatureInfo.setLocation(strdup(signDetailsLocation.toStdString().c_str()));
                }else{
                    signatureInfo.setReason(strdup(signOriginalReason.toStdString().c_str()));
                    signatureInfo.setLocation(strdup(signOriginalLocation.toStdString().c_str()));
                }

                signatureHash = openSCAPSignature(inputPath, outputPath,
                                transaction->AttributeSupplierCertificateChain, citizenName, citizenId,
                                attributeSupplierListString, attributeListString, signatureInfo, isVisible, isCC);

                if (signatureHash.size() == 0) {
                    PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_DEBUG, "ScapSignature", "openSCAPSignature() failed!");
                    return GAPI::ScapGenericError;
                }

#ifdef DEBUG
                std::string filename = std::string("/tmp/transaction_")+ std::to_string(i) + "_hash.bin";

                WriteToFile(filename.c_str(), (unsigned char *)signatureHash.data(), signatureHash.size());
#endif

                unsigned int sig_len = 0;
                unsigned char * scap_signature = callSCAPSignatureService(sp, signatureHash,
                                                                          transaction, sig_len, citizenId);
                
                if (sig_len > 0) {
#ifdef DEBUG            
                    std::string filename = std::string("/tmp/transaction_")+ std::to_string(i) + "_signature.bin";
                    WriteToFile(filename.c_str(), (unsigned char *)scap_signature, sig_len);
#endif
                    closeSCAPSignature(scap_signature, sig_len);
                }else{
                    PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_DEBUG, "ScapSignature", "callSCAPSignatureService() failed!");
                    return GAPI::ScapGenericError;
                }
                // Apply the next signature over the current one's output file
                inputPath = outputPath;
            }
            // Delete tempFiles the tempFiles
            for (auto& tempFile : tempFiles){
                delete tempFile;
            }

        }
    }

    return GAPI::ScapSucess;
}


/**
    !!! FROM HERE IS THE OLD SCAP SIGNATURE IMPLEMENTATION, USE IT JUST FOR REFERENCE !!!
**/

#ifdef SCAP_OLD_VERSION

//const char * pdf_endpoint = "/PADES/PDFSignature";
const char *pdf_endpoint = "/PADES/PDFSignatureWithAttach";

int PDFSignatureClient::signPDF(ProxyInfo proxyInfo, QString finalfilepath, QString filepath,
                                QString citizenName, QString citizenId, int ltv, PDFSignatureInfo signatureInfo,
                                std::vector<ns3__AttributeType *> &attributeTypeList)
{
    //const SOAP_ENV__Fault * fault = NULL;

    soap *sp = soap_new2(SOAP_C_UTFSTRING | SOAP_ENC_MTOM, SOAP_C_UTFSTRING | SOAP_ENC_MTOM);
    //soap * sp = soap_new2(SOAP_ENC_MTOM, SOAP_ENC_MTOM);

    //Define appropriate network timeouts
    sp->recv_timeout = RECV_TIMEOUT;
    sp->send_timeout = SEND_TIMEOUT;
    sp->connect_timeout = CONNECT_TIMEOUT;

    //soap_set_namespaces(sp, SCAPnamespaces);

    //TODO: this disables server certificate verification !!
    soap_ssl_client_context(sp, SOAP_SSL_NO_AUTHENTICATION, NULL, NULL, NULL, NULL, NULL);

    PDFSignatureWithAttachSoapBindingProxy proxy(sp);

    // Get endpoint from settings
    ScapSettings settings;
    std::string s_endpoint = QString("https://" + settings.getScapServerHost() + ":" +
                                     settings.getScapServerPort().append(pdf_endpoint)).toStdString();

    std::string proxy_host;
    long proxy_port;

    //Proxy support using the gsoap BindingProxy
    if (proxyInfo.isSystemProxy()) {
        proxyInfo.getProxyForHost(s_endpoint, &proxy_host, &proxy_port);
        if (proxy_host.size() > 0) {
            sp->proxy_host = proxy_host.c_str();
            sp->proxy_port = proxy_port;

            eIDMW::PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_DEBUG, "ScapSignature",
                             "signPDF: Using System Proxy: host=%s, port=%ld", sp->proxy_host, sp->proxy_port);
        }
    } else if (proxyInfo.getProxyHost().size() > 0) {
        sp->proxy_host = proxyInfo.getProxyHost().toUtf8().constData();
        sp->proxy_port = proxyInfo.getProxyPort().toLong();
        eIDMW::PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_DEBUG, "ScapSignature",
                         "signPDF: Using Manual Proxy: host=%s, port=%ld", sp->proxy_host, sp->proxy_port);

        if (proxyInfo.getProxyUser().size() > 0) {
            sp->proxy_userid = proxyInfo.getProxyUser().toUtf8().constData();
            sp->proxy_passwd = proxyInfo.getProxyPwd().toUtf8().constData();
        }
    }

    const char *c_endpoint = s_endpoint.c_str();
    proxy.soap_endpoint = c_endpoint;

    std::cerr << "SCAP PDFSignatureClient:: PDF Signing endpoint: " << c_endpoint << std::endl;

    // Get PDF File
    QFile file(filepath);

    if (!file.open(QIODevice::ReadOnly))
        return GAPI::ScapGenericError; // Error opening file

    // Finds signature position
    static const char needleValues[] = { 0x2F, 0x54, 0x20, 0x28 };
    static const char endNeedle = 0x29;
    //This would need QT 5.11
    //QString request_uuid = QUuid::createUuid().toString(QUuid::WithoutBraces);
    QString request_uuid = QUuid::createUuid().toString();
    QString cleanUuid = request_uuid.midRef(1, request_uuid.size() - 2).toString();

    QByteArray needle = QByteArray::fromRawData(needleValues, sizeof(needleValues));
    QByteArray fileByteArray = file.readAll();
    char *fileBinary = fileByteArray.data();
    int pdfBinaryLen = fileByteArray.size();

    int indexOfNeedle = fileByteArray.lastIndexOf(needle);
    if (indexOfNeedle < 0) {
        std::cout << "Could not find signature field1: " << pdfBinaryLen << std::endl;
        return GAPI::ScapGenericError;
    }
    indexOfNeedle += needle.length();

    int i = indexOfNeedle;
    int endOfNeedle = 0;
    while ( i != pdfBinaryLen ) {
        if (fileByteArray.at(i) == endNeedle) {
            endOfNeedle = i;
            break;
        }
        i++;
    }

    if (endOfNeedle == 0) {
        std::cout << "Could not find signature field2: " << pdfBinaryLen << std::endl;
        return GAPI::ScapGenericError;
    }
    QString signatureField(fileByteArray.mid(indexOfNeedle, endOfNeedle - indexOfNeedle));


    // Gets PDF Base 64
    xsd__base64Binary *base64PDF = soap_new_set_xsd__base64Binary(sp, (unsigned char *)fileBinary,
                                                                  pdfBinaryLen,
                                                                  NULL, (char *)"application/pdf", NULL);


    std::string citizenNIC = citizenId.toStdString();
    // Get Citizen Info
    pdf__PersonalDataType *personalData = soap_new_req_pdf__PersonalDataType(sp,
                                                                             citizenName.toStdString(), citizenNIC);
    std::cerr << "Citizen Name : " << citizenName.toStdString() << std::endl;
    std::cerr << "Citizen NIC : " << citizenNIC << std::endl;


    // Get Attributes selected on Tree
    std::vector<pdf__AttributeType *> attributes;
    for (uint pos = 0; pos < attributeTypeList.size(); pos++) {
        ns3__AttributeType *acAttributeType = attributeTypeList.at(pos);

        pdf__AttributeType *convertedAttribute = convertAttributeType(acAttributeType, sp);
        attributes.push_back(convertedAttribute);
    }

    // Create Attribute list type
    pdf__AttributeListType *attributeList = soap_new_req_pdf__AttributeListType(sp, attributes);

    pdf__SignatureOrientationEnumType orientationType =
        signatureInfo.isPortrait() ? pdf__SignatureOrientationEnumType__PORTRAIT :
        pdf__SignatureOrientationEnumType__LANDSCAPE;

    // Request Sign PDF
    try {
        pdf__SignRequestWithAttach *signRequest = soap_new_set_pdf__SignRequestWithAttach(sp,
                                                                                          cleanUuid.toStdString(), personalData, attributeList,
                                                                                          signatureField.toStdString(), *base64PDF, &ltv, signatureInfo.getSelectedPage(),
                                                                                          signatureInfo.getX(), signatureInfo.getY(), orientationType);

        pdf__SignResponse resp;
        int rc = proxy.Sign(signRequest, resp);

        if (rc != SOAP_OK) {
            if (rc == SOAP_FAULT) {
                if (proxy.soap->fault != NULL && proxy.soap->fault->faultstring != NULL)
                    eIDMW::PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_ERROR, "ScapSignature",
                                     "PDF Service returned SOAP Fault: %s",
                                     proxy.soap->fault->faultstring);
                return GAPI::ScapGenericError;
            } else if (rc == SOAP_EOF) {
                eIDMW::PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_ERROR, "ScapSignature",
                                 "Error in sign(): GSoap timeout Error");
                qDebug() << "ScapSignature : Error in sign(): GSoap timeout Error";
                return GAPI::ScapTimeOutError;
            } //SCAP Specific errors
            else if (rc == SCAP_ATTRIBUTES_EXPIRED) {
                eIDMW::PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_ERROR, "ScapSignature",
                                 "Error in sign(): GSoap returned Error code: %d", rc);
                qDebug() << "Request Sign PDF - Error in sign(): GSoap returned Error code" << rc ;
                return GAPI::ScapAttributesExpiredError;
            } else if (rc == SCAP_ZERO_ATTRIBUTES) {
                eIDMW::PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_ERROR, "ScapSignature",
                                 "Error in sign(): GSoap returned Error code: %d", rc);
                qDebug() << "Request Sign PDF - Error in sign(): GSoap returned Error code" << rc ;
                return GAPI::ScapZeroAttributesError;
            } else if (rc == SCAP_ATTRIBUTES_NOT_VALID) {
                eIDMW::PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_ERROR, "ScapSignature",
                                 "Error in sign(): GSoap returned Error code: %d", rc);
                qDebug() << "Request Sign PDF - Error in sign(): GSoap returned Error code" << rc ;
                return GAPI::ScapNotValidAttributesError;
            } else {
                eIDMW::PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_ERROR, "ScapSignature",
                                 "Error in sign(): GSoap returned Error code: %d", rc);
                qDebug() << "Request Sign PDF - Error in sign(): GSoap returned Error code" << rc ;
                return GAPI::ScapGenericError;
            }
        }

        std::string respCode = resp.Status->Code;

        if ( respCode.compare("00") != 0 ) {

            eIDMW::PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_ERROR, "ScapSignature",
                             "PDF Service application-level error Code: %s Message: %S", respCode.c_str(),
                             resp.Status->Message.c_str());
            std::cerr << "Service error code " << respCode << ". Service returned Message: " <<
                      resp.Status->Message << std::endl;
            return GAPI::ScapGenericError;
        }

        std::cout << "Service returned Message: " << resp.Status->Message << std::endl;

        // Save PDF to final destination
        xsd__base64Binary *signedDoc = resp.SignedDocument;
        QFile signedDocFile(finalfilepath);

        if (!signedDocFile.open(QIODevice::ReadWrite)) {
            fprintf(stderr, "Could not write on selected destination");
            return GAPI::ScapGenericError;
        }

        signedDocFile.write( (const char *)signedDoc->__ptr, signedDoc->__size);
        signedDocFile.close();

        return GAPI::ScapSucess;
    } catch (...) {
        const char *error_msg = "Exception thrown in pdf__SignResponse. Signed PDF File not written";
        fputs(error_msg, stderr);

        eIDMW::PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_ERROR, "ScapSignature", "%S", error_msg);
        return GAPI::ScapGenericError;
    }
}

#endif
