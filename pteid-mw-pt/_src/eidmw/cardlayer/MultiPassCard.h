#pragma once

#include "IcaoCard.h"
#include "PteidCard.h"

namespace eIDMW {

typedef struct {
	unsigned char ks_enc[16];
	unsigned char ks_mac[16];
	unsigned char ssc[8];
} sm_keys_t;

class CMultiPassCard : public CIcaoCard {
public:
	CMultiPassCard(SCARDHANDLE hCard, CContext *poContext, GenericPinpad *poPinpad, const void *protocol);

	void openBACChannel(const CByteArray &mrzInfo);
	CByteArray readToken();

	virtual tCardType GetType() override;
	CByteArray GetRandom();

private:
	sm_keys_t *m_sm_keys;
};

} // namespace eIDMW