========================================================================
    CONSOLE APPLICATION : PTeIDWin32CertificateImport Project Overview
========================================================================

PTeIDWin32CertificateImport is a little helper that will import Root Certificates
to Windows Certificate Store after installation.

PTeIDWin32CertificateImport.vcproj
    This is the main project file for VC++ projects generated using an Application Wizard. 
    It contains information about the version of Visual C++ that generated the file, and 
    information about the platforms, configurations, and project features selected with the
    Application Wizard.

PTeIDWin32CertificateImport.cpp
    This is the main application source file.

/////////////////////////////////////////////////////////////////////////////
Other standard files:

StdAfx.h, StdAfx.cpp
    These files are used to build a precompiled header (PCH) file
    named PTeIDWin32CertificateImport.pch and a precompiled types file named StdAfx.obj.

/////////////////////////////////////////////////////////////////////////////



////////////////////CODE/////////////////////////////////////////////////////
// PTeIDWin32CertificateImport.cpp : Defines the entry point for the console application.
//


// PTeIDWin32CertificateImport.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>
#include <windows.h>
#include <Cryptuiapi.h>



void ImportCert(const char *certpath)
{
	PCCERT_CONTEXT pCertCtx = NULL;

	if (CryptQueryObject (
		CERT_QUERY_OBJECT_FILE,
		L(certpath),
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
	char certraizpath[300] = "C:\\Program Files\\Portugal Identity Card\\eidstore\\certs\\ECRaizEstado_novo_assinado_GTE.der";
	char *certccpath = "C:\\Program Files\\Portugal Identity Card\\eidstore\\certs\\CartaodeCidadao001.der";

	ImportCert(certraizpath);
	Sleep(20000);
	return 0;
}

