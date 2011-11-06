/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2009 FedICT.
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
#ifndef _ANALYSESIGNTOKENDINFO_H_
#define _ANALYSESIGNTOKENDINFO_H_

#include "analysis.h"
#include "tokend.h"

//******************************************
// Perform a signing operation using tokend (Mac only)
// 
// Pass:
//		signing successful
// Fail:
//		signing failed
//******************************************
class AnalyseSignTokendInfo : public Analysis
{
public:
	AnalyseSignTokendInfo()
	{
		m_testName = "sign_tokend";
		m_friendlyName = "Tokend signing";
	}
	virtual ~AnalyseSignTokendInfo()
	{
	}
	virtual int run()
	{
		int retVal = DIAGLIB_OK;
		m_bPassed = false;
		setProgress(0);
		setStartTime();
		bool bAvailable = false;
		Report_TYPE reportType = REPORT_TYPE_RESULT;

		try
		{
			progressInit(2);

			wchar_t		sepa		= L'~';

			reportPrintHeader1(reportType, L"Tokend signing", sepa);
			resultToReport(reportType,L"[Info ]  Checking availability");
	
			retVal = tokendIsAvailable(&bAvailable);
			
			if (DIAGLIB_OK!=retVal)
			{
				processParamsToStop();
				resultToReport(reportType,L"[Error] Error calling tokendIsAvailable()");
				resultToReport(reportType,m_bPassed);
				progressRelease();
				return retVal;
			}
			if (!bAvailable)
			{
				processParamsToStop();
				resultToReport(reportType,L"[Error] tokend not available.");
				resultToReport(reportType,m_bPassed);
				progressRelease();
				return retVal;
			}
			
			progressIncrement();
			
			//------------------------------------------
			// Get the card list using PCSC
			//------------------------------------------
			Card_LIST cardList;
			retVal = cardGetListFromPcsc(&cardList);
						
			if (DIAGLIB_OK!=retVal)
			{
				processParamsToStop();
				resultToReport(reportType,L"[Error] Could not get card list using PCSC.");
				resultToReport(reportType,m_bPassed);
				progressRelease();
				return retVal;
			}
			
			if (0==cardList.size())
			{
				processParamsToStop();
				resultToReport(reportType,L"[Error] No eID card present in card reader(s).");
				resultToReport(reportType,m_bPassed);
				progressRelease();
				return retVal;
			}

			progressIncrement();
			
			int ToDo;		// check that only one card is inserted
			
			for (size_t idx=0; idx<cardList.size(); idx++)
			{
				bool bSucceed = false;
				
				resultToReport(reportType,L"[Info ] Test signing.");
				retVal = tokendTestSign(cardList.at(idx), AUTH_CERT_TYPE, &bSucceed);
				if (DIAGLIB_OK!=retVal)
				{
					std::wstringstream msg;
					switch(retVal)
					{
							//------------------------------------------
							// general errors
							//------------------------------------------
						case DIAGLIB_ERR_INTERNAL:
							msg << L"[Error] tokend internal error.";
							break;
						case DIAGLIB_ERR_LIBRARY_NOT_FOUND:
							msg << L"[Error] tokend library not found.";
							break;
							//------------------------------------------
							// PKCS errors
							//------------------------------------------
						case DIAGLIB_ERR_PKCS_INIT_FAILED:
							msg << L"[Error] tokend init failed";
							break;
						case DIAGLIB_ERR_PKCS_FAILED:
							msg << L"[Error] tokend failed";
							break;
						case DIAGLIB_ERR_PKCS_SIGNING_FAILED:
							msg << L"[Error] tokend signing failed";
							break;
						case DIAGLIB_ERR_PKCS_KEY_NOT_FOUND:
							msg << L"[Error] tokend key not found";
							break;
							//------------------------------------------
							// PIN errors
							//------------------------------------------
						case DIAGLIB_ERR_PIN_CANCEL:
							msg << L"[Error] tokend PIN code entry canceled";
							break;
						case DIAGLIB_ERR_PIN_BLOCKED:
							msg << L"[Error] tokend PIN code blocked";
							break;
						case DIAGLIB_ERR_PIN_WRONG:
							msg << L"[Error] tokend PIN code wrong";
							break;
						case DIAGLIB_ERR_PIN_FAILED:
							msg << L"[Error] tokend PIN code entry failed";
							break;
							//------------------------------------------
							// Card errors
							//------------------------------------------
						case DIAGLIB_ERR_CARD_NOT_FOUND:
							msg << L"[Error] Card not found";
							break;
						default:
							msg << L"[Error] tokend unknown error";
							break;
					}
					processParamsToStop();
					resultToReport(reportType,msg);
					resultToReport(reportType,m_bPassed);
					progressRelease();
					return retVal;
				}
				if (!bSucceed)
				{
					processParamsToStop();
					resultToReport(reportType,L"[Error] tokend did not succeed");
					resultToReport(reportType,m_bPassed);
					progressRelease();
					return retVal;
				}
			}
			
			processParamsToStop();
			
			m_bPassed = true;
			resultToReport(reportType,L"[Info ] Tokend verification succeeded");
			resultToReport(reportType,m_bPassed);
			progressRelease();
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
		progressRelease();
		return retVal;
	}
};

#endif
