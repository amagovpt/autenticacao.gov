/*-****************************************************************************

 * Copyright (C) 2020 Miguel Figueira - <miguel.figueira@caixamagica.pt>
 * Copyright (C) 2020 José Pinto - <jose.pinto@caixamagica.pt>
 *
 * Licensed under the EUPL V.1.2

****************************************************************************-*/

/*++

Abstract:
    Helper functions for CMD key storage provider

Implementation Note:
   In several places this sample returns a single, generic SECURITY_STATUS error when a called function
   returns any Win32 error.  This is only done for the sake of brevity in the sample.

   As a best practice, production code should provide a function to convert Win32 errors
   to appropriate SECURITY_STATUS errors, and use it to set SECURITY_STATUS error
   variables accurately.  This would allow relevant troubleshooting error information
   to propagate out of the KSP.
--*/

///////////////////////////////////////////////////////////////////////////////
//
// Headers...
//
///////////////////////////////////////////////////////////////////////////////
#include <windows.h>
#include <wincrypt.h>
#include <winuser.h>
#include <bcrypt.h>
#include <ncrypt.h>
#include <intsafe.h>
#include <strsafe.h>
#include "..\inc\KSP.h"
#include "..\Inc\log.h"
#include "psapi.h"
#include <codecvt>
#include "Util.h"

using namespace eIDMW;

HWND g_HWND = NULL;
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
    DWORD lpdwProcessId;
    GetWindowThreadProcessId(hwnd, &lpdwProcessId);
    CHAR lpClassName[256];
    if (GetClassName(
        hwnd,
        lpClassName,
        256) == 0)
    {
        LogTrace(LOGTYPE_ERROR, "EnumWindowsProcPID", "Error getting window class name.");
        return TRUE;
    }

    if (lpdwProcessId == GetCurrentProcessId() &&
        strcmp(lpClassName, (const char*)lParam) == 0)
    {
        g_HWND = hwnd;
        return FALSE;
    }
    return TRUE;
}


HWND GetMainWindow(LPSTR lpProcessName)
{
    /* The class of a window can be easily found with the Spy++ tool. */
    std::string windowClass;
    if (strcmp(lpProcessName, "EXCEL.EXE") == 0)
    {
        windowClass = "XLMAIN";
    }
    else if (strcmp(lpProcessName, "WINWORD.EXE") == 0)
    {
        windowClass = "OpusApp";
    }
    else if (strcmp(lpProcessName, "AcroRd32.exe") == 0)
    {
        windowClass = "AcrobatSDIWindow";
    }
    else
    {
        return NULL;
    }
    EnumWindows(EnumWindowsProc, (LPARAM)windowClass.c_str());
    return g_HWND;
}

///////////////////////////////////////////////////////////////////////////////
/******************************************************************************
*
* DESCRIPTION :     Convert NTSTATUS error code to SECURITY_STATUS error code
*
* INPUTS :
*            NTSTATUS NtStatus          Error code of NTSTATUS format
* RETURN :
*            SECURITY_STATUS            Converted error code
*/
SECURITY_STATUS
NormalizeNteStatus(
__in NTSTATUS NtStatus)
{
    SECURITY_STATUS SecStatus;

    switch (NtStatus)
    {
    case STATUS_SUCCESS:
        SecStatus = ERROR_SUCCESS;
        break;

    case STATUS_NO_MEMORY:
    case STATUS_INSUFFICIENT_RESOURCES:
        SecStatus = NTE_NO_MEMORY;
        break;

    case STATUS_INVALID_PARAMETER:
        SecStatus = NTE_INVALID_PARAMETER;
        break;

    case STATUS_INVALID_HANDLE:
        SecStatus = NTE_INVALID_HANDLE;
        break;

    case STATUS_BUFFER_TOO_SMALL:
        SecStatus = NTE_BUFFER_TOO_SMALL;
        break;

    case STATUS_NOT_SUPPORTED:
        SecStatus = NTE_NOT_SUPPORTED;
        break;

    case STATUS_INTERNAL_ERROR:
    case ERROR_INTERNAL_ERROR:
        SecStatus = NTE_INTERNAL_ERROR;
        break;

    case STATUS_INVALID_SIGNATURE:
        SecStatus = NTE_BAD_SIGNATURE;
        break;

    default:
        SecStatus = NTE_INTERNAL_ERROR;
        break;
    }

    return SecStatus;
}


///////////////////////////////////////////////////////////////////////////////
/*****************************************************************************
* DESCRIPTION :    Validate KSP provider handle
*
* INPUTS :
*           NCRYPT_PROV_HANDLE hProvider                A NCRYPT_PROV_HANDLE handle
*
* RETURN :
*           A pointer to a CMDKSP_PROVIDER struct    The function was successful.
*           NULL                                        The handle is invalid.
*/
CMDKSP_PROVIDER *
CmdKspValidateProvHandle(
__in    NCRYPT_PROV_HANDLE hProvider)
{
    CMDKSP_PROVIDER *pProvider = NULL;

    if (hProvider == 0)
    {
        return NULL;
    }

    pProvider = (CMDKSP_PROVIDER *)hProvider;

    if (pProvider->cbLength < sizeof(CMDKSP_PROVIDER) ||
        pProvider->dwMagic != CMDKSP_PROVIDER_MAGIC)
    {
        return NULL;
    }

    return pProvider;
}

/*****************************************************************************
* DESCRIPTION :    Validate KSP key handle
*
* INPUTS :
*           NCRYPT_KEY_HANDLE hKey                 An NCRYPT_KEY_HANDLE handle
*
* RETURN :
*           A pointer to a KSP_KEY struct    The function was successful.
*           NULL                                   The handle is invalid.
*/
CMDKSP_KEY *
CmdKspValidateKeyHandle(
__in    NCRYPT_KEY_HANDLE hKey)
{
    CMDKSP_KEY *pKey = NULL;

    if (hKey == 0)
    {
        return NULL;
    }

    pKey = (CMDKSP_KEY *)hKey;

    if (pKey->cbLength < sizeof(CMDKSP_KEY) ||
        pKey->dwMagic != CMDKSP_KEY_MAGIC)
    {
        return NULL;
    }

    return pKey;
}

///////////////////////////////////////////////////////////////////////////////
/******************************************************************************
*
* DESCRIPTION : Creates a new KSP key object.
*
* INPUTS :
*               LPCWSTR                Name of the key
* OUTPUTS :
*               KSP_KEY* pKey    New KSP key object
* RETURN :
*               ERROR_SUCCESS          The function was successful.
*               NTE_BAD_DATA           The key blob is not valid.
*               NTE_NO_MEMORY          A memory allocation failure occurred.
*               HRESULT                Error information returned by CryptProtectData.
*/
SECURITY_STATUS
WINAPI
CreateNewKeyObject(
__in_opt LPCWSTR pszKeyName,
__deref_out CMDKSP_KEY **ppKey)
{
    CMDKSP_KEY *pKey = NULL;
    DWORD   cbKeyName = 0;
    SECURITY_STATUS   Status = NTE_INTERNAL_ERROR;
    NTSTATUS          ntStatus = STATUS_INTERNAL_ERROR;
    BYTE *pbHashBuffer = NULL;

    LogTrace(LOGTYPE_TRACE, "CreateNewKeyObject", "CreateNewKeyObject");

    //Initialize the key object.
    pKey = (CMDKSP_KEY *)HeapAlloc(GetProcessHeap(), 0, sizeof(CMDKSP_KEY));
    if (pKey == NULL)
    {
        Status = NTE_NO_MEMORY;
        goto cleanup;
    }
    pKey->cbLength = sizeof(CMDKSP_KEY);
    pKey->dwMagic = CMDKSP_KEY_MAGIC;
    pKey->fFinished = FALSE;

    //Copy the keyname into the key struct.
    if (pszKeyName != NULL)
    {
        cbKeyName = (DWORD)(wcslen(pszKeyName) + 1)*sizeof(WCHAR);

        if (cbKeyName > MAX_PATH)
        {
            Status = NTE_INVALID_PARAMETER;
            goto cleanup;
        }
        cbKeyName = cbKeyName * sizeof(WCHAR);
        pKey->pszKeyName = (LPWSTR)HeapAlloc(GetProcessHeap(), 0, cbKeyName);
        if (pKey->pszKeyName == NULL)
        {
            Status = NTE_NO_MEMORY;
            goto cleanup;
        }
        CopyMemory(pKey->pszKeyName, pszKeyName, cbKeyName);
    }
    else
    {
        pKey->pszKeyName = NULL;
    }

    //Create the BCrypt handle.
    if (g_hRSAProvider == NULL)
    {
        //If cached handle is NULL, open the default RSA algorithm provider.
        ntStatus = BCryptOpenAlgorithmProvider(
            &g_hRSAProvider,
            BCRYPT_RSA_ALGORITHM,
            NULL,
            0);
        if (!NT_SUCCESS(ntStatus))
        {
            Status = NormalizeNteStatus(ntStatus);
            goto cleanup;
        }

    }
    pKey->hProvider = g_hRSAProvider;

    // Set process base name and look for default main window (the caller 
    // process can change it with KSPSetKeyProperty)
    {
        LPSTR lpProcessName = new CHAR[FILENAME_MAX];
        GetModuleBaseName(
            GetCurrentProcess(),
            NULL,
            lpProcessName,
            FILENAME_MAX);
        LogTrace(LOGTYPE_TRACE, "getDocName", "ProcessName=%s", lpProcessName);

        pKey->pszProcessBaseName = lpProcessName;
        pKey->hWnd = GetMainWindow(lpProcessName);
    }

    {
        // The key name is "CMD_" + "NR_"/"DS_" + associated certificate's SHA1.
        // The hash can be used to find the cert in the store.
        std::wstring hashHex = pszKeyName;
        hashHex = hashHex.substr(7);
        HCERTSTORE hMyStore = CertOpenSystemStoreW(NULL, L"MY");
        CRYPT_HASH_BLOB hash = { 0, 0 };
        DWORD cbBufferSize;
        if (!CryptStringToBinaryW(
            hashHex.c_str(),
            hashHex.size(),
            CRYPT_STRING_HEXRAW,
            NULL,
            &cbBufferSize,
            NULL,
            NULL))
        {
            LogTrace(LOGTYPE_ERROR, "CreateNewKeyObject", "Error computing the size of the hash buffer in CryptStringToBinary");
            Status = NTE_INTERNAL_ERROR;
            goto cleanup;
        }
        pbHashBuffer = new BYTE[cbBufferSize];
        if (!CryptStringToBinaryW(
            hashHex.c_str(),
            hashHex.size(),
            CRYPT_STRING_HEXRAW,
            pbHashBuffer,
            &cbBufferSize,
            NULL,
            NULL))
        {
            LogTrace(LOGTYPE_ERROR, "CreateNewKeyObject", "Error getting the hash in CryptStringToBinary");
            Status = NTE_INTERNAL_ERROR;
            goto cleanup;
        }

        hash.cbData = cbBufferSize;
        hash.pbData = pbHashBuffer;
        PCCERT_CONTEXT pCert = CertFindCertificateInStore(
            hMyStore,
            X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
            0,
            CERT_FIND_HASH,
            (const void *)&hash,
            NULL);

        if (pCert == NULL)
        {
            LogTrace(LOGTYPE_ERROR, "CreateNewKeyObject", "Could not find the certificate associated with this private key.");
            Status = NTE_INTERNAL_ERROR;
            goto cleanup;
        }
        pKey->pCert = pCert;

        // The CERT_FIRST_USER_PROP_ID was used to store the mobile number used to register the certificate
        DWORD cbMobileNumber;
        if (!CertGetCertificateContextProperty(
            pKey->pCert,
            CERT_FIRST_USER_PROP_ID,
            NULL,
            &cbMobileNumber))
        {
            LogTrace(LOGTYPE_ERROR, "CreateNewKeyObject", "Could not get size of certificate's friendly name.");
            Status = NTE_INTERNAL_ERROR;
            goto cleanup;
        }
        LPWSTR pszMobileNumber = new WCHAR[cbMobileNumber];
        if (!CertGetCertificateContextProperty(
            pKey->pCert,
            CERT_FIRST_USER_PROP_ID,
            pszMobileNumber,
            &cbMobileNumber))
        {
            LogTrace(LOGTYPE_ERROR, "CreateNewKeyObject", "Could not get certificate's friendly name.");
            Status = NTE_INTERNAL_ERROR;
            goto cleanup;
        }
        pKey->pszMobileNumber = pszMobileNumber;
    }

    pKey->dwKeyBitLength = CertGetPublicKeyLength(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, &pKey->pCert->pCertInfo->SubjectPublicKeyInfo);
    if (pKey->dwKeyBitLength == 0)
    {
        LogTrace(LOGTYPE_ERROR, "CreateNewKeyObject", "Could not get certificate's key bit length.");
        Status = NTE_INTERNAL_ERROR;
        goto cleanup;
    }
    LogTrace(LOGTYPE_TRACE, "CreateNewKeyObject", "Certificate's key bit length = %d", pKey->dwKeyBitLength);

    //The usage of the key is for signing.
    pKey->dwKeyUsagePolicy = NCRYPT_ALLOW_SIGNING_FLAG;

    *ppKey = pKey;
    pKey = NULL;
    Status = ERROR_SUCCESS;

cleanup:
    if (pKey != NULL)
    {
        DeleteKeyObject(pKey);
    }
    if (pbHashBuffer)
    {
        delete[] pbHashBuffer;
    }
    return Status;
}

/******************************************************************************
*
* DESCRIPTION : Deletes the passed key object from the KSP.
*
* INPUTS :
*               KSP_KEY *pKey    The key object to delete.
* RETURN :
*               ERROR_SUCCESS          The function was successful.
*/
SECURITY_STATUS
WINAPI
DeleteKeyObject(
__inout CMDKSP_KEY *pKey)
{
    PLIST_ENTRY pList = { 0 };
    SECURITY_STATUS Status = ERROR_SUCCESS;
    NTSTATUS ntStatus = STATUS_INTERNAL_ERROR;

    LogTrace(LOGTYPE_TRACE, "DeleteKeyObject", "DeleteKeyObject");

    //Delete the key name.
    if (pKey->pszKeyName)
    {
        HeapFree(GetProcessHeap(), 0, pKey->pszKeyName);
        pKey->pszKeyName = NULL;
    }

    //Delete public key handle.
    if (pKey->pCert)
    {
        CertFreeCertificateContext(pKey->pCert);
    }

    //Delete the mobile number.
    if (pKey->pszMobileNumber)
    {
        delete[] pKey->pszMobileNumber;
    }

    if (pKey->pszProcessBaseName)
    {
        delete[] pKey->pszProcessBaseName;
    }
    return Status;
}

std::wstring GetApplicationName(LPSTR lpProcessName) {
    if (strcmp(lpProcessName, "EXCEL.EXE") == 0)
    {
        return L"Microsoft Excel";
    }
    else if (strcmp(lpProcessName, "WINWORD.EXE") == 0)
    {
        return L"Microsoft Word";
    }
    else if (strcmp(lpProcessName, "AcroRd32.exe") == 0)
    {
        return L"Adobe Acrobat Reader";
    }
    else if (strcmp(lpProcessName, "OUTLOOK.EXE") == 0)
    {
        return L"Microsoft Outlook";
    }
    else
    {
        std::wstring processName = utilStringWiden(lpProcessName);
        size_t last_dot_idx = processName.find_last_of(L".");
        if (last_dot_idx != std::wstring::npos)
        {
            processName = processName.substr(0, last_dot_idx);
        }
        return processName;
    }
}

std::wstring GetShortHashString(PBYTE pbHash, DWORD cbHash) {
#define SHORT_HASH_LEN 8
    DWORD dwFlags = CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF;
    DWORD cchHash;
    LogTrace(LOGTYPE_TRACE, "GetHashString", "pbHash=%x cbHash=%d", pbHash, cbHash);
    if (!CryptBinaryToStringW(
        pbHash,
        cbHash,
        dwFlags,
        NULL,
        &cchHash))
    {
        LogTrace(LOGTYPE_ERROR, "GetHashString", "Error getting string size CryptBinaryToString");
        return L"";
    }

    LPWSTR chHash = new WCHAR[cchHash];
    if (!CryptBinaryToStringW(
        pbHash,
        cbHash,
        dwFlags,
        chHash,
        &cchHash))
    {
        LogTrace(LOGTYPE_ERROR, "GetHashString", "Error getting string CryptBinaryToString");
        delete[] chHash;
        return L"";
    }
    std::wstring hash = chHash;
    delete[] chHash;
    hash = hash.substr(hash.length() - SHORT_HASH_LEN, SHORT_HASH_LEN);
    return hash;
}

void
getDocName(
__in    HWND hParentWindow,
__in    LPSTR lpProcessName,
__out   LPSTR pszBuffer,
__in    DWORD cbBuffer,
__in    PBYTE pbHash,
__in    DWORD cbHash)
{
    DWORD cbWritten;
    LPWSTR pszTitle = NULL;
    BOOL bGotDocname = FALSE;
    
    DWORD cbWindowTitle;
    std::wstring title;
    std::string docname;

    if (!hParentWindow){
        LogTrace(LOGTYPE_WARNING, "getDocName", "Could not get main window.");
        goto cleanup;
    }

    // Get window title
    cbWindowTitle = GetWindowTextLengthA(hParentWindow);
    if (cbWindowTitle == 0)
    {
        LogTrace(LOGTYPE_WARNING, "getDocName", "Cannot get window title size.");
        goto cleanup;
    }
    pszTitle = new WCHAR[cbWindowTitle + 1];
    cbWritten = GetWindowTextW(hParentWindow, pszTitle, cbWindowTitle + 1);
    if (cbWritten == 0)
    {
        LogTrace(LOGTYPE_WARNING, "getDocName", "Cannot get window title.");
        goto cleanup;
    }
    title = pszTitle;

    // In Excel, Word and Adobe Acrobat Reader the docname comes before the last " - "
    size_t docnameEndingIdx = std::string::npos;
    if (strcmp(lpProcessName, "EXCEL.EXE") == 0 ||
        strcmp(lpProcessName, "WINWORD.EXE") == 0 ||
        strcmp(lpProcessName, "AcroRd32.exe") == 0 ||
        strcmp(lpProcessName, "OUTLOOK.EXE") == 0)
    {
        size_t idx = 0;
        do
        {
            docnameEndingIdx = (idx == 0 ? std::string::npos : idx);
            idx = title.find(L" - ", idx + 1);
        } while (idx != std::string::npos);
    }

    if (docnameEndingIdx == std::string::npos)
    {
        LogTrace(LOGTYPE_WARNING, "getDocName", "Could not find docname in title.");
        goto cleanup;
    }

    // In Outlook, if the word that comes after the last " - " is Outlook, then we are not in a 
    // "New Mail" window and the subject is not in the title
    if (strcmp(lpProcessName, "OUTLOOK.EXE") == 0 
        && title.substr(docnameEndingIdx).find(L"Outlook") != std::string::npos)
    {
        LogTrace(LOGTYPE_INFO, "getDocName", "Outlook window does not have Subject in title.");
        goto cleanup;
    }

    title = title.substr(0, docnameEndingIdx);

    bGotDocname = TRUE;

cleanup:
    if (pszTitle)
    {
        delete[] pszTitle;
    }
    if (!bGotDocname)
    {
        // If we did not get the docname, send "<application name> - <last 8 hash chars>"
        std::wstring applicationName = GetApplicationName(lpProcessName);
        std::wstring shortHash = GetShortHashString(pbHash, cbHash);
        title = L"Aplicação: " + applicationName + L" - Id: " + shortHash;
    }

    // Convert string: UTF-16 to UTF-8
#ifdef WIN32
    std::wstring u16Docname(title.begin(), title.end());
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> convert;
#else
    std::u16string u16Docname(title.begin(), title.end());
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
#endif
    docname = convert.to_bytes(u16Docname);
    StringCbCopyA(pszBuffer, cbBuffer, docname.c_str());
}