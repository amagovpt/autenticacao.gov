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
#include "log.h"
#include "util.h"
#include "p11.h"
#include "cal.h"

#define WHERE "C_CreateObject()"
CK_RV C_CreateObject(CK_SESSION_HANDLE hSession,    /* the session's handle */
                     CK_ATTRIBUTE_PTR  pTemplate,   /* the object's template */
                     CK_ULONG          ulCount,     /* attributes in template */
                     CK_OBJECT_HANDLE_PTR phObject) /* receives new object's handle. */
{
  log_trace(WHERE, "S: C_CreateObject(): nop");
  return (CKR_FUNCTION_NOT_SUPPORTED);
}
#undef WHERE


#define WHERE "C_CopyObject()"
CK_RV C_CopyObject(CK_SESSION_HANDLE    hSession,    /* the session's handle */
                   CK_OBJECT_HANDLE     hObject,     /* the object's handle */
                   CK_ATTRIBUTE_PTR     pTemplate,   /* template for new object */
                   CK_ULONG             ulCount,     /* attributes in template */
                   CK_OBJECT_HANDLE_PTR phNewObject) /* receives handle of copy */
{
   log_trace(WHERE, "S: C_CopyObject(): nop");
   return CKR_FUNCTION_NOT_SUPPORTED;
}
#undef WHERE





#define WHERE "C_DestroyObject()"
CK_RV C_DestroyObject(CK_SESSION_HANDLE hSession,  /* the session's handle */
                      CK_OBJECT_HANDLE  hObject)   /* the object's handle */
{
log_trace(WHERE, "S: C_DestroyObject(): nop");
return CKR_FUNCTION_NOT_SUPPORTED;
}
#undef WHERE




#define WHERE "C_GetObjectSize()"
CK_RV C_GetObjectSize(CK_SESSION_HANDLE hSession,  /* the session's handle */
                      CK_OBJECT_HANDLE  hObject,   /* the object's handle */
                      CK_ULONG_PTR      pulSize)   /* receives size of object */
{
   log_trace(WHERE, "S: C_GetObjectSize(): nop");
   return CKR_FUNCTION_NOT_SUPPORTED;
}
#undef WHERE



#define WHERE "C_GetAttributeValue()"
CK_RV C_GetAttributeValue(CK_SESSION_HANDLE hSession,   /* the session's handle */
                          CK_OBJECT_HANDLE  hObject,    /* the object's handle */
                          CK_ATTRIBUTE_PTR  pTemplate,  /* specifies attributes, gets values */
                          CK_ULONG          ulCount)    /* attributes in template */
{
/*
This function returns the values from the object.
Object is cached so objects are read only once and remain valid until new session is setup with token.
Objects are allready initialized (but not read) during connection with token.
*/

int status, ret = 0;
P11_SESSION *pSession = NULL;
P11_SLOT    *pSlot    = NULL;
P11_OBJECT  *pObject  = NULL;
unsigned int j = 0;
void  *pValue    = NULL;
CK_ULONG len = 0;

ret = p11_lock();
if (ret != CKR_OK)
   return ((CK_RV)ret);

log_trace(WHERE, "S: C_GetAttributeValue(hObject=%d)",hObject);

ret = p11_get_session(hSession, &pSession);
if (ret)
   {
   log_trace(WHERE, "E: Invalid session handle (%d)", hSession);
   goto cleanup;
   }

pSlot = p11_get_slot(pSession->hslot);
if (pSlot == NULL)
   {
   log_trace(WHERE, "E: p11_get_slot(%d) returns null", pSession->hslot);
   ret = CKR_SLOT_ID_INVALID;
   goto cleanup;
   }

pObject = p11_get_slot_object(pSlot, hObject);
if (pObject == NULL)
   {
   log_trace(WHERE, "E: slot %d: object %d does not exist", pSession->hslot, hObject);
   ret = CKR_OBJECT_HANDLE_INVALID;
   goto cleanup;
   }

//read object from token if not cached allready
if (pObject->state != P11_CACHED)
   {
   ret = cal_read_object(pSession->hslot, pObject);
   if (ret != 0)
      {
      log_trace(WHERE, "E: p11_read_object() returned %d", ret);
      goto cleanup;
      }
   }

/*   if (pSlot->login_type < 0) //CKU_SO=0; CKU_USER=1
   {
   if (p11_get_attribute_value(pObject->pAttr, pObject->count, CKA_PRIVATE, (CK_VOID_PTR *) &pbPrivate, &len) != CKR_OK)
      {
      log_trace(WHERE, "E: missing CKA_PRIVATE attribute in our object: bad implementation");
      continue;
      }

   if ( (len == sizeof(CK_BBOOL)) && (*pbPrivate == CK_TRUE) )
      {
      log_trace(WHERE, "E: Not allowed to retrieve private objects");
      continue;
      }
   }*/

//retrieve all objects as listed in template and fill the template
//action is done for all attributes, even if some attributes give errors or buffer is too small
//there is however only one return code to return so we have to keep the most important return code.
for (j = 0; j < ulCount; j++)
   {
   status = p11_get_attribute_value(pObject->pAttr, pObject->count, pTemplate[j].type, (CK_VOID_PTR *) &pValue, &len);
   if (status != CKR_OK)
      {
      log_attr(&pTemplate[j]);
      log_trace(WHERE, "E: p11_get_attribute_value (object=%d) returned %s", hObject, log_map_error(status));
      pTemplate[j].ulValueLen = (CK_ULONG) -1;
      ret = status;
      continue;
      }

   if (pTemplate[j].pValue == NULL)
      {
      /* in this case we return the real length of the value */
      pTemplate[j].ulValueLen = len;
      continue;
      }

   if (len > pTemplate[j].ulValueLen)
      {
      pTemplate[j].ulValueLen = (CK_ULONG) -1;
      ret = CKR_BUFFER_TOO_SMALL;
      continue;
      }

   pTemplate[j].ulValueLen = len;
   memcpy(pTemplate[j].pValue, pValue, len);
   }

if (ulCount != 0)
   log_template("I: Template out:", pTemplate, ulCount);

cleanup:
    p11_unlock();
    return ((CK_RV)ret);
}
#undef WHERE




#define WHERE "C_SetAttributeValue()"
CK_RV C_SetAttributeValue(CK_SESSION_HANDLE hSession,   /* the session's handle */
                          CK_OBJECT_HANDLE  hObject,    /* the object's handle */
                          CK_ATTRIBUTE_PTR  pTemplate,  /* specifies attributes and values */
                          CK_ULONG          ulCount)    /* attributes in template */
{
   log_trace(WHERE, "S: C_SetAttributeValue(): nop");
   return CKR_FUNCTION_NOT_SUPPORTED;
}
#undef WHERE


#define WHERE "C_FindObjectsInit()"
CK_RV C_FindObjectsInit(CK_SESSION_HANDLE hSession,   /* the session's handle */
                        CK_ATTRIBUTE_PTR  pTemplate,  /* attribute values to match */
                        CK_ULONG          ulCount)    /* attributes in search template */
{
   P11_SESSION *pSession = NULL;
   P11_FIND_DATA *pData = NULL;
   int ret;
   CK_ULONG      *pclass = NULL;
   CK_ULONG       len = 0;

   ret = p11_lock();
   if (ret != CKR_OK)
      return ((CK_RV)ret);

   log_trace(WHERE, "S: C_FindObjectsInit(session %d)", hSession);
   if (ulCount == 0)
      log_trace(WHERE, "I: empty template => search all objects");
   else
      log_template("I: Search template:", pTemplate, ulCount);

   /* add check here to avoid useless calls to C_FindObjects() */
   /* reason to add this is that e.g. Firefox for every certificate in its store starts a find operation with CKA_CLASS_TYPE */
   /* that is unknown by this implementation */
   /* CKA_CLASS_TYPE we support is only CKO_CERTIFICATE, CKO_PRIVATE_KEY and CKO_PUBLIC_KEY */
   /* Sun-PKCS11 cannot handle  CKR_ATTRIBUTE_VALUE_INVALID properly so => initialize search and findObjects will just return 0 matching objects
      in case of CKO_DATA */
   if (ulCount)
      {
      ret = p11_get_attribute_value(pTemplate, ulCount, CKA_CLASS, (CK_VOID_PTR *) &pclass, &len);
      if ( (ret == 0) && (len == sizeof(CK_ULONG) ) )
         {
         //CKO_SECRET_KEY is not supported but for SUN-PKCS11 we allow a search that will result in 0 objects
         if ( (*pclass != CKO_CERTIFICATE) && (*pclass != CKO_PRIVATE_KEY) && (*pclass != CKO_PUBLIC_KEY) && (*pclass != CKO_SECRET_KEY))
            {
            log_trace(WHERE, "I: CKA_CLASS (%0x) not supported by this PKCS11 module", *pclass);
            ret = CKR_ATTRIBUTE_VALUE_INVALID;
            goto cleanup;
            }
         }
      }

   ret = p11_get_session(hSession, &pSession);
// if (pSession == NULL)
   if (ret)
      {
      log_trace(WHERE, "E: Invalid session (%d) (%s)", hSession, log_map_error(ret));
      //if (ret == CKR_DEVICE_REMOVED)
         //ret = CKR_SESSION_HANDLE_INVALID;
         //ret = CKR_FUNCTION_FAILED;
      goto cleanup;
      }

   //is there an active search operation for this session
   if (pSession->Operation[P11_OPERATION_FIND].active)
      {
      log_trace(WHERE, "W: Session %d: search operation allready exists", hSession);
      ret = CKR_OPERATION_ACTIVE;
      goto cleanup;
      }

   /* init search operation */
   if((pData = pSession->Operation[P11_OPERATION_FIND].pData) == NULL)
      {
      pData = pSession->Operation[P11_OPERATION_FIND].pData = (P11_FIND_DATA *) malloc (sizeof(P11_FIND_DATA));
      if (pData == NULL)
         {
         log_trace( WHERE, "E: error allocating memory");
         ret = CKR_HOST_MEMORY;
         }
      }

   //first handle = 1
   pData->hCurrent = 1;
   pData->pSearch  = NULL;
   pData->size     = 0;

   //keep search template if at least one entry in the search template
   if (ulCount > 0)
      {
      pData->pSearch = (CK_ATTRIBUTE_PTR) malloc(sizeof(CK_ATTRIBUTE)*ulCount);
      if (pData->pSearch == NULL)
         {
         log_trace(WHERE, "E: error allocating memory for object search template()");
         ret = CKR_HOST_MEMORY;
         goto cleanup;
         }

      ret = p11_copy_object(pTemplate, ulCount, pData->pSearch);
      if (ret)
         {
         log_trace(WHERE, "E: p11_copy_object() returned %d", ret);
         goto cleanup;
         }
      }

   pData->size = ulCount;

  //set search operation to active state since there can be only one
  pSession->Operation[P11_OPERATION_FIND].active = 1;

ret = CKR_OK;

cleanup:
   p11_unlock();
   return ((CK_RV)ret);
}
#undef WHERE


#define WHERE "C_FindObjects()"
CK_RV C_FindObjects(CK_SESSION_HANDLE    hSession,          /* the session's handle */
                    CK_OBJECT_HANDLE_PTR phObject,          /* receives object handle array */
                    CK_ULONG             ulMaxObjectCount,  /* max handles to be returned */
                    CK_ULONG_PTR         pulObjectCount)    /* actual number returned */
{
/*

this function finds handles to objects but does not actually reads them.
this function returns handles to objects that exist on the token.
PKCS15 defines existance of attributes that should be readable from token

*/

   int ret = 0;
   P11_SESSION   *pSession = NULL;
   P11_SLOT      *pSlot = NULL;
   P11_FIND_DATA *pData = NULL;
   P11_OBJECT    *pObject = NULL;
   CK_BBOOL      *pbToken = NULL;
   void          *p = NULL;
   CK_ULONG      *pclass = NULL;
   int           match = 0;
   unsigned int           h,j = 0;

   CK_ULONG len = 0;

   ret = p11_lock();
   if (ret != CKR_OK)
      return ((CK_RV)ret);

   log_trace(WHERE, "S: C_FindObjects(session %d)", hSession);

   ret = p11_get_session(hSession, &pSession);
   if (pSession == NULL)
// if (ret)
      {
      log_trace(WHERE, "E: Invalid session handle (%d)", hSession);
      goto cleanup;
      }
   
   if (pSession->Operation[P11_OPERATION_FIND].active == 0)
      {
      log_trace(WHERE, "E: For this session no search operation is initiated");
      ret = CKR_OPERATION_NOT_INITIALIZED;
      goto cleanup;
      }

   //get search template
   pData = (P11_FIND_DATA *) pSession->Operation[P11_OPERATION_FIND].pData;
   if (pData == NULL)
      {
      log_trace(WHERE, "E: Session (%d): search data not initialized correctly", hSession);
      ret = CKR_OPERATION_NOT_INITIALIZED;
      goto cleanup;
      }

   /* VSC this code was moved to here since Sun-PKCS11 cannot handle CKR_Attribute_value_invalid in C_FindObjectsInit() properly!!! */
   /* here we just return 0 objects in case of class type that is not supported */
   ret = p11_get_attribute_value(pData->pSearch, pData->size, CKA_CLASS, (CK_VOID_PTR *) &pclass, &len);
   if ( (ret == 0) && (len == sizeof(CK_ULONG) ) )
      {
      if ( (*pclass != CKO_CERTIFICATE) && (*pclass != CKO_PRIVATE_KEY) && (*pclass != CKO_PUBLIC_KEY) )
         {
         ret = CKR_OK; //ret = CKR_ATTRIBUTE_VALUE_INVALID;
         *pulObjectCount = 0;
         goto cleanup;
         }
      }

   //check if we have a TOKEN attribute to look for
   //in case of null search template we search for all objects
   //Firefox does not set TOKEN object for CKO_PRIVATE_KEY objects so for the moment we allow looking for them, id has to match anyway.
   len = sizeof(CK_BBOOL);
   if (pData->size > 0)
      {
      ret = p11_get_attribute_value(pData->pSearch, pData->size, CKA_TOKEN, (CK_VOID_PTR *) &pbToken, &len);
      //if ((ret != CKR_OK) || ( (len == sizeof(CK_BBOOL) ) && (*pbToken == CK_FALSE) ) )
      //for the moment if CKA_TOKEN is specified and set to false, we reply that only token objects can be searched for and continue with ok
      if ((ret == CKR_OK) && (len == sizeof(CK_BBOOL)) && (*pbToken == CK_FALSE) ) 
         {
         log_trace(WHERE, "W: only token objects can be searched for");
         *pulObjectCount = 0;
         ret = CKR_OK;
         goto cleanup;
         }
      }

   pSlot = p11_get_slot(pSession->hslot);
   if (pSlot == NULL)
      {
      log_trace(WHERE, "E: p11_get_slot(%d) returns null", pSession->hslot);
      ret = CKR_SLOT_ID_INVALID;
      goto cleanup;
      }

   *pulObjectCount = 0;

   //for all objects in token, match with search template as long as we need, keep handle to current token object
   for (h = pData->hCurrent; h <= (pSlot->nobjects) && (*pulObjectCount < ulMaxObjectCount); h++, pData->hCurrent++)
      {
      pObject = p11_get_slot_object(pSlot, h);
      if (pObject == NULL)
         {
         log_trace(WHERE, "E: invalid object handle, call C_FindObjectsInit() first");
         ret = CKR_OPERATION_NOT_INITIALIZED;
         goto cleanup;
         }
      if (pObject->inuse == 0)
         continue; //this object is not in use by the token()

      //if not logged in, objects with missing CKA_PRIVATE or CKA_PRIVATE set to false will be ignored
#if 0 //TODO
      if (pSlot->login_type < 0) //CKU_SO=0; CKU_USER=1
         {
         if (p11_get_attribute_value(pObject->pAttr, pObject->count, CKA_PRIVATE, (CK_VOID_PTR *) &pbPrivate, &len) != CKR_OK)
            {
            log_trace(WHERE, "E: missing CKA_PRIVATE attribute in our object: bad implementation");
            continue;
            }

         if ( (len == sizeof(CK_BBOOL)) && (*pbPrivate == CK_TRUE) )
            {
            log_trace(WHERE, "E: Not allowed to retrieve private objects");
            continue;
            }
         }
#endif
      // Try to match every attribute
      match = 1;
      //if pData->size = 0 => this means that we will search for every object!!! match = 1 so for() is skipped and object is returned
      for (j = 0; j < pData->size; j++)
         {
         //get the value of the attribute from the token object and compare with the search attribute
         if (p11_get_attribute_value(pObject->pAttr, pObject->count, pData->pSearch[j].type, &p, &len) != CKR_OK)
            {
            match = 0;
            break;
            }
         if (pData->pSearch[j].ulValueLen != len)
            {
            match = 0;
            break;
            }
         if (memcmp(pData->pSearch[j].pValue, p, len) != 0)
            {
            match = 0;
            break;
            }
         }

      if (match)
         {
         log_trace(WHERE, "I: Slot %d: Object %d matches", pSession->hslot, h);
         //put handle to object in list
         phObject[*pulObjectCount] = (CK_OBJECT_HANDLE) h;
         *pulObjectCount +=1;
         }
      else
         log_trace(WHERE, "I: Slot %d: Object %d no match with search template", pSession->hslot, h);
      }

ret = CKR_OK;

cleanup: 
   p11_unlock();
   return ((CK_RV)ret);
}
#undef WHERE





#define WHERE "C_FindObjectsFinal()"
CK_RV C_FindObjectsFinal(CK_SESSION_HANDLE hSession) /* the session's handle */
{
   P11_SESSION *pSession = NULL;
   P11_FIND_DATA *pData = NULL;
   int ret;

   ret = p11_lock();
   if (ret != CKR_OK)
      return ((CK_RV)ret);

   log_trace(WHERE, "S: C_FindObjectsFinal(session %d)", hSession);

   ret = p11_get_session(hSession, &pSession);
   if (pSession == NULL)
//omit error card removed here since FireFox has a problem with it.
// if (ret)
      {
      log_trace(WHERE, "E: Invalid session handle (%d)", hSession);
      goto cleanup;
      }

   if (pSession->Operation[P11_OPERATION_FIND].active == 0)
      {
      log_trace(WHERE, "I: For this session no search operation is active");
      //we return without problem
      ret = CKR_OK;
      goto cleanup;
      }

   //get search template
   pData = (P11_FIND_DATA *) pSession->Operation[P11_OPERATION_FIND].pData;
   if (pData == NULL)
      {
      log_trace(WHERE, "I: For this session no search operation is active");
      //we return without problem
      ret = CKR_OK;
      goto cleanup;
      }

   //free search template
   if (pData->pSearch)
      free(pData->pSearch);

   pData->pSearch  = NULL;
   pData->hCurrent = 0;
   pData->size     = 0;

   //free find operation data
   free(pData);
   pSession->Operation[P11_OPERATION_FIND].pData = NULL;

   pSession->Operation[P11_OPERATION_FIND].active = 0;

ret = CKR_OK;

cleanup:
   p11_unlock();
   return ((CK_RV)ret);
}
#undef WHERE

