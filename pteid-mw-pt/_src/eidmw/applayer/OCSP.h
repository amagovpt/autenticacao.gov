/*-****************************************************************************

 * Copyright (C) 2014 André Guerreiro - <aguerreiro1985@gmail.com>
 *
 * Licensed under the EUPL V.1.1

****************************************************************************-*/

#include <openssl/x509.h>

namespace eIDMW
{
	CByteArray sendOCSPRequest(X509 *cert, X509* issuer, char *ocsp_url);
}