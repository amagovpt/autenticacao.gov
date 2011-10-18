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

#ifndef _CTHREADHEADER_
#define _CTHREADHEADER_

#include "stdafx.h"
#include <process.h> // _beginthread...

class CThread {

public:
	//constructors
	CThread ();

	//destructor
	virtual ~CThread ();

	bool CreateThread (void* threadParams);

	int	IsCreated () {	return (this->hThread != NULL);	}

	void Terminate();

	DWORD	Timeout;

	uintptr_t  GetThreadHandle ()	    {	return this->hThread;}
	DWORD	   GetThreadId ()	        {	return this->hThreadId;	}
	HANDLE	   GetMainThreadHandle ()	{	return this->hMainThread;	}
	DWORD	   GetMainThreadId ()	    {	return this->hMainThreadId;	}

protected:

	//overrideable abstract
	virtual unsigned long Execute(void *parameter) = 0;

	unsigned int hThreadId;
	uintptr_t	 hThread;
	DWORD		 hMainThreadId;
	HANDLE		 hMainThread;
	bool		_terminated;

private:

	static int runProcess (void* Param);
	virtual unsigned long Process (void* parameter);	


};

struct Param {
	CThread* pThread;
	void*    threadParams;
};

#endif // _CTHREADHEADER_