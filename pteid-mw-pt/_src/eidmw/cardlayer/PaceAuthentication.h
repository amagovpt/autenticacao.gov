#ifndef PACEAUTHENTICATION_H
#define PACEAUTHENTICATION_H

#include "APDU.h"
#include "BacAuthentication.h"
#include "Context.h"

#include <memory>

typedef struct evp_pkey_st EVP_PKEY;
typedef struct asn1_object_st ASN1_OBJECT;

namespace eIDMW {

struct OID_INFO {
	ASN1_OBJECT *object;
	int nid;
	const char *short_name;
};

// Wrappers around OpenPace NIDs for Applayer
EIDMW_CAL_API OID_INFO get_id_CA_ECDH_3DES_CBC_CBC();
EIDMW_CAL_API OID_INFO get_id_CA_ECDH_AES_CBC_CMAC_128();
EIDMW_CAL_API OID_INFO get_id_CA_ECDH_AES_CBC_CMAC_192();
EIDMW_CAL_API OID_INFO get_id_CA_ECDH_AES_CBC_CMAC_256();
EIDMW_CAL_API OID_INFO get_id_CA_DH_3DES_CBC_CBC();
EIDMW_CAL_API OID_INFO get_id_CA_DH_AES_CBC_CMAC_128();
EIDMW_CAL_API OID_INFO get_id_CA_DH_AES_CBC_CMAC_192();
EIDMW_CAL_API OID_INFO get_id_CA_DH_AES_CBC_CMAC_256();

class PaceAuthenticationImpl;
class EIDMW_CAL_API PaceAuthentication : public SecureMessaging {
public:
	PaceAuthentication(SCARDHANDLE hCard, CContext *poContext, const void *paramStructure);
	~PaceAuthentication();

	void initPaceAuthentication(SCARDHANDLE &hCard, const void *param_structure);
	bool chipAuthentication(SCARDHANDLE &hCard, const void *param_structure, EVP_PKEY *pkey, ASN1_OBJECT *oid);
	void setAuthentication(const char *secret, size_t secretLen, PaceSecretType secretType);

	virtual CByteArray sendSecureAPDU(const APDU &apdu, long &retValue) override;
	virtual CByteArray sendSecureAPDU(const CByteArray &apdu, long &retValue) override;

private:
	CByteArray sendAPDU(const CByteArray &plainAPDU, SCARDHANDLE &hCard, long &lRetVal, const void *param_structure);
	CByteArray sendAPDU(const APDU &apdu, SCARDHANDLE &hCard, long &lRetVal, const void *param_structure);
	std::unique_ptr<PaceAuthenticationImpl> m_impl;
};

} // namespace eIDMW

#endif // PACEAUTHENTICATION_H
