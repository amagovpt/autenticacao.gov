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

CK_RV test_getprivatekeys() {
	void *handle;
	CK_FUNCTION_LIST_PTR functions;
	int retVal = 0;
	CK_RV frv;
	CK_RV	trv = CKR_OK; 
	CK_SESSION_HANDLE session_handle;
	long slot_count;
	CK_SLOT_ID_PTR slotIds;
	int slotIdx;
	CK_ULONG ulObjectCount=1;

	testlog(LVL_INFO, "test_getprivatekeys enter\n");
	if (!InitializeTest(&handle,&functions))
	{
		testlog(LVL_INFO, "test_getprivatekeys leave\n");
		return 1;
	}
	frv = (*functions->C_Initialize) (NULL);
	if (ReturnedSuccesfull(frv,&trv, "C_Initialize", "test_getprivatekeys" ))
	{		
		frv = (*functions->C_GetSlotList) (0, 0, &slot_count);
		if (ReturnedSuccesfull(frv,&trv, "C_GetSlotList", "test_getprivatekeys" ))
		{
			testlog(LVL_INFO,"slot count: %i\n", slot_count);
			slotIds = malloc(slot_count * sizeof(CK_SLOT_INFO));
			frv = (*functions->C_GetSlotList) (CK_FALSE, slotIds, &slot_count);
			if (ReturnedSuccesfull(frv,&trv, "C_GetSlotList (X2)", "test_getprivatekeys" ))
			{
				for (slotIdx = 0; slotIdx < slot_count; slotIdx++) 
				{
					CK_SLOT_ID slotId = slotIds[slotIdx];
					CK_ULONG private_key = CKO_PRIVATE_KEY;
					CK_ATTRIBUTE attributes[1] = {CKA_CLASS,&private_key,sizeof(CK_ULONG)};
					CK_VOID_PTR pValue = malloc (256);
					CK_BYTE_PTR pByte;
					CK_ULONG ulvalueLen = 255;//last one is for the string termination
					CK_ATTRIBUTE attr_label_templ = {	CKA_LABEL,pValue,ulvalueLen};
					CK_OBJECT_HANDLE hKey;

					frv = (*functions->C_OpenSession)(slotId, CKF_SERIAL_SESSION, NULL_PTR, NULL_PTR, &session_handle);
					if (ReturnedSuccesfull(frv,&trv, "C_OpenSession", "test_getprivatekeys" ))
					{
						frv = (*functions->C_FindObjectsInit)(session_handle, attributes, 1); 
						if (ReturnedSuccesfull(frv,&trv, "C_FindObjectsInit", "test_getprivatekeys" ))
						{
							while (ulObjectCount > 0)
							{
								frv = (*functions->C_FindObjects)(session_handle, &hKey,1,&ulObjectCount); 
								ReturnedSuccesfull(frv,&trv, "C_FindObjects", "test_getprivatekeys" );
								if (ulObjectCount > 0)
								{
									testlog(LVL_INFO, "found key, CK_OBJECT_HANDLE = %d\n",hKey);
									frv = (*functions->C_GetAttributeValue)(session_handle,hKey,&attr_label_templ,1);
									if (ReturnedSuccesfull(frv,&trv, "C_GetAttributeValue", "test_getprivatekeys" ))
									{
										pByte = attr_label_templ.pValue;
										pByte[attr_label_templ.ulValueLen] = 0;
										testlog(LVL_INFO,"key LABEL value = %s\n", pByte);
									}
								}
							}
							frv = (*functions->C_FindObjectsFinal)(session_handle); 
							ReturnedSuccesfull(frv,&trv, "C_FindObjectsFinal", "test_getprivatekeys" );
						}
					}
					frv = (*functions->C_CloseSession) (session_handle);
					ReturnedSuccesfull(frv,&trv, "C_CloseSession", "test_getprivatekeys" );
				}
			}
		}
		frv = (*functions->C_Finalize) (NULL_PTR);
		ReturnedSuccesfull(frv,&trv, "C_Finalize", "test_getprivatekeys" );
	}
	dlclose(handle);
	testlog(LVL_INFO, "test_getprivatekeys leave\n");
	return trv;
} 
