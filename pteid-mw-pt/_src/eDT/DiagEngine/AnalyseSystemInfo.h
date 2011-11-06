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
#ifndef _ANALYSESYSTEMINFO_H_
#define _ANALYSESYSTEMINFO_H_

#include <ctime>
#include <iostream>
#include "error.h"
#include "analysis.h"
#include "system.h"
#include "Repository.h"

//******************************************
// Get the system info and report it
// 
// Pass:
//		always
//******************************************

class AnalyseSystemInfo : public Analysis
{
public:
	AnalyseSystemInfo()
	{
		m_testName = "system_info";
		m_friendlyName = "System info";
	}

	virtual ~AnalyseSystemInfo(void)
	{
	}

	virtual int run()
	{
		m_bPassed = false;
		int retVal = DIAGLIB_OK;

		setProgress(0);
		setStartTime();

		//------------------------------------------
		// generate the system report in REPORT_TYPE_MAIN
		//------------------------------------------
		if ( DIAGLIB_OK != (retVal = systemReport(REPORT_TYPE_MAIN)))
		{
			setEndTime();
			return retVal;
		}
		setEndTime();
		setProgress(100);
		m_bPassed = true;
		return retVal;
	}
};

#endif