#include <openssl/x509.h>
#include "APLCard.h"

namespace eIDMW
{

/*
 * Returns as hex-encoded string the PKCS7 signature of the input binary data
 *
 */
int pteid_sign_pkcs7( APL_Card *card
                    , unsigned char * data, unsigned long dataLen
                    , CByteArray certData
                    , bool timestamp
                    , const char ** signature_contents );
char * get_civil_name();

CByteArray hashCalculate_pkcs7( APL_Card *card
                                , unsigned char *data, unsigned long dataLen
                                , X509 *x509
                                , bool timestamp
                                , PKCS7 *out_p7
                                , PKCS7_SIGNER_INFO **out_signer_info );

int getSignedData_pkcs7( unsigned char *signature, unsigned int signatureLen
                        , PKCS7_SIGNER_INFO *signer_info
                        , bool timestamp
                        , PKCS7 *p7
                        , const char **signature_contents );
}
