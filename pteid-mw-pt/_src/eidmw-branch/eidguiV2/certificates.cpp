#include "certificates.h"
#include <QObject>
#include <QDebug>
#include <QDir>

#ifdef WIN32
#include <stdio.h>
#include <iostream>
#endif

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <Wincrypt.h>
#include <Cryptuiapi.h>
#else
#include "pteidversions.h"
typedef const void *PCX509CERT;
#define PCCERT_CONTEXT PCX509CERT
#endif

//MW libraries
#include "eidlib.h"
#include "eidlibException.h"

using namespace eIDMW;

/*
    CERTIFICATES - Certificates Interface
*/
//*****************************************************
// certificate contexts per card reader
//*****************************************************
tCertPerReader	CERTIFICATES::m_certContexts; //!< certificate contexts per reader
//*****************************************************
// Constructor
//*****************************************************
CERTIFICATES::CERTIFICATES()
{
}

#ifdef WIN32
void ImportCertFromDisk(void *cert_path)
{
    PCCERT_CONTEXT pCertCtx = NULL;

    if (CryptQueryObject (
        CERT_QUERY_OBJECT_FILE,
        cert_path,
        CERT_QUERY_CONTENT_FLAG_ALL,
        CERT_QUERY_FORMAT_FLAG_ALL,
        0,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        (const void **)&pCertCtx) != 0)
    {
        //Don't register any self-signed cert (as it shouldn't be in Intermediate Certificates Store)
        if (0 == memcmp(pCertCtx->pCertInfo->Issuer.pbData,
                       pCertCtx->pCertInfo->Subject.pbData,
                       pCertCtx->pCertInfo->Subject.cbData))
            return;

        HCERTSTORE hCertStore = CertOpenSystemStoreA (NULL, "CA");

        if (hCertStore != NULL)
        {
            CertAddEnhancedKeyUsageIdentifier (pCertCtx, szOID_PKIX_KP_EMAIL_PROTECTION);
            CertAddEnhancedKeyUsageIdentifier (pCertCtx, szOID_PKIX_KP_SERVER_AUTH);
            if (CertAddCertificateContextToStore (
                hCertStore,
                pCertCtx,
                CERT_STORE_ADD_ALWAYS,
                NULL))
            {
                std::cout << "Added certificate to store." << std::endl;
            }

            if (CertCloseStore (hCertStore, 0))
            {
                std::cout << "Cert. store handle closed." << std::endl;
            }
        }

        if (pCertCtx)
        {
            CertFreeCertificateContext (pCertCtx);
        }
    }
    else
    {
        PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui",
         "ImportCertFromDisk: CryptQueryObject returned error %08x", GetLastError());
    }
}
#endif
//**************************************************
// Checks of older registered certificates are not
// still bound to the CSP when the minidriver is used
//**************************************************
#ifdef WIN32
bool CERTIFICATES::ProviderNameCorrect (PCCERT_CONTEXT pCertContext )
{
    unsigned long dwPropId= CERT_KEY_PROV_INFO_PROP_ID;
    DWORD cbData = 0;
    CRYPT_KEY_PROV_INFO * pCryptKeyProvInfo;

    if(!(CertGetCertificateContextProperty(
            pCertContext,		// A pointer to the certificate where the property will be set.
            dwPropId,           // An identifier of the property to get.
            NULL,               // NULL on the first call to get the length.
            &cbData)))          // The number of bytes that must be allocated for the structure.
    {
        if (GetLastError() != CRYPT_E_NOT_FOUND) // The certificate does not have the specified property.
            return false;
    }
    if(!(pCryptKeyProvInfo = (CRYPT_KEY_PROV_INFO *)malloc(cbData)))
    {
        return true;
    }
    if(CertGetCertificateContextProperty(pCertContext, dwPropId, pCryptKeyProvInfo, &cbData))
    {
        if (!wcscmp(pCryptKeyProvInfo->pwszProvName, L"Portugal Identity Card CSP"))
            return false;
    }
    return true;
}
#endif
//*****************************************************
// store the user certificates of the card in a specific reader
//*****************************************************
bool CERTIFICATES::StoreUserCerts (PTEID_EIDCard& Card, PCCERT_CONTEXT pCertContext, unsigned char KeyUsageBits, PTEID_Certificate& cert, const char* readerName)
{
#ifdef WIN32
    unsigned long	dwFlags			= CERT_STORE_NO_CRYPT_RELEASE_FLAG;
    PCCERT_CONTEXT  pDesiredCert	= NULL;
    PCCERT_CONTEXT  pPrevCert		= NULL;
	HCERTSTORE		hMyStore = CertOpenSystemStore(NULL, L"MY");

    if ( NULL != hMyStore )
    {
        // ----------------------------------------------------
        // look if we already have a certificate with the same
        // subject (contains name and document number) in the store
        // If the certificate is not found --> NULL
        // ----------------------------------------------------
        do
        {
            if( NULL != (pDesiredCert = CertFindCertificateInStore(hMyStore, X509_ASN_ENCODING, 0, CERT_FIND_SUBJECT_NAME, &(pCertContext->pCertInfo->Subject) , pPrevCert)))
            {
                // ----------------------------------------------------
                // If the certificates are identical and function
                // succeeds, the return value is nonzero, or TRUE.
                // ----------------------------------------------------
                if(NULL == CertCompareCertificate(X509_ASN_ENCODING,pCertContext->pCertInfo,pDesiredCert->pCertInfo) ||
                        !ProviderNameCorrect(pDesiredCert) )
                {
                    // ----------------------------------------------------
                    // certificates are not identical, but have the same
                    // subject (contains name and document number),
                    // so we remove the one that was already in the store
                    // ----------------------------------------------------
                    if(NULL == CertDeleteCertificateFromStore(pDesiredCert))
                    {
                        if (E_ACCESSDENIED == GetLastError())
                        {
                            //QString strCaption(tr("Deleting former certificate"));
                            //QString strMessage(tr("Error deleting former certificate"));
                            //QMessageBox::information(NULL,strCaption,strMessage);
                        }
                    }
                    pPrevCert = NULL;
                    continue;
                }
            }
            pPrevCert = pDesiredCert;
        }
        while (NULL != pDesiredCert);


        if( NULL != (pDesiredCert = CertFindCertificateInStore(hMyStore, X509_ASN_ENCODING, 0, CERT_FIND_EXISTING, pCertContext , NULL))
        )
        {
            m_certContexts[readerName].push_back(pCertContext);
            // ----------------------------------------------------
            // certificate is already in the store, then just return
            // ----------------------------------------------------
            CertFreeCertificateContext(pDesiredCert);
            CertCloseStore (hMyStore, CERT_CLOSE_STORE_FORCE_FLAG);
            return true;
        }

        // ----------------------------------------------------
        // Initialize the CRYPT_KEY_PROV_INFO data structure.
        // Note: pwszContainerName and pwszProvName can be set to NULL
        // to use the default container and provider.
        // ----------------------------------------------------
        CRYPT_KEY_PROV_INFO* pCryptKeyProvInfo	= new CRYPT_KEY_PROV_INFO;
        unsigned long		 dwPropId			= CERT_KEY_PROV_INFO_PROP_ID;

        // ----------------------------------------------------
        // Get the serial number
        // ----------------------------------------------------
        PTEID_CardVersionInfo& versionInfo = Card.getVersionInfo();
        const char*			  pSerialKey  = versionInfo.getSerialNumber();

        QString				  strContainerName;

        //Using Minidriver
            if (KeyUsageBits & CERT_NON_REPUDIATION_KEY_USAGE)
            {
                strContainerName = "NR_";
            }
            else
            {
                strContainerName = "DS_";
            }
            strContainerName += pSerialKey;
            pCryptKeyProvInfo->pwszProvName			= (LPWSTR)L"Microsoft Base Smart Card Crypto Provider";
            pCryptKeyProvInfo->dwKeySpec			= AT_SIGNATURE;
        
        pCryptKeyProvInfo->pwszContainerName	= (LPWSTR)strContainerName.utf16();
        pCryptKeyProvInfo->dwProvType			= PROV_RSA_FULL;
        pCryptKeyProvInfo->dwFlags				= 0;
        pCryptKeyProvInfo->cProvParam			= 0;
        pCryptKeyProvInfo->rgProvParam			= NULL;

        // Set the property.
        if (CertSetCertificateContextProperty(
                pCertContext,       // A pointer to the certificate
                // where the propertiy will be set.
                dwPropId,           // An identifier of the property to be set.
                // In this case, CERT_KEY_PROV_INFO_PROP_ID
                // is to be set to provide a pointer with the
                // certificate to its associated private key
                // container.
                dwFlags,            // The flag used in this case is
                // CERT_STORE_NO_CRYPT_RELEASE_FLAG
                // indicating that the cryptographic
                // context aquired should not
                // be released when the function finishes.
                pCryptKeyProvInfo   // A pointer to a data structure that holds
                // infomation on the private key container to
                // be associated with this certificate.
        ))
        {
            if (NULL != pCryptKeyProvInfo)
            {
                delete pCryptKeyProvInfo;
                pCryptKeyProvInfo = NULL;
            }
            // Set friendly names for the certificates
            CRYPT_DATA_BLOB tpFriendlyName	= {0, 0};
            unsigned long	ulID			= 0;

            if (KeyUsageBits & CERT_NON_REPUDIATION_KEY_USAGE)
            {
                ulID = 0x03;
            }
            else
            {
                ulID = 0x02;
            }

            QString			strFriendlyName;
            strFriendlyName = QString::fromUtf8(cert.getOwnerName());
            int iFriendLen = (strFriendlyName.length() + 1) * sizeof(QChar);

            tpFriendlyName.pbData = new unsigned char[iFriendLen];

            memset(tpFriendlyName.pbData, 0, iFriendLen);
            memcpy(tpFriendlyName.pbData, strFriendlyName.utf16(), iFriendLen - sizeof(QChar));

            tpFriendlyName.cbData = iFriendLen;

            if (CertSetCertificateContextProperty(
                    pCertContext,       // A pointer to the certificate
                    // where the propertiy will be set.
                    CERT_FRIENDLY_NAME_PROP_ID,           // An identifier of the property to be set.
                    // In this case, CERT_KEY_PROV_INFO_PROP_ID
                    // is to be set to provide a pointer with the
                    // certificate to its associated private key
                    // container.
                    dwFlags,            // The flag used in this case is
                    // CERT_STORE_NO_CRYPT_RELEASE_FLAG
                    // indicating that the cryptographic
                    // context aquired should not
                    // be released when the function finishes.
                    &tpFriendlyName   // A pointer to a data structure that holds
                    // infomation on the private key container to
                    // be associated with this certificate.
            ))
            {
                if (KeyUsageBits & CERT_NON_REPUDIATION_KEY_USAGE)
                {
                    CertAddEnhancedKeyUsageIdentifier (pCertContext, szOID_PKIX_KP_EMAIL_PROTECTION);
                }
                else
                {
                    CertAddEnhancedKeyUsageIdentifier (pCertContext, szOID_PKIX_KP_EMAIL_PROTECTION);
                    CertAddEnhancedKeyUsageIdentifier (pCertContext, szOID_PKIX_KP_CLIENT_AUTH);
                }
                CertAddCertificateContextToStore(hMyStore, pCertContext, CERT_STORE_ADD_REPLACE_EXISTING, NULL);
                m_certContexts[readerName].push_back(pCertContext);
            }

            if (NULL != tpFriendlyName.pbData)
            {
                delete [] (tpFriendlyName.pbData);
                tpFriendlyName.pbData = NULL;
            }
        }
        CertCloseStore (hMyStore, CERT_CLOSE_STORE_FORCE_FLAG);
        hMyStore = NULL;
    }
#endif
    return true;
}
//*****************************************************
// store the authority certificates of the card in a specific reader
//*****************************************************
bool CERTIFICATES::StoreAuthorityCerts(PCCERT_CONTEXT pCertContext, unsigned char KeyUsageBits, const char* readerName)
{
#ifdef WIN32
    bool			bRet		 = false;
    HCERTSTORE		hMemoryStore = NULL;   // memory store handle
    PCCERT_CONTEXT  pDesiredCert = NULL;

    if ( 0 == memcmp ( pCertContext->pCertInfo->Issuer.pbData
            , pCertContext->pCertInfo->Subject.pbData
            , pCertContext->pCertInfo->Subject.cbData))
    {
        hMemoryStore = CertOpenSystemStoreA (NULL, "ROOT");
    }
    else
    {
        hMemoryStore = CertOpenSystemStoreA (NULL, "CA");
    }

    if(NULL != hMemoryStore)
    {
        pDesiredCert = CertFindCertificateInStore( hMemoryStore
                , X509_ASN_ENCODING
                , 0
                , CERT_FIND_EXISTING
                , pCertContext
                , NULL
        );
        if( pDesiredCert )
        {
            CertFreeCertificateContext(pDesiredCert);
        }
        else
        {
            CertAddEnhancedKeyUsageIdentifier (pCertContext, szOID_PKIX_KP_EMAIL_PROTECTION);
            CertAddEnhancedKeyUsageIdentifier (pCertContext, szOID_PKIX_KP_SERVER_AUTH);
            if (CertAddCertificateContextToStore(hMemoryStore, pCertContext, CERT_STORE_ADD_NEWER, NULL))
            {
                m_certContexts[readerName].push_back(pCertContext);
                bRet = true;
            }
            else
            {
                if (GetLastError() == ERROR_CANCELLED)
                    PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "User denied registration of root certificate! Certificate chain will be incomplete...");
            }
        }
        CertCloseStore (hMemoryStore, CERT_CLOSE_STORE_FORCE_FLAG);
        hMemoryStore = NULL;
    }
    return bRet;
#else
    return true;
#endif
}
//*****************************************************
// forget all the certificates we kept for a specific reader
//*****************************************************
void CERTIFICATES::forgetCertificates( QString const& reader )
{
    char readerName[256];
    readerName[0]=0;
    if (reader.length()>0)
    {
        strcpy(readerName,reader.toUtf8().data());
    }
#ifdef WIN32
    while ( 0 < m_certContexts[readerName].size() )
    {
        PCCERT_CONTEXT pContext = m_certContexts[readerName][m_certContexts[readerName].size()-1];
        CertFreeCertificateContext(pContext);
        m_certContexts[readerName].erase(m_certContexts[readerName].end()-1);
    }
#endif
}
//*****************************************************
// remove the certificates of a card in a specific reader
//*****************************************************
bool CERTIFICATES::RemoveCertificates( QString const& readerName )
{
    return RemoveCertificates( readerName.toLatin1().data() );
}
bool CERTIFICATES::RemoveCertificates( const char* readerName )
{
#ifdef WIN32

    if ( !readerName || 0 == strlen(readerName) )
    {
        return false;
    }

    PCCERT_CONTEXT	pCertContext = NULL;
    int nrCerts = m_certContexts[readerName].size();
    for (int CertIdx=0;CertIdx<nrCerts;CertIdx++)
    {

        // ----------------------------------------------------
        // create the certificate context with the certificate raw data
        // ----------------------------------------------------
        PCCERT_CONTEXT  pDesiredCert	= NULL;
		HCERTSTORE		hMyStore = CertOpenSystemStore(NULL, (LPWSTR)L"MY");

        pCertContext = m_certContexts[readerName][CertIdx];

        if ( NULL != hMyStore )
        {
            // ----------------------------------------------------
            // look if we already have the certificate in the store
            // If the certificate is not found --> NULL
            // ----------------------------------------------------
            if( NULL != (pDesiredCert = CertFindCertificateInStore(hMyStore, X509_ASN_ENCODING, 0, CERT_FIND_EXISTING, pCertContext , NULL))
            )
            {
                CertDeleteCertificateFromStore( pDesiredCert );
            }
        }
    }
    if (nrCerts>0)
    {
        forgetCertificates( readerName );
    }
#endif
    return true;
}
//*****************************************************
// import the certificates from the card in a specific reader
//*****************************************************
bool CERTIFICATES::ImportCertificates(QString const& readerName)
{
    return ImportCertificates( readerName.toLatin1().data() );
}

bool CERTIFICATES::ImportCertificates( const char* readerName )
{
#ifdef WIN32
    qDebug() << "CERTIFICATES::ImportCertificates" << readerName;
    if ( !readerName || 0 == strlen(readerName) )
    {
        return false;
    }

    PCCERT_CONTEXT	pCertContext = NULL;
    QString			strTip;
    bool			bImported	= false;
    wchar_t * cert_filepath = NULL;

    PTEID_ReaderContext&  ReaderContext  = ReaderSet.getReaderByName(readerName);
    if (!ReaderContext.isCardPresent())
    {
        return false;
    }

    //Register the higher-level CA Cert from disk files
    //TODO: maybe we should do the same for test cards
    QString certsdir("C:\\Program Files\\Portugal Identity Card\\eidstore\\certs\\");
    QDir dir(certsdir);

    QStringList filter("*.der");
    QStringList flist = dir.entryList(QStringList(filter),
                                   QDir::Files | QDir::NoSymLinks);

    foreach (QString str, flist) {
        QString filename = QString("%1%2").arg(certsdir).arg(str);
        unsigned int alloc_len = filename.size()+1;
        cert_filepath = new wchar_t[alloc_len];
        //This way we ensure the UTF-16 string is terminated
        memset(cert_filepath, 0, sizeof(wchar_t)*alloc_len);
        filename.toWCharArray(cert_filepath);
        ImportCertFromDisk(cert_filepath);

        delete[] cert_filepath;
    }
    try
    {
        PTEID_EIDCard&		 Card			= ReaderContext.getEIDCard();
        PTEID_Certificates&	 certificates	= Card.getCertificates();

        for (size_t CertIdx=0;CertIdx<Card.certificateCount();CertIdx++)
        {
            PTEID_Certificate&	 cert			= certificates.getCertFromCard(CertIdx);
            const PTEID_ByteArray certData		= cert.getCertData();

            // ----------------------------------------------------
            // create the certificate context with the certificate raw data
            // ----------------------------------------------------
            pCertContext = CertCreateCertificateContext(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, certData.GetBytes(), certData.Size());

            if( pCertContext )
            {
                if ( 0 == memcmp ( pCertContext->pCertInfo->Issuer.pbData,
                       pCertContext->pCertInfo->Subject.pbData,
                       pCertContext->pCertInfo->Subject.cbData
                       )
                    )
                    continue;
                unsigned char KeyUsageBits = 0; // Intended key usage bits copied to here.
                CertGetIntendedKeyUsage(X509_ASN_ENCODING, pCertContext->pCertInfo, &KeyUsageBits, 1);

                // ----------------------------------------------------
                // Only store the context of the certificates with usages for an end-user
                // i.e. no CA or root certificates
                // ----------------------------------------------------
                if((KeyUsageBits & CERT_KEY_CERT_SIGN_KEY_USAGE) == CERT_KEY_CERT_SIGN_KEY_USAGE)
                {
                    if(StoreAuthorityCerts (pCertContext, KeyUsageBits, readerName))
                    {
                        bImported = true;
                    }
                }
                else
                {
                    if(StoreUserCerts (Card, pCertContext, KeyUsageBits, cert, readerName))
                    {
                        bImported = true;
                    }
                }
                pCertContext = NULL;
            }
        }
    }
    catch (PTEID_Exception& e)
    {
        long err = e.GetError();
        err = err;
        //QString strCaption(tr("Retrieving certificates"));
        //QString strMessage(tr("Error retrieving certificates"));
        //QMessageBox::information(NULL,strCaption,strMessage);
    }

    //	showCertImportMessage(bImported);

    return bImported;
#else
    return true;
#endif
}
