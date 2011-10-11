/*****************************************************************************

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
#ifndef _ANALYSEMWACCESSINFO_H_
#define _ANALYSEMWACCESSINFO_H_

#include <exception>
#include <sstream>

#include "analysis.h"
#include "middleware.h"
#include "AnalysisError.h"


//******************************************
// Middleware access verification
// 
// Pass:
//
// Fail:
//
//******************************************
class AnalyseMWAccessInfo : public Analysis
{
public:
	AnalyseMWAccessInfo()
	{
		m_testName = "middleware_access";
		m_friendlyName = "Middleware card access";
	}
	virtual ~AnalyseMWAccessInfo()
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
			reportPrintHeader2(reportType, L"Middleware card access", sepa);

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

			{
				std::wstringstream text;
				text << L"[Info ] Nr cards inserted in cardreaders: " << cardList.size();
				resultToReport(reportType,text);
			}

			//------------------------------------------
			// card list can be generated, check if a card is inserted (size>0)
			//------------------------------------------
			if (0==cardList.size())
			{
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
						resultToReport(reportType,L"[Error] Bad function call to mwGetReaderList()");
						REP_CONTRIBUTE(L"error",L"bad_function_call_to_mwgetreaderlist");
						break;
					case DIAGLIB_ERR_LIBRARY_NOT_FOUND:
						resultToReport(reportType,L"[Error] Could not load Middleware");
						REP_CONTRIBUTE(L"error",L"could_not_load_middleware");
						break;
					case DIAGLIB_ERR_INTERNAL:
						resultToReport(reportType,L"[Error] Internal error calling mwGetReaderList()");
						REP_CONTRIBUTE(L"error",L"internal_error_calling_mwgetreaderlist");
					    break;
					default:
						resultToReport(reportType,L"[Error] Unknown error: mwGetReaderList()");
						REP_CONTRIBUTE(L"error",L"unknown_error_calling_mwgetreaderlist");
					    break;
					}
				}
				else
				{
					std::wstringstream text;
					text << L"[Info] Nr of card readers detected by Middleware: " << readerList.size();
					resultToReport(reportType,text);
					REP_CONTRIBUTE(L"readercount",L"%ld",readerList.size());
					for (size_t idx=0;idx<readerList.size();idx++)
					{
						Reader_INFO info;
						retVal = readerGetInfo(readerList.at(idx), &info);
						retVal = readerReportInfo(reportType, info);
					}
				}
				processParamsToStop();
				resultToReport(reportType,m_bPassed);
				return retVal;
			}

			//------------------------------------------
			// Loop over all the card readers that have a card inserted
			//------------------------------------------

			REP_CONTRIBUTE(L"cardcount",L"%ld",cardList.size());
			for (size_t cardIdx=0; cardIdx<cardList.size(); cardIdx++)
			{
				std::wstringstream text;
				text.str(L"");
				Card_INFO info;
				retVal = cardGetInfo(cardList.at(cardIdx), &info);
				switch(retVal)
				{
				case DIAGLIB_ERR_INTERNAL:
				//------------------------------------------------
				// Card_INFO* == NULL
				//------------------------------------------------
				case DIAGLIB_ERR_BAD_CALL:
				//------------------------------------------------
				// cardinfo can not be read from this list (== software error)
				//------------------------------------------------
				case DIAGLIB_ERR_NOT_AVAILABLE:
					text << L"[Error] Internal error retrieving info from card";
					REP_CONTRIBUTE(L"error",L"internal_error_retrieving_info_from_card");
					break;
				//------------------------------------------------
				// user clicked 'NO" when asking to grant access to cards
				//------------------------------------------------
				case DIAGLIB_ERR_NOT_ALLOWED_BY_USER:
					text << L"[Warn ] Access to cards refused by user";
					REP_CONTRIBUTE(L"error",L"access_to_card_refused_by_user");
					break;
				//------------------------------------------------
				// eidlib could not be loaded
				//------------------------------------------------
				case DIAGLIB_ERR_LIBRARY_NOT_FOUND:
					text << L"[Error] Middleware library could not be loaded.";
					REP_CONTRIBUTE(L"error",L"middleware_library_could_not_be_loaded");
					resultToReport(reportType,text);
					processParamsToStop();
					return retVal;
					break;
				//------------------------------------------------
				// no card in reader
				//------------------------------------------------
				case DIAGLIB_ERR_CARD_NOT_FOUND:
					text << L"[Error] Card could not be found.";
					REP_CONTRIBUTE(L"error",L"card_could_not_be_found");
					break;
				//------------------------------------------------
				// card type not recognized
				//------------------------------------------------
				case DIAGLIB_ERR_CARD_BAD_TYPE:
					text << L"[Warn ] Card not recognized.";
					REP_CONTRIBUTE(L"error",L"card_not_recognized");
					break;
				//------------------------------------------------
				// internal error
				//------------------------------------------------
				default:	// DIAGLIB_OK
					break;
				}
				resultToReport(reportType,text);
				retVal = cardReportInfo(reportType, info);
				cardContributeInfo(info);
				if (retVal != DIAGLIB_OK)
				{
					setEndTime();
					resultToReport(reportType,m_bPassed);
					return retVal;
				}
			}
			
			//------------------------------------------
			// it seems to be working ok, return
			//------------------------------------------
			m_bPassed = true;
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
				REP_CONTRIBUTE(L"error",L"bad_function_call_retrieving_card_list");
				break;
			case DIAGLIB_ERR_LIBRARY_NOT_FOUND:
				text << L"Library not found ";
				REP_CONTRIBUTE(L"error",L"library_not_found_retrieving_card_list");
				break;
			case DIAGLIB_ERR_INTERNAL:
				text << L"Internal error ";
				REP_CONTRIBUTE(L"error",L"internal_error_retrieving_card_list");
				break;
			default:
				text << L"Unknown error ";
				REP_CONTRIBUTE(L"error",L"unknown_error_retrieving_card_list");
				break;
			}
		}
		resultToReport(reportType,text);
		return retVal;
	}

};

#endif
