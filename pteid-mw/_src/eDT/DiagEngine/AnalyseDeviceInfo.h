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
#ifndef _ANALYSEDEVICEINFO_H_
#define _ANALYSEDEVICEINFO_H_

#include "analysis.h"
#include "error.h"
#include "device.h"

//******************************************
// Get a list of all devices on the system
// 
// Pass:
//		always
//******************************************
class AnalyseDeviceInfo : public Analysis
{
public:
	AnalyseDeviceInfo(void)
	{
		m_testName = "device_info";
		m_friendlyName = "Device info";
	}
	virtual ~AnalyseDeviceInfo(void)
	{
	}
	virtual int run()
	{
		m_bPassed = false;
		int retVal = DIAGLIB_OK;

		m_deviceList.clear();

		setProgress(0);
		setStartTime();

		//--------------------------------------------------
		// first of all, get the device list
		//--------------------------------------------------
		retVal = deviceGetList(&m_deviceList);

		if( DIAGLIB_OK != retVal)
		{
			setEndTime();
			return retVal;
		}

		setProgress(50);

		//--------------------------------------------------
		// generate the report in REPORT_TYPE_COMPLEMENT
		//--------------------------------------------------
		retVal = deviceReportList(REPORT_TYPE_COMPLEMENT, m_deviceList);

		if( DIAGLIB_OK != retVal)
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
	Device_LIST m_deviceList;

};


#endif