#include "cal.h"
#include "globmdrv.h"

#include "CardLayer.h"
#include "CardFactory.h"
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
		case EIDMW_ERR_NOT_TRANSACTED:
			lRet = SCARD_E_NOT_TRANSACTED; break;
	}

	return lRet;
}

DWORD cal_init(PCARD_DATA pCardData, const char* reader_name, DWORD protocol_) {
	try {
		readerName = reader_name;
		protocol = protocol_;
		
		if (!oCardLayer)
			oCardLayer = std::make_unique<CCardLayer>();

		auto &reader = oCardLayer->getReader(readerName);
		reader.Connect(pCardData->hScard, protocol);
		reader.setAskPinOnSign(false);

		if (reader.isCardContactless()) {
			const char* CAN = "460354";
			reader.initPaceAuthentication(CAN, 6, PaceSecretType::PACECAN);
		}
	}
	catch (CMWException e) {
		return EidmwToScardErr(e.GetError());
	}

	return SCARD_S_SUCCESS;
}

DWORD cal_read_cert(PCARD_DATA pCardData, DWORD dwCertSpec, DWORD *pcbCertif, PBYTE *ppbCertif) {
	auto &reader = oCardLayer->getReader(readerName);
	try {
		reader.UseHandle(pCardData->hScard);

		auto cert = reader.GetCert(dwCertSpec - 1);
		if (reader.GetCardType() == CARD_PTEID_IAS5)
			reader.SelectApplication({ PTEID_2_APPLET_EID, sizeof(PTEID_2_APPLET_EID) });

		auto file = reader.ReadFile(cert.csPath);
		*pcbCertif = file.Size();

		*ppbCertif = (PBYTE)pCardData->pfnCspAlloc(*pcbCertif);
		memcpy(*ppbCertif, file.GetBytes(), file.Size());
	}
	catch (CMWException e) {
		return EidmwToScardErr(e.GetError());
	}

	return SCARD_S_SUCCESS;
}

DWORD cal_get_card_sn(PCARD_DATA pCardData, PBYTE pbSerialNumber, DWORD cbSerialNumber, PDWORD pdwSerialNumber) {
	auto &reader = oCardLayer->getReader(readerName);
	try {
		reader.UseHandle(pCardData->hScard);
		
		size_t len = 0;
		auto vendor = reinterpret_cast<VENDOR_SPECIFIC*>(pCardData->pvVendorSpecific);

		if (!vendor->bSerialNumberSet) {
			CByteArray serialNumber = reader.GetSerialNrBytes();

			if (serialNumber.Size() > 16)
				return ERROR_INSUFFICIENT_BUFFER;
		
			memcpy(vendor->szSerialNumber, serialNumber.GetBytes(), serialNumber.Size());
			len = serialNumber.Size();
		}

		*pdwSerialNumber = len;
		memcpy(pbSerialNumber, vendor->szSerialNumber, len);
	}
	catch (CMWException e) {
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
		const int                   PUBKEY_LEN = 64;
		const int                   PUBKEY_OFFSET = 11;
		long len;
		CByteArray result_buff;

		if (dwCertSpec == 2)
			cmd[9] = 0x08;
		else
			cmd[9] = 0x06;

		result_buff = reader.SendAPDU({ cmd, sizeof(cmd) });
		if (result_buff.Size() == 0)
			return SCARD_E_UNEXPECTED;

		*pcbPubKey = sizeof(BCRYPT_ECCKEY_BLOB) + PUBKEY_LEN;
		*ppbPubKey = (PBYTE)pCardData->pfnCspAlloc(*pcbPubKey);
		((BCRYPT_ECCKEY_BLOB*)(*ppbPubKey))->dwMagic = BCRYPT_ECDSA_PUBLIC_P256_MAGIC;
		//It seems the Microsoft documentation is wrong on this cbKey field for EC keys: "The length, in bytes, of the key."
		((BCRYPT_ECCKEY_BLOB*)(*ppbPubKey))->cbKey = PUBKEY_LEN / 2;

		memcpy((*ppbPubKey) + sizeof(BCRYPT_ECCKEY_BLOB), result_buff.GetBytes() + PUBKEY_OFFSET, PUBKEY_LEN);

	}
	catch (CMWException e) {
		return EidmwToScardErr(e.GetError());
	}

	return SCARD_S_SUCCESS;
}

DWORD cal_auth_pin(PCARD_DATA pCardData, PBYTE pbPin, DWORD cbPin, PDWORD pcAttemptsRemaining, BYTE pin_id) {
	auto &reader = oCardLayer->getReader(readerName);
	try {
		reader.UseHandle(pCardData->hScard);

		if (card_type == IAS_V5_CARD) {
			reader.SelectApplication({ PTEID_2_APPLET_EID, sizeof(PTEID_2_APPLET_EID) });
		}
		else {
			reader.SelectApplication({ PTEID_1_APPLET_AID, sizeof(PTEID_1_APPLET_AID) });
		}

		tPin tpin = reader.GetPin(pin_id);

		std::string pin = std::string((const char*)pbPin, (size_t)cbPin);
		unsigned long ulRemaining = 0;
		auto res = reader.PinCmd(PIN_OP_VERIFY, tpin, pin, "", ulRemaining);

		if (ulRemaining == 0)
			return SCARD_W_CHV_BLOCKED;

		if (!res)
			return SCARD_W_WRONG_CHV;
	}
	catch (CMWException e) {
		return EidmwToScardErr(e.GetError());
	}

	return SCARD_S_SUCCESS;
}

DWORD cal_sign_data(PCARD_DATA pCardData, BYTE pin_id, DWORD cbToBeSigned, PBYTE pbToBeSigned, DWORD *pcbSignature, PBYTE *ppbSignature, BOOL pss_padding) {
	auto &reader = oCardLayer->getReader(readerName);
	try {
		reader.UseHandle(pCardData->hScard);

		auto pkey = reader.GetPrivKey(pin_id);
		auto signed_data = reader.Sign(pkey, pss_padding ? SIGN_ALGO_RSA_PSS : 0, { pbToBeSigned, cbToBeSigned });

		*pcbSignature = signed_data.Size();
		*ppbSignature = (PBYTE)pCardData->pfnCspAlloc(signed_data.Size());

		if (reader.GetCardType() == CARD_PTEID_IAS5) {
			memcpy(*ppbSignature, signed_data.GetBytes(), signed_data.Size());
		} else {
			for (int i = 0; i < *pcbSignature; i++)
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