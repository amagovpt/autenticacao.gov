/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2010 FedICT.
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
#include "globmdrv.h"

#include "log.h"
#include "util.h"
#include "smartcard.h"
#include "pteidmdrv.h"
#include "tlv.h"

/****************************************************************************************************/

//
// Function: CardGetContainerProperty
//

#define WHERE "CardGetContainerProperty()"
DWORD WINAPI   CardGetContainerProperty
               (
                  __in                                         PCARD_DATA     pCardData,
                  __in                                         BYTE           bContainerIndex,
                  __in                                         LPCWSTR        wszProperty,
                  __out_bcount_part_opt(cbData, *pdwDataLen)   PBYTE          pbData,
                  __in                                         DWORD          cbData,
                  __out                                        PDWORD         pdwDataLen,
                  __in                                         DWORD          dwFlags
               )
{
   DWORD             dwReturn = 0;
   CONTAINER_INFO    ContInfo;
   DWORD             cbCertif = 0;
   DWORD			 dwCertSpec = 0;
   PBYTE			 pbCertif = NULL;
   PIN_ID            dwPinId = 0;
   LogTrace(LOGTYPE_INFO, WHERE, "Enter API...");

   memset(&ContInfo, '\0', sizeof(ContInfo));

   /********************/
   /* Check Parameters */
   /********************/
   if ( pCardData == NULL )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pCardData]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }
   if ( ( bContainerIndex != 0 ) &&
        ( bContainerIndex != 1 ) )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [bContainerIndex]");
      CLEANUP(SCARD_E_NO_KEY_CONTAINER);
   }
   if ( wszProperty == NULL )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [wszProperty]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }
   if ( pbData == NULL )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pbData]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }
   if ( pdwDataLen == NULL )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pdwDataLen]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }

   if ( ( wcscmp(wszProperty, CCP_CONTAINER_INFO) != 0 ) &&
        ( wcscmp(wszProperty, CCP_PIN_IDENTIFIER) != 0 ) )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [wszProperty]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }
   if ( dwFlags != 0 )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [dwFlags]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }

   if ( wcscmp(wszProperty, CCP_CONTAINER_INFO) == 0 ) 
   {
      LogTrace(LOGTYPE_INFO, WHERE, "Property: [CCP_CONTAINER_INFO] for ContainerIndex: [%d]", bContainerIndex);

      if ( cbData < sizeof(CONTAINER_INFO) )
      {
         LogTrace(LOGTYPE_ERROR, WHERE, "Insufficient buffer[%d]<[%d]", cbData, sizeof(CONTAINER_INFO));
         CLEANUP(ERROR_INSUFFICIENT_BUFFER);
      }
	  if (bContainerIndex == 0) {
		  LogTrace(LOGTYPE_INFO, WHERE, "Creating Authentication Certif...");
		  dwCertSpec = CERT_AUTH;
	  }
	  if (bContainerIndex == 1) {
	  	  LogTrace(LOGTYPE_INFO, WHERE, "Creating Non-Repudiation Certif...");
		  dwCertSpec = CERT_NONREP;
	  }
	  dwReturn = PteidReadCert(pCardData, dwCertSpec, &cbCertif, &pbCertif);
	  if ( dwReturn != SCARD_S_SUCCESS )
	  {
		  if (bContainerIndex == 0)
			  LogTrace(LOGTYPE_ERROR, WHERE, "PteidReadCert[CERT_AUTH] returned [%d]", dwReturn);
		  if (bContainerIndex == 1)
			  LogTrace(LOGTYPE_ERROR, WHERE, "PteidReadCert[CERT_NONREP] returned [%d]", dwReturn);
		  CLEANUP(SCARD_E_UNEXPECTED);
	  }
      ContInfo.dwVersion      = CONTAINER_INFO_CURRENT_VERSION;
      ContInfo.dwReserved     = 0;
  	  dwReturn = PteidGetPubKey(pCardData, 
                            cbCertif, 
                            pbCertif, 
                            &(ContInfo.cbSigPublicKey), 
                            &(ContInfo.pbSigPublicKey));
	  if ( dwReturn != SCARD_S_SUCCESS )
	  {
		  LogTrace(LOGTYPE_ERROR, WHERE, "PteidGetPubKey returned [%d]", dwReturn);
		  CLEANUP(SCARD_E_UNEXPECTED);
	  }

      if ( ContInfo.pbSigPublicKey == NULL )
      {
         LogTrace(LOGTYPE_ERROR, WHERE, "Error allocating memory for public key data (pbSigPublicKey)...");
         CLEANUP(SCARD_E_NO_MEMORY);
      }

      memcpy (pbData, &ContInfo, sizeof(CONTAINER_INFO));

      *pdwDataLen = sizeof(CONTAINER_INFO);
   }
   else
   {
      LogTrace(LOGTYPE_INFO, WHERE, "Property: [CCP_PIN_IDENTIFIER] for ContainerIndex: [%d]", bContainerIndex);

      if ( cbData < sizeof(PIN_ID) )
      {
         LogTrace(LOGTYPE_ERROR, WHERE, "Insufficient buffer[%d]<[%d]", cbData, sizeof(PIN_ID));
         CLEANUP(ERROR_INSUFFICIENT_BUFFER);
      }
	  if (bContainerIndex == 0) {
		  dwPinId = ROLE_DIGSIG;
	  }
	  if (bContainerIndex == 1) {
		  dwPinId = ROLE_NONREP;
	  }
      
      memcpy (pbData, &(dwPinId), sizeof(PIN_ID));
      *pdwDataLen = sizeof(PIN_ID);
   }

cleanup:
   LogTrace(LOGTYPE_INFO, WHERE, "Exit API...");
   return(dwReturn);
}
#undef WHERE

/****************************************************************************************************/

//
// Function: CardSetContainerProperty
//

#define WHERE "CardSetContainerProperty()"
DWORD WINAPI   CardSetContainerProperty
               (
                  __in                    PCARD_DATA  pCardData,
                  __in                    BYTE        bContainerIndex,
                  __in                    LPCWSTR     wszProperty,
                  __in_bcount(cbDataLen)  PBYTE       pbData,
                  __in                    DWORD       cbDataLen,
                  __in                    DWORD       dwFlags
               )
{
   DWORD    dwReturn = 0;
   LogTrace(LOGTYPE_INFO, WHERE, "Enter API...");

   CLEANUP(SCARD_E_UNSUPPORTED_FEATURE);

cleanup:
   LogTrace(LOGTYPE_INFO, WHERE, "Exit API...");
   return(dwReturn);
}
#undef WHERE

/****************************************************************************************************/

//
// Function: CardGetProperty
//

#define WHERE "CardGetFreeSpace"
DWORD CardGetFreeSpace(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, PDWORD pdwDataLen, DWORD dwFlags)
{
   DWORD                   dwReturn    = 0;
   CARD_FREE_SPACE_INFO    SpaceInfo;

   LogTrace(LOGTYPE_INFO, WHERE, "GET Property: [CP_CARD_FREE_SPACE]");

   if ( dwFlags != 0 )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [dwFlags]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }
   if ( cbData < sizeof(SpaceInfo) )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Insufficient buffer");
      CLEANUP(SCARD_E_INSUFFICIENT_BUFFER);
   }

   SpaceInfo.dwVersion                 = CARD_FREE_SPACE_INFO_CURRENT_VERSION;
   /* We have a read-only card */
   SpaceInfo.dwBytesAvailable          = 0;
   SpaceInfo.dwKeyContainersAvailable  = 0;
   SpaceInfo.dwMaxKeyContainers        = 2; /* Authentication and Non-repudiation key */

   memcpy (pbData, &SpaceInfo, sizeof(SpaceInfo));
   *pdwDataLen = sizeof(SpaceInfo);

cleanup:
   return(dwReturn);
}
#undef WHERE

/****************************************************************************************************/

#define WHERE "CardSetFreeSpace"
DWORD CardSetFreeSpace(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, DWORD dwFlags)
{
   DWORD    dwReturn       = 0;

   LogTrace(LOGTYPE_INFO, WHERE, "SET Property: [CP_CARD_FREE_SPACE]");

   CLEANUP(SCARD_E_INVALID_PARAMETER);

cleanup:
   return(dwReturn);
}
#undef WHERE

/****************************************************************************************************/

#define WHERE "CardGetCapabilities"
DWORD CardGetCapabilities(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, PDWORD pdwDataLen, DWORD dwFlags)
{
   DWORD             dwReturn    = 0;
   CARD_CAPABILITIES CardCap;

   LogTrace(LOGTYPE_INFO, WHERE, "GET Property: [CP_CARD_CAPABILITIES]");

   if ( dwFlags != 0 )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [dwFlags]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }

   if ( cbData < sizeof(CardCap) )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Insufficient buffer");
      CLEANUP(ERROR_INSUFFICIENT_BUFFER);
   }

   CardCap.dwVersion                = CARD_CAPABILITIES_CURRENT_VERSION;
   CardCap.fCertificateCompression  = TRUE;
   CardCap.fKeyGen                  = FALSE;

   memcpy (pbData, &CardCap, sizeof(CardCap));
   *pdwDataLen = sizeof(CardCap);

cleanup:
   return(dwReturn);
}
#undef WHERE

/****************************************************************************************************/

#define WHERE "CardSetCapabilities"
DWORD CardSetCapabilities(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, DWORD dwFlags)
{
   DWORD    dwReturn       = 0;

   LogTrace(LOGTYPE_INFO, WHERE, "SET Property: [CP_CARD_CAPABILITIES]");

   CLEANUP(SCARD_E_INVALID_PARAMETER);

cleanup:
   return(dwReturn);
}
#undef WHERE

/****************************************************************************************************/

#define WHERE "CardGetKeysizes"
DWORD CardGetKeysizes(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, PDWORD pdwDataLen, DWORD dwFlags)
{
   DWORD             dwReturn  = 0;

   CARD_KEY_SIZES    KeySizes;

   int               iUnSupported = 0;
   int               iInValid     = 0;

   LogTrace(LOGTYPE_INFO, WHERE, "Property: [CP_CARD_KEYSIZES]");

   switch(dwFlags)
   {
   case AT_ECDHE_P256 :
   case AT_ECDHE_P384 :
   case AT_ECDHE_P521 :
   case AT_ECDSA_P256 :
   case AT_ECDSA_P384 :
   case AT_ECDSA_P521 :
      iUnSupported++;
      break;
   case AT_KEYEXCHANGE:
   case AT_SIGNATURE  :
      break;
   default:
      iInValid++;
      break;
   }
   if ( iInValid )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [dwFlags]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }
   if ( iUnSupported )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Unsupported parameter [dwFlags]");
      CLEANUP(SCARD_E_UNSUPPORTED_FEATURE);
   }

   KeySizes.dwVersion            = CARD_KEY_SIZES_CURRENT_VERSION;
   KeySizes.dwMinimumBitlen      = 1024;
   KeySizes.dwDefaultBitlen      = 1024;
   KeySizes.dwMaximumBitlen      = 2048;
   KeySizes.dwIncrementalBitlen  = 0;

   memcpy (pbData, &KeySizes, sizeof(KeySizes));
   *pdwDataLen = sizeof(KeySizes);

   CLEANUP(SCARD_S_SUCCESS);

cleanup:
   return(dwReturn);
}
#undef WHERE

/****************************************************************************************************/

#define WHERE "CardSetKeysizes"
DWORD CardSetKeysizes(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, DWORD dwFlags)
{
   DWORD    dwReturn       = 0;

   LogTrace(LOGTYPE_INFO, WHERE, "SET Property: [CP_CARD_KEYSIZES]");

   CLEANUP(SCARD_E_INVALID_PARAMETER);

cleanup:
   return(dwReturn);
}
#undef WHERE

/****************************************************************************************************/

#define WHERE "CardGetReadOnly"
DWORD CardGetReadOnly(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, PDWORD pdwDataLen, DWORD dwFlags)
{
   DWORD    dwReturn    = 0;
   BOOL     bReadOnly   = TRUE;

   LogTrace(LOGTYPE_INFO, WHERE, "Property: [CP_CARD_READ_ONLY]");

   if ( dwFlags != 0 )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [dwFlags]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }

   if ( cbData < sizeof(bReadOnly) )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Insufficient buffer");
      CLEANUP(ERROR_INSUFFICIENT_BUFFER);
   }

   memcpy (pbData, &bReadOnly, sizeof(bReadOnly));
   *pdwDataLen = sizeof(bReadOnly);

cleanup:
   return(dwReturn);
}
#undef WHERE

/****************************************************************************************************/

#define WHERE "CardSetReadOnly"
DWORD CardSetReadOnly(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, DWORD dwFlags)
{
   DWORD    dwReturn       = 0;

   LogTrace(LOGTYPE_INFO, WHERE, "SET Property: [CP_CARD_READ_ONLY]");

   CLEANUP(SCARD_E_INVALID_PARAMETER);

cleanup:
   return(dwReturn);
}
#undef WHERE

/****************************************************************************************************/

#define WHERE "CardGetCacheMode"
DWORD CardGetCacheMode(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, PDWORD pdwDataLen, DWORD dwFlags)
{
   DWORD    dwReturn    = 0;
	DWORD    dwCacheMode = CP_CACHE_MODE_NO_CACHE;// CP_CACHE_MODE_NO_CACHE;

   LogTrace(LOGTYPE_INFO, WHERE, "GET Property: [CP_CARD_CACHE_MODE]");

   if ( dwFlags != 0 )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [dwFlags]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }

   if ( cbData < sizeof(dwCacheMode) )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Insufficient buffer");
      CLEANUP(ERROR_INSUFFICIENT_BUFFER);
   }

   memcpy (pbData, &dwCacheMode, sizeof(dwCacheMode));
   *pdwDataLen = sizeof(dwCacheMode);

cleanup:
   return(dwReturn);
}
#undef WHERE

/****************************************************************************************************/

#define WHERE "CardSetCacheMode"
DWORD CardSetCacheMode(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, DWORD dwFlags)
{
   DWORD    dwReturn       = 0;

   LogTrace(LOGTYPE_INFO, WHERE, "SET Property: [CP_CARD_CACHE_MODE]");

   CLEANUP(SCARD_W_SECURITY_VIOLATION);

cleanup:
   return(dwReturn);
}
#undef WHERE

/****************************************************************************************************/

#define WHERE "CardGetSupportsWinX509Enrollment"
DWORD CardGetSupportsWinX509Enrollment(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, PDWORD pdwDataLen, DWORD dwFlags)
{
   DWORD    dwReturn       = 0;
   BOOL     bSupportEnroll = FALSE;

   LogTrace(LOGTYPE_INFO, WHERE, "GET Property: [CP_CARD_SUPPORTS_WIN_X509_ENROLLMENT]");

   if ( dwFlags != 0 )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [dwFlags]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }

   if ( cbData < sizeof(bSupportEnroll) )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Insufficient buffer");
      CLEANUP(ERROR_INSUFFICIENT_BUFFER);
   }

   memcpy (pbData, &bSupportEnroll, sizeof(bSupportEnroll));
   *pdwDataLen = sizeof(bSupportEnroll);

cleanup:
   return(dwReturn);
}
#undef WHERE

/****************************************************************************************************/

#define WHERE "CardSetSupportsWinX509Enrollment"
DWORD CardSetSupportsWinX509Enrollment(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, DWORD dwFlags)
{
   DWORD    dwReturn       = 0;

   LogTrace(LOGTYPE_INFO, WHERE, "SET Property: [CP_CARD_SUPPORTS_WIN_X509_ENROLLMENT]");

   CLEANUP(SCARD_W_SECURITY_VIOLATION);

cleanup:
   return(dwReturn);
}
#undef WHERE

/****************************************************************************************************/

#define WHERE "CardGetGuid"
DWORD CardGetGuid(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, PDWORD pdwDataLen, DWORD dwFlags)
{
   DWORD                   dwReturn         = 0;
   LogTrace(LOGTYPE_INFO, WHERE, "GET Property: [CP_CARD_GUID]");

   if ( dwFlags != 0 )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [dwFlags]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }
	if ( cbData < sizeof(GUID) )
   {
		LogTrace(LOGTYPE_ERROR, WHERE, "Insufficient buffer [%d][%d]", cbData, sizeof(GUID));
		CLEANUP(ERROR_INSUFFICIENT_BUFFER);
      }

	dwReturn = CardGetProperty(pCardData, 
		CP_CARD_SERIAL_NO, 
		pbData, 
		cbData,
		pdwDataLen,
		0);
	if (dwReturn != SCARD_S_SUCCESS)  
   {
		LogTrace(LOGTYPE_ERROR, WHERE, "Error CardGetProperty for [CP_CARD_SERIAL_NO]: 0x08X", dwReturn);
		CLEANUP(dwReturn);
   }

	LogTrace(LOGTYPE_INFO, WHERE, "Property: [CP_CARD_GUID] -> [%d]", pCardData->pvVendorSpecific);

cleanup:

   return(dwReturn);
}
#undef WHERE

/****************************************************************************************************/

#define WHERE "CardSetGuid"
DWORD CardSetGuid(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, DWORD dwFlags)
{
   DWORD    dwReturn       = 0;

   LogTrace(LOGTYPE_INFO, WHERE, "SET Property: [CP_CARD_GUID]");

   CLEANUP(SCARD_W_SECURITY_VIOLATION);

cleanup:
   return(dwReturn);
}
#undef WHERE

/****************************************************************************************************/

#define WHERE "CardGetSerialNo"
DWORD CardGetSerialNo(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, PDWORD pdwDataLen, DWORD dwFlags)
{
   DWORD    dwReturn    = 0;
	VENDOR_SPECIFIC * vs;
   LogTrace(LOGTYPE_INFO, WHERE, "GET Property: [CP_CARD_SERIAL_NO]");

	if ( dwFlags != 0 )
	{
		LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [dwFlags]");
   CLEANUP(SCARD_E_INVALID_PARAMETER);
	}
	if (cbData < 16) 
	{
		CLEANUP(ERROR_INSUFFICIENT_BUFFER);
	}
	// For further reference
	// we keep the serial number in pCardData->pvVendorSpecific
	vs = (VENDOR_SPECIFIC*)pCardData->pvVendorSpecific;
	if (vs->bSerialNumberSet == 0) 
	{
		// serial number not set
		dwReturn = PteidGetCardSN(pCardData, vs->szSerialNumber, 
			sizeof(vs->szSerialNumber), pdwDataLen);
		if ( dwReturn != SCARD_S_SUCCESS )
		{
			LogTrace(LOGTYPE_ERROR, WHERE, "PteidGetCardSN returned [%d]", dwReturn);
			CLEANUP(SCARD_E_UNEXPECTED);
		}
		vs->bSerialNumberSet = 1;
	}
	else 
	{
		*pdwDataLen = sizeof(vs->szSerialNumber);
	}

	LogTrace(LOGTYPE_INFO, WHERE, "Property: [CP_CARD_SERIAL_NO] -> [%d]", pCardData->pvVendorSpecific);
	memcpy (pbData, vs->szSerialNumber, sizeof(vs->szSerialNumber));
cleanup:
   return(dwReturn);
}
#undef WHERE

/****************************************************************************************************/

#define WHERE "CardSetSerialNo"
DWORD CardSetSerialNo(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, DWORD dwFlags)
{
   DWORD    dwReturn       = 0;

   LogTrace(LOGTYPE_INFO, WHERE, "SET Property: [CP_CARD_SERIAL_NO]");

   CLEANUP(SCARD_W_SECURITY_VIOLATION);

cleanup:
   return(dwReturn);
}
#undef WHERE

/****************************************************************************************************/

#define WHERE "CardGetPinInfo"
DWORD CardGetPinInfo(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, PDWORD pdwDataLen, DWORD dwFlags)
{
   DWORD             dwReturn    = 0;
	PIN_INFO			 pinInfo;
	FEATURES			 CCIDfeatures;

   LogTrace(LOGTYPE_INFO, WHERE, "GET Property: [CP_CARD_PIN_INFO][%d]", dwFlags);

   /* dwFlags contains the the identifier of the PIN to return */
   if ( (dwFlags < 0        ) ||
        (dwFlags > MAX_PINS ) )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [dwFlags = %d]", dwFlags);
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }

   if ( ( dwFlags != ROLE_DIGSIG ) &&
        ( dwFlags != ROLE_NONREP ) )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [dwFlags][%d]", dwFlags);
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }

   if ( cbData < sizeof(PIN_INFO) )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Insufficient buffer [%d][%d]", cbData, sizeof(PIN_INFO));
      CLEANUP(ERROR_INSUFFICIENT_BUFFER);
   }

	/**********************************************
	*  Pin info
	**********************************************/ 
	pinInfo.dwVersion                             = PIN_INFO_CURRENT_VERSION;
	pinInfo.dwChangePermission                    = CREATE_PIN_SET(dwFlags);
	pinInfo.dwUnblockPermission                   = 0;
	pinInfo.dwFlags                               = 0;
	pinInfo.PinPurpose                            = AuthenticationPin;
	if (dwFlags == ROLE_DIGSIG)
		pinInfo.PinPurpose                        = AuthenticationPin;
	if (dwFlags == ROLE_NONREP) 
		pinInfo.PinPurpose                        = NonRepudiationPin;
	/**********************************************
	*  Pin chache policy
	**********************************************/   
	pinInfo.PinCachePolicy.dwVersion              = PIN_CACHE_POLICY_CURRENT_VERSION;
	pinInfo.PinCachePolicy.dwPinCachePolicyInfo   = 0;
	pinInfo.PinCachePolicy.PinCachePolicyType     = PinCacheNone;
	if (dwFlags == ROLE_DIGSIG) 
		pinInfo.PinCachePolicy.PinCachePolicyType = PinCacheNormal;
	if (dwFlags == ROLE_NONREP) 
		pinInfo.PinCachePolicy.PinCachePolicyType = PinCacheNone;

	/*********************************************
	*  Check CCID features for external pin pad
	**********************************************/
	CCIDgetFeatures(&(CCIDfeatures), pCardData->hScard);
	if ((CCIDfeatures.VERIFY_PIN_START != 0) || (CCIDfeatures.VERIFY_PIN_DIRECT != 0))
		pinInfo.PinType                           = ExternalPinType;
	else
		pinInfo.PinType                           = AlphaNumericPinType;

	memcpy (pbData, &pinInfo, sizeof(PIN_INFO));
   *pdwDataLen = sizeof(PIN_INFO);

cleanup:
   return(dwReturn);
}
#undef WHERE

/****************************************************************************************************/

#define WHERE "CardSetPinInfo"
DWORD CardSetPinInfo(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, DWORD dwFlags)
{
   DWORD    dwReturn       = 0;

   LogTrace(LOGTYPE_INFO, WHERE, "SET Property: [CP_CARD_PIN_INFO]");

   CLEANUP(SCARD_W_SECURITY_VIOLATION);

cleanup:
   return(dwReturn);
}
#undef WHERE

/****************************************************************************************************/

#define WHERE "CardGetListPins"
DWORD CardGetListPins(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, PDWORD pdwDataLen, DWORD dwFlags)
{
   DWORD    dwReturn    = 0;
   DWORD    dwPinSet    = 0;

   LogTrace(LOGTYPE_INFO, WHERE, "GET Property: [CP_CARD_LIST_PINS]");

   if ( dwFlags != 0 )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [dwFlags]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }

   if ( cbData < sizeof(dwPinSet) )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Insufficient buffer");
      CLEANUP(ERROR_INSUFFICIENT_BUFFER);
   }

   /* We only have a normal user */
   dwPinSet = CREATE_PIN_SET(ROLE_DIGSIG);
   SET_PIN(dwPinSet, ROLE_NONREP);

   LogTrace(LOGTYPE_INFO, WHERE, "PINSET:[%d]", dwPinSet);
   memcpy (pbData, &dwPinSet, sizeof(dwPinSet));
   *pdwDataLen = sizeof(dwPinSet);

cleanup:
   return(dwReturn);
}
#undef WHERE

/****************************************************************************************************/

#define WHERE "CardSetListPins"
DWORD CardSetListPins(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, DWORD dwFlags)
{
   DWORD    dwReturn       = 0;

   LogTrace(LOGTYPE_INFO, WHERE, "SET Property: [CP_CARD_LIST_PINS]");

   CLEANUP(SCARD_E_INVALID_PARAMETER);

cleanup:
   return(dwReturn);
}
#undef WHERE

/****************************************************************************************************/

#define WHERE "CardGetAuthenticatedState"
DWORD CardGetAuthenticatedState(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, PDWORD pdwDataLen, DWORD dwFlags)
{
   DWORD    dwReturn       = 0;

   LogTrace(LOGTYPE_INFO, WHERE, "GET Property: [CP_CARD_AUTHENTICATED_STATE]");

   /*
    * certification changed on 23/07/2008
	* Must return SCARD_E_INVALID_PARAMETER instead of SCARD_E_UNSUPPORTED_FEATURE
	*/
   CLEANUP(SCARD_E_INVALID_PARAMETER);

cleanup:
   return(dwReturn);
}
#undef WHERE

/****************************************************************************************************/

#define WHERE "CardSetAuthenticatedState"
DWORD CardSetAuthenticatedState(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, DWORD dwFlags)
{
   DWORD    dwReturn       = 0;

   LogTrace(LOGTYPE_INFO, WHERE, "SET Property: [CP_CARD_AUTHENTICATED_STATE]");

   CLEANUP(SCARD_E_INVALID_PARAMETER);

cleanup:
   return(dwReturn);
}
#undef WHERE

/****************************************************************************************************/

#define WHERE "CardGetPinStrengthVerify"
DWORD CardGetPinStrengthVerify(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, PDWORD pdwDataLen, DWORD dwFlags)
{
   DWORD    dwReturn      = 0;
   DWORD    dwPinStrength = CARD_PIN_STRENGTH_PLAINTEXT;

   LogTrace(LOGTYPE_INFO, WHERE, "GET Property: [CP_CARD_PIN_STRENGTH_VERIFY]");

   if ( ( dwFlags < 0         ) ||
        ( dwFlags >= MAX_PINS ) )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [dwFlags][%d]", dwFlags);
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }

   if ( ( dwFlags != ROLE_DIGSIG ) &&
        ( dwFlags != ROLE_NONREP ) )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [dwFlags][%d]", dwFlags);
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }

	//if ( pCurrentCard->PinInfo[dwFlags].dwVersion != PIN_INFO_CURRENT_VERSION )
	//{
	//   LogTrace(LOGTYPE_ERROR, WHERE, "PIN identifier [%d] not supported by card", dwFlags);
	//   CLEANUP(SCARD_E_INVALID_PARAMETER);
	//}

   memcpy (pbData, &dwPinStrength, sizeof(dwPinStrength));
   *pdwDataLen = sizeof(dwPinStrength);

cleanup:
   return(dwReturn);
}
#undef WHERE

/****************************************************************************************************/

#define WHERE "CardSetPinStrengthVerify"
DWORD CardSetPinStrengthVerify(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, DWORD dwFlags)
{
   DWORD    dwReturn       = 0;

   LogTrace(LOGTYPE_INFO, WHERE, "SET Property: [CP_CARD_PIN_STRENGTH_VERIFY]");

   CLEANUP(SCARD_W_SECURITY_VIOLATION);

cleanup:
   return(dwReturn);
}
#undef WHERE

/****************************************************************************************************/

#define WHERE "CardGetPinStrengthChange"
DWORD CardGetPinStrengthChange(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, PDWORD pdwDataLen, DWORD dwFlags)
{
   DWORD    dwReturn       = 0;

   LogTrace(LOGTYPE_INFO, WHERE, "GET Property: [CP_CARD_PIN_STRENGTH_CHANGE]");

   CLEANUP(SCARD_E_UNSUPPORTED_FEATURE);

cleanup:
   return(dwReturn);
}
#undef WHERE

/****************************************************************************************************/

#define WHERE "CardSetPinStrengthChange"
DWORD CardSetPinStrengthChange(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, DWORD dwFlags)
{
   DWORD dwReturn = 0;

   LogTrace(LOGTYPE_INFO, WHERE, "SET Property: [CP_CARD_PIN_STRENGTH_CHANGE]");

   CLEANUP(SCARD_W_SECURITY_VIOLATION);

cleanup:
   return(dwReturn);
}
#undef WHERE

/****************************************************************************************************/

#define WHERE "CardGetPinStrengthUnblock"
DWORD CardGetPinStrengthUnblock(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, PDWORD pdwDataLen, DWORD dwFlags)
{
   DWORD    dwReturn       = 0;

   LogTrace(LOGTYPE_INFO, WHERE, "GET Property: [CP_CARD_PIN_STRENGTH_UNBLOCK]");

   CLEANUP(SCARD_E_UNSUPPORTED_FEATURE);

cleanup:
   return(dwReturn);
}
#undef WHERE

/****************************************************************************************************/

#define WHERE "CardSetPinStrengthUnblock"
DWORD CardSetPinStrengthUnblock(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, DWORD dwFlags)
{
   DWORD dwReturn = 0;

   LogTrace(LOGTYPE_INFO, WHERE, "SET Property: [CP_CARD_PIN_STRENGTH_UNBLOCK]");

   CLEANUP(SCARD_W_SECURITY_VIOLATION);

cleanup:
   return(dwReturn);
}
#undef WHERE

/****************************************************************************************************/

#define WHERE "CardGetParentWindow"
DWORD CardGetParentWindow(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, PDWORD pdwDataLen, DWORD dwFlags)
{
   DWORD    dwReturn       = 0;

   LogTrace(LOGTYPE_INFO, WHERE, "GET Property: [CP_PARENT_WINDOW]");

   CLEANUP(SCARD_E_UNSUPPORTED_FEATURE);

cleanup:
   return(dwReturn);
}
#undef WHERE

/****************************************************************************************************/

#define WHERE "CardSetParentWindow"
DWORD CardSetParentWindow(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, DWORD dwFlags)
{
   DWORD dwReturn   = 0;
   HWND  hWinHandle = 0;

   LogTrace(LOGTYPE_INFO, WHERE, "SET Property: [CP_PARENT_WINDOW][0x%X]", dwFlags);

   if ( dwFlags != 0 )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [dwFlags]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }
   if ( pbData == NULL )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pbData]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }
   if ( cbData != sizeof(HWND) )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [cbData]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }

   memcpy(&hWinHandle, pbData, sizeof(HWND));
   if ( ( hWinHandle           != 0 ) &&
        ( IsWindow(hWinHandle) == 0 ) )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid Window Handle [0x%X][0x%X]", hWinHandle, GetLastError());
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }
   LogTrace(LOGTYPE_INFO, WHERE, "Valid Window Handle [0x%X]", hWinHandle);

   CLEANUP(SCARD_S_SUCCESS);

cleanup:
   return(dwReturn);
}
#undef WHERE

/****************************************************************************************************/

#define WHERE "CardGetPinContextString"
DWORD CardGetPinContextString(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, PDWORD pdwDataLen, DWORD dwFlags)
{
   DWORD    dwReturn       = 0;

   LogTrace(LOGTYPE_INFO, WHERE, "GET Property: [CP_PIN_CONTEXT_STRING]");

   CLEANUP(SCARD_E_UNSUPPORTED_FEATURE);

cleanup:
   return(dwReturn);
}
#undef WHERE

/****************************************************************************************************/

#define WHERE "CardSetPinContextString"
DWORD CardSetPinContextString(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, DWORD dwFlags)
{
   DWORD dwReturn = 0;

   LogTrace(LOGTYPE_INFO, WHERE, "SET Property: [CP_PIN_CONTEXT_STRING][0x%X]", dwFlags);

   if ( dwFlags != 0 )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [dwFlags]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }

   CLEANUP(SCARD_S_SUCCESS);

cleanup:
   return(dwReturn);
}
#undef WHERE


/****************************************************************************************************/

/****************************************************************************************************/

#define WHERE "CardSetPropertyUnsupported"
DWORD CardSetPropertyUnsupported(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, DWORD dwFlags)
{
   DWORD dwReturn = 0;

   LogTrace(LOGTYPE_INFO, WHERE, "SET Property: []");

   CLEANUP(SCARD_W_SECURITY_VIOLATION);

cleanup:
   return(dwReturn);
}
#undef WHERE

/****************************************************************************************************/
/****************************************************************************************************/

#define WHERE "CardGetFirstTwoGivenNames"
DWORD CardGetFirstTwoGivenNames(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, PDWORD pdwDataLen, DWORD dwFlags)
{
   DWORD    dwReturn       = 0;
	PBYTE*   ppbFile = NULL;
	DWORD    cbFile         = 0;

   LogTrace(LOGTYPE_INFO, WHERE, "GET Property: [CP_ID_FIRST_TWO_GIVEN_NAMES]");

	dwReturn = pCardData->pfnCardReadFile(pCardData, "id", "id", 0, ppbFile, &cbFile);

	if (dwReturn != SCARD_S_SUCCESS) {
		LogTrace(LOGTYPE_ERROR, WHERE, "CardRead File Failed [0x%x]",dwReturn);
		CLEANUP(dwReturn);
	}

	dwReturn = TLVGetField(*ppbFile, cbFile, pbData, cbData, pdwDataLen, ID_FIRST_TWO_GIVEN_NAMES);

   if (dwReturn != SCARD_S_SUCCESS) {
		LogTrace(LOGTYPE_ERROR, WHERE, "TLVGetField Failed [0x%x]",dwReturn);
		CLEANUP(dwReturn);
	}
   CLEANUP(SCARD_S_SUCCESS);

cleanup:
   return(dwReturn);
}
#undef WHERE
/****************************************************************************************************/

#define WHERE "CardGetCardNumber"
DWORD CardGetCardNumber(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, PDWORD pdwDataLen, DWORD dwFlags)
{
   DWORD    dwReturn       = 0;
   CLEANUP(SCARD_S_SUCCESS);

cleanup:
   return(dwReturn);
}
#undef WHERE
/****************************************************************************************************/

#define WHERE "CardGetChipNumber"
DWORD CardGetChipNumber(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, PDWORD pdwDataLen, DWORD dwFlags)
{
   DWORD    dwReturn       = 0;
   CLEANUP(SCARD_S_SUCCESS);

cleanup:
   return(dwReturn);
}
#undef WHERE
/****************************************************************************************************/

#define WHERE "CardGetValidityBegin"
DWORD CardGetValidityBegin(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, PDWORD pdwDataLen, DWORD dwFlags)
{
   DWORD    dwReturn       = 0;
   CLEANUP(SCARD_S_SUCCESS);

cleanup:
   return(dwReturn);
}
#undef WHERE
/****************************************************************************************************/

#define WHERE "CardGetValidityEnd"
DWORD CardGetValidityEnd(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, PDWORD pdwDataLen, DWORD dwFlags)
{
   DWORD    dwReturn       = 0;
   CLEANUP(SCARD_S_SUCCESS);

cleanup:
   return(dwReturn);
}
#undef WHERE
/****************************************************************************************************/

#define WHERE "CardGetDeliveryMunicipality"
DWORD CardGetDeliveryMunicipality(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, PDWORD pdwDataLen, DWORD dwFlags)
{
   DWORD    dwReturn       = 0;
   CLEANUP(SCARD_S_SUCCESS);

cleanup:
   return(dwReturn);
}
#undef WHERE
/****************************************************************************************************/

#define WHERE "CardGetNationalNumber"
DWORD CardGetNationalNumber(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, PDWORD pdwDataLen, DWORD dwFlags)
{
   DWORD    dwReturn       = 0;
   CLEANUP(SCARD_S_SUCCESS);

cleanup:
   return(dwReturn);
}
#undef WHERE
/****************************************************************************************************/

#define WHERE "CardGetName"
DWORD CardGetName(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, PDWORD pdwDataLen, DWORD dwFlags)
{
   DWORD    dwReturn       = 0;
   CLEANUP(SCARD_S_SUCCESS);

cleanup:
   return(dwReturn);
}
#undef WHERE
/****************************************************************************************************/

#define WHERE "CardGetFirstLetterThirdGivenName"
DWORD CardGetFirstLetterThirdGivenName(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, PDWORD pdwDataLen, DWORD dwFlags)
{
   DWORD    dwReturn       = 0;
   CLEANUP(SCARD_S_SUCCESS);

cleanup:
   return(dwReturn);
}
#undef WHERE
/****************************************************************************************************/

#define WHERE "CardGetNationality"
DWORD CardGetNationality(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, PDWORD pdwDataLen, DWORD dwFlags)
{
   DWORD    dwReturn       = 0;
   CLEANUP(SCARD_S_SUCCESS);

cleanup:
   return(dwReturn);
}
#undef WHERE
/****************************************************************************************************/

#define WHERE "CardGetBirthLocation"
DWORD CardGetBirthLocation(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, PDWORD pdwDataLen, DWORD dwFlags)
{
   DWORD    dwReturn       = 0;
   CLEANUP(SCARD_S_SUCCESS);

cleanup:
   return(dwReturn);
}
#undef WHERE
/****************************************************************************************************/

#define WHERE "CardGetBirthDate"
DWORD CardGetBirthDate(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, PDWORD pdwDataLen, DWORD dwFlags)
{
   DWORD    dwReturn       = 0;
   CLEANUP(SCARD_S_SUCCESS);

cleanup:
   return(dwReturn);
}
#undef WHERE
/****************************************************************************************************/

#define WHERE "CardGetGender"
DWORD CardGetGender(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, PDWORD pdwDataLen, DWORD dwFlags)
{
   DWORD    dwReturn       = 0;
   CLEANUP(SCARD_S_SUCCESS);

cleanup:
   return(dwReturn);
}
#undef WHERE
/****************************************************************************************************/

#define WHERE "CardGetNobility"
DWORD CardGetNobility(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, PDWORD pdwDataLen, DWORD dwFlags)
{
   DWORD    dwReturn       = 0;
   CLEANUP(SCARD_S_SUCCESS);

cleanup:
   return(dwReturn);
}
#undef WHERE
/****************************************************************************************************/

#define WHERE "CardGetDocumentType"
DWORD CardGetDocumentType(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, PDWORD pdwDataLen, DWORD dwFlags)
{
   DWORD    dwReturn       = 0;
   CLEANUP(SCARD_S_SUCCESS);

cleanup:
   return(dwReturn);
}
#undef WHERE
/****************************************************************************************************/

#define WHERE "CardGetSpecialStatus"
DWORD CardGetSpecialStatus(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, PDWORD pdwDataLen, DWORD dwFlags)
{
   DWORD    dwReturn       = 0;
   CLEANUP(SCARD_S_SUCCESS);

cleanup:
   return(dwReturn);
}
#undef WHERE
/****************************************************************************************************/

#define WHERE "CardGetPhotoHash"
DWORD CardGetPhotoHash(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, PDWORD pdwDataLen, DWORD dwFlags)
{
   DWORD    dwReturn       = 0;
   CLEANUP(SCARD_S_SUCCESS);

cleanup:
   return(dwReturn);
}
#undef WHERE
/****************************************************************************************************/

#define WHERE "CardGetAddressStreetAndNumber"
DWORD CardGetAddressStreetAndNumber(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, PDWORD pdwDataLen, DWORD dwFlags)
{
   DWORD    dwReturn       = 0;
   CLEANUP(SCARD_S_SUCCESS);

cleanup:
   return(dwReturn);
}
#undef WHERE
/****************************************************************************************************/

#define WHERE "CardGetAddressZip"
DWORD CardGetAddressZip(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, PDWORD pdwDataLen, DWORD dwFlags)
{
   DWORD    dwReturn       = 0;
   CLEANUP(SCARD_S_SUCCESS);

cleanup:
   return(dwReturn);
}
#undef WHERE
/****************************************************************************************************/

#define WHERE "CardGetAddressMunicipality"
DWORD CardGetAddressMunicipality(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, PDWORD pdwDataLen, DWORD dwFlags)
{
   DWORD    dwReturn       = 0;
   CLEANUP(SCARD_S_SUCCESS);

cleanup:
   return(dwReturn);
}
#undef WHERE
/****************************************************************************************************/

typedef struct CardPropertyFnct
{
   LPWSTR   wszProperty;
	DWORD    (*GetFnct)(PCARD_DATA, PBYTE, DWORD, PDWORD, DWORD);
	DWORD    (*SetFnct)(PCARD_DATA, PBYTE, DWORD, DWORD);
} CardPropertyFnct;

CardPropertyFnct PropFnct [] = 
{
   {CP_CARD_FREE_SPACE                 , CardGetFreeSpace                , CardSetFreeSpace                },
   {CP_CARD_CAPABILITIES               , CardGetCapabilities             , CardSetCapabilities             },
   {CP_CARD_KEYSIZES                   , CardGetKeysizes                 , CardSetKeysizes                 },
   {CP_CARD_READ_ONLY                  , CardGetReadOnly                 , CardSetReadOnly                 },
   {CP_CARD_CACHE_MODE                 , CardGetCacheMode                , CardSetCacheMode                },
   {CP_SUPPORTS_WIN_X509_ENROLLMENT    , CardGetSupportsWinX509Enrollment, CardSetSupportsWinX509Enrollment},
   {CP_CARD_GUID                       , CardGetGuid                     , CardSetGuid                     },
   {CP_CARD_SERIAL_NO                  , CardGetSerialNo                 , CardSetSerialNo                 },
   {CP_CARD_PIN_INFO                   , CardGetPinInfo                  , CardSetPinInfo                  },
   {CP_CARD_LIST_PINS                  , CardGetListPins                 , CardSetListPins                 },
   {CP_CARD_AUTHENTICATED_STATE        , CardGetAuthenticatedState       , CardSetAuthenticatedState       },
   {CP_CARD_PIN_STRENGTH_VERIFY        , CardGetPinStrengthVerify        , CardSetPinStrengthVerify        },
   {CP_CARD_PIN_STRENGTH_CHANGE        , CardGetPinStrengthChange        , CardSetPinStrengthChange        },
   {CP_CARD_PIN_STRENGTH_UNBLOCK       , CardGetPinStrengthUnblock       , CardSetPinStrengthUnblock       },
   {CP_PARENT_WINDOW                   , CardGetParentWindow             , CardSetParentWindow             },
   {CP_PIN_CONTEXT_STRING              , CardGetPinContextString         , CardSetPinContextString         },
	{CP_ID_CARD_NUMBER                  , CardGetCardNumber               , CardSetPropertyUnsupported      },
	{CP_ID_CHIP_NUMBER                  , CardGetChipNumber               , CardSetPropertyUnsupported      },
	{CP_ID_CARD_VALIDITY_BEGIN          , CardGetValidityBegin            , CardSetPropertyUnsupported      },
	{CP_ID_CARD_VALIDITY_END            , CardGetValidityEnd              , CardSetPropertyUnsupported      },
	{CP_ID_CARD_DELIVIRY_MUNICIPALITY   , CardGetDeliveryMunicipality     , CardSetPropertyUnsupported      },
	{CP_ID_NATIONAL_NUMBER              , CardGetNationalNumber           , CardSetPropertyUnsupported      },
	{CP_ID_NAME                         , CardGetName                     , CardSetPropertyUnsupported      },
	{CP_ID_FIRST_TWO_GIVEN_NAMES        , CardGetFirstTwoGivenNames       , CardSetPropertyUnsupported      },
	{CP_ID_FIRST_LETTER_THIRD_GIVEN_NAME, CardGetFirstLetterThirdGivenName, CardSetPropertyUnsupported      },
	{CP_ID_NATIONALITY                  , CardGetNationality              , CardSetPropertyUnsupported      },
	{CP_ID_BIRTH_LOCATION               , CardGetBirthLocation            , CardSetPropertyUnsupported      },
	{CP_ID_BIRTH_DATE                   , CardGetBirthDate                , CardSetPropertyUnsupported      },
	{CP_ID_GENDER                       , CardGetGender                   , CardSetPropertyUnsupported      },
	{CP_ID_NOBILITY                     , CardGetNobility                 , CardSetPropertyUnsupported      },
	{CP_ID_DOCUMENT_TYPE                , CardGetDocumentType             , CardSetPropertyUnsupported      },
	{CP_ID_SPECIAL_STATUS               , CardGetSpecialStatus            , CardSetPropertyUnsupported      },
	{CP_ID_PHOTO_HASH                   , CardGetPhotoHash                , CardSetPropertyUnsupported      },
	{CP_ID_ADDRESS_STREET_AND_NUMBER    , CardGetAddressStreetAndNumber   , CardSetPropertyUnsupported      },
	{CP_ID_ADDRESS_ZIP                  , CardGetAddressZip               , CardSetPropertyUnsupported      },
	{CP_ID_ADDRESS_MUNICIPALITY         , CardGetAddressMunicipality      , CardSetPropertyUnsupported      },

   {NULL, NULL, NULL}
};

/****************************************************************************************************/

#define WHERE "CardGetProperty()"
DWORD WINAPI   CardGetProperty
(
   __in                                         PCARD_DATA  pCardData,
   __in                                         LPCWSTR     wszProperty,
   __out_bcount_part_opt(cbData, *pdwDataLen)   PBYTE       pbData,
   __in                                         DWORD       cbData,
   __out                                        PDWORD      pdwDataLen,
   __in                                         DWORD       dwFlags
)
{
   DWORD             dwReturn       = 0;
   int               i              = 0;


   LogTrace(LOGTYPE_INFO, WHERE, "Enter API...");

   /********************/
   /* Check Parameters */
   /********************/
   if ( pCardData == NULL )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pCardData]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }
   if ( wszProperty == NULL )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [wszProperty]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }
   if ( pbData == NULL )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pbData]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }
   if ( pdwDataLen == NULL )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pdwDataLen]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }

   while ( ( PropFnct[i].wszProperty != NULL ) &&
           ( PropFnct[i].GetFnct     != NULL ) )
   {
      if ( wcscmp(wszProperty, PropFnct[i].wszProperty) == 0 ) 
      {
			dwReturn = (*PropFnct[i].GetFnct)(pCardData, pbData, cbData, pdwDataLen, dwFlags);
         break;
      }
      i++;
   }
   if ( ( PropFnct[i].wszProperty == NULL ) ||
        ( PropFnct[i].GetFnct     == NULL ) )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [wszProperty not supported]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }

#ifdef _DEBUG
   LogDump (cbData, (char *)pbData);
#endif

cleanup:

   LogTrace(LOGTYPE_INFO, WHERE, "Exit API...");

   return(dwReturn);
}
#undef WHERE

/****************************************************************************************************/

//
// Function: CardSetProperty
//

#define WHERE "CardSetProperty()"
DWORD WINAPI   CardSetProperty
               (
                  __in                    PCARD_DATA     pCardData,
                  __in                    LPCWSTR        wszProperty,
                  __in_bcount(cbDataLen)  PBYTE          pbData,
                  __in                    DWORD          cbDataLen,
                  __in                    DWORD          dwFlags
               )
{
   DWORD             dwReturn       = 0;
   int               i              = 0;

   LogTrace(LOGTYPE_INFO, WHERE, "Enter API...");

   /********************/
   /* Check Parameters */
   /********************/
   if ( pCardData == NULL )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pCardData]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }
   if ( wszProperty == NULL )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [wszProperty]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }

   while ( ( PropFnct[i].wszProperty != NULL ) &&
           ( PropFnct[i].SetFnct     != NULL ) )
   {
      if ( wcscmp(wszProperty, PropFnct[i].wszProperty) == 0 ) 
      {
			dwReturn = (*PropFnct[i].SetFnct)(pCardData, pbData, cbDataLen, dwFlags);
         break;
      }
      i++;
   }
   if ( ( PropFnct[i].wszProperty == NULL ) ||
        ( PropFnct[i].SetFnct     == NULL ) )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [wszProperty not supported]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }

#ifdef _DEBUG
   LogDump (cbDataLen, (char *)pbData);
#endif

cleanup:

   LogTrace(LOGTYPE_INFO, WHERE, "Exit API...");

   return(dwReturn);
}
#undef WHERE

/****************************************************************************************************/
