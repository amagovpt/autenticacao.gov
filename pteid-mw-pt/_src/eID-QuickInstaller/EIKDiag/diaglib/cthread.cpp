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

#include "cthread.h"

//*********************************************************
//default constructor for class CThread
//*********************************************************
CThread::CThread () {
	this->hThread		= NULL;
	this->hThreadId		= 0;
	this->hMainThread	= ::GetCurrentThread ();
	this->hMainThreadId = ::GetCurrentThreadId ();
	this->Timeout		= 2000; //milliseconds
	this->_terminated   = false;
}

//*********************************************************
//destructor for class CObject
//*********************************************************
CThread::~CThread () {
	//waiting for the thread to terminate
	if (this->hThread) {
		if (::WaitForSingleObject ((HANDLE)this->hThread, this->Timeout) == WAIT_TIMEOUT)
			::TerminateThread ((HANDLE)this->hThread, 1);

		::CloseHandle ((HANDLE)this->hThread);
	}
}

//*********************************************************
//working method
//*********************************************************
unsigned long CThread::Process (void* parameter) {

	//a mechanism for terminating thread should be implemented
	//not allowing the method to be run from the main thread
	if (::GetCurrentThreadId () == this->hMainThreadId)
		return 0;
	else {
		return this->Execute(parameter);
	}

}

//*********************************************************
//creates the thread
//*********************************************************
bool CThread::CreateThread (void* threadParams) {

	if (!this->IsCreated ()) {

		Param* this_param = new Param;
		this_param->pThread	= this;
		this_param->threadParams = threadParams;

		this->hThread = ::_beginthreadex(
			NULL, 
			0, 
			(unsigned int (__stdcall *)(void *))this->runProcess, 
			(void *)this_param, 
			CREATE_SUSPENDED, 
			&this->hThreadId);
		if (this->hThread != 0) {
			this->_terminated = false;
			::ResumeThread((HANDLE)this->hThread);
		}
		return this->hThread ? true : false;
	}
	return false;

}

//*********************************************************
// static -- runs the thread code
//*********************************************************
int CThread::runProcess (void* Parameter) { 
	Param* par = (Param*)Parameter;
	CThread* thread = par->pThread;
	unsigned long result = thread->Process (par->threadParams);
	::_endthreadex(result);
	par->pThread->hThread = NULL;
	delete	(par);
	return result;
}

//*******************
// terminate request
//*******************
void CThread::Terminate() {
	this->_terminated = true;
}
