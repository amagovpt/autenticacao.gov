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
#include "ThreadPool.h"

using namespace eIDMW;

static bool g_bStop = true;

#define MAX_THREAD_WAIT_LOOP 100

CEventCallbackThread::CEventCallbackThread()
{
}

CEventCallbackThread::CEventCallbackThread(
	CPCSC *poPCSC, const std::string & csReader,
	void (* callback)(long lRet, unsigned long ulState, void *pvRef), void *pvRef)
{
	m_bStop = false;
	m_bRunning = false;
	m_poPCSC = poPCSC;
	m_csReader = csReader;
	m_callback = callback;
	m_ulCurrentState = 0;
	m_pvRef = pvRef;
}

void CEventCallbackThread::Run()
{
	m_bRunning = true;

	long lRet = EIDMW_OK;
	tReaderInfo tInfo = {m_csReader, m_ulCurrentState, 0};

	try
	{
		while (!g_bStop && ! m_bStop)
		{
			bool bChanged = m_poPCSC->GetStatusChange(10, &tInfo, 1);
			if (g_bStop || m_bStop)
				break;
			else if (bChanged)
				m_callback(lRet, tInfo.ulEventState, m_pvRef);
			else
			{
				for (int i = 0; i < 5 && !g_bStop && ! m_bStop; i++)
					CThread::SleepMillisecs(MAX_THREAD_WAIT_LOOP);
			}
			if (g_bStop || m_bStop)
				break;
		}
	}
	catch(const CMWException & e)
	{
		lRet = e.GetError();
	}
	catch (...)
	{
		lRet = EIDMW_ERR_UNKNOWN;
	}

	m_bRunning = false;
}

void CEventCallbackThread::Stop()
{
	m_bStop = true;
}

bool CEventCallbackThread::HasStopped()
{
	return !m_bRunning;
}

////////////////////////////////////////////////////////////////

CThreadPool::CThreadPool()
{
	m_ulCurrentHandle = 0;
	g_bStop = false;
}

CThreadPool::~CThreadPool()
{
	g_bStop = true;
}

CEventCallbackThread & CThreadPool::NewThread(
	CPCSC *poPCSC, const std::string & csReader,
	void (* callback)(long lRet, unsigned long ulState, void *pvRef),
	unsigned long & ulHandle, void *pvRef)
{
	CAutoMutex oAutoMutex(&m_mutex);

	m_ulCurrentHandle++;

	m_pool[m_ulCurrentHandle] = CEventCallbackThread(
		poPCSC, csReader, callback, pvRef);

	ulHandle = m_ulCurrentHandle;

	return m_pool[m_ulCurrentHandle];
}

void CThreadPool::RemoveThread(unsigned long ulHandle)
{
	CAutoMutex oAutoMutex(&m_mutex);

	// Signal the EventCallbackThread to stop
	CEventCallbackThread & oEventCallbackThread = m_pool[ulHandle];
	oEventCallbackThread.Stop();

	/* Remove all EventCallbackThreads that have stopped,
	 * this may not yet be the one that we just signalled
	 * to stop that's no problem; we'll remove it in one
	 * of the next calls to this function. */
	std::map <unsigned long, CEventCallbackThread>::iterator it;
	bool bChanged = true;
	while (bChanged && m_pool.size() > 0)
	{
		bChanged = false;
		for (it = m_pool.begin(); it != m_pool.end(); it++)
		{
			if (it->second.HasStopped())
			{
				m_pool.erase(it);
				bChanged = true;
				break;
			}
		}
	}
}

void CThreadPool::FinishThreads()
{
	CAutoMutex oAutoMutex(&m_mutex);

	if (m_pool.size() == 0)
		return;

	// First signal all threads to stop
	std::map <unsigned long, CEventCallbackThread>::iterator it;
	for (it = m_pool.begin(); it != m_pool.end(); it++)
		it->second.Stop();

	// Next wait until all threads have stopped.
	// Since the threads should stop in MAX_THREAD_WAIT_LOOP msec,
	// we only wait that long (in fact, we wait a little bit longer,
	// just to make sure..
	for (int i = 0; i < 12; i++)
	{
		bool bChanged = true;
		while (bChanged && m_pool.size() > 0)
		{
			bChanged = false;
			for (it = m_pool.begin(); it != m_pool.end(); it++)
			{
				if (it->second.HasStopped())
				{
					m_pool.erase(it);
					bChanged = true;
					break;
				}
			}
		}

		if (m_pool.size() == 0)
			break;
		else
			CThread::SleepMillisecs(MAX_THREAD_WAIT_LOOP / 10);
	}

	m_mutex.Lock();
}
