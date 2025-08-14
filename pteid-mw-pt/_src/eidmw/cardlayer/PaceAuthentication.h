#ifndef PACEAUTHENTICATION_H
#define PACEAUTHENTICATION_H

#include "APDU.h"
#include "SecureMessaging.h"
#include "Context.h"

#include <memory>

typedef struct evp_pkey_st EVP_PKEY;
typedef struct asn1_object_st ASN1_OBJECT;

namespace eIDMW {

struct OID_INFO {
	ASN1_OBJECT *object;
	int nid;
	const char *short_name;

	EIDMW_CAL_API bool is_valid();
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
	PaceAuthentication(PTEID_CardHandle hCard, CContext *poContext, const void *paramStructure);
	~PaceAuthentication();

	void initPaceAuthentication(PTEID_CardHandle &hCard, const void *param_structure);
	void setAuthentication(const char *secret, size_t secretLen, PaceSecretType secretType);

protected:
	virtual CByteArray encryptData(const CByteArray &data) override;
	virtual CByteArray decryptData(const CByteArray &encryptedData) override;
	virtual CByteArray computeMac(const CByteArray &data) override;
	virtual CByteArray addPadding(const CByteArray &data) override;
	virtual CByteArray removePadding(const CByteArray &data) override;
	virtual void incrementSSC() override;

private:
	std::unique_ptr<PaceAuthenticationImpl> m_impl;
};

} // namespace eIDMW

#endif // PACEAUTHENTICATION_H
