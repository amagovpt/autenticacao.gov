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
#pragma once

#ifndef __THREAD_H__
#define __THREAD_H__

#include "Export.h"

#ifdef WIN32
#define __WIN32__
#define VC_GE_2005( _MSC_VER )	( _MSC_VER >= 1400 )
#include <Windows.h> // needed for Sleep()
#else
#ifndef __UNIX__
#define __UNIX__
#endif
#define USE_THREADS
#include <unistd.h>
#endif

#ifndef CRYPT_OK
#define CRYPT_OK 0
#define CRYPT_ERROR -1
#endif

#include "ThreadDefines.h"

#ifdef WIN32
typedef DWORD pteid_thread_id;
#else
typedef pthread_t pteid_thread_id;
#endif

namespace eIDMW
{

class EIDMW_CMN_API CThread
{
public:
	CThread();
	virtual ~CThread();

	/** To be implemented by the subclasses */
	virtual void Run() = 0;

	/** Returns 0 if the thread was started, -1 otherwise */
	virtual int Start();

	/** Returns true if the thread is running, false otherwise */
	virtual bool IsRunning();

	static void SleepMillisecs(int millisecs);

	/** Ask the thread to stop */
	virtual void RequestStop();

	/** Force the thread to stop */
	virtual void ForceStop();

	/** Ask the thread to stop then wait until the thread ends */
	virtual void Stop(unsigned long ulSleepFrequency=100);

	/** Wait until the thread ends */
	virtual void WaitTillStopped(unsigned long ulSleepFrequency=100);

	/** Wait until the thread ends or the timeout occure (in second) 
	iStopMode : 0 = do not stop the thread after timeout
	            1 = request the thread o stop after timeout
				2 = force the thread to stop*/
	virtual bool WaitTimeout(unsigned long ulTimeout,int iStopMode=0);

	THREAD_OBJ m_SyncHandle;
	bool m_isRunning;
	bool m_bStopRequest;	/** Can be taken into account in the run method to end the thread properly before normal ending */

	static int getCurrentPid();
	static pteid_thread_id getCurrentThreadId();

};

}
#endif
