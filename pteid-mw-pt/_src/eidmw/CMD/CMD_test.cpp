#include "CMD_client.h"
#include "MiscUtil.h"

#define ENDPOINT_CC_MOVEL_SIGNATURE ( (const char *)"https://dev.cmd.autenticacao.gov.pt/Ama.Authentication.Service/CCMovelSignature.svc" )

#define ERR_NONE            0
#define ERR_GET_CERTIFICATE 1
#define ERR_SEND_HASH       2
#define ERR_GET_SIGNATURE   3

using namespace eIDMW;
using namespace std;

int main(){
    CMD_client *cmdClient = new CMD_client( ENDPOINT_CC_MOVEL_SIGNATURE );
    if ( !cmdClient->init( 20, 20, 20, 0 ) ){
        printf( "main() - Error @ init()\n" );
        return -1;
    }/* if ( !cmdClient->init() ) */

    string in_userId = "+351 914432445";
    string in_pin = "\x07\x06\x09\x05";

    printf( "\n*********************************\n"
              "*** cmdClient->getCertificate ***\n"
              "*********************************\n" );
    CByteArray certificate = cmdClient->getCertificate( in_pin, in_userId );
    if ( 0 == certificate.Size() ){
        printf( "main() - ZERO certificate size\n" );
        return ERR_GET_CERTIFICATE;
    }/* if ( 0 == certificate.Size() ) */

    printf( "\n**************************\n"
              "*** certificate Ok ***\n"
              "**************************\n" );

    X509 *x509 = DER_to_X509( certificate.GetBytes(), certificate.Size() );
    if ( NULL == x509 ){
        printf( "main() - NULL x509\n" );
        return ERR_GET_CERTIFICATE;
    }/* if ( NULL == x509 ) */

    printf( "name: %s\n", x509->name );

    printf( "\n**************************\n"
              "*** certificateX509 Ok ***\n"
              "**************************\n" );
    /*
        Calculate hash
    */
    string in_hash = "\xde\xb2\x53\x63\xff\x9c\x44\x2b\x67\xcb\xa3\xd9\xc5\xef\x21\x6e\x47\x22\xca\xd5";

    printf( "\n*********************************\n"
              "*** cmdClient->sendDataToSign ***\n"
              "*********************************\n" );
    bool bRet = cmdClient->sendDataToSign( in_hash );
    if ( !bRet ){
        printf( "main() - Error @ sendDataToSign()\n" );
        return ERR_SEND_HASH;
    }/* if ( !bRet ) */

    // End of CCMovelSign

    /*
        PIN confirmation
    */
    string in_code = "111111";

    // getSignature
    printf( "\n*******************************\n"
              "*** cmdClient->getSignature ***\n"
              "*******************************\n" );
    CByteArray signature = cmdClient->getSignature( in_code );
    if ( 0 == signature.Size() ){
        delete cmdClient;

        printf( "main() - Error @ getSignature()");
        return ERR_GET_SIGNATURE;
    }/* if ( 0 == signature.Size() ) */

    unsigned char *sign = signature.GetBytes();
    unsigned int signLen = signature.Size();

    printf( "Signature size(): %d\nSignature: ", signLen );
    for( int i = 0; i < signLen; i++ ){
        printf( "0x%02x ", *(sign + i) );
    }/* for( int i ) */
    printf( "\n" );

    delete cmdClient;

    return ERR_NONE;
}/* main() */
