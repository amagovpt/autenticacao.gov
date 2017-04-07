#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include <wincrypt.h>

// Defines
#define CERT_PERSONAL_STORE_NAME  "My"

//Change this to sign with a different card/certificate
#define SIGNER_NAME L"MANUEL SILVA"

#define MY_TYPE  (PKCS_7_ASN_ENCODING | X509_ASN_ENCODING)
#define BUFSIZE 1024

// Local functions
void Sign(char * SignerName, char* DataFileName, char * SignatureFileName);

// ShowUsageAndExit
void ShowUsageAndExit()
{
	printf("Usage:\n");
	printf(" - To sign : cryptoAPI-test signer_name data_file signature_file\n");
	printf("\n << Press any key to continue >> \n");
	_getch();
	exit(1);
}

// CheckError
void CheckError(BOOL condition, char * message)
{
	
	if (!condition)
	{
		fprintf(stderr, "%s FAILURE(0x % x)\n", message, GetLastError());
		exit(1);
	}
}


void main(int argc, char** argv)
{

	// Usage
	if (argc != 4) {
		ShowUsageAndExit();
	}
	
		// Sign
	Sign(argv[1], argv[2], argv[3]);
	

	// The end
	printf("\n << Press any key to continue >> \n");
	_getch();

}

#define MAX_SIZE_CERT_NAME 1024

static PCCERT_CONTEXT
FindCertificate(const HCERTSTORE hStore, const char* CertSearchString)
{
	PCCERT_CONTEXT capiCertificate = NULL;
	DWORD dType = CERT_X500_NAME_STR | CERT_NAME_STR_REVERSE_FLAG;
	char certname[MAX_SIZE_CERT_NAME] = { 0 };

	for (;;)
	{
		capiCertificate = CertEnumCertificatesInStore(hStore, capiCertificate);
		if (NULL == capiCertificate)
		{
			fprintf(stderr, "CertEnumCertificatesInStore returned %08x\n", GetLastError());
			break;
		}

		if (FALSE ==CertGetNameStringA(capiCertificate, CERT_NAME_RDN_TYPE,	0, &dType, certname, MAX_SIZE_CERT_NAME))
		{
			CertFreeCertificateContext(capiCertificate);
			capiCertificate = NULL;
			break;
		}
		fprintf(stderr, "DEBUG: Finding cert: %s\n", certname);

		if ((0 == strncmp(certname, CertSearchString, MAX_SIZE_CERT_NAME)) &&
			(capiCertificate->dwCertEncodingType == X509_ASN_ENCODING))
		{
			break;
		}
	}

	return capiCertificate;
}

// Sign
void Sign(char * SignerName, char * DataFileName, char * SignatureFileName)
{
	// Variables
	HCERTSTORE hStoreHandle = NULL;
	PCCERT_CONTEXT pSignerCert = NULL;
	HCRYPTPROV hCryptProv = NULL;
	DWORD dwKeySpec = 0;
	HCRYPTHASH hHash = NULL;
	HANDLE hDataFile = NULL;
	BOOL bResult = FALSE;
	BYTE rgbFile[BUFSIZE];
	DWORD cbRead = 0;
	DWORD dwSigLen = 0;
	BYTE * pbSignature = NULL;
	HANDLE hSignatureFile = NULL;
	DWORD lpNumberOfBytesWritten = 0;

	//char * certSearchString = "C=PT, O=Cart\xC3\xA3o de Cidad\xC3\xA3o, OU=Cidad\xC3\xA3o Portugu\xC3\xAAs, OU=(Teste) Assinatura Qualificada do Cidad\xC3\xA3o, SN=SILVA, GN=MANUEL, serialNumber=BI000006017, CN=MANUEL SILVA";

	printf("Signing...\n");

	// Open the certificate store.
	hStoreHandle = CertOpenStore(CERT_STORE_PROV_SYSTEM, 0, 0, CERT_SYSTEM_STORE_CURRENT_USER, L"My");

	CheckError((BOOL)hStoreHandle, "CertOpenStore..");

	// Get signer’s certificate with access to private key.
	do {
		// Get a certificate that matches the given X500 subject string
		//pSignerCert = FindCertificate(hStoreHandle, certSearchString);

		pSignerCert = CertFindCertificateInStore(hStoreHandle,
			MY_TYPE, 0,	CERT_FIND_SUBJECT_STR, SIGNER_NAME /* SignerName */, pSignerCert);

		CheckError((BOOL)pSignerCert, "CertFindCertificateInStore....");

		// Get the CSP, and check if we can sign with the private key
		//This function will block waiting for the correct card if the certificate is associated with a different smart card
		bResult = CryptAcquireCertificatePrivateKey(
			pSignerCert,
			0,
			NULL,
			&hCryptProv,
			&dwKeySpec,
			NULL
			);
		CheckError(bResult, "CryptAcquireCertificatePrivateKey. ");

	} while ((dwKeySpec & AT_SIGNATURE) != AT_SIGNATURE);

	// Create the hash object.
	bResult = CryptCreateHash(
		hCryptProv,
		CALG_SHA1,
		0,
		0,
		&hHash
		);
	CheckError(bResult, "CryptCreateHash....... ");

	// Open the file with the content to be signed
	hDataFile = CreateFile(DataFileName,
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_SEQUENTIAL_SCAN,
		NULL
		);
	CheckError((hDataFile != INVALID_HANDLE_VALUE), "CreateFile..........");

	// Compute the cryptographic hash of the data.
	while (bResult = ReadFile(hDataFile, rgbFile, BUFSIZE, &cbRead, NULL))
	{
		if (cbRead == 0)
		{
			break;
		}
		CheckError(bResult, "ReadFile..........");

		bResult = CryptHashData(
			hHash,
			rgbFile,
			cbRead,
			0
			);
		CheckError(bResult, "CryptHashData.........");

	}
	CheckError(bResult, "ReadFile..........");

	// Sign the hash object
	dwSigLen = 0;
	bResult = CryptSignHash(
		hHash,
		AT_SIGNATURE,
		NULL,
		0,
		NULL,
		&dwSigLen
		);
	CheckError(bResult, "CryptSignHash.........");

	pbSignature = (BYTE *)malloc(dwSigLen);
	CheckError((BOOL)pbSignature, "malloc.......... ");

	bResult = CryptSignHash(
		hHash,
		AT_SIGNATURE,
		NULL,
		0,
		pbSignature,
		&dwSigLen
		);
	CheckError(bResult, "CryptSignHash.........");

	// Create a file to save the signature
	hSignatureFile = CreateFile(
		SignatureFileName,
		GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL
		);
	CheckError((hSignatureFile != INVALID_HANDLE_VALUE), "CreateFile..........");

	// Write the signature to the file
	bResult = WriteFile(
		hSignatureFile,
		(LPCVOID)pbSignature,
		dwSigLen,
		&lpNumberOfBytesWritten,
		NULL
		);
	CheckError(bResult, "WriteFile......... ");

	// Clean up and free memory.
	free(pbSignature);

	CloseHandle(hDataFile);
	CloseHandle(hSignatureFile);

	bResult = CryptDestroyHash(hHash);
	CheckError(bResult, "CryptDestroyHash........");

	bResult = CertFreeCertificateContext(pSignerCert);
	CheckError(bResult, "CertFreeCertificateContext....");

	bResult = CertCloseStore(
		hStoreHandle,
		CERT_CLOSE_STORE_CHECK_FLAG
		);
	CheckError(bResult, "CertCloseStore........");

}

