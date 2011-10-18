/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2010 FedICT.
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
#include "basetest.h"
#include "logtest.h"

CK_RV test_open_close_session() {
	void *handle;
	CK_FUNCTION_LIST_PTR functions;
	int retVal = 0;
	CK_RV frv;
	CK_INFO info;
	CK_SESSION_HANDLE session_handle;
	long slot_count;
	CK_SLOT_ID_PTR slotIds;
	int slotIdx;

	testlog(LVL_INFO, "test_open_close_session enter\n");
	handle = dlopen(PKCS11_LIB, RTLD_LAZY); // RTLD_NOW is slower
	if (NULL == handle) {
	    testlog(LVL_ERROR, "dlopen error\n");
 		testlog(LVL_INFO, "test_finalize_initialize leave\n");
		return 1;
	}
	GetPKCS11FunctionList(&functions, handle);


	// C_Initialize
	frv = (*functions->C_Initialize) (NULL);
	if (CKR_OK != frv) {
	    testlog(LVL_ERROR, "C_Initialize error\n");
	    return 1;  
	}
	// C_GetInfo
	frv = (*functions->C_GetInfo) (&info);
	if (CKR_OK != frv) {
		testlog(LVL_ERROR, "C_GetInfo error\n");
		goto finalize;
	}
	testlog(LVL_INFO,"library version: %d.%d\n", info.libraryVersion.major, info.libraryVersion.minor);
	testlog(LVL_INFO,"PKCS#11 version: %d.%d\n", info.cryptokiVersion.major, info.cryptokiVersion.minor);

	// C_GetSlotList
	frv = (*functions->C_GetSlotList) (0, 0, &slot_count);
	if (CKR_OK != frv) {
		testlog(LVL_ERROR,"C_GetSlotList error\n");
		goto finalize;
	}
	testlog(LVL_INFO,"slot count: %i\n", slot_count);
	slotIds = malloc(slot_count * sizeof(CK_SLOT_INFO));
	frv = (*functions->C_GetSlotList) (CK_FALSE, slotIds, &slot_count);
	if (CKR_OK != frv) {
		testlog(LVL_ERROR,"C_GetSlotList (2) error\n");
		goto finalize;
	}
	for (slotIdx = 0; slotIdx < slot_count; slotIdx++) {
		CK_SLOT_INFO slotInfo;		
		CK_SLOT_ID slotId = slotIds[slotIdx];
		int idx;
		frv = (*functions->C_GetSlotInfo) (slotId, &slotInfo);
		if (CKR_OK != frv) {
			testlog(LVL_ERROR,"C_GetSlotInfo error\n");
			goto finalize;		
		}
		for (idx = 64 - 1; idx > 0; idx--) {
			if (slotInfo.slotDescription[idx] == ' ') {
				slotInfo.slotDescription[idx] = '\0';			
			} else {
				break;
			}		
		}
		testlog(LVL_INFO,"slot Id: %d\n", slotId);
		testlog(LVL_INFO,"slot description: %s\n", slotInfo.slotDescription);

		// C_OpenSession
		frv = (*functions->C_OpenSession)(slotId, CKF_SERIAL_SESSION, NULL_PTR, NULL_PTR, &session_handle);
		if (CKR_OK != frv) {
		    testlog(LVL_ERROR,"C_OpenSession error\n");
		    goto finalize;
		}

		// C_CloseSession
		frv = (*functions->C_CloseSession) (session_handle);
		if (CKR_OK != frv) {
			testlog(LVL_ERROR,"C_CloseSession error\n");
			goto finalize;
		}
	}
	// C_Finalize
finalize:
	frv = (*functions->C_Finalize) (NULL_PTR);
	if (CKR_OK != frv) {
		testlog(LVL_ERROR,"C_Finalize error\n");
		return 1;
	}
    dlclose(handle);
	return 0;
}

