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
#ifndef _ANALYSEMWDETECTINFO_H_
#define _ANALYSEMWDETECTINFO_H_

#include <exception>
#include <sstream>

#include "analysis.h"
#include "middleware.h"
#include "AnalysisError.h"
#include "Repository.h"


//******************************************
// Middleware detection
// Tries to detect the middleware installed on the machine:
// Passed:
//		Middleware version >= 3.5
// Failed:
//		Multiple middleware versions
//		Middleware version <= 3.0
//******************************************
class AnalyseMWDetectInfo : public Analysis
{
public:
	AnalyseMWDetectInfo()
	{
		m_testName		= "middleware_info";
		m_friendlyName	= "Middleware information";
	}
	virtual ~AnalyseMWDetectInfo()
	{
	}

	virtual int run()
	{
		m_bPassed = false;
		setProgress(0);
		setStartTime();

		int			retVal		= DIAGLIB_OK;
		Report_TYPE reportType	= REPORT_TYPE_RESULT;
		wchar_t		sepa		= L'~';

		reportPrintHeader2(reportType, L"Middleware version info", sepa);

		try
		{

			//------------------------------------------
			// write to the report what we're doing
			//------------------------------------------
			resultToReport(reportType,L"[Info ] Detecting installed Middleware version...");

			//------------------------------------------
			// Step 1: detect the installed MW
			//------------------------------------------
			//------------------------------------------
			// Get a list of the middleware versions installed.
			//------------------------------------------

			MW_LIST middlewareList;
			retVal = mwGetList(&middlewareList);
			if (retVal != DIAGLIB_OK)
			{
				processParamsToStop();
				commentToReport(reportType,L"[Error] Error generating Middleware list");
				resultToReport(reportType,m_bPassed);
				REP_CONTRIBUTE(L"error",L"generating_middleware_list");
				return retVal;
			}

			{
				std::wstring msgType = L"[Info ] ";
				if (middlewareList.size()>1)
				{
					msgType = L"[Warn ] ";
				}
				std::wstringstream text;
				text << msgType << L"Nr of installed Middlewares found: " << middlewareList.size();
				resultToReport(reportType,text);
				REP_CONTRIBUTE(L"count",L"%ld",middlewareList.size());
			}

			switch(middlewareList.size())
			{
			case 0:
				processParamsToStop();
				resultToReport(reportType,m_bPassed);
				retVal = ANALYSE_NO_MW_FOUND;
				return retVal;
				break;
			case 1:
				{
					MW_ID mwInfo = *middlewareList.begin();
					MW_INFO info;
					int retVal = mwGetInfo (mwInfo, &info);
					if (retVal != DIAGLIB_OK)
					{
						processParamsToStop();
						resultToReport(reportType,L"[Error] Retrieve Middleware info failed");
						resultToReport(reportType,m_bPassed);
						REP_CONTRIBUTE(L"error",L"retrieving_middleware_info");
						return retVal;
					}

					if (MW_VERSION_30>=info.Version)
					{
						processParamsToStop();
						std::wstringstream text;
						text << L"[Warn ] Wrong Middleware version found: " << info.LabelVersion;
						resultToReport(reportType,text);
						resultToReport(reportType,m_bPassed);
						retVal = ANALYSE_MW_VERSION_NOK;
						REP_CONTRIBUTE(L"error",L"wrong_middleware_version");
						return retVal;
					}
					retVal = mwReportList(reportType, middlewareList);
					processParamsToStop();

					if (retVal != DIAGLIB_OK)
					{
						std::wstringstream text;
						text << L"[Error] Middleware list could not be retrieved: ";

						switch(retVal)
						{
						case DIAGLIB_ERR_BAD_CALL:
							text << L"Bad function call ";
							REP_CONTRIBUTE(L"error",L"bad_function_call");
							break;
						default:
							text << L"Unknown error ";
							REP_CONTRIBUTE(L"error",L"unknown_error");
						    break;
						}
						resultToReport(reportType,text);
						resultToReport(reportType,m_bPassed);
						return retVal;
					}
					m_bPassed = true;
				}
				break;
			default:
				{
					processParamsToStop();

					for(MW_LIST::iterator it=middlewareList.begin();it!=middlewareList.end();it++)
					{
						MW_ID mwInfo = *it;
						MW_INFO info;
						int retVal = mwGetInfo (mwInfo, &info);
						if (retVal != DIAGLIB_OK)
						{
							return retVal;
						}

						std::wstringstream text;
						text << L"[Info ] Found Middleware version: " << info.LabelVersion;
						resultToReport(reportType,text);
						REP_CONTRIBUTE(L"version",info.LabelVersion);
					}
					retVal = mwReportList(REPORT_TYPE_MAIN, middlewareList);
					if (retVal != DIAGLIB_OK)
					{
						resultToReport(reportType,m_bPassed);
						return retVal;
					}

					retVal = ANALYSE_MULTIPLE_MW;
					resultToReport(reportType,m_bPassed);
					return retVal;
				}

				break;
			}
			//------------------------------------------
			// it seems to be working ok, return
			//------------------------------------------
			processParamsToStop();
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
		resultToReport(reportType,L"[Info ] Generating card list using installed Middleware ");

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
				text << L"library not found ";
				REP_CONTRIBUTE(L"error",L"library_not_found");
				break;
			case DIAGLIB_ERR_INTERNAL:
				text << L"Internal error ";
				REP_CONTRIBUTE(L"error",L"internal_error");
				break;
			default:
				text << L"Unknown error ";
				REP_CONTRIBUTE(L"error",L"unknown_error");
				break;
			}
		}
		else
		{
			text << L"[Info ] List of cards generated";

		}
		resultToReport(reportType,text);
		return retVal;
	}

};

#endif
