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
#include <stdlib.h>
#include <string.h>
#include "pteid_p11.h"
#include "util.h"
#include "log.h"
#include "p11.h"
#include "cal.h"
#include "phash.h"

#define WHERE "C_DigestInit()"
CK_RV C_DigestInit(CK_SESSION_HANDLE hSession,   /* the session's handle */
                   CK_MECHANISM_PTR  pMechanism) /* the digesting mechanism */
{
   int ret;
   P11_SESSION *pSession = NULL;
   P11_DIGEST_DATA *pDigestData = NULL;

   ret = p11_lock();
   if (ret != CKR_OK)
          return ((CK_RV)ret);

   log_trace(WHERE, "S: C_DigestInit()");

   ret = p11_get_session(hSession, &pSession);
   if (ret)
      {
      log_trace(WHERE, "E: Invalid session handle (%d)", hSession);
      goto cleanup;
      }

   //is there an active digest operation for this session
   if (pSession->Operation[P11_OPERATION_DIGEST].active)
      {
      log_trace(WHERE, "W: Session %d: digest operation allready exists", hSession);
      ret = CKR_OPERATION_ACTIVE;
      goto cleanup;
      }

   /* init search operation */
   if((pDigestData = pSession->Operation[P11_OPERATION_DIGEST].pData) == NULL)
      {
      pDigestData = pSession->Operation[P11_OPERATION_DIGEST].pData = (P11_DIGEST_DATA *) malloc (sizeof(P11_DIGEST_DATA));
      if (pDigestData == NULL)
         {
         log_trace( WHERE, "E: error allocating memory");
         ret = CKR_HOST_MEMORY;
         }
      }

   memset(pDigestData, 0, sizeof(P11_DIGEST_DATA));
   ret = hash_init(pMechanism, &(pDigestData->phash), &(pDigestData->l_hash));
   if(ret)
      {
      log_trace(WHERE, "E: could not initialize hash()");
      ret = CKR_FUNCTION_FAILED;
      goto cleanup;
      }
   pSession->Operation[P11_OPERATION_DIGEST].active = 1;

cleanup:
   p11_unlock();
   return ((CK_RV)ret);
}
#undef WHERE




#define WHERE "C_Digest()"
CK_RV C_Digest(CK_SESSION_HANDLE hSession,     /* the session's handle */
               CK_BYTE_PTR       pData,        /* data to be digested */
               CK_ULONG          ulDataLen,    /* bytes of data to be digested */
               CK_BYTE_PTR       pDigest,      /* receives the message digest */
               CK_ULONG_PTR      pulDigestLen) /* receives byte length of digest */
{
   int ret;
   P11_SESSION *pSession = NULL;
   P11_DIGEST_DATA *pDigestData = NULL;

   ret = p11_lock();
   if (ret != CKR_OK)
          return ((CK_RV)ret);

   log_trace(WHERE, "S: C_Digest()");

   ret = p11_get_session(hSession, &pSession);
   if (ret)
      {
      log_trace(WHERE, "E: Invalid session handle (%d)", hSession);
      goto cleanup;
      }

   //is there an active search operation for this session
   if ((pSession->Operation[P11_OPERATION_DIGEST].active) == 0)
      {
      log_trace(WHERE, "E: Session %d: no digest operation initialized", hSession);
      ret = CKR_OPERATION_NOT_INITIALIZED;
      goto cleanup;
      }

   /* get digest operation */
   if((pDigestData = pSession->Operation[P11_OPERATION_DIGEST].pData) == NULL)
      {
      log_trace(WHERE, "E: no digest operation initialized");
      ret = CKR_OPERATION_NOT_INITIALIZED;
      goto cleanup;
      }

   if(pDigestData->update)
      {
      log_trace(WHERE, "E: C_Digest() cannot be used to finalize C_DigestUpdate()");
      ret = CKR_FUNCTION_FAILED;
      goto cleanup;
      }

   if (pDigest == NULL)
      {
      *pulDigestLen = pDigestData->l_hash;
      /* return ok without terminating digest params */
      ret = CKR_OK;   
      goto cleanup;
      }

   if (*pulDigestLen < pDigestData->l_hash)
      {
      *pulDigestLen = pDigestData->l_hash;
      ret = CKR_BUFFER_TOO_SMALL;
      goto cleanup;
      }

   ret = hash_update(pDigestData->phash, (char*)pData, ulDataLen);
   if(ret == 0)
      ret = hash_final(pDigestData->phash, pDigest, pulDigestLen);
   if(ret)
      {
      log_trace(WHERE, "E: hash failed()");
      ret = CKR_FUNCTION_FAILED;
      //don't goto cleanup here
      }

   /* terminate digest operation */
   free(pDigestData);
   pSession->Operation[P11_OPERATION_DIGEST].pData = NULL;
   pSession->Operation[P11_OPERATION_DIGEST].active = 0;

cleanup:
   p11_unlock();

return ((CK_RV)ret);
}
#undef WHERE



#define WHERE "C_DigestUpdate()"
CK_RV C_DigestUpdate(CK_SESSION_HANDLE hSession,  /* the session's handle */
                     CK_BYTE_PTR       pPart,     /* data to be digested */
                     CK_ULONG          ulPartLen) /* bytes of data to be digested */
{
   int ret;
   P11_SESSION *pSession = NULL;
   P11_DIGEST_DATA *pDigestData = NULL;

   ret = p11_lock();
   if (ret != CKR_OK)
          return ((CK_RV)ret);

   log_trace(WHERE, "S: C_DigestUpdate()");

   ret = p11_get_session(hSession, &pSession);
   if (ret)
      {
      log_trace(WHERE, "E: Invalid session handle (%d)", hSession);
      goto cleanup;
      }

   //is there an active search operation for this session
   if (pSession->Operation[P11_OPERATION_DIGEST].active == 0)
      {
      log_trace(WHERE, "E: Session %d: no digest operation initialized", hSession);
      ret = CKR_OPERATION_NOT_INITIALIZED;
      goto cleanup;
      }

   /* get digest operation */
   if((pDigestData = pSession->Operation[P11_OPERATION_DIGEST].pData) == NULL)
      {
      log_trace( WHERE, "E: no digest operation initialized");
      ret = CKR_OPERATION_NOT_INITIALIZED;
      goto cleanup;
      }

   ret = hash_update(pDigestData->phash, (char*)pPart, ulPartLen);
   if(ret)
      {
      log_trace(WHERE, "E: hash_update failed()");
      ret = CKR_FUNCTION_FAILED;
      goto cleanup;
      }

cleanup:
   p11_unlock();

return ((CK_RV)ret);
}
#undef WHERE 



#define WHERE "C_DigestKey()"
CK_RV C_DigestKey(CK_SESSION_HANDLE hSession,  /* the session's handle */
                  CK_OBJECT_HANDLE  hKey)      /* handle of secret key to digest */
{
   log_trace(WHERE, "S: C_DigestKey(): nop");
   return CKR_FUNCTION_NOT_SUPPORTED;
}
#undef WHERE


#define WHERE "C_DigestFinal()"
CK_RV C_DigestFinal(CK_SESSION_HANDLE hSession,     /* the session's handle */
                    CK_BYTE_PTR       pDigest,      /* receives the message digest */
                    CK_ULONG_PTR      pulDigestLen) /* receives byte count of digest */
{
   int ret;
   P11_SESSION *pSession = NULL;
   P11_DIGEST_DATA *pDigestData = NULL;

   ret = p11_lock();
   if (ret != CKR_OK)
          return ((CK_RV)ret);

   log_trace(WHERE, "S: C_DigestFinal()");

   ret = p11_get_session(hSession, &pSession);
   if (ret)
      {
      log_trace(WHERE, "E: Invalid session handle (%d)", hSession);
      goto cleanup;
      }

   //is there an active search operation for this session
   if (pSession->Operation[P11_OPERATION_DIGEST].active == 0)
      {
      log_trace(WHERE, "E: Session %d: no digest operation initialized", hSession);
      ret = CKR_OPERATION_NOT_INITIALIZED;
      goto cleanup;
      }

   /* get digest operation */
   if((pDigestData = pSession->Operation[P11_OPERATION_DIGEST].pData) == NULL)
      {
      log_trace( WHERE, "E: no digest operation initialized");
      ret = CKR_OPERATION_NOT_INITIALIZED;
      goto cleanup;
      }

   if (pDigest == NULL)
      {
      *pulDigestLen = pDigestData->l_hash;
      /* return ok without terminating digest params */
      ret = CKR_OK;   
      goto cleanup;
      }

   if (*pulDigestLen < pDigestData->l_hash)
      {
      *pulDigestLen = pDigestData->l_hash;
      ret = CKR_BUFFER_TOO_SMALL;
      goto cleanup;
      }

   ret = hash_final(pDigestData->phash, pDigest, pulDigestLen);
   if(ret)
      {
      log_trace(WHERE, "E: hash_final failed()");
      ret = CKR_FUNCTION_FAILED;
      //don't goto cleanup here
      }
 
   //free digest operation data
   free(pDigestData);
   pSession->Operation[P11_OPERATION_DIGEST].pData = NULL;
   pSession->Operation[P11_OPERATION_DIGEST].active = 0;

cleanup:
   p11_unlock();

return ((CK_RV)ret);
}
#undef WHERE 



#define WHERE "C_SignInit()"
CK_RV C_SignInit(CK_SESSION_HANDLE hSession,    /* the session's handle */
                 CK_MECHANISM_PTR  pMechanism,  /* the signature mechanism */
                 CK_OBJECT_HANDLE  hKey)        /* handle of the signature key */
{
   int ret;
   P11_SESSION *pSession = NULL;
   P11_SLOT    *pSlot = NULL;
   P11_SIGN_DATA *pSignData = NULL;
   P11_OBJECT  *pObject = NULL;

   CK_BBOOL       *pcan_sign = NULL;
   CK_KEY_TYPE    *pkeytype = NULL;
   CK_ULONG       *pmodsize = NULL;
   CK_ULONG       *pid = NULL;
   CK_ULONG       *pclass = NULL;
   CK_ULONG len = 0;
   int ihash;

   ret = p11_lock();
   if (ret != CKR_OK)
      return ((CK_RV)ret);

   log_trace(WHERE, "S: C_SignInit() called with mechanism %d", pMechanism->mechanism);

   ret = p11_get_session(hSession, &pSession);
   if (ret)
      {
      log_trace(WHERE, "E: Invalid session handle (%d)", hSession);
      goto cleanup;
      }

   //is there an active sign operation for this session
   if (pSession->Operation[P11_OPERATION_SIGN].active)
      {
      log_trace(WHERE, "W: Session %d: sign operation allready exists", hSession);
      ret = CKR_OPERATION_ACTIVE;
      goto cleanup;
      }

   pSlot = p11_get_slot(pSession->hslot);
   if (pSlot == NULL)
      {
      log_trace(WHERE, "E: Slot not found for session %d", hSession);
      ret = CKR_SESSION_HANDLE_INVALID;
      goto cleanup;
      }

   //check mechanism
   //since this module is only for PTEID, we check for RSA here and we do not check the device capabilities
   //TODO check mechanism table for signing depending on token in slot
   switch(pMechanism->mechanism)
      {
      case CKM_MD5_RSA_PKCS:
      case CKM_SHA1_RSA_PKCS:
      case CKM_RIPEMD160_RSA_PKCS:
      case CKM_SHA256_RSA_PKCS:
      case CKM_SHA384_RSA_PKCS:
      case CKM_SHA512_RSA_PKCS: ihash = 1; break;
      case CKM_RSA_PKCS:        ihash = 0; break;
      default: 
         ret = CKR_MECHANISM_INVALID;
         goto cleanup;            
      }

   //can we use the object for signing?
   pObject = p11_get_slot_object(pSlot, hKey);
   if (pObject == NULL)
      {
      log_trace(WHERE, "E: invalid key handle");
      ret = CKR_KEY_HANDLE_INVALID;
      goto cleanup;
      }

   //check class, keytype and sign attribute CKO_PRIV_KEY
   /* CKR_KEY_TYPE_INCONSISTENT has higher rank than CKR_KEY_FUNCTION_NOT_PERMITTED */
   ret = p11_get_attribute_value(pObject->pAttr, pObject->count, CKA_KEY_TYPE, (CK_VOID_PTR*) &pkeytype, &len);
   if (ret || (len != sizeof(CK_KEY_TYPE)) || (*pkeytype != CKK_RSA))
      {
      log_trace(WHERE, "E: Wrong keytype");
      ret = CKR_KEY_TYPE_INCONSISTENT;
      goto cleanup;
      }

   ret = p11_get_attribute_value(pObject->pAttr, pObject->count, CKA_CLASS, (CK_VOID_PTR*) &pclass, &len);
   if (ret || (len != sizeof(CK_ULONG)) || (*pclass != CKO_PRIVATE_KEY))
      {
      log_trace(WHERE, "E: Key is not CKO_PRIVATE_KEY");
      ret = CKR_KEY_FUNCTION_NOT_PERMITTED;
      goto cleanup;
      }

   ret = p11_get_attribute_value(pObject->pAttr, pObject->count, CKA_SIGN, (CK_VOID_PTR*) &pcan_sign, &len);
   if (ret || (len != sizeof(CK_BBOOL)) || (*pcan_sign != CK_TRUE))
      {
      log_trace(WHERE, "E: Key cannot be used for signing");
      ret = CKR_KEY_FUNCTION_NOT_PERMITTED;
      goto cleanup;
      }

   ret = p11_get_attribute_value(pObject->pAttr, pObject->count, CKA_MODULUS_BITS, (CK_VOID_PTR*) &pmodsize, &len);
   if (ret || (len != sizeof(CK_ULONG)) )
      {
      log_trace(WHERE, "E: Lengh not defined for modulus bits for private key");
      ret = CKR_FUNCTION_FAILED;
      goto cleanup;
      }

   /* get ID to identify signature key */
   /* at this time, id should be available, otherwise, device is not connected and objects are not initialized */
   ret = p11_get_attribute_value(pObject->pAttr, pObject->count, CKA_ID, (CK_VOID_PTR*) &pid, &len);
   if (ret || (len != sizeof(CK_ULONG)))
      {
      log_trace(WHERE, "E: ID missing for key");
      ret = CKR_FUNCTION_FAILED;
      goto cleanup;
      }

   /* init sign operation */
   if((pSignData = pSession->Operation[P11_OPERATION_SIGN].pData) == NULL)
      {
      pSignData = pSession->Operation[P11_OPERATION_SIGN].pData = (P11_SIGN_DATA *) malloc (sizeof(P11_SIGN_DATA));
      if (pSignData == NULL)
         {
         log_trace( WHERE, "E: error allocating memory");
         ret = CKR_HOST_MEMORY;
         goto cleanup;
         }
      }

   memset(pSignData, 0, sizeof(P11_SIGN_DATA));

   pSignData->mechanism = pMechanism->mechanism;
   pSignData->hKey = hKey;
   if ( pmodsize != NULL )pSignData->l_sign = (*pmodsize+7)/8;
   if ( pid != NULL ) pSignData->id = *pid;

   if (ihash)
      {
      ret = hash_init(pMechanism, &(pSignData->phash), &(pSignData->l_hash));
      if(ret)
         {
         log_trace(WHERE, "E: could not initialize hash()");
         ret = CKR_FUNCTION_FAILED;
         goto cleanup;
         }
      }
   pSession->Operation[P11_OPERATION_SIGN].active = 1;

cleanup:       
   p11_unlock();

return ((CK_RV)ret);
}
#undef WHERE



#define WHERE "C_Sign()"
CK_RV C_Sign(CK_SESSION_HANDLE hSession,        /* the session's handle */
             CK_BYTE_PTR       pData,           /* the data to be signed */
             CK_ULONG          ulDataLen,       /* count of bytes to be signed */
             CK_BYTE_PTR       pSignature,      /* receives the signature */
             CK_ULONG_PTR      pulSignatureLen) /* receives byte count of signature */
{
   int 		  ret         = CKR_OK;
   P11_SESSION*   pSession    = NULL;
   P11_SIGN_DATA* pSignData   = NULL;
   unsigned char* pDigest     = NULL;
   unsigned long  ulDigestLen = 0;
// unsigned int ulSignatureLen = *pulSignatureLen;

   ret = p11_lock();
   if (ret != CKR_OK)
          return ((CK_RV)ret);

   log_trace(WHERE, "S: C_Sign()");

   ret = p11_get_session(hSession, &pSession);
   if (ret)
      {
      log_trace(WHERE, "E: Invalid session handle (%d)", hSession);
      goto cleanup;
      }

   //is there an active search operation for this session
   if (pSession->Operation[P11_OPERATION_SIGN].active == 0)
      {
      log_trace(WHERE, "E: Session %d: no sign operation initialized", hSession);
      ret = CKR_OPERATION_NOT_INITIALIZED;
      goto cleanup;
      }

   /* get sign operation */
   if((pSignData = pSession->Operation[P11_OPERATION_SIGN].pData) == NULL)
      {
      log_trace( WHERE, "E: no sign operation initialized");
      ret = CKR_OPERATION_NOT_INITIALIZED;
      goto cleanup;
      }

   if(pSignData->update)
      {
      log_trace(WHERE, "E: C_Sign() cannot be used to finalize a C_SignUpdate() function");
      ret = CKR_FUNCTION_FAILED;
      goto cleanup;
      }

   if (pSignature == NULL)
      {
      /* just return the signature size */
      *pulSignatureLen = pSignData->l_sign;
      ret = CKR_OK;
      goto cleanup;
      }

   if (pSignData->l_sign > *pulSignatureLen)
      {
      *pulSignatureLen = pSignData->l_sign;
      ret = CKR_BUFFER_TOO_SMALL;
      goto cleanup;
      }

   /* do we have to hash first? */
   if (pSignData->phash)
      {
      /* reserve space for data to sign */
      pDigest = (unsigned char*) malloc(pSignData->l_hash);
      if (pDigest == NULL)
         {
         ret = CKR_HOST_MEMORY;
         goto cleanup;
         }
      ret = hash_update(pSignData->phash, (char*)pData, ulDataLen);
      if(ret == 0)
         ret = hash_final(pSignData->phash, pDigest, &ulDigestLen);
      if(ret)
         {
         log_trace(WHERE, "E: hash failed()");
         ret = CKR_FUNCTION_FAILED;
         goto terminate;
         }
      }
   else
      {
      /* reserve space for data to sign */
      pDigest = (unsigned char*) malloc(ulDataLen);
      if (pDigest == NULL)
         {
         ret = CKR_HOST_MEMORY;
         goto cleanup;
         }
      memcpy(pDigest, pData, ulDataLen);
      ulDigestLen = ulDataLen;
      }

   /* do the signing (and add pkcs headers first if needed) */
   ret = cal_sign(pSession->hslot, pSignData, pDigest, ulDigestLen, pSignature, pulSignatureLen);
   if (ret != CKR_OK)
      log_trace(WHERE, "E: cal_sign() returned %s", log_map_error(ret));

terminate:
   //terminate sign operation
   free(pSignData);
   pSession->Operation[P11_OPERATION_SIGN].pData = NULL;
   pSession->Operation[P11_OPERATION_SIGN].active = 0;

cleanup:        
   if (pDigest)
      free(pDigest);
   p11_unlock();
return ((CK_RV)ret);
}
#undef WHERE


#define WHERE "C_SignUpdate()"
CK_RV C_SignUpdate(CK_SESSION_HANDLE hSession,  /* the session's handle */
                   CK_BYTE_PTR       pPart,     /* the data (digest) to be signed */
                   CK_ULONG          ulPartLen) /* count of bytes to be signed */
{
   int ret;
   P11_SESSION *pSession = NULL;
   P11_SIGN_DATA *pSignData = NULL;

   ret = p11_lock();
   if (ret != CKR_OK)
          return ((CK_RV)ret);

   log_trace(WHERE, "S: C_SignUpdate()");

   ret = p11_get_session(hSession, &pSession);
   if (ret)
      {
      log_trace(WHERE, "E: Invalid session handle (%d)", hSession);
      goto cleanup;
      }

   //is there an active search operation for this session
   if (pSession->Operation[P11_OPERATION_SIGN].active == 0)
      {
      log_trace(WHERE, "E: Session %d: no sign operation initialized", hSession);
      ret = CKR_OPERATION_NOT_INITIALIZED;
      goto cleanup;
      }

   /* get sign operation */
   if((pSignData = pSession->Operation[P11_OPERATION_SIGN].pData) == NULL)
      {
      log_trace( WHERE, "E: no sign operation initialized");
      ret = CKR_OPERATION_NOT_INITIALIZED;
      goto cleanup;
      }

   if (pSignData->phash == NULL)
      {
      if ( (ulPartLen + pSignData->lbuf) > pSignData->l_sign)
         {
         log_trace(WHERE, "E: size not possible for signing");
         ret = CKR_DATA_LEN_RANGE;
         goto cleanup;
         }
      pSignData->pbuf = (char*) realloc(pSignData->pbuf, pSignData->lbuf + ulPartLen);
      if (pSignData->pbuf == NULL)
         {
         log_trace(WHERE, "E: memory allocation problem for host");
         ret = CKR_HOST_MEMORY;
         goto cleanup;
         }
      //add data
      memcpy(pSignData->pbuf+pSignData->lbuf, pPart, ulPartLen);
      pSignData->lbuf += ulPartLen;
      }
   else
      {
      ret = hash_update(pSignData->phash, (char*)pPart, ulPartLen);
      if (ret)
         {
         log_trace(WHERE, "E: hash_update failed");
         ret = CKR_FUNCTION_FAILED;
         goto cleanup;
         }
      }

cleanup:

   p11_unlock();
return ((CK_RV)ret);
}
#undef WHERE



#define WHERE "C_SignFinal()"
CK_RV C_SignFinal(CK_SESSION_HANDLE hSession,        /* the session's handle */
                  CK_BYTE_PTR       pSignature,      /* receives the signature */
                  CK_ULONG_PTR      pulSignatureLen) /* receives byte count of signature */
{
   int ret;
   P11_SESSION *pSession = NULL;
   P11_SIGN_DATA *pSignData = NULL;
   unsigned char *pDigest = NULL;
   unsigned long ulDigestLen = 0;

   ret = p11_lock();
   if (ret != CKR_OK)
          return ((CK_RV)ret);

   log_trace(WHERE, "S: C_SignFinal()");

   ret = p11_get_session(hSession, &pSession);
   if (ret)
   {
      log_trace(WHERE, "E: Invalid session handle (%d)", hSession);      
      goto cleanup;
   }

   //is there an active search operation for this session
   if (pSession->Operation[P11_OPERATION_SIGN].active == 0)
   {
      log_trace(WHERE, "E: Session %d: no sign operation initialized", hSession);
      ret = CKR_OPERATION_NOT_INITIALIZED;
      goto cleanup;
   }

   /* get sign operation */
   if((pSignData = pSession->Operation[P11_OPERATION_SIGN].pData) == NULL)
   {
      log_trace( WHERE, "E: no sign operation initialized");
      ret = CKR_OPERATION_NOT_INITIALIZED;
      goto cleanup;
   }

   /* Specific call to get the allocation size needed for the signature: 
      described in section 5.2 of the PKCS#11 spec
   */
   if(pSignature == NULL)
   {
      *pulSignatureLen = pSignData->l_sign;
      ret = CKR_OK;
      goto cleanup;
   }

   if(*pulSignatureLen < pSignData->l_sign)
   {
      *pulSignatureLen = pSignData->l_sign;
      ret = CKR_BUFFER_TOO_SMALL;
      goto cleanup;
   }   

   if (pSignData->phash)
   {
      /* get hash */
      pDigest = (unsigned char*) malloc(pSignData->l_hash);
      if (pDigest == NULL)
      {
         ret = CKR_HOST_MEMORY;
         goto cleanup;
      }
      
      ret = hash_final(pSignData->phash, pDigest, &ulDigestLen);
      if(ret)
      {
         log_trace(WHERE, "E: hash_final failed()");
         ret = CKR_FUNCTION_FAILED;
         goto cleanup;
      }
   }
   else
   {
      /* no hash: get buffer to sign directly */
      pDigest = (unsigned char*) malloc(pSignData->lbuf);
      if (pDigest == NULL)
         {
         ret = CKR_HOST_MEMORY;
         goto cleanup;
         }
      memcpy(pDigest, pSignData->pbuf, pSignData->lbuf);
      ulDigestLen = pSignData->lbuf;
   }

   ret = cal_sign(pSession->hslot, pSignData, pDigest, ulDigestLen, pSignature, pulSignatureLen);
   if (ret != CKR_OK)
      log_trace(WHERE, "E: cal_sign() returned %s", log_map_error(ret));

   //terminate sign operation
   free(pSignData);
   pSession->Operation[P11_OPERATION_SIGN].pData = NULL;
   pSession->Operation[P11_OPERATION_SIGN].active = 0;

cleanup:
   if (pDigest)
      free(pDigest);

   p11_unlock();

return ((CK_RV)ret);
}
#undef WHERE




#define WHERE "C_SignRecoverInit()"
CK_RV C_SignRecoverInit(CK_SESSION_HANDLE hSession,   /* the session's handle */
                        CK_MECHANISM_PTR  pMechanism, /* the signature mechanism */
                        CK_OBJECT_HANDLE  hKey)       /* handle of the signature key */
{
log_trace(WHERE, "S: C_SignRecoverInit(): nop");
return CKR_FUNCTION_NOT_SUPPORTED;
}
#undef WHERE


#define WHERE "C_SignRecover()"
CK_RV C_SignRecover(CK_SESSION_HANDLE hSession,        /* the session's handle */
                    CK_BYTE_PTR       pData,           /* the data (digest) to be signed */
                    CK_ULONG          ulDataLen,       /* count of bytes to be signed */
                    CK_BYTE_PTR       pSignature,      /* receives the signature */
                    CK_ULONG_PTR      pulSignatureLen) /* receives byte count of signature */
{
log_trace(WHERE, "S: C_SignRecover(): nop");
return CKR_FUNCTION_NOT_SUPPORTED;
}
#undef WHERE 



#define WHERE "C_VerifyInit()"
CK_RV C_VerifyInit(CK_SESSION_HANDLE hSession,    /* the session's handle */
                   CK_MECHANISM_PTR  pMechanism,  /* the verification mechanism */
                   CK_OBJECT_HANDLE  hKey)        /* handle of the verification key */
{
   log_trace(WHERE, "S: C_VerifyInit(): nop");
   return CKR_FUNCTION_NOT_SUPPORTED;
}
#undef WHERE


#define WHERE "C_Verify()"
CK_RV C_Verify(CK_SESSION_HANDLE hSession,       /* the session's handle */
               CK_BYTE_PTR       pData,          /* plaintext data (digest) to compare */
               CK_ULONG          ulDataLen,      /* length of data (digest) in bytes */
               CK_BYTE_PTR       pSignature,     /* the signature to be verified */
               CK_ULONG          ulSignatureLen) /* count of bytes of signature */
{
   log_trace(WHERE, "S: C_Verify(): nop");
   return CKR_FUNCTION_NOT_SUPPORTED;
}
#undef WHERE 



#define WHERE "C_VerifyUpdate()"
CK_RV C_VerifyUpdate(CK_SESSION_HANDLE hSession,  /* the session's handle */
                     CK_BYTE_PTR       pPart,     /* plaintext data (digest) to compare */
                     CK_ULONG          ulPartLen) /* length of data (digest) in bytes */
{
log_trace(WHERE, "S: C_VerifyUpdate(): nop");
return CKR_FUNCTION_NOT_SUPPORTED;
}
#undef WHERE 



#define WHERE "C_VerifyFinal()"
CK_RV C_VerifyFinal(CK_SESSION_HANDLE hSession,       /* the session's handle */
                    CK_BYTE_PTR       pSignature,     /* the signature to be verified */
                    CK_ULONG          ulSignatureLen) /* count of bytes of signature */
{
   log_trace(WHERE, "S: C_VerifyFinal(): nop");
   return CKR_FUNCTION_NOT_SUPPORTED;
}
#undef WHERE



#define WHERE "C_VerifyRecoverInit()"
CK_RV C_VerifyRecoverInit(CK_SESSION_HANDLE hSession,    /* the session's handle */
                          CK_MECHANISM_PTR  pMechanism,  /* the verification mechanism */
                          CK_OBJECT_HANDLE  hKey)        /* handle of the verification key */
{
log_trace(WHERE, "S: C_VerifyRecoverInit(): nop");
   return CKR_FUNCTION_NOT_SUPPORTED;
}
#undef WHERE


#define WHERE "C_VerifyRecover()"
CK_RV C_VerifyRecover(CK_SESSION_HANDLE hSession,        /* the session's handle */
                      CK_BYTE_PTR       pSignature,      /* the signature to be verified */
                      CK_ULONG          ulSignatureLen,  /* count of bytes of signature */
                      CK_BYTE_PTR       pData,           /* receives decrypted data (digest) */
                      CK_ULONG_PTR      pulDataLen)      /* receives byte count of data */
{
   log_trace(WHERE, "S: C_VerifyRecover(): nop");
   return CKR_FUNCTION_NOT_SUPPORTED;
}
#undef WHERE
