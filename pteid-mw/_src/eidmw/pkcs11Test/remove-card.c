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
 * Tests the C_GetSlotList in combination with removal of the eID card.
 * Required interaction: insert/remove the eID card very fast.
 */

#include <stdio.h>
#include <opensc/pkcs11.h>
#include <dlfcn.h>
#include <unistd.h>
#include <stdlib.h>

#define PKCS11_LIB "/usr/local/lib/libpteidpkcs11.so" 
//#define PKCS11_LIB "/home/fcorneli/pteid-2.6.0/src/newpkcs11/src/pkcs11/libpteidpkcs11.so.2.1.0" 

int main() {
    	void *handle;
    	CK_C_GetFunctionList pC_GetFunctionList;
    	CK_RV rv;
    	CK_FUNCTION_LIST_PTR functions;
	CK_INFO info;
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
	while (1) {
	   	long slot_count;
    		CK_SLOT_ID_PTR slotIds;
		int slotIdx;
		// C_GetSlotList
	    	rv = (*functions->C_GetSlotList) (CK_FALSE, NULL_PTR, &slot_count);
	    	if (CKR_OK != rv) {
	    		fprintf(stderr, "C_GetSlotList error\n");
	    		goto finalize;
	    	}
	    	slotIds = malloc(slot_count * sizeof(CK_SLOT_ID));
	    	rv = (*functions->C_GetSlotList) (CK_FALSE, slotIds, &slot_count);
	    	if (CKR_OK != rv) {
	    		fprintf(stderr, "C_GetSlotList (2) error\n");
	    		goto finalize;
	    	}
		for (slotIdx = 0; slotIdx < slot_count; slotIdx++) {
			CK_SLOT_INFO slotInfo;		
			CK_SLOT_ID slotId = slotIds[slotIdx];
			rv = (*functions->C_GetSlotInfo) (slotId, &slotInfo);
			if (CKR_OK != rv) {
				fprintf(stderr, "C_GetSlotInfo error\n");
				goto finalize;		
			}
			if (slotInfo.flags & CKF_TOKEN_PRESENT) {
				printf("P");			
			} else {
				printf("R");
			}
		}
		free(slotIds);
	}
	// C_Finalize
finalize:
	rv = (*functions->C_Finalize) (NULL_PTR);
	if (CKR_OK != rv) {
		fprintf(stderr, "C_Finalize error\n");
		exit(1);
	}
    	dlclose(handle);
}

