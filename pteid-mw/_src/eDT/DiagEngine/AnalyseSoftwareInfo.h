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
#ifndef _ANALYSESOFTWAREINFO_H_
#define _ANALYSESOFTWAREINFO_H_

#include "error.h"
#include "analysis.h"
#include "software.h"

//******************************************
// Get a list of installed software and report it
// 
// Pass:
//		allways
//******************************************
class AnalyseSoftwareInfo : public Analysis
{
public:
	AnalyseSoftwareInfo()
	{
		m_testName = "software_info";
		m_friendlyName = "Software info";
	}
	virtual ~AnalyseSoftwareInfo()
	{
	}
	virtual int run()
	{
		m_bPassed = false;
		int retVal = DIAGLIB_OK;
		setProgress(0);
		setStartTime();
		
		Soft_LIST softList;
		retVal = softwareGetList(&softList);

		if (DIAGLIB_OK != retVal)
		{
			setEndTime();
			return retVal;
		}
 		setProgress(50);
 
		retVal = softwareReportList(REPORT_TYPE_COMPLEMENT, softList);

		if (DIAGLIB_OK != retVal)
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
