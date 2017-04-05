#include <openssl/x509.h>
#include "APLCard.h"
#include <openssl/pkcs7.h>

namespace eIDMW
{

/*
 * Returns as hex-encoded string the PKCS7 signature of the input binary data
 *
 */
char * get_civil_name();

CByteArray PteidSign( APL_Card *card, CByteArray &to_sign );

CByteArray computeHash_pkcs7( unsigned char *data, unsigned long dataLen
                            , CByteArray certificate
                            , CByteArray CA_certificate
                            , bool timestamp
                            , PKCS7 *p7
                            , PKCS7_SIGNER_INFO **out_signer_info );

int getSignedData_pkcs7( unsigned char *signature, unsigned int signatureLen
                        , PKCS7_SIGNER_INFO *signer_info
                        , bool timestamp
                        , PKCS7 *p7
                        , const char **signature_contents );
}
