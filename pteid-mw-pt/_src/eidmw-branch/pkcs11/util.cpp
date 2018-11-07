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
//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
#include "pteid_p11.h"
#include "Mutex.h"
#include "util.h"

using namespace eIDMW;

//EID LOCKING
CMutex g_mutex;

//locking can be provided by app
static CK_C_INITIALIZE_ARGS_PTR _locking;
static void *_lock = NULL;


void memwash(char *p_in, unsigned int len)
{
  for(; len > 0; len--)
    *p_in++ = 0x00;

  return;
}


void strcpy_n(unsigned char *to, const char *from, size_t n, char padding)
{
   size_t c = strlen(from) > n ? n : (int) strlen(from);

   memset((char*) to, padding, n);
   memcpy((char*) to, from, c);  //do not copy 0 char
}




/*
 * Locking functions
 */
CK_RV p11_init_lock(CK_C_INITIALIZE_ARGS_PTR args)
{
CK_RV ret = CKR_OK;

if (_lock)
   return CKR_OK;

// No CK_C_INITIALIZE_ARGS pointer, no locking
if (!args)
   return CKR_OK;

//if (args->pReserved)
//   return CKR_ARGUMENTS_BAD;

//If the app tells us OS locking is okay,
//use that. Otherwise use the supplied functions.
_locking = NULL;

if (args->flags & CKF_OS_LOCKING_OK)
   {
/*
#if (defined(HAVE_PTHREAD) && !defined(PKCS11_THREAD_LOCKING))
                // FIXME:
                // Mozilla uses the CKF_OS_LOCKING_OK flag in C_Initialize().
                // The result is that the Mozilla process doesn't end when
                // closing Mozilla, so you have to kill the process yourself.
                // (If Mozilla would do a C_Finalize, the sc_pkcs11_free_lock()
                // would be called and there wouldn't be a problem.)
                // Therefore, we don't use the PTHREAD locking mechanisms, even
                // if they are requested. This is the old situation which seems
                // to work fine for Mozilla, BUT will cause problems for apps
                // that use multiple threads to access this lib simultaneously.
                // If you do want to use OS threading, compile with
                //   -DPKCS11_THREAD_LOCKING
                //
                 return CKR_OK;
#endif*/
   _lock = (void*) &g_mutex;
      //g_Mutex = new CMutex();
      //if (g_Mutex == NULL)
      //   ret = CKR_CANT_LOCK;
   }
#undef CreateMutex
else if (args->CreateMutex && args->DestroyMutex && args->LockMutex && args->UnlockMutex)
   {
   ret = args->CreateMutex(&_lock);
   if (ret == CKR_OK)
      _locking = args;
   }
#define CreateMutex CreateMutexW

return ret;
}

CK_RV p11_lock()
{
//      if (context == NULL)
//              return CKR_CRYPTOKI_NOT_INITIALIZED;
if (!_lock)
  return CKR_OK;
if (_locking)
  {
  while (_locking->LockMutex(_lock) != CKR_OK)
     ;
  }
else
  {
  g_mutex.Lock();
  }
return ((CK_RV)CKR_OK);
}

static void __p11_unlock(void *lock)
{
if (!lock)
   return;
if (_locking)
   {
   while (_locking->UnlockMutex(lock) != CKR_OK)    ;
   }
else
   {
   g_mutex.Unlock();
   }
}


void p11_unlock()
{
   __p11_unlock(_lock);
}

/*
 * Free the lock - note the lock must be held when
 * you come here
 */
void p11_free_lock()
{
void  *tempLock;

if (!(tempLock = _lock))
    return;

/* Clear the global lock pointer - once we've
* unlocked the mutex it's as good as gone */
_lock = NULL;

/* Now unlock. On SMP machines the synchronization
* primitives should take care of flushing cleanup
* all changed data to RAM */
__p11_unlock(tempLock);

if (_locking)
    _locking->DestroyMutex(tempLock);
else
   {
   //sc_mutex_free((sc_mutex_t *) tempLock);
   }
_locking = NULL;
}



void util_init_lock(void **lock)
{
if (*lock == NULL)
   *lock = (void*) new CMutex();
}

void util_clean_lock(void **lock)
{
if(*lock)
   delete((CMutex*)*lock);

*lock = NULL;
}

void util_lock(void *lock)
{
if(lock)
   {
   CMutex *mutex = (CMutex*) lock;
   mutex->Lock();
   }
}

void util_unlock(void *lock)
{
if(lock)
   {
   CMutex *mutex = (CMutex*) lock;
   mutex->Unlock();
   }
}

