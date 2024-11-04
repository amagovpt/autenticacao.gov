#ifndef PACEAUTHENTICATION_H
#define PACEAUTHENTICATION_H

#include "APDU.h"
#include "Context.h"

#include <memory>
#include <openssl/types.h>

namespace eIDMW {
class PaceAuthenticationImpl;
class EIDMW_CAL_API PaceAuthentication {
public:
	PaceAuthentication(CContext *poContext);
	~PaceAuthentication();

	void initPaceAuthentication(SCARDHANDLE &hCard, const void *param_structure);
	void chipAuthentication(SCARDHANDLE &hCard, const void *param_structure, EVP_PKEY *pkey);
	bool isInitialized();

	CByteArray sendAPDU(const CByteArray &plainAPDU, SCARDHANDLE &hCard, long &lRetVal, const void *param_structure);
	CByteArray sendAPDU(const APDU &apdu, SCARDHANDLE &hCard, long &lRetVal, const void *param_structure);
	void setAuthentication(const char *secret, size_t secretLen, PaceSecretType secretType);

private:
	std::unique_ptr<PaceAuthenticationImpl> m_impl;

	bool initialized;
};

} // namespace eIDMW

#endif // PACEAUTHENTICATION_H
