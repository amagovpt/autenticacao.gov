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

//
// Function: CardQueryCapabilities
//
// Purpose: Query the card module for specific functionality
//          provided by this card.
//

#define WHERE "CardQueryCapabilities()"
DWORD WINAPI   CardQueryCapabilities
(
    __in      PCARD_DATA          pCardData,
    __in      PCARD_CAPABILITIES  pCardCapabilities
)
{
   DWORD    dwReturn  = 0;
   DWORD    dwVersion = 0;

   LogTrace(LOGTYPE_INFO, WHERE, "Enter API...");

   /********************/
   /* Check Parameters */
   /********************/
   if ( pCardData == NULL )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pCardData]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }
   if ( pCardCapabilities == NULL )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pCardCapabilities]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }
   dwVersion = (pCardCapabilities->dwVersion == 0) ? 1 : pCardCapabilities->dwVersion;
   if ( dwVersion != CARD_CAPABILITIES_CURRENT_VERSION )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pCardCapabilities->dwVersion]");
      CLEANUP(ERROR_REVISION_MISMATCH );
   }

   /* It's the Base CSP/CNG KSP performs these operations if the card minidriver cannot do so */
   //pCardCapabilities->dwVersion                = CARD_CAPABILITIES_CURRENT_VERSION;
   pCardCapabilities->fCertificateCompression  = TRUE;
   pCardCapabilities->fKeyGen                  = FALSE;

cleanup:
   LogTrace(LOGTYPE_INFO, WHERE, "Exit API...");
   return(dwReturn);
}
#undef WHERE

/****************************************************************************************************/
