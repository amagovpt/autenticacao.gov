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
#include "Thread.h"
#include <stdlib.h>

namespace eIDMW
{

/** run_internal() function, has different
 * return code etc. on different platform */
THREAD_INITFUNCTION(run_internal, arg)
{
	CThread *pThread = (CThread *) arg;

	pThread->Run();

	// Fixme: this should be done after THREAD_END(),
	// but we can't do anything after this; therefore
	// we add a little delay in IsRunning()
	pThread->m_isRunning = false;

	THREAD_END(pThread->m_SyncHandle);
}

CThread::CThread()
{
	m_isRunning = false;
	m_bStopRequest = false;
	m_SyncHandle = NULL;
}
CThread::~CThread() 
{
	THREAD_CLEANUP(m_SyncHandle);
}

int CThread::Start()
{
	int status;
	THREAD_ID threadHandle;

	m_isRunning = true;

	// Here we start the thread. The entry point is the
	// run_internal() function, which on it's turn calls this->Run()
	THREAD_START(run_internal, this, threadHandle, m_SyncHandle, status);

	if (status != 0)
		m_isRunning = false;

	return status;
}

bool CThread::IsRunning()
{
	// To make 'sure' THREAD_END() is called 
	if (!m_isRunning)
		SleepMillisecs(1);

	return m_isRunning;
}

void CThread::SleepMillisecs(int millisecs)
{
#ifdef WIN32
	Sleep(millisecs);
#else
	usleep(millisecs * 1000);
#endif
}

void CThread::RequestStop()
{
	//Ask the thread to stop
	m_bStopRequest=true;
}

void CThread::ForceStop()
{
	//The thread must be killed
	//TODO (Now, we just ask the thread to stop)
	RequestStop();
}

void CThread::Stop(unsigned long ulSleepFrequency)
{
	//Ask the thread to stop
	RequestStop();

	//Wait until the end of the thread
	WaitTillStopped(ulSleepFrequency);
}

void CThread::WaitTillStopped(unsigned long ulSleepFrequency)
{
	//Wait until the end of the thread
	while(IsRunning())
	{
		SleepMillisecs(ulSleepFrequency);
	}
}

bool CThread::WaitTimeout(unsigned long ulTimeout,int iStopMode)
{
	unsigned long ulCount=ulTimeout;

	//Wait until the end of the thread or the timeout
	while(IsRunning() && ulCount>0)
	{
		SleepMillisecs(1000);
		ulCount--;
	}

	/*
	iStopMode : 0 = do not stop the thread after timeout
	            1 = request the thread o stop after timeout
				2 = force the thread to stop
	*/
	if(IsRunning())
	{
		switch(iStopMode)
		{
		case 1:
			RequestStop();
			break;
		case 2:
			ForceStop();
			break;
		default:
			break;
		}
		return false;
	}
	else
	{
		return true;
	}
}

int CThread::getCurrentPid()
{
#ifdef WIN32
	return _getpid();
#else
	return getpid();
#endif	
}

pteid_thread_id CThread::getCurrentThreadId()
{
#ifdef WIN32
	return GetCurrentThreadId();
#else
	return pthread_self();
#endif	
}

}
