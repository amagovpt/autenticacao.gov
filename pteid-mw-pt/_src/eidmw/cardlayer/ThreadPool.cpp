/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2009 FedICT.
 * Copyright (C) 2019 Caixa Magica Software.
 * Copyright (C) 2011 Vasco Silva - <vasco.silva@caixamagica.pt>
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
#include "Log.h"

using namespace eIDMW;

static bool g_bStop = true;

#define MAX_THREAD_WAIT_LOOP 100

CEventCallbackThread::CEventCallbackThread(const std::string &csReader, CardInterface &cardInterface,
										   void (*callback)(long lRet, unsigned long ulState, void *pvRef), void *pvRef)
	: m_cardInterface(cardInterface) {
	m_bStop = false;
	m_bRunning = false;
	m_csReader = csReader;
	m_callback = callback;
	m_ulCurrentState = 0;
	m_pvRef = pvRef;
}

void CEventCallbackThread::Run() {
	m_bRunning = true;

	long lRet = EIDMW_OK;
	tReaderInfo tInfo = {m_csReader, m_ulCurrentState, 0};

	try {
		m_cardInterface.EstablishContext();
		while (!g_bStop && !m_bStop) {
			bool bChanged = m_cardInterface.GetStatusChange(10, &tInfo, 1);
			if (g_bStop || m_bStop)
				break;
			else if (bChanged)
				m_callback(lRet, tInfo.ulEventState, m_pvRef);
			else {
				for (int i = 0; i < 5 && !g_bStop && !m_bStop; i++)
					CThread::SleepMillisecs(MAX_THREAD_WAIT_LOOP);
			}
			if (g_bStop || m_bStop)
				break;
		}
	} catch (const CMWException &e) {
		lRet = e.GetError();
	} catch (...) {
		lRet = EIDMW_ERR_UNKNOWN;
	}

	m_bRunning = false;
}

void CEventCallbackThread::Stop() { m_bStop = true; }

bool CEventCallbackThread::HasStopped() { return !m_bRunning; }

////////////////////////////////////////////////////////////////

CThreadPool::CThreadPool() {
	m_ulCurrentHandle = 0;
	g_bStop = false;
}

CThreadPool::~CThreadPool() { g_bStop = true; }

CEventCallbackThread &CThreadPool::NewThread(const std::string &csReader,
											 void (*callback)(long lRet, unsigned long ulState, void *pvRef),
											 unsigned long &ulHandle, void *pvRef) {
// if PCSC is disabled, m_cardInterface must have been initialized to an instance
#if __USE_PCSC__ == 0
	if (!m_cardInterface.has_value() || m_cardInterface.value() == nullptr) {
		MWLOG_CTX(LEV_ERROR, MOD_CAL, "Can not create CEventCallbackThread without a defined card interface.");
		throw CMWEXCEPTION(EIDMW_ERR_NOT_SUPPORTED);
	}
#endif

	CAutoMutex oAutoMutex(&m_mutex);

	CardInterface &cardInterface = *m_cardInterface.value();

	m_ulCurrentHandle++;
	ulHandle = m_ulCurrentHandle;

	auto [it, inserted] =
		m_pool.emplace(m_ulCurrentHandle, CEventCallbackThread(csReader, cardInterface, callback, pvRef));

	return it->second;
}

void CThreadPool::RemoveThread(unsigned long ulHandle) {
	CAutoMutex oAutoMutex(&m_mutex);

	// Signal the EventCallbackThread to stop
	CEventCallbackThread &oEventCallbackThread = m_pool.find(ulHandle)->second;
	oEventCallbackThread.Stop();

	/* Remove all EventCallbackThreads that have stopped,
	 * this may not yet be the one that we just signalled
	 * to stop that's no problem; we'll remove it in one
	 * of the next calls to this function. */
	std::map<unsigned long, CEventCallbackThread>::iterator it;
	bool bChanged = true;
	while (bChanged && m_pool.size() > 0) {
		bChanged = false;
		for (it = m_pool.begin(); it != m_pool.end(); it++) {
			if (it->second.HasStopped()) {
				m_pool.erase(it);
				bChanged = true;
				break;
			}
		}
	}
}

void CThreadPool::FinishThreads() {
	CAutoMutex oAutoMutex(&m_mutex);

	if (m_pool.size() == 0)
		return;

	// First signal all threads to stop
	std::map<unsigned long, CEventCallbackThread>::iterator it;
	for (it = m_pool.begin(); it != m_pool.end(); it++)
		it->second.Stop();

	// Next wait until all threads have stopped.
	// Since the threads should stop in MAX_THREAD_WAIT_LOOP msec,
	// we only wait that long (in fact, we wait a little bit longer,
	// just to make sure..
	for (int i = 0; i < 12; i++) {
		bool bChanged = true;
		while (bChanged && m_pool.size() > 0) {
			bChanged = false;
			for (it = m_pool.begin(); it != m_pool.end(); it++) {
				if (it->second.HasStopped()) {
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

void CThreadPool::SetCardInterface(CardInterface *cardInterface) {
	if (cardInterface == nullptr) {
		MWLOG_CTX(LEV_ERROR, MOD_CAL, "Invalid card interface pointer");
		throw CMWEXCEPTION(EIDMW_ERR_PARAM_BAD);
	}

	this->m_cardInterface = std::make_optional(cardInterface);
}