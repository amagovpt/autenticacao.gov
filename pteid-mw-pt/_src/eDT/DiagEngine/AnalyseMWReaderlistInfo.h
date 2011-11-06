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
#ifndef _ANALYSEMWREADERLISTINFO_H_
#define _ANALYSEMWREADERLISTINFO_H_

#include <exception>
#include <sstream>

#include "analysis.h"
#include "middleware.h"
#include "AnalysisError.h"
#include "Repository.h"


//******************************************
// Middleware readerlist verification
//		Verify the list of readers connected using the installed MW
//
// Pass:
//		At least one card reader found
// Fail:
//		No card readers found
//******************************************
class AnalyseMWReaderlistInfo : public Analysis
{
public:
	AnalyseMWReaderlistInfo()
	{
		m_testName = "middleware_readerlist";
		m_friendlyName = "Middleware readerlist";
	}
	virtual ~AnalyseMWReaderlistInfo()
	{
	}

	virtual int run()
	{
		m_bPassed = false;
		setProgress(0);
		setStartTime();
		int retVal = DIAGLIB_OK;
		Report_TYPE reportType = REPORT_TYPE_RESULT;

		try
		{
			wchar_t		sepa		= L'~';
			reportPrintHeader2(reportType, L"Middleware readerlist", sepa);

			//------------------------------------------
			// write to the report what we're doing
			//------------------------------------------
			resultToReport(reportType,L"[Info ] Accessing Middleware ...");

			//------------------------------------------
			// get the list of readers we found using the middleware
			//------------------------------------------
			Reader_LIST readerList;
			retVal = mwGetReaderList(&readerList);
			if (DIAGLIB_OK!=retVal)
			{
				switch(retVal)
				{
				case DIAGLIB_ERR_BAD_CALL:
					std::wstring(L"[Error] Bad function call to mwGetReaderList()");
					REP_CONTRIBUTE(L"error",L"bad_function_call_to_mwreadergetlist");
					break;
				case DIAGLIB_ERR_LIBRARY_NOT_FOUND:
					std::wstring(L"[Error] Could not load Middleware");
					REP_CONTRIBUTE(L"error",L"could_not_load_middleware");
					break;
				case DIAGLIB_ERR_INTERNAL:
					std::wstring(L"[Error] Internal error calling mwGetReaderList()");
					REP_CONTRIBUTE(L"error",L"internal_error_calling_mwgetreaderlist");
					break;
				default:
					std::wstring(L"[Error] Unknown error: mwGetReaderList()");
					REP_CONTRIBUTE(L"error",L"unknown_error_calling_mwgetreaderlist");
					break;
				}
			}
			else
			{
				std::wstringstream text;
				text << L"[Info] Nr of card readers detected by Middleware: " << readerList.size();
				resultToReport(reportType,text);
				REP_CONTRIBUTE(L"count",L"%ld",readerList.size());

				if (0==readerList.size())
				{
					m_bPassed = false;
				} 
				else
				{
					for (size_t idx=0;idx<readerList.size();idx++)
					{
						Reader_INFO info;
						retVal = readerGetInfo(readerList.at(idx), &info);

						if (DIAGLIB_OK!=retVal)
						{
							m_bPassed = false;

							resultToReport(reportType,L"[Error] Error retrieving info from reader");
							Reader_ID reader = readerList.at(idx);
							std::wstringstream txt;
							txt << L"[Error] Reader:" << reader.Name;
							resultToReport(reportType,txt);
							processParamsToStop();
							resultToReport(reportType,m_bPassed);
							REP_CONTRIBUTE(L"error",L"retrieving_info_from_reader");
							return retVal;
						}

						retVal = readerReportInfo(reportType, info);

						if (DIAGLIB_OK!=retVal)
						{
							m_bPassed = false;

							resultToReport(reportType,L"[Error] Error in readerReportInfo()");
							Reader_ID reader = readerList.at(idx);
							std::wstringstream txt;
							txt << L"[Error] Reader:" << reader.Name;
							resultToReport(reportType,txt);
							processParamsToStop();
							resultToReport(reportType,m_bPassed);
							REP_CONTRIBUTE(L"error",L"readerreportinfo_failed");
							return retVal;
						}
					}
					m_bPassed = true;
				}
			}
			//------------------------------------------
			// it seems to be working ok, return
			//------------------------------------------
			processParamsToStop();
			resultToReport(reportType,m_bPassed);
			return retVal;
		}
		//------------------------------------------
		// exception from writing to the report
		//------------------------------------------
		catch (ExcReport& exc) 
		{
			processParamsToStop();
			retVal = exc.getErr();
		}
		resultToReport(reportType,m_bPassed);
		return retVal;
	}
};

#endif
