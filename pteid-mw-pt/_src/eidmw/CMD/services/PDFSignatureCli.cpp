#include <algorithm>
#include "PDFSignatureCli.h"
#include "MiscUtil.h"
#include "StringOps.h"

// retirar quando tivermos certificateCA - PTEID_FILE_CERT_ROOT_SIGN
#include "CardPteidDef.h"

#define EXTERNAL_CERTIFICATE

static char logBuf[512];

namespace eIDMW{
    bool isDBG = true;

    void printData( char *msg, unsigned char *data, unsigned int dataLen ) {
        if ( NULL == msg ){
            printf( "Data:\n" );
        } else{
            printf( "%s\n", msg );
        }

        for(int i = 0; i < dataLen; i++ ){
            if ( ( i > 0) && ( 0 == ( i % 20 ) ) ) printf("\n");
            printf( "0x%02x ", data[i] );
        }
        printf( "\n" );
    }/* printData() */

    std::string ltrim( std::string& str ){
        size_t pos = str.find_first_not_of(' ');
        if ( string::npos == pos ) return str;

        str.replace( 0, pos , "" );
        return str;
    }/* ltrim() */

    std::string rtrim( std::string& str ){
        size_t pos = str.find_last_not_of( ' ' );
        if ( string::npos == pos ) return str;

        str.replace( pos + 1, str.length(), "" );
        return str;
    }/* rtrim() */

    std::string trim( std::string& str ){
        size_t first = str.find_first_not_of(' ');
        if ( string::npos == first ) return "";

        size_t last = str.find_last_not_of(' ');
        return str.substr( first, ( last-first+1 ) );
    }/* trim() */

    PDFSignatureCli::PDFSignatureCli( PTEID_EIDCard *in_card, PTEID_PDFSignature *in_pdf_handler ){
        m_card = in_card;
        m_pdf_handler = in_pdf_handler;
    }/* PDFSignatureCli::PDFSignatureCli() */

    PDFSignatureCli::~PDFSignatureCli(){
        m_pdf_handler = NULL;
    }/* PDFSignatureCli::PDFSignatureCli() */

    std::string PDFSignatureCli::getUserId(){
        return m_userId;
    }/* PDFSignatureCli::getUserId() */

    void PDFSignatureCli::setUserId( std::string in_userId ){
        m_userId = in_userId;
    }/* PDFSignatureCli::setUserId() */

    std::string PDFSignatureCli::getUserPin(){
        return m_pin;
    }/* PDFSignatureCli::getUserPin() */

    void PDFSignatureCli::setUserPin( std::string in_pin ){
        m_pin = in_pin;
    }/* PDFSignatureCli::setUserPin() */

    std::string PDFSignatureCli::getReceiveCode(){
        return m_receiveCode;
    }/* PDFSignatureCli::getReceiveCode() */

    void PDFSignatureCli::setReceiveCode( std::string in_receiveCode ){
        m_receiveCode = in_receiveCode;
    }/* PDFSignatureCli::setReceiveCode() */

/*  *********************************************************
    ***    PDFSignatureCli::cli_getCertificate()          ***
    ********************************************************* */
    int PDFSignatureCli::cli_getCertificate( std::string in_userId ){

#if defined(EXTERNAL_CERTIFICATE)
        if ( NULL == m_pdf_handler ){
            MWLOG_ERR( logBuf, "NULL pdf_handler" );
            return ERR_NULL_PDF_HANDLER;
        }/* if ( NULL == m_pdf_handler ) */

        printf( "\n*******************************\n"
                  "*** getCertificate          ***\n"
                  "*******************************\n" );
        CByteArray certificate = cmdService.getCertificate( in_userId );
        if ( 0 == certificate.Size() ){
            MWLOG_ERR( logBuf, "getCertificate failed\n" );
            return ERR_GET_CERTIFICATE;
        }/* if ( 0 == certificate.Size() ) */

        printf( "\n*******************************\n"
                  "*** getCertificate: OK      ***\n"
                  "*******************************\n" );

        /* printData */
        if ( isDBG ){
            printData( (char *)"Certificate: ", certificate.GetBytes(), certificate.Size() );
        }/* if ( isDBG ) */

        PDFSignature *pdf = m_pdf_handler->getPdfSignature();
        if ( NULL == pdf ){
            MWLOG_ERR( logBuf, "NULL Pdf\n" );
            return ERR_NULL_PDF;
        }/* if ( NULL == m_pdf ) */

        //TODO - get certificateCA
        PTEID_ByteArray iData;
        long out = m_card->readFile( (const char *)PTEID_FILE_CERT_ROOT_SIGN, iData );
        if ( out < 1 ){
            MWLOG_ERR( logBuf
                    , "readFile() fail - certificate CA - read from file, out: %d", out );
            return ERR_INV_CERTIFICATE_CA;
        }/* if ( out < 1 ) */

        if ( 0 == iData.Size() ){
            MWLOG_ERR( logBuf, "Invalid certificate CA - read from file" );
            return ERR_INV_CERTIFICATE_CA;
        }/* if ( 0 == iData.Size() ) */

        CByteArray certificateCA( iData.GetBytes(), iData.Size() );
        if ( 0 == certificateCA.Size() ){
            MWLOG_ERR( logBuf, "Invalid certificate CA" );
            return ERR_INV_CERTIFICATE_CA;
        }/* if ( 0 == certificateCA.Size() ) */

        /* At this moment, it is possible sign only one document.
            Then, batch mode should be set tio false
        */
        pdf->setBatch_mode( false );
        pdf->setExternCertificate( certificate );
        pdf->setExternCertificateCA( certificateCA );
#endif /* defined(EXTERNAL_CERTIFICATE) */

        return ERR_NONE;
    }/* PDFSignatureCli::cli_getCertificate() */

/*  *********************************************************
    ***    PDFSignatureCli::cli_sendDataToSign()          ***
    ********************************************************* */
    int PDFSignatureCli::cli_sendDataToSign( std::string in_pin ){
#if defined(EXTERNAL_CERTIFICATE)
        if ( NULL == m_pdf_handler ){
            MWLOG_ERR( logBuf, "NULL pdf_handler" );
            return ERR_NULL_PDF_HANDLER;
        }/* if ( NULL == m_pdf_handler ) */

        PDFSignature *pdf = m_pdf_handler->getPdfSignature();
        if ( NULL == pdf ){
            MWLOG_ERR( logBuf, "NULL Pdf\n" );
            return ERR_NULL_PDF;
        }/* if ( NULL == m_pdf ) */

        /* Calculate hash */
        CByteArray hashByteArray = pdf->getHash();
        if ( 0 == hashByteArray.Size() ){
            MWLOG_ERR( logBuf, "getHash failed\n" );
            return ERR_GET_HASH;
        }/* if ( 0 == hashByteArray.Size() ) */

        /* printData */
        if ( isDBG ){
            printData( (char *)"\nhash: "
                        , hashByteArray.GetBytes()
                        , hashByteArray.Size() );
        }/* if ( isDBG ) */

        /* Convert hashByteArray to hex std::string */
        std::string in_hash = hashByteArray.ToString( true, false );
        replace( in_hash, "\n", "" );
        replace( in_hash, "\t", " " );

        in_hash = rtrim( in_hash );
        replace( in_hash, " ", "\\x" );
        std::transform( in_hash.begin(), in_hash.end(), in_hash.begin(), ::tolower);

        if ( isDBG ){
            std::cout << "in_hash: <" << in_hash << ">\n";
        }/* if ( isDBG ) */

        printf( "\n*******************************\n"
                  "*** sendDataToSign          ***\n"
                  "*******************************\n" );
        if ( !cmdService.sendDataToSign( in_hash, in_pin ) ){
            MWLOG_ERR( logBuf, "main() - Error @ sendDataToSign()\n" );
            return ERR_SEND_HASH;
        }/* if ( !cmdService.sendDataToSign() ) */

        printf( "\n*******************************\n"
                  "*** sendDataToSign: OK      ***\n"
                  "*******************************\n" );
#endif /* defined(EXTERNAL_CERTIFICATE) */

        return ERR_NONE;
    }/* PDFSignatureCli::cli_sendDataToSign() */

/*  *********************************************************
    ***    PDFSignatureCli::signOpen()                    ***
    ********************************************************* */
    int PDFSignatureCli::signOpen( std::string in_userId, std::string in_pin
                                    , int page
                                    , double coord_x, double coord_y
                                    , const char *location
                                    , const char *reason
                                    , const char *outfile_path ){

        if ( NULL == m_card ){
            MWLOG_ERR( logBuf, "NULL card" );
            return ERR_NULL_CARD;
        }/* if ( NULL == m_card ) */

        int ret = cli_getCertificate( in_userId );
        if ( ret != ERR_NONE ) return ret;

        ret = m_card->SignPDF( *m_pdf_handler
                                , page, coord_x, coord_y
                                , location, reason
                                , outfile_path );
        if ( ret != ERR_NONE ){
            MWLOG_ERR( logBuf, "SignPDF failed: %d", ret );
            return ERR_SIGN_PDF;
        }/* if ( ret != ERR_NONE ) */

        ret = cli_sendDataToSign( in_pin );
        return ret;
    }/* PDFSignatureCli::signOpen() */

/*  *********************************************************
    ***    PDFSignatureCli::signOpen()                    ***
    ********************************************************* */
    int PDFSignatureCli::signOpen( std::string in_userId, std::string in_pin
                                    , int page
                                    , int page_sector
                                    , bool is_landscape
                                    , const char *location
                                    , const char *reason
                                    , const char *outfile_path ){
        if ( NULL == m_card ){
            MWLOG_ERR( logBuf, "NULL card" );
            return ERR_NULL_CARD;
        }/* if ( NULL == m_card ) */

        int ret = cli_getCertificate( in_userId );
        if ( ret != ERR_NONE ) return ret;

        ret = m_card->SignPDF( *m_pdf_handler
                                , page, page_sector, is_landscape
                                , location, reason
                                , outfile_path );
        if ( ret != ERR_NONE ){
            MWLOG_ERR( logBuf, "SignPDF failed: %d", ret );
            return ERR_SIGN_PDF;
        }/* if ( ret != ERR_NONE ) */

        ret = cli_sendDataToSign( in_pin );
        return ret;
    }/* PDFSignatureCli::signOpen() */

/*  *********************************************************
    ***    PDFSignatureCli::cli_getSignature()            ***
    ********************************************************* */
    PTEID_ByteArray PDFSignatureCli::cli_getSignature( std::string in_code ){
        PTEID_ByteArray empty_signature;

        if ( NULL == m_pdf_handler ){
            MWLOG_ERR( logBuf, "NULL pdf_handler" );
            return empty_signature;
        }/* if ( NULL == m_pdf_handler ) */

        printf( "\n*******************************\n"
                  "*** getSignature            ***\n"
                  "*******************************\n" );
        CByteArray cb = cmdService.getSignature( in_code );
        if ( 0 == cb.Size() ){
            MWLOG_ERR( logBuf, "Error @ getSignature()\n" );
            return empty_signature;
        }/* if ( 0 == cb.Size() ) */

        PTEID_ByteArray signature( (const unsigned char*)cb.GetBytes()
                                    , cb.Size() );
        return signature;
    }/* PDFSignatureCli::cli_getSignature() */

/*  *********************************************************
    ***    PDFSignatureCli::signClose()                   ***
    ********************************************************* */
    int PDFSignatureCli::signClose( std::string in_code ){
#if defined(EXTERNAL_CERTIFICATE)
        if ( NULL == m_card ){
            MWLOG_ERR( logBuf, "NULL card" );
            return ERR_NULL_CARD;
        }/* if ( NULL == m_card ) */

        PTEID_ByteArray signature = cli_getSignature( in_code );
        if ( 0 == signature.Size() ) return ERR_GET_SIGNATURE;

        int ret = m_card->SignClose( *m_pdf_handler, signature );
        if ( ret != ERR_NONE ){
            MWLOG_ERR( logBuf, "SignClose failed" );
            return ERR_SIGN_CLOSE;
        }/* if ( ret != ERR_NONE ) */

        if ( isDBG ){
            printData( (char *)"\nSignature: "
                        , (unsigned char *)signature.GetBytes()
                        , signature.Size() );
        }/* if ( isDBG ) */

        printf( "\n*******************************\n"
                  "*** getSignature: OK        ***\n"
                  "*******************************\n" );
#endif /* defined(EXTERNAL_CERTIFICATE) */
        return ERR_NONE;
    }/* int PDFSignatureCli::signClose() */
}/* namespace eIDMW */
