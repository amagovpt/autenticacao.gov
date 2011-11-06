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
#ifdef WIN32

#include <windows.h>
#include "diaglib.h"
#include "process.h"
#include "error.h"
#include "log.h"
#include "hardware.h"
#include "WinRegKey.h"
#include "Repository.h"

int hardwareGetInfo(Hardware_INFO *info)
{
	int iReturnCode;
	try
	{
		WinRegKey rkey(HKEY_LOCAL_MACHINE,L"HARDWARE\\DESCRIPTION\\System\\BIOS");
		info->systemProductName=rkey(L"SystemProductName");
		info->systemManufacturer=rkey(L"SystemManufacturer");
		iReturnCode=DIAGLIB_OK;
	}
	catch(WinRegKeyExeption rke)
	{
		iReturnCode=DIAGLIB_ERR_NOT_AVAILABLE;
	}
	return iReturnCode;
}

int hardwareReportInfo(Report_TYPE type, const Hardware_INFO &info)
{
	reportPrint(type,L"systemManufacturer = %ls (%ls)\n",	info.systemManufacturer.c_str());
	reportPrint(type,L" systemProductName = %ls\n",			info.systemProductName.	c_str());
	reportPrintSeparator(type, REPORT_MW_SEPARATOR);
	return DIAGLIB_OK;
}

void hardwareContributeInfo(const Hardware_INFO &info)
{
	REP_PREFIX(L"hardware");
	REP_CONTRIBUTE(L"systemManufacturer",	info.systemManufacturer.c_str());
	REP_CONTRIBUTE(L"systemProductName",	info.systemProductName.	c_str());
	REP_UNPREFIX();
}

int hardwareReport(Report_TYPE type)
{
	int iReturnCode = DIAGLIB_OK;
	reportPrintHeader2(type, L"Hardware Information", REPORT_SYSTEM_SEPARATOR);
	Hardware_INFO info;
	if(DIAGLIB_OK == (iReturnCode = hardwareGetInfo(&info)))
	{
		hardwareReportInfo(type,info);
		hardwareContributeInfo(info);
	}
	return iReturnCode;
}

#endif
