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
#ifndef _ANALYSESMARTCARDINFO_H_
#define _ANALYSESMARTCARDINFO_H_

#include "analysis.h"
#include "pcsc.h"
#include "Repository.h"

//******************************************
// Check the smartcard service (SCardSvr) running
// it will try to load the winscard.dll (= windows PCSC service) and
// 
// Pass:
//		SCardSvr available
// Fail:
//		SCardSvr not available
//******************************************
class AnalyseSmartcardInfo : public Analysis
{
public:
	AnalyseSmartcardInfo()
	{
		m_testName		= "smartcard_info";
		m_friendlyName	= "Smartcard service";
	}
	virtual ~AnalyseSmartcardInfo()
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
			reportPrintHeader2(reportType, L"Smartcard service", sepa);

			//------------------------------------------
			// Step 1: detect the PCSC daemon/service is available
			//------------------------------------------
			resultToReport(reportType,L"[Info ] Checking PCSC service available");
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
				REP_CONTRIBUTE(L"error",L"establishing_context_to_pcsc");
				resultToReport(reportType,L"[Info ] Checking service");

				Service_ID		service=L"SCardSvr";
				Service_INFO	info;
				retVal = serviceGetInfo(service, &info);

				if (DIAGLIB_OK!=retVal)
				{
					switch(retVal)
					{
					case DIAGLIB_ERR_BAD_CALL:
						resultToReport(reportType,L"[Error] Error calling serviceGetInfo()");
						REP_CONTRIBUTE(L"error",L"error_calling_servicegetinfo");
						break;
					case DIAGLIB_ERR_INTERNAL:
						resultToReport(reportType,L"[Error] Internal error serviceGetInfo()");
						REP_CONTRIBUTE(L"error",L"internal_error_calling_servicegetinfo");
						break;
					default:
						resultToReport(reportType,L"[Error] Unknown error serviceGetInfo()");
						REP_CONTRIBUTE(L"error",L"unknown_error_calling_servicegetinfo");
						break;
					}
					processParamsToStop();
					return retVal;
				}
				retVal = serviceReportInfo(reportType, info);
				serviceContributeInfo(info);
				processParamsToStop();
				resultToReport(reportType,m_bPassed);
				return retVal;
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
};

#endif
