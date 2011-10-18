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
#ifndef _ANALYSESERVICEINFO_H_
#define _ANALYSESERVICEINFO_H_

#include "analysis.h"
#include "services.h"

//******************************************
// Get a list of the running services
// 
// Pass:
//		always
//******************************************
class AnalyseServiceInfo : public Analysis
{
public:
	AnalyseServiceInfo()
	{
		m_testName = "service_info";
		m_friendlyName = "Service info";
	}
	virtual ~AnalyseServiceInfo()
	{
	}
	virtual int run()
	{
		m_bPassed = false;
		m_serviceList.clear();
		int retVal = DIAGLIB_OK;
		setProgress(0);
		setStartTime();

		retVal = serviceGetList (&m_serviceList);
		if (DIAGLIB_OK != retVal)
		{
			setEndTime();
			return retVal;
		}

		setProgress(50);

		retVal = serviceReportList(REPORT_TYPE_COMPLEMENT, m_serviceList);
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
private:
	Service_LIST m_serviceList;

};

#endif