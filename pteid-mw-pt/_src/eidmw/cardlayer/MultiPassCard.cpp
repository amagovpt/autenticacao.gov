#include "MultiPassCard.h"

/* Constructor for MultiPass cards in CL mode */
CMultiPassCard::CMultiPassCard(SCARDHANDLE hCard, CContext *poContext, GenericPinpad *poPinpad, const void *protocol)
	: CIcaoCard(hCard, poContext, poPinpad, protocol) {
	setProtocol(protocol);
	m_cardType = CARD_MULTIPASS;
	m_pace.reset(nullptr);
}

void CMultiPassCard::openBACChannel(const CByteArray &mrzInfo) {
}

CByteArray CMultiPassCard::GetRandom() {
	APDU apdu;
	apdu.ins() = 0x84;
	apdu.p1() = 0x00;
	apdu.p2() = 0x00;
	apdu.setLe(0x08);
	return SendAPDU(apdu);
}

tCardType CMultiPassCard::GetType() { return m_cardType; }

// // Must `implement` these methods for successeful compilation...
// CByteArray CMultiPassCard::GetSerialNrBytes() { throw CMWEXCEPTION(EIDMW_ERR_NOT_IMPLEMENTED); }

// unsigned long CMultiPassCard::PinStatus(const tPin &pin) { throw CMWEXCEPTION(EIDMW_ERR_NOT_IMPLEMENTED); }
// CByteArray CMultiPassCard::RootCAPubKey() { throw CMWEXCEPTION(EIDMW_ERR_NOT_IMPLEMENTED); }

// bool CMultiPassCard::Activate(const char *pinCode, CByteArray &BCDDate, bool blockActivationPIN) {
// 	throw CMWEXCEPTION(EIDMW_ERR_NOT_IMPLEMENTED);
// }
// bool CMultiPassCard::unlockPIN(const eIDMW::tPin &, const eIDMW::tPin *, const char *, const char *,
// 							   long unsigned int &, long unsigned int) {
// 	throw CMWEXCEPTION(EIDMW_ERR_NOT_IMPLEMENTED);
// }

// DlgPinUsage CMultiPassCard::PinUsage2Dlg(const eIDMW::tPin &, const eIDMW::tPrivKey *) {
// 	throw CMWEXCEPTION(EIDMW_ERR_NOT_IMPLEMENTED);
// }
// void CMultiPassCard::InitEncryptionKey() { throw CMWEXCEPTION(EIDMW_ERR_NOT_IMPLEMENTED); }
// void CMultiPassCard::ReadSerialNumber() { throw CMWEXCEPTION(EIDMW_ERR_NOT_IMPLEMENTED); }
// void CMultiPassCard::showPinDialog(eIDMW::tPinOperation, const eIDMW::tPin &, std::string &, std::string &,
// 								   const eIDMW::tPrivKey *, void *) {
// 	throw CMWEXCEPTION(EIDMW_ERR_NOT_IMPLEMENTED);
// }

// void CMultiPassCard::SetSecurityEnv(const eIDMW::tPrivKey &, long unsigned int, long unsigned int) {
// 	throw CMWEXCEPTION(EIDMW_ERR_NOT_IMPLEMENTED);
// }
// CByteArray CMultiPassCard::SignInternal(const eIDMW::tPrivKey &, long unsigned int, const eIDMW::CByteArray &,
// 										const eIDMW::tPin *) {
// 	throw CMWEXCEPTION(EIDMW_ERR_NOT_IMPLEMENTED);
// }