/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2009 FedICT.
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

/*
 * Integration test for the PKCS#11 library.
 * Tests the opening and closing of a PKCS#11 session.
 * Required interaction: none.
 */

#include <stdio.h>
#ifdef WIN32
//allign at 1 byte
#pragma pack(push, cryptoki, 1)
#include <win32.h>
#include <pkcs11.h>
#pragma pack(pop, cryptoki)
//back to default allignment

#include <windows.h>
#include <conio.h>

#define dlopen(lib,h) LoadLibrary(lib)
#define dlsym(h, function) GetProcAddress(h, function)
#define dlclose(h) FreeLibrary(h)
#define PKCS11_LIB "..\\_Binaries35\\Debug\\pteid35pkcs11D.dll"
#define RTLD_LAZY	1
#define RTLD_NOW	2
#define RTLD_GLOBAL 4

#else
#include <opensc/pkcs11.h>
#include <dlfcn.h>
#include <unistd.h>
#define PKCS11_LIB "/usr/local/lib/libpteidpkcs11.so" 
#endif
#include <stdlib.h>

int main() {
    	void *handle;
    	CK_C_GetFunctionList pC_GetFunctionList;
    	CK_RV rv;
    	CK_FUNCTION_LIST_PTR functions;
		CK_INFO info;
    	CK_SESSION_HANDLE session_handle;
    	CK_SLOT_ID_PTR slot_list;
    	long slot_count;
    	CK_SLOT_ID_PTR slotIds;
	int slotIdx;
    	printf("PKCS11 test\n");

    	handle = dlopen(PKCS11_LIB, RTLD_LAZY); // RTLD_NOW is slower
    	if (NULL == handle) {
    	    fprintf(stderr, "dlopen error\n");
     	   exit(1);    
    	}
    	// get function pointer to C_GetFunctionList
    	pC_GetFunctionList = (CK_C_GetFunctionList) dlsym(handle, "C_GetFunctionList");
    	if (pC_GetFunctionList == NULL) {
    	    dlclose(handle);
    	    printf("failure\n");
    	    exit(1);
    	}

    	// invoke C_GetFunctionList
    	rv = (*pC_GetFunctionList) (&functions);
    	if (rv != CKR_OK) {
    	    fprintf(stderr, "C_GetFunctionList failed\n");
    	    exit(1);    
    	}

    	// C_Initialize
    	rv = (*functions->C_Initialize) (NULL);
    	if (CKR_OK != rv) {
    	    fprintf(stderr, "C_Initialize error\n");
    	    exit(1);    
    	}
	// C_GetInfo
	rv = (*functions->C_GetInfo) (&info);
	if (CKR_OK != rv) {
		fprintf(stderr, "C_GetInfo error\n");
		goto finalize;
	}
	printf("library version: %d.%d\n", info.libraryVersion.major, info.libraryVersion.minor);
	printf("PKCS#11 version: %d.%d\n", info.cryptokiVersion.major, info.cryptokiVersion.minor);

//repeat the slot and token finding sequal
	while(1)
	{

	// C_GetSlotList
    	rv = (*functions->C_GetSlotList) (0, 0, &slot_count);
    	if (CKR_OK != rv) {
    		fprintf(stderr, "C_GetSlotList error\n");
    		goto finalize;
    	}
    	printf("slot count: %i\n", slot_count);
    	slotIds = malloc(slot_count * sizeof(CK_SLOT_INFO));
    	rv = (*functions->C_GetSlotList) (CK_FALSE, slotIds, &slot_count);
    	if (CKR_OK != rv) {
    		fprintf(stderr, "C_GetSlotList (2) error\n");
    		goto finalize;
    	}
		for (slotIdx = 0; slotIdx < slot_count; slotIdx++) {
			CK_SLOT_INFO slotInfo;		
			CK_SLOT_ID slotId = slotIds[slotIdx];
			int idx;
			rv = (*functions->C_GetSlotInfo) (slotId, &slotInfo);
			if (CKR_OK != rv) {
				fprintf(stderr, "C_GetSlotInfo error\n");
				goto finalize;		
			}
			for (idx = 64 - 1; idx > 0; idx--) {
				if (slotInfo.slotDescription[idx] == ' ') {
					slotInfo.slotDescription[idx] = '\0';			
				} else {
					break;
				}		
			}
			printf("slot Id: %d\n", slotId);
			printf("slot description: %s\n", slotInfo.slotDescription);
			// C_OpenSession
    			rv = (*functions->C_OpenSession)(slotId, CKF_SERIAL_SESSION, NULL_PTR, NULL_PTR, &session_handle);
    			if (CKR_OK != rv) {
    				fprintf(stderr, "C_OpenSession error\n");
    				goto finalize;
    			}
			// C_CloseSession
			rv = (*functions->C_CloseSession) (session_handle);
			if (CKR_OK != rv) {
				fprintf(stderr, "C_CloseSession error\n");
				goto finalize;
			}
		}//end of for
		Sleep(2000);
	}//end of while

	// C_Finalize
finalize:
	rv = (*functions->C_Finalize) (NULL_PTR);
	if (CKR_OK != rv) {
		fprintf(stderr, "C_Finalize error\n");
		exit(1);
	}
    dlclose(handle);
}



