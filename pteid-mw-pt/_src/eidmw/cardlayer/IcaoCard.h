#pragma once

#include "PkiCard.h"

namespace eIDMW {

class CIcaoCard : public CPkiCard {
public:
	CIcaoCard(PTEID_CardHandle hCard, CContext *poContext, GenericPinpad *poPinpad, const void *protocol);
	~CIcaoCard(void);

	virtual tCardType GetType();
	virtual CByteArray GetSerialNrBytes();

	virtual DlgPinUsage PinUsage2Dlg(const tPin &Pin, const tPrivKey *pKey);
	virtual bool PinCmd(tPinOperation operation, const tPin &Pin, const std::string &csPin1, const std::string &csPin2,
						unsigned long &ulRemaining, const tPrivKey *pKey = NULL, bool bShowDlg = true,
						void *wndGeometry = 0, unsigned long unblockFlags = 0);
	virtual unsigned long PinStatus(const tPin &Pin);
	virtual bool isPinVerified(const tPin &Pin);
	virtual CByteArray RootCAPubKey();
	virtual bool Activate(const char *pinCode, CByteArray &BCDDate, bool blockActivationPIN);
	virtual bool unlockPIN(const tPin &pin, const tPin *puk, const char *pszPuk, const char *pszNewPin,
						   unsigned long &triesLeft, unsigned long unblockFlags);

	virtual void InitEncryptionKey();
	virtual void ReadSerialNumber();

	virtual unsigned long GetSupportedAlgorithms();

protected:
	virtual bool ShouldSelectApplet(unsigned char ins, unsigned long ulSW12);
	virtual tFileInfo SelectFile(const std::string &csPath, bool bReturnFileInfo = false) override;
	virtual bool SelectApplet();
	virtual CByteArray ReadUncachedFile(const std::string &csPath, unsigned long ulOffset = 0,
										unsigned long ulMaxLen = FULL_FILE);

	virtual void showPinDialog(tPinOperation operation, const tPin &Pin, std::string &csPin1, std::string &csPin2,
							   const tPrivKey *pKey, void *wndGeometry = 0);

	virtual void SetSecurityEnv(const tPrivKey &key, unsigned long paddingType, unsigned long ulInputLen);
	virtual CByteArray SignInternal(const tPrivKey &key, unsigned long paddingType, const CByteArray &oData,
									const tPin *pPin = NULL);

	CByteArray m_oCardData;
	CByteArray m_oSerialNr;
};
} // namespace eIDMW