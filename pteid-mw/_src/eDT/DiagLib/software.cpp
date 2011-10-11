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

#include "software.h"
#include "error.h"
#include "log.h"
#include "progress.h"

#include "Repository.h"

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PRIVATE FUNCTIONS DECLARATION ////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
int softwareFillList(Soft_LIST *softList, const wchar_t *guid, const Soft_TYPE *type);

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PUBLIC FUNCTIONS /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
int softwareGetList (Soft_LIST *softList)
{
	return softwareFillList(softList, NULL, NULL);
} 

////////////////////////////////////////////////////////////////////////////////////////////////
int softwareGetIDs(Soft_GUID guid, Soft_LIST *softList)
{
	return softwareFillList(softList, guid.c_str(), NULL);
}

////////////////////////////////////////////////////////////////////////////////////////////////
int softwareReportInfo(Report_TYPE type, const Soft_INFO &info)
{
	int iReturnCode = DIAGLIB_OK;

	reportPrint(type,L"              id = %ls (%ls)\n",info.id.Guid.c_str(),info.id.Type==PER_USER_SOFT_TYPE?L"Per-User":L"Per-Machine");
	reportPrint(type,L"     DisplayName = %ls\n", info.DisplayName.c_str());
	reportPrint(type,L"  DisplayVersion = %ls\n", info.DisplayVersion.c_str());
	reportPrint(type,L" UninstallString = %ls\n", info.UninstallString.c_str());
	reportPrint(type,L"       Publisher = %ls\n", info.Publisher.c_str());
	reportPrint(type,L" InstallLocation = %ls\n", info.InstallLocation.c_str());
	reportPrintSeparator(type, REPORT_SOFTWARE_SEPARATOR);

	return iReturnCode;
}

void softwareContributeInfo(const Soft_INFO &info)
{
	REP_PREFIX(										info.id.Guid.c_str());
	REP_CONTRIBUTE(L"id",							info.id.Guid.c_str());
	REP_CONTRIBUTE(L"PerUserSoftType",				info.id.Type==PER_USER_SOFT_TYPE?L"PerUser":L"PerMachine");
	REP_CONTRIBUTE(L"DisplayName",					info.DisplayName.c_str());
	REP_CONTRIBUTE(L"DisplayVersion",				info.DisplayVersion.c_str());
	REP_CONTRIBUTE(L"UninstallString",				info.UninstallString.c_str());
	REP_CONTRIBUTE(L"Publisher",					info.Publisher.c_str());
	REP_CONTRIBUTE(L"InstallLocation",				info.InstallLocation.c_str());
	REP_UNPREFIX();
}

////////////////////////////////////////////////////////////////////////////////////////////////
int softwareReportList(Report_TYPE type, const Soft_LIST &softwareList, const wchar_t *TitleIn)
{
	int iReturnCode = DIAGLIB_OK;

	std::wstring Title;

	if(TitleIn!=NULL)
		Title=TitleIn;
	else
		Title=L"Software list";
	
	Title.append(L" (#");
	wchar_t buf[10];
	if(-1==swprintf_s(buf,10,L"%ld",softwareList.size()))
	{
		Title.append(L"???");
		LOG_ERROR(L"swprintf_s failed");
	}
	else
	{
		Title.append(buf);
	}
	Title.append(L")");

	reportPrintHeader2(type, Title.c_str(), REPORT_SOFTWARE_SEPARATOR);

	Soft_INFO info;

	progressInit((int)softwareList.size());

	Soft_LIST::const_iterator itr;
	for(itr=softwareList.begin();itr!=softwareList.end();itr++)
	{
		if(DIAGLIB_OK == softwareGetInfo(*itr,&info))
		{
			softwareReportInfo(type,info);
			softwareContributeInfo(info);
		}
		progressIncrement();
	}

	progressRelease();

	return iReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PRIVATE FUNCTIONS ////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
