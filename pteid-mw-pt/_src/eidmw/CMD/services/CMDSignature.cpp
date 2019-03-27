#include <algorithm>
#include "CMDSignature.h"
#include "MiscUtil.h"
#include "StringOps.h"
#include "PDFSignature.h"
#include "cmdServices.h"
#include <QByteArray>
#include <QCryptographicHash>
#include "eidlibException.h"

static char logBuf[512];

namespace eIDMW {

    bool isDBG = false;

    void printData(char *msg, unsigned char *data, unsigned int dataLen) {
        if ( NULL == msg ) {
            printf( "Data:\n" );
        }
        else {
            printf( "%s\n", msg );
        }

        for (unsigned int i = 0; i < dataLen; i++ ) {
            if ( ( i > 0) && ( 0 == ( i % 20 ))) printf("\n");
            printf( "%02x", data[i] );
        }
        printf( "\n" );
    }

    CMDSignature::CMDSignature(std::string basicAuthUser, std::string basicAuthPassword, std::string applicationId) {
        m_pdf_handler = NULL;
        cmdService = new CMDServices(basicAuthUser, basicAuthPassword, applicationId);
    }

    CMDSignature::CMDSignature(std::string userId, std::string password, std::string applicationId, PTEID_PDFSignature *in_pdf_handler) {
        m_pdf_handler = in_pdf_handler;
        cmdService = new CMDServices(userId, password, applicationId);
    }

    CMDSignature::~CMDSignature() {
        m_pdf_handler = NULL;
        delete cmdService;
    }

    void CMDSignature::set_pdf_handler(PTEID_PDFSignature *in_pdf_handler ) {
        m_pdf_handler = in_pdf_handler;
    }

    void CMDSignature::set_string_handler(std::string in_docname_handle,
                                          QByteArray in_array_handler) {
        m_docname_handle = in_docname_handle;
        m_array_handler = in_array_handler;
    }

    char * CMDSignature::getCertificateCitizenName() {
        PDFSignature *pdf = m_pdf_handler->getPdfSignature();

        return pdf->getCitizenCertificateName();
    }

    char * CMDSignature::getCertificateCitizenID() {
        PDFSignature *pdf = m_pdf_handler->getPdfSignature();
        
        return pdf->getCitizenCertificateID();
    }

    int CMDSignature::cli_getCertificate( std::string in_userId) {
        if ( NULL == m_pdf_handler
             && (m_docname_handle.size() == 0 || m_array_handler.size() == 0)) {
            MWLOG_ERR( logBuf, "NULL handler" );
            return ERR_NULL_PDF_HANDLER;
        }

        if ( isDBG ) {
            printData( (char *)"\nUserId: ", (unsigned char *)in_userId.c_str(), in_userId.size());
        }

        std::vector<CByteArray> certificates;
        int ret = cmdService->getCertificate(m_proxyInfo, in_userId, certificates);

        if ( ret != ERR_NONE ) return ret;

        if ( 0 == certificates.size()) {
            MWLOG_ERR( logBuf, "getCertificate failed\n" );
            return ERR_GET_CERTIFICATE;
        }

        if ( m_pdf_handler != NULL ) {
            PDFSignature *pdf = m_pdf_handler->getPdfSignature();
            if ( NULL == pdf ) {
                MWLOG_ERR( logBuf, "NULL Pdf\n" );
                return ERR_NULL_PDF;
            }

            /* TODO: At the moment, it is only possible to sign one document. */
            pdf->setBatch_mode(false);

            pdf->setExternCertificate(certificates.at(0));

            pdf->setIsCC(false);

            std::vector<CByteArray> newVec(certificates.begin()+1, certificates.end());
            pdf->setExternCertificateCA( newVec );
        }
        else
        {
            CByteArray externCertificate = certificates.at(0);

            QByteArray ba((const char *)externCertificate.GetBytes(),
                          externCertificate.Size());

            QByteArray temp = ba.toHex(0);
            m_string_certificate = temp.toStdString();
            if ( isDBG ){
                printData( (char *)"\n Certificate: ",
                           (unsigned char *)externCertificate.GetBytes(),
                           externCertificate.Size());
            }
        }

        return ERR_NONE;
    }

/*  *********************************************************
    ***    CMDSignature::cli_sendDataToSign()          ***
    ********************************************************* */
    int CMDSignature::cli_sendDataToSign( std::string in_pin) {

        if ( NULL == m_pdf_handler
             && (m_docname_handle.size() == 0 || m_array_handler.size() == 0)) {
            MWLOG_ERR( logBuf, "NULL handler" );
            return ERR_NULL_PDF_HANDLER;
        }

        /* printData */
        if (isDBG) {
            printData( (char *)"\nIN - User Pin: "
                        , (unsigned char *)in_pin.c_str()
                        , in_pin.size() );
        }

        std::string userPin = in_pin;
        CByteArray hashByteArray;
        std::string DocName;

        if ( m_pdf_handler != NULL ) {
            PDFSignature *pdf = m_pdf_handler->getPdfSignature();
            if ( NULL == pdf ) {
                MWLOG_ERR( logBuf, "NULL Pdf\n" );
                return ERR_NULL_PDF;
            }
            /* Calculate hash */
            hashByteArray = pdf->getHash();
            DocName = pdf->getDocName();
        }
        else{
            /* Calculate hash */
            QCryptographicHash sha256(QCryptographicHash::Sha256);
            sha256.addData(m_array_handler);

            QByteArray res = sha256.result();

            // convert to std::string
            std::string in(res.toStdString());

            hashByteArray.Append(in);
            DocName = m_docname_handle;
        }

        if ( 0 == hashByteArray.Size() ) {
            MWLOG_ERR( logBuf, "getHash failed\n" );
            return ERR_GET_HASH;
        }

        /* printData */
        if ( isDBG ) {
            printData( (char *)"\nhash: "
                        , hashByteArray.GetBytes()
                        , hashByteArray.Size() );
        }

        /*
          The actual signature input for RSA is a DigestInfo ASN.1 structure according to PKCS #1
          and we are always using SHA-256 digest so it should work like this
          For reference
          DigestInfo ::= SEQUENCE {
                  digestAlgorithm AlgorithmIdentifier,
                  digest OCTET STRING
               }
        */
        unsigned char sha256SigPrefix[] = {
            0x30, 0x31, 0x30, 0x0d, 0x06, 0x09,
        0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x01,
        0x05, 0x00, 0x04, 0x20 };

        CByteArray signatureInput(sha256SigPrefix, sizeof(sha256SigPrefix));
        signatureInput.Append(hashByteArray);

		if (isDBG) {
			MWLOG_DEBUG(logBuf, "DocName is %s", DocName.c_str());
		}

        int ret = cmdService->ccMovelSign(m_proxyInfo, signatureInput.GetBytes(), DocName, userPin);
        if ( ret != ERR_NONE ) {
            MWLOG_ERR( logBuf, "CMDSignature - Error @ sendDataToSign()" );
            return ret;
        }

        return ERR_NONE;
    }

/*  *********************************************************
    ***    CMDSignature::signOpen()                       ***
    ********************************************************* */
    int CMDSignature::signOpen(CMDProxyInfo proxyinfo, std::string in_userId, std::string in_pin
                                    , int page
                                    , double coord_x, double coord_y
                                    , const char *location
                                    , const char *reason
                                    , const char *outfile_path) {

        m_proxyInfo = proxyinfo;
        int ret = cli_getCertificate( in_userId );
        if (ret != ERR_NONE)
           return ret;

        if ( m_pdf_handler != NULL )
        {
            PDFSignature *pdf = m_pdf_handler->getPdfSignature();

            if (coord_x >= 0 && coord_y >= 0){
                pdf->setVisibleCoordinates(page, coord_x, coord_y);
            }

            ret = pdf->signFiles(location, reason, outfile_path);
            if ( ret != ERR_NONE ) {
                MWLOG_ERR( logBuf, "PDFSignature::signFiles failed: %d", ret );
                return ERR_SIGN_PDF;
            }
        }
        else
        {
            if ( isDBG ) {
                printf( " Sign String\n" );
                printData( (char *)"\n String: ",
                           (unsigned char *)m_docname_handle.c_str(), m_docname_handle.size());
            }
        }

        ret = cli_sendDataToSign( in_pin );
        return ret;
    }


    int CMDSignature::cli_getSignature(std::string in_code,
                                    PTEID_ByteArray &out_sign) {
        if ( NULL == m_pdf_handler
             && (m_docname_handle.size() == 0 || m_array_handler.size() == 0)) {
            MWLOG_ERR( logBuf, "NULL pdf_handler" );
            return ERR_NULL_HANDLER;
        }

        /* printData */
        if ( isDBG ) {
            printData( (char *)"\nReceived code: ",
                        (unsigned char *)in_code.c_str(),
                        in_code.size() );
        }

        CByteArray cb;
        int ret = cmdService->getSignature(m_proxyInfo, in_code, cb);
        if ( ret != ERR_NONE )
            return ret;

        out_sign.Clear();
        out_sign.Append( (const unsigned char*)cb.GetBytes()
                        , cb.Size() );

        return ERR_NONE;
    }

    int CMDSignature::signClose( std::string in_code )
    {

        PTEID_ByteArray signature;
        int ret = cli_getSignature( in_code, signature );

        if ( ret != ERR_NONE )
            return ret;

        CByteArray signature_cba(signature.GetBytes(), signature.Size());

        if ( m_pdf_handler != NULL ) {

            PDFSignature * pdf = m_pdf_handler->getPdfSignature();
            try{
                ret = pdf->signClose(signature_cba);
            }
            catch (CMWException &e) {
                e.GetError();
                throw PTEID_Exception(e.GetError());
            }

            if (ret != ERR_NONE) {
                MWLOG_ERR( logBuf, "SignClose failed" );
                return ERR_SIGN_CLOSE;
            }
        }
        else
        {
            if ( isDBG ){
                printf( "Sign Close String\n" );
                printData( (char *)"\n String: ",
                           (unsigned char *)m_docname_handle.c_str(), m_docname_handle.size());
            }

            QByteArray ba((const char *)signature.GetBytes(),
                          signature.Size());

            QByteArray temp = ba.toHex(0);
            m_string_signature = temp.toStdString();
        }

        if ( isDBG ){
            printData( (char *)"\nSignature: "
                        , (unsigned char *)signature.GetBytes()
                        , signature.Size() );
        }

        return ERR_NONE;
    }

}
