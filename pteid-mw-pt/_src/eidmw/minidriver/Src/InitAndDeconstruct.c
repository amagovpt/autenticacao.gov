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
/****************************************************************************************************/

#include "globmdrv.h"

#include "log.h"
#include "util.h"

/****************************************************************************************************/

#define MINIMUM_VERSION_SUPPORTED      CARD_DATA_VERSION_FOUR
#define CURRENT_VERSION_SUPPORTED      CARD_DATA_VERSION_SIX



/* Supported Cards(ATRs)
*/
CARD_ATR  CardAtr[] = 
            {
				//IAS cards
			   {{0x3B,0x65,0x00,0x00,0xd0,0x00,0x54,0x01,0x31}, 9},
			   {{0x3B,0x65,0x00,0x00,0xd0,0x00,0x54,0x01,0x32}, 9},
               {{0x3B,0x95,0x95,0x40,0xFF,0xD0,0x00,0x54,0x01,0x31}, 10},
			   {{0x3B,0x95,0x95,0x40,0xFF,0xD0,0x00,0x54,0x01,0x32}, 10},
			    //GEMSAFE cards
			   {{0x3B,0x7D,0x95,0x00,0x00,0x80,0x31,0x80,0x65,0xb0,0x83,0x11,0x00,0xc8,0x83,0x00,0x90,0x00}, 18},
			   {{0x3B,0x7D,0x95,0x00,0x00,0x80,0x31,0x80,0x65,0xb0,0x83,0x11,0x00,0xa9,0x83,0x00,0x90,0x00}, 18},
			   {{0x3B,0x7D,0x95,0x00,0x00,0x80,0x31,0x80,0x65,0xb0,0x83,0x11,0xC0,0xa9,0x83,0x00,0x90,0x00}, 18},
			   {{0x3B,0x7D,0x95,0x00,0x00,0x80,0x31,0x80,0x65,0xb0,0x83,0x11,0x00,0xc8,0x83,0x00}, 16},
			   {{0x3B,0x7D,0x95,0x00,0x00,0x80,0x31,0x80,0x65,0xb0,0x83,0x11,0x00,0xa9,0x83,0x00}, 16},
			   {{0x3B,0xFF,0x96,0x00,0x00,0x81,0x31,0x80,0x43,0x80,0x31,0x80,0x65,0xb0,0x85,0x03,0x00,0xEF,0x12,0x0F,0xFF,0x82,0x90,0x00,0x67}, 25},
			   {{0x3B,0xFF,0x96,0x00,0x00,0x81,0x31,0xFE,0x43,0x80,0x31,0x80,0x65,0xB0,0x85,0x04,0x01,0x20,0x12,0x0F,0xFF,0x82,0x90,0x00,0xD0}, 25}
            };

#define SUPPORTED_CARDS sizeof(CardAtr)

/****************************************************************************************************/


//
// Function: CardAcquireContext
//
// Purpose: Initialize the CARD_DATA structure which will be used by
//          the CSP to interact with a specific card.
//

#define WHERE "CardAcquireContext()"
DWORD WINAPI   CardAcquireContext
               (
                  IN    PCARD_DATA  pCardData,
                  __in	DWORD       dwFlags
               )
{
   DWORD                   dwReturn    = 0;
   
   int                     iAtr        = 0;
   int                     iCardCnt    = 0;
   int                     iLgCnt      = 0;
   char readerName[256];
   DWORD readername_len = sizeof(readerName);
   DWORD protocol = 0;
   BYTE bAtr[32];
   DWORD cByte = 32;

   LogTrace(LOGTYPE_INFO, WHERE, "Enter API...");

   /********************/
   /* Check Parameters */
   /********************/
   if ( pCardData == NULL )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pCardData]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }

   if ( dwFlags != 0 )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [dwFlags]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }

   /* The lowest supported version */
   if ( pCardData->dwVersion < MINIMUM_VERSION_SUPPORTED )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pCardData->dwVersion][%d]",pCardData->dwVersion);
      CLEANUP(ERROR_REVISION_MISMATCH);
   }
   /* Set the version to what we support, but don't exceed the requested version */
   pCardData->dwVersion = min(pCardData->dwVersion, CURRENT_VERSION_SUPPORTED);

   /* Check ATR */
   if ( pCardData->pbAtr == NULL )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pCardData->pbAtr]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }
	
   LogTrace(LOGTYPE_INFO, WHERE, "ATR input value: ");
   LogDump(pCardData->cbAtr, pCardData->pbAtr);

   for ( iAtr = 0 ; iAtr < SUPPORTED_CARDS ; iAtr++ )
   {
      if ( pCardData->cbAtr == CardAtr[iAtr].cbAtr )
      {
         if ( memcmp(pCardData->pbAtr, CardAtr[iAtr].pbAtr, pCardData->cbAtr) == 0 )
         {
			if (iAtr > 3)
			{
				Is_Gemsafe = 1; //GEMSAFE
				LogTrace(LOGTYPE_INFO, WHERE, "Gemsafe Card Detected");
			}
			else
			{
				Is_Gemsafe = 0; //IAS
				LogTrace(LOGTYPE_INFO, WHERE, "IAS Card Detected");
			}
            iCardCnt++;
            break;
         }

         iLgCnt++;
      }
   }
   if ( iCardCnt == 0 )
   {
      if ( iLgCnt == 0 )
      {
         LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pCardData->cbAtr]");
         CLEANUP(SCARD_E_INVALID_PARAMETER);
      }
      else
      {
         LogTrace(LOGTYPE_ERROR, WHERE, "Unknown card");
         CLEANUP(SCARD_E_UNKNOWN_CARD);
      }
   }

   /* Card name */
   if ( pCardData->pwszCardName == NULL )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pCardData->pwszCardName]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }
  // Memory management functions 
   if ( ( pCardData->pfnCspAlloc   == NULL ) ||
        ( pCardData->pfnCspReAlloc == NULL ) ||
        ( pCardData->pfnCspFree    == NULL ) )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pCardData->pfnCspAlloc], [pCardData->pfnCspReAlloc] or [pCardData->pfnCspFree]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }
   
   /* Card context */
   if ( pCardData->hSCardCtx == 0 )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "[pCardData->hSCardCtx = 0] not Fatal...");
      //CLEANUP(SCARD_E_INVALID_HANDLE);
   }

   /* Card handle */
   if ( pCardData->hScard == 0 )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pCardData->hScard]");
      CLEANUP(SCARD_E_INVALID_HANDLE);
   }
      
   SCardStatus(pCardData->hScard, readerName, &readername_len,
	   NULL, &protocol, (LPBYTE)&bAtr, &cByte);
	
   //Store active protocol
   switch(protocol)
   {
   case SCARD_PROTOCOL_T0:
	   g_pioSendPci = SCARD_PCI_T0;
	   break;
   case SCARD_PROTOCOL_T1:
	   g_pioSendPci = SCARD_PCI_T1;
	   break;
   }
   
   LogTrace(LOGTYPE_INFO, WHERE, "Context, handle:[0x%02X][0x%02X]", pCardData->hSCardCtx, pCardData->hScard);

   /********************************/
   /* Initialize function pointers */
   /********************************/

   /* InitAndDeconstruct.c */
   pCardData->pfnCardDeleteContext           = CardDeleteContext;

   /* CardPinOps.c */
   pCardData->pfnCardAuthenticatePin         = CardAuthenticatePin;
   pCardData->pfnCardGetChallenge            = CardGetChallenge;
   pCardData->pfnCardAuthenticateChallenge   = CardAuthenticateChallenge;
   pCardData->pfnCardDeauthenticate          = NULL;//CardDeauthenticate;
   pCardData->pfnCardAuthenticateEx          = CardAuthenticateEx;
   pCardData->pfnCardGetChallengeEx          = CardGetChallengeEx;
   pCardData->pfnCardDeauthenticateEx        = CardDeauthenticateEx; /* This can be an optional export */
   pCardData->pfnCardChangeAuthenticatorEx   = CardChangeAuthenticatorEx;
   pCardData->pfnCardUnblockPin              = CardUnblockPin;
   pCardData->pfnCardChangeAuthenticator     = CardChangeAuthenticator;

   /* PubDataOps.c */
   pCardData->pfnCardCreateDirectory         = CardCreateDirectory;
   pCardData->pfnCardDeleteDirectory         = CardDeleteDirectory;
   pCardData->pfnCardReadFile                = CardReadFile;
   pCardData->pfnCardCreateFile              = CardCreateFile;
   pCardData->pfnCardGetFileInfo             = CardGetFileInfo;
   pCardData->pfnCardWriteFile               = CardWriteFile;
   pCardData->pfnCardDeleteFile              = CardDeleteFile;
   pCardData->pfnCardEnumFiles               = CardEnumFiles;
   pCardData->pfnCardQueryFreeSpace          = CardQueryFreeSpace;

   /* CardCap.c */
   pCardData->pfnCardQueryCapabilities       = CardQueryCapabilities;

   /* CardAndContProp.c */
   pCardData->pfnCardGetContainerProperty    = CardGetContainerProperty;
   pCardData->pfnCardSetContainerProperty    = CardSetContainerProperty;
   pCardData->pfnCardGetProperty             = CardGetProperty;
   pCardData->pfnCardSetProperty             = CardSetProperty;

   /* KeyContainer.c */
   pCardData->pfnCardCreateContainer         = CardCreateContainer;
   pCardData->pfnCardDeleteContainer         = CardDeleteContainer;
   pCardData->pfnCardGetContainerInfo        = CardGetContainerInfo;

   /* CryptoOps.c */
   pCardData->pfnCardRSADecrypt              = CardRSADecrypt;

#ifdef _ECC_
   pCardData->pfnCardConstructDHAgreement    = CardConstructDHAgreement;
   pCardData->pfnCardDeriveKey               = CardDeriveKey;
   pCardData->pfnCardDestroyDHAgreement      = CardDestroyDHAgreement;
#else
   pCardData->pfnCardConstructDHAgreement    = NULL;
   pCardData->pfnCardDeriveKey               = NULL;
   pCardData->pfnCardDestroyDHAgreement      = NULL;
#endif

   pCardData->pfnCardSignData                = CardSignData;
   pCardData->pfnCardQueryKeySizes           = CardQueryKeySizes;

   /* Not defined */
   pCardData->pfnCspGetDHAgreement           = NULL;

	/* Vendor specific */
	pCardData->pvVendorSpecific               = pCardData->pfnCspAlloc(sizeof(VENDOR_SPECIFIC));
	memset(pCardData->pvVendorSpecific, 0, sizeof(VENDOR_SPECIFIC));
cleanup:

   LogTrace(LOGTYPE_INFO, WHERE, "Exit API...");

   return (dwReturn);
}
#undef WHERE

/****************************************************************************************************/

//
// Function: CardDeleteContext
//
// Purpose: Free resources consumed by the CARD_DATA structure.
//

#define WHERE "CardDeleteContext()"
DWORD WINAPI   CardDeleteContext
               (
                  __inout PCARD_DATA  pCardData
               )
{
   DWORD             dwReturn    = 0;

   LogTrace(LOGTYPE_INFO, WHERE, "Enter API...");

   /********************/
   /* Check Parameters */
   /********************/
   if ( pCardData == NULL )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pCardData]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }

   LogTrace(LOGTYPE_INFO, WHERE, "Context:[0x%08X]", pCardData->hSCardCtx);

	pCardData->pfnCspFree(pCardData->pvVendorSpecific);
	pCardData->pvVendorSpecific = NULL;
cleanup:

   LogTrace(LOGTYPE_INFO, WHERE, "Exit API...");

   return (dwReturn);
}
#undef WHERE

/****************************************************************************************************/
