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
#ifndef _ANALYSEHW_H_
#define _ANALYSEHW_H_

#ifdef WIN32

#include "analysis.h"
#include "hardware.h"
#include "Repository.h"

class AnalyseHardware : public Analysis
{
public:
	AnalyseHardware()
	{
		m_testName = "hardware_info";
		m_friendlyName = "Hardware";
	}

	virtual ~AnalyseHardware()
	{
	}

	virtual int run()
	{
		int retVal = DIAGLIB_OK;
		m_bPassed = true;
		setProgress(0);
		setStartTime();
		Report_TYPE reportType = REPORT_TYPE_RESULT;

		try
		{
			progressInit(1);
			reportPrintHeader2(reportType, L"Hardware",L'~');
			if (DIAGLIB_OK != (retVal = hardwareReport(REPORT_TYPE_MAIN)))
			{
				setEndTime();
				return retVal;
			}
			progressIncrement();
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

#endif
