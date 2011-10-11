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
#ifndef __ANALYSIS_H__
#define __ANALYSIS_H__

#include <sstream>
#include <time.h>
#include "CallbackData.h"
#include "error.h"
#include "report.h"
#include "progress.h"

class ExcAnalyse : public std::exception
{
public:
	ExcAnalyse(int err, std::wstring const& msg)
		: m_err(err)
		, m_msg(msg)
	{
	}
	virtual ~ExcAnalyse() throw()
	{
	}
	std::wstring const& What() const throw()
	{
		return m_msg;
	}
	int getErr()
	{
		return m_err;
	}
private:
	int m_err;
	std::wstring m_msg;
};

class ExcReport : public ExcAnalyse
{
public:
	ExcReport(int err, std::wstring const& msg)
		: ExcAnalyse(err, msg)
	{

	}
};

//----------------------------------------------------
// analyze base class
// each diagnostic class should derive from this class
// and implement the function 'run'
//----------------------------------------------------
class Analysis 
{
public:
	Analysis();					// ctor
	virtual ~Analysis();		// dtor
	virtual int run()
	{
		m_bPassed = false;
		return -1;
	};
	int setCallback( CallbackData* callbackData )
	{
		m_callbackData = callbackData;
		return 0;
	};
	void setProgress(size_t progress)
	{
		m_progress = progress;
		doCallback();
	}
	void doCallback()
	{
		if (m_callbackData)
		{
			m_callbackData->m_callback(m_callbackData->m_userData,m_testName,m_progress);
		}
	}
	std::string getTestName( void )
	{
		return m_testName;
	}
	std::string getTestFriendlyName( void )
	{
		return m_friendlyName;
	}
	void setStartTime()
	{
		m_startTime = clock();
	}
	void setEndTime()
	{
		m_endTime = clock();
	}

	int getExecutionTime( void )
	{
		return m_endTime - m_startTime;
	}
	bool testPassed()
	{
		return m_bPassed;
	}
	void resultToReport(Report_TYPE reportType,bool bPassed)
	{
		std::wstring str(L"[Error] Failed");
		if (bPassed)
		{
			str = L"[Info ] Passed";
		}
		resultToReport(reportType,str.c_str());
	};
	void resultToReport(Report_TYPE reportType,const wchar_t* text)
	{
		std::wstringstream streamText;
		streamText << text;
		resultToReport(reportType,streamText);
	}
	void resultToReport(Report_TYPE reportType,std::wstringstream const& text)
	{
		int retVal = reportPrintResult(reportType,text.str().c_str());
		if (retVal != DIAGLIB_OK)
		{
			ExcReport exc(retVal,text.str());
			throw exc;
		}
	}
	void commentToReport(Report_TYPE reportType,std::wstring const& text)
	{
		std::wstringstream streamText;
		streamText << text;
		commentToReport(reportType,streamText);
	}
	void commentToReport(Report_TYPE reportType,std::wstringstream const& text)
	{
		int retVal = reportPrintComment(reportType,text.str().c_str());
		if (retVal != DIAGLIB_OK)
		{
			ExcReport exc(retVal,text.str());
			throw exc;
		}
	}
	void processParamsToStop()
	{
		setEndTime();
		setProgress(100);
	}
	
	static std::string trim_right(const std::string &source , const std::string& t = " ")
	{
		std::string str = source;
		return str.erase( str.find_last_not_of(t) + 1);
	}
	
	static std::string trim_left( const std::string& source, const std::string& t = " ")
	{
		std::string str = source;
		return str.erase(0 , source.find_first_not_of(t) );
	}
	
	static std::string trim(const std::string& source, const std::string& t = " ")
	{
		std::string str = source;
		return trim_left( trim_right( str , t) , t );
	} 
	
// 	static void progressCallbackFn(int progressPercent)
// 	{
// 		m_progressPercent=progressPercent;
// 	
	

private:
	CallbackData* m_callbackData;

protected:
	std::string	  m_testName;		// name of the analysis engine
	std::string   m_friendlyName;	// friendly name
	bool		  m_bPassed;		// test passed/failed

//	static int    m_progressPercent;

private:
	size_t		  m_progress;		// progress (percentage) of the analysis
	clock_t		  m_startTime;
	clock_t		  m_endTime;
};


#endif //__DIAGENGINE_ANALYSIS_H__
