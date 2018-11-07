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
#include "Mutex.h"

#ifndef TRUE
#define TRUE true
#endif
#ifndef FALSE
#define FALSE false
#endif
#ifndef WIN32
#include "Thread.h"
#endif

namespace eIDMW
{

CMutex::CMutex()
{
#ifdef WIN32
	InitializeCriticalSection(&m_Mutex);
#else
	pthread_mutex_init(&m_Mutex, NULL);
	m_MutexLockcount = 0;
	m_MutexOwner = 0;
#endif
}

CMutex::~CMutex()
{
#ifdef WIN32
	EnterCriticalSection(&m_Mutex);
	LeaveCriticalSection(&m_Mutex);
	DeleteCriticalSection(&m_Mutex);
#else
	pthread_mutex_destroy(&m_Mutex);
#endif
}

void CMutex::Lock()
{
#ifdef WIN32
	EnterCriticalSection(&m_Mutex);
#else
	if (pthread_mutex_trylock(&m_Mutex))
	{
		if (! pthread_equal( m_MutexOwner, pthread_self()  ) )
		{
			pthread_mutex_lock(&m_Mutex);
			m_MutexOwner = pthread_self();
			m_MutexLockcount++;
		}
		else 
		{
			m_MutexLockcount++;
		}
	}
	else
	{	
		m_MutexOwner = pthread_self();
		m_MutexLockcount++;
	}
#endif
}

void CMutex::Unlock()
{
#ifdef WIN32
	LeaveCriticalSection(&m_Mutex);
#else
	if ( pthread_equal( m_MutexOwner, pthread_self()  ) )
	{
		if( m_MutexLockcount > 1 )
		{
			m_MutexLockcount--;
		}
		else
		{
			m_MutexOwner = 0;
			m_MutexLockcount--;
			pthread_mutex_unlock(&m_Mutex);
		}
	}
#endif
}

}
