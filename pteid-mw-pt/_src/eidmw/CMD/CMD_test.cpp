#include "CMD_client.h"

#define ENDPOINT_CC_MOVEL_SIGNATURE ( (const char *)"https://dev.cmd.autenticacao.gov.pt/Ama.Authentication.Service/CCMovelSignature.svc" )

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

    X509 *certificateX509 = cmdClient->getCertificateX509( in_pin, in_userId );
    if ( certificateX509 == NULL ){
        printf( "main() - NULL x509\n" );
        return SOAP_NULL;
    }/* if ( certificateX509 == NULL ) */

    if ( certificateX509 != NULL ){
        printf( "name: %s\n", certificateX509->name );
    }/* if ( certificateX509 != NULL ) */

    /*
        Calculate hash
    */
    string in_hash = "\xde\xb2\x53\x63\xff\x9c\x44\x2b\x67\xcb\xa3\xd9\xc5\xef\x21\x6e\x47\x22\xca\xd5";

    int ret = cmdClient->CCMovelSign( in_hash, NULL, NULL );
    if ( ret != SOAP_OK ){
        printf( "main() - Error @ CCMovelSign() -> ret: %d\n", ret );
        return ret;
    }/* if ( ret != SOAP_OK ) */

    // End of CCMovelSign

    /*
        PIN confirmation
    */
    string in_code = "111111";
    unsigned char *signature = NULL;
    unsigned int signatureLen = 0;

    // ValidateOtp
    ret = cmdClient->ValidateOtp( in_code, &signature, &signatureLen );
    if ( ret != SOAP_OK ){
        printf( "main() - Error @ ValidateOtp() -> ret: %d\n", ret );
        return ret;
    }/* if ( ret != SOAP_OK ) */

    if ( signature == NULL ){
        delete cmdClient;
        printf( "%s - NULL signature\n", __FUNCTION__ );
        return -1;
    }/* if ( signature == NULL ) */

    printf( "Signature size(): %d\nSignature: ", signatureLen );
    for( int i = 0; i < signatureLen; i++ ){
        printf( "0x%02x ", *(signature + i) );
    }/* for( int i ) */
    printf( "\n" );

    free( signature );
    delete cmdClient;

    return 0;
}/* main() */
