//*****************************************************************************
// eID SDK sample code.
//*****************************************************************************
#include <iostream>
#include <stdio.h>
#ifdef WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#include <sys/stat.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include "cryptoki.h"	//Download from http://www.rsa.com/rsalabs/node.asp?id=2133


#ifdef WIN32
#define FILE_TO_SIGN "sign_p11.exe"
typedef CK_RV (*P11_GetFunctionList)(CK_FUNCTION_LIST_PTR_PTR ppFunctionList);
#else
#define FILE_TO_SIGN "sign_p11"
#endif
CK_RV LastRV=CKR_OK;

const char* signPkcs11( )
{

	CK_FUNCTION_LIST_PTR p11;
#ifdef WIN32
	HMODULE hLibrary=LoadLibrary(L"beidpkcs11.dll");
	if(!hLibrary)
		return "Library not found";

 	P11_GetFunctionList getP11Function = (P11_GetFunctionList) GetProcAddress(hLibrary, "C_GetFunctionList"); 
	getP11Function(&p11);
#else
#ifdef __APPLE__
	const char *P11_PATH = "/usr/local/lib/libbeidpkcs11.dylib";
#else
	const char *P11_PATH = "/usr/local/lib/libbeidpkcs11.so";
#endif
	void * dllPtr = NULL;
	void (*symPtr)(CK_FUNCTION_LIST_PTR_PTR ppFunctionList) = NULL;

	/* Open the PKCS11 API shared library, and inform the user if there is an
	* error */
	dllPtr = dlopen(P11_PATH, RTLD_NOW);
	if (!dllPtr) 
	{
		std::cout << "dlopen failed" << std::endl;
		return "Library libbeidpkcs11.so not found";
	}
	/* Get the list of the PKCS11 functions this token supports */
	symPtr = (void (*)(CK_FUNCTION_LIST_PTR_PTR ppFunctionList))dlsym(dllPtr, "C_GetFunctionList");
	if (!symPtr) 
	{
		return "Error getting function list";
	}
	symPtr(&p11);
#endif	
	//Initialize PKCS#11
	LastRV=p11->C_Initialize(NULL); 

	if(LastRV != CKR_OK) return "C_Initialize failed.";

	//Get the list of reader with card (we are only interested with the first one)
	CK_SLOT_ID p11_slot=0;
	CK_ULONG p11_num_slots=1;
	LastRV=p11->C_GetSlotList(TRUE, &p11_slot, &p11_num_slots);

	if(LastRV != CKR_OK) return "C_GetSlotList failed.";

	//Open the P11 session
	CK_SESSION_HANDLE p11_session = CK_INVALID_HANDLE;
	LastRV=p11->C_OpenSession(p11_slot,CKF_SERIAL_SESSION | CKF_RW_SESSION,NULL, NULL, &p11_session); 

	if(LastRV != CKR_OK) return "C_OpenSession failed.";

	//Find the signature private key
	CK_ATTRIBUTE attrs[2];
	CK_OBJECT_CLASS cls = CKO_PRIVATE_KEY;
	CK_ULONG id=3;
	CK_OBJECT_HANDLE signaturekey=CK_INVALID_HANDLE;
	CK_ULONG count;

	attrs[0].type = CKA_CLASS;
	attrs[0].pValue = &cls;
	attrs[0].ulValueLen = sizeof(cls);
	attrs[1].type = CKA_ID;
	attrs[1].pValue = &id;
	attrs[1].ulValueLen = sizeof(id);

	LastRV=p11->C_FindObjectsInit(p11_session,attrs,2);

	if(LastRV != CKR_OK) return "C_FindObjectsInit failed.";

	LastRV=p11->C_FindObjects(p11_session, &signaturekey, 1, &count);		

	if(LastRV != CKR_OK) return "C_FindObjects failed.";
	if(count==0) 	     return "Signature key not found.";

	LastRV=p11->C_FindObjectsFinal(p11_session);

	if(LastRV != CKR_OK) return "C_FindObjectsFinal failed.";

	//Initialize the signature
	CK_MECHANISM	mech;
	mech.mechanism=CKM_SHA1_RSA_PKCS;
	mech.pParameter=NULL;
	mech.ulParameterLen=0;
	LastRV=p11->C_SignInit(p11_session, &mech, signaturekey);

	if(LastRV != CKR_OK) return "C_SignInit failed.";

	//Open the data to sign
	unsigned char signature[1024];
	CK_ULONG sig_len = sizeof(signature);
	unsigned char *data=NULL;
	CK_ULONG data_len=0;
	FILE *f=NULL;

	f = fopen(FILE_TO_SIGN, "rb");

	if (f == NULL) return "Could not find file sign_p11";		
	
	struct stat file_info = {0};

	if(0 == fstat(fileno(f), &file_info))
	{
		data = new unsigned char[file_info.st_size];
		data_len = file_info.st_size;
		if(data_len != fread(data, sizeof(unsigned char), data_len, f)) 
		{
			delete(data);
			return "Problem reading file sign_p11";
		}
	}
	fclose(f);
	f=NULL;

	//Sign the data
	LastRV=p11->C_Sign(p11_session, data, data_len, signature, &sig_len);	
	
	if(LastRV != CKR_OK) return "C_Sign failed.";

	delete(data);

	//Write the signature into file
	f = fopen("sign_p11_cpp.sig", "wb");
	if (f == NULL) return "Could not create sign_p11.sig";		
	if(sig_len != fwrite(signature, sizeof(unsigned char), sig_len, f)) return "Problem writing file sign_p11.sig";
	fclose(f);
	f=NULL;

	//Close the session
	LastRV=p11->C_CloseSession(p11_session);
	
	if(LastRV != CKR_OK) return "C_CloseSession failed.";

	//Finalize PKCS#11
	LastRV=p11->C_Finalize(NULL);
	
	if(LastRV != CKR_OK) return "C_Finalize failed.";
#ifdef WIN32	
	FreeLibrary(hLibrary);
#else
	if (dllPtr) 
		dlclose(dllPtr);
#endif
	return "SUCCEED";
}

//*****************************************************************************
// Main entry point
//*****************************************************************************
int main( int argc, char* argv[] )
{
	std::cout << "Please insert a card to sign (Press enter when ready)" << std::endl;

	char c=getchar();
 
	const char *signMsg=signPkcs11();
	std::cout << "Signature: " << signMsg << std::endl;

	if(LastRV)
		std::cout << "LastRv=" << std::hex << LastRV << std::endl;
}

