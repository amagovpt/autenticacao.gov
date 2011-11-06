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
#ifndef _ANALYSEPCSCREADERLISTINFO_H_
#define _ANALYSEPCSCREADERLISTINFO_H_

#include <exception>
#include <sstream>

#include "analysis.h"
#include "pcsc.h"
#include "AnalysisError.h"
#include "Repository.h"


//******************************************
// PCSC readerlist verification
//		Verify the list of readers connected using PCSC
//
// Pass:
//		At least one card reader found
// Fail:
//		No card readers found
//******************************************
class AnalysePCSCReaderlistInfo : public Analysis
{
public:
	AnalysePCSCReaderlistInfo()
	{
		m_testName = "pcsc_readerlist";
		m_friendlyName = "PCSC readerlist";
	}
	virtual ~AnalysePCSCReaderlistInfo()
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
			reportPrintHeader2(reportType, L"PCSC readerlist", sepa);

			//------------------------------------------
			// write to the report what we're doing
			//------------------------------------------
			resultToReport(reportType,L"[Info ] Accessing PCSC ...");

			//------------------------------------------
			// get the list of readers we found using the pcsc
			//------------------------------------------
			Reader_LIST readerList;
			retVal = pcscGetReaderList(&readerList);
			if (DIAGLIB_OK!=retVal)
			{
				std::wstringstream text;
				switch(retVal)
				{
				case DIAGLIB_ERR_BAD_CALL:
					text << L"[Error] Bad function call to pcscGetReaderList()";
					REP_CONTRIBUTE(L"error",L"bad_function_call_to_pcscgetreaderlist");
					break;
				case DIAGLIB_ERR_LIBRARY_NOT_FOUND:
					text << L"[Error] Could not load PCSC library";
					REP_CONTRIBUTE(L"error",L"could_not_load_pcsc_library");
					break;
				case DIAGLIB_ERR_INTERNAL:
					text <<  L"[Error] Internal error calling pcscGetReaderList()";
					REP_CONTRIBUTE(L"error",L"internal_error_calling_pcscgetreaderlist");
					break;
				case DIAGLIB_ERR_READER_NOT_FOUND:
					text << L"[Error] No Reader found";
					REP_CONTRIBUTE(L"error",L"no_reader_found");
					break;
				default:
					text << L"[Error] Unknown error: pcscGetReaderList()";
					REP_CONTRIBUTE(L"error",L"unknown_error_calling_pcscgetreaderlist");
					break;
				}
				resultToReport(reportType,text);
			}
			else
			{
				std::wstringstream text;
				text << L"[Info] Nr of card readers detected by PCSC: " << readerList.size();
				resultToReport(reportType,text);
				bool bPassed = true;
				REP_CONTRIBUTE(L"count",L"%ld",readerList.size());


				if (0==readerList.size())
				{
					bPassed = false;
				}
				for (size_t idx=0;idx<readerList.size();idx++)
				{
					Reader_INFO info;
					retVal = readerGetInfo(readerList.at(idx), &info);

					if (retVal!=DIAGLIB_OK)
					{
						Reader_ID reader = readerList.at(idx);
						std::wstringstream txt;
						txt << L"[Error] Error get info from reader:" << reader.Name;
						REP_CONTRIBUTE(L"error",L"getting_info_from_reader");
						resultToReport(reportType,txt);
						bPassed = false;
					}

					retVal = readerReportInfo(reportType, info);
				}
				m_bPassed = bPassed;
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
