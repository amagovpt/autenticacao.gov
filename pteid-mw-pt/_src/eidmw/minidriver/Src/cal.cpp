#include "cal.h"
#include "globmdrv.h"

#include "CardLayer.h"
#include "CardFactory.h"
#include <memory>

using namespace eIDMW;

std::unique_ptr<CCardLayer> oCardLayer;
std::unique_ptr<CReadersInfo> oReadersInfo;
std::string readerName;
DWORD protocol;

int cal_init(const char* reader_name, DWORD protocol_) {
	try {
		oCardLayer = std::make_unique<CCardLayer>();
		oReadersInfo = std::make_unique<CReadersInfo>(oCardLayer->ListReaders());

		readerName = reader_name;
		protocol = protocol_;
	}
	catch (...) {
		printf("E: Error initializing card layer!\n");
		return -1;
	}

	return 1;
}

DWORD cal_read_cert(PCARD_DATA pCardData, DWORD dwCertSpec, DWORD *pcbCertif, PBYTE *ppbCertif) {
	auto &reader = oCardLayer->getReader(readerName);
	try {
		reader.Connect(pCardData->hScard, protocol);
		auto cert = reader.GetCert(dwCertSpec - 1);
		reader.SelectApplication({ PTEID_2_APPLET_EID, sizeof(PTEID_2_APPLET_EID) });

		auto file = reader.ReadFile(cert.csPath);
		*pcbCertif = file.Size();

		*ppbCertif = (PBYTE)pCardData->pfnCspAlloc(*pcbCertif);
		memcpy(*ppbCertif, file.GetBytes(), file.Size());
	}
	catch (CMWException e) {
		printf("%s %d %lx\n",e.GetFile(), e.GetLine(), e.GetError());
		return 0;
	}

	return SCARD_S_SUCCESS;
}

DWORD cal_get_card_sn(PCARD_DATA pCardData, PBYTE pbSerialNumber, DWORD cbSerialNumber, PDWORD pdwSerialNumber) {
	auto &reader = oCardLayer->getReader(readerName);\
	try {
		reader.Connect(pCardData->hScard, protocol);
		reader.SelectApplication({ PTEID_2_APPLET_EID, sizeof(PTEID_2_APPLET_EID) });
		auto serial_number = reader.GetSerialNr();

		if (cbSerialNumber < serial_number.size())
			return ERROR_INSUFFICIENT_BUFFER;

		*pdwSerialNumber = serial_number.size();
		memcpy(pbSerialNumber, serial_number.c_str(), serial_number.size());
	}
	catch (...) {
		printf("E: Error getting card serial number!\n");
		return -1;
	}
	return 0;
}
