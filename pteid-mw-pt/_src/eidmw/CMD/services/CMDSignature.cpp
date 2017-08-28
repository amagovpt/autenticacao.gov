#include <algorithm>
#include "CMDSignature.h"
#include "MiscUtil.h"
#include "StringOps.h"
#include "PDFSignature.h"

static char logBuf[512];

namespace eIDMW {
    bool isDBG = true;

    void printData(char *msg, unsigned char *data, unsigned int dataLen) {
        if ( NULL == msg ) {
            printf( "Data:\n" );
        }
        else {
            printf( "%s\n", msg );
        }

        for(unsigned int i = 0; i < dataLen; i++ ) {
            if ( ( i > 0) && ( 0 == ( i % 20 ))) printf("\n");
            printf( "%02x", data[i] );
        }
        printf( "\n" );
    }

    CMDSignature::CMDSignature() {
        m_pdf_handler = NULL;
    }

    CMDSignature::CMDSignature(PTEID_PDFSignature *in_pdf_handler ) {
        m_pdf_handler = in_pdf_handler;
    }

    CMDSignature::~CMDSignature() {
        m_pdf_handler = NULL;
    }

    void CMDSignature::set_pdf_handler(PTEID_PDFSignature *in_pdf_handler ) {
        m_pdf_handler = in_pdf_handler;
    }

    int CMDSignature::cli_getCertificate( std::string in_userId) {
        if ( NULL == m_pdf_handler ){
            MWLOG_ERR( logBuf, "NULL pdf_handler" );
            return ERR_NULL_PDF_HANDLER;
        }

        /* printData */
        if ( isDBG ) {
            printData( (char *)"\nUserId: "
                        , (unsigned char *)in_userId.c_str()
                        , in_userId.size() );
        }

        std::vector<CByteArray> certificates;
        int ret = cmdService.getCertificate( in_userId, certificates);

        if ( ret != ERR_NONE ) return ret;

        if ( 0 == certificates.size()) {
            MWLOG_ERR( logBuf, "getCertificate failed\n" );
            return ERR_GET_CERTIFICATE;
        }

        /* printData */
        if ( isDBG ) {
            /*
            printData( (char *)"Certificate: "
                        , certificate.GetBytes()
                        , certificate.Size() );
            */                        
        }

        PDFSignature *pdf = m_pdf_handler->getPdfSignature();
        if ( NULL == pdf ){
            MWLOG_ERR( logBuf, "NULL Pdf\n" );
            return ERR_NULL_PDF;
        }

        /* TODO: At the moment, it is only possible to sign one document. */
        pdf->setBatch_mode(false);

        pdf->setExternCertificate(certificates.at(0));
        
        std::vector<CByteArray> newVec(certificates.begin()+1, certificates.end());
        pdf->setExternCertificateCA( newVec );

        return ERR_NONE;
    }

/*  *********************************************************
    ***    CMDSignature::cli_sendDataToSign()          ***
    ********************************************************* */
    int CMDSignature::cli_sendDataToSign( std::string in_pin) {

        if ( NULL == m_pdf_handler ) {
            MWLOG_ERR( logBuf, "NULL pdf_handler" );
            return ERR_NULL_PDF_HANDLER;
        }

        /* printData */
        if (isDBG) {
            printData( (char *)"\nIN - User Pin: "
                        , (unsigned char *)in_pin.c_str()
                        , in_pin.size() );
        }

        std::string userPin = in_pin;

        if ( 0 == userPin.size() ){
            MWLOG_ERR( logBuf, "Invalid converted PIN\n" );
            return ERR_INV_USERPIN;
        }

        /* printData */
        if ( isDBG ) {
            printData( (char *)"\nUser Pin: "
                        , (unsigned char *)userPin.c_str()
                        , userPin.size() );
        }

        PDFSignature *pdf = m_pdf_handler->getPdfSignature();
        if ( NULL == pdf ) {
            MWLOG_ERR( logBuf, "NULL Pdf\n" );
            return ERR_NULL_PDF;
        }

        /* Calculate hash */
        CByteArray hashByteArray = pdf->getHash();
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

        std::string in_hash((const char *)hashByteArray.GetBytes(), hashByteArray.Size());

        int ret = cmdService.sendDataToSign(in_hash, userPin);
        if ( ret != ERR_NONE ){
            MWLOG_ERR( logBuf, "main() - Error @ sendDataToSign()\n" );
            return ret;
        }

        return ERR_NONE;
    }

/*  *********************************************************
    ***    CMDSignature::signOpen()                    ***
    ********************************************************* */
    int CMDSignature::signOpen( std::string in_userId, std::string in_pin
                                    , int page
                                    , double coord_x, double coord_y
                                    , const char *location
                                    , const char *reason
                                    , const char *outfile_path) {

        int ret = cli_getCertificate( in_userId );
        if (ret != ERR_NONE)
           return ret;

        PDFSignature *pdf = m_pdf_handler->getPdfSignature();

        pdf->setVisibleCoordinates(page, coord_x, coord_y);
        ret = pdf->signFiles(location, reason, outfile_path);

        if ( ret != ERR_NONE ) {
            MWLOG_ERR( logBuf, "PDFSignature::signFiles failed: %d", ret );
            return ERR_SIGN_PDF;
        }

        ret = cli_sendDataToSign( in_pin );
        return ret;
    }


    int CMDSignature::cli_getSignature(std::string in_code,
                                    PTEID_ByteArray &out_sign) {
        if ( NULL == m_pdf_handler ) {
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
        int ret = cmdService.getSignature( in_code, cb );
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

        PDFSignature * pdf = m_pdf_handler->getPdfSignature();

        CByteArray signature_cba(signature.GetBytes(), signature.Size());
        ret = pdf->signClose(signature_cba);

        if (ret != ERR_NONE) {
            MWLOG_ERR( logBuf, "SignClose failed" );
            return ERR_SIGN_CLOSE;
        }

        if ( isDBG ){
            printData( (char *)"\nSignature: "
                        , (unsigned char *)signature.GetBytes()
                        , signature.Size() );
        }

        return ERR_NONE;
    }

}
