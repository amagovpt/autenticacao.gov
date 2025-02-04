#pragma once

#include "BacAuthentication.h"
#include "IcaoCard.h"
#include "PteidCard.h"

namespace eIDMW {

// TODO: Remove this class and implement this methods on the CC2 version instead
class CMultiPassCard : public CIcaoCard {
public:
	CMultiPassCard(SCARDHANDLE hCard, CContext *poContext, GenericPinpad *poPinpad, const void *protocol);

	void openBACChannel(const CByteArray &mrzInfo);
	CByteArray readToken();

	virtual tCardType GetType() override;

private:
	BacAuthentication m_bac;
};

} // namespace eIDMW
