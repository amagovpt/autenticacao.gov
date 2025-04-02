/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2009 FedICT.
 * Copyright (C) 2019 Caixa Magica Software.
 * Copyright (C) 2011-2012 Vasco Silva - <vasco.silva@caixamagica.pt>
 * Copyright (C) 2011-2018 André Guerreiro - <aguerreiro1985@gmail.com>
 * Copyright (C) 2011-2012 lmcm - <lmcm@caixamagica.pt>
 * Copyright (C) 2012 Rui Martinho - <rui.martinho@ama.pt>
 * Copyright (C) 2016-2017 Luiz Lemos - <luiz.lemos@caixamagica.pt>
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

#include <openssl/ec.h>
#include <openssl/err.h>
#ifdef __GNUC__
#include <termios.h>
#endif

#include "PteidCard.h"
#include "Log.h"
#include "Config.h"
#include "CardLayer.h"
#include "PaceAuthentication.h"

using namespace eIDMW;

/* martinho - the id must not be changed */
static const unsigned long PTEIDNG_ACTIVATION_CODE_ID = 0x87;
/* martinho - ANY_ID_BIGGER_THAN_6 will be the ulID in the tPin struct 1-6 are already taken */
static const unsigned long ANY_ID_BIGGER_THAN_6 = 7;
/* martinho - some meaningful label */
static const std::string LABEL = "Card Activation Code";
/* martinho - date in bcd format must have 4 bytes*/
static const unsigned long BCDSIZE = 4;
/* martinho - trace file*/
static const std::string TRACEFILE = "3F000003";

unsigned long ulVersion;

static bool PteidCardSelectApplet(CContext *poContext, SCARDHANDLE hCard, const void *protocol_struct,
								  std::unique_ptr<PaceAuthentication> &paceAuthentication) {
	long lRetVal = 0;
	unsigned char tucSelectApp[] = {0x00, 0xA4, 0x04, 0x00};
	CByteArray oCmd(sizeof(PTEID_1_APPLET_AID) + 5);
	oCmd.Append(tucSelectApp, sizeof(tucSelectApp));
	oCmd.Append((unsigned char)sizeof(PTEID_1_APPLET_AID));
	oCmd.Append(PTEID_1_APPLET_AID, sizeof(PTEID_1_APPLET_AID));

	CByteArray oResp;
	if (paceAuthentication.get()) {
		oResp = paceAuthentication->sendAPDU(oCmd, hCard, lRetVal, protocol_struct);
	} else {
		oResp = poContext->m_oPCSC.Transmit(hCard, oCmd, &lRetVal, protocol_struct);
	}
	return (oResp.Size() == 2 && (oResp.GetByte(0) == 0x61 || oResp.GetByte(0) == 0x90));
}

CCard *PteidCardGetInstance(unsigned long ulVersion, const char *csReader, SCARDHANDLE hCard, CContext *poContext,
							GenericPinpad *poPinpad, const void *protocol_struct) {

	CCard *poCard = NULL;

	try {
		// Don't remove these brackets, CAutoLock dtor must be called!
		{
			CAutoLock oAutLock(&poContext->m_oPCSC, hCard);

			poCard = new CPteidCard(hCard, poContext, poPinpad, ALW_SELECT_APPLET, ulVersion, protocol_struct);
			MWLOG(LEV_DEBUG, MOD_CAL, "Creating new card instance: %p", poCard);

			// NOTE: PteidCardSelectAppllet does not support V5 cards
			// Applet already selected on

			// bool selected = PteidCardSelectApplet(poContext, hCard, protocol_struct);
			// if (selected) {
			// 	// We don't support PTEID_IAS101 cards anymore...
			// 	// ulVersion = 1;
			// 	poCard = new CPteidCard(hCard, poContext, poPinpad, ALW_SELECT_APPLET, ulVersion, protocol_struct);
			// 	MWLOG(LEV_DEBUG, MOD_CAL, "Creating new card instance: %p", poCard);
			// }
		}
	} catch (CMWException &e) {
		MWLOG(LEV_ERROR, MOD_CAL, "Exception in card object creation! Error code: %0x on %s:%ld", e.GetError(),
			  e.GetFile().c_str(), e.GetLine());
	} catch (const std::exception &e) {
		MWLOG(LEV_ERROR, MOD_CAL, "Std::exception in card object creation! Msg: %s", e.what());
	}

	return poCard;
}

CPteidCard::CPteidCard(SCARDHANDLE hCard, CContext *poContext, GenericPinpad *poPinpad,
					   tSelectAppletMode selectAppletMode, unsigned long ulVersion, const void *protocol)
	: CPkiCard(hCard, poContext, poPinpad) {
	switch (ulVersion) {
	case 1:
		m_cardType = CARD_PTEID_IAS07;
		break;
	case 2:
		m_cardType = CARD_PTEID_IAS101;
		break;
	case 3:
		m_cardType = CARD_PTEID_IAS5;
		break;
	}
	setProtocol(protocol);
	m_ucCLA = 0x00;

	ReadSerialNumber();
}

/* Constructor for IASv5 cards in CL mode */
CPteidCard::CPteidCard(SCARDHANDLE hCard, CContext *poContext, GenericPinpad *poPinpad, const void *protocol)
	: CPkiCard(hCard, poContext, poPinpad) {

	setProtocol(protocol);
	m_cardType = CARD_PTEID_IAS5;
}

void CPteidCard::ReadSerialNumber() {

	try {

		// Get card serial number
		// CPLC Data is only available on eID application on PTEID_2 cards
		if (m_cardType == CARD_PTEID_IAS5) {
			SelectApplication({PTEID_2_APPLET_EID, sizeof(PTEID_2_APPLET_EID)});
		} else {
			// The IAS v4 application was already selected in CardFactory
			m_lastSelectedApplication = {PTEID_1_APPLET_AID, sizeof(PTEID_1_APPLET_AID)};
		}
		m_oSerialNr = SendAPDU(0xCA, 0x9F, 0x7F, 0x2D).GetBytes(13, 8);
	} catch (CMWException e) {
		MWLOG(LEV_WARN, MOD_CAL, "Failed to read serial number: 0x%0x File: %s, Line:%ld", e.GetError(),
			  e.GetFile().c_str(), e.GetLine());
		Disconnect(DISCONNECT_LEAVE_CARD);
	} catch (const std::exception&) {
		MWLOG(LEV_WARN, MOD_CAL, L"Failed to read serial number std::exception thrown");
		Disconnect(DISCONNECT_LEAVE_CARD);
	}
}

CPteidCard::~CPteidCard(void) {}

tCardType CPteidCard::GetType() { return m_cardType; }

CByteArray CPteidCard::GetSerialNrBytes() {
	if (m_oSerialNr.Size() == 0 && m_cardType == CARD_PTEID_IAS5)
		ReadSerialNumber();

	return m_oSerialNr;
}

CByteArray CPteidCard::GetInfo() { return m_oCardData; }

std::string CPteidCard::GetAppletVersion() {
	std::string applet_version;
	const size_t VERSION_OFFSET = 3, VERSION_LEN = 7;
	if (m_cardType == CARD_PTEID_IAS07) {
		const unsigned char apdu_appletversion[] = {0x00, 0xCA, 0xDF, 0x30, 0x00};
		CByteArray applet_version_ba(apdu_appletversion, sizeof(apdu_appletversion));
		CByteArray resp = SendAPDU(applet_version_ba);
		unsigned long ulSW12 = getSW12(resp);

		if (ulSW12 == 0x9000) {

			const unsigned char *data = resp.GetBytes();
			applet_version.append((const char *)data + VERSION_OFFSET, VERSION_LEN);
		}
	} else {
		throw CMWEXCEPTION(EIDMW_ERR_NOT_SUPPORTED);
	}

	return applet_version;
}

unsigned long CPteidCard::PinStatus(const tPin &Pin) {
	unsigned long ulSW12 = 0;

	try {
		if (this->GetType() == CARD_PTEID_IAS5) {
			SelectApplication({PTEID_2_APPLET_EID, sizeof(PTEID_2_APPLET_EID)});
		}

		CByteArray oResp = SendAPDU(0x20, 0x00, (unsigned char)Pin.ulPinRef, 0);
		ulSW12 = getSW12(oResp);
		MWLOG(LEV_DEBUG, MOD_CAL, L"PinStatus APDU returned: %x", ulSW12);
		if (ulSW12 == 0x9000)
			return 3; // Maximum Try Counter for PteID Cards

		if (oResp.GetByte(0) != 0x63) {
			throw m_poContext->m_oPCSC.SW12ToErr(ulSW12);
		}

		return (ulSW12 % 16);
	} catch (...) {
		// m_ucCLA = 0x00;
		MWLOG(LEV_ERROR, MOD_CAL, L"Error in PinStatus: %04x", ulSW12);
		throw;
	}
}

bool CPteidCard::isPinVerified(const tPin &Pin) {

	try {
		if (this->GetType() == CARD_PTEID_IAS5) {
			SelectApplication({PTEID_2_APPLET_EID, sizeof(PTEID_2_APPLET_EID)});
		}
		CByteArray oResp = SendAPDU(0x20, 0x00, (unsigned char)Pin.ulPinRef, 0);
		unsigned long ulSW12 = getSW12(oResp);
		MWLOG(LEV_DEBUG, MOD_CAL, L"PinStatus APDU returned: %x", ulSW12);
		return ulSW12 == 0x9000;
	} catch (...) {
		// m_ucCLA = 0x00;
		MWLOG(LEV_ERROR, MOD_CAL, L"Error in isPinVerified");
		throw;
	}
}

CByteArray CPteidCard::RootCAPubKey() {
	CByteArray oResp;

	try {
		switch (GetType()) {
		case CARD_PTEID_IAS101: {
			CByteArray select("3F00", true);
			oResp = SendAPDU(0xA4, 0x00, 0x0C, select);
			getSW12(oResp, 0x9000);

			// 4D - extended header list, 04 - size, FFA001 - SDO root CA, 80 - give me all?
			CByteArray getData("4D04FFA00180", true);
			oResp = SendAPDU(0xCB, 0x3F, 0xFF, getData);
			getSW12(oResp, 0x9000);
			oResp.Chop(2); // remove the SW12 bytes
		} break;
		case CARD_PTEID_IAS07: {
			unsigned char apdu_cvc_pubkey_mod[] = {0x00, 0xCB, 0x00, 0xFF, 0x0A, 0xB6, 0x03, 0x83,
												   0x01, 0x44, 0x7F, 0x49, 0x02, 0x81, 0x00, 0x00};

			unsigned char apdu_cvc_pubkey_exponent[] = {0x00, 0xCB, 0x00, 0xFF, 0x0A, 0xB6, 0x03, 0x83,
														0x01, 0x44, 0x7F, 0x49, 0x02, 0x82, 0x00, 0x00};

			CByteArray getModule(apdu_cvc_pubkey_mod, sizeof(apdu_cvc_pubkey_mod));
			CByteArray oRespModule = SendAPDU(getModule);
			getSW12(oRespModule, 0x9000);
			oRespModule.Chop(2); // remove the SW12 bytes

			CByteArray getExponent(apdu_cvc_pubkey_exponent, sizeof(apdu_cvc_pubkey_exponent));
			CByteArray oRespExponent = SendAPDU(getExponent);
			getSW12(oRespExponent, 0x9000);
			oRespExponent.Chop(2); // remove the SW12 bytes

			oResp.Append(oRespModule);
			oResp.Append(oRespExponent);
		} break;

		case CARD_PTEID_IAS5: {
			SelectApplication({PTEID_2_APPLET_EID, sizeof(PTEID_2_APPLET_EID)});
			unsigned char apdu_cvc_pubkey[] = {0x00, 0xCB, 0x00, 0xFF, 0x0A, 0xB6, 0x03, 0x83,
											   0x01, 0x44, 0x7F, 0x49, 0x02, 0x86, 0x00, 0x00};
			CByteArray get_data_ca_pubkey(apdu_cvc_pubkey, sizeof(apdu_cvc_pubkey));
			CByteArray resp_pubkey_data = SendAPDU(get_data_ca_pubkey);
			getSW12(resp_pubkey_data, 0x9000);
			resp_pubkey_data.Chop(2); // remove the SW12 bytes
			oResp.Append(resp_pubkey_data);
		} break;

		default:
			throw CMWEXCEPTION(EIDMW_ERR_CARDTYPE_UNKNOWN);
			break;
		}
	} catch (CMWException e) {
		MWLOG(LEV_ERROR, MOD_CAL, L"Error in RootCAPubKey: Specific error code: %08x", e.GetError());
		throw;
	}
	return oResp;
}

#define COMPAT_ERR_PIN_CODE_BLOCKED -1212
#define COMPAT_ERR_PIN_CODE_INCORRECT -1214

bool CPteidCard::Activate(const char *pinCode, CByteArray &BCDDate, bool blockActivationPIN) {
	unsigned char padChar;
	CByteArray tracefile_data;

	switch (GetType()) {
	case CARD_PTEID_IAS101:
		padChar = 0x2F;
		break;
	case CARD_PTEID_IAS07:
	case CARD_PTEID_IAS5:
		padChar = 0xFF;
		break;
	default:
		throw CMWEXCEPTION(EIDMW_ERR_CARDTYPE_UNKNOWN);
	}

	/* The activation PIN is not listed in the internal card structures */
	tPin activationPin = {
		true,	 LABEL,			0,	0, 0, ANY_ID_BIGGER_THAN_6, 0, 0, 4, 8, 8, PTEIDNG_ACTIVATION_CODE_ID,
		padChar, PIN_ENC_ASCII, "", ""};
	unsigned long ulRemaining;
	std::string strPinCode = pinCode != NULL ? std::string(pinCode) : "";

	bool bOK = PinCmd(PIN_OP_VERIFY, activationPin, strPinCode, "", ulRemaining, NULL);
	if (!bOK) {
		if (ulRemaining == 0)
			throw CMWEXCEPTION(COMPAT_ERR_PIN_CODE_BLOCKED);
		else
			throw CMWEXCEPTION(COMPAT_ERR_PIN_CODE_INCORRECT);
	}

	if (BCDDate.Size() != BCDSIZE)
		return false;

	tracefile_data.Append(BCDDate);
	tracefile_data.Append(0x00);
	tracefile_data.Append(0x01); // data = day month year 0 1   -- 6 bytes written to 3F000003 trace file

	WriteFile(TRACEFILE, 0, tracefile_data);

	// Block the Activation PIN so that the operation can be performed only once per card
	// 1000 is always invalid PIN because the valid ones have 6 digits
	if (blockActivationPIN) {
		while (ulRemaining > 0)
			PinCmd(PIN_OP_VERIFY, activationPin, "1000", "", ulRemaining, NULL);
	}

	return true;
}

bool CPteidCard::unlockPIN(const tPin &pin, const tPin *puk, const char *pszPuk, const char *pszNewPin,
						   unsigned long &triesLeft, unsigned long unblockFlags) {
	CByteArray oResp;
	bool bOK = false;
	unsigned long ulRemaining;

	MWLOG(LEV_DEBUG, MOD_CAL, L"PteidCard::unlockPIN called with PUK length=%d unblockFlags=%lu", strlen(pszPuk),
		  unblockFlags);

	try {
		// This implementation is deprecated because there are no more IAS 1.01 cards
		// So we don't even use the new flags param...
		if (m_cardType == CARD_PTEID_IAS101) {
			if (PinCmd(PIN_OP_VERIFY, *puk, pszPuk, "", ulRemaining, NULL))		 // Verify PUK
				bOK = PinCmd(PIN_OP_RESET, pin, pszNewPin, "", triesLeft, NULL); // Reset PIN
		} else if (m_cardType == CARD_PTEID_IAS07 || m_cardType == CARD_PTEID_IAS5) {

			std::string pin_str;
			if (pszNewPin != NULL)
				pin_str = pszNewPin;

			std::string puk_str;
			if (pszPuk != NULL)
				puk_str = pszPuk;

			bOK = PinCmd(strlen(pszPuk) == 8 && (unblockFlags & UNBLOCK_FLAG_PUK_MERGE) == 0 ? PIN_OP_RESET_NO_PUK
																							 : PIN_OP_RESET,
						 pin, puk_str, pin_str, triesLeft, NULL, true, NULL, unblockFlags);
		}
	} catch (CMWException e) {
		MWLOG(LEV_ERROR, MOD_CAL, L"Error in unlockPIN: Specific error code: %08x", e.GetError());
		throw;
	}

	return bOK;
}

DlgPinUsage CPteidCard::PinUsage2Dlg(const tPin &Pin, const tPrivKey *pKey) {
	DlgPinUsage usage = DLG_PIN_UNKNOWN;

	if (Pin.ulID == 1 || Pin.ulPinRef == 0x84)
		usage = DLG_PIN_AUTH;
	else if (Pin.ulID == 2 || Pin.ulPinRef == 0x85)
		usage = DLG_PIN_SIGN;
	else if (Pin.ulID == 3 || Pin.ulPinRef == 0x86)
		usage = DLG_PIN_ADDRESS;
	else
		usage = DLG_PIN_ACTIVATE;

	return usage;
}

#ifdef __linux__

/*
 * Alternative Console PIN UI for Linux systems with no X server
 *
 */
int consoleAskForPin(tPinOperation operation, const tPin &Pin, char *sPin1, char *sPin2) {

	struct termios oflags, nflags;
	char password[64];

	/* disabling echo */
	tcgetattr(fileno(stdin), &oflags);
	nflags = oflags;
	nflags.c_lflag &= ~ECHO;
	nflags.c_lflag |= ECHONL;

	if (tcsetattr(fileno(stdin), TCSANOW, &nflags) != 0) {
		perror("tcsetattr");
		return EXIT_FAILURE;
	}

	printf("Please introduce your %s: ", Pin.csLabel.c_str());
	if (fgets(password, sizeof(password), stdin) == NULL)
		return EXIT_FAILURE;

	// Delete trailing newline
	password[strlen(password) - 1] = 0;

	/* restore terminal */
	if (tcsetattr(fileno(stdin), TCSANOW, &oflags) != 0) {
		perror("tcsetattr");
		return EXIT_FAILURE;
	}

	strncpy(sPin1, password, PIN_MAX_LENGTH);

	if (operation == PIN_OP_CHANGE) {
		memset(password, 0, sizeof(password));
		printf("New PIN: ");
		if (tcsetattr(fileno(stdin), TCSANOW, &nflags) != 0) {
			perror("tcsetattr");
			return EXIT_FAILURE;
		}

		if (fgets(password, sizeof(password), stdin) == NULL)
			return EXIT_FAILURE;
		// Delete trailing newline
		password[strlen(password) - 1] = 0;

		if (tcsetattr(fileno(stdin), TCSANOW, &oflags) != 0) {
			perror("tcsetattr");
			return EXIT_FAILURE;
		}
		strncpy(sPin2, password, PIN_MAX_LENGTH);
	}

	return 0;
}

bool detectWaylandRunning() { return getenv("WAYLAND_DISPLAY") != NULL; }

bool detectXorgRunning() {

	char *display = getenv("DISPLAY");
	char *x_authority = getenv("XAUTHORITY");

	return display && x_authority;
}

#endif

void CPteidCard::showPinDialog(tPinOperation operation, const tPin &Pin, std::string &csPin1, std::string &csPin2,
							   const tPrivKey *pKey, void *wndGeometry) {

	// Convert params
	wchar_t wsPin1[PIN_MAX_LENGTH + 1];
	wchar_t wsPin2[PIN_MAX_LENGTH + 1];

	memset(wsPin1, 0, sizeof(wsPin1));
	memset(wsPin2, 0, sizeof(wsPin1));

	DlgPinOperation pinOperation = PinOperation2Dlg(operation);
	DlgPinUsage usage = PinUsage2Dlg(Pin, pKey);
	DlgPinInfo pinInfo = {Pin.ulMinLen, Pin.ulMaxLen, PIN_FLAG_DIGITS};

	// The actual call
	DlgRet ret;

#ifdef __linux__
	if (!detectXorgRunning() && !detectWaylandRunning()) {
		char sPin1[PIN_MAX_LENGTH + 1];
		char sPin2[PIN_MAX_LENGTH + 1];

		memset(sPin1, 0, sizeof(sPin1));
		memset(sPin2, 0, sizeof(sPin1));
		int rc = consoleAskForPin(operation, Pin, sPin1, sPin2);

		if (rc == 0) {
			csPin1 = std::string(sPin1);
			csPin2 = std::string(sPin2);
		}

		return;
	} else {
#endif
		std::wstring wideLabel = utilStringWiden(Pin.csLabel);
		if (operation == PIN_OP_VERIFY) {
			ret = DlgAskPin(pinOperation, usage, wideLabel.c_str(), pinInfo, wsPin1, PIN_MAX_LENGTH + 1, wndGeometry);
		} else {

			ret = DlgAskPins(pinOperation, usage, wideLabel.c_str(), pinInfo, wsPin1, PIN_MAX_LENGTH + 1, pinInfo,
							 wsPin2, PIN_MAX_LENGTH + 1, wndGeometry);
		}
#ifdef __linux__
	}
#endif

	// Convert back
	if (ret == DLG_OK) {
		csPin1 = utilStringNarrow(wsPin1);
		if (operation != PIN_OP_VERIFY)
			csPin2 = utilStringNarrow(wsPin2);
	} else if (ret == DLG_CANCEL)
		throw CMWEXCEPTION(EIDMW_ERR_PIN_CANCEL);
	else if (ret == DLG_BAD_PARAM)
		throw CMWEXCEPTION(EIDMW_ERR_PARAM_BAD);
	else
		throw CMWEXCEPTION(EIDMW_ERR_UNKNOWN);
}

bool CPteidCard::PinCmd(tPinOperation operation, const tPin &Pin, const std::string &csPin1, const std::string &csPin2,
						unsigned long &ulRemaining, const tPrivKey *pKey, bool bShowDlg, void *wndGeometry,
						unsigned long unblockFlags) {
	bool pincheck;
	tPin pteidPin = Pin;
	// There's a path in the EF(AODF) for the PINs, but it's
	// not necessary, so we can save a Select File command
	pteidPin.csPath = "";
	std::string pin1 = csPin1;

	MWLOG(LEV_DEBUG, MOD_CAL, L"CPteidCard::PinCmd called with operation=%d for pinRef: %lu", (int)operation,
		  Pin.ulPinRef);
	if (m_poContext->m_bSSO) {
		/* Verify with cached PIN is supported for CC2 and address PIN only */
		if (operation == PIN_OP_VERIFY && Pin.ulPinRef == 0x83 && GetType() == CARD_PTEID_IAS5) {
			if (m_verifiedPINs.find(Pin.ulID) != m_verifiedPINs.end()) {
				pin1 = m_verifiedPINs[Pin.ulID];
			}
		}
	}

	pteidPin.encoding = PIN_ENC_ASCII; // PT uses ASCII only for PIN
	if (GetType() == CARD_PTEID_IAS07 || GetType() == CARD_PTEID_IAS5) {
		pincheck =
			CPkiCard::PinCmd(operation, pteidPin, pin1, csPin2, ulRemaining, pKey, bShowDlg, wndGeometry, unblockFlags);
	} else {
		pincheck = CPkiCard::PinCmdIAS(operation, pteidPin, csPin1, csPin2, ulRemaining, pKey, bShowDlg, wndGeometry);
	}

	return pincheck;
}

unsigned long CPteidCard::GetSupportedAlgorithms() {
	unsigned long ulAlgos = 0;

	if (m_cardType == CARD_PTEID_IAS07) {
		std::string applet_version = GetAppletVersion();
		char major_version = applet_version[0] == 'v' ? applet_version[1] : applet_version[0];

		ulAlgos |= SIGN_ALGO_RSA_PKCS | SIGN_ALGO_SHA1_RSA_PKCS | SIGN_ALGO_SHA256_RSA_PKCS;

		// We could assume that future versions also support these but it's best to be conservative about
		// supported algorithms
		if (major_version == '4')
			ulAlgos |= SIGN_ALGO_SHA384_RSA_PKCS | SIGN_ALGO_SHA512_RSA_PKCS | SIGN_ALGO_RSA_PSS;
	} else if (m_cardType == CARD_PTEID_IAS5) {
		ulAlgos |= SIGN_ALGO_ECDSA | SIGN_ALGO_ECDSA_SHA1 | SIGN_ALGO_ECDSA_SHA224 | SIGN_ALGO_ECDSA_SHA256 |
				   SIGN_ALGO_ECDSA_SHA384 | SIGN_ALGO_ECDSA_SHA512;
	}

	return ulAlgos;
}

void CPteidCard::SetSecurityEnv(const tPrivKey &key, unsigned long paddingType, unsigned long ulInputLen) {
	CByteArray oDataias, oDatagem;
	unsigned char ucAlgo = 0x02;
	CByteArray oResp;

	m_ucCLA = 0x00;
	if (m_cardType == CARD_PTEID_IAS5) {
		oDatagem.Append(0x80);
		oDatagem.Append(0x01);
		// Algorithm: ECDSA
		if (ulInputLen == SHA256_LEN)
			ucAlgo = 0x44;
		else if (ulInputLen == SHA384_LEN)
			ucAlgo = 0x54;
		else if (ulInputLen == SHA512_LEN)
			ucAlgo = 0x64;
		else if (ulInputLen == SHA1_LEN)
			ucAlgo = 0x14;
		oDatagem.Append(ucAlgo);
		oDatagem.Append(0x84);
		oDatagem.Append(0x01);
		assert(key.ulKeyRef <= UCHAR_MAX);
		oDatagem.Append((unsigned char)key.ulKeyRef);
		oResp = SendAPDU(0x22, 0x41, 0xB6, oDatagem);

	} else if (m_cardType == CARD_PTEID_IAS07) {
		oDatagem.Append(0x80);
		oDatagem.Append(0x01);
		// Algorithm: RSA with PKCS#1 Padding
		if (ulInputLen == SHA256_LEN)
			ucAlgo = 0x42;
		else if (ulInputLen == SHA384_LEN)
			ucAlgo = 0x52;
		else if (ulInputLen == SHA512_LEN)
			ucAlgo = 0x62;
		else if (ulInputLen == SHA1_LEN)
			ucAlgo = 0x12;

		// Algorithm: RSA with PSS Padding
		if (paddingType == SIGN_ALGO_RSA_PSS) {
			ucAlgo += 3;
		}
		oDatagem.Append(ucAlgo);
		oDatagem.Append(0x84);
		oDatagem.Append(0x01);
		assert(key.ulKeyRef <= UCHAR_MAX);
		oDatagem.Append((unsigned char)key.ulKeyRef);
		oResp = SendAPDU(0x22, 0x41, 0xB6, oDatagem);
	} else {
		// Legacy IAS v1 cards
		oDataias.Append(0x95);
		oDataias.Append(0x01);
		oDataias.Append(0x40);
		oDataias.Append(0x84);
		oDataias.Append(0x01);
		assert(key.ulKeyRef <= UCHAR_MAX);
		oDataias.Append((unsigned char)key.ulKeyRef);
		oDataias.Append(0x80);
		oDataias.Append(0x01);
		oDataias.Append(0x02);

		oResp = SendAPDU(0x22, 0x41, 0xA4, oDataias);
	}

	unsigned long ulSW12 = getSW12(oResp);

	getSW12(oResp, 0x9000);
}

void KeepAliveThread::Run() {
	while (1) {
		CThread::SleepMillisecs(100);
		// If the card was removed stop this thread
		if (!m_poPCSC->Status(m_hCard))
			break;

		if (m_bStopRequest)
			break;
	}

	MWLOG(LEV_DEBUG, MOD_CAL, "Stopping KeepAliveThread");
}

CByteArray CPteidCard::SignInternal(const tPrivKey &key, unsigned long paddingType, const CByteArray &oData,
									const tPin *pPin) {
	CAutoLock autolock(this);
	bool bOK = false;
	m_ucCLA = 0x00;

	MWLOG(LEV_DEBUG, MOD_CAL, L"CPteidCard::SignInternal called with algoID=%04x and data length=%d", paddingType,
		  oData.Size());

	if (m_askPinOnSign) {
		if (pPin != NULL) {
			unsigned long ulRemaining = 0;
			if (m_poContext->m_bSSO) {
				std::string cached_pin = "";
				if (m_verifiedPINs.find(pPin->ulID) != m_verifiedPINs.end()) {
					cached_pin = m_verifiedPINs[pPin->ulID];

					MWLOG(LEV_DEBUG, MOD_CAL, "Using cached pin for %s", pPin->csLabel.c_str());
				}
				bOK = PinCmd(PIN_OP_VERIFY, *pPin, cached_pin, "", ulRemaining, &key);
			} else {
#ifdef WIN32
				// Regularly call SCardStatus()
				MWLOG(LEV_DEBUG, MOD_CAL,
					  L"Starting KeepAliveThread to keep transaction while waiting for user PIN input");
				eIDMW::KeepAliveThread keepAlive(&(m_poContext->m_oPCSC), m_hCard);
				keepAlive.Start();
#endif

				bOK = PinCmd(PIN_OP_VERIFY, *pPin, "", "", ulRemaining, &key);
			}

			if (!bOK)
				throw CMWEXCEPTION(ulRemaining == 0 ? EIDMW_ERR_PIN_BLOCKED : EIDMW_ERR_PIN_BAD);
		}
	}

	SetSecurityEnv(key, paddingType, oData.Size());

	CByteArray oData1;

	oData1.Append(0x90); // SHA-1 Hash as Input
	assert(oData.Size() <= UCHAR_MAX);
	oData1.Append((unsigned char)oData.Size());

	oData1.Append(oData);

	CByteArray oResp, oResp1;

	if (GetType() == CARD_PTEID_IAS07 || GetType() == CARD_PTEID_IAS5) {
		// PSO: Hash GEMSAFE
		oResp1 = SendAPDU(0x2A, 0x90, 0xA0, oData1);

		unsigned long SW12 = getSW12(oResp1);
		if (SW12 != 0x9000) {
			if (SW12 == 0x6985) {
				throw CMWEXCEPTION(EIDMW_ERR_ALGO_BAD);
			} else {
				throw CMWEXCEPTION(m_poContext->m_oPCSC.SW12ToErr(SW12));
			}
		}

		// PSO: Compute Digital Signature GEMSAFE
		oResp = SendAPDU(0x2A, 0x9E, 0x9A, 0x00);

		// 3072 key support: Get the remaining 128 bytes of the signature
		if (oResp.GetByte(oResp.Size() - 2) == 0x61) {
			char remaining = oResp.GetByte(oResp.Size() - 1);
			oResp.Chop(2);
			oResp.Append(SendAPDU(0xC0, 0x00, 0x00, remaining));
		}
	} else {
		// PSO:Hash IAS - does CDS
		oResp = SendAPDU(0x88, 0x02, 0x00, oData);
	}

	unsigned long ulSW12 = getSW12(oResp);
	MWLOG(LEV_INFO, MOD_CAL, L"PSO-Compute digital signature returned: 0x%2X", ulSW12);

	if (ulSW12 != 0x9000)
		throw CMWEXCEPTION(m_poContext->m_oPCSC.SW12ToErr(ulSW12));

	if (GetType() == CARD_PTEID_IAS5) {
		CByteArray rapdu = SendAPDU(0x20, 0xFF, (unsigned char)pPin->ulPinRef, 0x00);
		getSW12(rapdu, 0x9000);
	}

	// Remove SW1-SW2 from the response
	oResp.Chop(2);

	return oResp;
}

bool CPteidCard::ShouldSelectApplet(unsigned char ins, unsigned long ulSW12) {

	if (m_selectAppletMode != TRY_SELECT_APPLET)
		return false;

	if (ins == 0xA4)
		return ulSW12 == 0x6A82 || ulSW12 == 0x6A86;
	return ulSW12 == 0x6A82 || ulSW12 == 0x6A86 || ulSW12 == 0x6D00;
}

bool CPteidCard::SelectApplet() { return PteidCardSelectApplet(m_poContext, m_hCard, getProtocolStructure(), m_pace); }

/**
 * The Pteid card doesn't support select by path (only
 * select by File ID or by AID), so we perform a loop with
 * 'select by file' commands.
 * E.g. if path = AAAABBBCCC the we do
 *   Select(AAAA)
 *   Select(BBBB)
 *   Select(CCCC)
 *
 */
CByteArray CPteidCard::OldSelectByPath(const std::string &csPath, bool bReturnFileInfo) {
	std::string csPathCopy = csPath;
	if (csPath.find("3F00") != std::string::npos || csPath.find("3f00") != std::string::npos)
		csPathCopy.erase(0, 4);

	unsigned long ulOffset = 0;
	// 1. Do a loop of "Select File by file ID" commands
	unsigned long ulPathLen = (unsigned long)csPathCopy.size() / 2;
	for (ulOffset = 0; ulOffset < ulPathLen; ulOffset += 2) {
		CByteArray oPath(ulPathLen);
		oPath.Append(Hex2Byte(csPathCopy, ulOffset));
		oPath.Append(Hex2Byte(csPathCopy, ulOffset + 1));

		CByteArray oResp = SendAPDU(0xA4, 0x00, 0x0C, oPath);
		unsigned long ulSW12 = getSW12(oResp);
		if ((ulSW12 == 0x6A82 || ulSW12 == 0x6A86) && m_selectAppletMode == TRY_SELECT_APPLET) {
			// The file still wasn't found, so let's first try to select the applet
			/*if (SelectApplet())
			{
			m_selectAppletMode = ALW_SELECT_APPLET;*/
			oResp = SendAPDU(0xA4, 0x00, 0x0C, oPath);
			//}
		}
		getSW12(oResp, 0x9000);
	}

	return CByteArray((unsigned char *)csPathCopy.c_str(), (unsigned long)csPathCopy.size());
}

void CPteidCard::ResetApplication() { m_lastSelectedApplication.ClearContents(); }

void CPteidCard::SelectApplication(const CByteArray &oAID) {
	if (m_lastSelectedApplication.Size() > 0 && oAID.Size() > 0 &&
		memcmp(oAID.GetBytes(), m_lastSelectedApplication.GetBytes(), oAID.Size()) == 0) {
		return;
	}

	long lRetVal = 0;
	unsigned char tucSelectApp[] = {0x00, 0xA4, 0x04, 0x00};
	CByteArray oCmd(sizeof(oAID) + 5);
	oCmd.Append(tucSelectApp, sizeof(tucSelectApp));
	oCmd.Append((unsigned char)oAID.Size());
	oCmd.Append(oAID.GetBytes(), oAID.Size());

	CByteArray oResp = SendAPDU(oCmd);

	if (getSW12(oResp) == 0x9000) {
		// If select application was a success, update the state
		m_lastSelectedApplication = oAID;
	}
}
/* Extract file length value from smartcard FCI data:
   There are two possible tags for file size attribute in FCI data
   0x81 for eid applet and 0x80 for national data applet */
int extractEFSize(CByteArray &fci_data) {
	const unsigned char *desc_data = fci_data.GetBytes() + 2;
	// const unsigned char *old_data = desc_data;
	int xclass = 0;
	int asn1Tag = 0;
	long genTag = 0, size = 0;

	long ret = ASN1_get_object(&desc_data, &size, &asn1Tag, &xclass, fci_data.Size() - 2);
	int constructed = ret == V_ASN1_CONSTRUCTED ? 1 : 0;
	genTag = xclass | (constructed & 0b1) << 5 | asn1Tag;
	if (genTag == 0x81 && size == 2) { // eID app
		return (*desc_data << 8) | *(desc_data + 1);
	} else if (genTag == 0x80 && size == 3) { // National data app
		return (*desc_data << 16) | (*(desc_data + 1) << 8) | *(desc_data + 2);
	} else {
		MWLOG(LEV_ERROR, MOD_CAL, "%s: Malformed FCI data, tag found at offset 2: %2x", __FUNCTION__,
			  fci_data.GetByte(2));
		return 0;
	}
}

tFileInfo CPteidCard::SelectFile(const std::string &csPath, const unsigned char *oAID, bool bReturnFileInfo) {
	auto ulPathLen = static_cast<unsigned long>(csPath.size());
	tFileInfo info;
	info.lFileLen = info.lReadPINRef = info.lWritePINRef = 0;
	// path must not contain any incomplete directory or file id
	if (ulPathLen % 4 != 0 || ulPathLen == 0)
		throw CMWEXCEPTION(EIDMW_ERR_BAD_PATH);

	// each byte is 2 characters
	ulPathLen /= 2;
	CByteArray responseSelection;
	CAutoLock autolock(this);
	{
		// Try to select from current application
		responseSelection = SelectByPath(csPath, bReturnFileInfo);

		auto ulSW12 = getSW12(responseSelection);
		if ((ulSW12 >> 0x8) & 0x6A) // Select File any error
		{
			// If failed, try to select the respective application
			SelectApplication({oAID, sizeof(oAID)});

			// Select by path again
			responseSelection = SelectByPath(csPath, bReturnFileInfo);

			// Should be expecting 0x9000 (success)
			getSW12(responseSelection, 0x9000);
		}
		// TODO: return lReadPINRef and lWritePINRef, from security attributes tag 0x8C
		if (bReturnFileInfo) {
			CByteArray fci_data = responseSelection.GetBytes(0, responseSelection.GetByte(1));
			char *fci_data_hex = bin2AsciiHex(responseSelection.GetBytes(), responseSelection.Size());
			MWLOG(LEV_DEBUG, MOD_CAL, "%s: FCI data: %s", __FUNCTION__, fci_data_hex);

			info.lFileLen = extractEFSize(fci_data);
		}
	}

	return info;
}

// Compatible with older CC where only 1 AID present
tFileInfo CPteidCard::SelectFile(const std::string &csPath, bool bReturnFileInfo) {
	return SelectFile(csPath, PTEID_1_APPLET_AID, bReturnFileInfo);
}

CByteArray CPteidCard::SelectByPath(const std::string &csPath, bool bReturnFileInfo) {
	//
	// Old version of CC
	// Only accepts path length of 2-4 bytes (1 file from root directory OR 1 DF and 1 file from DF)
	// Example 1: apdu 00 A4 08 04 02 2f 00 		- selects file 2f00 from root directory 3f00
	// Example 2: apdu 00 A4 08 04 04 5f 00 ef 12 	- selects file ef12 from DF 5f00 from root directory 3f00
	// Note: DF MUST NOT BE ROOT DIRECTORY (3f 00). Operations like (apdu 00 A4 08 04 04 3f 00 2f 00) will return 0x6A
	// Operation not supported
	//
	std::string csPathCopy = csPath;
	bool select_by_path_from_mf = false;
	MWLOG(LEV_DEBUG, MOD_CAL, "%s: csPath: %s", __FUNCTION__, csPath.c_str());
	if (csPath.find("3F00") != std::string::npos || csPath.find("3f00") != std::string::npos) {
		csPathCopy.erase(0, 4);
		select_by_path_from_mf = true;
	}

	unsigned long ulPathLen = (unsigned long)csPathCopy.size() / 2;
	CByteArray oPath(ulPathLen);
	for (unsigned long ulOffset = 0; ulOffset < ulPathLen; ulOffset += 2) {
		oPath.Append(Hex2Byte(csPathCopy, ulOffset));
		oPath.Append(Hex2Byte(csPathCopy, ulOffset + 1));
	}

	//
	// Send APDU and validate response
	//
	CByteArray selectByPathAPDU(6);
	selectByPathAPDU.Append(m_ucCLA);
	selectByPathAPDU.Append(0xA4);
	selectByPathAPDU.Append(oPath.Size() > 2 || select_by_path_from_mf ? 0x08 : 0x02);
	selectByPathAPDU.Append(bReturnFileInfo ? 0x00 : 0x0C);
	assert(oPath.Size() <= UCHAR_MAX);
	selectByPathAPDU.Append((unsigned char)oPath.Size());
	selectByPathAPDU.Append(oPath);
	selectByPathAPDU.Append(0x00);

	auto oResp = SendAPDU(selectByPathAPDU);
	getSW12(oResp, 0x9000);

	return oResp;
}

tCacheInfo CPteidCard::GetCacheInfo(const std::string &csPath) {
	tCacheInfo dontCache = {DONT_CACHE, 0};
	tCacheInfo simpleCache = {SIMPLE_CACHE, 0};
	tCacheInfo certCache = {CERT_CACHE, 0};
	tCacheInfo check16Cache = {CHECK_16_CACHE, 0}; // Check 16 bytes at offset 0
	tCacheInfo checkSerial = {CHECK_SERIAL, 0};	   // Check if the card serial nr is present

	bool cache_enabled =
		CConfig::GetLong(CConfig::EIDMW_CONFIG_PARAM_GENERAL_PTEID_CACHE_ENABLED) && m_cardType == CARD_PTEID_IAS07;
	if (!cache_enabled)
		return dontCache;

	// csPath -> file ID ... FIXME get the right IDs
	unsigned int uiFileID = 0;
	unsigned long ulLen = (unsigned long)(csPath.size() / 2);
	if (ulLen >= 2)
		uiFileID = Hex2Byte(csPath, ulLen - 2) + Hex2Byte(csPath, ulLen - 1);

	switch (uiFileID) {
	case 3:	  // 0003 (TRACE)
	case 47:  // EF(ODF) 4F005031 (Dont cache otherwise will cause issues on IAS cards)
	case 69:  // AOD (4401)
	case 129: // EF(ODF) 4F005031 (ID on OSX Dont cache otherwise will cause issues on IAS cards)
	case 130: // EF(TokenInfo)
	case 244: // EF05 (Address)
	case 245: // EF06 (SOD)
	case 246: // EF07 (PersoData)
	case 251: // EF0C (CertD)
	case 252: // PrkD
		return dontCache;
	case 241: // EF02 (ID)
		return simpleCache;
	case 247: // EF08 (Cert Sign)
	case 248: // EF09 (Cert Auth)
	case 254: // EF0F (Cert Root Sign)
	case 255: // EF10 (Cert Root Auth)
	case 256: // EF11 (CERT ROOT CA)
		return certCache;
	}

	// Should not happen...
	return dontCache;
}

void CPteidCard::InitEncryptionKey() {
	try {
		CByteArray hash;
		if (GetType() == CARD_PTEID_IAS07) {
			unsigned char apduSelectFile[] = {0x00, 0xA4, 0x00, 0x0C, 0x02, 0x5F, 0x00};

			// Select file 5F00
			CByteArray selectFileRes = SendAPDU(CByteArray(apduSelectFile, sizeof(apduSelectFile)));

			// Check select file success
			getSW12(selectFileRes, 0x9000);

			// Get hash from SOD file
			m_ucCLA = 0x00;
			hash = SendAPDU(0xB0, 0x86, 0xD4, 0x10);
			getSW12(hash, 0x9000);
		} else {
			// Get hash from SOD file in national data application
			m_ucCLA = 0x00;
			SelectApplication({PTEID_2_APPLET_NATIONAL_DATA, sizeof(PTEID_2_APPLET_NATIONAL_DATA)});
			hash = SendAPDU(0xB0, 0x9D, 0xD2, 0x10);
			getSW12(hash, 0x9000);
		}

		// Remove 2 last bytes (SW12) and store encryption key
		hash.Chop(2);
		m_oCache.setEncryptionKey(hash);
	} catch (CMWException e) {
	}
}
