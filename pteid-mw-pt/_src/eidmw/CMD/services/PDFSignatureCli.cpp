#include <algorithm>
#include "PDFSignatureCli.h"
#include "MiscUtil.h"
#include "StringOps.h"

static char logBuf[512];

namespace eIDMW {
    bool isDBG = true;

    void printData( char *msg, unsigned char *data, unsigned int dataLen ) {
        if ( NULL == msg ) {
            printf( "Data:\n" );
        }
        else {
            printf( "%s\n", msg );
        }

        for(int i = 0; i < dataLen; i++ ) {
            if ( ( i > 0) && ( 0 == ( i % 20 ))) printf("\n");
            printf( "%02x", data[i] );
        }
        printf( "\n" );
    }

    std::string toHex( std::string str ) {
        std::string hex;

        const char *p = str.c_str();
        for(unsigned int i = 0; i < str.size(); i++){

            if ( ( p[i] >= 0x30 ) && ( p[i] <= 0x39 )) {
                hex.push_back( p[i] - 0x30 );
            }
            else {
                hex = str;
                break;
            }
            return hex;
        }
    }

        

    std::string ltrim( std::string& str) {
        size_t pos = str.find_first_not_of(' ');
        if ( string::npos == pos ) return str;

        str.replace( 0, pos , "" );
        return str;
    }

    std::string rtrim( std::string& str) {
        size_t pos = str.find_last_not_of( ' ' );
        if ( string::npos == pos ) return str;

        str.replace( pos + 1, str.length(), "" );
        return str;
    }

    std::string trim( std::string& str) {
        size_t first = str.find_first_not_of(' ');
        if ( string::npos == first ) return "";

        size_t last = str.find_last_not_of(' ');
        return str.substr( first, ( last-first+1 ) );
    }

    PDFSignatureCli::PDFSignatureCli(PTEID_PDFSignature *in_pdf_handler ) {
        m_pdf_handler = in_pdf_handler;
    }

    PDFSignatureCli::~PDFSignatureCli() {
        m_pdf_handler = NULL;
    }

    std::string PDFSignatureCli::getUserId(){
        return m_userId;
    }

    void PDFSignatureCli::setUserId( std::string in_userId ) {
        m_userId = in_userId;
    }

    std::string PDFSignatureCli::getUserPin() {
        return m_pin;
    }

    void PDFSignatureCli::setUserPin( std::string in_pin ){
        m_pin = in_pin;
    }

    std::string PDFSignatureCli::getReceiveCode(){
        return m_receiveCode;
    }

    void PDFSignatureCli::setReceiveCode( std::string in_receiveCode) {
        m_receiveCode = in_receiveCode;
    }


    int PDFSignatureCli::cli_getCertificate( std::string in_userId) {
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

        printf( "\n*******************************\n"
                  "*** getCertificate          ***\n"
                  "*******************************\n" );
        std::vector<CByteArray> certificates;
        int ret = cmdService.getCertificate( in_userId, certificates);

        if ( ret != ERR_NONE ) return ret;

        if ( 0 == certificates.size()) {
            MWLOG_ERR( logBuf, "getCertificate failed\n" );
            return ERR_GET_CERTIFICATE;
        }

        printf( "\n*******************************\n"
                  "*** getCertificate: OK      ***\n"
                  "*******************************\n" );

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
    ***    PDFSignatureCli::cli_sendDataToSign()          ***
    ********************************************************* */
    int PDFSignatureCli::cli_sendDataToSign( std::string in_pin) {

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

        //std::string userPin = toHex( in_pin );
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

        /* Convert hashByteArray to hex std::string */
        std::string in_hash((const char *)hashByteArray.GetBytes(), hashByteArray.Size());  //hashByteArray.ToString( true, false );

        int ret = cmdService.sendDataToSign(in_hash, userPin);
        if ( ret != ERR_NONE ){
            MWLOG_ERR( logBuf, "main() - Error @ sendDataToSign()\n" );
            return ret;
        }

        return ERR_NONE;
    }

/*  *********************************************************
    ***    PDFSignatureCli::signOpen()                    ***
    ********************************************************* */
    int PDFSignatureCli::signOpen( std::string in_userId, std::string in_pin
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


    int PDFSignatureCli::cli_getSignature(std::string in_code,
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

    int PDFSignatureCli::signClose( std::string in_code )
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

        printf( "\n*******************************\n"
                  "*** getSignature: OK        ***\n"
                  "*******************************\n" );
        return ERR_NONE;
    }

}
