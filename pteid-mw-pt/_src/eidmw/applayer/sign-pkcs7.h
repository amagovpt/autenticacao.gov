#include <openssl/x509.h>
#include "APLCard.h"

namespace eIDMW
{

/*
 * Returns as hex-encoded string the PKCS7 signature of the input binary data
 *
 */
int pteid_sign_pkcs7 (eIDMW::APL_Card *card, unsigned char * data, unsigned long data_len,
	       	bool timestamp, const char **signature_contents);
char * get_civil_name();

CByteArray hashCalculate_pkcs7( unsigned char *data, unsigned long dataLen
                                , X509 *x509
                                , PKCS7 *p7
                                , bool timestamp
                                , PKCS7_SIGNER_INFO **out_signer_info );

int dataSign_pkcs7( unsigned char *signature, unsigned int signatureLen
                    , PKCS7_SIGNER_INFO *signer_info
                    , PKCS7 *p7
                    , bool timestamp
                    , const char **signature_contents );
}
