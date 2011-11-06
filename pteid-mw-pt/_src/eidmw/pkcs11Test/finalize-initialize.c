/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2009-2010 FedICT.
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
/*
 * Integration test for the PKCS#11 library.
 * Tests the finalize/initialize sequence in multiple threads
 * Required interaction: none.
 */

int readslots(CK_FUNCTION_LIST_PTR functions) {

	CK_RV frv;
	CK_C_INITIALIZE_ARGS init_args;
	long slot_count;
	CK_SLOT_ID_PTR slotIds;
	int slotIdx;
	CK_INFO info;
	CK_SESSION_HANDLE session_handle;

	testlog(LVL_INFO,"readslots enter\n");
	init_args.flags = CKF_OS_LOCKING_OK;
	init_args.pReserved = NULL;

	// C_Initialize
	frv = (*functions->C_Initialize) ( (CK_VOID_PTR)&init_args );
	if (CKR_OK != frv) {
	    testlog(LVL_ERROR, "C_Initialize error\n");
	    testlog(LVL_INFO,"readslots leave\n");
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
		testlog(LVL_ERROR, "C_GetSlotList error\n");
		goto finalize;
	}
	testlog(LVL_DEBUG,"slot count: %i\n", slot_count);
	slotIds = malloc(slot_count * sizeof(CK_SLOT_INFO));
	frv = (*functions->C_GetSlotList) (CK_FALSE, slotIds, &slot_count);
	if (CKR_OK != frv) {
		testlog(LVL_ERROR, "C_GetSlotList (2) error\n");
		goto finalize;
	}

	for (slotIdx = 0; slotIdx < slot_count; slotIdx++) {
		CK_SLOT_INFO slotInfo;		
		CK_SLOT_ID slotId = slotIds[slotIdx];
		int idx;
		frv = (*functions->C_GetSlotInfo) (slotId, &slotInfo);
		if (CKR_OK != frv) {
			testlog(LVL_ERROR, "C_GetSlotInfo error\n");
			goto finalize;		
		}
		for (idx = 64 - 1; idx > 0; idx--) {
			if (slotInfo.slotDescription[idx] == ' ') {
				slotInfo.slotDescription[idx] = '\0';			
			} else {
				break;
			}		
		}
		testlog(LVL_DEBUG,"slot Id: %d\n", slotId);
		testlog(LVL_DEBUG,"slot description: %s\n", slotInfo.slotDescription);
		// C_OpenSession
			frv = (*functions->C_OpenSession)(slotId, CKF_SERIAL_SESSION, NULL_PTR, NULL_PTR, &session_handle);
			if (CKR_OK != frv) {
				testlog(LVL_ERROR, "C_OpenSession error\n");
				goto finalize;
			}
		// C_CloseSession
		frv = (*functions->C_CloseSession) (session_handle);
		if (CKR_OK != frv) {
			testlog(LVL_ERROR, "C_CloseSession error\n");
			goto finalize;
		}
	}//end of for

	// C_Finalize
finalize:
	frv = (*functions->C_Finalize) (NULL_PTR);
	if (CKR_OK != frv) {
		testlog(LVL_ERROR, "C_Finalize error\n");
		exit(1);
	}
	testlog(LVL_INFO,"readslots leave\n");
	return 0;
}

typedef struct test_finalize_initialize_threadvars
{
	int threadRetVal;
	CK_FUNCTION_LIST_PTR functions;
} *LP_TEST_FIN_INI_VARS;

DWORD WINAPI pkcs11Thread( LPVOID testThreadVars ) 
{ 
	LP_TEST_FIN_INI_VARS threadVars = (LP_TEST_FIN_INI_VARS)testThreadVars;
	CK_FUNCTION_LIST_PTR functions;

    functions = threadVars->functions;

	threadVars->threadRetVal = readslots(functions);

    return 0; 
} 

CK_RV test_finalize_initialize() {
	void *handle;
	CK_FUNCTION_LIST_PTR functions;
    DWORD   dwThreadId;
	HANDLE  hThreadHandle; 
	int retVal = 0;
	struct test_finalize_initialize_threadvars threadVars;
	
	testlog(LVL_INFO, "test_finalize_initialize enter\n");
	handle = dlopen(PKCS11_LIB, RTLD_LAZY); // RTLD_NOW is slower
	if (NULL == handle) {
	    testlog(LVL_ERROR, "dlopen error\n");
 		testlog(LVL_INFO, "test_finalize_initialize leave\n");
		return 1;
	}
	GetPKCS11FunctionList(&functions, handle);

	readslots(functions);

	threadVars.functions = functions;
	threadVars.threadRetVal = 0;

	// Create pkcs11 thread
    hThreadHandle = CreateThread( 
        NULL,                   // default security attributes
        0,                      // use default stack size  
        pkcs11Thread,			// thread function name
        &threadVars,				// argument to thread function 
        0,                      // use default creation flags 
        &dwThreadId);			// returns the thread identifier 

	if (hThreadHandle == NULL)
	{
		retVal = -1;
	}
	else
	{
		// Wait until pkcs11 thread is terminated.
		if (WAIT_OBJECT_0 != WaitForSingleObject(hThreadHandle, INFINITE))
		{
			testlog(LVL_ERROR, "WaitForSingleObject failed\n");
		}	
	}
	dlclose(handle);

	testlog(LVL_DEBUG, "second thread returned %d\n",threadVars.threadRetVal);
	testlog(LVL_INFO, "test_finalize_initialize leave\n");

	return retVal;
}



