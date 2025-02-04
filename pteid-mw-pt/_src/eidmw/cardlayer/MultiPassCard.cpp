#include "MultiPassCard.h"
#include "Util.h"
#include <openssl/des.h>
#include <openssl/rand.h>
#include <openssl/sha.h>

/* Constructor for MultiPass cards in CL mode */
CMultiPassCard::CMultiPassCard(SCARDHANDLE hCard, CContext *poContext, GenericPinpad *poPinpad, const void *protocol)
	: CIcaoCard(hCard, poContext, poPinpad, protocol), m_bac(poContext) {
	setProtocol(protocol);
	m_cardType = CARD_MULTIPASS;
	m_pace.reset(nullptr);
}

void CMultiPassCard::openBACChannel(const CByteArray &mrzInfo) {
	m_bac.authenticate(m_hCard, m_comm_protocol, mrzInfo);
}

CByteArray CMultiPassCard::readToken() {
	CByteArray plaintext_apdu("00A4020C02010D", true);
	m_bac.sendSecureAPDU(plaintext_apdu);

	CByteArray read_binary("00B0000000", true);
	auto resp = m_bac.sendSecureAPDU(read_binary);
	auto token = m_bac.decryptData(resp);

	return token;
}

tCardType CMultiPassCard::GetType() { return m_cardType; }
