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
#include <stdio.h>
#include <string.h>
#include "pteid_p11.h"
#include "util.h"
#include "log.h"
#include "p11.h"
#include "cal.h"

#define LOG_MAX_REC  10

extern CK_FUNCTION_LIST pkcs11_function_list;
//extern void *logmutex;

static int g_final = 0; 
static int g_init  = 0;

#ifndef WIN32
#define _strdup strdup
#define _snprintf snprintf
#endif

//  CMutex gMutex;
//  CMutex SlotMutex[MAX_READERS];

/* If there's no card reader present, this function used to
 * return CKR_DEVICE_ERROR, which causes Mozilla and Firefox
 * to crash.
 * So now we let this function (and C_GetInfo() as well)
 * always return OK, and do the real initialization when
 * C_GetSlotList() is called, this function is logically
 * called immediately after C_Initialize() and/or C_GetInfo().
 */

#define WHERE "C_Initialize()"
CK_RV C_Initialize(CK_VOID_PTR pReserved)
{
	int ret = CKR_OK;
	char sep;
	const char *temp = NULL;
	const char *env_vr = NULL, *tmpvar = NULL;
	char *log_path;
	CK_C_INITIALIZE_ARGS_PTR p_args;
	int isDup = 0;

#ifdef WIN32
	env_vr = "APPDATA";
    sep = '\\';
#else
   env_vr = "HOME";
   sep = '/';
#endif

   temp = getenv(env_vr);
   if (temp != NULL) {
       tmpvar = _strdup(temp);
	   isDup = 1;
   }
   if (tmpvar == NULL)
   {
#ifndef WIN32
	tmpvar = "/tmp";
#else
	tmpvar = "C:\\Temp";
#endif
   }

   log_path = (char *)malloc(strlen(tmpvar)+20);
   _snprintf (log_path, strlen(tmpvar)+20, "%s%c.pteid-pkcs11.log", tmpvar, sep);

#if _DEBUG
   log_init(log_path, LOG_LEVEL_INFO);
#else
   log_init(log_path, LOG_LEVEL_WARNING);
#endif
   free(log_path);

	log_trace(WHERE, "I: enter pReserved = %p",pReserved);
   if (g_init)
      {
      ret = CKR_CRYPTOKI_ALREADY_INITIALIZED;
      log_trace(WHERE, "I: Module is already initialized in this process");
      }
   else
      {
      g_init = 1;
      if (pReserved != NULL)
         {
         p_args = (CK_C_INITIALIZE_ARGS *)pReserved;
         p11_init_lock(p_args);
         }
      cal_init();
      log_trace(WHERE, "S: Initialize this PKCS11 Module");
      log_trace(WHERE, "S: =============================");
      }
   log_trace(WHERE, "I: leave, ret = %i",ret);

   if (isDup > 0) {
      free((char *)tmpvar);
   }

   return ((CK_RV)ret);
}
#undef WHERE



#define WHERE "C_Finalize()"
CK_RV C_Finalize(CK_VOID_PTR pReserved)
{

CK_RV ret = CKR_OK;
	log_trace(WHERE, "I: enter");
if (!g_init)
{
	log_trace(WHERE, "I: leave, CKR_CRYPTOKI_NOT_INITIALIZED");
   return (CKR_CRYPTOKI_NOT_INITIALIZED);
}

ret = p11_lock();
if (ret != CKR_OK)
{
	log_trace(WHERE, "I: leave, p11_lock failed with %i",ret);
   return ret;
}

log_trace(WHERE, "S: C_Finalize()");

if (pReserved != NULL)
   {
   ret = CKR_ARGUMENTS_BAD;
   goto cleanup;
   }

g_final = 0; 

ret = cal_close();

g_init = 0;

cleanup:
  /* Release and destroy the mutex */
  p11_free_lock();
  // util_clean_lock(&logmutex);
log_trace(WHERE, "I: p11_free_lock()");
log_trace(WHERE, "I: leave, ret = %i",ret);
return ret;
}
#undef WHERE




#define WHERE "C_GetInfo()"
CK_RV C_GetInfo(CK_INFO_PTR pInfo)
{

   CK_RV ret = CKR_OK;
	log_trace(WHERE, "I: enter");
   if (pInfo == NULL_PTR)
      {
      ret = CKR_ARGUMENTS_BAD;
      goto cleanup;
      }

   log_trace(WHERE, "S: C_GetInfo()");

   memset(pInfo, 0, sizeof(CK_INFO));
   pInfo->cryptokiVersion.major = 2;
   pInfo->cryptokiVersion.minor = 11;
   strcpy_n(pInfo->manufacturerID,  "Portuguese Government",  sizeof(pInfo->manufacturerID), ' ');
   strcpy_n(pInfo->libraryDescription, "Portugal eID PKCS#11 interface v2", sizeof(pInfo->libraryDescription), ' ');
   pInfo->libraryVersion.major = 1;
   pInfo->libraryVersion.minor = 0;

   cleanup:
   log_trace(WHERE, "I: leave, ret = %i",ret);
   return ret;
}
#undef WHERE



#define WHERE "C_GetFunctionList()"
CK_RV C_GetFunctionList(CK_FUNCTION_LIST_PTR_PTR ppFunctionList)
{
	log_trace(WHERE, "I: enter");
   log_trace(WHERE, "S: C_GetFunctionList()");

   if (ppFunctionList == NULL_PTR)
   {
	   log_trace(WHERE, "I: leave, CKR_ARGUMENTS_BAD");
      return CKR_ARGUMENTS_BAD;
   }

   *ppFunctionList = &pkcs11_function_list;

   log_trace(WHERE, "I: leave, CKR_OK");
   return CKR_OK;
}
#undef WHERE



#define WHERE "C_GetSlotList()"
CK_RV C_GetSlotList(CK_BBOOL       tokenPresent,  /* only slots with token present */
                    CK_SLOT_ID_PTR pSlotList,     /* receives the array of slot IDs */
                    CK_ULONG_PTR   pulCount)      /* receives the number of slots */
{

   P11_SLOT *pSlot;
   CK_RV ret = CKR_OK;
   int h;
   CK_ULONG c = 0;
   static int l=0;

	log_trace(WHERE, "I: enter");
   if (!g_init)
   {
   	log_trace(WHERE, "I: CKR_CRYPTOKI_NOT_INITIALIZED");
      return (CKR_CRYPTOKI_NOT_INITIALIZED);
   }
   ret = p11_lock();
   log_trace(WHERE, "I: p11_lock() acquired");
   if (ret != CKR_OK)
   {
   	log_trace(WHERE, "I: leave, p11_lock failed with %i",ret);
      return (ret);
   }

   if (++l<LOG_MAX_REC)
      log_trace(WHERE, "S: C_GetSlotList()");

   if (pulCount == NULL_PTR)
   {
      ret = CKR_ARGUMENTS_BAD;
      goto cleanup;
   }

//init slots already done
//update info on tokens in slot, could be removed if thread keeps track of these token states
//BUG in Adobe Acrobat reader: adobe asks for slots with pSlotList = NULL, so only nr of slots will be returned. This is ok.
//a second time Adobe calls this, pSlotList still is NULL, so the array with SlotIDs cannot be returned, again, nr of slots is returned.
//Adobe just assumes that the first slot has ID=0 !!! and uses this ID=0 for all further actions.
//to overcome this problem, we start our SlotIDs from 0 and not 1 !!!
   log_trace(WHERE, "I: h=0");
   for (h=0; h < p11_get_nreaders(); h++)
   {
   	   log_trace(WHERE, "I: h=%i",h);
      pSlot = p11_get_slot(h);

      if (l < LOG_MAX_REC)
         log_trace(WHERE, "I: slot[%d]: %s", h, pSlot->name);

      if (tokenPresent == CK_TRUE)
         {
   	//printf("... go cal... \n");
         if (cal_token_present(h))
            {
   		//printf("cal_token_present?\n");
   			 log_trace(WHERE, "I: cal_token_present");
            c++;
            if ((pSlotList != NULL_PTR) && (c <= *pulCount) )
               pSlotList[c-1] =  h;
            }
   	//printf("why???\n");
         continue;
         }
      else
         {
         //get all slots
         c++;
         if ((pSlotList != NULL_PTR) && (c <= *pulCount) )
            {
            pSlotList[c-1] =  h;
            }
         continue;
         }
      } //end for

   //if more slots are found than can be returned in slotlist, return buffer too smal
   if ((c > *pulCount) && (pSlotList != NULL_PTR) )
      ret = CKR_BUFFER_TOO_SMALL;

   //number of slots should always be returned.
   *pulCount = c;
   //printf("There are %d slots\n",c);
   cleanup:
      log_trace(WHERE, "I: p11_unlock()");
      p11_unlock();
      log_trace(WHERE, "I: leave, ret = %i",ret);
   //printf("... bye... bye\n");
   return ret;
}
#undef WHERE


#define WHERE "C_GetSlotInfo()"
CK_RV C_GetSlotInfo(CK_SLOT_ID slotID, CK_SLOT_INFO_PTR pInfo)
{
   CK_RV ret;
   P11_SLOT *slot;
   static int l=0;
	log_trace(WHERE, "I: enter");

   if (!g_init)
   {
	   log_trace(WHERE, "I: CKR_CRYPTOKI_NOT_INITIALIZED");
      return(CKR_CRYPTOKI_NOT_INITIALIZED);
   }

   ret = p11_lock();
   if (ret != CKR_OK)
   {
	   log_trace(WHERE, "I: leave, p11_lock failed with %i",ret);
      return ret;
   }

   if (++l < LOG_MAX_REC)
      log_trace(WHERE, "S: C_GetSlotInfo(slot %d)", slotID);

   if (pInfo == NULL_PTR)
      {
      log_trace(WHERE, "E: pInfo = NULL");
      CLEANUP(CKR_ARGUMENTS_BAD);
      }

   slot = p11_get_slot(slotID);
   if (slot == NULL)
      {
      log_trace(WHERE, "E: p11_get_slot(%d) returns null", slotID);
      CLEANUP(CKR_SLOT_ID_INVALID);
      }

   //fill in slot info
   strcpy_n(pInfo->slotDescription, slot->name, 64, ' ');
	strcpy_n(pInfo->manufacturerID, "_ID_", 32, ' ');
	pInfo->flags = CKF_REMOVABLE_DEVICE | CKF_HW_SLOT;
	pInfo->hardwareVersion.major = 1;
	pInfo->hardwareVersion.minor = 0;
	pInfo->firmwareVersion.major = 1;
   pInfo->firmwareVersion.minor = 0;

   //check if token is present
   if (cal_token_present(slotID))
      //VSC:don't remove other flags
      pInfo->flags |= CKF_TOKEN_PRESENT;

cleanup:
   p11_unlock();
   log_trace(WHERE, "I: leave, ret = %i",ret);
   return ret;
}
#undef WHERE



#define WHERE "C_GetTokenInfo()"
CK_RV C_GetTokenInfo(CK_SLOT_ID slotID, CK_TOKEN_INFO_PTR pInfo)
{
CK_RV ret;
log_trace(WHERE, "I: enter");
if (!g_init)
{
	log_trace(WHERE, "I: CKR_CRYPTOKI_NOT_INITIALIZED");
      return (CKR_CRYPTOKI_NOT_INITIALIZED);
}

   ret = p11_lock();
   if (ret != CKR_OK)
   {
	   log_trace(WHERE, "I: leave, p11_lock failed with %i",ret);
      return ret;
   }

   log_trace(WHERE, "S: C_GetTokenInfo(slot %d)", slotID);
   if (pInfo == NULL_PTR)
      {
      log_trace(WHERE, "E: pInfo = NULL");
      CLEANUP(CKR_ARGUMENTS_BAD);
      }

   ret = cal_get_token_info(slotID, pInfo);
   if (ret != CKR_OK)
      {
      log_trace(WHERE, "E: p11_get_token_info returns %d", ret);
      goto cleanup;
      }

cleanup:
   p11_unlock();
   log_trace(WHERE, "I: leave, ret = %i",ret);
   return ret;
}
#undef WHERE



#define WHERE "C_GetMechanismList()"
CK_RV C_GetMechanismList(CK_SLOT_ID slotID,
                         CK_MECHANISM_TYPE_PTR pMechanismList,
                         CK_ULONG_PTR pulCount)
{
CK_RV ret;
log_trace(WHERE, "I: enter");
if (!g_init)
   return (CKR_CRYPTOKI_NOT_INITIALIZED);

ret = p11_lock();
if (ret != CKR_OK)
{
	log_trace(WHERE, "I: leave, p11_lock failed with %i",ret);
   return ret;
}

log_trace(WHERE, "S: C_GetMechanismList(slot %d)", slotID);

ret = cal_get_mechanism_list(slotID, pMechanismList,pulCount);
if (ret != CKR_OK)
   {
   log_trace(WHERE, "E: cal_get_mechanism_list(slotid=%d) returns %s", slotID, log_map_error(ret));
   goto cleanup;
   }

cleanup:

   p11_unlock();
   log_trace(WHERE, "I: leave, ret = %i",ret);
   return ret;
}
#undef WHERE


#define WHERE "C_GetMechanismInfo()"
CK_RV C_GetMechanismInfo(CK_SLOT_ID slotID,
                         CK_MECHANISM_TYPE type,
                         CK_MECHANISM_INFO_PTR pInfo)
{
CK_RV ret;
log_trace(WHERE, "I: enter");
if (!g_init)
{
	log_trace(WHERE, "I: leave, CKR_CRYPTOKI_NOT_INITIALIZED");
   return (CKR_CRYPTOKI_NOT_INITIALIZED);
}

ret = p11_lock();
if (ret != CKR_OK)
{
	log_trace(WHERE, "I: leave, p11_lock failed with %i",ret);
   return ret;
}

log_trace(WHERE, "S: C_GetMechanismInfo(slot %d)", slotID);

if (pInfo == NULL_PTR)
   {
   CLEANUP(CKR_ARGUMENTS_BAD);
   }

ret = cal_get_mechanism_info(slotID, type, pInfo);
if (ret != CKR_OK)
   {
   log_trace(WHERE, "E: p11_get_mechanism_info(slotid=%d) returns %d", slotID, ret);
   goto cleanup;
   }

cleanup:
   p11_unlock();
   log_trace(WHERE, "I: leave, ret = %i",ret);
   return ret;
}
#undef WHERE





#define WHERE "C_InitToken()"
CK_RV C_InitToken(CK_SLOT_ID slotID,
                  CK_CHAR_PTR pPin,
                  CK_ULONG ulPinLen,
                  CK_CHAR_PTR pLabel)
{
	log_trace(WHERE, "I: CKR_FUNCTION_NOT_SUPPORTED");
log_trace(WHERE, "S: C_InitToken(slot %d)", slotID);
return (CKR_FUNCTION_NOT_SUPPORTED);
}
#undef WHERE


#define WHERE "C_WaitForSlotEvent("
CK_RV C_WaitForSlotEvent(CK_FLAGS flags,   /* blocking/nonblocking flag */
                         CK_SLOT_ID_PTR pSlot,  /* location that receives the slot ID */
                         CK_VOID_PTR pReserved) /* reserved.  Should be NULL_PTR */
{
CK_RV ret = CKR_OK;
int block = 0;
int h;
int cardevent = 0;
P11_SLOT *p11Slot = NULL;
int i = 0;
int locked = 0;

log_trace(WHERE, "I: enter");
ret = p11_lock();
if (ret != CKR_OK)
{
	log_trace(WHERE, "I: leave, p11_lock failed with %i",ret);
  return ret;
}
  locked = 1;

log_trace(WHERE, "S: C_WaitForSlotEvent(flags = 0x%0x)", flags);

// Doesn't seem to work on Linux: if you insert a card then Mozilla freezes
// until you remove the card. This function however seems to work fine.
#ifndef _WIN32
   CLEANUP(CKR_FUNCTION_NOT_SUPPORTED);
#endif

//first check if no events are set for slots in previous run
//this could happen if more cards are inserted/removed at the same time
for (i=0; i < p11_get_nreaders(); i++)
   {
   p11Slot = p11_get_slot(i);
   if (p11Slot->ievent != P11_EVENT_NONE)
      {
      *pSlot = i;
      //clear event
      p11Slot->ievent = P11_EVENT_NONE;
      CLEANUP(CKR_OK);
      }
   }


if (flags & CKF_DONT_BLOCK)
   block = 0;
else
   {
   if (locked)
      {
      p11_unlock();
      locked = 0;
      }
   block = 1;
   }

//log_trace(WHERE, "W: C_WaitForSlotEvent (block=%d)", block);
ret = cal_wait_for_slot_event(block, &cardevent, &h);

if (cardevent == 1)
   *pSlot = h;
else
   CLEANUP(CKR_NO_EVENT);

/* Firefox 1.5 tries to call this function (with the blocking flag)
* in a separate thread; and this causes the pkcs11 lib to hang on Linux
* So we might have to return "not supported" in which case Ff 1.5 defaults
* to polling in the main thread, like before. */

cleanup:

if (locked)
   p11_unlock();

log_trace(WHERE, "I: leave, ret = %i",ret);
return ret;
}
#undef WHERE




CK_FUNCTION_LIST pkcs11_function_list = {
   { 2, 11 },
   C_Initialize,
   C_Finalize,
   C_GetInfo,
   C_GetFunctionList,
   C_GetSlotList,
   C_GetSlotInfo,
   C_GetTokenInfo,
   C_GetMechanismList,
   C_GetMechanismInfo,
   C_InitToken,
   C_InitPIN,
   C_SetPIN,
   C_OpenSession,
   C_CloseSession,
   C_CloseAllSessions,
   C_GetSessionInfo,
   C_GetOperationState,
   C_SetOperationState,
   C_Login,
   C_Logout,
   C_CreateObject,
   C_CopyObject,
   C_DestroyObject,
   C_GetObjectSize,
   C_GetAttributeValue,
   C_SetAttributeValue,
   C_FindObjectsInit,
   C_FindObjects,
   C_FindObjectsFinal,
   C_EncryptInit,
   C_Encrypt,
   C_EncryptUpdate,
   C_EncryptFinal,
   C_DecryptInit,
   C_Decrypt,
   C_DecryptUpdate,
   C_DecryptFinal,
   C_DigestInit,
   C_Digest,
   C_DigestUpdate,
   C_DigestKey,
   C_DigestFinal,
   C_SignInit,
   C_Sign,
   C_SignUpdate,
   C_SignFinal,
   C_SignRecoverInit,
   C_SignRecover,
   C_VerifyInit,
   C_Verify,
   C_VerifyUpdate,
   C_VerifyFinal,
   C_VerifyRecoverInit,
   C_VerifyRecover,
   C_DigestEncryptUpdate,
   C_DecryptDigestUpdate,
   C_SignEncryptUpdate,
   C_DecryptVerifyUpdate,
   C_GenerateKey,
   C_GenerateKeyPair,
   C_WrapKey,
   C_UnwrapKey,
   C_DeriveKey,
   C_SeedRandom,
   C_GenerateRandom,
   C_GetFunctionStatus,
   C_CancelFunction,
   C_WaitForSlotEvent
};
