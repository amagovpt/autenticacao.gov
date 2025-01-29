#pragma once

#include "IcaoCard.h"
#include "PteidCard.h"

namespace eIDMW {

class CMultiPassCard : public CIcaoCard {
public:
	CMultiPassCard(SCARDHANDLE hCard, CContext *poContext, GenericPinpad *poPinpad, const void *protocol);

	void openBACChannel(const CByteArray &mrzInfo);

	virtual tCardType GetType() override;
	CByteArray GetRandom();

	// virtual CByteArray GetSerialNrBytes();
	// virtual unsigned long PinStatus(const tPin &pin);
	// virtual CByteArray RootCAPubKey();

	// virtual bool Activate(const char *pinCode, CByteArray &BCDDate, bool blockActivationPIN);
	// virtual bool unlockPIN(const eIDMW::tPin &, const eIDMW::tPin *, const char *, const char *, long unsigned int &,
	// 					   long unsigned int);
	// virtual DlgPinUsage PinUsage2Dlg(const eIDMW::tPin &, const eIDMW::tPrivKey *);
	// virtual void InitEncryptionKey();
	// virtual void ReadSerialNumber();
	// virtual void showPinDialog(eIDMW::tPinOperation, const eIDMW::tPin &, std::string &, std::string &,
	// 						   const eIDMW::tPrivKey *, void *);
	// virtual void SetSecurityEnv(const eIDMW::tPrivKey &, long unsigned int, long unsigned int);
	// virtual CByteArray SignInternal(const eIDMW::tPrivKey &, long unsigned int, const eIDMW::CByteArray &,
	// 								const eIDMW::tPin *);
};

} // namespace eIDMW