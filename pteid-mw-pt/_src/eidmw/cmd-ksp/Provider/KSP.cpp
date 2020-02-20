// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.

/*++

Abstract:
    Implementation of the CMD CNG RSA key storage provider
--*/


///////////////////////////////////////////////////////////////////////////////
//
// Headers
//
///////////////////////////////////////////////////////////////////////////////
#include <windows.h>
#include <wincrypt.h>
#include <bcrypt.h>
#include <ncrypt.h>
#include "../inc/KSP.h"
#include "../Inc/log.h"
#include "../Inc/KSPDlgHelper.h"
#include "CMDSignature.h"
#include "proxyinfo.h"
#include "credentials.h"
#include "dialogs.h"
#include "Util.h"
#include "language.h"

using namespace eIDMW;

///////////////////////////////////////////////////////////////////////////////
//
// Ncrypt key storage provider function table
//
///////////////////////////////////////////////////////////////////////////////
NCRYPT_KEY_STORAGE_FUNCTION_TABLE KSPFunctionTable =
{
    CMDKSP_INTERFACE_VERSION,
    KSPOpenProvider,
    KSPOpenKey,
    KSPCreatePersistedKey,
    KSPGetProviderProperty,
    KSPGetKeyProperty,
    KSPSetProviderProperty,
    KSPSetKeyProperty,
    KSPFinalizeKey,
    KSPDeleteKey,
    KSPFreeProvider,
    KSPFreeKey,
    KSPFreeBuffer,
    KSPEncrypt,
    KSPDecrypt,
    KSPIsAlgSupported,
    KSPEnumAlgorithms,
    KSPEnumKeys,
    KSPImportKey,
    KSPExportKey,
    KSPSignHash,
    KSPVerifySignature,
    KSPPromptUser,
    KSPNotifyChangeKey,
    KSPSecretAgreement,
    KSPDeriveKey,
    KSPFreeSecret
};

///////////////////////////////////////////////////////////////////////////////
//
// Global Variables
//
///////////////////////////////////////////////////////////////////////////////
HINSTANCE dllHandle;

///////////////////////////////////////////////////////////////////////////////
//
// Dll entry
//
///////////////////////////////////////////////////////////////////////////////

BOOL
WINAPI
DllMain(
HMODULE hInstDLL,
DWORD dwReason,
LPVOID lpvReserved)
{
    UNREFERENCED_PARAMETER(lpvReserved);

    if (dwReason == DLL_PROCESS_ATTACH) {
        dllHandle = (HINSTANCE)hInstDLL;
        LogInit();

#if _WIN64
        LogTrace(LOGTYPE_INFO, "GetKeyStorageInterface", "Loaded 64-bit dll");
#else
        LogTrace(LOGTYPE_INFO, "GetKeyStorageInterface", "Loaded 32-bit dll");
#endif

    }
    else if (dwReason == DLL_PROCESS_DETACH) {
        if (g_hRSAProvider)
        {
            BCryptCloseAlgorithmProvider(g_hRSAProvider, 0);
        }
    }
    return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
/******************************************************************************
* DESCRIPTION :     Get the CMD KSP key storage Interface function
*                   dispatch table
*
* INPUTS :
*            LPCWSTR pszProviderName        Name of the provider (unused)
*            DWORD   dwFlags                Flags (unused)
* OUTPUTS :
*            char    **ppFunctionTable      The key storage interface function
*                                           dispatch table
* RETURN :
*            ERROR_SUCCESS                  The function was successful.
*/
NTSTATUS
WINAPI
GetKeyStorageInterface(
__in   LPCWSTR pszProviderName,
__out  NCRYPT_KEY_STORAGE_FUNCTION_TABLE **ppFunctionTable,
__in   DWORD dwFlags)
{

    UNREFERENCED_PARAMETER(pszProviderName);
    UNREFERENCED_PARAMETER(dwFlags);

    LogTrace(LOGTYPE_INFO, "GetKeyStorageInterface", "Call GetKeyStorageInterface");

    *ppFunctionTable = &KSPFunctionTable;

    return ERROR_SUCCESS;
}

/*******************************************************************
* DESCRIPTION :     Load and initialize the CMD KSP provider
*
* INPUTS :
*            LPCWSTR pszProviderName         Name of the provider
*            DWORD   dwFlags                 Flags (unused)
* OUTPUTS :
*            NCRYPT_PROV_HANDLE *phProvider  The provider handle
* RETURN :
*            ERROR_SUCCESS                   The function was successful.
*            NTE_INVALID_PARAMETER           One or more parameters are invalid.
*            NTE_NO_MEMORY                   A memory allocation failure occurred.
*/
SECURITY_STATUS
WINAPI
KSPOpenProvider(
__out   NCRYPT_PROV_HANDLE *phProvider,
__in    LPCWSTR pszProviderName,
__in    DWORD   dwFlags)
{
    SECURITY_STATUS status = NTE_INTERNAL_ERROR;
    CMDKSP_PROVIDER *pProvider = NULL;
    DWORD cbLength = 0;
    size_t cbProviderName = 0;
    UNREFERENCED_PARAMETER(dwFlags);

    LogTrace(LOGTYPE_INFO, "KSPOpenProvider", "Call KSPOpenProvider");

    // Validate input parameters.
    if (phProvider == NULL)
    {
        status = NTE_INVALID_PARAMETER;
        goto cleanup;
    }
    if (pszProviderName == NULL)
    {
        status = NTE_INVALID_PARAMETER;
        goto cleanup;
    }

    //The size of the provider name should be limited.
    cbProviderName = (wcslen(pszProviderName) + 1) * sizeof(WCHAR);
    if (cbProviderName > MAXUSHORT)
    {
        status = NTE_INVALID_PARAMETER;
        goto cleanup;
    }

    // Allocate memory for provider object.
    cbLength = sizeof(CMDKSP_PROVIDER) + (DWORD)cbProviderName;
    pProvider = (CMDKSP_PROVIDER*)HeapAlloc(GetProcessHeap(), 0, cbLength);;
    if (NULL == pProvider)
    {
        status = NTE_NO_MEMORY;
        goto cleanup;
    }

    //Assign values to fields of the provider handle.
    pProvider->cbLength = cbLength;
    pProvider->dwMagic = CMDKSP_PROVIDER_MAGIC;
    pProvider->dwFlags = 0;
    pProvider->pszName = (LPWSTR)(pProvider + 1);
    CopyMemory(pProvider->pszName, pszProviderName, cbProviderName);
    pProvider->pszContext = NULL;

    //Assign the output value.
    *phProvider = (NCRYPT_PROV_HANDLE)pProvider;
    pProvider = NULL;
    status = ERROR_SUCCESS;
cleanup:
    if (pProvider)
    {
        HeapFree(GetProcessHeap(), 0, pProvider);
    }

    return status;
}


/******************************************************************************
* DESCRIPTION :     Release a handle to the CMD KSP provider
*
* INPUTS :
*            NCRYPT_PROV_HANDLE hProvider    A handle to the CMD KSP provider
* RETURN :
*            ERROR_SUCCESS                   The function was successful.
*            NTE_INVALID_HANDLE              The handle is not a valid CMD KSP
*                                            provider handle.
*/
SECURITY_STATUS
WINAPI
KSPFreeProvider(
__in    NCRYPT_PROV_HANDLE hProvider)
{
    SECURITY_STATUS Status = NTE_INTERNAL_ERROR;
    CMDKSP_PROVIDER *pProvider = NULL;

    LogTrace(LOGTYPE_INFO, "KSPFreeProvider", "Call KSPFreeProvider");

    // Validate input parameters.
    pProvider = CmdKspValidateProvHandle(hProvider);

    if (pProvider == NULL)
    {
        Status = NTE_INVALID_HANDLE;
        goto cleanup;
    }

    // Free context.
    if (pProvider->pszContext)
    {
        HeapFree(GetProcessHeap(), 0, pProvider->pszContext);
        pProvider->pszContext = NULL;
    }

    ZeroMemory(pProvider, pProvider->cbLength);
    HeapFree(GetProcessHeap(), 0, pProvider);

    Status = ERROR_SUCCESS;
cleanup:

    return Status;
}


/******************************************************************************
* DESCRIPTION :     Open a key in the CMD key storage provider
*
* INPUTS :
*            NCRYPT_PROV_HANDLE hProvider    A handle to the CMD KSP provider
*            LPCWSTR pszKeyName              Name of the key
DWORD  dwLegacyKeySpec          Flags for legacy key support (unused)
*            DWORD   dwFlags                 Flags (unused)
* OUTPUTS:
*            NCRYPT_KEY_HANDLE               A handle to the opened key
* RETURN :
*            ERROR_SUCCESS                   The function was successful.
*            NTE_INVALID_HANDLE              The handle is not a valid CMD KSP
*                                            provider handle.
*            NTE_INVALID_PARAMETER           One or more parameters are invalid.
*            NTE_NO_MEMORY                   A memory allocation failure occurred.
*/

SECURITY_STATUS
WINAPI
KSPOpenKey(
__inout NCRYPT_PROV_HANDLE hProvider,
__out   NCRYPT_KEY_HANDLE *phKey,
__in    LPCWSTR pszKeyName,
__in_opt DWORD  dwLegacyKeySpec,
__in    DWORD   dwFlags)
{
    SECURITY_STATUS Status = NTE_INTERNAL_ERROR;
    CMDKSP_PROVIDER *pProvider = NULL;
    CMDKSP_KEY *pKey = NULL;

    LogTrace(LOGTYPE_INFO, "KSPOpenKey", "Call KSPOpenKey");
    LogTrace(LOGTYPE_INFO, "KSPOpenKey", "pProvider = [0x%08x]", pProvider);
    LogTrace(LOGTYPE_INFO, "KSPOpenKey", "phKey = [0x%08x] pszKeyName = [0x%08x]", phKey, pszKeyName);
    LogTrace(LOGTYPE_INFO, "KSPOpenKey", "Credential ID = %S", pszKeyName);

    //
    // Validate input parameters.
    //
    UNREFERENCED_PARAMETER(dwLegacyKeySpec);
    UNREFERENCED_PARAMETER(dwFlags);

    pProvider = CmdKspValidateProvHandle(hProvider);

    if (pProvider == NULL)
    {
        Status = NTE_INVALID_HANDLE;
        goto cleanup;
    }

    if ((phKey == NULL) || (pszKeyName == NULL))
    {
        Status = NTE_INVALID_PARAMETER;
        goto cleanup;
    }

    //Initialize the key object.
    Status = CreateNewKeyObject(pszKeyName, &pKey);
    if (Status != ERROR_SUCCESS)
    {
        goto cleanup;
    }

    pKey->fFinished = TRUE;
    *phKey = (NCRYPT_KEY_HANDLE)pKey;
    pKey = NULL;
    Status = ERROR_SUCCESS;

cleanup:

    if (pKey)
    {
        DeleteKeyObject(pKey);
    }

    return Status;
}


/******************************************************************************
* DESCRIPTION :     Create a new key and stored it into the user profile
*                   key storage area
*/
SECURITY_STATUS
WINAPI
KSPCreatePersistedKey(
__in    NCRYPT_PROV_HANDLE hProvider,
__out   NCRYPT_KEY_HANDLE *phKey,
__in    LPCWSTR pszAlgId,
__in_opt LPCWSTR pszKeyName,
__in    DWORD   dwLegacyKeySpec,
__in    DWORD   dwFlags)
{
    LogTrace(LOGTYPE_INFO, "KSPCreatePersistedKey", "Call KSPCreatePersistedKey - NTE_NOT_SUPPORTED");

    return NTE_NOT_SUPPORTED;

}

/******************************************************************************
* DESCRIPTION :  Retrieves the value of a named property for a key storage
*                provider object.
*
* INPUTS :
*            NCRYPT_PROV_HANDLE hProvider    A handle to the CMD KSP provider
*            LPCWSTR pszProperty             Name of the property
*            DWORD   cbOutput                Size of the output buffer
*            DWORD   dwFlags                 Flags
* OUTPUTS:
*            PBYTE   pbOutput                Output buffer containing the value
*                                            of the property.  If pbOutput is NULL,
*                                            required buffer size will return in
*                                            *pcbResult.
*            DWORD * pcbResult               Required size of the output buffer
* RETURN :
*            ERROR_SUCCESS                   The function was successful.
*            NTE_INVALID_HANDLE              The handle is not a valid CMD KSP
*                                            provider handle.
*            NTE_NOT_FOUND                   Cannot find such a property.
*            NTE_INVALID_PARAMETER           One or more parameters are invalid.
*            NTE_BUFFER_TOO_SMALL            Output buffer is too small.
*            NTE_NOT_SUPPORTED               The property is not supported.
*            NTE_BAD_FLAGS                   dwFlags contains invalid value.
*/
SECURITY_STATUS
WINAPI
KSPGetProviderProperty(
__in    NCRYPT_PROV_HANDLE hProvider,
__in    LPCWSTR pszProperty,
__out_bcount_part_opt(cbOutput, *pcbResult) PBYTE pbOutput,
__in    DWORD   cbOutput,
__out   DWORD * pcbResult,
__in    DWORD   dwFlags)
{

    SECURITY_STATUS Status = NTE_INTERNAL_ERROR;
    CMDKSP_PROVIDER *pProvider = NULL;
    DWORD cbResult = 0;
    DWORD dwProperty = 0;

    LogTrace(LOGTYPE_INFO, "KSPGetProviderProperty", "Call KSPGetProviderProperty");

    //
    // Validate input parameters.
    //
    pProvider = CmdKspValidateProvHandle(hProvider);

    if (pProvider == NULL)
    {
        Status = NTE_INVALID_HANDLE;
        goto cleanup;
    }

    if ((pszProperty == NULL) || (pcbResult == NULL))
    {
        Status = NTE_INVALID_PARAMETER;
        goto cleanup;
    }

    if (wcslen(pszProperty) > NCRYPT_MAX_PROPERTY_NAME)
    {
        Status = NTE_INVALID_PARAMETER;
        goto cleanup;
    }


    if ((dwFlags & ~(NCRYPT_SILENT_FLAG)) != 0)
    {
        Status = NTE_BAD_FLAGS;
        goto cleanup;
    }

    //
    //Determine the size of the properties.
    //

    if (wcscmp(pszProperty, NCRYPT_IMPL_TYPE_PROPERTY) == 0)
    {
        dwProperty = CMDKSP_IMPL_TYPE_PROPERTY;
        cbResult = sizeof(DWORD);
    }
    else if (wcscmp(pszProperty, NCRYPT_MAX_NAME_LENGTH_PROPERTY) == 0)
    {
        dwProperty = CMDKSP_MAX_NAME_LEN_PROPERTY;
        cbResult = sizeof(DWORD);
    }
    else if (wcscmp(pszProperty, NCRYPT_NAME_PROPERTY) == 0)
    {
        dwProperty = CMDKSP_NAME_PROPERTY;
        cbResult = (DWORD)((wcslen(pProvider->pszName) + 1) * sizeof(WCHAR));
    }
    else if (wcscmp(pszProperty, NCRYPT_VERSION_PROPERTY) == 0)
    {
        dwProperty = CMDKSP_VERSION_PROPERTY;
        cbResult = sizeof(DWORD);
    }
    else if (wcscmp(pszProperty, NCRYPT_USE_CONTEXT_PROPERTY) == 0)
    {
        dwProperty = CMDKSP_USE_CONTEXT_PROPERTY;
        cbResult = 0;

        if (pProvider->pszContext)
        {
            cbResult =
                (DWORD)(wcslen(pProvider->pszContext) + 1) * sizeof(WCHAR);
        }

        if (cbResult == 0)
        {
            goto cleanup;
        }
    }
    else if (wcscmp(pszProperty, NCRYPT_SECURITY_DESCR_SUPPORT_PROPERTY) == 0)
    {
        dwProperty = CMDKSP_SECURITY_DESCR_SUPPORT_PROPERTY;
        cbResult = sizeof(DWORD);
    }
    else
    {
        Status = NTE_NOT_SUPPORTED;
        goto cleanup;
    }

    *pcbResult = cbResult;

    //Output buffer is empty, this is a property length query, and we can exit early.
    if (pbOutput == NULL)
    {
        Status = ERROR_SUCCESS;
        goto cleanup;
    }

    //Otherwise, validate the size.
    if (cbOutput < *pcbResult)
    {
        Status = NTE_BUFFER_TOO_SMALL;
        goto cleanup;
    }

    //
    //Retrieve the requested property data
    //if the property is not supported, we have already returned NTE_NOT_SUPPORTED.
    //
    switch (dwProperty)
    {
    case CMDKSP_IMPL_TYPE_PROPERTY:
        *(DWORD *)pbOutput = NCRYPT_IMPL_SOFTWARE_FLAG;
        break;

    case CMDKSP_MAX_NAME_LEN_PROPERTY:
        *(DWORD *)pbOutput = MAX_PATH;
        break;

    case CMDKSP_NAME_PROPERTY:
        CopyMemory(pbOutput, pProvider->pszName, cbResult);
        break;

    case CMDKSP_VERSION_PROPERTY:
        *(DWORD *)pbOutput = CMDKSP_VERSION;
        break;

    case CMDKSP_USE_CONTEXT_PROPERTY:
        CopyMemory(pbOutput, pProvider->pszContext, cbResult);
        break;

    case CMDKSP_SECURITY_DESCR_SUPPORT_PROPERTY:
        *(DWORD *)pbOutput = CMDKSP_SUPPORT_SECURITY_DESCRIPTOR;
        break;
    }

    Status = ERROR_SUCCESS;

cleanup:

    return Status;
}

/******************************************************************************
* DESCRIPTION :  Retrieves the value of a named property for a key storage
*                key object.
*
* INPUTS :
*            NCRYPT_PROV_HANDLE hProvider    A handle to a CMD KSP provider
*                                            object
*            NCRYPT_KEY_HANDLE hKey          A handle to a CMD KSP key object
*            LPCWSTR pszProperty             Name of the property
*            DWORD   cbOutput                Size of the output buffer
*            DWORD   dwFlags                 Flags
* OUTPUTS:
*            PBYTE   pbOutput                Output buffer containing the value
*                                            of the property.  If pbOutput is NULL,
*                                            required buffer size will return in
*                                            *pcbResult.
*            DWORD * pcbResult               Required size of the output buffer
* RETURN :
*            ERROR_SUCCESS                   The function was successful.
*            NTE_INVALID_HANDLE              The handle is not a valid CMD KSP
*                                            provider handle.
*            NTE_NOT_FOUND                   Cannot find such a property.
*            NTE_INVALID_PARAMETER           One or more parameters are invalid.
*            NTE_BUFFER_TOO_SMALL            Output buffer is too small.
*            NTE_NOT_SUPPORTED               The property is not supported.
*            NTE_BAD_FLAGS                   dwFlags contains invalid value.
*/
SECURITY_STATUS
WINAPI
KSPGetKeyProperty(
__in    NCRYPT_PROV_HANDLE hProvider,
__in    NCRYPT_KEY_HANDLE hKey,
__in    LPCWSTR pszProperty,
__out_bcount_part_opt(cbOutput, *pcbResult) PBYTE pbOutput,
__in    DWORD   cbOutput,
__out   DWORD * pcbResult,
__in    DWORD   dwFlags)
{

    SECURITY_STATUS Status = NTE_INTERNAL_ERROR;
    CMDKSP_PROVIDER *pProvider = NULL;
    CMDKSP_KEY *pKey = NULL;
    DWORD dwProperty = 0;
    DWORD cbResult = 0;
    LPWSTR pszAlgorithm = NULL;
    LPWSTR pszAlgorithmGroup = NULL;
    PBYTE pbSecurityInfo = NULL;
    DWORD cbSecurityInfo = 0;
    DWORD cbTmp = 0;

    LogTrace(LOGTYPE_INFO, "KSPGetKeyProperty", "Call KSPGetKeyProperty");

    //
    // Validate input parameters.
    //
    pProvider = CmdKspValidateProvHandle(hProvider);

    if (pProvider == NULL)
    {
        Status = NTE_INVALID_HANDLE;
        goto cleanup;
    }

    pKey = CmdKspValidateKeyHandle(hKey);

    if (pKey == NULL)
    {
        Status = NTE_INVALID_HANDLE;
        goto cleanup;
    }

    if ((pszProperty == NULL) ||
        (wcslen(pszProperty) > NCRYPT_MAX_PROPERTY_NAME) ||
        (pcbResult == NULL))
    {
        Status = NTE_INVALID_PARAMETER;
        goto cleanup;
    }

    //NCRYPT_SILENT_FLAG is ignored in this KSP.
    dwFlags &= ~NCRYPT_SILENT_FLAG;

    //If this is to get the security descriptor, the flags
    //must be one of the OWNER_SECURITY_INFORMATION |GROUP_SECURITY_INFORMATION |
    //DACL_SECURITY_INFORMATION|LABEL_SECURITY_INFORMATION | SACL_SECURITY_INFORMATION.
    if (wcscmp(pszProperty, NCRYPT_SECURITY_DESCR_PROPERTY) == 0)
    {

        if ((dwFlags == 0) || ((dwFlags & ~(OWNER_SECURITY_INFORMATION |
            GROUP_SECURITY_INFORMATION |
            DACL_SECURITY_INFORMATION |
            LABEL_SECURITY_INFORMATION |
            SACL_SECURITY_INFORMATION)) != 0))
        {
            Status = NTE_BAD_FLAGS;
            goto cleanup;
        }
    }
    else
    {
        //Otherwise,Only NCRYPT_PERSIST_ONLY_FLAG is a valid flag.
        if (dwFlags & ~NCRYPT_PERSIST_ONLY_FLAG)
        {
            Status = NTE_BAD_FLAGS;
            goto cleanup;
        }
    }

    //
    //Determine length of requested property.
    //
    if (wcscmp(pszProperty, NCRYPT_ALGORITHM_PROPERTY) == 0)
    {
        dwProperty = CMDKSP_ALGORITHM_PROPERTY;
        pszAlgorithm = BCRYPT_RSA_ALGORITHM;
        cbResult = (DWORD)(wcslen(pszAlgorithm) + 1) * sizeof(WCHAR);
    }
    else if (wcscmp(pszProperty, NCRYPT_BLOCK_LENGTH_PROPERTY) == 0)
    {
        dwProperty = CMDKSP_BLOCK_LENGTH_PROPERTY;
        cbResult = sizeof(DWORD);
    }
    else if (wcscmp(pszProperty, NCRYPT_EXPORT_POLICY_PROPERTY) == 0)
    {
        dwProperty = CMDKSP_EXPORT_POLICY_PROPERTY;
        cbResult = sizeof(DWORD);
    }
    else if (wcscmp(pszProperty, NCRYPT_KEY_USAGE_PROPERTY) == 0)
    {
        dwProperty = CMDKSP_KEY_USAGE_PROPERTY;
        cbResult = sizeof(DWORD);
    }
    else if (wcscmp(pszProperty, NCRYPT_KEY_TYPE_PROPERTY) == 0)
    {
        dwProperty = CMDKSP_KEY_TYPE_PROPERTY;
        cbResult = sizeof(DWORD);
    }
    else if (wcscmp(pszProperty, NCRYPT_LENGTH_PROPERTY) == 0)
    {
        dwProperty = CMDKSP_LENGTH_PROPERTY;
        cbResult = sizeof(DWORD);
    }
    else if (wcscmp(pszProperty, NCRYPT_LENGTHS_PROPERTY) == 0)
    {
        dwProperty = CMDKSP_LENGTHS_PROPERTY;
        cbResult = sizeof(NCRYPT_SUPPORTED_LENGTHS);
    }
    else if (wcscmp(pszProperty, NCRYPT_NAME_PROPERTY) == 0)
    {
        dwProperty = CMDKSP_NAME_PROPERTY;
        if (pKey->pszKeyName == NULL)
        {
            // This should only happen if this is an ephemeral key.
            Status = NTE_NOT_SUPPORTED;
            goto cleanup;
        }
        cbResult = (DWORD)(wcslen(pKey->pszKeyName) + 1) * sizeof(WCHAR);
    }
    else if (wcscmp(pszProperty, NCRYPT_ALGORITHM_GROUP_PROPERTY) == 0)
    {
        dwProperty = CMDKSP_ALGORITHM_GROUP_PROPERTY;
        pszAlgorithmGroup = NCRYPT_RSA_ALGORITHM_GROUP;
        cbResult = (DWORD)(wcslen(pszAlgorithmGroup) + 1) * sizeof(WCHAR);
    }
    else if (wcscmp(pszProperty, NCRYPT_UNIQUE_NAME_PROPERTY) == 0)
    {
        //For this sample, the unique name property and the name property are
        //the same, which is the name of the key file.
        dwProperty = CMDKSP_UNIQUE_NAME_PROPERTY;

        if (pKey->pszKeyName == NULL)
        {
            // This should only happen if this is a public key object.
            Status = NTE_NOT_SUPPORTED;
            goto cleanup;
        }

        cbResult = (DWORD)(wcslen(pKey->pszKeyName) + 1) * sizeof(WCHAR);
    }
    else
    {
        Status = NTE_NOT_SUPPORTED;
        goto cleanup;
    }


    //
    // Validate the size of the output buffer.
    //

    *pcbResult = cbResult;

    if (pbOutput == NULL)
    {
        Status = ERROR_SUCCESS;
        goto cleanup;
    }

    if (cbOutput < *pcbResult)
    {
        Status = NTE_BUFFER_TOO_SMALL;
        goto cleanup;
    }

    //
    // Retrieve the requested property data.
    //
    switch (dwProperty)
    {
    case CMDKSP_ALGORITHM_PROPERTY:
        CopyMemory(pbOutput, pszAlgorithm, cbResult);
        break;

    case CMDKSP_BLOCK_LENGTH_PROPERTY:
        *(DWORD *)pbOutput = (pKey->dwKeyBitLength + 7) / 8;
        break;

    case CMDKSP_EXPORT_POLICY_PROPERTY:
        *(DWORD *)pbOutput = pKey->dwExportPolicy;
        break;

    case CMDKSP_KEY_USAGE_PROPERTY:
        *(DWORD *)pbOutput = pKey->dwKeyUsagePolicy;
        break;

    case CMDKSP_KEY_TYPE_PROPERTY:
        //This sample KSP does not support machine keys.
        *(DWORD *)pbOutput = 0;
        break;

    case CMDKSP_LENGTH_PROPERTY:
        *(DWORD *)pbOutput = pKey->dwKeyBitLength;
        break;

    case CMDKSP_LENGTHS_PROPERTY:
    {
        NCRYPT_SUPPORTED_LENGTHS UNALIGNED *pLengths = (NCRYPT_SUPPORTED_LENGTHS *)pbOutput;

        Status = BCryptGetProperty(pKey->hProvider,
            BCRYPT_KEY_LENGTHS,
            pbOutput,
            cbOutput,
            &cbTmp,
            0);
        if (ERROR_SUCCESS != Status)
        {
            goto cleanup;
        }

        pLengths->dwDefaultLength = CMDKSP_DEFAULT_KEY_LENGTH;
        break;
    }

    case CMDKSP_NAME_PROPERTY:
        CopyMemory(pbOutput, pKey->pszKeyName, cbResult);
        break;

    case CMDKSP_UNIQUE_NAME_PROPERTY:
        CopyMemory(pbOutput, pKey->pszKeyName, cbResult);
        break;

    case CMDKSP_SECURITY_DESCR_PROPERTY:
        CopyMemory(pbOutput, pbSecurityInfo, cbResult);
        break;

    case CMDKSP_ALGORITHM_GROUP_PROPERTY:
        CopyMemory(pbOutput, pszAlgorithmGroup, cbResult);
        break;

    }

    Status = ERROR_SUCCESS;

cleanup:

    if (pbSecurityInfo)
    {
        HeapFree(GetProcessHeap(), 0, pbSecurityInfo);
    }

    return Status;
}

/******************************************************************************
* DESCRIPTION :  Sets the value for a named property for a CNG key storage
*                provider object.
*
* INPUTS :
*            NCRYPT_PROV_HANDLE hProvider    A handle to the CMD KSP provider
*            LPCWSTR pszProperty             Name of the property
*            PBYTE   pbInput                 Input buffer containing the value
*                                            of the property
*            DWORD   cbOutput                Size of the input buffer
*            DWORD   dwFlags                 Flags
*
* RETURN :
*            ERROR_SUCCESS                   The function was successful.
*            NTE_INVALID_HANDLE              The handle is not a valid CMD KSP
*                                            provider handle.
*            NTE_INVALID_PARAMETER           One or more parameters are invalid.
*            NTE_NOT_SUPPORTED               The property is not supported.
*            NTE_BAD_FLAGS                   dwFlags contains invalid value.
*            NTE_NO_MEMORY                   A memory allocation failure occurred.
*/
SECURITY_STATUS
WINAPI
KSPSetProviderProperty(
__in    NCRYPT_PROV_HANDLE hProvider,
__in    LPCWSTR pszProperty,
__in_bcount(cbInput) PBYTE pbInput,
__in    DWORD   cbInput,
__in    DWORD   dwFlags)
{
    SECURITY_STATUS Status = NTE_INTERNAL_ERROR;
    CMDKSP_PROVIDER *pProvider = NULL;

    LogTrace(LOGTYPE_INFO, "KSPSetProviderProperty", "Call KSPSetProviderProperty");

    // Validate input parameters.
    pProvider = CmdKspValidateProvHandle(hProvider);

    if (pProvider == NULL)
    {
        Status = NTE_INVALID_HANDLE;
        goto cleanup;
    }

    if ((pszProperty == NULL) ||
        (wcslen(pszProperty) > NCRYPT_MAX_PROPERTY_NAME) ||
        (pbInput == NULL))
    {
        Status = NTE_INVALID_PARAMETER;
        goto cleanup;
    }

    if ((dwFlags & ~(NCRYPT_SILENT_FLAG)) != 0)
    {
        Status = NTE_BAD_FLAGS;
        goto cleanup;
    }

    //Update the property.
    if (wcscmp(pszProperty, NCRYPT_USE_CONTEXT_PROPERTY) == 0)
    {

        if (pProvider->pszContext)
        {
            HeapFree(GetProcessHeap(), 0, pProvider->pszContext);
        }

        pProvider->pszContext = (LPWSTR)HeapAlloc(GetProcessHeap(), 0, cbInput);
        if (pProvider->pszContext == NULL)
        {
            Status = NTE_NO_MEMORY;
            goto cleanup;
        }

        CopyMemory(pProvider->pszContext, pbInput, cbInput);

    }
    else
    {
        Status = NTE_NOT_SUPPORTED;
        goto cleanup;
    }


    Status = ERROR_SUCCESS;

cleanup:
    return Status;
}

/******************************************************************************
* DESCRIPTION :  Set the value of a named property for a key storage
*                key object.
*
* INPUTS :
*            NCRYPT_PROV_HANDLE hProvider    A handle to a CMD KSP provider
*                                            object
*            NCRYPT_KEY_HANDLE hKey          A handle to a CMD KSP key object
*            LPCWSTR pszProperty             Name of the property
*            PBYTE   pbInput                 Input buffer containing the value
*                                            of the property
*            DWORD   cbOutput                Size of the input buffer
*            DWORD   dwFlags                 Flags
*
* RETURN :
*            ERROR_SUCCESS                   The function was successful.
*            NTE_INVALID_HANDLE              The handle is not a valid CMD KSP
*                                            provider handle or a valid key handle
*            NTE_INVALID_PARAMETER           One or more parameters are invalid.
*            NTE_NOT_SUPPORTED               The property is not supported.
*            NTE_BAD_FLAGS                   dwFlags contains invalid value.
*            NTE_NO_MEMORY                   A memory allocation failure occurred.
*/
SECURITY_STATUS
WINAPI
KSPSetKeyProperty(
__in    NCRYPT_PROV_HANDLE hProvider,
__inout NCRYPT_KEY_HANDLE hKey,
__in    LPCWSTR pszProperty,
__in_bcount(cbInput) PBYTE pbInput,
__in    DWORD   cbInput,
__in    DWORD   dwFlags)
{

    SECURITY_STATUS         Status = NTE_INTERNAL_ERROR;
    CMDKSP_PROVIDER      *pProvider = NULL;
    CMDKSP_KEY           *pKey = NULL;
    DWORD                   dwTempFlags = dwFlags;

    LogTrace(LOGTYPE_INFO, "KSPSetKeyProperty", "Call KSPSetKeyProperty");
    LogTrace(LOGTYPE_INFO, "KSPSetKeyProperty", "pszProperty=%S", pszProperty);

    // Validate input parameters.
    pProvider = CmdKspValidateProvHandle(hProvider);

    if (pProvider == NULL)
    {
        Status = NTE_INVALID_HANDLE;
        goto cleanup;
    }

    pKey = CmdKspValidateKeyHandle(hKey);

    if (pKey == NULL)
    {
        Status = NTE_INVALID_HANDLE;
        goto cleanup;
    }

    if ((pszProperty == NULL) ||
        (wcslen(pszProperty) > NCRYPT_MAX_PROPERTY_NAME) ||
        (pbInput == NULL))
    {
        Status = NTE_INVALID_PARAMETER;
        goto cleanup;
    }

    if (wcscmp(L"HWND Handle", pszProperty) == 0) {
        pKey->hWnd = *((HWND *)pbInput);

        if (IsWindow((HWND)pKey->hWnd) == 0) {
            pKey->hWnd = NULL;
            LogTrace(LOGTYPE_WARNING, "KSPSetKeyProperty", "IsWindow fail");
            goto cleanup;
        }
        LogTrace(LOGTYPE_INFO, "KSPSetKeyProperty", "HWND Handle = [0x%08x]", pKey->hWnd);
    }
    else {
        LogTrace(LOGTYPE_WARNING, "KSPSetKeyProperty", "KSPSetProperty called for unsupported property - doing nothing...");
    }

    Status = ERROR_SUCCESS;

cleanup:
    return Status;
}

/******************************************************************************
* DESCRIPTION :     Completes a CMD key storage key. The key cannot be used
*                   until this function has been called.
*/
SECURITY_STATUS
WINAPI
KSPFinalizeKey(
__in    NCRYPT_PROV_HANDLE hProvider,
__in    NCRYPT_KEY_HANDLE hKey,
__in    DWORD   dwFlags)
{

    LogTrace(LOGTYPE_INFO, "KSPFinalizeKey", "Call KSPFinalizeKey NTE_NOT_SUPPORTED");

    return NTE_NOT_SUPPORTED;
}

/******************************************************************************
* DESCRIPTION :     Deletes a CNG CMD KSP key

*/
SECURITY_STATUS
WINAPI
KSPDeleteKey(
__in    NCRYPT_PROV_HANDLE hProvider,
__inout NCRYPT_KEY_HANDLE hKey,
__in    DWORD   dwFlags)
{

    LogTrace(LOGTYPE_INFO, "KSPDeleteKey", "Call KSPDeleteKey NTE_NOT_SUPPORTED");

    return NTE_NOT_SUPPORTED;
}

/******************************************************************************
* DESCRIPTION :     Free a CNG CMD KSP key object
*
* INPUTS :
*            NCRYPT_PROV_HANDLE hProvider    A handle to the CMD KSP provider
*            NCRYPT_KEY_HANDLE hKey          A handle to a CMD KSP key
* RETURN :
*            ERROR_SUCCESS                   The function was successful.
*            NTE_INVALID_HANDLE              The handle is not a valid CMD KSP
*/
SECURITY_STATUS
WINAPI
KSPFreeKey(
__in    NCRYPT_PROV_HANDLE hProvider,
__in    NCRYPT_KEY_HANDLE hKey)
{
    SECURITY_STATUS Status;
    CMDKSP_PROVIDER *pProvider;
    CMDKSP_KEY *pKey = NULL;

    LogTrace(LOGTYPE_INFO, "KSPFreeKey", "Call KSPFreeKey");

    // Validate input parameters.
    pProvider = CmdKspValidateProvHandle(hProvider);

    if (pProvider == NULL)
    {
        Status = NTE_INVALID_HANDLE;
        goto cleanup;
    }

    pKey = CmdKspValidateKeyHandle(hKey);

    if (pKey == NULL)
    {
        Status = NTE_INVALID_HANDLE;
        goto cleanup;
    }


    //
    // Free key object.
    //
    Status = DeleteKeyObject(pKey);

cleanup:

    return Status;
}

/******************************************************************************
* DESCRIPTION :     free a CNG CMD KSP memory buffer object
*
* INPUTS :
*            PVOID   pvInput                 The buffer to free.
* RETURN :
*            ERROR_SUCCESS                   The function was successful.
*/
SECURITY_STATUS
WINAPI
KSPFreeBuffer(
__deref PVOID   pvInput)
{

    LogTrace(LOGTYPE_INFO, "KSPFreeBuffer", "Call KSPFreeBuffer");
    //
    // Free the buffer from the heap.
    //

    HeapFree(GetProcessHeap(), 0, pvInput);

    return ERROR_SUCCESS;
}


/******************************************************************************
* DESCRIPTION :  encrypts a block of data.
*/
SECURITY_STATUS
WINAPI
KSPEncrypt(
__in    NCRYPT_PROV_HANDLE hProvider,
__in    NCRYPT_KEY_HANDLE hKey,
__in_bcount(cbInput) PBYTE pbInput,
__in    DWORD   cbInput,
__in    VOID *pPaddingInfo,
__out_bcount_part_opt(cbOutput, *pcbResult) PBYTE pbOutput,
__in    DWORD   cbOutput,
__out   DWORD * pcbResult,
__in    DWORD   dwFlags)
{
    LogTrace(LOGTYPE_INFO, "KSPEncrypt", "Call KSPEncrypt NTE_NOT_SUPPORTED");

    return NTE_NOT_SUPPORTED;
}

/******************************************************************************
* DESCRIPTION :  Decrypts a block of data.

*/

SECURITY_STATUS
WINAPI
KSPDecrypt(
__in    NCRYPT_PROV_HANDLE hProvider,
__in    NCRYPT_KEY_HANDLE hKey,
__in_bcount(cbInput) PBYTE pbInput,
__in    DWORD   cbInput,
__in    VOID *pPaddingInfo,
__out_bcount_part_opt(cbOutput, *pcbResult) PBYTE pbOutput,
__in    DWORD   cbOutput,
__out   DWORD * pcbResult,
__in    DWORD   dwFlags)
{
    LogTrace(LOGTYPE_INFO, "KSPDecrypt", "Call KSPDecrypt NTE_NOT_SUPPORTED");

    return NTE_NOT_SUPPORTED;
}

/******************************************************************************
* DESCRIPTION :  Determines if a CMD key storage provider supports a
*                specific cryptographic algorithm.
*
* INPUTS :
*            NCRYPT_PROV_HANDLE hProvider    A handle to a CMD KSP provider
*                                            object
*            LPCWSTR pszAlgId                Name of the cryptographic
*                                            Algorithm in question
*            DWORD   dwFlags                 Flags
* RETURN :
*            ERROR_SUCCESS                   The algorithm is supported.
*            NTE_INVALID_HANDLE              The handle is not a valid CMD KSP
*                                            provider or key handle.
*            NTE_INVALID_PARAMETER           One or more parameters are invalid.
*            NTE_BAD_FLAGS                   dwFlags contains invalid value.
*            NTE_NOT_SUPPORTED               This algorithm is not supported.
*/
SECURITY_STATUS
WINAPI
KSPIsAlgSupported(
__in    NCRYPT_PROV_HANDLE hProvider,
__in    LPCWSTR pszAlgId,
__in    DWORD   dwFlags)
{
    CMDKSP_PROVIDER *pProvider = NULL;
    SECURITY_STATUS Status = NTE_INTERNAL_ERROR;

    LogTrace(LOGTYPE_INFO, "KSPIsAlgSupported", "Call KSPIsAlgSupported");

    // Validate input parameters.
    pProvider = CmdKspValidateProvHandle(hProvider);

    if (pProvider == NULL)
    {
        Status = NTE_INVALID_HANDLE;
        goto cleanup;
    }

    if (pszAlgId == NULL)
    {
        Status = NTE_INVALID_PARAMETER;
        goto cleanup;
    }

    if ((dwFlags & ~NCRYPT_SILENT_FLAG) != 0)
    {
        Status = NTE_BAD_FLAGS;
        goto cleanup;
    }

    // This KSP only supports the RSA algorithm.
    if (wcscmp(pszAlgId, NCRYPT_RSA_ALGORITHM) != 0)
    {
        Status = NTE_NOT_SUPPORTED;
        goto cleanup;
    }

    Status = ERROR_SUCCESS;
cleanup:
    return Status;
}

/******************************************************************************
* DESCRIPTION :  Obtains the names of the algorithms that are supported by
*                the CMD key storage provider.
*
* INPUTS :
*            NCRYPT_PROV_HANDLE hProvider    A handle to a CMD KSP provider
*                                            object.
*            DWORD   dwAlgOperations         The crypto operations that are to
*                                            be enumerated.
*            DWORD   dwFlags                 Flags
*
* OUTPUTS:
*            DWORD * pdwAlgCount             Number of supported algorithms.
*            NCryptAlgorithmName **ppAlgList List of supported algorithms.
* RETURN :
*            ERROR_SUCCESS                   The function was successful.
*            NTE_INVALID_HANDLE              The handle is not a valid CMD KSP
*                                            provider handle.
*            NTE_INVALID_PARAMETER           One or more parameters are invalid.
*            NTE_BAD_FLAGS                   dwFlags contains invalid value.
*            NTE_NOT_SUPPORTED               The crypto operations are not supported.
*/
SECURITY_STATUS
WINAPI
KSPEnumAlgorithms(
__in    NCRYPT_PROV_HANDLE hProvider,
__in    DWORD   dwAlgOperations,
__out   DWORD * pdwAlgCount,
__deref_out_ecount(*pdwAlgCount) NCryptAlgorithmName **ppAlgList,
__in    DWORD   dwFlags)
{
    SECURITY_STATUS Status = NTE_INTERNAL_ERROR;
    CMDKSP_PROVIDER *pProvider = NULL;
    NCryptAlgorithmName *pCurrentAlg = NULL;
    PBYTE pbCurrent = NULL;
    PBYTE pbOutput = NULL;
    DWORD cbOutput = 0;

    LogTrace(LOGTYPE_INFO, "KSPEnumAlgorithms", "Call KSPEnumAlgorithms");

    // Validate input parameters.
    pProvider = CmdKspValidateProvHandle(hProvider);

    if (pProvider == NULL)
    {
        Status = NTE_INVALID_HANDLE;
        goto cleanup;
    }

    if (pdwAlgCount == NULL || ppAlgList == NULL)
    {
        Status = NTE_INVALID_PARAMETER;
        goto cleanup;
    }

    if ((dwFlags & ~NCRYPT_SILENT_FLAG) != 0)
    {
        Status = NTE_BAD_FLAGS;
        goto cleanup;
    }


    if (dwAlgOperations == 0 ||
        ((dwAlgOperations & NCRYPT_ASYMMETRIC_ENCRYPTION_OPERATION) != 0) ||
        ((dwAlgOperations & NCRYPT_SIGNATURE_OPERATION)) != 0)
    {
        cbOutput += sizeof(NCryptAlgorithmName) +
            sizeof(BCRYPT_RSA_ALGORITHM);
    }
    else
    {
        //KSP only supports RSA.
        Status = NTE_NOT_SUPPORTED;
        goto cleanup;
    }

    //Allocate the output buffer.
    pbOutput = (PBYTE)HeapAlloc(GetProcessHeap(), 0, cbOutput);
    if (pbOutput == NULL)
    {
        Status = NTE_NO_MEMORY;
        goto cleanup;
    }

    pCurrentAlg = (NCryptAlgorithmName *)pbOutput;
    pbCurrent = pbOutput + sizeof(NCryptAlgorithmName);

    pCurrentAlg->dwFlags = 0;
    pCurrentAlg->dwClass = NCRYPT_ASYMMETRIC_ENCRYPTION_INTERFACE;
    pCurrentAlg->dwAlgOperations = NCRYPT_ASYMMETRIC_ENCRYPTION_OPERATION |
        NCRYPT_SIGNATURE_OPERATION;

    pCurrentAlg->pszName = (LPWSTR)pbCurrent;
    CopyMemory(pbCurrent,
        BCRYPT_RSA_ALGORITHM,
        sizeof(BCRYPT_RSA_ALGORITHM));
    pbCurrent += sizeof(BCRYPT_RSA_ALGORITHM);

    *pdwAlgCount = 1;
    *ppAlgList = (NCryptAlgorithmName *)pbOutput;

    Status = ERROR_SUCCESS;

cleanup:

    return Status;
}

/*****************************************************************************
* DESCRIPTION :  Obtains the names of the keys that are stored by the provider.
*/
SECURITY_STATUS
WINAPI
KSPEnumKeys(
__in    NCRYPT_PROV_HANDLE hProvider,
__in_opt LPCWSTR pszScope,
__deref_out NCryptKeyName **ppKeyName,
__inout PVOID * ppEnumState,
__in    DWORD   dwFlags)
{

    LogTrace(LOGTYPE_INFO, "KSPEnumKeys", "Call KSPEnumKeys - NTE_NOT_SUPPORTED");

    return NTE_NOT_SUPPORTED;

}

/******************************************************************************
* DESCRIPTION :  Imports a key into the CMD KSP from a memory BLOB.
*/
SECURITY_STATUS
WINAPI
KSPImportKey(
__in    NCRYPT_PROV_HANDLE hProvider,
__in_opt NCRYPT_KEY_HANDLE hImportKey,
__in    LPCWSTR pszBlobType,
__in_opt NCryptBufferDesc *pParameterList,
__out   NCRYPT_KEY_HANDLE *phKey,
__in_bcount(cbData) PBYTE pbData,
__in    DWORD   cbData,
__in    DWORD   dwFlags)
{
    LogTrace(LOGTYPE_INFO, "KSPImportKey", "Call KSPImportKey NTE_NOT_SUPPORTED");

    return NTE_NOT_SUPPORTED;

}

/******************************************************************************
* DESCRIPTION :  Exports a CMD key storage key into a memory BLOB.
*
* INPUTS :
*            NCRYPT_PROV_HANDLE hProvider     A handle to a CMD KSP provider
*                                             object.
*            NCRYPT_KEY_HANDLE hKey           A handle to the CMD KSP key
*                                             object to export.
*            NCRYPT_KEY_HANDLE hExportKey     Unused
*            LPCWSTR pszBlobType              Type of the key blob.
*            NCryptBufferDesc *pParameterList Additional parameter information.
*            DWORD   cbOutput                 Size of the key blob.
*            DWORD   dwFlags                  Flags
*
* OUTPUTS:
*            PBYTE pbOutput                  Key blob.
*            DWORD * pcbResult               Required size of the key blob.
* RETURN :
*            ERROR_SUCCESS                   The function was successful.
*            NTE_INVALID_HANDLE              The handle is not a valid CMD KSP
*                                            provider handle.
*            NTE_INVALID_PARAMETER           One or more parameters are invalid.
*            NTE_BAD_FLAGS                   dwFlags contains invalid value.
*            NTE_NOT_SUPPORTED               The type of the key blob is not
*                                            supported.
*            NTE_NO_MEMORY                   A memory allocation failure occurred.
*            NTE_INTERNAL_ERROR              Encoding failed.
*/
SECURITY_STATUS
WINAPI
KSPExportKey(__in    NCRYPT_PROV_HANDLE hProvider,
__in    NCRYPT_KEY_HANDLE hKey,
__in_opt NCRYPT_KEY_HANDLE hExportKey,
__in    LPCWSTR pszBlobType,
__in_opt NCryptBufferDesc *pParameterList,
__out_bcount_part_opt(cbOutput, *pcbResult) PBYTE pbOutput,
__in    DWORD   cbOutput,
__out   DWORD * pcbResult,
__in    DWORD   dwFlags)
{
    CMDKSP_PROVIDER        *pProvider = NULL;
    CMDKSP_KEY             *pKey = NULL;
    PBYTE               pbTemp = NULL;
    BOOL                fPublicKeyBlob = FALSE;
    NTSTATUS            ntStatus = STATUS_INTERNAL_ERROR;
    SECURITY_STATUS     Status = NTE_INTERNAL_ERROR;
    UNREFERENCED_PARAMETER(hExportKey);

    LogTrace(LOGTYPE_INFO, "KSPExportKey", "Call KSPExportKey");
    LogTrace(LOGTYPE_INFO, "KSPExportKey", "pszBlobType param= %S", pszBlobType);

    // Validate input parameters.
    pProvider = CmdKspValidateProvHandle(hProvider);
    if (pProvider == NULL)
    {
        Status = NTE_INVALID_HANDLE;
        goto cleanup;
    }
    pKey = CmdKspValidateKeyHandle(hKey);
    if (pKey == NULL)
    {
        Status = NTE_INVALID_HANDLE;
        goto cleanup;
    }
    if (pcbResult == NULL)
    {
        Status = NTE_INVALID_PARAMETER;
        goto cleanup;
    }
    if ((dwFlags & ~(NCRYPT_SILENT_FLAG | NCRYPT_EXPORT_LEGACY_FLAG)) != 0)
    {
        Status = NTE_BAD_FLAGS;
        goto cleanup;
    }
    if (dwFlags & NCRYPT_EXPORT_LEGACY_FLAG)
    {
        Status = NTE_NOT_SUPPORTED;
        goto cleanup;
    }

    //
    // Export public key
    //
    if (wcscmp(pszBlobType, BCRYPT_PUBLIC_KEY_BLOB) == 0 || wcscmp(pszBlobType, BCRYPT_RSAPUBLIC_BLOB) == 0)
    {
        fPublicKeyBlob = TRUE;
    }
    else {
        Status = NTE_NOT_SUPPORTED;
        goto cleanup;
    }

    /*
     * Export the public key blob.
     * This function with RSAPUBLICBLOB BlobType parameter is called by Word and possibly other Office applications
     */
    if (fPublicKeyBlob)
    {
        PCCERT_CONTEXT pCert = pKey->pCert;
        CRYPT_BIT_BLOB publicKeyBlob = pCert->pCertInfo->SubjectPublicKeyInfo.PublicKey;

        BCRYPT_RSAKEY_BLOB publicKey;

        /* publicKeyBlob.pbData is encoded with ASN.1 */
        const int MOD_LEN = pKey->dwKeyBitLength / 8;

        // in both 2048 and 3072 bit keys the offset to the modulus is 9 bytes
        const int MOD_OFFSET = 9;

        // The exponent comes after the modulus. We take the length from the ASN.1.
        const int EXP_LEN = *(publicKeyBlob.pbData + MOD_OFFSET + MOD_LEN + 1);

        const int EXP_OFFSET = publicKeyBlob.cbData - EXP_LEN;

        *pcbResult = sizeof(publicKey) + MOD_LEN + EXP_LEN;
        if (pbOutput == NULL) {
            LogTrace(LOGTYPE_INFO, "KSPExportKey", "This call is to query the size.");
            LogTrace(LOGTYPE_INFO, "KSPExportKey", "Key size = %d", *pcbResult);
            LogTrace(LOGTYPE_INFO, "KSPExportKey", "MOD_LEN=%d, EXP_LEN=%d", MOD_LEN, EXP_LEN);
            Status = STATUS_SUCCESS;
            goto cleanup;
        }

        publicKey.Magic = BCRYPT_RSAPUBLIC_MAGIC;
        publicKey.cbModulus = MOD_LEN;
        publicKey.cbPublicExp = EXP_LEN;
        publicKey.BitLength = MOD_LEN * 8;
        //Private key fields so not used here
        publicKey.cbPrime1 = 0;
        publicKey.cbPrime2 = 0;

        memcpy(pbOutput, (void *)&publicKey, sizeof(publicKey));
        //Copy pubkey public exponent
        memcpy(pbOutput + sizeof(publicKey), publicKeyBlob.pbData + EXP_OFFSET, EXP_LEN);
        //Copy pubkey modulus
        memcpy(pbOutput + sizeof(publicKey) + EXP_LEN, publicKeyBlob.pbData + MOD_OFFSET, MOD_LEN);

        Status = ERROR_SUCCESS;
    }

cleanup:
    if (pbTemp)
    {
        SecureZeroMemory(pbTemp, *pcbResult);
        HeapFree(GetProcessHeap(), 0, pbTemp);
    }
    return Status;
}



#define SHA256_LEN 32
#define DOCNAME_BUFFER_SIZE 50
#define PIN_BUFFER_SIZE 9
#define OTP_BUFFER_SIZE 7     // OTP is 6 digit

bool checkCmdErrorAndShowDlg(HWND hWnd, bool proxyUsed, int error, HWND parentWindow)
{
    std::wstring msg;
    if (error == SOAP_TCP_ERROR)
    {
        msg += (proxyUsed ? GETSTRING_DLG(PossibleProxyError) : GETSTRING_DLG(ConnectionError));
    }
    else if (error == SOAP_ERR_INVALID_OTP)
    {
        msg += GETSTRING_DLG(InvalidPinOrOtp);
    }
    else if (error == SOAP_ERR_OTP_VALIDATION_ERROR)
    {
        msg += GETSTRING_DLG(OtpValidationFailed);
    }
    if (!msg.empty())
    {
        CmdKspOpenDialogError(msg.c_str(), DlgCmdMsgType::DLG_CMD_WARNING_MSG, parentWindow);
    }
    return error != ERR_NONE;
}

/******************************************************************************
* DESCRIPTION :  creates a signature of a hash value.
*
* INPUTS :
*            NCRYPT_PROV_HANDLE hProvider    A handle to a CMD KSP provider
*                                            object
*            NCRYPT_KEY_HANDLE hKey          A handle to a CMD KSP key object
*            VOID    *pPaddingInfo           Padding information is padding sheme
*                                            is used
*            PBYTE  pbHashValue              Hash to sign.
*            DWORD  cbHashValue              Size of the hash.
*            DWORD  cbSignature              Size of the signature
*            DWORD  dwFlags                  Flags
* OUTPUTS:
*            PBYTE  pbSignature              Output buffer containing signature.
*                                            If pbOutput is NULL, required buffer
*                                            size will return in *pcbResult.
*            DWORD * pcbResult               Required size of the output buffer.
* RETURN :
*            ERROR_SUCCESS                   The function was successful.
*            NTE_BAD_KEY_STATE               The key identified by the hKey
*                                            parameter has not been finalized
*                                            or is incomplete.
*            NTE_INVALID_HANDLE              The handle is not a valid CMD KSP
*                                            provider or key handle.
*            NTE_INVALID_PARAMETER           One or more parameters are invalid.
*            NTE_BUFFER_TOO_SMALL            Output buffer is too small.
*            NTE_BAD_FLAGS                   dwFlags contains invalid value.
*/
SECURITY_STATUS
WINAPI
KSPSignHash(
__in    NCRYPT_PROV_HANDLE hProvider,
__in    NCRYPT_KEY_HANDLE hKey,
__in_opt    VOID  *pPaddingInfo,
__in_bcount(cbHashValue) PBYTE pbHashValue,
__in    DWORD   cbHashValue,
__out_bcount_part_opt(cbSignaturee, *pcbResult) PBYTE pbSignature,
__in    DWORD   cbSignature,
__out   DWORD * pcbResult,
__in    DWORD   dwFlags)
{
    SECURITY_STATUS     Status = NTE_INTERNAL_ERROR;
    int Status2 = 0;
    NTSTATUS            ntStatus = STATUS_INTERNAL_ERROR;
    CMDKSP_KEY          *pKey = NULL;
    DWORD               cbTmpSig = 0;
    DWORD               cbTmp = 0;
    LPWSTR              csSubject = NULL;

    LogTrace(LOGTYPE_INFO, "KSPSignHash", "Call KSPSignHash hKey = [0x%08x]", hKey);

    pKey = CmdKspValidateKeyHandle(hKey);

    if (pKey == NULL)
    {
        LogTrace(LOGTYPE_ERROR, "KSPSignHash", "pKey == NULL");
        Status = NTE_INVALID_HANDLE;
        goto cleanup;
    }

    if (pcbResult == NULL)
    {
        LogTrace(LOGTYPE_ERROR, "KSPSignHash", "pcbResult == NULL");
        Status = NTE_INVALID_PARAMETER;
        goto cleanup;
    }

    if (dwFlags & ~(BCRYPT_PAD_PKCS1 | BCRYPT_PAD_PSS | NCRYPT_SILENT_FLAG))
    {
        LogTrace(LOGTYPE_ERROR, "KSPSignHash", "NTE_BAD_FLAGS");
        Status = NTE_BAD_FLAGS;
        goto cleanup;
    }

    if (pKey->fFinished == FALSE)
    {
        LogTrace(LOGTYPE_ERROR, "KSPSignHash", "NTE_BAD_KEY_STATE");
        Status = NTE_BAD_KEY_STATE;
        goto cleanup;
    }

    //
    // Verify that this key is allowed to perform sign operations.
    //
    if ((pKey->dwKeyUsagePolicy & NCRYPT_ALLOW_SIGNING_FLAG) == 0)
    {
        LogTrace(LOGTYPE_ERROR, "KSPSignHash", "Verify that this key is allowed to perform sign operations");
        Status = (DWORD)NTE_PERM;
        goto cleanup;
    }

    if (pbSignature == NULL)
    {
        //This call is to query the size.
        LogTrace(LOGTYPE_INFO, "KSPSignHash", "This call is to query the size.");
        // Return the size of the key used to sign
        Status = ERROR_SUCCESS;
        *pcbResult = pKey->dwKeyBitLength / 8;
        LogTrace(LOGTYPE_INFO, "KSPSignHash", "Signature size = %d", *pcbResult);
        goto cleanup;
    }

    if (pbHashValue == NULL || cbHashValue == 0)
    {
        LogTrace(LOGTYPE_ERROR, "KSPSignHash", "pbHashValue == NULL || cbHashValue == 0");
        Status = NTE_INVALID_PARAMETER;
        goto cleanup;
    }

    // This block tells the compiler the objects instantiated here won't be used after the label cleanup
    // (see compiler error C2362)
    {
        std::wstring registeredMobileNumber = pKey->pszMobileNumber;
        unsigned long ulPinBufferLen = PIN_BUFFER_SIZE;
        wchar_t csPin[PIN_BUFFER_SIZE];
        DWORD dwSubjectLen = CertGetNameStringW(
                                pKey->pCert,
                                CERT_NAME_SIMPLE_DISPLAY_TYPE,
                                0,
                                NULL,
                                NULL,
                                0);
        csSubject = new WCHAR[dwSubjectLen];
        CertGetNameStringW(
            pKey->pCert,
            CERT_NAME_SIMPLE_DISPLAY_TYPE,
            0,
            NULL,
            csSubject,
            dwSubjectLen);

        Status = CmdKspOpenDialogSign(
            registeredMobileNumber.c_str(),
            registeredMobileNumber.size(),
            csPin, ulPinBufferLen, 
            csSubject,
            dwSubjectLen,
            pKey->hWnd);
        if (Status != ERROR_SUCCESS)
        {
            LogTrace(LOGTYPE_ERROR, "KSPSignHash", "Error in CmdKspOpenDialogSign.");
            goto cleanup;
        }
        std::wstring pinW(csPin);

        std::string mobileNumber = utilStringNarrow(registeredMobileNumber);
        std::string pin = utilStringNarrow(pinW);

        CMDSignature cmd_signature(CMDCredentials::getCMDBasicAuthUserId(),
                                   CMDCredentials::getCMDBasicAuthPassword(),
                                   CMDCredentials::getCMDBasicAuthAppId());

        CMDProxyInfo cmd_proxyinfo = CMDProxyInfo::buildProxyInfo();

        CByteArray hashBytes((const unsigned char *)pbHashValue, cbHashValue);
        char docnameBuffer[DOCNAME_BUFFER_SIZE];
        getDocName(pKey->hWnd, pKey->pszProcessBaseName, docnameBuffer, DOCNAME_BUFFER_SIZE, pbHashValue, cbHashValue);

        int ret;
        {
            CmdSignThread signOpenThread(&cmd_proxyinfo, &cmd_signature, mobileNumber, pin, hashBytes, docnameBuffer);
            signOpenThread.Start();

            // Blocks until user cancels or thread returns
            Status = CmdKspOpenDialogProgress(false, pKey->hWnd);

            if (Status != ERROR_SUCCESS)
            {
                LogTrace(LOGTYPE_WARNING, "KSPSignHash", "CmdKspOpenDialogProgress (pin) returned with Status=%d.", Status);
                signOpenThread.Stop();
                goto cleanup;
            }
            ret = signOpenThread.GetSignResult();
        }

        if (checkCmdErrorAndShowDlg(pKey->hWnd, cmd_proxyinfo.host.size() > 0, ret, pKey->hWnd))
        {
            LogTrace(LOGTYPE_ERROR, "KSPSignHash", "Error in signOpen: %d.", ret);
            Status = NTE_INTERNAL_ERROR;
            goto cleanup;
        }

        unsigned long ulOtpBufferLen = OTP_BUFFER_SIZE;
        wchar_t csOtp[OTP_BUFFER_SIZE];
        std::string docName(docnameBuffer);
        std::string idPattern = " - Id: ";
        // If we have the docname in the form "<application name> - <last 8 hash chars>" show
        // only the shorten hash in the dialog
        if (docName.find(idPattern) != std::wstring::npos)
        {
            docName = docName.substr(docName.find(idPattern) + idPattern.length());
        }
        Status = CmdKspOpenDialogOtp(csOtp, ulOtpBufferLen, 
            (wchar_t *)utilStringWiden(docName).c_str(), pKey->hWnd);
        if (Status != ERROR_SUCCESS)
        {
            LogTrace(LOGTYPE_ERROR, "KSPSignHash", "Error in CmdKspOpenDialogOtp.");
            goto cleanup;
        }

        std::wstring otpW(csOtp);
        std::string otp = utilStringNarrow(otpW);
        {
            CmdSignThread signCloseThread(&cmd_signature, otp);
            signCloseThread.Start();

            // Blocks until user cancels or thread returns
            Status = CmdKspOpenDialogProgress(true, pKey->hWnd);

            if (Status != ERROR_SUCCESS)
            {
                LogTrace(LOGTYPE_WARNING, "KSPSignHash", "CmdKspOpenDialogProgress (otp) returned with Status=%d.", Status);
                signCloseThread.Stop();
                goto cleanup;
            }
            ret = signCloseThread.GetSignResult();
        }

        if (checkCmdErrorAndShowDlg(pKey->hWnd, cmd_proxyinfo.host.size() > 0, ret, pKey->hWnd))
        {
            LogTrace(LOGTYPE_ERROR, "KSPSignHash", "Error in signClose: %d.", ret);
            Status = NTE_INTERNAL_ERROR;
            goto cleanup;
        }

        DWORD signature_len = cbSignature;
        Status = CryptStringToBinaryA(
            cmd_signature.m_string_signature.c_str(),
            cmd_signature.m_string_signature.length(),
            CRYPT_STRING_HEX_ANY,
            pbSignature,
            &signature_len,
            NULL,
            NULL
            );
        *pcbResult = signature_len;

        if (Status == 0)
        {
            LogTrace(LOGTYPE_ERROR, "KSPSignHash", "Failed to decode base64 signature: length: %d sign: %s", cmd_signature.m_string_signature.length(), 
                                                                                                             cmd_signature.m_string_signature.c_str());
            Status = NTE_INTERNAL_ERROR;
            goto cleanup;
        }
        else {
            LogTrace(LOGTYPE_INFO, "KSPSignHash", "Successfully decoded %d signature bytes!", signature_len);
        }
        Status = ERROR_SUCCESS;
    }
cleanup:
    if (csSubject)
    {
        delete[] csSubject;
    }
    return Status;
}

/******************************************************************************
* DESCRIPTION :  Verifies that the specified signature matches the specified hash
*
* INPUTS :
*            NCRYPT_PROV_HANDLE hProvider    A handle to a CMD KSP provider
*                                            object.
*            NCRYPT_KEY_HANDLE hKey          A handle to a CMD KSP key object
*            VOID    *pPaddingInfo           Padding information is padding sheme
*                                            is used.
*            PBYTE  pbHashValue              Hash data
*            DWORD  cbHashValue              Size of the hash
*            PBYTE  pbSignature              Signature data
*            DWORD  cbSignaturee             Size of the signature
*            DWORD  dwFlags                  Flags
*
* RETURN :
*            ERROR_SUCCESS                   The signature is a valid signature.
*            NTE_BAD_KEY_STATE               The key identified by the hKey
*                                            parameter has not been finalized
*                                            or is incomplete.
*            NTE_INVALID_HANDLE              The handle is not a valid CMD KSP
*                                            provider or key handle.
*            NTE_INVALID_PARAMETER           One or more parameters are invalid.
*            NTE_BAD_FLAGS                   dwFlags contains invalid value.
*/
SECURITY_STATUS
WINAPI
KSPVerifySignature(
__in    NCRYPT_PROV_HANDLE hProvider,
__in    NCRYPT_KEY_HANDLE hKey,
__in_opt    VOID *pPaddingInfo,
__in_bcount(cbHashValue) PBYTE pbHashValue,
__in    DWORD   cbHashValue,
__in_bcount(cbSignaturee) PBYTE pbSignature,
__in    DWORD   cbSignaturee,
__in    DWORD   dwFlags)
{
    NTSTATUS    ntStatus = STATUS_INTERNAL_ERROR;
    SECURITY_STATUS Status = NTE_INTERNAL_ERROR;
    CMDKSP_KEY *pKey;
    UNREFERENCED_PARAMETER(hProvider);

    LogTrace(LOGTYPE_INFO, "KSPVerifySignature", "Call KSPVerifySignature.");

    // Validate input parameters.
    pKey = CmdKspValidateKeyHandle(hKey);
    if (pKey == NULL)
    {
        LogTrace(LOGTYPE_ERROR, "KSPVerifySignature", "pKey == NULL");
        Status = NTE_INVALID_HANDLE;
        goto cleanup;
    }

    if (pKey->fFinished == FALSE)
    {
        LogTrace(LOGTYPE_ERROR, "KSPVerifySignature", "pKey->fFinished == FALSE");
        Status = NTE_BAD_KEY_STATE;
        goto cleanup;
    }

    if (pbHashValue == NULL || cbHashValue == 0)
    {
        LogTrace(LOGTYPE_ERROR, "KSPVerifySignature", "pbHashValue == NULL || cbHashValue == 0");
        Status = NTE_INVALID_PARAMETER;
        goto cleanup;
    }

    if (pbSignature == NULL || cbSignaturee == 0)
    {
        LogTrace(LOGTYPE_ERROR, "KSPVerifySignature", "pbSignature == NULL || cbSignaturee == 0");
        Status = NTE_INVALID_PARAMETER;
        goto cleanup;
    }

    if (dwFlags & ~(BCRYPT_PAD_PKCS1 | BCRYPT_PAD_PSS | NCRYPT_SILENT_FLAG))
    {
        LogTrace(LOGTYPE_ERROR, "KSPVerifySignature", "Bad dwFlags");
        Status = NTE_BAD_FLAGS;
        goto cleanup;
    }

    BCRYPT_KEY_HANDLE hPublicKey;
    if (!CryptImportPublicKeyInfoEx2(
        X509_ASN_ENCODING,
        &pKey->pCert->pCertInfo->SubjectPublicKeyInfo,
        0,
        NULL,
        &hPublicKey)
        )
    {
        LogTrace(LOGTYPE_ERROR, "KSPVerifySignature", "Error in CryptImportPublicKeyInfoEx2: 0x%x", GetLastError());
        Status = NTE_INTERNAL_ERROR;
        goto cleanup;
    }

    //Verify the signature.
    ntStatus = BCryptVerifySignature(
        hPublicKey,
        pPaddingInfo,
        pbHashValue,
        cbHashValue,
        pbSignature,
        cbSignaturee,
        dwFlags);

    BCryptDestroyKey(hPublicKey);

    if (!NT_SUCCESS(ntStatus))
    {
        LogTrace(LOGTYPE_ERROR, "KSPVerifySignature", "Error in BCryptVerifySignature: %x", ntStatus);
        Status = NormalizeNteStatus(Status);
        goto cleanup;
    }

    Status = ERROR_SUCCESS;
cleanup:

    return Status;
}

SECURITY_STATUS
WINAPI
KSPPromptUser(
__in    NCRYPT_PROV_HANDLE hProvider,
__in_opt NCRYPT_KEY_HANDLE hKey,
__in    LPCWSTR  pszOperation,
__in    DWORD   dwFlags)
{
    UNREFERENCED_PARAMETER(hProvider);
    UNREFERENCED_PARAMETER(hKey);
    UNREFERENCED_PARAMETER(pszOperation);
    UNREFERENCED_PARAMETER(dwFlags);

    LogTrace(LOGTYPE_INFO, "KSPPromptUser", "Call KSPPromptUser NTE_NOT_SUPPORTED");

    return NTE_NOT_SUPPORTED;
}

SECURITY_STATUS
WINAPI
KSPNotifyChangeKey(
__in    NCRYPT_PROV_HANDLE hProvider,
__inout HANDLE *phEvent,
__in    DWORD   dwFlags)
{
    UNREFERENCED_PARAMETER(hProvider);
    UNREFERENCED_PARAMETER(phEvent);
    UNREFERENCED_PARAMETER(dwFlags);

    LogTrace(LOGTYPE_INFO, "KSPNotifyChangeKey", "Call KSPNotifyChangeKey NTE_NOT_SUPPORTED");

    return NTE_NOT_SUPPORTED;
}


SECURITY_STATUS
WINAPI
KSPSecretAgreement(
__in    NCRYPT_PROV_HANDLE hProvider,
__in    NCRYPT_KEY_HANDLE hPrivKey,
__in    NCRYPT_KEY_HANDLE hPubKey,
__out   NCRYPT_SECRET_HANDLE *phAgreedSecret,
__in    DWORD   dwFlags)
{
    UNREFERENCED_PARAMETER(hProvider);
    UNREFERENCED_PARAMETER(hPrivKey);
    UNREFERENCED_PARAMETER(hPubKey);
    UNREFERENCED_PARAMETER(phAgreedSecret);
    UNREFERENCED_PARAMETER(dwFlags);

    LogTrace(LOGTYPE_INFO, "KSPSecretAgreement", "Call KSPSecretAgreement NTE_NOT_SUPPORTED");

    return NTE_NOT_SUPPORTED;
}


SECURITY_STATUS
WINAPI
KSPDeriveKey(
__in        NCRYPT_PROV_HANDLE   hProvider,
__in_opt    NCRYPT_SECRET_HANDLE hSharedSecret,
__in        LPCWSTR              pwszKDF,
__in_opt    NCryptBufferDesc     *pParameterList,
__out_bcount_part_opt(cbDerivedKey, *pcbResult) PUCHAR pbDerivedKey,
__in        DWORD                cbDerivedKey,
__out       DWORD                *pcbResult,
__in        ULONG                dwFlags)
{
    UNREFERENCED_PARAMETER(hProvider);
    UNREFERENCED_PARAMETER(hSharedSecret);
    UNREFERENCED_PARAMETER(pwszKDF);
    UNREFERENCED_PARAMETER(pParameterList);
    UNREFERENCED_PARAMETER(pbDerivedKey);
    UNREFERENCED_PARAMETER(cbDerivedKey);
    UNREFERENCED_PARAMETER(pcbResult);
    UNREFERENCED_PARAMETER(dwFlags);

    LogTrace(LOGTYPE_INFO, "KSPDeriveKey", "Call KSPDeriveKey NTE_NOT_SUPPORTED");

    return NTE_NOT_SUPPORTED;
}

SECURITY_STATUS
WINAPI
KSPFreeSecret(
__in    NCRYPT_PROV_HANDLE hProvider,
__in    NCRYPT_SECRET_HANDLE hSharedSecret)
{
    UNREFERENCED_PARAMETER(hProvider);
    UNREFERENCED_PARAMETER(hSharedSecret);

    LogTrace(LOGTYPE_INFO, "KSPFreeSecret", "Call KSPFreeSecret NTE_NOT_SUPPORTED");

    return NTE_NOT_SUPPORTED;
}