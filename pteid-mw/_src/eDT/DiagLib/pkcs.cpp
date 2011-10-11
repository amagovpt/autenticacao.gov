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
#ifdef WIN32
#include <windows.h>
#elif __APPLE__
#include "Mac/mac_helper.h"
#endif

#include "progress.h"

#include "pkcs.h"
#include "error.h"
#include "log.h"
#include "util.h"

#include "diaglib.h"

#include "pkcs11/cryptoki.h"

typedef CK_RV (*P11_GetFunctionList)(CK_FUNCTION_LIST_PTR_PTR ppFunctionList); 

static HMODULE g_hPkcsLib=NULL;
static CK_FUNCTION_LIST_PTR g_p11Function;

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PRIVATE FUNCTIONS DECLARATION ////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
int loadPkcsLib(CK_FUNCTION_LIST_PTR *pp11Function);
int unloadPkcsLib();

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PUBLIC FUNCTIONS /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
int pkcsIsAvailable(bool *available)
{
	int iReturnCode = DIAGLIB_OK;

	if(available == NULL)
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;
	}

	*available = false;

	CK_FUNCTION_LIST_PTR p11Function;

	if(DIAGLIB_OK != (iReturnCode = loadPkcsLib(&p11Function)))
	{
		return DIAGLIB_OK;
	}

	int err = CKR_OK;

	//Initialize PKCS#11
	if(CKR_OK != (err = p11Function->C_Initialize(NULL)))
	{
		LOG_ERRORCODE(L"C_Initialize failed",err);
		return DIAGLIB_OK;
	}

	*available = true;

	//Finalize PKCS#11
	if(CKR_OK != (err = p11Function->C_Finalize(NULL)))
	{
		LOG_ERRORCODE(L"C_Finalize failed",err);
	}
	
	if(DIAGLIB_OK != unloadPkcsLib())
	{
		LOG_ERROR(L"unloadPkcsLib failed");
	}

	return iReturnCode;
} 

////////////////////////////////////////////////////////////////////////////////////////////////
int pkcsTestSign(Card_ID id, Cert_TYPE cert, bool *succeed)
{
	int iReturnCode = DIAGLIB_OK;

	if(succeed == NULL)
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;
	}

	*succeed=false;

	CK_FUNCTION_LIST_PTR p11Function;

	if(DIAGLIB_OK != (iReturnCode = loadPkcsLib(&p11Function)))
	{
		return RETURN_LOG_ERROR(DIAGLIB_ERR_LIBRARY_NOT_FOUND);
	}

	int err = CKR_OK;

	//Initialize PKCS#11
	if(CKR_OK != (err = p11Function->C_Initialize(NULL)))
	{
		LOG_ERRORCODE(L"C_Initialize failed",err);
		return RETURN_LOG_ERROR(DIAGLIB_ERR_PKCS_INIT_FAILED);
	}

	progressInit(4);

	CK_SLOT_ID_PTR p11_slot_list = NULL;
	CK_ULONG p11_count_slots=0;
	CK_SLOT_INFO p11_slot_info;
	bool slot_found = false;
	CK_ULONG slot_num=0;
	CK_SESSION_HANDLE p11_session = CK_INVALID_HANDLE;
	CK_ATTRIBUTE attrs[2];
	CK_OBJECT_CLASS cls = CKO_PRIVATE_KEY;
	CK_ULONG key_id=(cert==SIGN_CERT_TYPE?3:2);
	CK_OBJECT_HANDLE key=CK_INVALID_HANDLE;
	CK_ULONG key_count;
	CK_MECHANISM	mech;
	unsigned char signature[1024];
	CK_ULONG sig_len = sizeof(signature);


	//Get the list of reader with card
	if(CKR_OK != (err = p11Function->C_GetSlotList(TRUE, NULL_PTR, &p11_count_slots)))
	{
		LOG_ERRORCODE(L"C_GetSlotList failed",err);
		iReturnCode = DIAGLIB_ERR_PKCS_FAILED;
		goto finalize;
	}

	if(p11_count_slots<=0)
	{
		LOG_ERRORCODE(L"No slot found",err);
		iReturnCode = DIAGLIB_ERR_CARD_NOT_FOUND;
		goto finalize;
	}

	if(NULL == (p11_slot_list = (CK_SLOT_ID_PTR) malloc(p11_count_slots*sizeof(CK_SLOT_ID))))
	{
		LOG_ERROR(L"malloc failed");
		iReturnCode = DIAGLIB_ERR_INTERNAL;
		goto finalize;
	}


	if(CKR_OK != (err = p11Function->C_GetSlotList(TRUE, p11_slot_list, &p11_count_slots)))
	{
		LOG_ERRORCODE(L"C_GetSlotList failed",err);
		iReturnCode = DIAGLIB_ERR_PKCS_FAILED;
		goto finalize;
	}

	//Parse the slot to find the Card_ID
	progressIncrement();
	
	for(CK_ULONG i=0;i<p11_count_slots;i++)
	{
		if(CKR_OK != (err = p11Function->C_GetSlotInfo(p11_slot_list[i], &p11_slot_info)))
		{
			LOG_ERRORCODE(L"C_GetSlotInfo failed",err);
		}
		else
		{
			if(memcmp(p11_slot_info.slotDescription,string_From_wstring(id.Reader.Name).c_str(),id.Reader.Name.length())==0)
			{
				slot_num=i;
				slot_found=true;
				break;
			}
		}
	}

	if(!slot_found)
	{
		iReturnCode = DIAGLIB_ERR_CARD_NOT_FOUND;
		goto finalize;
	}

	//Open the P11 session
	progressIncrement();
	
	if(CKR_OK != (err = p11Function->C_OpenSession(p11_slot_list[slot_num],CKF_SERIAL_SESSION | CKF_RW_SESSION,NULL, NULL, &p11_session)))
	{
		LOG_ERRORCODE(L"C_OpenSession failed",err);
		iReturnCode = DIAGLIB_ERR_PKCS_FAILED;
		goto finalize;
	}

	//Find the private key
	
	attrs[0].type = CKA_CLASS;
	attrs[0].pValue = &cls;
	attrs[0].ulValueLen = sizeof(cls);
	attrs[1].type = CKA_ID;
	attrs[1].pValue = &key_id;
	attrs[1].ulValueLen = sizeof(key_id);

	if(CKR_OK != (err = p11Function->C_FindObjectsInit(p11_session,attrs,2)))
	{
		LOG_ERRORCODE(L"C_FindObjectsInit failed",err);
		iReturnCode = DIAGLIB_ERR_PKCS_FAILED;
		goto close_session;
	}

	if(CKR_OK != (err = p11Function->C_FindObjects(p11_session, &key, 1, &key_count)))
	{
		LOG_ERRORCODE(L"C_FindObjects failed",err);
		iReturnCode = DIAGLIB_ERR_PKCS_FAILED;
		goto close_session;
	}

	if(key_count<=0) 
	{
		LOG_ERRORCODE(L"Key not found",err);
		iReturnCode = DIAGLIB_ERR_PKCS_KEY_NOT_FOUND;
		goto close_session;
	}

	if(CKR_OK != (err = p11Function->C_FindObjectsFinal(p11_session)))
	{
		LOG_ERRORCODE(L"C_FindObjectsFinal failed",err);
		iReturnCode = DIAGLIB_ERR_PKCS_FAILED;
		goto close_session;
	}

	//Initialize the signature
	mech.mechanism=CKM_SHA1_RSA_PKCS;
	mech.pParameter=NULL;
	mech.ulParameterLen=0;
	if(CKR_OK != (err = p11Function->C_SignInit(p11_session, &mech, key)))
	{
		LOG_ERRORCODE(L"C_SignInit failed",err);
		iReturnCode = DIAGLIB_ERR_PKCS_SIGNING_FAILED;
		goto close_session;
	}

	progressIncrement();

	//Sign the data
	if(CKR_OK != (err = p11Function->C_Sign(p11_session, (CK_BYTE_PTR)DATA_TO_SIGN, sizeof(DATA_TO_SIGN), signature, &sig_len)))
	{
		LOG_ERRORCODE(L"C_Sign failed",err);
		switch(err)
		{
		case CKR_FUNCTION_CANCELED:
			LOG_ERROR(L"The pin was canceled by the user");
			iReturnCode = DIAGLIB_ERR_PIN_CANCEL;
			break;
		case CKR_PIN_LOCKED:
			LOG_ERROR(L"The card is blocked");
			iReturnCode = DIAGLIB_ERR_PIN_BLOCKED;
			break;
		case CKR_PIN_INCORRECT:
			LOG_ERROR(L"Wrong pin entered");
			iReturnCode = DIAGLIB_ERR_PIN_WRONG;
			break;
		case CKR_PIN_INVALID:
			LOG_ERROR(L"Authentication problem");
			iReturnCode = DIAGLIB_ERR_PIN_FAILED;
			break;
		default:
			iReturnCode = DIAGLIB_ERR_PKCS_SIGNING_FAILED;
			break;
		}
		goto close_session;
	}

	*succeed=true;
	progressIncrement();

close_session:
	//Close the session
	if(CKR_OK != (err = p11Function->C_CloseSession(p11_session)))
	{
		LOG_ERRORCODE(L"C_CloseSession failed",err);
	}

finalize:
	//Finalize PKCS#11
	if(CKR_OK != (err = p11Function->C_Finalize(NULL)))
	{
		LOG_ERRORCODE(L"C_Finalize failed",err);
	}

	if(p11_slot_list)
	{
		free(p11_slot_list);
		p11_slot_list = NULL;
	}
	
	if(DIAGLIB_OK != unloadPkcsLib())
	{
		LOG_ERROR(L"unloadPkcsLib failed");
	}

	progressRelease();

	return iReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PRIVATE FUNCTIONS ////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
int loadPkcsLib(CK_FUNCTION_LIST_PTR *pp11Function)
{
	int iReturnCode = DIAGLIB_OK;

	if(pp11Function == NULL)
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;
	}

	if(g_hPkcsLib != NULL && g_p11Function != NULL)
	{
		*pp11Function = g_p11Function;
	}
	else
	{
#ifdef WIN32
	const wchar_t *pkcsLib = L"pteidpkcs11.dll";
#elif __APPLE__
	const wchar_t *pkcsLib = L"/usr/local/lib/libpteidpkcs11.dylib";
#endif
		if(NULL == (g_hPkcsLib=LoadLibrary(pkcsLib)))
		{
			LOG_LASTERROR(L"LoadLibrary failed");
			return DIAGLIB_ERR_LIBRARY_NOT_FOUND;
		}

		P11_GetFunctionList getP11Function = NULL;
		if(NULL == (getP11Function = reinterpret_cast<P11_GetFunctionList>(GetProcAddress(g_hPkcsLib, "C_GetFunctionList"))))
		{
			LOG_LASTERROR(L"GetProcAddress on C_GetFunctionList failed");
			FreeLibrary(g_hPkcsLib);
			g_hPkcsLib=NULL;
			return DIAGLIB_ERR_INTERNAL;
		}

 		getP11Function(&g_p11Function);

		if(g_p11Function == NULL)
		{
			LOG_ERROR(L"getP11Function failed");
			FreeLibrary(g_hPkcsLib);
			g_hPkcsLib=NULL;
			return DIAGLIB_ERR_INTERNAL;
		}
		else
		{
			*pp11Function = g_p11Function;
		}
	}

	return iReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
int unloadPkcsLib()
{
	int iReturnCode = DIAGLIB_OK;
	
	if(g_hPkcsLib == NULL)
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;
	}

	if(!FreeLibrary(g_hPkcsLib))
	{
		LOG_LASTERROR(L"FreeLibrary failed");
		iReturnCode = DIAGLIB_ERR_INTERNAL;
	}
	else
	{
		g_hPkcsLib=NULL;
		g_p11Function=NULL;
	}

	return iReturnCode;
}