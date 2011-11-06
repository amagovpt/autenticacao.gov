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
#include <time.h>
#include <vector>
#include <string>
#include "user.h"
#include "report.h"
#include "diagengine.h"
#include "AnalyseSystemInfo.h"
#include "AnalyseDeviceInfo.h"
#include "AnalyseSoftwareInfo.h"
#include "AnalyseServiceInfo.h"
#include "AnalyseProcessInfo.h"
#include "AnalyseReaderDetectInfo.h"
#include "AnalyseCardDetectInfo.h"
#include "AnalyseMWFileInfo.h"
#include "AnalyseMWDetectInfo.h"
#include "AnalyseMWAccessInfo.h"
#include "AnalyseMWReaderlistInfo.h"
#include "AnalyseMWCardlistInfo.h"
#include "AnalysePCSCTimingInfo.h"
#include "AnalysePCSCDetectInfo.h"
#include "AnalyseSignPKCS11Info.h"
#ifdef WIN32
#include "AnalyseSignCSPInfo.h"
#endif
#include "AnalysePCSCReaderlistInfo.h"
#include "AnalysePCSCCardlistInfo.h"
#include "AnalysePCSCCardInfo.h"
#include "AnalyseHardware.h"
#include "AnalyseModuleInfo.h"
#ifdef __APPLE__
#include "AnalyseSignTokendInfo.h"
#include "AnalyseModuleInfo.h"
#endif

#include "AnalyseMeta.h"

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PRIVATE FUNCTIONS DECLARATION ////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PUBLIC FUNCTIONS /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
DiagEngine::DiagEngine()
{
	reportInit();
	m_reportFinalizeOk = false;

	//---------------------------------
	// setup a list with all tests. The sequence of the list dictates the sequence of
	// the tests in case all tests are requested at the same time.	
	//---------------------------------
	
	Analysis* test = NULL;
	
	test = new AnalyseSystemInfo;
	m_availableTests.insert(m_availableTests.end(), test );
	
	test = new AnalyseDeviceInfo;
	m_availableTests.insert(m_availableTests.end(),test  );
	
	test = new AnalyseSoftwareInfo;
	m_availableTests.insert(m_availableTests.end(),test  );	
	
#ifdef WIN32
	
	test = new AnalyseServiceInfo;								// services only on Windows
	m_availableTests.insert(m_availableTests.end(),test );

	test =new AnalyseProcessInfo;
	m_availableTests.insert(m_availableTests.end(),test );
	
	test = new AnalyseModuleInfo;
	m_availableTests.insert(m_availableTests.end(),test );
	
	test = new AnalyseHardware;
	m_availableTests.insert(m_availableTests.end(),test );		// Hardware analysis only for Windows (so far)

	test = new AnalyseMWFileInfo;
	m_availableTests.insert(m_availableTests.end(),test );

	test = new AnalyseReaderDetectInfo;
	m_availableTests.insert(m_availableTests.end(),test );

	test = new AnalysePCSCTimingInfo;
	m_availableTests.insert(m_availableTests.end(),test );

	test = new AnalyseCardDetectInfo;
	m_availableTests.insert(m_availableTests.end(),test );

	test = new AnalyseMWCardlistInfo;
	m_availableTests.insert(m_availableTests.end(),test );

	test = new AnalyseMWDetectInfo;
	m_availableTests.insert(m_availableTests.end(),test );

	test = new AnalyseMWReaderlistInfo;
	m_availableTests.insert(m_availableTests.end(),test );

	test = new AnalyseMWAccessInfo;
	m_availableTests.insert(m_availableTests.end(),test );

	test = new AnalysePCSCDetectInfo;
	m_availableTests.insert(m_availableTests.end(),test );

	test = new AnalysePCSCReaderlistInfo;						// reader list only on Windows
	m_availableTests.insert(m_availableTests.end(),test );

	test = new AnalysePCSCCardlistInfo;
	m_availableTests.insert(m_availableTests.end(),test );

	test = new AnalysePCSCCardInfo;
	m_availableTests.insert(m_availableTests.end(),test );

	test = new AnalyseSignPKCS11Info;
	m_availableTests.insert(m_availableTests.end(),test );

	test = new AnalyseSignCSPInfo;								// signature test via CSP, on Windows
	m_availableTests.insert(m_availableTests.end(),test );

#else // OSX
	
	test =new AnalyseProcessInfo;
	m_availableTests.insert(m_availableTests.end(),test );
	
	test = new AnalyseModuleInfo;
	m_availableTests.insert(m_availableTests.end(),test );

	test = new AnalyseMWFileInfo;
	m_availableTests.insert(m_availableTests.end(),test );
	
	test = new AnalyseReaderDetectInfo;
	m_availableTests.insert(m_availableTests.end(),test );
	
	test = new AnalysePCSCTimingInfo;
	m_availableTests.insert(m_availableTests.end(),test );
	
	test = new AnalyseCardDetectInfo;
	m_availableTests.insert(m_availableTests.end(),test );
	
	test = new AnalyseMWCardlistInfo;
	m_availableTests.insert(m_availableTests.end(),test );
	
	test = new AnalyseMWDetectInfo;
	m_availableTests.insert(m_availableTests.end(),test );
	
	test = new AnalyseMWReaderlistInfo;
	m_availableTests.insert(m_availableTests.end(),test );
	
	test = new AnalyseMWAccessInfo;
	m_availableTests.insert(m_availableTests.end(),test );
	
	test = new AnalysePCSCDetectInfo;
	m_availableTests.insert(m_availableTests.end(),test );
	
	test = new AnalysePCSCReaderlistInfo;
	m_availableTests.insert(m_availableTests.end(),test );
	
	test = new AnalysePCSCCardlistInfo;
	m_availableTests.insert(m_availableTests.end(),test );
	
	test = new AnalysePCSCCardInfo;
	m_availableTests.insert(m_availableTests.end(),test );
	
	test = new AnalyseSignPKCS11Info;
	m_availableTests.insert(m_availableTests.end(),test );
	
	test = new AnalyseSignTokendInfo;							// signature test on OSX is via tokend
	m_availableTests.insert(m_availableTests.end(),test );	
	
#endif 
	
	test = new AnalyseMeta;
	m_availableTests.insert(m_availableTests.end(),test );		// analyse results from other modules together

	for (size_t idx=0;idx<m_availableTests.size();idx++)
	{
		m_availableTestNames.insert(m_availableTestNames.end(),m_availableTests.at(idx)->getTestName());
		m_availableTestFriendlyNames.insert(m_availableTestFriendlyNames.end(),m_availableTests.at(idx)->getTestFriendlyName());
	}
}
DiagEngine::~DiagEngine(void)
{
	finalizeReport(); //Should be done to avoid to let temporary files behind

	//---------------------------------
	// remove all the test objects available
	// do NOT remove the objects in the 'm_executeTests' because these
	// are the same ptrs as we will be deleting
	//---------------------------------
	while (m_availableTests.size()>0)
	{
		Analysis* test=m_availableTests.at(m_availableTests.size()-1);
		delete test;
		m_availableTests.erase(m_availableTests.end()-1);
	}
	m_availableTests.clear();
	m_availableTestNames.clear();
	m_availableTestFriendlyNames.clear();
}

int DiagEngine::init(void)
{
	int retVal = ENG_OK;
	m_executeTests.clear();

	//---------------------------------
	// we must be Admin to be able to run this tool
	//---------------------------------
	bool	bAdmin = false;
	User_ID userID = CURRENT_USER_ID;
	if ( DIAGLIB_OK != (retVal = userIsAdministrator(userID,&bAdmin)) )
	{
		return retVal;
	}

	if (!bAdmin)
	{
		return ENG_NO_ADMIN;
	}

	return retVal;
}

//------------------------------------------
// return the names of all available tests
//------------------------------------------
DiagEngine::tTestNames const& DiagEngine::getAvailableTestNames()
{
	return m_availableTestNames;
}

DiagEngine::tTestNames const& DiagEngine::getTestFriendlyNames()
{
	return m_availableTestFriendlyNames;
}

//------------------------------------------
// return the number of available tests
//------------------------------------------
size_t DiagEngine::getNrAvailableTests()
{
	return m_availableTestNames.size();
}

//------------------------------------------
// return the number of requested tests
//------------------------------------------
size_t DiagEngine::getNrRequestedTests()
{
	return m_executeTests.size();
}

size_t DiagEngine::checkTestRequested(std::string testName, tTestNames const& requestedTests)
{
	bool bFound = false;
	size_t idx = -1;
	for (tTestNames::const_iterator it = requestedTests.begin()
		;it != requestedTests.end()
		;it++,idx++
		)
	{
		if (*it==testName)
		{
			idx++;
			bFound = true;
			break;
		}
	}
	if (bFound)
	{
		return idx;
	}
	return -1;
}

int DiagEngine::run(tTestNames const& requestedTests,tTestCallbackList const& callbackList)
{
	m_executeTests.clear();

	//--------------------------------------------------
	// loop over the available tests and check if the test is requested.
	// if the test is requested, then add at the end of the list of tests
	// to execute
	// In this way, we keep the test sequence as it is preset in the available tests
	//--------------------------------------------------
	for (size_t testIdx=0; testIdx<m_availableTestNames.size(); testIdx++)
	{
		size_t testNr = checkTestRequested(m_availableTestNames.at(testIdx),requestedTests);
		if (-1!=testNr)
		{
			m_executeTests.insert(m_executeTests.end(),m_availableTests.at(testIdx));
			m_executeCallbackList.insert(m_executeCallbackList.end(),NULL);
		}
	}

	int result = DIAGLIB_OK;

	//--------------------------------------------------
	// loop over the requested tests and execute them
	//--------------------------------------------------
	size_t idx=0;
	for (tTestList::const_iterator it = m_executeTests.begin()
		;it!=m_executeTests.end()
		;it++
		)
	{
		(*it)->setCallback(callbackList.at(idx));
		m_progressPercent = 0;

		REP_PREFIX((*it)->getTestName());	// set Repository root Prefix

		result = (*it)->run();				// run the test

		REP_AVAILABLE((*it)->testPassed());	// contribute "available" if test passed
		REP_UNPREFIX();						// remove Repository root Prefix
		idx++;
	}

	return result;
}

int DiagEngine::getExecutionTime(std::string const& testName)
{
	for (size_t idx=0;idx<m_executeTests.size();idx++)
	{
		if ( testName == m_executeTests.at(idx)->getTestName())
		{
			return m_executeTests.at(idx)->getExecutionTime();
		}
	}
	return 0;
}
int DiagEngine::getExecutionTime(size_t testNr)
{
	return m_availableTests.at(testNr)->getExecutionTime();
}
bool DiagEngine::testPassed(std::string const& testName)
{
	for (size_t idx=0;idx<m_executeTests.size();idx++)
	{
		if ( testName == m_executeTests.at(idx)->getTestName())
		{
			return m_executeTests.at(idx)->testPassed();
		}
	}
	return false;
}

int DiagEngine::finalizeReport(void)
{
	int retVal=DIAGLIB_OK;

	if(!m_reportFinalizeOk)
		retVal= reportFinalize(NULL,NULL);

	m_reportFinalizeOk = true;		//Finalize only once but at least once

	return retVal;
}

int DiagEngine::getReportFilename( std::wstring& reportFile,bool bAbstractOnly)
{
	finalizeReport();
	const wchar_t* file=NULL;
	int retVal = reportGetFileName(&file,bAbstractOnly);
	if (DIAGLIB_OK!=retVal)
	{
		return retVal;
	}
	reportFile.assign(file);
	return retVal;
}

int		DiagEngine::m_progressPercent = 0;
time_t	DiagEngine::m_timeStamp = time(NULL);

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PRIVATE FUNCTIONS ////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

