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
#ifndef __MUTEX_H__
#define __MUTEX_H__

#include "Export.h"

#ifdef WIN32
#include <Windows.h>
#else
#include <pthread.h>
#include "ThreadDefines.h"
#endif

namespace eIDMW
{

class EIDMW_CMN_API CMutex
{
public:
	CMutex();
	~CMutex();

	void Lock();

	void Unlock();

private:
    // No copies allowed
    CMutex(const CMutex & oMutex);
    CMutex & operator = (const CMutex & oMutex);

#ifdef WIN32
	CRITICAL_SECTION m_Mutex;
#else
	pthread_mutex_t m_Mutex;
	pthread_t m_MutexOwner;
	int m_MutexLockcount;
#endif
};

/**
 * A usefull helper class, similar to the std::auto_ptr class.
 * The constructor does a Lock() and the destructor does an Unlock();
 * so you don't need to do an explicit Unlock(), it is done when
 * the CAutoMutex object goes out of scope.
 */
class CAutoMutex
{
public:
	CAutoMutex(CMutex *poMutex): m_poMutex(poMutex)
	{
		m_poMutex->Lock();
	}

	~CAutoMutex()
	{
		m_poMutex->Unlock();
	}

private:
	CMutex *m_poMutex;
};

//-------------------------------
// Abstract base class for mutexes
//-------------------------------
/* The following code is untested
class IMutex
{
	virtual void Lock()   = 0;
	virtual void Unlock() = 0;
};

class SystemMutex : public IMutex
{
public:
#ifdef WIN32
	SystemMutex(HANDLE& mutex, std::string const& name)
		: m_Mutex(mutex)
	{
		SECURITY_ATTRIBUTES* pAttribs = 0;
		mutex = CreateMutex( pAttribs, FALSE, name); 
	}
	virtual ~SystemMutex()
	{
		CloseHandle(m_Mutex);
	}
	virtual void Lock()
	{
		WaitForSingleObject(m_Mutex, INFINITE));
	}
	virtual void Unlock()
	{
		ReleaseMutex(m_Mutex); 
	}
private:
	CRITICAL_SECTION& m_Mutex;

#else
	SystemMutex(pthread_mutex_t* pMutex)
		: m_Mutex(pMutex)
	{
		pthread_mutex_init(m_pMutex, NULL));
	}
	virtual ~SystemMutex()
	{
		pthread_mutex_destroy(m_pMutex);
	}
	virtual void Lock()
	{
		pthread_mutex_lock(m_pMutex);
	}
	virtual void Unlock()
	{
		pthread_mutex_unlock(m_pMutex) 
	}
private:
	pthread_mutex_t* m_pMutex;
	pthread_t		 m_MutexOwner;
	int				 m_MutexLockcount;
#endif

private:
	// No copies allowed
	SystemMutex(const SystemMutex & oMutex);
	SystemMutex & operator = (const SystemMutex & oMutex);

};
*/
}
#endif
