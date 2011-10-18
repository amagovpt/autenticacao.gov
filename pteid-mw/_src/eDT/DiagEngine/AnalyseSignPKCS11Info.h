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
#ifndef _ANALYSESIGNPKCS11INFO_H_
#define _ANALYSESIGNPKCS11INFO_H_

#include "analysis.h"
#include "pkcs.h"
#include "Repository.h"

//******************************************
// Perform a signing operation using the PKCS11
// 
// Pass:
//		Signing sucessful
// Fail:
//		signing failed
//******************************************
class AnalyseSignPKCS11Info : public Analysis
{
public:
	AnalyseSignPKCS11Info()
	{
		m_testName = "sign_pkcs11";
		m_friendlyName = "PKCS signing";
	}
	virtual ~AnalyseSignPKCS11Info()
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

			reportPrintHeader2(reportType, L"PKCS signing", sepa);
			resultToReport(reportType,L"[Info ] PKCS Checking availability");
			retVal = pkcsIsAvailable(&bAvailable);

			if (DIAGLIB_OK!=retVal)
			{
				processParamsToStop();
				resultToReport(reportType,L"[Error] Error calling pkcsIsAvailable()");
				REP_CONTRIBUTE(L"error",L"calling_pkcsisavailable");
				resultToReport(reportType,m_bPassed);
				progressRelease();
				return retVal;
			}

			if (!bAvailable)
			{
				processParamsToStop();
				resultToReport(reportType,L"[Error] PKCS not available, could not acquire context.");
				REP_CONTRIBUTE(L"error",L"pkcs_not_available");
				resultToReport(reportType,m_bPassed);
				progressRelease();
				return retVal;
			}
			//------------------------------------------
			// Get the card list using PCSC
			//------------------------------------------
			Card_LIST cardList;
			retVal = cardGetListFromPcsc(&cardList);

			progressIncrement();

			if (DIAGLIB_OK!=retVal)
			{
				processParamsToStop();
				resultToReport(reportType,L"[Error] Could not get card list using PCSC.");
				REP_CONTRIBUTE(L"error",L"getting_cardlist_using_pcsc");
				resultToReport(reportType,m_bPassed);
				progressRelease();
				return retVal;
			}

			if (0==cardList.size())
			{
				processParamsToStop();
				resultToReport(reportType,L"[Error] No eID card present in cardreader(s).");
				REP_CONTRIBUTE(L"error",L"no_eid_present");
				resultToReport(reportType,m_bPassed);
				progressRelease();
				return retVal;
			}

			if (1<cardList.size())
			{
				processParamsToStop();
				resultToReport(reportType,L"[Error] More than one eID card inserted");
				REP_CONTRIBUTE(L"error",L"multiple_eid_present");
				resultToReport(reportType,m_bPassed);
				progressRelease();
				return retVal;
			}

			for (size_t idx=0; idx<cardList.size(); idx++)
			{
				bool bSucceed = false;

				resultToReport(reportType,L"[Info ] Test signing.");
				retVal = pkcsTestSign(cardList.at(idx), AUTH_CERT_TYPE, &bSucceed);
				if (DIAGLIB_OK!=retVal)
				{
					std::wstringstream msg;
					switch(retVal)
					{
					//------------------------------------------
					// general errors
					//------------------------------------------
					case DIAGLIB_ERR_INTERNAL:
						msg << L"[Error] PKCS internal error.";
						REP_CONTRIBUTE(L"error",L"pkcs_internal_error");
						break;
					case DIAGLIB_ERR_LIBRARY_NOT_FOUND:
						msg << L"[Error] PKCS library not found.";
						REP_CONTRIBUTE(L"error",L"pkcs_library_not_found");
						break;
					//------------------------------------------
					// PKCS errors
					//------------------------------------------
					case DIAGLIB_ERR_PKCS_INIT_FAILED:
						msg << L"[Error] PKCS init failed";
						REP_CONTRIBUTE(L"error",L"pkcs_init_failed");
						break;
					case DIAGLIB_ERR_PKCS_FAILED:
						msg << L"[Error] PKCS failed";
						REP_CONTRIBUTE(L"error",L"pkcs_failed");
						break;
					case DIAGLIB_ERR_PKCS_SIGNING_FAILED:
						msg << L"[Error] PKCS signing failed";
						REP_CONTRIBUTE(L"error",L"pkcs_signing_failed");
						break;
					case DIAGLIB_ERR_PKCS_KEY_NOT_FOUND:
						msg << L"[Error] PKCS key not found";
						REP_CONTRIBUTE(L"error",L"pkcs_key_not_found");
						break;
					//------------------------------------------
					// PIN errors
					//------------------------------------------
					case DIAGLIB_ERR_PIN_CANCEL:
						msg << L"[Error] PKCS PIN code entry canceled";
						REP_CONTRIBUTE(L"error",L"pkcs_pin_entry_cancelled");
						break;
					case DIAGLIB_ERR_PIN_BLOCKED:
						msg << L"[Error] PKCS PIN code blocked";
						REP_CONTRIBUTE(L"error",L"pkcs_pin_blocked");
						break;
					case DIAGLIB_ERR_PIN_WRONG:
						msg << L"[Error] PKCS PIN code wrong";
						REP_CONTRIBUTE(L"error",L"pkcs_pin_wrong");
						break;
					case DIAGLIB_ERR_PIN_FAILED:
						msg << L"[Error] PKCS PIN code entry failed";
						REP_CONTRIBUTE(L"error",L"pkcs_entry_failed");
						break;
					//------------------------------------------
					// Card errors
					//------------------------------------------
					case DIAGLIB_ERR_CARD_NOT_FOUND:
						msg << L"[Error] Card not found";
						REP_CONTRIBUTE(L"error",L"card_not_found");
						break;
					default:
						msg << L"[Error] PKCS unknown error";
						REP_CONTRIBUTE(L"error",L"pkcs_unknown_error");
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
					resultToReport(reportType,L"[Error] PKCS did not succeed");
					REP_CONTRIBUTE(L"result",L"failed");
					resultToReport(reportType,m_bPassed);
					progressRelease();
					return retVal;
				}
			}
			progressIncrement();

			processParamsToStop();
			m_bPassed = true;
			resultToReport(reportType,L"[Info ] PKCS verification succeeded");
			REP_CONTRIBUTE(L"result",L"passed");
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
