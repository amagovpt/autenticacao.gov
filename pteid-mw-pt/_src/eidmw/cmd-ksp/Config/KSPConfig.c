
/**
KSPConfig - command-line utility that performs registration and removal of the KSP module
and removal of associated certificates from local "MY" certificate store

*/


///////////////////////////////////////////////////////////////////////////////
//
// Headers...
//
///////////////////////////////////////////////////////////////////////////////
#include <windows.h>
#include <wincrypt.h>
#include <stdlib.h>
#include <stdio.h>
#include <bcrypt.h>
#include <ncrypt.h>
#include "..\inc\KSP.h"
///////////////////////////////////////////////////////////////////////////////
//
// Forward declarations of local routines...
//
///////////////////////////////////////////////////////////////////////////////
static int
DisplayUsage(
    void
    );

static int
EnumProviders(
    void
    );

static int
RegisterProvider(
    void
    );

static int
UnRegisterProvider(
    void
    );

static int
RemoveCmdCertificates(
    void
    );


///////////////////////////////////////////////////////////////////////////////
//
// The following section defines the characteristics of the
// provider being registered...
//
///////////////////////////////////////////////////////////////////////////////
//
// File name of sample key storage provider's binary. *NO* path.
//
#define CMDKSP_BINARY       L"cmdKSP.dll"

//
// An array of algorithm names, all belonging to the
// same algorithm class...
//
PWSTR AlgorithmNames[1] = {
    NCRYPT_KEY_STORAGE_ALGORITHM
};

//
// Definition of ONE class of algorithms supported
// by the provider...
//
CRYPT_INTERFACE_REG AlgorithmClass = {
    NCRYPT_KEY_STORAGE_INTERFACE,       // ncrypt key storage interface
    CRYPT_LOCAL,                        // Scope: local system only
    1,                                  // One algorithm in the class
    AlgorithmNames                      // The name(s) of the algorithm(s) in the class
};

//
// An array of ALL the algorithm classes supported
// by the provider...
//
PCRYPT_INTERFACE_REG AlgorithmClasses[1] = {
    &AlgorithmClass
};

//
// Definition of the provider's user-mode binary...
//
CRYPT_IMAGE_REG KspImage = {
    CMDKSP_BINARY,                   // File name of the sample KSP binary
    1,                                  // Number of algorithm classes the binary supports
    AlgorithmClasses                    // List of all algorithm classes available
};

//
// Definition of the overall provider...
//
CRYPT_PROVIDER_REG KSPProvider = {
    0,
    NULL,
    &KspImage,  // Image that provides user-mode support
    NULL              // Image that provides kernel-mode support (*MUST* be NULL)
};
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
//
// Main entry point...
//
///////////////////////////////////////////////////////////////////////////////
int __cdecl
wmain(
    __in int argc,
    __in_ecount(argc) PWSTR *argv)
{
    int exitCode = 0;
    if ( argc > 1 )
    {
        if ((_wcsicmp(argv[1], L"-register") == 0))
        {
            exitCode = RegisterProvider();
        }
        else if ((_wcsicmp(argv[1], L"-unregister")  == 0))
        {
            exitCode = UnRegisterProvider();
        }
        else if ((_wcsicmp(argv[1], L"-enum")  == 0))
        {
            exitCode = EnumProviders();
        }
        else if ((_wcsicmp(argv[1], L"-removeCerts") == 0))
        {
            exitCode = RemoveCmdCertificates();
        }
        else
        {
            wprintf(L"Unrecognized command \"%s\"\n", argv[1]);
            DisplayUsage();
        }
    }
    else
    {
        DisplayUsage();
        return 1;
    }

    return exitCode;
}
///////////////////////////////////////////////////////////////////////////////

int
DisplayUsage()
{
    wprintf(L"Usage: KSPconfig -enum | -register | -unregister | -removeCerts\n");
    exit(1);
}
///////////////////////////////////////////////////////////////////////////////

int
EnumProviders()
{
    NTSTATUS ntStatus = STATUS_SUCCESS;

    DWORD cbBuffer = 0;
    PCRYPT_PROVIDERS pBuffer = NULL;
    DWORD i = 0;

    ntStatus = BCryptEnumRegisteredProviders(&cbBuffer, &pBuffer);

    if (NT_SUCCESS(ntStatus))
    {
        if (pBuffer == NULL)
        {
            wprintf(L"BCryptEnumRegisteredProviders returned a NULL ptr\n");
        }
        else
        {
            for (i = 0; i < pBuffer->cProviders; i++)
            {
                wprintf(L"%s\n", pBuffer->rgpszProviders[i]);
            }
        }
    }
    else
    {
        wprintf(L"BCryptEnumRegisteredProviders failed with error code 0x%08x\n", ntStatus);
        return ntStatus;
    }

    if (pBuffer != NULL)
    {
        BCryptFreeBuffer(pBuffer);
    }
    return 0;
}
///////////////////////////////////////////////////////////////////////////////

int
RegisterProvider(
    void
    )
{
    NTSTATUS ntStatus = STATUS_SUCCESS;

    //
    // Make CNG aware that our provider
    // exists...
    //
    ntStatus = BCryptRegisterProvider(
                    CMDKSP_PROVIDER_NAME,
                    0,                          // Flags: fail if provider is already registered
                    &KSPProvider
                    );
    if (!NT_SUCCESS(ntStatus))
    {
        wprintf(L"BCryptRegisterProvider failed with error code 0x%08x\n", ntStatus);
        return ntStatus;
    }

    //
    // Add the algorithm name to the priority list of the
    // symmetric cipher algorithm class. (This makes it
    // visible to BCryptResolveProviders.)
    //
    ntStatus = BCryptAddContextFunction(
                    CRYPT_LOCAL,                    // Scope: local machine only
                    NULL,                           // Application context: default
                    NCRYPT_KEY_STORAGE_INTERFACE,   // Algorithm class
                    NCRYPT_KEY_STORAGE_ALGORITHM,   // Algorithm name
                    CRYPT_PRIORITY_BOTTOM           // Lowest priority
                    );
    if ( !NT_SUCCESS(ntStatus))
    {
        wprintf(L"BCryptAddContextFunction failed with error code 0x%08x\n", ntStatus);
        return ntStatus;
    }

    //
    // Identify our new provider as someone who exposes
    // an implementation of the new algorithm.
    //
    ntStatus = BCryptAddContextFunctionProvider(
                    CRYPT_LOCAL,                    // Scope: local machine only
                    NULL,                           // Application context: default
                    NCRYPT_KEY_STORAGE_INTERFACE,   // Algorithm class
                    NCRYPT_KEY_STORAGE_ALGORITHM,   // Algorithm name
                    CMDKSP_PROVIDER_NAME,        // Provider name
                    CRYPT_PRIORITY_BOTTOM           // Lowest priority
                    );
    if ( !NT_SUCCESS(ntStatus))
    {
        wprintf(L"BCryptAddContextFunctionProvider failed with error code 0x%08x\n", ntStatus);
        return ntStatus;
    }
    return 0;
}
///////////////////////////////////////////////////////////////////////////////

int
UnRegisterProvider()
{
    NTSTATUS ntStatus = STATUS_SUCCESS;

    //
    // Tell CNG that this provider no longer supports
    // this algorithm.
    //
    ntStatus = BCryptRemoveContextFunctionProvider(
                    CRYPT_LOCAL,                    // Scope: local machine only
                    NULL,                           // Application context: default
                    NCRYPT_KEY_STORAGE_INTERFACE,   // Algorithm class
                    NCRYPT_KEY_STORAGE_ALGORITHM,   // Algorithm name
                    CMDKSP_PROVIDER_NAME         // Provider
                    );
    if (!NT_SUCCESS(ntStatus))
    {
        wprintf(L"BCryptRemoveContextFunctionProvider failed with error code 0x%08x\n", ntStatus);
        return ntStatus;
    }


    //
    // Tell CNG to forget about our provider component.
    //
    ntStatus = BCryptUnregisterProvider(CMDKSP_PROVIDER_NAME);
    if (!NT_SUCCESS(ntStatus))
    {
        wprintf(L"BCryptUnregisterProvider failed with error code 0x%08x\n", ntStatus);
        return ntStatus;
    }
    return 0;
}
///////////////////////////////////////////////////////////////////////////////
int
RemoveCmdCertificates(void)
{
    HCERTSTORE hMyStore = NULL;
    PCCERT_CONTEXT pCert = NULL;

    hMyStore = CertOpenSystemStore((HCRYPTPROV_LEGACY)NULL, L"MY");
    if (!hMyStore) {
		DWORD err = GetLastError();
        wprintf(L"CertOpenSystemStore failed with error code 0x%08x\n", err);
        CertCloseStore(hMyStore, CERT_CLOSE_STORE_FORCE_FLAG);
        return err;
    }

    while (pCert = CertFindCertificateInStore(
            hMyStore,
            X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
            0,
            CERT_FIND_ISSUER_STR,
            L"EC de Chave Móvel Digital",
            pCert
            ))
    {
        if (!CertDuplicateCertificateContext(pCert))
        {
            DWORD err = GetLastError();
            wprintf(L"CertDuplicateCertificateContext failed with error code 0x%08x\n", err);
            return err;
        }
        if (!CertDeleteCertificateFromStore(pCert))
        {
            DWORD err = GetLastError();
            wprintf(L"CertDeleteCertificateFromStore failed with error code 0x%08x\n", err);
            return err;
        }
    }

    CertCloseStore(hMyStore, CERT_CLOSE_STORE_FORCE_FLAG);
    return 0;
}
///////////////////////////////////////////////////////////////////////////////