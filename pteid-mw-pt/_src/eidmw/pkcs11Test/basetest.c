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

/*
 * Integration test for the PKCS#11 library.
 * Required interaction: none.
 */

#include <stdio.h>
#include "basetest.h"
#include "logtest.h"

int GetPKCS11FunctionList(CK_FUNCTION_LIST_PTR *pFunctions, void *handle){
	CK_C_GetFunctionList pC_GetFunctionList;
	CK_RV frv;

	testlog(LVL_INFO,"GetPKCS11FunctionList enter\n");

	// get function pointer to C_GetFunctionList
	pC_GetFunctionList = (CK_C_GetFunctionList) dlsym(handle, "C_GetFunctionList");
	if (pC_GetFunctionList == NULL) {
	    dlclose(handle);
	    testlog(LVL_ERROR,"dlsym failed\n");
		testlog(LVL_INFO,"GetPKCS11FunctionList leave\n");
		return 1;
	}

	// invoke C_GetFunctionList
	frv = (*pC_GetFunctionList) (pFunctions);
	if (frv != CKR_OK) {
	    testlog(LVL_ERROR,"C_GetFunctionList failed\n");
	    testlog(LVL_INFO,"GetPKCS11FunctionList leave\n");
		return 1;  
	}

	testlog(LVL_INFO,"GetPKCS11FunctionList leave\n");
	return 0;
}

CK_BBOOL ReturnedSuccesfull(CK_RV frv, CK_RV *ptrv, char* pkcs11function, char* test_name )
{
	if (CKR_OK != frv) {
	  testlog(LVL_ERROR, "%s error frv = 0x%.8x \n",pkcs11function,frv);
		testlog(LVL_INFO, "%s leave\n",test_name);
		if(*ptrv == CKR_OK)
		{
			*ptrv = frv;
		}
	  return CK_FALSE;  
	}
	return CK_TRUE;
}

CK_BBOOL InitializeTest(void **phandle,CK_FUNCTION_LIST_PTR *pfunctions)
{
	testlog(LVL_INFO, "InitializeTest enter\n");
	*phandle = dlopen(PKCS11_LIB, RTLD_LAZY); // RTLD_NOW is slower
	if (NULL == *phandle) {
		testlog(LVL_ERROR, "dlopen error, couldn't open pkcs11 lib\n");
		testlog(LVL_INFO, "InitializeTest leave\n");
		return CK_FALSE;
	}
	GetPKCS11FunctionList(pfunctions, *phandle);

	testlog(LVL_INFO, "InitializeTest leave\n");
	return CK_TRUE;
}
