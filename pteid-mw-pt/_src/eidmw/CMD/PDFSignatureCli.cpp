#include "PDFSignatureCli.h"
#include "MiscUtil.h"

#define ERR_NONE            0
#define ERR_GET_CERTIFICATE 1
#define ERR_SEND_HASH       2
#define ERR_GET_SIGNATURE   3
#define ERR_NULL_PDF        4

static char logBuf[512];

namespace eIDMW{
    PDFSignatureCli::PDFSignatureCli( PDFSignature *in_pdf ){
        pdf = in_pdf;
    }/* PDFSignatureCli::PDFSignatureCli() */

    PDFSignatureCli::~PDFSignatureCli(){
        pdf = NULL;
    }/* PDFSignatureCli::PDFSignatureCli() */

/*  *********************************************************
    ***    PDFSignatureCli::signOpen()                    ***
    ********************************************************* */
    int PDFSignatureCli::signOpen( std::string in_userId
                                    , std::string in_pin
                                    , const char *location
                                    , const char *reason
                                    , const char *outfile_path ){

        if ( NULL == pdf ){
            MWLOG_ERR( logBuf, "ZERO pdf" );
            return ERR_NULL_PDF;
        }/* if ( NULL == pdf ) */

        printf( "\n*******************************\n"
                  "*** getCertificate          ***\n"
                  "*******************************\n" );
        CByteArray certificate = cmdService.getCertificate( in_userId );
        if ( 0 == certificate.Size() ){
            MWLOG_ERR( logBuf, "getCertificate failed\n" );
            return ERR_GET_CERTIFICATE;
        }/* if ( 0 == certificate.Size() ) */

        pdf->setPdfCertificate( certificate );

        /*
            Calculate hash
        */
        std::string in_hash = "\xde\xb2\x53\x63\xff\x9c\x44\x2b\x67\xcb\xa3\xd9\xc5\xef\x21\x6e\x47\x22\xca\xd5";

        printf( "\n*******************************\n"
                  "*** sendDataToSign          ***\n"
                  "*******************************\n" );
        if ( !cmdService.sendDataToSign( in_hash, in_pin ) ){
            MWLOG_ERR( logBuf, "main() - Error @ sendDataToSign()\n" );
            return ERR_SEND_HASH;
        }/* if ( !cmdService.sendDataToSign() ) */

        int ret = 0;
        //ret = pdf->signFiles( location, reason, outfile_path );
        printf( "%s() - TODO:: pdf->signFiles()\n", __FUNCTION__ );

        return ret;
    }/* PDFSignatureCli::signOpen() */

/*  *********************************************************
    ***    PDFSignatureCli::signClose()                   ***
    ********************************************************* */
    int PDFSignatureCli::signClose( std::string in_code ){

        if ( NULL == pdf ){
            MWLOG_ERR( logBuf, "ZERO pdf\n" );
            return ERR_NULL_PDF;
        }/* if ( NULL == pdf ) */

        // getSignature
        printf( "\n*******************************\n"
                  "*** getSignature            ***\n"
                  "*******************************\n" );
        CByteArray signature = cmdService.getSignature( in_code );
        if ( 0 == signature.Size() ){
            MWLOG_ERR( logBuf, "Error @ getSignature()\n" );
            return ERR_GET_SIGNATURE;
        }/* if ( 0 == signature.Size() ) */

        int ret = 0;
        //ret = pdf->signClose( signature );
        printf( "%s() - TODO:: pdf->signClose()\n", __FUNCTION__ );

        unsigned char *sign = signature.GetBytes();
        unsigned int signLen = signature.Size();

        printf( "Signature size(): %d\nSignature:\n", signLen );
        for( int i = 0; i < signLen; i++ ){
            printf( "0x%02x ", *(sign + i) );
        }/* for( int i ) */
        printf( "\n" );

        return ret;
    }/* int PDFSignatureCli::signClose() */

}/* namespace eIDMW */
