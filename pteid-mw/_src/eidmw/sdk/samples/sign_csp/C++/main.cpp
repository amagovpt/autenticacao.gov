//*****************************************************************************
// eID SDK sample code.
//*****************************************************************************
#include <iostream>
#include <windows.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "eidlib.h"


using namespace eIDMW;

int LastError=0;

const char* signCSP(const char *card_serial_number )
{
	HCRYPTPROV hprov = NULL;

	//Get a context
	if (! CryptAcquireContextA(&hprov, card_serial_number, "Portugal Identity Card CSP", PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)) {
		LastError = GetLastError();
		return "CryptAcquireContext failed.";
	}

	//Open the data to sign
	unsigned char signature[1024];
	unsigned long sig_len = sizeof(signature);
	unsigned char *data=NULL;
	unsigned long data_len=0;
	FILE *f=NULL;

	f = fopen("sign_csp.exe", "rb");
	if (f == NULL)															
		return "Could not find file sign_csp.exe";		
    struct _stat file_info = {0};
    if(0 == _fstat(_fileno(f), &file_info))
    {
        data = (unsigned char *)malloc(file_info.st_size);
		data_len = file_info.st_size;
		if(data_len != fread(data, sizeof(unsigned char), data_len, f)) 
		{
			free(data);														
			return "Problem reading file sign_csp.exe";
		}
	}
	fclose(f);
	f=NULL; 

	HCRYPTHASH hhash;

	if (! CryptCreateHash(hprov, CALG_SHA1, 0, 0, &hhash)) {
		LastError = GetLastError();
		return "CryptCreateHash failed.";
	}

	if (! CryptHashData(hhash, data, data_len, 0)) {
		LastError = GetLastError();
		return "CryptHashData failed";
	}

	if (! CryptSignHash(hhash, AT_SIGNATURE, NULL, 0, signature, &sig_len)) {
		LastError = GetLastError();
		return "CryptSignHash failed.";
	}

	//Write the signature into file
	f = fopen("sign_csp_cpp.sig", "wb");
	if (f == NULL)	
	{
		return "Could not create sign_csp.sig";		
	}
	if(sig_len != fwrite(signature, sizeof(unsigned char), sig_len, f))	
	{
		return "Problem writing file sign_csp.sig";
	}
	fclose(f);
	f=NULL;
 
	if (NULL != data)
		free(data);

	if (NULL != hhash)
		CryptDestroyHash(hhash);

	if (NULL != hprov)
		CryptReleaseContext(hprov, 0);

	return "SUCCEED";
}

//*****************************************************************************
// Main entry point
//*****************************************************************************
int main( int argc, char* argv[] )
{
	std::cout << "Please insert a card to sign.\nBe sure that certificate are registered in the windows store.\n(Press enter when ready)" << std::endl;

	char c=getchar();
 

	PTEID_InitSDK();
	PTEID_ReaderContext &reader = ReaderSet.getReader();
	PTEID_EIDCard &card = reader.getEIDCard();
	PTEID_CardVersionInfo &doc = card.getVersionInfo();

	char card_serial_number[50];
	sprintf(card_serial_number,"Signature(%s)",doc.getSerialNumber());

	std::cout << "Container = " << card_serial_number << std::endl;

	const char *signMsg=signCSP(card_serial_number);
	std::cout << "Signature: " << signMsg << std::endl;

	if(LastError)
		std::cout << "LastError=" << std::hex << LastError << std::endl;

	PTEID_ReleaseSDK();

}

