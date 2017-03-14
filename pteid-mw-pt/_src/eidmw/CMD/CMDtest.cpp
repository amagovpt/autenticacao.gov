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
    string in_hash = "\xde\xb2\x53\x63\xff\x9c\x44\x2b\x67\xcb\xa3\xd9\xc5\xef\x21\x6e\x47\x22\xca\xd5";
    string in_pin = "\x07\x06\x09\x05";
    string certificate;
    string in_code = "111111";
    unsigned char *Signature = NULL;
    unsigned int SignatureLen = 0;

    int ret = cmdClient->CCMovelSign( in_hash, in_pin, in_userId, certificate );
    if ( ret != SOAP_OK ){
        printf( "main() - Error @ CCMovelSign() -> ret: %d\n", ret );
        return ret;
    }/* if ( ret != SOAP_OK ) */

    cout << "The service returned certificate" << endl << certificate << endl << endl;
    // End of CCMovelSign

    // ValidateOtp
    ret = cmdClient->ValidateOtp( in_code, &Signature, &SignatureLen );
    if ( ret != SOAP_OK ){
        printf( "main() - Error @ ValidateOtp() -> ret: %d\n", ret );
        return ret;
    }/* if ( ret != SOAP_OK ) */

    printf( "m_Signature + Size(): %d\n", SignatureLen );

    delete Signature;
    delete cmdClient;

    return 0;
}/* main() */
