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
#ifndef _ANALYSEPCSCTIMINGINFO_H_
#define _ANALYSEPCSCTIMINGINFO_H_

#include <exception>

#include "analysis.h"
#include "middleware.h"
#include "AnalysisError.h"
#include "pcsc.h"
#include "Repository.h"

//******************************************
// Perform a timing check if PCSC is running
// it will perform 2 timing checks:
// - connect delay
// - transmit delay
// 
// For each a number of attempts is made. As soon as the timing is such
// that a card can be accessed, the time is reported
// 
// Pass:
//		timing is within limits
// Fail:
//		timing is out of limits
//******************************************
class AnalysePCSCTimingInfo : public Analysis
{
public:
	AnalysePCSCTimingInfo()
	{
		m_testName = "pcsc_timing";
		m_friendlyName = "PCSC Timing";
	}
	virtual ~AnalysePCSCTimingInfo()
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
			//------------------------------------------
			// write to the report what we're doing
			//------------------------------------------
			wchar_t		sepa		= L'~';
			reportPrintHeader2(reportType, L"PCSC Timing verification", sepa);

			//------------------------------------------
			// Step 1: detect the PCSC daemon/service is available
			//------------------------------------------
			resultToReport(reportType,L"[Info ] Checking PCSC available");
			bool bPCSCAvailable = false;
			retVal = pcscIsAvailable(&bPCSCAvailable);

			if(DIAGLIB_OK!=retVal)
			{
				std::wstring msg;
				switch(retVal)
				{
				//------------------------------------------
				// incorrect function call, should not happen
				//------------------------------------------
				case DIAGLIB_ERR_BAD_CALL:
					msg = L"[Error] Internal error calling pcscIsAvailable()";
					REP_CONTRIBUTE(L"error",L"internal_error_calling_pcscisavailable");
					break;
				//------------------------------------------
				// the winscard dll could not be loaded
				//------------------------------------------
				case DIAGLIB_ERR_LIBRARY_NOT_FOUND:
					msg = L"[Error] PCSC service/daemon not found";
					REP_CONTRIBUTE(L"error",L"pcsc_service_not_found");
					break;
				default:
					msg = L"[Error] Unknown error calling pcscIsAvailable()";
					REP_CONTRIBUTE(L"error",L"unknown_error_calling_pcscisavailable");
					break;
				}
				processParamsToStop();
				resultToReport(reportType,msg.c_str());
				resultToReport(reportType,m_bPassed);
				return retVal;
			}

			if (false == bPCSCAvailable)
			{
				resultToReport(reportType,L"[Error] Could not establish context to PCSC");
				REP_CONTRIBUTE(L"error",L"could_not_establish_context_to_pcsc");
				processParamsToStop();
				resultToReport(reportType,m_bPassed);
				return retVal;
			}
			//------------------------------------------
			// Step 2: get the cardlist using PCSC
			//------------------------------------------
			Card_LIST cardList;
			retVal = getPCSCCardList(reportType,&cardList);

			if (DIAGLIB_OK!=retVal)
			{
				processParamsToStop();
				resultToReport(reportType,m_bPassed);
				return retVal;
			}

			//------------------------------------------
			// card list can be generated, check if a card is inserted (size>0)
			//------------------------------------------
			if (0==cardList.size())
			{
				processParamsToStop();
				resultToReport(reportType,L"[Error] No card(s) inserted in cardreader(s)");
				REP_CONTRIBUTE(L"error",L"no_cards_inserted");
				resultToReport(reportType,m_bPassed);
				return retVal;
			}

			std::wstringstream text;
			text << L"[Info ] Nr of cardreaders with card inserted: " << cardList.size();
			resultToReport(reportType,text);

			REP_CONTRIBUTE(L"count",L"%ld",cardList.size());

			resultToReport(reportType,L"[Info ] Checking timings using PCSC");

			int ConnectDelayOptimum=0;
			int TransmitDelayOptimum=0;
			//------------------------------------------
			// Loop over all the card readers that have a card inserted
			//------------------------------------------
			bool bCheckTimings = true;
			for (size_t cardIdx=0; cardIdx<cardList.size(); cardIdx++)
			{
				{
					int delayMin = 200;
					int delayMax = 400;
					int delayStep = 50;
					int NbrOfSuccess = 100;
					int delayOptimum = 0;
					retVal = pcscOptimizeConnect(cardList.at(cardIdx), NbrOfSuccess, delayMin,	delayMax, delayStep, &delayOptimum); //--> give optimum delay if > 200

					if (DIAGLIB_OK!=retVal)
					{
						resultToReport(reportType,L"[Error] Optimize connect delay failed");
						REP_CONTRIBUTE(L"error",L"optimize_connect_delay_failed");
						bCheckTimings = false;
						break;
					}

					if (delayOptimum>delayMin)
					{
						std::wstringstream text;
						text << "[Info ] Optimum connect delay:  " << delayOptimum;
						resultToReport(reportType,text);
						retVal = pcscSetConnectTiming(delayOptimum);
						REP_CONTRIBUTE(L"optimum_connect_delay",L"%d",delayOptimum);
					}

				}
				{
					int delayMin = 3;
					int delayMax = 10;
					int delayStep = 2;
					int NbrOfSuccess = 5;
					int delayOptimum = 0;

					retVal =  pcscOptimizeTransmit(cardList.at(cardIdx), NbrOfSuccess, delayMin, delayMax, delayStep, &delayOptimum);

					if (DIAGLIB_OK!=retVal)
					{
						resultToReport(reportType,L"[Error] Optimize transmit delay failed");
						REP_CONTRIBUTE(L"error",L"optimize_transmit_delay_failed");
						bCheckTimings = false;
						break;
					}

					if (delayOptimum>delayMin)
					{
						std::wstringstream text;
						text << "[Info ] Optimum transmit delay:  " << delayOptimum;
						resultToReport(reportType,text);
						retVal = pcscSetTransmitTiming(delayOptimum);
						REP_CONTRIBUTE(L"optimum_transmit_delay",L"%d",delayOptimum);
					}
				}
			}
			
			//------------------------------------------
			// it seems to be working ok, return
			//------------------------------------------
			m_bPassed = bCheckTimings;
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
	int getPCSCCardList(Report_TYPE reportType, Card_LIST* cardList)
	{
		resultToReport(reportType,L"[Info ] Generating card list using PCSC");

		int retVal = cardGetListFromPcsc(cardList);

		std::wstringstream text;
		if (retVal != DIAGLIB_OK)
		{
			resultToReport(reportType,L"[Error] Card list could not be retrieved");

			switch(retVal)
			{
			case DIAGLIB_ERR_BAD_CALL:
				text << L"[Error] Bad function call";
				REP_CONTRIBUTE(L"error",L"bad_function_call_retrieving_card_list");
				break;
			case DIAGLIB_ERR_LIBRARY_NOT_FOUND:
				text << L"[Error] Library not found";
				REP_CONTRIBUTE(L"error",L"library_not_found_retrieving_card_list");
				break;
			case DIAGLIB_ERR_INTERNAL:
				text << L"[Error] Internal error";
				REP_CONTRIBUTE(L"error",L"internal_error_retrieving_card_list");
				break;
			default:
				text << L"[Error] Unknown error";
				REP_CONTRIBUTE(L"error",L"unknown_error_retrieving_card_list");
				break;
			}
			resultToReport(reportType,text);
		}
		return retVal;
	}
};

#endif
