/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2009 FedICT.
 * Copyright (C) 2019 Caixa Magica Software.
 * Copyright (C) 2011 Vasco Silva - <vasco.silva@caixamagica.pt>
 * Copyright (C) 2012 lmcm - <lmcm@caixamagica.pt>
 * Copyright (C) 2012 Rui Martinho - <rui.martinho@ama.pt>
 * Copyright (C) 2012-2014, 2016-2018 André Guerreiro - <aguerreiro1985@gmail.com>
 * Copyright (C) 2017 Luiz Lemos - <luiz.lemos@caixamagica.pt>
 * Copyright (C) 2019 Miguel Figueira - <miguelblcfigueira@gmail.com>
 * Copyright (C) 2019 Adriano Campos - <adrianoribeirocampos@gmail.com>
 *
 * This is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License version
 * 3.0 as published by the Free Software Foundation.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this software; if not, see
 * http://www.gnu.org/licenses/.

**************************************************************************** */

// pteid-common headers
#include "Log.h"
#include "Config.h"
// cardlayer headers
#include "MultiPassCard.h"
#include "Reader.h"
#include "Card.h"
#include "APDU.h"
#include "ReaderDeviceInfo.h"

#include "CardFactory.h"
#include <openssl/types.h>

namespace eIDMW {

static const unsigned char SHA1_AID[] = {0x30, 0x21, 0x30, 0x09, 0x06, 0x05, 0x2b, 0x0e,
										 0x03, 0x02, 0x1a, 0x05, 0x00, 0x04, 0x14};
static const unsigned char SHA256_AID[] = {0x30, 0x31, 0x30, 0x0d, 0x06, 0x09, 0x60, 0x86, 0x48, 0x01,
										   0x65, 0x03, 0x04, 0x02, 0x01, 0x05, 0x00, 0x04, 0x20};
static const unsigned char SHA384_AID[] = {0x30, 0x41, 0x30, 0x0d, 0x06, 0x09, 0x60, 0x86, 0x48, 0x01,
										   0x65, 0x03, 0x04, 0x02, 0x02, 0x05, 0x00, 0x04, 0x30};
static const unsigned char SHA512_AID[] = {0x30, 0x51, 0x30, 0x0d, 0x06, 0x09, 0x60, 0x86, 0x48, 0x01,
										   0x65, 0x03, 0x04, 0x02, 0x03, 0x05, 0x00, 0x04, 0x40};

CReader::CReader(const std::string &csReader, CContext *poContext)
	: m_poCard(NULL), m_oPKCS15(poContext), m_isContactless(false) {
	m_csReader = csReader;
	m_wsReader = utilStringWiden(csReader);
	m_poContext = poContext;
	m_poCard = NULL;
	m_bIgnoreRemoval = false;
	m_oPinpad = new CPinpad(m_poContext, m_csReader);
}

CReader::~CReader(void) {
	if (m_poCard != NULL)
		Disconnect(DISCONNECT_RESET_CARD);

	delete m_oPinpad;
}

std::string &CReader::GetReaderName() { return m_csReader; }

unsigned long CReader::SetEventCallback(void (*callback)(long lRet, unsigned long ulState, void *pvRef), void *pvRef) {
	unsigned long ulHandle;
	CEventCallbackThread &oEventCallbackThread =
		m_poContext->m_oThreadPool.NewThread(m_csReader, callback, ulHandle, pvRef);

	// Start the thread
	oEventCallbackThread.Start();

	MWLOG(LEV_INFO, MOD_CAL, L"    Started event callback thread %d", ulHandle);

	return ulHandle;
}

bool CReader::CardPresent(unsigned long ulState) { return (ulState & 0x20) == 0x20; }

void CReader::StopEventCallback(unsigned long ulHandle) {
	m_poContext->m_oThreadPool.RemoveThread(ulHandle);
	MWLOG(LEV_INFO, MOD_CAL, L"    Stopped event callback thread %d", ulHandle);
}

// Use for logging in Status()
static const inline wchar_t *Status2String(tCardStatus status) {
	switch (status) {
	case CARD_INSERTED:
		return L"card inserted";
	case CARD_NOT_PRESENT:
		return L"no card present";
	case CARD_STILL_PRESENT:
		return L"card still present";
	case CARD_REMOVED:
		return L"card removed";
	case CARD_OTHER:
		return L"card removed and (another) card inserted";
	default:
		return L"unknown state?!?";
	}
}

tCardStatus CReader::Status(bool bReconnect) {
	tCardStatus status;
	static int iStatusCount = 0;

	if (m_poCard == NULL) {
		if (m_poContext->m_oPCSC.Status(m_csReader)) {
			status = Connect() ? CARD_INSERTED : CARD_NOT_PRESENT;
		} else
			status = CARD_NOT_PRESENT;
	} else {
		bool bCardStillPresent = m_poCard->Status();
		if (bCardStillPresent) {
			status = CARD_STILL_PRESENT;
		} else {
			Disconnect();
			// if bReconnect = true, then we try to connect to a
			// possibly new card that has been inserted
			if (bReconnect && m_poContext->m_oPCSC.Status(m_csReader))
				status = Connect() ? CARD_OTHER : CARD_REMOVED;
			else
				status = CARD_REMOVED;
		}
	}

	if (iStatusCount < 5) {
		MWLOG(LEV_DEBUG, MOD_CAL, L"    ReaderStatus(): %ls", Status2String(status));
		iStatusCount++;
	}

	return status;
}

// Used for logging in Connect()
static const inline wchar_t *Type2String(tCardType cardType) {
	switch (cardType) {
	case CARD_PTEID_IAS07:
	case CARD_PTEID_IAS101:
	case CARD_PTEID_IAS5:
		return L"PT eID";
		;
	default:
		return L"unknown";
	}
}

// Tags specified in PC/SC Part 10
#define PCSCv2_PART10_PROPERTY_wIdVendor 11	 /**< wIdVendor */
#define PCSCv2_PART10_PROPERTY_wIdProduct 12 /**< wIdProduct */

static void parse_reader_device_properties(unsigned char *bRecvBuffer, unsigned long length, int *vendor_id,
										   int *product_id) {
	unsigned char *p;

	p = bRecvBuffer;
	while (p - bRecvBuffer < length) {
		int tag, len, value, other_prop;

		tag = *p++;
		len = *p++;

		switch (len) {
		case 1:
			value = *p;
			break;
		case 2:
			value = *p + (*(p + 1) << 8);
			break;
		case 4:
			value = *p + (*(p + 1) << 8) + (*(p + 2) << 16) + (*(p + 3) << 24);
			break;
		default:
			value = -1;
		}

		switch (tag) {

		case PCSCv2_PART10_PROPERTY_wIdVendor:
			*vendor_id = value;
			break;
		case PCSCv2_PART10_PROPERTY_wIdProduct:
			*product_id = value;
			break;
		default:
			// Property we don't currently use
			other_prop = value;
		}

		p += len;
	}
}

void CReader::readerDeviceInfo(SCARDHANDLE hCard, ReaderDeviceInfo *deviceInfo, int ioctl_get_properties_tlv) {
	// No input data for this reader command
	CByteArray oCmd;
	try {

		CByteArray resp = m_poContext->m_oPCSC.Control(hCard, ioctl_get_properties_tlv, oCmd);
		parse_reader_device_properties(resp.GetBytes(), resp.Size(), &deviceInfo->vendorID, &deviceInfo->productID);
	} catch (const CMWException &e) {
		MWLOG(LEV_WARN, MOD_CAL, "Failed to get device info for reader: %s Error code: %ld", m_csReader.c_str(),
			  e.GetError());
	}
}

void CReader::UseHandle(SCARDHANDLE hCard) {
	if (m_poCard) {
		// reset last application incase card was reset
		m_poCard->ResetApplication();

		m_poCard->m_hCard = hCard;
	}

	if (m_oPinpad)
		m_oPinpad->Init(hCard);
}

bool CReader::Connect(SCARDHANDLE hCard, DWORD protocol) {
	m_poCard = CardConnect(hCard, protocol, m_csReader, m_poContext, NULL, m_isContactless);
	if (m_poCard != NULL) {
		if (m_isContactless)
			m_poCard->createPace();

		m_oPKCS15.SetCard(m_poCard);
		m_oPinpad->Init(m_poCard->m_hCard);
		CConfig config;
		long pinpadEnabled = config.GetLong(CConfig::EIDMW_CONFIG_PARAM_GENERAL_PINPAD_ENABLED);
		if (pinpadEnabled == 1 && m_oPinpad->UsePinpad()) {
			MWLOG(LEV_DEBUG, MOD_CAL, L"Using Pinpad reader. pinpadEnabled=%ld", pinpadEnabled);
			m_poCard->setPinpadHandler(m_oPinpad->getPinpadHandler());

		} else
			MWLOG(LEV_DEBUG, MOD_CAL, L"Using non-pinpad reader. pinpadEnabled=%ld", pinpadEnabled);

#ifdef WIN32
		// Get info on all connected readers using Win32 SetupAPI as TLV Properties Control command is not available for
		// all readers
		std::vector<ReaderDeviceInfo> readerDevices = win32ReaderDevices();
		for (auto dev : readerDevices) {
			MWLOG(LEV_INFO, MOD_CAL, "Windows reader: %s %s (vendorID: %04x, productID: %04x) Driver: %s",
				  dev.manufacturer.c_str(), dev.name.c_str(), dev.vendorID, dev.productID, dev.driver.c_str());
		}
		MWLOG(LEV_INFO, MOD_CAL, L" Connected to %ls card in reader %ls", Type2String(m_poCard->GetType()),
			  m_wsReader.c_str());
#else
		ReaderDeviceInfo device_info = {0};
		readerDeviceInfo(m_poCard->m_hCard, &device_info, m_oPinpad->getTlvPropertiesIoctl());
		MWLOG(LEV_INFO, MOD_CAL, L" Connected to %ls card in reader %ls (vendorID: %04x, productID: %04x)",
			  Type2String(m_poCard->GetType()), m_wsReader.c_str(), device_info.vendorID, device_info.productID);
#endif
	}

	return true;
}

bool CReader::Connect() {
	if (m_poCard != NULL)
		Disconnect(DISCONNECT_LEAVE_CARD);

	m_poCard = CardConnect(m_csReader, m_poContext, NULL, m_isContactless);
	if (m_poCard != NULL) {
		if (m_isContactless || m_poCard->GetType() == tCardType::CARD_ICAO) {
			m_poCard->createPace();
		}

		m_oPKCS15.SetCard(m_poCard);
		m_oPinpad->Init(m_poCard->m_hCard);
		CConfig config;
		long pinpadEnabled = config.GetLong(CConfig::EIDMW_CONFIG_PARAM_GENERAL_PINPAD_ENABLED);
		if (pinpadEnabled == 1 && m_oPinpad->UsePinpad()) {
			MWLOG(LEV_DEBUG, MOD_CAL, L"Using Pinpad reader. pinpadEnabled=%ld", pinpadEnabled);
			m_poCard->setPinpadHandler(m_oPinpad->getPinpadHandler());

		} else
			MWLOG(LEV_DEBUG, MOD_CAL, L"Using non-pinpad reader. pinpadEnabled=%ld", pinpadEnabled);

#ifdef WIN32
		// Get info on all connected readers using Win32 SetupAPI as TLV Properties Control command is not available for
		// all readers
		std::vector<ReaderDeviceInfo> readerDevices = win32ReaderDevices();
		for (auto dev : readerDevices) {
			MWLOG(LEV_INFO, MOD_CAL, "Windows reader: %s %s (vendorID: %04x, productID: %04x) Driver: %s",
				  dev.manufacturer.c_str(), dev.name.c_str(), dev.vendorID, dev.productID, dev.driver.c_str());
		}
		MWLOG(LEV_INFO, MOD_CAL, L" Connected to %ls card in reader %ls", Type2String(m_poCard->GetType()),
			  m_wsReader.c_str());
#else
		ReaderDeviceInfo device_info = {0};
		readerDeviceInfo(m_poCard->m_hCard, &device_info, m_oPinpad->getTlvPropertiesIoctl());
		MWLOG(LEV_INFO, MOD_CAL, L" Connected to %ls card in reader %ls (vendorID: %04x, productID: %04x)",
			  Type2String(m_poCard->GetType()), m_wsReader.c_str(), device_info.vendorID, device_info.productID);
#endif
	}

	return m_poCard != NULL;
}

void CReader::setSSO(bool value) { m_poCard->setSSO(value); }

void CReader::Disconnect(tDisconnectMode disconnectMode) {
	m_oPKCS15.Clear(NULL);

	if (m_poCard != NULL) {
		// Strange behaviour with Ctrl-C:
		// It's possible that this function is called multiple times. Normally,
		// this doesn't hurt except that after a Ctrl-C, m_poCard->Disconnect()
		// throws us out of this function WITHOUT an exception! So the m_poCard
		// is not deleted() and set to NULL allthough the next call to this function
		// it contains rubbish => CRASH.
		// So we set m_poCard = NULL in advance, and only if an exception is thrown
		// we assign it the old value.
		CCard *poTemp = m_poCard;
		m_poCard = NULL;
		try {
			poTemp->Disconnect(disconnectMode);
			MWLOG(LEV_INFO, MOD_CAL, L" Disconnected from card in reader %ls", m_wsReader.c_str());
			delete poTemp;
		} catch (...) {
			m_poCard = poTemp;
		}
	}
}

CByteArray CReader::GetATR() {
	if (m_poCard == NULL)
		throw CMWEXCEPTION(EIDMW_ERR_NO_CARD);

	return m_poCard->GetATR();
}

void CReader::setNextAPDUClearText() {
	if (m_poCard == NULL)
		throw CMWEXCEPTION(EIDMW_ERR_NO_CARD);

	return m_poCard->setNextAPDUClearText();
}

/*TODO: Is this really needed?    */
bool CReader::IsPinpadReader() { return m_oPinpad->UsePinpad(); }

tCardType CReader::GetCardType() {
	if (m_poCard == NULL)
		throw CMWEXCEPTION(EIDMW_ERR_NO_CARD);

	return m_poCard->GetType();
}

std::string CReader::GetSerialNr() {
	if (m_poCard == NULL)
		throw CMWEXCEPTION(EIDMW_ERR_NO_CARD);

	try {
		return m_poCard->GetSerialNr();
	} catch (CMWException &e) {
		unsigned long err = e.GetError();
		err = err;
		return m_oPKCS15.GetSerialNr();
	}
}

CByteArray CReader::GetSerialNrBytes() {
	if (m_poCard == NULL)
		throw CMWEXCEPTION(EIDMW_ERR_NO_CARD);

	return m_poCard->GetSerialNrBytes();
}

std::string CReader::GetCardLabel() {
	if (m_poCard == NULL)
		throw CMWEXCEPTION(EIDMW_ERR_NO_CARD);

	std::string csLabel = m_poCard->GetLabel();

	if (csLabel == "")
		csLabel = m_oPKCS15.GetCardLabel();

	return csLabel;
}

std::string CReader::GetAppletVersion() {
	if (m_poCard == NULL)
		throw CMWEXCEPTION(EIDMW_ERR_NO_CARD);

	return m_poCard->GetAppletVersion();
}

void CReader::Lock() {
	if (m_poCard == NULL)
		throw CMWEXCEPTION(EIDMW_ERR_NO_CARD);

	return m_poCard->Lock();
}

void CReader::Unlock() {
	if (m_poCard == NULL)
		throw CMWEXCEPTION(EIDMW_ERR_NO_CARD);

	return m_poCard->Unlock();
}

void CReader::SelectApplication(const CByteArray &oAID) {
	if (m_poCard == NULL)
		throw CMWEXCEPTION(EIDMW_ERR_NO_CARD);

	return m_poCard->SelectApplication(oAID);
}

void CReader::ResetApplication() {
	if (m_poCard == NULL)
		throw CMWEXCEPTION(EIDMW_ERR_NO_CARD);

	return m_poCard->ResetApplication();
}

bool CReader::isCardContactless() const { return m_isContactless; }

void CReader::initPaceAuthentication(const char *secret, size_t secretLen, PaceSecretType secretType) {
	m_poCard->initPaceAuthentication(secret, secretLen, secretType);
}

bool CReader::initChipAuthentication(EVP_PKEY *pkey, ASN1_OBJECT *oid) {
	return m_poCard->initChipAuthentication(pkey, oid);
}

void CReader::openBACChannel(const CByteArray& mrz_info) {
	printf("CReader:: Trying to open BAC Channel\n");
	reinterpret_cast<CMultiPassCard*>(m_poCard)->openBACChannel(mrz_info);
	printf("Done\n");
}

CByteArray CReader::ReadFile(const std::string &csPath, unsigned long ulOffset, unsigned long ulMaxLen,
							 bool bDoNotCache) {
	if (m_poCard == NULL)
		throw CMWEXCEPTION(EIDMW_ERR_NO_CARD);

	try {
		return m_poCard->ReadFile(csPath, ulOffset, ulMaxLen, bDoNotCache);
	} catch (const CNotAuthenticatedException &e) {
		// A PIN is needed to read -> ask the correct PIN and do a verification
		unsigned long ulRemaining;
		tPin pin = m_oPKCS15.GetPinByRef(e.GetPinRef());
		if (pin.bValid) {
			if (m_poCard->PinCmd(PIN_OP_VERIFY, pin, "", "", ulRemaining, NULL)) {
				return m_poCard->ReadFile(csPath, ulOffset, ulMaxLen);
			} else
				throw CMWEXCEPTION(ulRemaining == 0 ? EIDMW_ERR_PIN_BLOCKED : EIDMW_ERR_PIN_BAD);
		} else
			throw CMWEXCEPTION(EIDMW_ERR_CMD_NOT_ALLOWED);
	}
}

void CReader::WriteFile(const std::string &csPath, unsigned long ulOffset, const CByteArray &oData) {
	if (m_poCard == NULL)
		throw CMWEXCEPTION(EIDMW_ERR_NO_CARD);

	try {
		return m_poCard->WriteFile(csPath, ulOffset, oData);
	} catch (const CNotAuthenticatedException &e) {
		// A PIN is needed to write -> ask the correct PIN and do a verification
		unsigned long ulRemaining;
		tPin pin = m_oPKCS15.GetPinByRef(e.GetPinRef());
		if (pin.bValid) {
			if (m_poCard->PinCmd(PIN_OP_VERIFY, pin, "", "", ulRemaining, NULL)) {
				return m_poCard->WriteFile(csPath, ulOffset, oData);
			} else
				throw CMWEXCEPTION(ulRemaining == 0 ? EIDMW_ERR_PIN_BLOCKED : EIDMW_ERR_PIN_BAD);
		} else
			throw CMWEXCEPTION(EIDMW_ERR_CMD_NOT_ALLOWED);
	}
}

unsigned long CReader::PinStatus(const tPin &Pin) {
	if (m_poCard == NULL)
		throw CMWEXCEPTION(EIDMW_ERR_NO_CARD);

	return m_poCard->PinStatus(Pin);
}

bool CReader::isPinVerified(const tPin &Pin) {
	if (m_poCard == NULL)
		throw CMWEXCEPTION(EIDMW_ERR_NO_CARD);

	return m_poCard->isPinVerified(Pin);
}

CByteArray CReader::RootCAPubKey() {
	if (m_poCard == NULL)
		throw CMWEXCEPTION(EIDMW_ERR_NO_CARD);

	return m_poCard->RootCAPubKey();
}

bool CReader::Activate(const char *pinCode, CByteArray &BCDDate, bool blockActivationPIN) {
	if (m_poCard == NULL)
		throw CMWEXCEPTION(EIDMW_ERR_NO_CARD);

	return m_poCard->Activate(pinCode, BCDDate, blockActivationPIN);
}

bool CReader::unlockPIN(const tPin &pin, const tPin *puk, const char *pszPuk, const char *pszNewPin,
						unsigned long &triesLeft, unsigned long unblockFlags) {
	if (m_poCard == NULL)
		throw CMWEXCEPTION(EIDMW_ERR_NO_CARD);

	return m_poCard->unlockPIN(pin, puk, pszPuk, pszNewPin, triesLeft, unblockFlags);
}

bool CReader::PinCmd(tPinOperation operation, const tPin &Pin, const std::string &csPin1, const std::string &csPin2,
					 unsigned long &ulRemaining, bool bShowDlg, void *wndGeometry, unsigned long unblockFlags) {
	if (m_poCard == NULL)
		throw CMWEXCEPTION(EIDMW_ERR_NO_CARD);

	return m_poCard->PinCmd(operation, Pin, csPin1, csPin2, ulRemaining, NULL, bShowDlg, wndGeometry, unblockFlags);
}

unsigned long CReader::GetSupportedAlgorithms() {
	if (m_poCard == NULL)
		throw CMWEXCEPTION(EIDMW_ERR_NO_CARD);

	return m_poCard->GetSupportedAlgorithms();
}

void CReader::setAskPinOnSign(bool bAsk) {
	if (m_poCard == NULL)
		throw CMWEXCEPTION(EIDMW_ERR_NO_CARD);

	m_poCard->setAskPinOnSign(bAsk);
}

CByteArray CReader::Sign(const tPrivKey &key, unsigned long paddingType, const CByteArray &oData) {
	if (m_poCard == NULL)
		throw CMWEXCEPTION(EIDMW_ERR_NO_CARD);

	unsigned long ulSupportedAlgos = m_poCard->GetSupportedAlgorithms();

	CByteArray oAID_Data;

	if (m_poCard->GetType() == CARD_PTEID_IAS101) {
		if (oData.Size() == SHA1_LEN)
			oAID_Data.Append(SHA1_AID, sizeof(SHA1_AID));
		else if (oData.Size() == SHA256_LEN)
			oAID_Data.Append(SHA256_AID, sizeof(SHA256_AID));
		else
			throw CMWEXCEPTION(EIDMW_ERR_ALGO_BAD);
	}

	oAID_Data.Append(oData);

	if (ulSupportedAlgos & SIGN_ALGO_RSA_PKCS || ulSupportedAlgos & SIGN_ALGO_ECDSA) {
		return m_poCard->Sign(key, GetPinByID(key.ulAuthID), paddingType, oAID_Data);
	} else if (ulSupportedAlgos & SIGN_ALGO_RSA_RAW) {
		if (oAID_Data.Size() > key.ulKeyLenBytes - 11) {
			throw CMWEXCEPTION(EIDMW_ERR_PARAM_RANGE);
		}

		CByteArray oRawData(NULL, 0, key.ulKeyLenBytes);

		oRawData.Append(0x00);
		oRawData.Append(0x01);
		for (unsigned long i = 2; i < key.ulKeyLenBytes - oAID_Data.Size() - 1; i++)
			oRawData.Append(0xFF);
		oRawData.Append(0x00);
		oRawData.Append(oAID_Data);

		return m_poCard->Sign(key, GetPinByID(key.ulID), SIGN_ALGO_RSA_RAW, oData);
	} else
		throw CMWEXCEPTION(EIDMW_ERR_CHECK);
}

CByteArray CReader::GetRandom(unsigned long ulLen) {
	if (m_poCard == NULL)
		throw CMWEXCEPTION(EIDMW_ERR_NO_CARD);

	return m_poCard->GetRandom(ulLen);
}

CByteArray CReader::SendAPDU(const CByteArray &oCmdAPDU) {
	if (m_poCard == NULL)
		throw CMWEXCEPTION(EIDMW_ERR_NO_CARD);

	return m_poCard->SendAPDU(oCmdAPDU);
}

CByteArray CReader::SendAPDU(const APDU &oCmdAPDU) {
	if (m_poCard == NULL)
		throw CMWEXCEPTION(EIDMW_ERR_NO_CARD);

	return m_poCard->SendAPDU(oCmdAPDU);
}

unsigned long CReader::PinCount() {
	if (m_poCard == NULL)
		throw CMWEXCEPTION(EIDMW_ERR_NO_CARD);

	return m_oPKCS15.PinCount();
}

tPin CReader::GetPin(unsigned long ulIndex) {
	if (m_poCard == NULL) {
		throw CMWEXCEPTION(EIDMW_ERR_NO_CARD);
	}
	return m_oPKCS15.GetPin(ulIndex);
}

tPin CReader::GetPinByID(unsigned long ulID) {
	if (m_poCard == NULL)
		throw CMWEXCEPTION(EIDMW_ERR_NO_CARD);

	return m_oPKCS15.GetPinByID(ulID);
}

unsigned long CReader::CertCount() {
	if (m_poCard == NULL)
		throw CMWEXCEPTION(EIDMW_ERR_NO_CARD);

	return m_oPKCS15.CertCount();
}

tCert CReader::GetCert(unsigned long ulIndex) {
	if (m_poCard == NULL)
		throw CMWEXCEPTION(EIDMW_ERR_NO_CARD);

	return m_oPKCS15.GetCert(ulIndex);
}

tCert CReader::GetCertByID(unsigned long ulID) {
	if (m_poCard == NULL)
		throw CMWEXCEPTION(EIDMW_ERR_NO_CARD);

	return m_oPKCS15.GetCertByID(ulID);
}

unsigned long CReader::PrivKeyCount() {
	if (m_poCard == NULL)
		throw CMWEXCEPTION(EIDMW_ERR_NO_CARD);

	return m_oPKCS15.PrivKeyCount();
}

tPrivKey CReader::GetPrivKey(unsigned long ulIndex) {
	if (m_poCard == NULL)
		throw CMWEXCEPTION(EIDMW_ERR_NO_CARD);

	return m_oPKCS15.GetPrivKey(ulIndex);
}

tPrivKey CReader::GetPrivKeyByID(unsigned long ulID) {
	if (m_poCard == NULL)
		throw CMWEXCEPTION(EIDMW_ERR_NO_CARD);

	return m_oPKCS15.GetPrivKeyByID(ulID);
}

PinUsage CReader::GetPinUsage(const tPin &pin) {
	if (m_poCard == NULL)
		throw CMWEXCEPTION(EIDMW_ERR_NO_CARD);

	DlgPinUsage usage = m_poCard->PinUsage2Dlg(pin, NULL);

	switch (usage) {
	case DLG_PIN_AUTH:
		return DLG_USG_PIN_AUTH;
	case DLG_PIN_SIGN:
		return DLG_USG_PIN_SIGN;
	case DLG_PIN_ADDRESS:
		return DLG_USG_PIN_ADDRESS;
	default:
		return PIN_USG_UNKNOWN;
	}
}

} // namespace eIDMW
