#pragma once

#include "globmdrv.h"

#ifdef __cplusplus
extern "C" {
#endif

	DWORD cal_init(PCARD_DATA pCardData, const char* reader_name, DWORD protocol);
	DWORD cal_read_cert(PCARD_DATA pCardData, DWORD dwCertSpec, DWORD *pcbCertif, PBYTE *ppbCertif);
	DWORD cal_get_card_sn(PCARD_DATA pCardData, PBYTE pbSerialNumber, DWORD cbSerialNumber, PDWORD pdwSerialNumber);
	DWORD cal_read_pub_key(PCARD_DATA pCardData, DWORD dwCertSpec, DWORD *pcbPubKey, PBYTE *ppbPubKey);

#ifdef __cplusplus
}
#endif