#include "IcaoCard.h"

namespace eIDMW {
CIcaoCard::CIcaoCard(SCARDHANDLE hCard, CContext *poContext, GenericPinpad *poPinpad, const void *protocol)
	: CPkiCard(hCard, poContext, poPinpad) {

	setProtocol(protocol);
	m_cardType = CARD_ICAO;
}

CIcaoCard::~CIcaoCard(void) {}

tCardType CIcaoCard::GetType() { return tCardType::CARD_ICAO; }

void CIcaoCard::InitEncryptionKey() { m_oCache.setEncryptionKey({}); }

tFileInfo CIcaoCard::SelectFile(const std::string &csPath, bool bReturnFileInfo) {
	return CPkiCard::SelectFile(csPath, ICAO_APPLET_MRTD, bReturnFileInfo);
}

CByteArray CIcaoCard::GetSerialNrBytes() { throw CMWEXCEPTION(EIDMW_ERR_NOT_SUPPORTED); }

DlgPinUsage CIcaoCard::PinUsage2Dlg(const tPin &Pin, const tPrivKey *pKey) {
	throw CMWEXCEPTION(EIDMW_ERR_NOT_SUPPORTED);
}

bool CIcaoCard::PinCmd(tPinOperation operation, const tPin &Pin, const std::string &csPin1, const std::string &csPin2,
					   unsigned long &ulRemaining, const tPrivKey *pKey, bool bShowDlg, void *wndGeometry,
					   unsigned long unblockFlags) {
	throw CMWEXCEPTION(EIDMW_ERR_NOT_SUPPORTED);
}

unsigned long CIcaoCard::PinStatus(const tPin &Pin) { throw CMWEXCEPTION(EIDMW_ERR_NOT_SUPPORTED); }

bool CIcaoCard::isPinVerified(const tPin &Pin) { throw CMWEXCEPTION(EIDMW_ERR_NOT_SUPPORTED); }

CByteArray CIcaoCard::RootCAPubKey() { throw CMWEXCEPTION(EIDMW_ERR_NOT_SUPPORTED); }

bool CIcaoCard::Activate(const char *pinCode, CByteArray &BCDDate, bool blockActivationPIN) {
	throw CMWEXCEPTION(EIDMW_ERR_NOT_SUPPORTED);
}

bool CIcaoCard::unlockPIN(const tPin &pin, const tPin *puk, const char *pszPuk, const char *pszNewPin,
						  unsigned long &triesLeft, unsigned long unblockFlags) {
	throw CMWEXCEPTION(EIDMW_ERR_NOT_SUPPORTED);
}

void CIcaoCard::ReadSerialNumber() { throw CMWEXCEPTION(EIDMW_ERR_NOT_SUPPORTED); }

unsigned long CIcaoCard::GetSupportedAlgorithms() { throw CMWEXCEPTION(EIDMW_ERR_NOT_SUPPORTED); }

bool CIcaoCard::ShouldSelectApplet(unsigned char ins, unsigned long ulSW12) {
	throw CMWEXCEPTION(EIDMW_ERR_NOT_SUPPORTED);
}

bool CIcaoCard::SelectApplet() { throw CMWEXCEPTION(EIDMW_ERR_NOT_SUPPORTED); }

void CIcaoCard::showPinDialog(tPinOperation operation, const tPin &Pin, std::string &csPin1, std::string &csPin2,
							  const tPrivKey *pKey, void *wndGeometry) {
	throw CMWEXCEPTION(EIDMW_ERR_NOT_SUPPORTED);
}

void CIcaoCard::SetSecurityEnv(const tPrivKey &key, unsigned long paddingType, unsigned long ulInputLen) {
	throw CMWEXCEPTION(EIDMW_ERR_NOT_SUPPORTED);
}

CByteArray CIcaoCard::SignInternal(const tPrivKey &key, unsigned long paddingType, const CByteArray &oData,
								   const tPin *pPin) {
	throw CMWEXCEPTION(EIDMW_ERR_NOT_SUPPORTED);
}
} // namespace eIDMW
