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
		if (!oCardLayer)
		oCardLayer = std::make_unique<CCardLayer>();

		readerName = reader_name;
		protocol = protocol_;

		auto &reader = oCardLayer->getReader(readerName);
		reader.Connect(pCardData->hScard, protocol);
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

		reader.SelectApplication({ PTEID_2_APPLET_EID, sizeof(PTEID_2_APPLET_EID) });
		auto serial_number = reader.GetSerialNr();

		if (cbSerialNumber < serial_number.size())
			return ERROR_INSUFFICIENT_BUFFER;

		*pdwSerialNumber = serial_number.size();
		memcpy(pbSerialNumber, serial_number.c_str(), serial_number.size());
	}
	catch (CMWException e) {
		return EidmwToScardErr(e.GetError());
	}

	return SCARD_S_SUCCESS;
}
		return -1;
	}
	return 0;
}
