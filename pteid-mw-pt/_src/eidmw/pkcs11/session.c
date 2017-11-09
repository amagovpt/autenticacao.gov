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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include "windows.h"
#endif
#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif

#ifdef __linux__
//readlink(2)
#include <unistd.h>
#endif

#include "pteid_p11.h"
#include "p11.h"
#include "log.h"
#include "util.h"
#include "cal.h"



#define WHERE "C_OpenSession()"
CK_RV C_OpenSession(CK_SLOT_ID            slotID,        /* the slot's ID */
                    CK_FLAGS              flags,         /* defined in CK_SESSION_INFO */
                    CK_VOID_PTR           pApplication,  /* pointer passed to callback */
                    CK_NOTIFY             Notify,        /* notification callback function */
                    CK_SESSION_HANDLE_PTR phSession)     /* receives new session handle */
{
   int ret;
   P11_SLOT* pSlot = NULL;
   P11_SESSION *pSession = NULL;

//   CAutoMutex(&g_oSlotMutex);
log_trace(WHERE, "I: enter");
   ret = p11_lock(slotID);   /* mutex per slot slot 0 tot 9 FF=global slot*/
   if (ret != CKR_OK)
{
	log_trace(WHERE, "I: leave, p11_lock failed with %i",ret);
   return ((CK_RV)ret);
}

   log_trace(WHERE, "S: C_OpenSession (slot %d)", slotID);

   if (!(flags & CKF_SERIAL_SESSION))
     {
     ret = CKR_SESSION_PARALLEL_NOT_SUPPORTED;
     goto cleanup;
     }

   //XXX check this
/*   if (flags & ~(CKF_SERIAL_SESSION | CKF_RW_SESSION))
      {
      ret = CKR_ARGUMENTS_BAD;
      goto cleanup;
      }*/

   pSlot = p11_get_slot(slotID);
   if (pSlot == NULL)
      {
      log_trace(WHERE, "E: p11_get_slot(%d) returns null", slotID);
      ret = CKR_SLOT_ID_INVALID;
      goto cleanup;
      }

  /* Check that no conflictions sessions exist */
  /* RO session when SO session exists is not allowed */
  if ( !(flags & CKF_RW_SESSION) && (pSlot->login_type == CKU_SO))
     {
     log_trace(WHERE, "E: R/W Session exists", slotID);
     ret = CKR_SESSION_READ_WRITE_SO_EXISTS;
     goto cleanup;
     }

  //get a free session object reserve it by setting inuse flag
  ret = p11_get_free_session(phSession, &pSession);
  if (ret != CKR_OK)
     {
     log_trace(WHERE, "E: p11_get_free_session() returns %d", ret);
     goto cleanup;
     }

  //connect to card if present
  ret = cal_connect(slotID);
  if (ret != CKR_OK)
     {
	//printf("Did not connect to card!!!!\n");
     log_trace(WHERE, "E: cal_connect(slot %d) failed", slotID);
     //release session so it can be reused
     pSession->inuse = 0;
     goto cleanup;
     }
//printf("in use? %d... slotID = %d\n",pSession->inuse,slotID);
  pSession->hslot = slotID;
  pSession->flags = flags;
  pSession->pdNotify = pApplication;
  pSession->pfNotify = Notify;
  //initial state
  pSession->state = P11_CARD_STILL_PRESENT;

  /* keep the nr of sessions for this slot */
  pSlot->nsessions++;

  log_trace(WHERE, "S: Open session (slot %d: hsession = %d )", slotID, *phSession);

cleanup:
   p11_unlock();
   log_trace(WHERE, "I: leave, ret = %i",ret);
   return ((CK_RV)ret);
}
#undef WHERE



#define WHERE "C_CloseSession()"
CK_RV C_CloseSession(CK_SESSION_HANDLE hSession)
{
P11_SESSION *pSession = NULL;
P11_SLOT *pSlot = NULL;
CK_RV ret;
log_trace(WHERE, "I: enter");
ret = p11_lock();
if (ret != CKR_OK)
{
	log_trace(WHERE, "I: leave, p11_lock failed with %i",ret);
   return ((CK_RV)ret);
}

log_trace(WHERE, "S: C_CloseSession (session %d)", hSession);

//get session, of pSession is found, regardless the ret value, we can clean it up
ret = p11_get_session(hSession, &pSession);
if (pSession == NULL)
   {
   log_trace(WHERE, "E: Invalid session handle (%d)", hSession);
   goto cleanup;
   }

//get slot, if not exist, we allow to close session anyway
pSlot = p11_get_slot(pSession->hslot);
if (pSlot == NULL)
   {
   log_trace(WHERE, "W: Invalid slot (%d) for session (%d)", pSession->hslot, hSession);
   //ret = CKR_OK;
   }
else
   {
   //
   if (pSlot->nsessions > 0)
      pSlot->nsessions--;

   if ((pSlot->nsessions < 1) && (pSlot->login_type >= 0) )
      {
      //TODO what to do if no session longer exists?
//      cal_logout(pSlot);
      pSlot->login_type = -1;
      }

   //disconnect this session to device
   ret = cal_disconnect(pSession->hslot);
   }

//clear data so it can be reused
pSession->state = 0;
pSession->inuse = 0;
pSession->flags = 0;
pSession->hslot = 0;
pSession->pdNotify = NULL;
pSession->pfNotify = NULL;

cleanup:
   p11_unlock();
   log_trace(WHERE, "I: leave, ret = %i",ret);
   return ((CK_RV)ret);
}
#undef WHERE




#define WHERE "C_CloseAllSessions()"
CK_RV C_CloseAllSessions(CK_SLOT_ID slotID) /* the token's slot */
{
   int ret;
log_trace(WHERE, "I: enter");
   ret = p11_lock();
if (ret != CKR_OK)
{
	log_trace(WHERE, "I: leave, p11_lock failed with %i",ret);
   return ((CK_RV)ret);
}

   log_trace(WHERE, "S: C_CloseAllSessions(slot %d)", slotID);

   ret = p11_close_all_sessions(slotID);

   p11_unlock();
   log_trace(WHERE, "I: leave, ret = %i",ret);
   return ((CK_RV)ret);
}
#undef WHERE

int isAcroread()
{
   const int buf_len = 1024;
   char *buf = (char*)malloc(buf_len);

#ifdef _WIN32

   DWORD bufsize;
   bufsize = GetModuleFileNameA(NULL,(LPTSTR)buf, (DWORD)buf_len);
   #endif
#ifdef __linux__
   ssize_t s = readlink("/proc/self/exe", buf, (size_t)buf_len);
   buf[s] = 0;
#endif
#ifdef __APPLE__
   _NSGetExecutablePath(buf, &buf_len);
#endif

   if (strstr(buf, "acroread") != NULL
		   || strstr(buf, "AcroRd32") != NULL
		   || strstr(buf, "AdobeReader") != NULL
		   || strstr(buf, "Acrobat") != NULL
		   )
   {
	   fprintf(stderr, "We're being called by acroread!!\n");
	   free(buf);//LL
	   return 1;

   }
   free(buf);//LL
   return 0;

}


#define WHERE "C_GetSessionInfo()"
CK_RV C_GetSessionInfo(CK_SESSION_HANDLE hSession,  /* the session's handle */
                       CK_SESSION_INFO_PTR pInfo)   /* receives session information */
{
   int ret;
   char buf[256];
   P11_SESSION *pSession = NULL;
   P11_SLOT *pSlot = NULL;
   CK_TOKEN_INFO tokeninfo;
   log_trace(WHERE, "I: enter");
   ret = p11_lock();
   if (ret != CKR_OK)
   {
	   log_trace(WHERE, "I: leave, p11_lock failed with %i",ret);
	   return ((CK_RV)ret);
   }

   log_trace(WHERE, "S: C_GetSessionInfo(session %d)", hSession);

   if (pInfo == NULL_PTR)
   {
	   ret = CKR_ARGUMENTS_BAD;
	   goto cleanup;
   }

   ret = p11_get_session(hSession, &pSession);
   if (ret)
   {
	   log_trace(WHERE, "E: Invalid session handle (%d) (%s)", hSession, log_map_error(ret));
	   goto cleanup;
   }



   pInfo->slotID = pSession->hslot;
   pInfo->flags = pSession->flags;
   pInfo->ulDeviceError = 0;

   pSlot = p11_get_slot(pSession->hslot);
   if (pSlot == NULL)
      {
      log_trace(WHERE, "E: slot not found for session %d", hSession);
      ret = CKR_SESSION_HANDLE_INVALID;
      goto cleanup;
      }

   //SO only can create RW_SO sessions
   if (pSlot->login_type == CKU_SO)
      {
      pInfo->state = CKS_RW_SO_FUNCTIONS;
      }
   //USER can create RW or RO sessions
   else if (pSlot->login_type == CKU_USER)
      {
      pInfo->state = (pSession->flags & CKF_RW_SESSION)? CKS_RW_USER_FUNCTIONS : CKS_RO_USER_FUNCTIONS;
      }
   //if login not required => we can also get USER sessions without being logged on
   else
      {
      ret = cal_get_token_info(pSession->hslot, &tokeninfo);
      if ( (ret == CKR_OK) && !(tokeninfo.flags & CKF_LOGIN_REQUIRED) )
         pInfo->state = (pSession->flags & CKF_RW_SESSION)? CKS_RW_USER_FUNCTIONS : CKS_RO_USER_FUNCTIONS;
      else
         pInfo->state = (pSession->flags & CKF_RW_SESSION) ? CKS_RW_PUBLIC_SESSION : CKS_RO_PUBLIC_SESSION;
      }

cleanup:
   p11_unlock();
   log_trace(WHERE, "I: leave, ret = %i",ret);
   return ((CK_RV)ret);
}
#undef WHERE


#define WHERE "C_GetOperationState()"
CK_RV C_GetOperationState(CK_SESSION_HANDLE hSession,             /* the session's handle */
                          CK_BYTE_PTR       pOperationState,      /* location receiving state */
                          CK_ULONG_PTR      pulOperationStateLen) /* location receiving state length */
{
   log_trace(WHERE, "S: C_GetOperationState(sesssion %d)", hSession);
   return CKR_FUNCTION_NOT_SUPPORTED;
}
#undef WHERE



#define WHERE "C_SetOperationState()"
CK_RV C_SetOperationState(CK_SESSION_HANDLE hSession,            /* the session's handle */
                          CK_BYTE_PTR      pOperationState,      /* the location holding the state */
                          CK_ULONG         ulOperationStateLen,  /* location holding state length */
                          CK_OBJECT_HANDLE hEncryptionKey,       /* handle of en/decryption key */
                          CK_OBJECT_HANDLE hAuthenticationKey)   /* handle of sign/verify key */
{
   log_trace(WHERE, "S: C_SetOperationState(session %d)", hSession);
   return CKR_FUNCTION_NOT_SUPPORTED;
}
#undef WHERE


#define WHERE "C_Login()"
CK_RV C_Login(CK_SESSION_HANDLE hSession,  /* the session's handle */
              CK_USER_TYPE      userType,  /* the user type */
              CK_CHAR_PTR       pPin,      /* the user's PIN */
              CK_ULONG          ulPinLen)  /* the length of the PIN */
{
  int ret;
  P11_SESSION *pSession = NULL;
  P11_SLOT *pSlot = NULL;
  CK_TOKEN_INFO tokeninfo;

  log_trace(WHERE, "I: enter");
  ret = p11_lock();

  if (ret != CKR_OK)
  {
    log_trace(WHERE, "I: leave, p11_lock failed with %i",ret);
    return ((CK_RV)ret);
  }

  if (isAcroread())
  {
   return CKR_OK;
  }

  memset(&tokeninfo, 0, sizeof(CK_TOKEN_INFO));

  log_trace(WHERE, "S: Login (session %d)", hSession);

  if (userType != CKU_USER && userType != CKU_SO)
  {
    ret = CKR_USER_TYPE_INVALID;

    goto cleanup;
  }
  ret = p11_get_session(hSession, &pSession);
  if (ret)
  {
    log_trace(WHERE, "E: Invalid session handle (%d)", hSession);
    goto cleanup;
  }

  pSlot = p11_get_slot(pSession->hslot);
  if (pSlot == NULL)
  {
    log_trace(WHERE, "E: Slot not found for session %d", hSession);
    ret = CKR_SESSION_HANDLE_INVALID;
    goto cleanup;
  }

  if (pSlot->login_type >= 0)
  {
	  // Needed for Acrobat, in case you want to sign with a 2nd card
      ret = CKR_OK; //CKR_USER_ALREADY_LOGGED_IN;
      goto cleanup;
  }

  ret = cal_logon(pSession->hslot, ulPinLen, pPin, 0);
  if (ret == CKR_OK) {
      pSlot->login_type = userType;
  }
cleanup:
  p11_unlock();
  log_trace(WHERE, "I: leave, ret = %i",ret);

  return ((CK_RV)ret);
}
#undef WHERE



#define WHERE "C_Logout()"
CK_RV C_Logout(CK_SESSION_HANDLE hSession) /* the session's handle */
{
int ret = CKR_OK;
P11_SESSION *pSession = NULL;
P11_SLOT *pSlot = NULL;
log_trace(WHERE, "I: enter");
ret = p11_lock();
if (ret != CKR_OK)
{
	log_trace(WHERE, "I: leave, p11_lock failed with %i",ret);
   return ((CK_RV)ret);
}

log_trace(WHERE, "S: Logout (session %d)", hSession);

ret = p11_get_session(hSession, &pSession);
if (ret)
   {
   log_trace(WHERE, "E: Invalid session handle (%d)", hSession);
   goto cleanup;
   }

pSlot = p11_get_slot(pSession->hslot);
if (pSlot == NULL)
   {
   log_trace(WHERE, "E: Slot not found for session %d", hSession);
   ret = CKR_SESSION_HANDLE_INVALID;
   goto cleanup;
   }

if (pSlot->login_type >= 0)
   {
   pSlot->login_type = -1;
	//printf("We ask for cal_logout...\n");
   ret = cal_logout(pSession->hslot);
	//printf("We are done!\n");
   }
else
   ret = CKR_USER_NOT_LOGGED_IN;

/* TODO  cleanup all active operations (see standard) */
/* TODO: invalidate all private objects */
/* TODO: destroy all private session objects (we only have private token objects and they are unreadable anyway) */

cleanup:
   p11_unlock();
   log_trace(WHERE, "I: leave, ret = %i",ret);
   return ((CK_RV)ret);
}
#undef WHERE




#define WHERE "C_InitPIN()"
CK_RV C_InitPIN(CK_SESSION_HANDLE hSession,
                CK_CHAR_PTR pPin,
                CK_ULONG ulPinLen)
{
   log_trace(WHERE, "S: C_InitPIN (session %d)", hSession);
   return (CKR_FUNCTION_NOT_SUPPORTED);
}
#undef WHERE


#define WHERE "C_SetPIN()"
CK_RV C_SetPIN(CK_SESSION_HANDLE hSession,
               CK_CHAR_PTR pOldPin,
               CK_ULONG ulOldLen,
               CK_CHAR_PTR pNewPin,
               CK_ULONG ulNewLen)
{
   int ret;
   P11_SESSION *pSession = NULL;
log_trace(WHERE, "I: enter");
   ret = p11_lock();
if (ret != CKR_OK)
{
	log_trace(WHERE, "I: leave, p11_lock failed with %i",ret);
   return ((CK_RV)ret);
}

   log_trace(WHERE, "S: C_SetPIN(session %d)", hSession);

   ret = p11_get_session(hSession, &pSession);
   if (ret)
      {
      log_trace(WHERE, "E: Invalid session handle (%d)", hSession);
      goto cleanup;
      }

   ret = cal_change_pin(pSession->hslot, ulOldLen, pOldPin, ulNewLen, pNewPin);
cleanup:
   p11_unlock();
   log_trace(WHERE, "I: leave, ret = %i",ret);
   return ((CK_RV)ret);
}
#undef WHERE
