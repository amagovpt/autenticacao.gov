#include "APLCard.h"

/*
 * Returns as hex-encoded string the PKCS7 signature of the input binary data
 *
 */
int pteid_sign_pkcs7 (eIDMW::APL_Card *card, unsigned char * data, unsigned long data_len,
	       	bool timestamp, const char **signature_contents);
char * get_civil_name();
