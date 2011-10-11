/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2009 FedICT.
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
#include <windows.h>
#include "eidmwcspdk.h"

#include "../dialogs/dialogs.h"
#include "../cardlayer/CardLayer.h"
#include "../common/Util.h"
#include "ProviderAlgorithms.h"
#include "ProviderContainers.h"
#include "ProviderContextPool.h"
#include "csperr.h"
#include "cspdefines.h"
#include "csputil.h"

using namespace eIDMW;

static CProviderContextPool g_oProviderContextPool;
static CProviderAlgorithms g_oProviderAlgorithms;
static CProviderContainers m_ProviderContainers;

static CCardLayer g_oCardLayer;
static CReader *FindCard(CProviderContext *poProxCtx);
static tPrivKey FindKey(CReader *poReader, const std::string &csContainerName);
static CMutex g_oCalMutex;

BOOL WINAPI
DllMain(
	HINSTANCE hinstDLL, //!< Handle to this DLL
	DWORD fdwReason,    //!< Reason why this function was called
	LPVOID lpvReserved) //!< Reserved for later use
{
	BOOL bReturnVal = TRUE;
	DWORD dwLastError = 0;

	__CSP_TRY__

	switch (fdwReason)
	{
		case DLL_PROCESS_ATTACH:
			MWLOG(LEV_INFO, MOD_CSP, L"*********************************************");
			MWLOG(LEV_INFO, MOD_CSP, L"CSP DllMain(ATTACH) called");
			break;
		case DLL_PROCESS_DETACH:
			MWLOG(LEV_INFO, MOD_CSP, L"CSP DllMain(DETACH) called");
			break;
	}

	__CSP_CATCH_SETLASTERROR__

	return (bReturnVal);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \brief
//! The CPAcquireContext function is used to acquire a context handle to a cryptographic service provider (CSP).
//
//! Because this CSP is oriented towards the eID card and this card does not allow the creation
//! or deletion of key material on the card following values for dwFlags are not supported:
//!   - CRYPT_NEWKEYSET 
//!   - CRYPT_MACHINE_KEYSET 
//!   - CRYPT_DELETEKEYSET
//! Setting these values for dwFlags anyway will result in the error NTE_BAD_FLAGS being set through SetLastError () and
//! FALSE being returned as an indication that the function call failed.
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL WINAPI
eidmwAcquireContext(
	OUT HCRYPTPROV *phProv,        //!< Handle to crypto provider
	IN  LPCSTR szContainer,        //!< Container of the key material that will be used
	IN  DWORD dwFlags,             //!< Type of context that is required
	IN  PVTableProvStruc pVTable)  //!< List of functions provided by the operating system
{
	BOOL bReturnVal = FALSE;
	DWORD dwLastError = 0;

	__CSP_TRY__

	MWLOG(LEV_INFO, MOD_CSP, L"CPAcquireContext() called");
	MWLOG(LEV_INFO, MOD_CSP, L"  - IN szContainer: %ls",
		szContainer ? utilStringWiden(szContainer).c_str() : L"NULL");
	MWLOG(LEV_INFO, MOD_CSP, L"  - IN dwFlags    : 0x%0x", dwFlags);

	if (dwFlags == CRYPT_NEWKEYSET || dwFlags == CRYPT_MACHINE_KEYSET || dwFlags == CRYPT_DELETEKEYSET)
	{
		dwLastError = NTE_BAD_FLAGS;
	}
	else if (szContainer != NULL && strlen(szContainer) != 0 &&
		!m_ProviderContainers.ContainerExists(szContainer))
	{
		dwLastError = NTE_BAD_KEYSET;
	}
	else
	{
		*phProv = g_oProviderContextPool.AddProviderContext(szContainer, dwFlags, pVTable);
		bReturnVal = TRUE;
	}

	__STORE_LASTERROR__

	MWLOG(LEV_INFO, MOD_CSP, L" Returning CPAcquireContext() with code 0x%0x",
		bReturnVal ? 0 : dwLastError);
	if (bReturnVal)
		MWLOG(LEV_INFO, MOD_CSP, L"   - OUT: *phProv = %d", *phProv);

	__CSP_CATCH_SETLASTERROR__

	return (bReturnVal);
}

extern BOOL WINAPI
eidmwAcquireContextW(
    OUT HCRYPTPROV *phProv,
    IN  LPCWSTR szContainer,
    IN  DWORD dwFlags,
    IN  PVTableProvStrucW pVTable)
{
	BOOL bReturnVal = FALSE;
	DWORD dwLastError = 0;

	__CSP_TRY__

	MWLOG(LEV_INFO, MOD_CSP, L"CPAcquireContextW() called");
	MWLOG(LEV_INFO, MOD_CSP, L"  - IN szContainer: %ls",
		szContainer ? szContainer : L"NULL");
	MWLOG(LEV_INFO, MOD_CSP, L"  - IN dwFlags    : 0x%0x", dwFlags);

	if (dwFlags == CRYPT_NEWKEYSET || dwFlags == CRYPT_MACHINE_KEYSET || dwFlags == CRYPT_DELETEKEYSET)
	{
		dwLastError = NTE_BAD_FLAGS;
	}
	else
	{
		std::string strContainer;
		const char *szContainerA = NULL;
		if (szContainer != NULL)
		{
			strContainer = utilStringNarrow(szContainer);
			szContainerA = strContainer.c_str();
		}
		if (szContainerA != NULL && strlen(szContainerA) != 0 &&
			!m_ProviderContainers.ContainerExists(szContainerA))
		{
			dwLastError = NTE_BAD_KEYSET;
		}
		else
		{
			VTableProvStruc vTableA = {
				pVTable->Version,
				NULL,
				pVTable->FuncReturnhWnd,
				pVTable->dwProvType,
				pVTable->pbContextInfo,
				pVTable->cbContextInfo,
				NULL
			};
				
			*phProv = g_oProviderContextPool.AddProviderContext(szContainerA, dwFlags, &vTableA);
			bReturnVal = TRUE;
		}
	}

	__STORE_LASTERROR__

	MWLOG(LEV_INFO, MOD_CSP, L" Returning CPAcquireContextW() with code 0x%0x",
		bReturnVal ? 0 : dwLastError);
	if (bReturnVal)
		MWLOG(LEV_INFO, MOD_CSP, L"   - OUT: *phProv = %d", *phProv);

	__CSP_CATCH_SETLASTERROR__

	return (bReturnVal);}

BOOL WINAPI
eidmwReleaseContext(
    IN  HCRYPTPROV hProv,  //!< Handle to provider previously obtained through a call to CPAcquireContext
    IN  DWORD dwFlags)     //!< Flags. Currently no flags defined
{
	BOOL bReturnVal = FALSE;
	DWORD dwLastError = 0;

	__CSP_TRY__

	MWLOG(LEV_INFO, MOD_CSP, L"CPReleaseContext(hProv = %d) called", hProv);

	if (dwFlags != 0)
	{
		dwLastError = NTE_BAD_FLAGS;
	}
	else
	{
		bReturnVal = g_oProviderContextPool.DeleteProviderContext(hProv);
		if (!bReturnVal)
			dwLastError = NTE_BAD_UID;
	}

	__STORE_LASTERROR__

	MWLOG(LEV_INFO, MOD_CSP, L" Returning CPReleaseContext(hProv = %d) with code 0x%0x",
		hProv, bReturnVal ? 0 : dwLastError);

	__CSP_CATCH_SETLASTERROR__

	return (bReturnVal);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \brief
//! This function sets parameters for the provider (CSP)
//
//! Although this function is implemented it does not make sense calling it. The only parameter that can be set is
//! PP_KEYSET_SEC_DESCR. However, since the key material is not stored in the registry it does not make sense
//! setting it. At a higher level (CryptSetProvParam) one extra parameter is available PP_CLIENT_HWND. This 
//! parameter is however not passed to this function but buffered until CryptAcquireContext is called. The handle
//! to the window can then be obtained through the parameter /a pVTable.
//! Calling this function anyway will not result in an error (provided the parameter passed is PP_KEYSET_SEC_DESCR)
//! but no parameters will be set.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL WINAPI
eidmwSetProvParam(
    IN  HCRYPTPROV hProv,    //!< Handle to provider previously obtained through a call to CPAcquireContext
    IN  DWORD dwParam,       //!< Parameter to set   
    IN  CONST BYTE *pbData,  //! Data of the parameter
    IN  DWORD dwFlags)       //! Flags related to the parameter PP_KEYSET_SEC_DESCR
{
	BOOL bReturnVal = TRUE;
	DWORD dwLastError = 0;

	__CSP_TRY__

	MWLOG(LEV_INFO, MOD_CSP, L"CPSetKeyParam(hProv = %d) called", hProv);

	switch (dwParam)
	{
	case PP_KEYSET_SEC_DESCR:
		break;
	default:
		bReturnVal = FALSE;
		dwLastError = NTE_BAD_TYPE;
		break;
	}

	__STORE_LASTERROR__

	MWLOG(LEV_INFO, MOD_CSP, L" Returning CPSetProvParam(%d) with %ls",
		hProv, bReturnVal ? L"OK" : L"NTE_BAD_TYPE");

	__CSP_CATCH_SETLASTERROR__

	return (bReturnVal);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \brief
//! This function allows the retrieval of parameters from the provider (CSP)
//
//! An application can interrogate the capabilities of the CSP through this function. Following are the parameters 
//! that can be obtained:
//!  - PP_UNIQUE_CONTAINER
//!  - PP_CONTAINER
//!  - PP_ENUMALGS
//!  - PP_ENUMALGS_EX
//!  - PP_ENUMCONTAINERS
//!  - PP_IMPTYPE
//!  - PP_NAME
//!  - PP_VERSION
//!  - PP_PROVTYPE
//!
//! The parameters PP_UNIQUE_CONTAINER and PP_CONTAINER both return the same data when called, the name of 
//! the current container.
//! In case of the enumeration parameters the argument \a dwFlags must be set to CRYPT_FIRST for the first element
//! in the enumeration. If the first request is not started with the flag CRYPT_FIRST flag, the error 
//! NTE_BAD_FLAGS is returned through SetLastError (). For subsequent element the value of \a dwFlags 
//! is not important. When the last element of an enumeration is reached then the error ERROR_NO_MORE_ITEMS is set 
//! through SetLastError (). \n
//! In case of the enumeration PP_ENUMCONTAINERS the size of the data returned by \a pcbDataLen is the length of 
//! the largest element in the enumeration. The length of the data to be returned can be obtained by setting \a pbData 
//! to NULL. The length of the data that will be returned is then placed in \a pcbDataLen. \n
//! When the buffer pointed to be \a pbData is too small to contain the requested data, the error ERROR_MORE_DATA
//! is set through SetLastError ().
//! Since the key material is stored on a smartcard, the parameter PP_KEYSET_SEC_DESCR is not supported. Asking for
//! this parameter any way will set the error NTE_BAD_TYPE through SetLastError ().
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL WINAPI
eidmwGetProvParam(
    IN  HCRYPTPROV hProv,      //!< Handle to provider previously obtained through a call to CPAcquireContext
    IN  DWORD dwParam,         //!< Parameter to get   
    OUT LPBYTE pbData,         //!< Data of the parameter
    IN OUT LPDWORD pcbDataLen, //!< Length of the data to be returned
    IN  DWORD dwFlags)         //!< Flags for a certain parameter
{
	BOOL bReturnVal = FALSE;
	DWORD dwLastError = 0;

	__CSP_TRY__

	MWLOG(LEV_INFO, MOD_CSP, L"CPGetProvParam(hProv = %d) called", hProv);
	MWLOG(LEV_INFO, MOD_CSP, L"  - IN: dwParam = 0x%0x", dwParam);
	MWLOG(LEV_INFO, MOD_CSP, L"  - IN: pbData = 0x%p, *pcbDataLen = %d", pbData, pcbDataLen);
	MWLOG(LEV_INFO, MOD_CSP, L"  - IN: dwFlags = 0x%0x", dwFlags);

	CProviderContext *poProvCtx = g_oProviderContextPool.GetProviderContext(hProv);

	DWORD dwValue;
	switch(dwParam)
	{
	case PP_CONTAINER:
	case PP_UNIQUE_CONTAINER:
		{
		std::string csContainer = poProvCtx->GetContainerName();
		bReturnVal = FillProvParam(pbData, pcbDataLen,
			csContainer.c_str(), csContainer.size() + 1);
		break;
		}
	case PP_ENUMALGS:
		bReturnVal = g_oProviderAlgorithms.EnumAlgs(pbData, pcbDataLen, dwFlags);
		break;
	case PP_ENUMALGS_EX:
		bReturnVal = g_oProviderAlgorithms.EnumAlgsEx(pbData, pcbDataLen, dwFlags);
		break;
	case PP_ENUMCONTAINERS:
		bReturnVal = m_ProviderContainers.EnumContainers(pbData, pcbDataLen, dwFlags);
		break;
	case PP_IMPTYPE:
		dwValue = CRYPT_IMPL_MIXED;
		bReturnVal = FillProvParam(pbData, pcbDataLen, &dwValue, sizeof(DWORD));
		break;
	case PP_NAME:
		bReturnVal = FillProvParam(pbData, pcbDataLen, CSP_NAME, strlen(CSP_NAME) + 1);
		break;
	case PP_VERSION:
		dwValue = CSP_VERSION;
		bReturnVal = FillProvParam(pbData, pcbDataLen, &dwValue, sizeof(DWORD));
		break;
	case PP_PROVTYPE:
		dwValue = PROV_RSA_FULL;
		bReturnVal = FillProvParam(pbData, pcbDataLen, &dwValue, sizeof(DWORD));
		break;
	default:
		dwLastError = NTE_BAD_TYPE;
	}

	__STORE_LASTERROR__

	MWLOG(LEV_INFO, MOD_CSP, L" Returning CPGetProvParam(hProv = %d) with code 0x%0x",
		hProv, bReturnVal ? 0 : dwLastError);
	if (bReturnVal)
		MWLOG(LEV_INFO, MOD_CSP, L"   - OUT: *pcbDataLen = %d", *pcbDataLen);

	__CSP_CATCH_SETLASTERROR__

	return (bReturnVal);
}

BOOL WINAPI
eidmwCreateHash(
    IN  HCRYPTPROV hProv,   //!< Handle to provider previously obtained through a call to CPAcquireContext
    IN  ALG_ID Algid,       //!< Algorithm to be used during the hashing operation
    IN  HCRYPTKEY hKey,     //!< Optional handle to a key
    IN  DWORD dwFlags,      //!< Currently no flags defined
    OUT HCRYPTHASH *phHash) //!< Handle to the hash object
{
	BOOL bReturnVal = TRUE;
	DWORD dwLastError = 0;

	__CSP_TRY__

	MWLOG(LEV_INFO, MOD_CSP, L"CPCreateHash(hProv = %d) called", hProv);
	MWLOG(LEV_INFO, MOD_CSP, L"  - IN: Algid = 0x%0x (%d)", Algid, Algid);

	CProviderContext *poProvCtx = g_oProviderContextPool.GetProviderContext(hProv);
	
	*phHash = poProvCtx->AddHash(Algid);

	__STORE_LASTERROR__

	MWLOG(LEV_INFO, MOD_CSP, L" Returning CPCreateHash(hProv = %d) with code 0x%0x",
		hProv, bReturnVal ? 0 : dwLastError);
	if (bReturnVal)
		MWLOG(LEV_INFO, MOD_CSP, L"   - OUT: *phHash = %d", *phHash);
	
	__CSP_CATCH_SETLASTERROR__

	return (bReturnVal);
}

BOOL WINAPI
eidmwDuplicateHash(
    IN  HCRYPTPROV hProv,     //!< Handle to provider previously obtained through a call to CPAcquireContext
    IN  HCRYPTHASH hHash,     //!< Handle to the hash object to be duplicated
    IN  LPDWORD pdwReserved,  //!< Reserved
    IN  DWORD dwFlags,        //!< Reserved for future use, should be 0
    OUT HCRYPTHASH *phHash)   //!< Handle to the new (duplicated) hash object
{
	BOOL bReturnVal = TRUE;
	DWORD dwLastError = 0;

	__CSP_TRY__

	MWLOG(LEV_INFO, MOD_CSP, L"CPDuplicateHash(hProv = %d) called", hProv);
	MWLOG(LEV_INFO, MOD_CSP, L"  - IN: hHash = %d", hHash);

	CProviderContext *poProvCtx = g_oProviderContextPool.GetProviderContext(hProv);

	*phHash = poProvCtx->DuplicateHash(hHash);

	__STORE_LASTERROR__

	MWLOG(LEV_INFO, MOD_CSP, L" Returning CPDuplicateHash(hProv = %d) with code 0x%0x",
		hProv, bReturnVal ? 0 : dwLastError);
	if (bReturnVal)
		MWLOG(LEV_INFO, MOD_CSP, L"   - OUT: *phHash = %d", *phHash);

	__CSP_CATCH_SETLASTERROR__

	return (bReturnVal);
}

BOOL WINAPI
eidmwHashData(
    IN  HCRYPTPROV hProv,    //!< Handle to provider previously obtained through a call to CPAcquireContext
    IN  HCRYPTHASH hHash,    //!< Handle of the hash object to use for this operation
    IN  CONST BYTE *pbData,  //!< Pointer to a buffer containing the data to be hashed
    IN  DWORD cbDataLen,     //!< Length of the data to be hashed
    IN  DWORD dwFlags)       //!< No flags currently supported
{
	BOOL bReturnVal = TRUE;
	DWORD dwLastError = 0;

	__CSP_TRY__

	MWLOG(LEV_INFO, MOD_CSP, L"CPHashData(hProv = %d) called", hProv);
	MWLOG(LEV_INFO, MOD_CSP, L"  - IN: hHash = %d", hHash);
	MWLOG(LEV_INFO, MOD_CSP, L"  - IN: cbDataLen = %d", cbDataLen);

	CProviderContext *poProvCtx = g_oProviderContextPool.GetProviderContext(hProv);
	CProviderHash *poProvHash = poProvCtx->GetHash(hHash);

	poProvHash->Update(CByteArray(pbData, cbDataLen));

	__STORE_LASTERROR__

	MWLOG(LEV_INFO, MOD_CSP, L" Returning CPHashData(hProv = %d) with code 0x%0x",
		hProv, bReturnVal ? 0 : dwLastError);

	__CSP_CATCH_SETLASTERROR__

	return (bReturnVal);
}

BOOL WINAPI
eidmwHashSessionKey(
    IN  HCRYPTPROV hProv,  //!< Handle to provider previously obtained through a call to CPAcquireContext
    IN  HCRYPTHASH hHash,  //!< Handle to an existing hash object
    IN  HCRYPTKEY hKey,    //!< Handle to the session key that needs to be hashed
    IN  DWORD dwFlags)     //!< Currently there are no flags defined
{
	BOOL bReturnVal = FALSE;

	MWLOG(LEV_INFO, MOD_CSP, L"CPHashSessionKey(hProv = %d) called", hProv);

	MWLOG(LEV_INFO, MOD_CSP, L" Returning CPHashSessionKey(hProv = %d) with E_NOTIMPL", hProv);

	SetLastError(E_NOTIMPL);

	return bReturnVal;
}

BOOL WINAPI
eidmwSetHashParam(
    IN  HCRYPTPROV hProv,   //!< Handle to provider previously obtained through a call to CPAcquireContext
    IN  HCRYPTHASH hHash,   //!< A reference to the hash object for which to set the parameter
    IN  DWORD dwParam,      //!< Parameter to set
    IN  CONST BYTE *pbData, //!< Data for the parameter to set
    IN  DWORD dwFlags)      //!< No flags currently defined
{
	BOOL bReturnVal = TRUE;
	DWORD dwLastError = 0;

	__CSP_TRY__

	MWLOG(LEV_INFO, MOD_CSP, L"CPSetHashParam(hProv = %d) called", hProv);
	MWLOG(LEV_INFO, MOD_CSP, L"  - IN: hHash = %d", hHash);
	MWLOG(LEV_INFO, MOD_CSP, L"  - IN: dwParam = 0x%0x", dwParam);

	CProviderContext *poProvCtx = g_oProviderContextPool.GetProviderContext(hProv);
	CProviderHash *poProvHash = poProvCtx->GetHash(hHash);

	switch(dwParam)
	{
	case HP_HASHVAL:
		{
			poProvHash->SetHashValue(CByteArray(pbData, poProvHash->GetLengthBytes()));
		break;
		}
	default:
		dwLastError = NTE_BAD_TYPE;
		bReturnVal = FALSE;
	}

	__STORE_LASTERROR__

	MWLOG(LEV_INFO, MOD_CSP, L" Returning CPSetHashParam(hProv = %d) with code 0x%0x",
		hProv, bReturnVal ? 0 : dwLastError);

	__CSP_CATCH_SETLASTERROR__

	return (bReturnVal);
}

BOOL WINAPI
eidmwGetHashParam(
    IN  HCRYPTPROV hProv,      //!< Handle to provider previously obtained through a call to CPAcquireContext
    IN  HCRYPTHASH hHash,      //!< A reference to the hash object for which to set the parameter
    IN  DWORD dwParam,         //!< Parameter to set
    OUT LPBYTE pbData,         //!< A data buffer in which the parameter data will be returned
    IN OUT LPDWORD pcbDataLen, //!< Length of the data to be returned
    IN  DWORD dwFlags)         //!< No flags currently defined
{
	BOOL bReturnVal = FALSE;
	DWORD dwLastError = 0;

	__CSP_TRY__

	MWLOG(LEV_INFO, MOD_CSP, L"CPGetHashParam(hProv = %d) called", hProv);
	MWLOG(LEV_INFO, MOD_CSP, L"  - IN: hHash = %d", hHash);
	MWLOG(LEV_INFO, MOD_CSP, L"  - IN: dwParam = 0x%0x", dwParam);
	MWLOG(LEV_INFO, MOD_CSP, L"  - IN: pbData = %p", pbData);

	CProviderContext *poProvCtx = g_oProviderContextPool.GetProviderContext(hProv);
	CProviderHash *poProvHash = poProvCtx->GetHash(hHash);

	switch(dwParam)
	{
	case HP_ALGID:
		{
		DWORD dwAlgID = (DWORD) poProvHash->GetAlgid();
		bReturnVal = FillProvParam(pbData, pcbDataLen, &dwAlgID, sizeof(DWORD));
		break;
		}
	case HP_HASHSIZE:
		{
		DWORD dwHashSize = (DWORD) poProvHash->GetLengthBytes();
		bReturnVal = FillProvParam(pbData, pcbDataLen, &dwHashSize, sizeof(DWORD));
		break;
		}
	case HP_HASHVAL:
		{
		CByteArray oHashValue = poProvHash->GetHashValue();
		bReturnVal = FillProvParam(pbData, pcbDataLen, oHashValue.GetBytes(), oHashValue.Size());
		break;
		}
	default:
		dwLastError = NTE_BAD_TYPE;
	}

	__STORE_LASTERROR__

	MWLOG(LEV_INFO, MOD_CSP, L" Returning CPGetHashParam(hProv = %d) with code 0x%0x",
		hProv, bReturnVal ? 0 : dwLastError);
	if (bReturnVal)
		MWLOG(LEV_INFO, MOD_CSP, L"   - OUT: *pcbDataLen = %d", *pcbDataLen);

	__CSP_CATCH_SETLASTERROR__

	return (bReturnVal);
}

BOOL WINAPI
eidmwDestroyHash(
    IN  HCRYPTPROV hProv,  //!< Handle to provider previously obtained through a call to CPAcquireContext
    IN  HCRYPTHASH hHash)  //!< Handle to the hash object to destroy
{
	BOOL bReturnVal = TRUE;
	DWORD dwLastError = 0;

	__CSP_TRY__

	MWLOG(LEV_INFO, MOD_CSP, L"CPDestroyHash(hProv = %d) called", hProv);
	MWLOG(LEV_INFO, MOD_CSP, L"  - IN: hHash = %d", hHash);

	CProviderContext *poProvCtx = g_oProviderContextPool.GetProviderContext(hProv);

	bReturnVal = poProvCtx->DeleteHash(hHash);
	if (!bReturnVal)
		dwLastError = NTE_BAD_HASH;

	__STORE_LASTERROR__

	MWLOG(LEV_INFO, MOD_CSP, L" Returning CPDestroyHash(hProv = %d) with code 0x%0x",
		hProv, bReturnVal ? 0 : dwLastError);

	__CSP_CATCH_SETLASTERROR__

	return (bReturnVal);
}

BOOL WINAPI
eidmwSignHash(
    IN  HCRYPTPROV hProv,      //!< Handle to provider previously obtained through a call to CPAcquireContext
    IN  HCRYPTHASH hHash,      //!< Handle to the hash object to be signed
    IN  DWORD dwKeySpec,       //!< Key specifier of the key with which to sign
    IN  LPCWSTR szDescription, //!< Discription of the signature (this parameter should no longer be used)
    IN  DWORD dwFlags,         //!<Currently no flags defined, should be 0
    OUT LPBYTE pbSignature,    //!< Pointer to an output buffer that will contain the resulting signature
    IN OUT LPDWORD pcbSigLen)  //!< Length of the resulting signature 
{
	BOOL bReturnVal = FALSE;
	DWORD dwLastError = 0;

	__CSP_TRY__

	MWLOG(LEV_INFO, MOD_CSP, L"CPSignHash(hProv = %d) called", hProv);
	MWLOG(LEV_INFO, MOD_CSP, L"  - IN: hHash = %d", hHash);
	MWLOG(LEV_INFO, MOD_CSP, L"  - IN: dwKeySpec = %d", dwKeySpec);
	MWLOG(LEV_INFO, MOD_CSP, L"  - IN: pbSignature = %p", pbSignature);

	CProviderContext *poProvCtx = g_oProviderContextPool.GetProviderContext(hProv);
	CProviderHash *poProvHash = poProvCtx->GetHash(hHash, false);
	std::string csContainerName = poProvCtx->GetContainerName();

	if (csContainerName == "")
	{
		dwLastError = NTE_NO_KEY;
	}
	else if (poProvHash == NULL)
	{
		dwLastError = NTE_BAD_HASH;
	}
	else
	{
		CAutoMutex oAutoMutex(&g_oCalMutex);

		CReader *poReader = FindCard(poProvCtx);
		if (poReader == NULL)
		{
			dwLastError = NTE_NO_KEY;
		}
		else
		{
			tPrivKey privKey = FindKey(poReader, csContainerName);

			if (pbSignature == NULL)
			{
				*pcbSigLen = privKey.ulKeyLenBytes;
				if (*pcbSigLen == 0)
					*pcbSigLen = 128; // Default, just in case
				bReturnVal = TRUE;
			}
			else
			{
				unsigned long ulSignAlgo = GetSignAlgo(poProvHash->GetAlgid());
				CByteArray oSignature;
				// If the hash has already been calculated (or given via CPSetHashParam())
				// then we present this value. Otherwise we present the hash object itself
				CByteArray & oHashValue = poProvHash->GetHashValue();
				if (oHashValue.Size() == 0)
					oSignature = poReader->Sign(privKey, ulSignAlgo, poProvHash->GetHashObject());
				else
					oSignature = poReader->Sign(privKey, ulSignAlgo, oHashValue);

				bReturnVal = FillProvParam(pbSignature, pcbSigLen,
					oSignature.GetBytes(), oSignature.Size());
				if (bReturnVal)
					ReverseByteOrder(pbSignature, *pcbSigLen);
			}
		}
	}

	__STORE_LASTERROR__

	MWLOG(LEV_INFO, MOD_CSP, L" Returning CPSignHash(hProv = %d) with code 0x%0x",
		hProv, bReturnVal ? 0 : dwLastError);
	if (bReturnVal)
		MWLOG(LEV_INFO, MOD_CSP, L"   - OUT: *pcbSigLen = %d", *pcbSigLen);

	__CSP_CATCH_SETLASTERROR__

	return (bReturnVal);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \brief
//! This function returns a handle to a key stored in a container managed by the CSP

//! This function returns a handle to the public key of the certificate defined through CPAcquireContext. The public
//! key info extracted from the certificate is then fed to the base CSP. The handle received from the base CSP is 
//! then forwarded to the calling process.\n
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL WINAPI
eidmwGetUserKey(
    IN  HCRYPTPROV hProv,      //!< Handle to provider previously obtained through a call to CPAcquireContext
    IN  DWORD dwKeySpec,       //!< Type of key to get
    OUT HCRYPTKEY *phUserKey)  //!< Handle to the requested key
{
	BOOL bReturnVal = FALSE;
	DWORD dwLastError = 0;

	__CSP_TRY__

	MWLOG(LEV_INFO, MOD_CSP, L"CPGetUserKey(hProv = %d) called", hProv);

	CProviderContext *poProvCtx = g_oProviderContextPool.GetProviderContext(hProv);

	std::string csContainerName = poProvCtx->GetContainerName();
	if (csContainerName != "")
	{
		CERT_PUBLIC_KEY_INFO xPubKeyInfo;
		xPubKeyInfo.Algorithm.Parameters.pbData = NULL;
		xPubKeyInfo.PublicKey.pbData = NULL;

		bReturnVal = m_ProviderContainers.GetPublicKeyInfo(csContainerName, &xPubKeyInfo);
		if (bReturnVal)
		{
			bReturnVal = poProvCtx->GetBaseProvider().ImportKey(
				&xPubKeyInfo, dwKeySpec, phUserKey);
			FreePubKeyInfo(&xPubKeyInfo);
		}
	}
	else
	{
		dwLastError = NTE_NO_KEY;
	}

	__STORE_LASTERROR__

	MWLOG(LEV_INFO, MOD_CSP, L" Returning CPGetUserKey(hProv = %d) with code 0x%0x",
		hProv, bReturnVal ? 0 : dwLastError);
	if (bReturnVal)
		MWLOG(LEV_INFO, MOD_CSP, L"   - OUT: *phUserKey = %d", *phUserKey);

	__CSP_CATCH_SETLASTERROR__

	return (bReturnVal);
}

BOOL WINAPI
eidmwGenKey(
    IN  HCRYPTPROV hProv,  //!< Handle to provider previously obtained through a call to CPAcquireContext
    IN  ALG_ID Algid,      //!< Identifier for the algorithm for which the key is to be generated
    IN  DWORD dwFlags,     //!< Flags specifying attributes pertaining to the generated key
    OUT HCRYPTKEY *phKey)  //!< Handle to the newly generated key
{
	BOOL bReturnVal= FALSE;

	MWLOG(LEV_INFO, MOD_CSP, L"CPGenKey(hProv = %d) called", hProv);

	MWLOG(LEV_INFO, MOD_CSP, L" Returning CPGenKey(hProv = %d) with E_NOTIMPL", hProv);

	SetLastError(E_NOTIMPL);

	return (bReturnVal);
}

BOOL WINAPI
eidmwDeriveKey(
    IN  HCRYPTPROV hProv, //!< Handle to provider previously obtained through a call to CPAcquireContext
    IN  ALG_ID Algid,     //!< Identifier for the algorithm for which the key is to be generated
    IN  HCRYPTHASH hHash, //!< Handle to a hash object that has been fed exactly the base data
    IN  DWORD dwFlags,    //!< Flags specifying attributes pertaining to the session key generated
    OUT HCRYPTKEY *phKey) //!< Handle to the newly generated key
{
	BOOL bReturnVal = FALSE;

	MWLOG(LEV_INFO, MOD_CSP, L"CPDeriveKey(hProv = %d) called", hProv);

	MWLOG(LEV_INFO, MOD_CSP, L" Returning CPDeriveKey(hProv = %d) with E_NOTIMPL", hProv);

	SetLastError(E_NOTIMPL);

	return (bReturnVal);
}

BOOL WINAPI
eidmwSetKeyParam(
    IN  HCRYPTPROV hProv,   //!< Handle to provider previously obtained through a call to CPAcquireContext
    IN  HCRYPTKEY hKey,     //!< Handle to the key for which to set the parameters
    IN  DWORD dwParam,      //!< Parameter to set
    IN  CONST BYTE *pbData, //!< Data to set
    IN  DWORD dwFlags)      //!< Currently not used
{
	BOOL bReturnVal = FALSE;

	MWLOG(LEV_INFO, MOD_CSP, L"CPSetKeyParam(hProv = %d) called", hProv);

	MWLOG(LEV_INFO, MOD_CSP, L" Returning CPSetKeyParam(hProv = %d) with E_NOTIMPL", hProv);

	SetLastError(E_NOTIMPL);

	return (bReturnVal);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \brief
//! This function gets parameters for a given key

//! This function gets parameters from a previously obtained handle to a key. In this CSP this handle could only be
//! obtained through a call to CPGetUserKey. The actual code to destroy the key is delegated to the base CSP.
//! The KP_ALGID parameter is required in order to handle an incompatibility between .Net 1.0 and more recent versions
BOOL WINAPI
eidmwGetKeyParam(
    IN  HCRYPTPROV hProv,       //!< Handle to provider previously obtained through a call to CPAcquireContext
    IN  HCRYPTKEY hKey,         //!< Handle to the key for which to set the parameters
    IN  DWORD dwParam,          //!< Parameter to get
    OUT LPBYTE pbData,          //!< Data for the requested parameter
    IN OUT LPDWORD pcbDataLen,  //!< Length of the requested data
    IN  DWORD dwFlags)          //!< Currently not used
{
	BOOL bReturnVal = TRUE;
	DWORD dwLastError = 0;

	__CSP_TRY__

	MWLOG(LEV_INFO, MOD_CSP, L"CPGetKeyParam(hProv = %d) called", hProv);
	MWLOG(LEV_INFO, MOD_CSP, L"  - IN hKey: %d", hKey);
	MWLOG(LEV_INFO, MOD_CSP, L"  - IN: dwParam = 0x%0x", dwParam);
	MWLOG(LEV_INFO, MOD_CSP, L"  - IN: pbData = 0x%p, *pcbDataLen = %d", pbData, pcbDataLen);
	MWLOG(LEV_INFO, MOD_CSP, L"  - IN: dwFlags = 0x%0x", dwFlags);

	CProviderContext *poProvCtx = g_oProviderContextPool.GetProviderContext(hProv);

	bReturnVal = poProvCtx->GetBaseProvider().GetKeyParam(hKey, dwParam, pbData, pcbDataLen, dwFlags);

	__STORE_LASTERROR__

	MWLOG(LEV_INFO, MOD_CSP, L" Returning CPGetKeyParam(hProv = %d) with code 0x%0x",
		hProv, bReturnVal ? 0 : dwLastError);
	if (bReturnVal)
		MWLOG(LEV_INFO, MOD_CSP, L"   - OUT: *pcbDataLen = %d", *pcbDataLen);

	__CSP_CATCH_SETLASTERROR__

	return (bReturnVal);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \brief
//! This function exports the given key from the CSP container

//! This function export only public keys. Since the private keys are stored on smartcard and are not exportable, it
//! does not make sense to allow any other blob type than PUBLICKEYBLOB. Specifying another blob type will cause
//! the error NTE_BAD_TYPE to be returned. Since only public key blobs can be returned, the handle \a hPubKey must
//! be set to NULL. Not doing so will cause the error NTE_BAD_KEY to be set. The actual operation of this function is
//! delegated to the CryptExportKey function of the base CSP. \n
//! The handle to the key to be exported can be obtainer through a call to CPGetUserKey. Depending on which
//! container was selected in CPAcquireContext different public keys will be returned.
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL WINAPI
eidmwExportKey(
    IN  HCRYPTPROV hProv,       //!< Handle to provider previously obtained through a call to CPAcquireContext
    IN  HCRYPTKEY hKey,         //!< Handle of the key to export
    IN  HCRYPTKEY hPubKey,      //!< Handle of the exchange public key of the recipient of the exported key 
    IN  DWORD dwBlobType,       //!< Type of key blob to export
    IN  DWORD dwFlags,          //!< No flags currently defined
    OUT LPBYTE pbData,          //!< Data buffer to contain the exported key
    IN OUT LPDWORD pcbDataLen)  //!< Length of the data stored in previous argument
{
	BOOL bReturnVal = TRUE;
	DWORD dwLastError = 0;

	__CSP_TRY__

	MWLOG(LEV_INFO, MOD_CSP, L"CPExportKey(hProv = %d) called", hProv);
	MWLOG(LEV_INFO, MOD_CSP, L"  - IN hKey: %d", hKey);
	MWLOG(LEV_INFO, MOD_CSP, L"  - IN hPubKey: %d", hPubKey);
	MWLOG(LEV_INFO, MOD_CSP, L"  - IN dwBlobType: 0x%0x", dwBlobType);
	MWLOG(LEV_INFO, MOD_CSP, L"  - IN: dwFlags = 0x%0x", dwFlags);
	MWLOG(LEV_INFO, MOD_CSP, L"  - IN: pbData = 0x%p, *pcbDataLen = %d", pbData, pcbDataLen);

	CProviderContext *poProvCtx = g_oProviderContextPool.GetProviderContext(hProv);

	bReturnVal = poProvCtx->GetBaseProvider().ExportKey(hKey, hPubKey, dwBlobType,
		dwFlags, pbData, pcbDataLen);

	__STORE_LASTERROR__

	MWLOG(LEV_INFO, MOD_CSP, L" Returning CPExportKey(hProv = %d) with code 0x%0x",
		hProv, bReturnVal ? 0 : dwLastError);
	if (bReturnVal)
		MWLOG(LEV_INFO, MOD_CSP, L"   - OUT: *pcbDataLen = %d", *pcbDataLen);

	__CSP_CATCH_SETLASTERROR__

	return (bReturnVal);
}

BOOL WINAPI
eidmwImportKey(
    IN  HCRYPTPROV hProv,    //!< Handle to provider previously obtained through a call to CPAcquireContext
    IN  CONST BYTE *pbData,  //!< Data buffer containing the key to import
    IN  DWORD cbDataLen,     //!< Length of the data returned
    IN  HCRYPTKEY hPubKey,   //!< Handle to the public key imported into the CSP
    IN  DWORD dwFlags,       //!< Flags irt. this operation
    OUT HCRYPTKEY *phKey)    //!< Handle to the imported key
{
	BOOL bReturnVal = FALSE;

	MWLOG(LEV_INFO, MOD_CSP, L"CPImportKey(hProv = %d) called", hProv);

	MWLOG(LEV_INFO, MOD_CSP, L" Returning CPImportKey(hProv = %d) with E_NOTIMPL", hProv);

	SetLastError(E_NOTIMPL);

	return (bReturnVal);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \brief
//! This function derives cryptographic keys from base data

//! This function destroys a previously obtained handle to a key. In this CSP this handle could only be obtained through
//! call to CPGetUserKey. The actual code to destroy the key is delegated to the base CSP.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL WINAPI
eidmwDestroyKey(
    IN  HCRYPTPROV hProv, //!< Handle to provider previously obtained through a call to CPAcquireContext
    IN  HCRYPTKEY hKey)   //!< Handle to the key to be deleted
{
	BOOL bReturnVal = TRUE;
	DWORD dwLastError = 0;

	__CSP_TRY__

	MWLOG(LEV_INFO, MOD_CSP, L"CPDestroyKey(hProv = %d) called", hProv);
	MWLOG(LEV_INFO, MOD_CSP, L"  - IN hKey: %d", hKey);

	CProviderContext *poProvCtx = g_oProviderContextPool.GetProviderContext(hProv);

	bReturnVal = poProvCtx->GetBaseProvider().DestroyKey(hKey);

	__STORE_LASTERROR__

	MWLOG(LEV_INFO, MOD_CSP, L" Returning CPDestroyKey(hProv = %d) with code 0x%0x",
		hProv, bReturnVal ? 0 : dwLastError);

	__CSP_CATCH_SETLASTERROR__

	return (bReturnVal);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \brief
//! This function verifies a digital signature

//! This function verifies a digital signature using the public key that was obtained through \a GetUserKey. The actual
//! signature verification is performed by the base CSP.
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL WINAPI
eidmwVerifySignature(
    IN  HCRYPTPROV hProv,         //!< Handle to provider previously obtained through a call to CPAcquireContext
    IN  HCRYPTHASH hHash,         //!< Handle to the hash object to be signed/verified
    IN  CONST BYTE *pbSignature,  //!< Pointer to signature data to be verified
    IN  DWORD cbSigLen,           //!< Length of the signature data
    IN  HCRYPTKEY hPubKey,        //!< Handle to the public key with which to verify the signature
    IN  LPCWSTR szDescription,    //!< String describing the signed data
    IN  DWORD dwFlags)            //!< No flags are currently defined
{
	BOOL bReturnVal = FALSE;
	DWORD dwLastError = 0;

	__CSP_TRY__

	MWLOG(LEV_INFO, MOD_CSP, L"CPVerifySignature(hProv = %d) called", hProv);
	MWLOG(LEV_INFO, MOD_CSP, L"  - IN: hHash = %d", hHash);
	MWLOG(LEV_INFO, MOD_CSP, L"  - IN: csSigLen = %d", cbSigLen);
	MWLOG(LEV_INFO, MOD_CSP, L"  - IN: hPubKey = 0x%0x", hPubKey);
	MWLOG(LEV_INFO, MOD_CSP, L"  - IN: dwFlags = 0x%0x", dwFlags);

	CProviderContext *poProvCtx = g_oProviderContextPool.GetProviderContext(hProv);

	CProviderHash *poProvHash = poProvCtx->GetHash(hHash);
	if (poProvHash == NULL)
	{
		dwLastError = NTE_BAD_HASH;
	}
	else
	{
		bReturnVal = poProvCtx->GetBaseProvider().VerifySignature(*poProvHash,
			pbSignature, cbSigLen, hPubKey, szDescription, dwFlags);
	}

	__STORE_LASTERROR__

	MWLOG(LEV_INFO, MOD_CSP, L" Returning CPVerifySignature(hProv = %d) with code 0x%0x",
		hProv, bReturnVal ? 0 : dwLastError);

	__CSP_CATCH_SETLASTERROR__

	return (bReturnVal);
}

BOOL WINAPI
eidmwDuplicateKey(
    IN  HCRYPTPROV hProv,     //!< Handle to provider previously obtained through a call to CPAcquireContext
    IN  HCRYPTKEY hKey,       //!< Handle of the key to duplicate
    IN  LPDWORD pdwReserved,  //!< Reserved
    IN  DWORD dwFlags,        //!< Reserved for future use, should be 0
    OUT HCRYPTKEY *phKey)     //!< Handle to the new (duplicated) key
{
	BOOL bReturnVal = TRUE;

	MWLOG(LEV_INFO, MOD_CSP, L"CPDuplicateKey(hProv = %d) called", hProv);

	MWLOG(LEV_INFO, MOD_CSP, L" Returning CPDuplicateKey(hProv = %d) with E_NOTIMPL", hProv);

	SetLastError(E_NOTIMPL);

	return (FALSE);
}

BOOL WINAPI
eidmwGenRandom(
    IN  HCRYPTPROV hProv,  //!< Handle to provider previously obtained through a call to CPAcquireContext
    IN  DWORD cbLen,       //!< Number of bytes of random data requested
    OUT LPBYTE pbBuffer)   //!< Buffer to store the random data
{
	BOOL bReturnVal = FALSE;
	DWORD dwLastError = 0;

	__CSP_TRY__

	MWLOG(LEV_INFO, MOD_CSP, L"CPGenRandom(hProv = %d) called", hProv);
	MWLOG(LEV_INFO, MOD_CSP, L"  - IN: cbLen = %d", cbLen);

	CProviderContext *poProvCtx = g_oProviderContextPool.GetProviderContext(hProv);
	std::string csContainerName = poProvCtx->GetContainerName();

	if (csContainerName == "")
	{
		dwLastError = NTE_NO_KEY;
	}
	else
	{
		CAutoMutex oAutoMutex(&g_oCalMutex);

		CReader *poReader = FindCard(poProvCtx);
		if (poReader == NULL)
		{
			dwLastError = NTE_FAIL;
		}
		else
		{
			CByteArray oRandom = poReader->GetRandom(cbLen);
			memcpy(pbBuffer, oRandom.GetBytes(), oRandom.Size());
			bReturnVal = TRUE;
		}
	}

	__STORE_LASTERROR__

	MWLOG(LEV_INFO, MOD_CSP, L" Returning CPGenRandom(hProv = %d) with code 0x%0x",
		hProv, bReturnVal ? 0 : dwLastError);

	__CSP_CATCH_SETLASTERROR__

	return (bReturnVal);
}

BOOL WINAPI
eidmwEncrypt(
    IN  HCRYPTPROV hProv,       //!< Handle to provider previously obtained through a call to CPAcquireContext
    IN  HCRYPTKEY hKey,         //!< Handle to the key used for the encryption process
    IN  HCRYPTHASH hHash,       //!< Handle to a hash object in case a hash needs to be calculated simultaneously
    IN  BOOL fFinal,            //!< Is this the last block in a sequence?
    IN  DWORD dwFlags,          //!< Currently no flags defined
    IN OUT LPBYTE pbData,       //!< Buffer container the plain text before and encrypted text after the operation
    IN OUT LPDWORD pcbDataLen,  //!< Length of the data entered and returned
    IN  DWORD cbBufLen)         //!< Size in bytes of the \a pbData buffer
{
	BOOL bReturnVal = FALSE;

	MWLOG(LEV_INFO, MOD_CSP, L"CPEncrypt(hProv = %d) called", hProv);

	MWLOG(LEV_INFO, MOD_CSP, L" Returning CPEncrypt(hProv = %d) with E_NOTIMPL", hProv);

	SetLastError(E_NOTIMPL);

	return (bReturnVal);
}

BOOL WINAPI
eidmwDecrypt(
    IN  HCRYPTPROV hProv,      //!< Handle to provider previously obtained through a call to CPAcquireContext
    IN  HCRYPTKEY hKey,        //!< Handle to the key used for the decryption process
    IN  HCRYPTHASH hHash,      //!< Handle to a hash object in case a hash needs to be calculated simultaneously
    IN  BOOL fFinal,           //!< Is this the last block in a sequence?
    IN  DWORD dwFlags,         //!< Currently no flags defined
    IN OUT LPBYTE pbData,      //!< IN: the data to be decrypted - OUT: plaintext 
    IN OUT LPDWORD pcbDataLen) //!< Length of the data entered and returned
{
	BOOL bReturnVal = FALSE;

	MWLOG(LEV_INFO, MOD_CSP, L"CPDecrypt(hProv = %d) called", hProv);

	MWLOG(LEV_INFO, MOD_CSP, L" Returning CPDecrypt(hProv = %d) with E_NOTIMPL", hProv);

	SetLastError(E_NOTIMPL);

	return (bReturnVal);
}

//////////////////////// Non-CryptoAPI functions //////////////////////////

/**
 * Find the reader containing the card that corresponds to the serial number in
 * the container name.
 * If the card couldn't be found then ask for it (at least, if a reader is present)
 * If a reader has been found previously, then check if the card is still present;
 * otherwise look for the card again.
 */
static CReader * FindCard(CProviderContext *poProvCtx)
{
	std::string csSerialNr = poProvCtx->GetSerialNr();
	if (csSerialNr == "")
		return NULL;

	// Get the Reader that (perhaps) we used earlier
	CReader *poReader = poProvCtx->GetReader();

	if (poReader == NULL)
	{
		MWLOG(LEV_INFO, MOD_CSP, L" Searching for the card with serial number \"%ls\"",
			utilStringWiden(csSerialNr).c_str());

try_again:
		// Loop over all readers present, look for a card with this serial nr
		CReadersInfo oReadersInfo = g_oCardLayer.ListReaders();
		CReader *poTmpReader = NULL;
		for (unsigned long i = 0; i < oReadersInfo.ReaderCount(); i++)
		{
			try {
				poTmpReader = &g_oCardLayer.getReader(oReadersInfo.ReaderName(i));
				if (poTmpReader->Connect() && poTmpReader->GetSerialNr() == csSerialNr)
				{
					poReader = poTmpReader;
					break;
				}
			}
			catch(CMWException &e)
			{
				MWLOG(LEV_ERROR, MOD_CSP, L"CReader::Connect(%d) return error code 0x%0x\n", i, e.GetError());
			}
		}

		// If the card couldn't be found, ask the user for it
		if (poReader == NULL && oReadersInfo.ReaderCount() != 0)
		{
			DlgRet ret = DlgDisplayModal(DLG_ICON_INFO, DLG_MESSAGE_ENTER_CORRECT_CARD,
				L"", DLG_BUTTON_OK | DLG_BUTTON_CANCEL,DLG_BUTTON_OK, DLG_BUTTON_CANCEL);
			if (ret == DLG_OK)
				goto try_again;
			else if (ret != DLG_CANCEL)
				MWLOG(LEV_ERROR, MOD_CSP, L"CSP dialog returned error code %d\n", ret);
		}

		// Log
		if (poReader == NULL)
		{
			MWLOG(LEV_INFO, MOD_CSP, L" The card couldn't be found in the %d reader(s) present",
				oReadersInfo.ReaderCount());
		}
		else
		{
			MWLOG(LEV_INFO, MOD_CSP, L" The card was found in reader \"%ls\"",
				utilStringWiden(poReader->GetReaderName()).c_str());
		}
	}
	else
	{
		// If we already found the reader, check if the same
		// card is still inserted
		poReader->Status(true);

		if (poReader->GetSerialNr() != csSerialNr)
		{
			MWLOG(LEV_INFO, MOD_CSP, L" Card no longer present in reader \"%ls\"",
				utilStringWiden(poReader->GetReaderName()).c_str());
			poProvCtx->SetReader(NULL);
			poReader = FindCard(poProvCtx);
		}
	}

	// Keep this Reader for the next time this function is called
	poProvCtx->SetReader(poReader);

	return poReader;
}

/**
 * For now, key containers are named as below:
 * - Signature(534C494E336600296CFF2623660B0826)
 * - Authentication(534C494E336600296CFF2491AA090425)
 * So we'll have to harcode the link between them and
 * the corresponding private key.
 *
 * In the future, we should register key containers
 * e.g. as follows: serialnr_keyid; so we won't have
 * to hardcode this anymore.
 * So we already add the code for this here.
 */
static tPrivKey FindKey(CReader *poReader, const std::string &csContainerName)
{
	const char *csCont = csContainerName.c_str();
	unsigned long ulKeyID = 0;

	const char *csKeyID = strstr("_", csCont);
	if (csKeyID == NULL)
	{
		// Hardcoded mapping
		if (csCont[0] == 'A' && csCont[1] == 'u' && csCont[2] == 't')
		{
#ifdef CSP_PTEID
			ulKeyID = 2;
#endif
#ifdef CSP_PTEID
			ulKeyID = 1;
#endif
		}
		else
		{
#ifdef CSP_PTEID
			ulKeyID = 3;
#endif
#ifdef CSP_PTEID
			ulKeyID = 2;
#endif
		}
	}
	else
	{
		// The key ID should be after the '_'
		csKeyID++;
		while (*csKeyID >= '0' && *csKeyID <= '9')
			ulKeyID = 10 * ulKeyID + (*csKeyID - 0x30);
	}

	tPrivKey key = poReader->GetPrivKeyByID(ulKeyID);
	if (!key.bValid)
	{
		MWLOG(LEV_ERROR, MOD_CSP, L" Key with ID = 0x%0x not found", ulKeyID);
		throw CMWEXCEPTION(EIDMW_ERR_PARAM_BAD);
	}

	MWLOG(LEV_INFO, MOD_CSP, L" Found key \"%ls\" with ID 0x%0x",
		utilStringWiden(key.csLabel).c_str(), ulKeyID);

	return key;
}


