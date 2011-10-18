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
#elif __APPLE__
#include "Mac/mac_helper.h"
#endif

#include "diaglib.h"

#include "services.h"
#include "error.h"
#include "log.h"
#include "progress.h"

#include "Repository.h"

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PRIVATE FUNCTIONS DECLARATION ////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
int serviceFillList (Service_LIST *serviceList, const wchar_t *processDisplayName);

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PUBLIC FUNCTIONS /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
int serviceGetList (Service_LIST *serviceList)
{
	return serviceFillList(serviceList,NULL);
} 

////////////////////////////////////////////////////////////////////////////////////////////////
int serviceGetIDs (Service_NAME service, Service_LIST *serviceList)
{
	return serviceFillList(serviceList,service.c_str());
} 

////////////////////////////////////////////////////////////////////////////////////////////////
int serviceReportInfo(Report_TYPE type, const Service_INFO &info)
{
	int iReturnCode = DIAGLIB_OK;

	reportPrint(type,L"           id = %ls\n",info.id.c_str());
	reportPrint(type,L" Display name = %ls\n",info.DisplayName.c_str());
	reportPrint(type,L"       Status = %ld (%ls)\n",info.Status, info.StatusLabel.c_str());
	reportPrint(type,L"   Process Id = %ld\n",info.ProcessId);
	reportPrintSeparator(type, REPORT_SERVICE_SEPARATOR);

	return iReturnCode;
}

void serviceContributeInfo(const Service_INFO &info)
{
	REP_PREFIX(L"%ls",					info.id.c_str());
	REP_CONTRIBUTE(L"id",				info.id.c_str());
	REP_CONTRIBUTE(L"DisplayName",		info.DisplayName.c_str());
	REP_CONTRIBUTE(L"Status",L"%ld",	info.Status);
	REP_CONTRIBUTE(L"StatusLabel",		info.StatusLabel.c_str());
	REP_CONTRIBUTE(L"ProcessId",L"%ld",	info.ProcessId);
	REP_UNPREFIX();
}

////////////////////////////////////////////////////////////////////////////////////////////////
int serviceReportList(Report_TYPE type, const Service_LIST &serviceList, const wchar_t *TitleIn)
{
	int iReturnCode = DIAGLIB_OK;

	std::wstring Title;

	if(TitleIn!=NULL)
		Title=TitleIn;
	else
		Title=L"Services list";
	
	Title.append(L" (#");
	wchar_t buf[10];
	if(-1==swprintf_s(buf,10,L"%ld",serviceList.size()))
	{
		Title.append(L"???");
		LOG_ERROR(L"swprintf_s failed");
	}
	else
	{
		Title.append(buf);
	}
	Title.append(L")");

	reportPrintHeader2(type, Title.c_str(), REPORT_SERVICE_SEPARATOR);

	Service_INFO info;

	progressInit((int)serviceList.size());

	Service_LIST::const_iterator itr;
	for(itr=serviceList.begin();itr!=serviceList.end();itr++)
	{
		if(DIAGLIB_OK == serviceGetInfo(*itr,&info))
		{
			serviceReportInfo(type,info);
			serviceContributeInfo(info);
		}
		progressIncrement();
	}

	progressRelease();

	return iReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PRIVATE FUNCTIONS ////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
