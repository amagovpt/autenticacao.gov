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
#include "diaglib.h"

#include "system.h"
#include "error.h"
#include "log.h"

#include "Repository.h"

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PRIVATE FUNCTIONS DECLARATION ////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PUBLIC FUNCTIONS /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
int systemReportInfo(Report_TYPE type, const System_INFO &info)
{
	int iReturnCode = DIAGLIB_OK;

	reportPrint(type,L"          OsType = %ls\n", info.OsType.c_str());
	reportPrint(type,L"      PlatformId = %ld\n", info.PlatformId);
	reportPrint(type,L"    MajorVersion = %ls\n", info.MajorVersion.c_str());
	reportPrint(type,L"    MinorVersion = %ls\n", info.MinorVersion.c_str());
	reportPrint(type,L"     BuildNumber = %ls\n", info.BuildNumber.c_str());
	reportPrint(type,L"     ProductType = %ld\n", info.ProductType);
	reportPrint(type,L"     ServicePack = %ls\n", info.ServicePack.c_str());
	reportPrint(type,L"     ProductName = %ls\n", info.ProductName.c_str());
	reportPrint(type,L"     Description = %ls\n", info.Description.c_str());
	reportPrint(type,L" DefaultLanguage = %ls\n", info.DefaultLanguage.c_str());
	reportPrint(type,L" Architecture = %ls\n", info.Architecture.c_str());
	reportPrintSeparator(type, REPORT_SYSTEM_SEPARATOR);

	return iReturnCode;
}

void systemContributeInfo(const System_INFO &info)
{
	REP_CONTRIBUTE(L"osType",			L"%ls",	info.OsType.c_str());
	REP_CONTRIBUTE(L"platformId",		L"%ld",	info.PlatformId);
	REP_CONTRIBUTE(L"majorVersion",		L"%ls",	info.MajorVersion.c_str());
	REP_CONTRIBUTE(L"minorVersion",		L"%ls",	info.MinorVersion.c_str());
	REP_CONTRIBUTE(L"buildNumber",		L"%ls",	info.BuildNumber.c_str());
	REP_CONTRIBUTE(L"productType",		L"%ld",	info.ProductType);
	REP_CONTRIBUTE(L"servicePack",		L"%ls",	info.ServicePack.c_str());
	REP_CONTRIBUTE(L"productName",		L"%ls",	info.ProductName.c_str());
	REP_CONTRIBUTE(L"description",		L"%ls",	info.Description.c_str());
	REP_CONTRIBUTE(L"defaultLanguage",	L"%ls", info.DefaultLanguage.c_str());
	if(info.Architecture.find(L"running on 64 bit")!=wstring::npos)
		REP_CONTRIBUTE(L"arch",L"64");
	else
		REP_CONTRIBUTE(L"arch",L"32");
}

////////////////////////////////////////////////////////////////////////////////////////////////
int systemReport(Report_TYPE type, const wchar_t *Title)
{
	int iReturnCode = DIAGLIB_OK;

	if(Title!=NULL)
		reportPrintHeader2(type, Title, REPORT_SYSTEM_SEPARATOR);
	else
		reportPrintHeader2(type, L"System information", REPORT_SYSTEM_SEPARATOR);

	System_INFO info;
	if(DIAGLIB_OK == (iReturnCode = systemGetInfo(&info)))
	{
		systemReportInfo(type,info);
		systemContributeInfo(info);
	}

	return iReturnCode;
}
////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PRIVATE FUNCTIONS ////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
