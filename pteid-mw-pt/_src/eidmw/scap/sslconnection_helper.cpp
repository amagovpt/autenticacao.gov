#ifdef _WIN32
#pragma warning(disable : 4996)
#include <openssl/rsa.h>
#include <openssl/ec.h>
#endif

#include "eidlib.h"
#include "eidlibException.h"
#include "Log.h"

namespace eIDMW {

/* Code related to TLS client authentication, for Windows it needs to be contained in this module (effectively in the
   GUI app) because of our use of static libraries for openssl
 */

thread_local static PTEID_EIDCard *sslconnection_card = NULL;

void setThreadLocalCardInstance(PTEID_EIDCard *card) { sslconnection_card = card; }

static int rsa_sign(int type, const unsigned char *m, unsigned int m_len, unsigned char *sigret, unsigned int *siglen,
					const RSA *rsa) {
	MWLOG(LEV_DEBUG, MOD_SCAP, "scapclient rsa_sign() called with type=%d", type);

	PTEID_ByteArray to_sign;
	to_sign.Append(m, m_len);

	PTEID_ByteArray signed_data;

	try {
		// Sign with Authentication Key
		signed_data = sslconnection_card->Sign(to_sign, false);
	} catch (PTEID_Exception &e) {
		MWLOG(LEV_ERROR, MOD_SCAP,
			  "scapclient rsa_sign(): Exception caught in card.Sign. Aborting connection! Error code: %08x",
			  e.GetError());
		return 0;
	}

	if (signed_data.Size() > 0) {
		memcpy(sigret, signed_data.GetBytes(), signed_data.Size());
		*siglen = (unsigned int)signed_data.Size();

		return 1;
	} else
		return 0;
}

static ECDSA_SIG *pteid_ecdsa_sign(const unsigned char *dgst, int dgstlen, const BIGNUM *kinv, const BIGNUM *rp,
								   EC_KEY *eckey) {

	MWLOG(LEV_DEBUG, MOD_APL, "pteid_ecdsa_sign() called with digest_len: %u", dgstlen);

	PTEID_ByteArray to_sign;
	to_sign.Append(dgst, dgstlen);
	PTEID_ByteArray signed_data;

	BIGNUM *bns = NULL, *bnr = NULL;
	// This size is only valid for curve NIST P-256
	const int EC_KEY_BYTE_LEN = 32;
	try {
		// Sign with Authentication Key
		signed_data = sslconnection_card->Sign(to_sign, false);
	} catch (PTEID_Exception &e) {
		MWLOG(LEV_ERROR, MOD_SCAP,
			  "pteid_ecdsa_sign(): Exception caught in card.Sign. Aborting connection! Error code: %08x", e.GetError());
		return NULL;
	}

	if (signed_data.Size() == EC_KEY_BYTE_LEN * 2) {
		ECDSA_SIG *signature = ECDSA_SIG_new();

		bnr = BN_bin2bn(signed_data.GetBytes(), EC_KEY_BYTE_LEN, NULL);
		bns = BN_bin2bn(signed_data.GetBytes() + EC_KEY_BYTE_LEN, EC_KEY_BYTE_LEN, NULL);

		ECDSA_SIG_set0(signature, bnr, bns);

		return signature;
	} else {
		MWLOG(LEV_ERROR, MOD_APL, "pteid_ecdsa_sign(): Unexpected length of returned signature!");
		return NULL;
	}
}

void setupSSLCallbackFunction(bool is_ecdsa) {
	if (is_ecdsa) {
		EC_KEY_METHOD *ec_method_default = (EC_KEY_METHOD *)EC_KEY_get_default_method();
		MWLOG(LEV_DEBUG, MOD_SCAP, "Modifying default EC_KEY_method: %p", ec_method_default);

		EC_KEY_METHOD *ecc_method = EC_KEY_METHOD_new(ec_method_default);
		if (ecc_method == NULL)
			return;
		// Keep the original function for sign as we only need to change the implementation of sign_sig() in
		// the EC_KEY_METHOD
		int (*orig_sign)(int, const unsigned char *, int, unsigned char *, unsigned int *, const BIGNUM *,
						 const BIGNUM *, EC_KEY *) = NULL;

		EC_KEY_METHOD_get_sign(ecc_method, &orig_sign, NULL, NULL);
		EC_KEY_METHOD_set_sign(ecc_method, orig_sign, NULL, eIDMW::pteid_ecdsa_sign);
		EC_KEY_set_default_method(ecc_method);
	} else {
		// Change the default RSA_METHOD to use our function for signing
		RSA_METHOD *current_method = (RSA_METHOD *)RSA_get_default_method();

		RSA_meth_set_sign(current_method, eIDMW::rsa_sign);
		RSA_meth_set_flags(current_method, RSA_METHOD_FLAG_NO_CHECK);
	}
}

} // namespace eIDMW