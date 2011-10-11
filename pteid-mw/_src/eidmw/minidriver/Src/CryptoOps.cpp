extern "C"
{
#include "globmdrv.h"
#include "bcrypt.h"
}


/****************************************************************************************************/

extern BYTE                    gbLoggedOn;
extern VFO_CONTAINER_INFO      gContainerInfo[MAX_CONTAINERS];

/****************************************************************************************************/

//
// Function: CardRSADecrypt
//
// Purpose: Perform a private key decryption on the supplied data.  The
//          card module should assume that pbData is the length of the
//          key modulus.
//

#define WHERE "CardRSADecrypt()"
DWORD WINAPI   CardRSADecrypt
               (
                  __in        PCARD_DATA              pCardData,
                  __inout     PCARD_RSA_DECRYPT_INFO  pInfo
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
// Function:  CardConstructDHAgreement
//
// Purpose: compute a DH secret agreement from a ECDH key on the card
// and the public portion of another ECDH key
//

#define WHERE "CardConstructDHAgreement()"
DWORD WINAPI   CardConstructDHAgreement
               (
                   __in     PCARD_DATA pCardData,
                   __in     PCARD_DH_AGREEMENT_INFO pAgreementInfo
               )
{
   DWORD    dwReturn = 0;
   LogTrace(LOGTYPE_INFO, WHERE, "Enter API...");

   /* 
    * For RSA-only card minidrivers, this entry point is not defined and is
    * set to NULL in the CARD_DATA structure returned from CardAcquireContext
    */
   CLEANUP(SCARD_E_UNSUPPORTED_FEATURE);

cleanup:
   LogTrace(LOGTYPE_INFO, WHERE, "Exit API...");
   return(dwReturn);
}
#undef WHERE

/****************************************************************************************************/

//
// Function:  CardDeriveKey
//
// Purpose: Generate a dervived session key using a generated agreed 
// secret and various other parameters.
//

#define WHERE "CardDeriveKey()"
DWORD WINAPI CardDeriveKey
(
    __in    PCARD_DATA        pCardData,
    __in    PCARD_DERIVE_KEY  pAgreementInfo
)
{
   DWORD    dwReturn = 0;
   LogTrace(LOGTYPE_INFO, WHERE, "Enter API...");

   /* 
    * For RSA-only card minidrivers, this entry point is not defined and is
    * set to NULL in the CARD_DATA structure returned from CardAcquireContext
    */
   CLEANUP(SCARD_E_UNSUPPORTED_FEATURE);

cleanup:
   LogTrace(LOGTYPE_INFO, WHERE, "Exit API...");
   return(dwReturn);
}
#undef WHERE

/****************************************************************************************************/

//
// Function:  CardDestroyDHAgreement
//

#define WHERE "CardDestroyDHAgreement()"
DWORD WINAPI   CardDestroyDHAgreement
(
   __in PCARD_DATA pCardData,
   __in BYTE       bSecretAgreementIndex,
   __in DWORD      dwFlags
)
{
   DWORD    dwReturn = 0;
   LogTrace(LOGTYPE_INFO, WHERE, "Enter API...");

   /* 
    * For RSA-only card minidrivers, this entry point is not defined and is
    * set to NULL in the CARD_DATA structure returned from CardAcquireContext
    */
   CLEANUP(SCARD_E_UNSUPPORTED_FEATURE);

cleanup:
   LogTrace(LOGTYPE_INFO, WHERE, "Exit API...");
   return(dwReturn);
}
#undef WHERE

/****************************************************************************************************/

//
// Function: CardSignData
//
// Purpose: Sign inupt data using a specified key
//

#define WHERE "CardSignData()"
DWORD WINAPI   CardSignData
(
    __in      PCARD_DATA          pCardData,
    __in      PCARD_SIGNING_INFO  pInfo
)
{
   DWORD    dwReturn       = 0;

   BCRYPT_PKCS1_PADDING_INFO  PkcsPadInfo;

   LogTrace(LOGTYPE_INFO, WHERE, "Enter API...");

   /********************/
   /* Check Parameters */
   /********************/
   if ( pCardData == NULL )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pCardData]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }
   if ( pInfo == NULL )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pInfo]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }

   if ( ( pInfo->dwVersion != CARD_SIGNING_INFO_BASIC_VERSION   ) &&
        ( pInfo->dwVersion != CARD_SIGNING_INFO_CURRENT_VERSION ) )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pInfo->dwVersion][0x%X]", pInfo->dwVersion);
      CLEANUP(ERROR_REVISION_MISMATCH);
   }

   if ( pInfo->pbData == NULL )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pInfo->pbData]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }

LogTrace(LOGTYPE_ERROR, WHERE, "[pInfo->bContainerIndex][0x%X]", pInfo->bContainerIndex);
   if ( ( pInfo->bContainerIndex != 0 ) &&
        ( pInfo->bContainerIndex != 1 ) )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pInfo->bContainerIndex]");
      CLEANUP(SCARD_E_NO_KEY_CONTAINER);
   }
LogTrace(LOGTYPE_ERROR, WHERE, "[pInfo->dwKeySpec][0x%X]", pInfo->dwKeySpec);
   if ( pInfo->dwKeySpec != AT_SIGNATURE )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pInfo->dwKeySpec]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }

LogTrace(LOGTYPE_ERROR, WHERE, "[pInfo->dwSigningFlags][0x%X]", pInfo->dwSigningFlags);
   if ( pInfo->dwSigningFlags == 0xFFFFFFFF )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pInfo->dwSigningFlags]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }

   if ( ( pInfo->dwSigningFlags & CARD_BUFFER_SIZE_ONLY ) == CARD_BUFFER_SIZE_ONLY)
   {
      LogTrace(LOGTYPE_INFO, WHERE, "pInfo->dwSigningFlags: CARD_BUFFER_SIZE_ONLY");

      pInfo->cbSignedData = 128;
      CLEANUP(SCARD_S_SUCCESS);
   }

   if ( ( pInfo->dwSigningFlags & CARD_PADDING_INFO_PRESENT ) == CARD_PADDING_INFO_PRESENT)
   {
      LogTrace(LOGTYPE_INFO, WHERE, "pInfo->dwSigningFlags: CARD_PADDING_INFO_PRESENT");
      switch(pInfo->dwPaddingType)
      {
      case CARD_PADDING_PKCS1:
         LogTrace(LOGTYPE_INFO, WHERE, "pInfo->dwPaddingType: CARD_PADDING_PKCS1");
         memcpy (&PkcsPadInfo, pInfo->pPaddingInfo, sizeof(PkcsPadInfo));
         break;
      case CARD_PADDING_PSS:
         LogTrace(LOGTYPE_INFO, WHERE, "pInfo->dwPaddingType: CARD_PADDING_PSS");
         break;
      case CARD_PADDING_NONE:
         LogTrace(LOGTYPE_INFO, WHERE, "pInfo->dwPaddingType: CARD_PADDING_NONE");
         break;
      default:
         LogTrace(LOGTYPE_INFO, WHERE, "pInfo->dwPaddingType: UNSUPPORTED");
         break;
      }
   }

LogTrace(LOGTYPE_ERROR, WHERE, "[pInfo->aiHashAlg][0x%X]");
   if ( pInfo->aiHashAlg == 0xFFFFFFFF )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pInfo->aiHashAlg][0x%X]",pInfo->aiHashAlg);
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }
   if ( ( pInfo->aiHashAlg != CALG_MD2     ) &&
        ( pInfo->aiHashAlg != CALG_MD4     ) &&
        ( pInfo->aiHashAlg != CALG_MD5     ) &&
        ( pInfo->aiHashAlg != CALG_SHA1    ) &&
        ( pInfo->aiHashAlg != CALG_SHA_256 ) &&
        ( pInfo->aiHashAlg != CALG_SHA_384 ) &&
        ( pInfo->aiHashAlg != CALG_SHA_512 ) )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "UNSUPPORTED parameter [pInfo->aiHashAlg][0x%X]",pInfo->aiHashAlg);
      CLEANUP(SCARD_E_UNSUPPORTED_FEATURE);
   }

   if ( gbLoggedOn == 0 )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Not logged on...");
      CLEANUP( SCARD_W_SECURITY_VIOLATION);
   }

#ifdef _DEBUG
   LogTrace(LOGTYPE_INFO, WHERE, "Data to be Signed...[%d]", pInfo->cbData);
   LogDump (pInfo->cbData, (char *)pInfo->pbData);
#endif

   dwReturn = PTeidSignData(pCardData, 
                           pInfo->aiHashAlg,
                           pInfo->cbData, 
                           pInfo->pbData, 
                           &(pInfo->cbSignedData), 
                           &(pInfo->pbSignedData));
   if ( dwReturn != 0 )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "PTeidSignData() returned [0x%X]", dwReturn);
      CLEANUP(dwReturn);
   }

#ifdef _DEBUG
   LogTrace(LOGTYPE_INFO, WHERE, "Signature Data...[%d]", pInfo->cbSignedData);
   LogDump (pInfo->cbSignedData, (char *)pInfo->pbSignedData);
#endif

cleanup:
   LogTrace(LOGTYPE_INFO, WHERE, "Exit API...");
   return(dwReturn);
}
#undef WHERE

/****************************************************************************************************/

//
// Function: CardQueryKeySizes
//

#define WHERE "CardQueryKeySizes()"
DWORD WINAPI   CardQueryKeySizes
               (
                  __in      PCARD_DATA       pCardData,
                  __in      DWORD            dwKeySpec,
                  __in      DWORD            dwFlags,
                  __in      PCARD_KEY_SIZES  pKeySizes
               )
{
   DWORD             dwReturn = 0;

   LogTrace(LOGTYPE_INFO, WHERE, "Enter API...");

   /********************/
   /* Check Parameters */
   /********************/
   if ( pCardData == NULL )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pCardData]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }
   if ( dwKeySpec != AT_SIGNATURE )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [dwKeySpec]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }
   if ( dwFlags != 0 )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [dwKeySpec]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }
   if ( pKeySizes == NULL )
   {
      LogTrace(LOGTYPE_ERROR, WHERE, "Invalid parameter [pKeySizes]");
      CLEANUP(SCARD_E_INVALID_PARAMETER);
   }

   pKeySizes->dwVersion           = CARD_KEY_SIZES_CURRENT_VERSION;
   pKeySizes->dwMinimumBitlen     = 1024;
   pKeySizes->dwDefaultBitlen     = 1024;
   pKeySizes->dwMaximumBitlen     = 1024;
   pKeySizes->dwIncrementalBitlen = 0;

cleanup:
   LogTrace(LOGTYPE_INFO, WHERE, "Exit API...");
   return(dwReturn);
}
#undef WHERE

/****************************************************************************************************/
