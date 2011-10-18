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

#include "Transaction.h"
#ifndef _WIN32
#include <cstring>
#include <pthread.h>
#include <sys/shm.h>
#include <errno.h>
#endif

using namespace eidmw::pcscproxy;

Transaction::Transaction(const char* name) : cnt(0), isJustStarted(false)
{
	#ifdef _WIN32
	hMutex = CreateMutex(NULL, false, name);
	#else
	key_t key = hash(name);
	shmid = shmget(key, sizeof(pthread_mutex_t), 0644 | IPC_CREAT);
	int   *shm_ptr = (int*) shmat(shmid, (void*) 0, 0);
	hMutex = (pthread_mutex_t*) shm_ptr;
	if (0 == pthread_mutex_trylock(hMutex))
	{
		pthread_mutexattr_t attr;
		pthread_mutexattr_init(&attr);
		pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
		pthread_mutex_init(hMutex, &attr);
		pthread_mutex_unlock(hMutex);
	}
	#endif
}

Transaction::~Transaction()
{
	#ifdef _WIN32
	ReleaseMutex(hMutex);
	CloseHandle(hMutex);
	#else
	shmdt(hMutex);
	shmctl(shmid, IPC_RMID, NULL);
	#endif
}

void Transaction::aquire()
{
	if (0 == cnt++)
	{
		#ifdef _WIN32
		WaitForSingleObject(hMutex, INFINITE);
		#else
		pthread_mutex_lock(hMutex);
		#endif
		isJustStarted = true;
	}
}

void Transaction::release()
{
	if (cnt > 0)
		cnt--;
	if (0 == cnt)
	{
		isJustStarted = false;
		#ifdef _WIN32
		ReleaseMutex(hMutex);
		#else
		pthread_mutex_unlock(hMutex);
		#endif
	}
}

int Transaction::hash(const char* str)
{
	unsigned long hash = 5381;
	char          c;
	while ((c = *str++))
	{
		hash = ((hash << 5) + hash) + c; // hash*33 + c
	}
	return hash;
}

/**
 * Returns true the first time this method is called after an aquire(),
 * false the next times this method is called.
 */
bool Transaction::justStarted()
{
	bool ret = isJustStarted;
	isJustStarted = false;
	return ret;
}


