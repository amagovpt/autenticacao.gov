#include "cal.h"
#include "globmdrv.h"

#include "CardLayer.h"
#include "CardFactory.h"
#include "Config.h"
#include <memory>

using namespace eIDMW;

std::unique_ptr<CCardLayer> oCardLayer;
std::string readerName;
DWORD protocol;

long EidmwToScardErr(unsigned long lEidmwErr)
{
	long lRet = SCARD_F_INTERNAL_ERROR;
	switch (lEidmwErr) {
		case EIDMW_ERR_CARD_COMM:
			lRet = SCARD_F_COMM_ERROR; break;
		case EIDMW_ERR_PARAM_RANGE:
			lRet = SCARD_E_INSUFFICIENT_BUFFER; break;
		case EIDMW_ERR_PARAM_BAD:
			lRet = SCARD_E_INVALID_PARAMETER; break;
		case EIDMW_ERR_NO_CARD:
			lRet = SCARD_W_REMOVED_CARD; break;
		case EIDMW_ERR_CMD_NOT_ALLOWED:
			lRet = SCARD_E_NO_ACCESS; break;
		case EIDMW_ERR_CANT_CONNECT:
			lRet = SCARD_W_UNRESPONSIVE_CARD; break;
		case EIDMW_ERR_NO_READER:
			lRet = SCARD_E_NO_SERVICE; break;
		case EIDMW_ERR_CARD_RESET:
			lRet = SCARD_W_RESET_CARD; break;
		case EIDMW_ERR_CARD_SHARING:
			lRet = SCARD_E_SHARING_VIOLATION; break;
		case EIDMW_ERR_ALGO_BAD:
			lRet = SCARD_E_UNSUPPORTED_FEATURE; break;
		case EIDMW_ERR_NOT_TRANSACTED:
			lRet = SCARD_E_NOT_TRANSACTED; break;
	}

	return lRet;
}

std::string get_can(std::string serial_number) {
	std::wstring wsn = std::wstring(serial_number.begin(), serial_number.end());
	std::wstring cache_key = L"can_" + wsn;

	const struct CConfig::Param_Str test = { L"can_cache", cache_key.c_str(), L"" };
	auto can = CConfig::GetString(test);

	return { can.begin(), can.end() };
}

void break_pace() {
	auto &reader = oCardLayer->getReader(readerName);

	// simply send a dummy non secure apdu
	reader.setNextAPDUClearText();
	unsigned char apdu[] = { 0x00, 0xA4, 0x02, 0x00, 0x02, 0xAB, 0xCD };
	reader.SendAPDU({ apdu, sizeof(apdu) });
}

DWORD cal_init(PCARD_DATA pCardData, const char* reader_name, DWORD protocol_) {
	readerName = reader_name;
	protocol = protocol_;

	if (!oCardLayer)
		oCardLayer = std::make_unique<CCardLayer>();
	
	auto &reader = oCardLayer->getReader(readerName);
	try {
		reader.Connect(pCardData->hScard, protocol, false);
		reader.setAskPinOnSign(false);

		break_pace();

		if (reader.isCardContactless()) {
			const auto can = get_can(reader.GetSerialNr());
			if (can.empty())
				return SCARD_F_INTERNAL_ERROR;

			reader.initPaceAuthentication(can.c_str(), can.size(), PaceSecretType::PACECAN);
		}
	}
	catch (CMWException& e) {
		return EidmwToScardErr(e.GetError());
	}

	return SCARD_S_SUCCESS;
}

DWORD cal_read_cert(PCARD_DATA pCardData, DWORD dwCertSpec, DWORD *pcbCertif, PBYTE *ppbCertif) {
	auto &reader = oCardLayer->getReader(readerName);
	try {
		reader.UseHandle(pCardData->hScard);

		const auto cert = reader.GetCert(dwCertSpec - 1);
		if (reader.GetCardType() == CARD_PTEID_IAS5)
			reader.SelectApplication({ PTEID_2_APPLET_EID, sizeof(PTEID_2_APPLET_EID) });

		auto file = reader.ReadFile(cert.csPath);
		*pcbCertif = file.Size();

		*ppbCertif = static_cast<PBYTE>(pCardData->pfnCspAlloc(*pcbCertif));
		memcpy(*ppbCertif, file.GetBytes(), file.Size());
	}
	catch (CMWException& e) {
		return EidmwToScardErr(e.GetError());
	}

	return SCARD_S_SUCCESS;
}

DWORD cal_get_card_sn(PCARD_DATA pCardData, PBYTE pbSerialNumber, DWORD cbSerialNumber, PDWORD pdwSerialNumber) {
	auto &reader = oCardLayer->getReader(readerName);
	try {
		reader.UseHandle(pCardData->hScard);
		
		DWORD len = 0;
		const auto vendor = static_cast<VENDOR_SPECIFIC*>(pCardData->pvVendorSpecific);

		if (!vendor->bSerialNumberSet) {
			CByteArray serialNumber = reader.GetSerialNrBytes();

			if (serialNumber.Size() > 16)
				return ERROR_INSUFFICIENT_BUFFER;
		
			memcpy(vendor->szSerialNumber, serialNumber.GetBytes(), serialNumber.Size());
			len = serialNumber.Size();
			vendor->bSerialNumberSet = true;
		}

		*pdwSerialNumber = len;
		memcpy(pbSerialNumber, vendor->szSerialNumber, len);
	}
	catch (CMWException& e) {
		return EidmwToScardErr(e.GetError());
	}

	return SCARD_S_SUCCESS;
}

DWORD cal_read_pub_key(PCARD_DATA pCardData, DWORD dwCertSpec, DWORD *pcbPubKey, PBYTE *ppbPubKey) {
	auto &reader = oCardLayer->getReader(readerName);
	try {
		reader.UseHandle(pCardData->hScard);
		if (reader.GetCardType() == CARD_PTEID_IAS5)
			reader.SelectApplication({ PTEID_2_APPLET_EID, sizeof(PTEID_2_APPLET_EID) });	

		unsigned char cmd[16] = { 0x00, 0xCB, 0x00, 0xFF, 0x0A, 0xB6, 0x03, 0x83, 0x01, 0x08, 0x7F, 0x49, 0x02, 0x86, 0x00, 0x00 };
		constexpr int                   PUBKEY_LEN = 64;
		constexpr int                   PUBKEY_OFFSET = 11;

		if (dwCertSpec == 2)
			cmd[9] = 0x08;
		else
			cmd[9] = 0x06;

		CByteArray result_buff = reader.SendAPDU({cmd, sizeof(cmd)});
		if (result_buff.Size() == 0)
			return SCARD_E_UNEXPECTED;

		*pcbPubKey = sizeof(BCRYPT_ECCKEY_BLOB) + PUBKEY_LEN;
		*ppbPubKey = static_cast<PBYTE>(pCardData->pfnCspAlloc(*pcbPubKey));
		reinterpret_cast<BCRYPT_ECCKEY_BLOB*>(*ppbPubKey)->dwMagic = BCRYPT_ECDSA_PUBLIC_P256_MAGIC;
		//It seems the Microsoft documentation is wrong on this cbKey field for EC keys: "The length, in bytes, of the key."
		reinterpret_cast<BCRYPT_ECCKEY_BLOB*>(*ppbPubKey)->cbKey = PUBKEY_LEN / 2;

		memcpy((*ppbPubKey) + sizeof(BCRYPT_ECCKEY_BLOB), result_buff.GetBytes() + PUBKEY_OFFSET, PUBKEY_LEN);

	}
	catch (CMWException& e) {
		return EidmwToScardErr(e.GetError());
	}

	return SCARD_S_SUCCESS;
}

DWORD cal_auth_pin(PCARD_DATA pCardData, PBYTE pbPin, DWORD cbPin, PDWORD pcAttemptsRemaining, BYTE pin_id) {
	auto &reader = oCardLayer->getReader(readerName);
	try {
		reader.UseHandle(pCardData->hScard);

		// Reset pace authentication if contactless
		if (reader.isCardContactless()) {
			const auto can = get_can(reader.GetSerialNr());
			if (can.empty())
				return SCARD_F_INTERNAL_ERROR;

			reader.initPaceAuthentication(can.c_str(), can.size(), PaceSecretType::PACECAN);
		}

		if (card_type == IAS_V5_CARD) {
			reader.SelectApplication({ PTEID_2_APPLET_EID, sizeof(PTEID_2_APPLET_EID) });
		}
		else {
			reader.SelectApplication({ PTEID_1_APPLET_AID, sizeof(PTEID_1_APPLET_AID) });
		}

		const tPin tpin = reader.GetPin(pin_id);

		const auto pin = std::string(reinterpret_cast<const char*>(pbPin), (size_t)cbPin);
		unsigned long ulRemaining = 0;
		const auto res = reader.PinCmd(PIN_OP_VERIFY, tpin, pin, "", ulRemaining);

		if (ulRemaining == 0)
			return SCARD_W_CHV_BLOCKED;

		if (!res)
			return SCARD_W_WRONG_CHV;
	}
	catch (CMWException& e) {
		return EidmwToScardErr(e.GetError());
	}

	return SCARD_S_SUCCESS;
}

DWORD cal_sign_data(PCARD_DATA pCardData, BYTE container_id, DWORD cbToBeSigned, PBYTE pbToBeSigned, DWORD *pcbSignature, PBYTE *ppbSignature, BOOL pss_padding) {
	auto &reader = oCardLayer->getReader(readerName);
	try {
		reader.UseHandle(pCardData->hScard);

		const auto pkey = reader.GetPrivKey(container_id);
		auto signed_data = reader.Sign(pkey, pss_padding ? SIGN_ALGO_RSA_PSS : 0, { pbToBeSigned, cbToBeSigned });

		*pcbSignature = signed_data.Size();
		*ppbSignature = static_cast<PBYTE>(pCardData->pfnCspAlloc(signed_data.Size()));

		if (reader.GetCardType() == CARD_PTEID_IAS5) {
			memcpy(*ppbSignature, signed_data.GetBytes(), signed_data.Size());
		} else {
			for (unsigned int i = 0; i < *pcbSignature; i++)
			{
				(*ppbSignature)[i] = signed_data.GetByte(*pcbSignature - i - 1);
			}
		}

	}
	catch (CMWException e) {
		return EidmwToScardErr(e.GetError());
	}

	return SCARD_S_SUCCESS;
}