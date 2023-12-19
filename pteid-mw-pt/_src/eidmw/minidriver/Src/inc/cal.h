#pragma once

#include "globmdrv.h"

#ifdef __cplusplus
extern "C" {
#endif

	int cal_init(const char* reader_name, DWORD protocol);
	DWORD cal_read_cert(PCARD_DATA pCardData, DWORD dwCertSpec, DWORD *pcbCertif, PBYTE *ppbCertif);

#ifdef __cplusplus
}
#endif