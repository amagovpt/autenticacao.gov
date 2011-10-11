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

CK_RV test_sign() {
	void *handle;
	CK_FUNCTION_LIST_PTR functions;
	int retVal = 0;
	CK_RV frv;
	CK_RV trv = CKR_OK;
	CK_SESSION_HANDLE session_handle;
	long slot_count;
	CK_SLOT_ID_PTR slotIds;
	int slotIdx;
	CK_ULONG         ulObjectCount;

	testlog(LVL_INFO, "test_sign enter\n");
	if (!InitializeTest(&handle,&functions))
	{
		testlog(LVL_INFO, "test_verify_signature leave\n");
		return CKR_GENERAL_ERROR;
	}
	frv = (*functions->C_Initialize) (NULL);
	if (ReturnedSuccesfull(frv,&trv, "C_Initialize", "test_sign" ))
	{		
		frv = (*functions->C_GetSlotList) (0, 0, &slot_count);
		if (ReturnedSuccesfull(frv,&trv, "C_GetSlotList", "test_sign" ))
		{
			testlog(LVL_INFO,"slot count: %i\n", slot_count);
			slotIds = malloc(slot_count * sizeof(CK_SLOT_INFO));
			frv = (*functions->C_GetSlotList) (CK_FALSE, slotIds, &slot_count);
			if (ReturnedSuccesfull(frv,&trv, "C_GetSlotList (X2)", "test_sign" ))
			{
				for (slotIdx = 0; slotIdx < slot_count; slotIdx++) 
				{
					CK_SLOT_ID slotId = slotIds[slotIdx];
					CK_ULONG private_key = CKO_PRIVATE_KEY;
					CK_ATTRIBUTE attributes[2] = {	{CKA_CLASS,&private_key,sizeof(CK_ULONG)},
					{CKA_LABEL,"Signature",(CK_ULONG) strlen("Signature")}};
					CK_OBJECT_HANDLE hKey;
					CK_MECHANISM mechanism = {CKM_RSA_PKCS, NULL_PTR, 0};
					CK_BYTE_PTR data = "testsignthis"; 
					CK_BYTE signature[256];
					CK_ULONG signLength = 256;

					frv = (*functions->C_OpenSession)(slotId, CKF_SERIAL_SESSION, NULL_PTR, NULL_PTR, &session_handle);
					if (ReturnedSuccesfull(frv,&trv, "C_OpenSession", "test_sign" ))
					{
						frv = (*functions->C_FindObjectsInit)(session_handle, attributes, 2); 
						if (ReturnedSuccesfull(frv,&trv, "C_FindObjectsInit", "test_sign" ))
						{
							frv = (*functions->C_FindObjects)(session_handle, &hKey,1,&ulObjectCount); 
							if (ReturnedSuccesfull(frv,&trv, "C_FindObjects", "test_sign" ))
							{
								frv = (*functions->C_FindObjectsFinal)(session_handle); 
								if (ReturnedSuccesfull(frv,&trv, "C_FindObjectsFinal", "test_sign" ))
								{
									frv = (*functions->C_SignInit)(session_handle, &mechanism, hKey); 
									if (ReturnedSuccesfull(frv,&trv, "C_SignInit", "test_sign" ))
									{
										frv = (*functions->C_Sign)(session_handle,data,(CK_ULONG) strlen(data),signature,&signLength);
										ReturnedSuccesfull(frv,&trv, "C_Sign", "test_sign" );
									}
								}
							}
							frv = (*functions->C_FindObjectsFinal)(session_handle); 
							ReturnedSuccesfull(frv,&trv, "C_FindObjectsFinal", "test_sign");
						}
						frv = (*functions->C_CloseSession) (session_handle);
						ReturnedSuccesfull(frv,&trv, "C_CloseSession", "test_sign" );
					}
				}
			}
		}
		frv = (*functions->C_Finalize) (NULL_PTR);
		ReturnedSuccesfull(frv,&trv, "C_Finalize", "test_sign" );
	}
	dlclose(handle);
	testlog(LVL_INFO, "test_sign leave\n");
	return trv;
} 

