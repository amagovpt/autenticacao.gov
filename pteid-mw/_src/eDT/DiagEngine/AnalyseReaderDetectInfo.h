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
#ifndef _ANALYSEREADERDETECTINFO_H_
#define _ANALYSEREADERDETECTINFO_H_

#include "analysis.h"
#include "reader.h"
#include "Repository.h"

//******************************************
// Get a list of available readers detected by the OS
// On Windows, even disabled readers are listed
// 
// Pass:
//		readers are detected
// Fail:
//		no readers detected
//******************************************
class AnalyseReaderDetectInfo : public Analysis
{
public:
	AnalyseReaderDetectInfo()
	{
		m_testName = "reader_detect";
		m_friendlyName = "USB reader detection";
	}
	virtual ~AnalyseReaderDetectInfo()
	{
	}
	virtual int run()
	{
		int retVal = DIAGLIB_OK;
		m_bPassed = false;
		Report_TYPE reportType = REPORT_TYPE_RESULT;
		setProgress(0);
		setStartTime();
		try
		{
			//------------------------------------------
			// write to the report what we're doing
			//------------------------------------------
			wchar_t		sepa		= L'~';
			reportPrintHeader2(reportType, L"Detecting card reader devices", sepa);

			Device_CLASS device = L"SmartCardReader";
			Device_LIST  deviceList;
			retVal = deviceGetIDsByClass(device, &deviceList);
			if (DIAGLIB_OK!=retVal)
			{
				resultToReport(reportType,L"[Error] Error calling deviceGetIDsByClass()");
				REP_CONTRIBUTE(L"error",L"calling_devicegetidbyclass");
				switch(retVal)
				{
				case DIAGLIB_ERR_INTERNAL:
				default:
					resultToReport(reportType,L"[Error] Internal error");
					REP_CONTRIBUTE(L"error",L"internal_error");
				    break;
				}
				setEndTime();
				setProgress(100);
				return retVal;
			}

			//------------------------------------------
			// Report if the device list is empty, but don't fail
			//------------------------------------------
			if ( 0==deviceList.size() )
			{
				m_bPassed = true;
				setEndTime();
				setProgress(100);
				resultToReport(reportType,L"[Warn ] No card reader detected");
				REP_CONTRIBUTE(L"error",L"no_card_readers_detected");
				return retVal;
			}
			//------------------------------------------
			// Report all the devices found
			//------------------------------------------
			retVal = deviceReportList(reportType, deviceList);
			if (DIAGLIB_OK!=retVal)
			{
				setEndTime();
				setProgress(100);
				return retVal;
			}
		}
		//------------------------------------------
		// exception from writing to the report
		//------------------------------------------
		catch (ExcReport& exc) 
		{
			setEndTime();
			setProgress(100);
			retVal = exc.getErr();
		}
		setEndTime();
		setProgress(100);
		m_bPassed = true;
		return retVal;
	}
};

#endif
