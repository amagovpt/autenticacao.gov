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
#pragma once
#ifndef THREADPOOL_H
#define THREADPOOL_H

#include "PCSC.h"
#include "Thread.h"
#include "Mutex.h"

#include <map>
#include <string>
#include <optional>

namespace eIDMW {

/** Thread class to handle a callback */
class EIDMW_CAL_API CEventCallbackThread : public CThread {
public:
	CEventCallbackThread(const std::string &csReader, CardInterface &cardInterface,
						 void (*callback)(long lRet, unsigned long ulState, void *pvRef), void *pvRef);

	void Run();

	void Stop();

	bool HasStopped();

private:
	bool m_bStop;
	std::string m_csReader;
	void (*m_callback)(long lRet, unsigned long ulState, void *pvRef);
	unsigned long m_ulCurrentState;
	bool m_bRunning;
	void *m_pvRef;
	CardInterface &m_cardInterface;
};

////////////////////////////////////////////////////////////////

class EIDMW_CAL_API CThreadPool {
public:
	CThreadPool();

	~CThreadPool();

	CEventCallbackThread &NewThread(const std::string &csReader,
									void (*callback)(long lRet, unsigned long ulState, void *pvRef),
									unsigned long &ulHandle, void *pvRef);

	void RemoveThread(unsigned long ulHandle);

	void FinishThreads();

	void SetCardInterface(CardInterface *cardInterface);

private:
	unsigned long m_ulCurrentHandle;
	CMutex m_mutex;
	std::optional<CardInterface *> m_cardInterface = std::nullopt;

#ifdef WIN32
// See http://groups.google.com/group/microsoft.public.vc.stl/msg/c4dfeb8987d7b8f0
#pragma warning(push)
#pragma warning(disable : 4251)
#endif
	std::map<unsigned long, CEventCallbackThread> m_pool;
#ifdef WIN32
#pragma warning(pop)
#endif
};

} // namespace eIDMW
#endif
