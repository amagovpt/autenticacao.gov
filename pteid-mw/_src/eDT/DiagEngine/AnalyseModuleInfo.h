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
#ifndef _ANALYSEMODULEINFO_H_
#define _ANALYSEMODULEINFO_H_

#include <vector>
#include "error.h"
#include "module.h"
#include "analysis.h"

class AnalyseModuleInfo : public Analysis
{
public:
	AnalyseModuleInfo()
	{
		m_testName="module_info";
		m_friendlyName="Module Info";
	}

	virtual ~AnalyseModuleInfo()
	{
	}

	virtual int run()
	{
		m_bPassed = false;
		int retVal = DIAGLIB_OK;
		m_processList.clear();
		setProgress(0);
		setStartTime();
		retVal = processGetList(&m_processList);
		if (DIAGLIB_OK!=retVal)
		{
			setEndTime();
			return retVal;
		}
		setProgress(50);

		retVal = moduleReportList(REPORT_TYPE_COMPLEMENT, m_processList);
		if (DIAGLIB_OK!=retVal)
		{
			setEndTime();
			return retVal;
		}

		setEndTime();
		setProgress(100);
		m_bPassed = true;
		return 0;
	}
private:
	Proc_LIST	m_processList;
};

#endif //_ANALYSEMODULEINFO_H_
