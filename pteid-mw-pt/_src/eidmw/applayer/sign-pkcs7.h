/*-****************************************************************************

 * Copyright (C) 2012-2013, 2017 André Guerreiro - <aguerreiro1985@gmail.com>
 * Copyright (C) 2016-2017 Luiz Lemos - <luiz.lemos@caixamagica.pt>
 * Copyright (C) 2019 Adriano Campos - <adrianoribeirocampos@gmail.com>
 *
 * Licensed under the EUPL V.1.1

****************************************************************************-*/

#include <openssl/x509.h>
#include <openssl/pkcs7.h>
#include <vector>
#include "APLCard.h"


namespace eIDMW
{

/*
 * Returns as hex-encoded string the PKCS7 signature of the input binary data
 *
 */
char * get_civil_name();

CByteArray PteidSign( APL_Card *card, CByteArray &to_sign );

// ca_certificates vector should be empty for card signatures because they are retrieved from already loaded
// APL_Certifs object
CByteArray computeHash_pkcs7( unsigned char *data, unsigned long dataLen
                            , CByteArray certificate
                            , std::vector<CByteArray> &ca_certificates 
                            , bool timestamp
                            , PKCS7 *p7
                            , PKCS7_SIGNER_INFO **out_signer_info 
                            , bool isCardSign);

int getSignedData_pkcs7( unsigned char *signature, unsigned int signatureLen
                        , PKCS7_SIGNER_INFO *signer_info
                        , bool timestamp
                        , PKCS7 *p7
                        , const char **signature_contents );
}
