#include <openssl/x509.h>

namespace eIDMW
{
	CByteArray sendOCSPRequest(X509 *cert, X509* issuer, char *ocsp_url);
}