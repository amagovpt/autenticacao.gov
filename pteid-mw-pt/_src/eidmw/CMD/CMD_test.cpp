#include "PDFSignatureCli.h"
#include "eidErrors.h"

using namespace eIDMW;

int main(){

    string in_userId = "+351 914432445";
    string in_pin = "\x07\x06\x09\x05";
    string in_code = "111111";

    const char *location = "LX";
    const char *reason = "Test";
    const char *outfile_path = "./CartaoCidadao_signed.pdf";

    PDFSignature pdf;
    PDFSignatureCli client( &pdf );
    int ret;

    ret = client.signOpen( in_userId, in_pin, location, reason, outfile_path );
    if ( ret != EIDMW_OK ){
        printf( "%s() - signOpen failed\n", __FUNCTION__ );
        return -1;
    }/* if ( ret != EIDMW_OK ) */

    ret = client.signClose( in_code );
    if ( ret != EIDMW_OK ){
        printf( "%s() - signClose failed\n", __FUNCTION__ );
        return -1;
    }/* if ( ret != EIDMW_OK ) */

    return ERR_NONE;
}/* main() */
