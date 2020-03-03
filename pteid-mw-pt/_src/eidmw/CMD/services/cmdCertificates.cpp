#include "cmdCertificates.h"
#include "cmdServices.h"
#include "credentials.h"
#include <iostream>
#include "Util.h"

static char logBuf[512];

namespace eIDMW
{

    CMDCertificates::CMDCertificates()
    {
        m_cmdService = NULL;
    }

    CMDCertificates::~CMDCertificates()
    {
        if (m_cmdService)
        {
            delete m_cmdService;
        }
        for (auto cert : m_certificates) {
            delete cert;
        }
    }

    int CMDCertificates::ImportCertificatesOpen(std::string mobileNumber, std::string pin) {

        if (m_cmdService)
        {
            delete m_cmdService;
        }
        m_cmdService = new CMDServices(CMDCredentials::getCMDBasicAuthUserId(),
                                       CMDCredentials::getCMDBasicAuthPassword(),
                                       CMDCredentials::getCMDBasicAuthAppId());

        CMDProxyInfo proxyInfo = CMDProxyInfo::buildProxyInfo();
        m_mobileNumber = mobileNumber;
        int ret = m_cmdService->askForCertificate(proxyInfo, mobileNumber, pin);
        return ret;
    }

    int CMDCertificates::ImportCertificatesClose(std::string otp) {
        PCCERT_CONTEXT	pCertContext = NULL;
        int res;

        res = fetchCertificates(otp);
        if (res != ERR_NONE)
            return res;

        for (auto cert : m_certificates)
        {
            pCertContext = CertCreateCertificateContext(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, (BYTE *)cert->c_str(), cert->length());
            if (!pCertContext){
                MWLOG_ERR(logBuf, "Error creating certificate context: %x\n", GetLastError());
                return ERR_INV_CERTIFICATE;
            }

            unsigned char KeyUsageBits = 0; // Intended key usage bits copied to here.
            CertGetIntendedKeyUsage(X509_ASN_ENCODING, pCertContext->pCertInfo, &KeyUsageBits, 1);


            if ((KeyUsageBits & CERT_KEY_CERT_SIGN_KEY_USAGE) == CERT_KEY_CERT_SIGN_KEY_USAGE)
            {
                if (!StoreAuthorityCerts(pCertContext, KeyUsageBits))
                    return ERR_INV_CERTIFICATE_CA;
            }
            else
            {
                if (!StoreUserCert(pCertContext, KeyUsageBits, m_mobileNumber))
                    return ERR_INV_CERTIFICATE;
            }
        }

        return ERR_NONE;
    }

    void CMDCertificates::CancelImport() {
        m_cmdService->cancelRequest();
    }

    void CMDCertificates::GetRegisteredPhoneNumbers(std::vector<std::string> *phoneNumsOut) {
        HCERTSTORE hMyStore = NULL;
        PCCERT_CONTEXT pCert = NULL;

        hMyStore = CertOpenSystemStore(NULL, L"MY");
        if (!hMyStore){
            MWLOG_ERR(logBuf, "Could not open \"MY\" System Store: 0x%x\n", GetLastError());
            goto cleanup;
        }

        while (pCert = CertFindCertificateInStore(
            hMyStore,
            X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
            0,
            CERT_FIND_ISSUER_STR,
            L"EC de Chave Móvel Digital",
            pCert
            ))
        {
            DWORD cbMobileNumber;
            if (!CertGetCertificateContextProperty(
                pCert,
                CERT_FIRST_USER_PROP_ID,
                NULL,
                &cbMobileNumber))
            {
                MWLOG_ERR(logBuf, "Could not get size of certificate's friendly name.");
                goto cleanup;
            }
            LPWSTR pszMobileNumber = new WCHAR[cbMobileNumber];
            if (!CertGetCertificateContextProperty(
                pCert,
                CERT_FIRST_USER_PROP_ID,
                pszMobileNumber,
                &cbMobileNumber))
            {
                MWLOG_ERR(logBuf, "Could not get certificate's friendly name.");
                goto cleanup;
            }

            std::string mobileNum = utilStringNarrow(pszMobileNumber);
            phoneNumsOut->push_back(mobileNum);
            delete pszMobileNumber;
        }
    cleanup:
        if (hMyStore)
        {
            CertCloseStore(hMyStore, CERT_CLOSE_STORE_FORCE_FLAG);
        }
    }

    /*****************************************************************************
    Fetch the CMD certificate chain associated with the user with the given mobileNumber
    ******************************************************************************/
    int CMDCertificates::fetchCertificates(std::string otp) {
        CMDProxyInfo proxyInfo = CMDProxyInfo::buildProxyInfo();
        std::vector<CByteArray> certificates;
        int ret = m_cmdService->getCMDCertificate(proxyInfo, otp, certificates);
        if (ret != ERR_NONE)
            return ret;

        if (0 == certificates.size())
        {
            MWLOG_ERR(logBuf, "getCertificate failed\n");
            return ERR_GET_CERTIFICATE;
        }

        for (size_t i = 0; i < certificates.size(); i++)
        {
            CByteArray cert = certificates.at(i);
            std::string *m_certificateString = new std::string((char *)cert.GetBytes(), cert.Size());
            m_certificates.push_back(m_certificateString);
        }
        return ERR_NONE;
    }

    /**********************
    Add User cert to Store
    ***********************/
    bool CMDCertificates::StoreUserCert(PCCERT_CONTEXT pCertContext, unsigned char KeyUsageBits, std::string mobileNumber) {
        HCERTSTORE hMyStore = CertOpenSystemStore(NULL, L"MY");
        BOOL bSuccess = FALSE;

        if (!hMyStore){
            MWLOG_ERR(logBuf, "Could not open \"MY\" System Store: 0x%x\n", GetLastError());
            goto cleanup;
        }

        if (!SetPrivateKeyContainerInfo(pCertContext, KeyUsageBits, mobileNumber)) {
            goto cleanup;
        }
        if (!SetCertificateFriendlyName(pCertContext, mobileNumber)) {
            goto cleanup;
        }

        RemoveOlderUserCerts(pCertContext);

        if (!CertAddCertificateContextToStore(hMyStore, pCertContext, CERT_STORE_ADD_REPLACE_EXISTING, NULL)) {
            MWLOG_ERR(logBuf, "Error adding certificate to store: 0x%x\n", GetLastError());
            goto cleanup;
        }
        bSuccess = true;

    cleanup:
        if (hMyStore)
        {
            CertCloseStore(hMyStore, CERT_CLOSE_STORE_FORCE_FLAG);
        }
        return bSuccess;
    }

    /**********************
    Add authority cert to Store
    ***********************/
    bool CMDCertificates::StoreAuthorityCerts(PCCERT_CONTEXT pCertContext, unsigned char KeyUsageBits) {

        HCERTSTORE hMemoryStore;
        if (0 != memcmp(pCertContext->pCertInfo->Issuer.pbData
            , pCertContext->pCertInfo->Subject.pbData
            , pCertContext->pCertInfo->Subject.cbData))
        {
            hMemoryStore = CertOpenSystemStoreA(NULL, "CA");
        }
        else
        {
            return true;
        }

        if (!hMemoryStore){
            MWLOG_ERR(logBuf, "Could not open System Store to add authority certificate: 0x%x\n", GetLastError());
            return false;
        }

        PCCERT_CONTEXT  pDesiredCert = CertFindCertificateInStore(hMemoryStore
            , X509_ASN_ENCODING
            , 0
            , CERT_FIND_EXISTING
            , pCertContext
            , NULL
            );

        if (pDesiredCert)
        {
            CertFreeCertificateContext(pDesiredCert);
        }
        else
        {
            CertAddEnhancedKeyUsageIdentifier(pCertContext, szOID_PKIX_KP_EMAIL_PROTECTION);
            CertAddEnhancedKeyUsageIdentifier(pCertContext, szOID_PKIX_KP_SERVER_AUTH);
            if (!CertAddCertificateContextToStore(hMemoryStore, pCertContext, CERT_STORE_ADD_NEWER, NULL))
            {
                CertCloseStore(hMemoryStore, CERT_CLOSE_STORE_FORCE_FLAG);
                if (GetLastError() == ERROR_CANCELLED)
                {
                    MWLOG_ERR(logBuf, "User denied registration of root certificate! Certificate chain will be incomplete...");
                }
                else
                {
                    MWLOG_ERR(logBuf, "Error adding certificate to store: 0x%x\n", GetLastError());
                }
                return false;
            }
        }

        CertCloseStore(hMemoryStore, CERT_CLOSE_STORE_FORCE_FLAG);
        hMemoryStore = NULL;
        return true;
    }

    bool CMDCertificates::SetPrivateKeyContainerInfo(PCCERT_CONTEXT pCertContext, unsigned char KeyUsageBits, std::string mobileNumber) {
        CRYPT_KEY_PROV_INFO cryptKeyProvInfo;
        unsigned long dwPropId = CERT_KEY_PROV_INFO_PROP_ID;
        unsigned long dwFlags = CERT_STORE_NO_CRYPT_RELEASE_FLAG;

        cryptKeyProvInfo.pwszProvName = (LPWSTR)L"Chave Móvel Digital Key Storage Provider"; // name of the prov
        cryptKeyProvInfo.dwKeySpec = AT_SIGNATURE;

        std::wstring strContainerName = L"CMD_";
        strContainerName += (KeyUsageBits & CERT_NON_REPUDIATION_KEY_USAGE ? L"NR_" : L"DS_");

        DWORD certHashBufferSize;
        if (!CryptHashCertificate2(BCRYPT_SHA1_ALGORITHM, NULL, NULL, pCertContext->pbCertEncoded,
            pCertContext->cbCertEncoded, NULL, &certHashBufferSize))
        {
            MWLOG_ERR(logBuf, "Error getting cert hash size in CryptHashCertificate2");
            return false;
        }
        BYTE *certHashBuffer = new BYTE[certHashBufferSize];
        if (!CryptHashCertificate2(BCRYPT_SHA1_ALGORITHM, NULL, NULL, pCertContext->pbCertEncoded,
            pCertContext->cbCertEncoded, certHashBuffer, &certHashBufferSize))
        {
            MWLOG_ERR(logBuf, "Error getting cert hash in CryptHashCertificate2");
            delete[] certHashBuffer;
            return false;
        }

        // convert hash to readable hex string.
        DWORD certHashHexBufferSize;
        if (!CryptBinaryToStringA(
            certHashBuffer,
            certHashBufferSize,
            CRYPT_STRING_HEXRAW,
            NULL,
            &certHashHexBufferSize)
            ){
            MWLOG_ERR(logBuf, "Error in computing hash size in hex in CryptBinaryToStringA");
            delete[] certHashBuffer;
            return false;
        }
        LPSTR certHashHexBuffer = new CHAR[certHashHexBufferSize];
        if (!CryptBinaryToStringA(
            certHashBuffer,
            certHashBufferSize,
            CRYPT_STRING_HEXRAW,
            certHashHexBuffer,
            &certHashHexBufferSize)
            ){
            MWLOG_ERR(logBuf, "Error in converting hash to hex in CryptBinaryToStringA");
            delete[] certHashBuffer;
            delete[] certHashHexBuffer;
            return false;
        }
        std::string certSerialNumber = certHashHexBuffer;
        strContainerName += utilStringWiden(certSerialNumber);
        delete[] certHashBuffer;
        delete[] certHashHexBuffer;

        cryptKeyProvInfo.pwszContainerName = (LPWSTR)strContainerName.c_str(); //Name of the key to retrieve
        cryptKeyProvInfo.dwProvType = 0; // 0 for CNG KSPs
        cryptKeyProvInfo.dwFlags = 0;
        cryptKeyProvInfo.cProvParam = 0;
        cryptKeyProvInfo.rgProvParam = NULL;

        if (!CertSetCertificateContextProperty(pCertContext, dwPropId, dwFlags, &cryptKeyProvInfo))
        {
            MWLOG_ERR(logBuf, "Error in CertSetCertificateContextProperty for key container info: 0x%x", GetLastError());
            return false;
        }
        return true;
    }

    bool CMDCertificates::SetCertificateFriendlyName(PCCERT_CONTEXT pCertContext, std::string mobileNumber) {
        std::wstring mobileNumberW = utilStringWiden(mobileNumber);

        CRYPT_DATA_BLOB tpMobileNumber = { 0, 0 };
        tpMobileNumber.pbData = (BYTE *)mobileNumberW.c_str();
        tpMobileNumber.cbData = (mobileNumberW.size() + 1) * sizeof(wchar_t);

        if (!CertSetCertificateContextProperty(
            pCertContext,
            CERT_FIRST_USER_PROP_ID,
            CERT_STORE_NO_CRYPT_RELEASE_FLAG,
            &tpMobileNumber))
        {
            MWLOG_ERR(logBuf, "Error in CertSetCertificateContextProperty for certificate's associated mobile number: 0x%x", GetLastError());
            return false;
        }
        return true;
    }

    void CMDCertificates::RemoveOlderUserCerts(PCCERT_CONTEXT pTargetCert) {
        HCERTSTORE hMyStore = CertOpenSystemStore(NULL, L"MY");

        DWORD dwSubjectLen = CertGetNameStringW(
            pTargetCert,
            CERT_NAME_ATTR_TYPE,
            0,
            szOID_DEVICE_SERIAL_NUMBER,
            NULL,
            0);

        PWSTR csTargetSubject = new WCHAR[dwSubjectLen];
        CertGetNameStringW(
            pTargetCert,
            CERT_NAME_ATTR_TYPE,
            0,
            szOID_DEVICE_SERIAL_NUMBER,
            csTargetSubject,
            dwSubjectLen);

        PWSTR csCandidateSubject = new WCHAR[dwSubjectLen];

        PCCERT_CONTEXT pCert = NULL;
        while (pCert = CertFindCertificateInStore(
            hMyStore,
            X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
            0,
            CERT_FIND_ISSUER_STR,
            L"EC de Chave Móvel Digital",
            pCert
            ))
        {
            CertGetNameStringW(
                pCert,
                CERT_NAME_ATTR_TYPE,
                0,
                szOID_DEVICE_SERIAL_NUMBER,
                csCandidateSubject,
                dwSubjectLen);

            if (wcscmp(csTargetSubject, csCandidateSubject) == 0)
            {
                if (!CertDuplicateCertificateContext(pCert))
                {
                    MWLOG_ERR(logBuf, "CertDuplicateCertificateContext failed with error code 0x%08x", GetLastError());
                    goto cleanup;
                }
                if (!CertDeleteCertificateFromStore(pCert))
                {
                    MWLOG_ERR(logBuf, "CertDeleteCertificateFromStore failed with error code 0x%08x", GetLastError());
                    goto cleanup;
                }
            }
        }

    cleanup:
        delete[] csTargetSubject;
        delete[] csCandidateSubject;
    }
}