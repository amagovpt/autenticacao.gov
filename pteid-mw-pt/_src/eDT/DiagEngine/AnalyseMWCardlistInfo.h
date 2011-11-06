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
#ifndef _ANALYSEMWCARDLISTINFO_H_
#define _ANALYSEMWCARDLISTINFO_H_

#include <exception>
#include <sstream>

#include "analysis.h"
#include "middleware.h"
#include "AnalysisError.h"


//******************************************
// Middleware card list verification
//		Try to generate a list of inserted cards. When cards are inserted
//		the corresponding card reader will be reported
// Pass:
//		card list size > 0
// Fail:
//		card list size = 0
//******************************************
class AnalyseMWCardlistInfo : public Analysis
{
public:
	AnalyseMWCardlistInfo()
	{
		m_testName = "middleware_cardlist";
		m_friendlyName = "Middleware card list";
	}
	virtual ~AnalyseMWCardlistInfo()
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
			reportPrintHeader2(reportType, L"Middleware card list", sepa);

			//------------------------------------------
			// write to the report what we're doing
			//------------------------------------------
			resultToReport(reportType,L"[Info ] Accessing Middleware ...");

			//------------------------------------------
			// Load the installed MW and read the card(s)
			// If the MW fails then stop
			//------------------------------------------
			Card_LIST cardList;
			retVal = getMWCardList(reportType, &cardList);

			if (DIAGLIB_OK!=retVal)
			{
				processParamsToStop();
				resultToReport(reportType,m_bPassed);
				return retVal;
			}

			//------------------------------------------
			// card list can be generated, check if a card is inserted (size>0)
			//------------------------------------------

			REP_CONTRIBUTE(L"count",L"%ld",cardList.size());

			if (0==cardList.size())
			{
				resultToReport(reportType,L"[Error] No Belgian eID card detected by Middleware");
				processParamsToStop();
				resultToReport(reportType,m_bPassed);
				REP_CONTRIBUTE(L"error",L"no_eid_detected");
				return retVal;
			}
			else
			{
				std::wstringstream text;
				text << L"[Info ] Nr cards inserted in cardreaders: " << cardList.size();
				resultToReport(reportType,text);

				bool bPassed = true;
				for (size_t cardIdx=0;cardIdx<cardList.size();cardIdx++)
				{

					Reader_ID reader = cardList.at(cardIdx).Reader;
					Reader_INFO info;
					retVal = readerGetInfo(reader, &info);

					if (DIAGLIB_OK!=retVal)
					{
						resultToReport(reportType,L"[Error] Error getting reader information");
						std::wstringstream txt;
						txt << L"[Error] Reader:" << reader.Name;
						resultToReport(reportType,txt);
						REP_CONTRIBUTE(L"error",L"getting_reader_information");
						bPassed = false;
					}

					retVal = readerReportInfo(reportType, info);

					if (DIAGLIB_OK!=retVal)
					{
						resultToReport(reportType,L"[Error] Error reporting info of reader");
						std::wstringstream txt;
						txt << L"[Error] Reader:" << reader.Name;
						resultToReport(reportType,txt);
						REP_CONTRIBUTE(L"error",L"reporting_reader_info");
						bPassed = false;
					}
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
private:
	int getMWCardList(Report_TYPE reportType, Card_LIST* cardList)
	{
		resultToReport(reportType,L"[Info ] Generating card list using installed Middleware");

		int retVal =  cardGetListFromMW(cardList);
		std::wstringstream text;

		if (retVal != DIAGLIB_OK)
		{
			text << L"[Error] Card list could not be retrieved: ";
			switch(retVal)
			{
			case DIAGLIB_ERR_BAD_CALL:
				text << L"Bad function call ";
				REP_CONTRIBUTE(L"error",L"bad_function_call");
				break;
			case DIAGLIB_ERR_LIBRARY_NOT_FOUND:
				text << L"Library not found ";
				REP_CONTRIBUTE(L"error",L"library_not_found");
				break;
			case DIAGLIB_ERR_INTERNAL:
				text << L"Internal error ";
				REP_CONTRIBUTE(L"error",L"internal_error");
				break;
			case DIAGLIB_ERR_READER_NOT_FOUND:
				text << L"No Reader found ";
				REP_CONTRIBUTE(L"error",L"no_reader_found");
				break;
			default:
				text << L"Unknown error ";
				REP_CONTRIBUTE(L"error",L"unknown_error");
				break;
			}
		}
		resultToReport(reportType,text);
		return retVal;
	}

};

#endif
