#include "stdafx.h"

#include <iostream>
#include <windows.h>
#include <Cryptuiapi.h>



void ImportECRaizCert()
{
	PCCERT_CONTEXT pCertCtx = NULL;

	if (CryptQueryObject (
		CERT_QUERY_OBJECT_FILE,
		L"C:\\Program Files\\Portugal Identity Card\\eidstore\\certs\\ECRaizEstado_novo_assinado_GTE.der",
		CERT_QUERY_CONTENT_FLAG_ALL,
		CERT_QUERY_FORMAT_FLAG_ALL,
		0,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		(const void **)&pCertCtx) != 0)
	{
		HCERTSTORE hCertStore = CertOpenStore (
			CERT_STORE_PROV_SYSTEM,
			0,
			0,
			CERT_STORE_OPEN_EXISTING_FLAG |
			CERT_SYSTEM_STORE_LOCAL_MACHINE,
			L"ROOT");
		if (hCertStore != NULL)
		{
			if (CertAddCertificateContextToStore (
				hCertStore,
				pCertCtx,
				CERT_STORE_ADD_ALWAYS,
				NULL))
			{
				std::cout << "Added certificate to store." << std::endl;
			}

			if (CertCloseStore (hCertStore, 0))
			{
				std::cout << "Cert. store handle closed." << std::endl;
			}
		}

		if (pCertCtx)
		{
			CertFreeCertificateContext (pCertCtx);
		}
	}
}

void ImportCCCert()
{
	PCCERT_CONTEXT pCertCtx = NULL;

	if (CryptQueryObject (
		CERT_QUERY_OBJECT_FILE,
		L"C:\\Program Files\\Portugal Identity Card\\eidstore\\certs\\CartaodeCidadao001.der",
		CERT_QUERY_CONTENT_FLAG_ALL,
		CERT_QUERY_FORMAT_FLAG_ALL,
		0,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		(const void **)&pCertCtx) != 0)
	{
		HCERTSTORE hCertStore = CertOpenStore (
			CERT_STORE_PROV_SYSTEM,
			0,
			0,
			CERT_STORE_OPEN_EXISTING_FLAG |
			CERT_SYSTEM_STORE_LOCAL_MACHINE,
			L"ROOT");
		if (hCertStore != NULL)
		{
			if (CertAddCertificateContextToStore (
				hCertStore,
				pCertCtx,
				CERT_STORE_ADD_ALWAYS,
				NULL))
			{
				std::cout << "Added certificate to store." << std::endl;
			}

			if (CertCloseStore (hCertStore, 0))
			{
				std::cout << "Cert. store handle closed." << std::endl;
			}
		}

		if (pCertCtx)
		{
			CertFreeCertificateContext (pCertCtx);
		}
	}
}

int _tmain(int argc, _TCHAR* argv[])
{

	std::cout << "PTeID Certificate import." << std::endl;

	ImportECRaizCert();
	ImportCCCert();
	return 0;
}


