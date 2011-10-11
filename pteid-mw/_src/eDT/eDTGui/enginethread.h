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
#ifndef _ENGINETHREAD_H_
#define _ENGINETHREAD_H_

#include <iostream>
#include <string>
#include <QCoreApplication>
#include <qthread.h>
#include <qevent.h>
#include "diagengine.h"
#include "error.h"
#include "AnalysisError.h"

class eDTGui;

struct testSequence
{
	char* currentTest;
	char* nextTestOnPass;
	char* nextTestOnFail;
};
static struct testSequence tstSequence[]=
{
	
#ifdef WIN32	
	 {"system_info"				, "device_info"				, "diagnostics"}
	,{"device_info"				, "software_info"			, "diagnostics"}
	,{"software_info"			, "service_info"			, "diagnostics"}
	,{"service_info"			, "process_info"			, "diagnostics"}
	,{"process_info"			, "module_info"				, "diagnostics"}
	,{"module_info"				, "hardware_info"			, "diagnostics"}	
	,{"hardware_info"			, "middleware_files"		, "diagnostics"}
	,{"middleware_files"		, "reader_detect"			, "diagnostics"}
	//------ end info part ----
	//------ begin infrastructure part ----
	,{"reader_detect"			, "pcsc_detect"				, "diagnostics"}
	,{"pcsc_detect"				, "pcsc_readerlist"			, "diagnostics"}
	,{"pcsc_readerlist"			, "card_detect"				, "diagnostics"}
	,{"card_detect"				, "middleware_info"			, "diagnostics"}
	//------ end infrastructure part ----
	//------ begin middleware part ----
	,{"middleware_info"			, "middleware_readerlist"	, "diagnostics"}
	,{"middleware_readerlist"	, "middleware_cardlist"		, "pcsc_readerlist"}
	,{"middleware_cardlist"		, "middleware_access"		, "pcsc_cardlist"}
	,{"middleware_access"		, "pcsc_readerlist"			, "diagnostics"}

	,{"pcsc_readerlist"			, "pcsc_cardlist"			, "diagnostics"}
	,{"pcsc_cardlist"			, "pcsc_access"				, "diagnostics"}
	,{"pcsc_access"				, "diagnostics"				, "pcsc_timing"}
	,{"pcsc_timing"				, "diagnostics"				, "diagnostics"}
	,{"diagnostics"				, ""						, ""}
	//------ end middleware part ----
#else //OSX: no service, hardware, reader_detect
	//-------------------------------------------------------------
	// current test				, passed					, failed
	//-------------------------------------------------------------
	//------ begin info part ----
	 {"system_info"				, "device_info"				, "diagnostics"}
	,{"device_info"				, "software_info"			, "diagnostics"}
	,{"software_info"			, "process_info"			, "diagnostics"}
	,{"process_info"			, "module_info"				, "diagnostics"}
	,{"module_info"				, "middleware_files"		, "diagnostics"}
	,{"middleware_files"		, "pcsc_detect"				, "diagnostics"}
	//------ end info part ----
	//------ begin infrastructure part ----
	,{"pcsc_detect"				, "pcsc_readerlist"			, "diagnostics"}
	,{"pcsc_readerlist"			, "card_detect"				, "diagnostics"}
	,{"card_detect"				, "middleware_info"			, "diagnostics"}
	//------ end infrastructure part ----
	//------ begin middleware part ----
	,{"middleware_info"			, "middleware_readerlist"	, "diagnostics"}
	,{"middleware_readerlist"	, "middleware_cardlist"		, "pcsc_readerlist"}
	,{"middleware_cardlist"		, "middleware_access"		, "pcsc_cardlist"}
	,{"middleware_access"		, "pcsc_readerlist"			, "diagnostics"}
	
	,{"pcsc_readerlist"			, "pcsc_cardlist"			, "diagnostics"}
	,{"pcsc_cardlist"			, "pcsc_access"				, "diagnostics"}
	,{"pcsc_access"				, "diagnostics"				, "pcsc_timing"}
	,{"pcsc_timing"				, "diagnostics"				, "diagnostics"}
	,{"diagnostics"				, ""						, ""}
	//------ end middleware part ----	
#endif
};

class ProcessEvent : public QEvent
{
public:
	enum eProcessEventType
	{
		PE_UNKNOWN
		, PE_PROGRESS
	};
	ProcessEvent(std::string const& testName, int progress, eProcessEventType eType )
		: QEvent(QEvent::User)
		, m_testName(testName)
		, m_progress(progress)
		, m_eventType(eType)
	{
	}
	~ProcessEvent()
	{
	}
	std::string const& getTestName( void )
	{
		return m_testName;
	}
	std::string const& getFriendlyName( void )
	{
		return m_friendlyName;
	}
	eProcessEventType getType( void )
	{
		return m_eventType;
	}
	int getProgress( void )
	{
		return m_progress;
	}
private:
	std::string			m_testName;
	std::string			m_friendlyName;
	int				    m_progress;
	eProcessEventType	m_eventType;

};

class MyCallbackData : public CallbackData
{
public:
	MyCallbackData()
	{

	}
	~MyCallbackData(){}

public:
};

extern int fn1(void* pUserData,std::string testName,size_t progress);

class EngineThread  : public QThread
{
public:
	class TestInfo
	{
	public:
		std::string m_testName;
		int			m_testErrorCode;
		bool		m_testPassed;
	};
public:
	enum
	{
		  ENGINETHREAD_IDLE = 0
		, ENGINETHREAD_NOREQUEST = ENGINETHREAD_IDLE
		, ENGINETHREAD_RUNNING
		, ENGINETHREAD_REQUEST_STOP
	};
	enum
	{
		  TESTS_NONE = 0
		, TESTS_DIAGNOSTIC
		, TESTS_INTERACTIVE
	};
public:
	EngineThread(eDTGui* pMainWnd = NULL)
		: m_status(ENGINETHREAD_IDLE)
		, m_request(ENGINETHREAD_NOREQUEST)
		, m_testType(TESTS_NONE)
	{
		m_pMainWnd = pMainWnd;
		m_diagEngine.init();
		QThread::setTerminationEnabled(true);
	}
	virtual ~EngineThread()
	{
	}
	void setMainWnd(eDTGui* pMainWnd)
	{
		m_pMainWnd = pMainWnd;
	}

	//------------------------------------------
	// Entry point of the worker thread
	// The thread will execute depending on its setting:
	// - the diagnostics tests (diagnostics)
	// - signing tests (interactive)
	//------------------------------------------
	virtual void run()
	{
		switch(m_testType)
		{
		case TESTS_DIAGNOSTIC:
			run_diagnostics();
			break;
		case TESTS_INTERACTIVE:
			run_interactive();
		default:
			break;
		}
	}
	void runTest(DiagEngine::tTestNames const& testsToRun, DiagEngine::tTestCallbackList const& callbackList, std::string const& currentTest)
	{
		DiagEngine::tTestNames		  requestList;
		DiagEngine::tTestCallbackList cbList;
		for (size_t testNr = 0; testNr<testsToRun.size(); testNr++)
		{
			if (currentTest == testsToRun.at(testNr))
			{
				requestList.insert(requestList.end(),currentTest);
				cbList.insert(cbList.end(),callbackList.at(testNr));
				break;
			}
		}
		//-----------------------------------------
		// Run the test and get the return code
		//-----------------------------------------
		int retVal = m_diagEngine.run(requestList,cbList);
		
		//-----------------------------------------
		// Keep the error code returned from the test
		//-----------------------------------------
		TestInfo testInfo;
		testInfo.m_testName		 = currentTest;
		testInfo.m_testErrorCode = retVal;
		testInfo.m_testPassed	 = m_diagEngine.testPassed(currentTest);

		m_testInfo.insert(m_testInfo.end(),testInfo);
	}
	//-----------------------------------------
	// get the next test after the current test
	//-----------------------------------------
	std::string getNextTest(size_t currentTest)
	{
		std::string nextTest("");
		nextTest = m_testInfo.at(currentTest).m_testPassed?tstSequence[currentTest].nextTestOnPass:tstSequence[currentTest].nextTestOnFail;
		return nextTest;
	}
	//------------------------------------------
	// run the diagnostic tests
	//------------------------------------------
	virtual void run_diagnostics(void)
	{
		m_status = ENGINETHREAD_RUNNING;

		//------------------------------------------
		// first of all, make a list of all the test we want to run
		// and associate a callback function to each test we want to run
		//------------------------------------------
		DiagEngine::tTestNames		  testsToRun;
		DiagEngine::tTestCallbackList callbackList;

		for (size_t testIdx=0;testIdx<sizeof(tstSequence)/sizeof(struct testSequence);testIdx++)
		{
			testsToRun.insert(testsToRun.end(),std::string(tstSequence[testIdx].currentTest));
			MyCallbackData* myCallbackData	= new MyCallbackData;
			myCallbackData->m_callback		= (int(*)(void*,std::string,size_t))&fn1;
			myCallbackData->m_userData		= m_pMainWnd;
			callbackList.insert(callbackList.end(),myCallbackData);
		}

		m_NrRequestedTests = testsToRun.size();

		//------------------------------------------
		// run each test separately.
		// iterate over the list of tests by copying the test name and its callback
		// to a list. This way we can see the result of the test and change the sequence 
		// if needed.
		//------------------------------------------

		std::string currentTest(tstSequence[0].currentTest);
		m_currTestIdx = 0;

		bool bDone = false;

		do 
		{
			runTest(testsToRun,callbackList,currentTest);
			std::string nextTest = getNextTest(m_currTestIdx);
			if (0 == nextTest.length() || currentTest.compare("diagnostics")==0)
			{
				bDone = true;
			}
			else
			{
				m_currTestIdx++;
				currentTest = nextTest;
			}
			if (ENGINETHREAD_REQUEST_STOP == m_request)
			{
				m_request = ENGINETHREAD_NOREQUEST;
				break;
			}
		} 
		while(!bDone);

		//-----------------------------------------
		// We're done, remove the callbacks
		//-----------------------------------------
		for (size_t idx=0;idx<callbackList.size();idx++)
		{
			MyCallbackData* data = (MyCallbackData*)callbackList.at(idx);
			delete data;
		}

		m_status = ENGINETHREAD_IDLE;
	}
	//------------------------------------------
	// run the interactive tests (signing)
	//------------------------------------------
	virtual void run_interactive(void)
	{
		time_t* pHearbeat = m_diagEngine.getHeartbeat();
		*pHearbeat = time(NULL);
		int retVal = DiagEngine::ENG_OK;
		m_status = ENGINETHREAD_RUNNING;

		DiagEngine::tTestNames testsToRun;
		testsToRun.insert(testsToRun.end(),std::string("sign_pkcs11"));
#ifdef WIN32
		testsToRun.insert(testsToRun.end(),std::string("sign_csp"));
#else
		testsToRun.insert(testsToRun.end(),std::string("sign_tokend"));
#endif

		testsToRun.insert(testsToRun.end(),std::string("diagnostics"));
		m_NrRequestedTests = testsToRun.size();

		DiagEngine::tTestNames		  requestList;
		DiagEngine::tTestCallbackList callbackList;

		for(int i=0;i<2;i++)
		{
			MyCallbackData* myCallbackData	= new MyCallbackData;
			myCallbackData->m_callback		= (int(*)(void*,std::string,size_t))&fn1;
			myCallbackData->m_userData		= m_pMainWnd;
			callbackList.insert(callbackList.end(),myCallbackData);
		}

		for (size_t testNr = 0; testNr<testsToRun.size(); testNr++)
		{
			std::string testname = testsToRun.at(testNr);
			requestList.insert(requestList.end(),testsToRun.at(testNr));

			//-----------------------------------------
			// Run the test and get the return code
			//-----------------------------------------
			int retVal = m_diagEngine.run(requestList,callbackList);

			//-----------------------------------------
			// Keep the error code returned from the test
			//-----------------------------------------
			TestInfo testInfo;
			testInfo.m_testName		 = testname;
			testInfo.m_testErrorCode = retVal;
			testInfo.m_testPassed	 = m_diagEngine.testPassed(testname);

			m_testInfo.insert(m_testInfo.end(),testInfo);
			if (DiagEngine::ENG_OK!=retVal)
			{
				m_lastError = retVal;
			}

			requestList.clear();
			if (ENGINETHREAD_REQUEST_STOP == m_request)
			{
				m_request = ENGINETHREAD_NOREQUEST;
				break;
			}
		}

		//-----------------------------------------
		// We're done, remove the callbacks
		//-----------------------------------------
		for (size_t idx=0;idx<callbackList.size();idx++)
		{
			MyCallbackData* data = (MyCallbackData*)callbackList.at(idx);
			delete data;
		}

		m_status = ENGINETHREAD_IDLE;
	}

	int getStatus( void )
	{
		return m_status;
	}

	void setRequest( int status )
	{
		m_request = status;
	}
	size_t getNrAvailableTests()
	{
		return m_diagEngine.getNrAvailableTests();
	}
	size_t getNrOfExecutedTests()
	{
		return m_testInfo.size();
	}
	std::string getExecutedTestName(size_t testIdx)
	{
		return m_testInfo.at(testIdx).m_testName;
	}
	size_t getNrRequestedTests()
	{
		//if(TESTS_DIAGNOSTIC==m_testType)
		//{
			return m_NrRequestedTests;
		//}
		//return 0;
	}
	std::string getAvailableTestName(size_t testIdx)
	{
		std::string testName;
		testName = m_diagEngine.getAvailableTestNames().at(testIdx);
		return testName;
	};
	std::string getTestFriendlyName(size_t testIdx)
	{
		std::string testName;
		testName = m_diagEngine.getTestFriendlyNames().at(testIdx);
		return testName;
	};
	std::string getTestFriendlyName(std::string const& testName)
	{
		std::string testFriendlyName;
		const DiagEngine::tTestNames& testNames = m_diagEngine.getAvailableTestNames();
		for (size_t idx=0;idx<testNames.size();idx++)
		{
			if (testNames.at(idx)==testName)
			{
				testFriendlyName = m_diagEngine.getTestFriendlyNames().at(idx);
				break;
			}
		}
		return testFriendlyName;
	};
	void setTestType(size_t type)
	{
		m_testType = type;
	}
	int getReportFilename(std::wstring& reportFile,bool bAbstractOnly = false)
	{
		return m_diagEngine.getReportFilename( reportFile, bAbstractOnly);
	}
	int getTestErrorCode(size_t testNr)
	{
		return m_testInfo.at(testNr).m_testErrorCode;
	};
	int getTestErrorCode(std::string const& testName)
	{
		int retVal = ANALYSE_TEST_NOT_FOUND;
		for (size_t testNr=0;testNr<m_testInfo.size();testNr++)
		{
			if (m_testInfo.at(testNr).m_testName==testName)
			{
				retVal = m_testInfo.at(testNr).m_testErrorCode;
				break;
			}
		}
		return retVal;
	}
	bool testPassed(std::string const& testName)
	{
		bool bPassed = false;
		for (size_t testNr=0;testNr<m_testInfo.size();testNr++)
		{
			if (testName == m_testInfo.at(testNr).m_testName)
			{
				bPassed = m_testInfo.at(testNr).m_testPassed;
				break;
			}
		}
		return bPassed;
	}
	DiagEngine*	getDiagEngine()
	{
		return &m_diagEngine;
	}
private:
	DiagEngine				m_diagEngine;		// engine to run the tests
	int						m_status;			// engine status (idle, running,...)
	int						m_request;			// test request to engine (diagnostic, interactive)
	eDTGui*					m_pMainWnd;
	size_t					m_nrTests;			// nr of tests to run
	size_t					m_NrRequestedTests;
	size_t					m_testType;
	size_t					m_currTestIdx;		// test index of current test in the sequence of tests
	std::vector<TestInfo>	m_testInfo;			// requested test passed/failed

	int						m_lastError;
};

#endif
