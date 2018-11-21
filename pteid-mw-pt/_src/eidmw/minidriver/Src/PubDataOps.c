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

/****************************************************************************************************/

//
// Function: CardCreateDirectory
//
// Purpose: Register the specified application name on the card, and apply the
//          provided access condition.
//
// Return Value:
//          ERROR_FILE_EXISTS - directory already exists
//

#define WHERE "CardCreateDirectory()"
DWORD WINAPI   CardCreateDirectory
               (
                  __in    PCARD_DATA                        pCardData,
                  __in    LPSTR                             pszDirectoryName,
                  __in    CARD_DIRECTORY_ACCESS_CONDITION   AccessCondition
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
// Function: CardDeleteDirectory
//
// Purpose: Unregister the specified application from the card.
//
// Return Value:
//          SCARD_E_DIR_NOT_FOUND - directory does not exist
//          ERROR_DIR_NOT_EMPTY - the directory is not empty
//

#define WHERE "CardDeleteDirectory()"
DWORD WINAPI   CardDeleteDirectory
               (
                  __in    PCARD_DATA  pCardData,
                  __in    LPSTR       pszDirectoryName
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
// Function: CardReadFile
//
// Purpose: Read the specified file from the card.
//
//          The pbData parameter should be allocated
//          by the card module and freed by the CSP.  The card module
//          must set the cbData parameter to the size of the returned buffer.
//

#define WHERE "CardReadFile()"
DWORD WINAPI   CardReadFile
(
   __in                             PCARD_DATA  pCardData,
   __in                             LPSTR       pszDirectoryName,
   __in                             LPSTR       pszFileName,
   __in                             DWORD       dwFlags,
   __deref_out_bcount(*pcbData)     PBYTE      *ppbData,
   __out                            PDWORD      pcbData
)
{
   DWORD                dwReturn    = 0;
   int                  i           = 0;
   int                  DirFound    = 0;
   int                  FileFound   = 0;
	CONTAINER_MAP_RECORD cmr[2];
	BYTE                 pbSerialNumber[16];
	DWORD				      cbSerialNumber = sizeof(pbSerialNumber);
	DWORD				      cbDataLen;
	char					   szSerialNumber[33];
	char					   szContainerName[64];
	int					   iReturn;

   LogTrace(LOGTYPE_INFO, WHERE, "Enter API...");


   /********************/
   /* Check Parameters */
   /********************/
   if ( pCardData == NULL )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pCardData]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }
   if ( ( pszFileName    == NULL ) ||
        ( pszFileName[0] == '\0' ) )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pszFileName]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }
   if ( ppbData == NULL )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [ppbData]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }
   if ( pcbData == NULL )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pcbData]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }
   if ( ( pszDirectoryName != NULL ) &&
        ( ( strlen(pszDirectoryName) < 1 ) || 
          ( strlen(pszDirectoryName) > 8 ) ) )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pszDirectoryName]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }
   if ( dwFlags != 0 )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [dwFlags]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }

   if ( pszDirectoryName == NULL )
   {
      LogTrace(LOGTYPE_INFO, WHERE, "pszDirectoryName = [NULL]");
   }
   else
   {
      LogTrace(LOGTYPE_INFO, WHERE, "pszDirectoryName = [%s]", pszDirectoryName);
   }
   if ( pszFileName == NULL )
   {
      LogTrace(LOGTYPE_INFO, WHERE, "pszFileName = [%s]", pszFileName);
   }
   else
   {
      LogTrace(LOGTYPE_INFO, WHERE, "pszFileName = [%s]", pszFileName);
   }

	if ( pszDirectoryName == NULL)                                 /* root */
    {
		DirFound++;
		if (_stricmp("cardid", pszFileName) == 0)                   /* /cardid */
		{
			FileFound++;
			*ppbData = (PBYTE)pCardData->pfnCspAlloc(sizeof(GUID));
			if ( *ppbData == NULL )
			{
				LogTrace(LOGTYPE_ERROR, WHERE, "Error allocating memory for [*ppbData]");
				CLEANUP(SCARD_E_NO_MEMORY);
    }
			dwReturn = CardGetProperty(pCardData, 
				CP_CARD_GUID, 
				*ppbData, 
				sizeof(GUID),
				pcbData,
				0);
			if (dwReturn != SCARD_S_SUCCESS)  {
				LogTrace(LOGTYPE_ERROR, WHERE, "Error CardGetProperty for [CP_CARD_GUID]: 0x08X", dwReturn);
				CLEANUP(dwReturn);
			}

			LogTrace(LOGTYPE_INFO, WHERE, "#bytes: [%d]", *pcbData);
		}

		if ( _stricmp("cardapps", pszFileName) == 0)				      /* /cardapps */
      {
			FileFound++;
			*pcbData = 5;
			*ppbData = (PBYTE)pCardData->pfnCspAlloc(*pcbData);
			if ( *ppbData == NULL )
			{
				LogTrace(LOGTYPE_ERROR, WHERE, "Error allocating memory for [*ppbData]");
				CLEANUP(SCARD_E_NO_MEMORY);
			}
			memcpy (*ppbData, "mscp", *pcbData);
		}
		if (_stricmp("cardcf", pszFileName) == 0)					      /* /cardcf */
		{
			FileFound++;
			*pcbData = 6;
			*ppbData = (PBYTE)pCardData->pfnCspAlloc(*pcbData);
			if ( *ppbData == NULL )
			{
				LogTrace(LOGTYPE_ERROR, WHERE, "Error allocating memory for [*ppbData]");
				CLEANUP(SCARD_E_NO_MEMORY);
			}
			// zero-filled CARD_CACHE_FILE_FORMAT
			memset (*ppbData, '\0', *pcbData);
		}
	}
	else                         									         /* not on root */
	{
		if ( _stricmp("mscp", pszDirectoryName) == 0)               /* /mscp */
		{
         DirFound++;
			if (_stricmp("cmapfile", pszFileName) == 0)			      /* /mscp/cmapfile */
         {
			WORD keySize;
            FileFound++;
				
				dwReturn = CardGetProperty(pCardData, 
					CP_CARD_SERIAL_NO, 
					pbSerialNumber, 
					cbSerialNumber,
					&cbDataLen,
					0);
				if (dwReturn != SCARD_S_SUCCESS)  {
					LogTrace(LOGTYPE_ERROR, WHERE, "Error CardGetProperty for [CP_CARD_SERIAL_NO]: 0x08X", dwReturn);
					CLEANUP(dwReturn);
				}
				
				for (i = 0; i < cbDataLen; i++) {
					sprintf(szSerialNumber + 2*i*sizeof(char),
						"%02X", pbSerialNumber[i]);
				}
				szSerialNumber[16] = '\0';

				/* Cleanup CMR first */
				memset(&cmr, '\0', sizeof(cmr));

				/***************************/
				/* Authentication Key Info */
				/***************************/
				/* Container name for Authentication key */
				sprintf (szContainerName, "DS_%s", szSerialNumber);
				
				PteidReadPrKDF(pCardData, pcbData, ppbData);

				dwReturn = PteidParsePrKDF(pCardData, pcbData, *ppbData, &keySize);

				if (dwReturn != SCARD_S_SUCCESS)
				{
					LogTrace(LOGTYPE_ERROR, WHERE, "Error PteidParsePrKDF: 0x08X", dwReturn);
					CLEANUP(dwReturn);
				}
				//Save this value for the next call of CardSignData...
				g_keySize = keySize;
		
				memset(cmr[0].wszGuid, '\0', sizeof(cmr[0].wszGuid));
				iReturn = MultiByteToWideChar(CP_UTF8, 0, szContainerName, strlen(szContainerName), cmr[0].wszGuid, sizeof(cmr[0].wszGuid));

				if (iReturn == 0) 
				{
					dwReturn = GetLastError();
					LogTrace(LOGTYPE_ERROR, WHERE, "Error MultiByteToWideChar: 0x08X", dwReturn);
					CLEANUP(dwReturn);
				}
				cmr[0].bFlags                     = CONTAINER_MAP_VALID_CONTAINER|CONTAINER_MAP_DEFAULT_CONTAINER;
				cmr[0].bReserved                  = 0;
				cmr[0].wSigKeySizeBits            = keySize;
				cmr[0].wKeyExchangeKeySizeBits    = 0;

				/****************************/
				/* Non-Repudiation Key Info */
				/****************************/
				/* Container name for Non-repudiation key */
				sprintf (szContainerName, "NR_%s", szSerialNumber);
				memset(cmr[1].wszGuid, '\0', sizeof(cmr[1].wszGuid));
				iReturn = MultiByteToWideChar(CP_UTF8, 0, szContainerName, strlen(szContainerName), cmr[1].wszGuid, sizeof(cmr[1].wszGuid));

				if (iReturn == 0) 
				{
					dwReturn = GetLastError();
					LogTrace(LOGTYPE_ERROR, WHERE, "Error MultiByteToWideChar: 0x08X", dwReturn);
					CLEANUP(dwReturn);
				}
				cmr[1].bFlags                     = CONTAINER_MAP_VALID_CONTAINER;
				cmr[1].bReserved                  = 0;
				cmr[1].wSigKeySizeBits            = keySize;
				cmr[1].wKeyExchangeKeySizeBits    = 0;

				*pcbData = sizeof(cmr);
				*ppbData = (PBYTE)pCardData->pfnCspReAlloc(*ppbData, *pcbData);
				if ( *ppbData == NULL )
				{
					LogTrace(LOGTYPE_ERROR, WHERE, "Error allocating memory for [*ppbData]");
					CLEANUP(SCARD_E_NO_MEMORY);
				}

				memcpy (*ppbData, &cmr, *pcbData);
			}
			if ( _stricmp("ksc00", pszFileName) == 0)					   /* /mscp/ksc00 */
			{
				FileFound++;
				dwReturn = PteidReadCert(pCardData, CERT_AUTH, pcbData, ppbData);
				if ( dwReturn != SCARD_S_SUCCESS )
				{
					LogTrace(LOGTYPE_ERROR, WHERE, "PteidReadCert[CERT_AUTH] returned [%d]", dwReturn);
					CLEANUP(SCARD_E_UNEXPECTED);
				}
			}
			if ( _stricmp("ksc01", pszFileName) == 0)					   /* /mscp/ksc01 */
			{
				FileFound++;
				dwReturn = PteidReadCert(pCardData, CERT_NONREP, pcbData, ppbData);
				if ( dwReturn != SCARD_S_SUCCESS )
				{
					LogTrace(LOGTYPE_ERROR, WHERE, "PteidReadCert[CERT_NONREP] returned [%d]", dwReturn);
					CLEANUP(SCARD_E_UNEXPECTED);
				}
			}
			
			if ( _stricmp("msroots", pszFileName) == 0)					// /mscp/msroots
			{
				FileFound++;
				dwReturn = PteidCreateMSRoots(pCardData, pcbData, ppbData);
				if ( dwReturn != SCARD_S_SUCCESS )
				{
					LogTrace(LOGTYPE_ERROR, WHERE, "PteidCreateMSRoots returned [%d]", dwReturn);
					CLEANUP(SCARD_E_UNEXPECTED);
				}
				if ( *ppbData == NULL )
				{
					LogTrace(LOGTYPE_ERROR, WHERE, "Error allocating memory for [*ppbData]");
					CLEANUP(SCARD_E_NO_MEMORY);
				}
			}
		} 
		if ( _stricmp("id", pszDirectoryName) == 0)               /* /id */
		{
			if ( _stricmp("id", pszFileName) == 0)					    /* /id/id */
			{
				BYTE  pbFileID [] = { 0x3f, 0x00, 0x5f, 0x00, 0xef, 0x02};
				BYTE cbFileID  = (BYTE)sizeof(pbFileID);
				FileFound++;
				dwReturn = PteidSelectAndReadFile(pCardData, 0, cbFileID , pbFileID , pcbData, ppbData);
				if ( dwReturn != SCARD_S_SUCCESS )
				{
					LogTrace(LOGTYPE_ERROR, WHERE, "PteidSelectAndReadFile returned [%d]", dwReturn);
					CLEANUP(SCARD_E_UNEXPECTED);
				}
				if ( *ppbData == NULL )
				{
					LogTrace(LOGTYPE_ERROR, WHERE, "Error allocating memory for [*ppbData]");
					CLEANUP(SCARD_E_NO_MEMORY);
				}
			}
			if ( _stricmp("id_sgn", pszFileName) == 0)					    /* /id/id_sgn */
			{
				BYTE  pbFileID [] = { 0x3f, 0x00, 0xdf, 0x01, 0x40, 0x32};
				BYTE cbFileID  = (BYTE)sizeof(pbFileID);
				FileFound++;
				dwReturn = PteidSelectAndReadFile(pCardData, 0, cbFileID , pbFileID , pcbData, ppbData);
				if ( dwReturn != SCARD_S_SUCCESS )
				{
					LogTrace(LOGTYPE_ERROR, WHERE, "PteidSelectAndReadFile returned [%d]", dwReturn);
					CLEANUP(SCARD_E_UNEXPECTED);
				}
				if ( *ppbData == NULL )
				{
					LogTrace(LOGTYPE_ERROR, WHERE, "Error allocating memory for [*ppbData]");
					CLEANUP(SCARD_E_NO_MEMORY);
				}
			}
			if ( _stricmp("addr", pszFileName) == 0)					    /* /id/addr */
			{
				BYTE  pbFileID [] = { 0x3f, 0x00, 0xdf, 0x01, 0x40, 0x33};
				BYTE cbFileID  = (BYTE)sizeof(pbFileID);
				FileFound++;
				dwReturn = PteidSelectAndReadFile(pCardData, 0, cbFileID , pbFileID , pcbData, ppbData);
				if ( dwReturn != SCARD_S_SUCCESS )
				{
					LogTrace(LOGTYPE_ERROR, WHERE, "PteidSelectAndReadFile returned [%d]", dwReturn);
					CLEANUP(SCARD_E_UNEXPECTED);
				}
				if ( *ppbData == NULL )
				{
					LogTrace(LOGTYPE_ERROR, WHERE, "Error allocating memory for [*ppbData]");
					CLEANUP(SCARD_E_NO_MEMORY);
				}
			}
			if ( _stricmp("addr_sgn", pszFileName) == 0)					    /* /id/addr_sgn */
			{
				BYTE  pbFileID [] = { 0x3f, 0x00, 0xdf, 0x01, 0x40, 0x34};
				BYTE cbFileID  = (BYTE)sizeof(pbFileID);
				FileFound++;
				dwReturn = PteidSelectAndReadFile(pCardData, 0, cbFileID , pbFileID , pcbData, ppbData);
				if ( dwReturn != SCARD_S_SUCCESS )
				{
					LogTrace(LOGTYPE_ERROR, WHERE, "PteidSelectAndReadFile returned [%d]", dwReturn);
					CLEANUP(SCARD_E_UNEXPECTED);
				}
				if ( *ppbData == NULL )
				{
					LogTrace(LOGTYPE_ERROR, WHERE, "Error allocating memory for [*ppbData]");
					CLEANUP(SCARD_E_NO_MEMORY);
				}
			}
			if ( _stricmp("photo", pszFileName) == 0)					    /* /id/photo */
			{
				/*TODO: Needs to be fixed for PTEid Photo File */
				BYTE  pbFileID [] = { 0x3f, 0x00, 0xdf, 0x01, 0x40, 0x35};
				BYTE cbFileID  = (BYTE)sizeof(pbFileID);
				FileFound++;
				dwReturn = PteidSelectAndReadFile(pCardData, 0, cbFileID , pbFileID , pcbData, ppbData);
				if ( dwReturn != SCARD_S_SUCCESS )
				{
					LogTrace(LOGTYPE_ERROR, WHERE, "PteidSelectAndReadFile returned [%d]", dwReturn);
					CLEANUP(SCARD_E_UNEXPECTED);
				}
				if ( *ppbData == NULL )
				{
					LogTrace(LOGTYPE_ERROR, WHERE, "Error allocating memory for [*ppbData]");
					CLEANUP(SCARD_E_NO_MEMORY);
				}
			}
		}
	}

	if ( ! FileFound )
	{
		if ( ! DirFound )
		{
			LogTrace(LOGTYPE_ERROR, WHERE, "Dir not found");
			CLEANUP(SCARD_E_DIR_NOT_FOUND);
		}
		else
		{
			LogTrace(LOGTYPE_ERROR, WHERE, "File not found");
			CLEANUP(SCARD_E_FILE_NOT_FOUND);
		}
	}

	LogTrace(LOGTYPE_INFO, WHERE, "#bytes read: [%d]", *pcbData);

cleanup:
   LogTrace(LOGTYPE_INFO, WHERE, "Exit API...");
   return(dwReturn);
}
#undef WHERE

/****************************************************************************************************/

//
// Function: CardCreateFile
//

#define WHERE "CardCreateFile()"
DWORD WINAPI   CardCreateFile
               (
                  __in    PCARD_DATA  pCardData,
                  __in    LPSTR       pszDirectoryName,
                  __in    LPSTR       pszFileName,
                  __in    DWORD       cbInitialCreationSize,
                  __in    CARD_FILE_ACCESS_CONDITION AccessCondition
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
// Function: CardGetFileInfo
//

#define WHERE "CardGetFileInfo()"
DWORD WINAPI   CardGetFileInfo
               (
                  __in         PCARD_DATA       pCardData,
                  __in         LPSTR            pszDirectoryName,
                  __in         LPSTR            pszFileName,
                  __in         PCARD_FILE_INFO  pCardFileInfo
               )
{
   DWORD                dwReturn    = 0;
   int                  i           = 0;
   int                  FileFound   = 0;
   int                  DirFound    = 0;
   DWORD                dwVersion   = 0;

   LogTrace(LOGTYPE_INFO, WHERE, "Enter API...");

   /********************/
   /* Check Parameters */
   /********************/
   if ( pCardData == NULL )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pCardData]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }
   if ( ( pszFileName    == NULL ) ||
        ( pszFileName[0] == '\0' ) )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pszFileName]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }
   if ( ( pszDirectoryName != NULL ) &&
        ( ( strlen(pszDirectoryName) < 1 ) || 
          ( strlen(pszDirectoryName) > 8 ) ) )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pszDirectoryName]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }
   if ( pCardFileInfo == NULL )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pCardFileInfo]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }

   dwVersion = (pCardFileInfo->dwVersion == 0) ? 1 : pCardFileInfo->dwVersion;
   if ( dwVersion != CARD_FILE_INFO_CURRENT_VERSION ) 
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pCardFileInfo->dwVersion]");
      CLEANUP(ERROR_REVISION_MISMATCH );
   }

   /* Some trace info */
   if ( pszDirectoryName == NULL )
   {
      LogTrace(LOGTYPE_INFO, WHERE, "pszDirectoryName = [NULL]");
   }
   else
   {
      LogTrace(LOGTYPE_INFO, WHERE, "pszDirectoryName = [%s]", pszDirectoryName);
   }
   if ( pszFileName == NULL )
   {
      LogTrace(LOGTYPE_INFO, WHERE, "pszFileName = [%s]", pszFileName);
   }
   else
   {
      LogTrace(LOGTYPE_INFO, WHERE, "pszFileName = [%s]", pszFileName);
   }
	if ( pszDirectoryName == NULL)                              /* root */
   {
		DirFound++;
		if (_stricmp("cardid", pszFileName) == 0)                   /* /cardid */
		{
			FileFound++;
			pCardFileInfo->cbFileSize      = sizeof(GUID);
   }

		if ( _stricmp("cardapps", pszFileName) == 0)				      /* /cardapps */
   {
			FileFound++;
			pCardFileInfo->cbFileSize      =  5;
		}
		if (_stricmp("cardcf", pszFileName) == 0)					      /* /cardcf */
      {
			FileFound++;
			pCardFileInfo->cbFileSize       = 6;
		}
	}
	else                         									         /* not on root */
	{
		if ( _stricmp("mscp", pszDirectoryName) == 0)               /* /mscp */
		{
         DirFound++;
			if (_stricmp("cmapfile", pszFileName) == 0)			      /* /mscp/cmapfile */
         {
            FileFound++;
				pCardFileInfo->cbFileSize = sizeof(CONTAINER_MAP_RECORD) * 2;
         }
			if ( _stricmp("ksc00", pszFileName) == 0)					   /* /mscp/ksc00 */
			{
				FileFound++;
				dwReturn = PteidReadCert(pCardData, CERT_AUTH, &(pCardFileInfo->cbFileSize), NULL);
				if ( dwReturn != SCARD_S_SUCCESS )
				{
					LogTrace(LOGTYPE_ERROR, WHERE, "PteidReadCert[CERT_AUTH] returned [%d]", dwReturn);
					CLEANUP(SCARD_E_UNEXPECTED);
      }
   }
			if ( _stricmp("ksc01", pszFileName) == 0)					   /* /mscp/ksc01 */
			{
				FileFound++;
				dwReturn = PteidReadCert(pCardData, CERT_NONREP, &(pCardFileInfo->cbFileSize), NULL);
				if ( dwReturn != SCARD_S_SUCCESS )
				{
					LogTrace(LOGTYPE_ERROR, WHERE, "PteidReadCert[CERT_NONREP] returned [%d]", dwReturn);
					CLEANUP(SCARD_E_UNEXPECTED);
				}
			}

			if ( _stricmp("msroots", pszFileName) == 0)					// /mscp/msroots
			{
				FileFound++;
				dwReturn = PteidCreateMSRoots(pCardData, &(pCardFileInfo->cbFileSize), NULL);
				if ( dwReturn != SCARD_S_SUCCESS )
				{
					LogTrace(LOGTYPE_ERROR, WHERE, "PteidCreateMSRoots returned [%d]", dwReturn);
					CLEANUP(SCARD_E_UNEXPECTED);
				}

			}
		}
	}
   if ( ! FileFound )
   {
      if ( ! DirFound )
      {
         LogTrace(LOGTYPE_ERROR, WHERE, "Dir not found");
         CLEANUP(SCARD_E_DIR_NOT_FOUND);
      }
      else
      {
         LogTrace(LOGTYPE_ERROR, WHERE, "File not found");
         CLEANUP(SCARD_E_FILE_NOT_FOUND);
      }
   }

   pCardFileInfo->dwVersion       = CARD_FILE_INFO_CURRENT_VERSION;
	pCardFileInfo->AccessCondition = EveryoneReadUserWriteAc;

   LogTrace(LOGTYPE_INFO, WHERE, "FileInfo: [%d][%d][%d]"
                        , pCardFileInfo->dwVersion
                        , pCardFileInfo->cbFileSize
                        , pCardFileInfo->AccessCondition);

cleanup:
   LogTrace(LOGTYPE_INFO, WHERE, "Exit API...");
   return(dwReturn);
}
#undef WHERE

/****************************************************************************************************/

//
// Function: CardWriteFile
//

#define WHERE "CardWriteFile()"
DWORD WINAPI   CardWriteFile
               (
                  __in                     PCARD_DATA  pCardData,
                  __in                     LPSTR       pszDirectoryName,
                  __in                     LPSTR       pszFileName,
                  __in                     DWORD       dwFlags,
                  __in_bcount(cbData)      PBYTE       pbData,
                  __in                     DWORD       cbData
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
// Function: CardDeleteFile
//
#define WHERE "CardDeleteFile()"
DWORD WINAPI   CardDeleteFile
               (
                  __in    PCARD_DATA  pCardData,
                  __in    LPSTR       pszDirectoryName,
                  __in    LPSTR       pszFileName,
                  __in    DWORD       dwFlags
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
// Function: CardEnumFiles
//
// Purpose: Return a multi-string list of the general files
//          present on this card.  The multi-string is allocated
//          by the card module and must be freed by the CSP.
//
//  The caller must provide a logical file directory name in the
//  pmwszFileNames parameter (see Logical Directory Names, above).
//  The logical directory name indicates which group of files will be
//  enumerated.
//
//  The logical directory name is expected to be a static string, so the
//  the card module will not free it.  The card module
//  will allocate a new buffer in *pmwszFileNames to store the multi-string
//  list of enumerated files using pCardData->pfnCspAlloc.
//
//  If the function fails for any reason, *pmwszFileNames is set to NULL.
//

#define WHERE "CardEnumFiles()"
DWORD WINAPI   CardEnumFiles
               (
                  __in                          PCARD_DATA  pCardData,
                  __in                          LPSTR       pszDirectoryName,
                  __out_ecount(*pdwcbFileName)  LPSTR      *pmszFileNames,
                  __out                         LPDWORD     pdwcbFileName,
                  __in                          DWORD       dwFlags
               )
{
   DWORD                dwReturn    = 0;
   int                  i           = 0;

   int                  iFileNameLg = 0;
   int                  iFileCnt    = 0;
   int                  iIndex      = 0;
	LPSTR                szFileNames;

   LogTrace(LOGTYPE_INFO, WHERE, "Enter API...");

   /********************/
   /* Check Parameters */
   /********************/
   if ( pCardData == NULL )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pCardData]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }
   if ( pmszFileNames == NULL )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pmszFileNames]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }
   if ( pdwcbFileName == NULL )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pdwcbFileName]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }

   if ( dwFlags != 0 )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [dwFlags]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }
   if ( ( pszDirectoryName != NULL ) &&
        ( ( strlen(pszDirectoryName) < 1 ) || 
          ( strlen(pszDirectoryName) > 8 ) ) )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pszDirectoryName]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }

	if ( pszDirectoryName == NULL)                              /* root */
   {
		szFileNames = "cardid\0cardcf\0cardapps\0\0";
		*pdwcbFileName = 24 * sizeof(CHAR);                      /* length of szFileNames */
   }
	else                         									      /* not on root */
   {
		if ( _stricmp("mscp", pszDirectoryName) == 0)            /* /mscp */
      {
			szFileNames = "cmapfile\0msroots\0ksc00\0ksc01\0\0";  /* length of szFileNames */
			*pdwcbFileName = 30 * sizeof(CHAR);                   /* length of szFileNames */
      }
		else
   {
			LogTrace(LOGTYPE_ERROR, WHERE, "Dir not found");
      CLEANUP(SCARD_E_DIR_NOT_FOUND);
   }
	}

   *pmszFileNames = pCardData->pfnCspAlloc(*pdwcbFileName);
   if ( *pmszFileNames == NULL )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Error allocating memory for [*pmszFileNames]");
      CLEANUP(SCARD_E_NO_MEMORY);
   }
	memcpy(*pmszFileNames, szFileNames, *pdwcbFileName);

cleanup:
   LogTrace(LOGTYPE_INFO, WHERE, "Exit API...");
   return(dwReturn);
}
#undef WHERE

/****************************************************************************************************/

//
// Function: CardQueryFreeSpace
//

#define WHERE "CardQueryFreeSpace()"
DWORD WINAPI   CardQueryFreeSpace
               (
                  __in  PCARD_DATA              pCardData,
                  __in  DWORD                   dwFlags,
                  __in  PCARD_FREE_SPACE_INFO   pCardFreeSpaceInfo
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
   if ( dwFlags != 0 )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [dwFlags]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }
   if ( pCardFreeSpaceInfo == NULL )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pCardFreeSpaceInfo]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }
   dwVersion = (pCardFreeSpaceInfo->dwVersion == 0) ? 1 : pCardFreeSpaceInfo->dwVersion;
   if ( dwVersion != CARD_FREE_SPACE_INFO_CURRENT_VERSION )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pCardFileInfo->dwVersion]");
      CLEANUP(ERROR_REVISION_MISMATCH );
   }

   /* We have a read-only card */
   pCardFreeSpaceInfo->dwBytesAvailable          = 0;
   pCardFreeSpaceInfo->dwKeyContainersAvailable  = 0;
   pCardFreeSpaceInfo->dwMaxKeyContainers        = 2; /* Authentication and Non-repudiation key */

cleanup:
   LogTrace(LOGTYPE_INFO, WHERE, "Exit API...");
   return(dwReturn);
}
#undef WHERE

/****************************************************************************************************/
