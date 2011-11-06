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
#ifndef __DIAGENGINE_H__
#define __DIAGENGINE_H__

#include <time.h>
#include <string>
#include <vector>

#include "error.h"
#include "../DiagLib/system.h"
#include "../DiagLib/report.h"
#include "CallbackData.h"

class Analysis;

class DiagEngine
{
	class TestNameInfo
	{
	public:
		TestNameInfo(std::string const& name, std::string const& friendlyName)
			: m_name(name)
			, m_friendlyName(friendlyName)
		{
		}
	public:
		std::string m_name;
		std::string m_friendlyName;
	};
public:
	typedef std::vector<std::string>		tTestNames;
	typedef std::vector<CallbackData*>		tTestCallbackList;
	enum
	{
		  ENG_OK = 0
		, ENG_NO_ADMIN = 9000
	};
public:
	DiagEngine(void);
	virtual ~DiagEngine(void);

	int					init( void );						// initialize the diagnostic engine
	tTestNames const&	getAvailableTestNames( void );		// get the list of sequential tests
	tTestNames const&	getTestFriendlyNames();
	size_t				getNrAvailableTests( void );
	size_t				getNrRequestedTests(void);

	int run( tTestNames const& requestedTests, tTestCallbackList const& callbackList );	// run the diagnostic engine

	int getReportFilename( std::wstring& reportFile,bool bAbstractOnly = false);

	int getExecutionTime(std::string const& testName);
	int getExecutionTime(size_t testNr);
	bool testPassed(std::string const& testName);

public:
	static int		m_progressPercent;
	static time_t	m_timeStamp;
	static void progressCallbackFn(int progressPercent)
	{
		m_progressPercent=progressPercent;
		m_timeStamp = time(NULL);
	}
	time_t* getHeartbeat()
	{
		return &m_timeStamp;
	}
	int getProgress()
	{
		return m_progressPercent;
	}

private:
	typedef std::vector<Analysis*>	tTestList;

private:
	size_t checkTestRequested(std::string testName, tTestNames const& requestedTests);
	int finalizeReport(void);

	tTestNames m_availableTestNames;	// list of available tests
	tTestNames m_availableTestFriendlyNames; // list of friendly names of available tests
	tTestList  m_availableTests;		// list of the available test objects
	tTestList  m_executeTests;			// list of tests to be executed
	tTestCallbackList m_executeCallbackList;

	bool m_reportFinalizeOk;
};


#endif //__DIAGENGINE_H__
