#include "APLCard.h"

/*
 * Returns as hex-encoded string the PKCS7 signature of the input binary data
 *
 */
char * pteid_sign_pkcs7 (eIDMW::APL_Card *card, unsigned char * data, unsigned long data_len, bool timestamp);
char * get_civil_name();
