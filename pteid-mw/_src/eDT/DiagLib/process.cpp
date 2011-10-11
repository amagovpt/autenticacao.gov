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
#include "process.h"
#include "error.h"
#include "log.h"
#include "progress.h"
#include "util.h"
#include "repository.h"

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PRIVATE FUNCTIONS DECLARATION ////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
int processFillList(Proc_LIST *processList, const wchar_t *processName);

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PUBLIC FUNCTIONS /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
int processGetList(Proc_LIST *processList)
{
	return processFillList(processList,NULL);
} 

////////////////////////////////////////////////////////////////////////////////////////////////
int processGetIDs(Proc_NAME process, Proc_LIST *processList)
{
	return processFillList(processList,process.c_str());
} 

////////////////////////////////////////////////////////////////////////////////////////////////
int processKillByName(Proc_NAME process)
{
	int iReturnCode = DIAGLIB_OK;

	if(process.empty())
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;
	}

	LOG_TIME(L"Ask for killing process named '%ls' --> ",process.c_str());

	Proc_LIST processList;
	if(DIAGLIB_OK != (iReturnCode=processFillList(&processList,process.c_str())))
	{
		return RETURN_LOG_ERROR(iReturnCode);
	}

	if(processList.size()==0)
	{
		LOG(L"NO PROCESS FOUND\n");
		return DIAGLIB_OK;
	}

	LOG(L"FOUND %ld pocesses\n", processList.size());

	Proc_LIST::iterator itr;
	for(itr=processList.begin();itr!=processList.end();itr++)
	{
		if(DIAGLIB_OK != (iReturnCode=processKill(*itr)))
		{
			break;
		}
	}

	return iReturnCode;
} 

////////////////////////////////////////////////////////////////////////////////////////////////
int processReportInfo(Report_TYPE type, const Proc_INFO &info)
{ 
	reportPrint(type,L"        id = %ld\n",info.id);
	reportPrint(type,L"   Running = true\n");
	reportPrint(type,L"      Name = %ls\n", info.Name.c_str());
	reportPrint(type,L"      Path = %ls\n", info.Path.c_str());
	reportPrint(type,L" Full path = %ls\n", info.FullPath.c_str());
	for(ModuleIterator iter=info.modulesLoaded.begin();iter!=info.modulesLoaded.end();iter++)
		reportPrint(REPORT_TYPE_COMPLEMENT,L"Uses Library [%ls]\n",iter->c_str());
	reportPrintSeparator(type, REPORT_PROCESS_SEPARATOR);
	return DIAGLIB_OK;
}

void processContributeInfo(const Proc_INFO &info)
{
	REP_PREFIX(							info.Name.c_str());
	REP_CONTRIBUTE( L"running",			L"true");
	REP_CONTRIBUTE(	L"id",	L"%ld",		info.id);
	REP_CONTRIBUTE(	L"Name",			info.Name.c_str());
	REP_CONTRIBUTE(	L"Path",			info.Path.c_str());
	REP_CONTRIBUTE(	L"FullPath",		info.FullPath.c_str());
	for(ModuleIterator iter=info.modulesLoaded.begin();iter!=info.modulesLoaded.end();iter++)
		REP_CONTRIBUTE( L"Uses",iter->c_str());
	REP_UNPREFIX();
}

////////////////////////////////////////////////////////////////////////////////////////////////
int processReportList(Report_TYPE type, const Proc_LIST &processList, const wchar_t *TitleIn)
{
	int				iReturnCode=DIAGLIB_OK;
	std::wstring	Title;

	if(TitleIn!=NULL)
		Title=TitleIn;
	else
		Title=L"Process list";
	
	Title.append(L" (#");
	wchar_t buf[10];
	if(-1==swprintf_s(buf,10,L"%ld",processList.size()))
	{
		Title.append(L"???");
		LOG_ERROR(L"swprintf_s failed");
	}
	else
	{
		Title.append(buf);
	}
	Title.append(L")");

	reportPrintHeader2(type, Title.c_str(), REPORT_PROCESS_SEPARATOR);

	Proc_INFO info;

	progressInit(processList.size());

	Proc_LIST::const_iterator itr;
	for(itr=processList.begin();itr!=processList.end();itr++)
	{
		if(DIAGLIB_OK == processGetInfo(*itr,&info))
		{
			processReportInfo(type,info);
			processContributeInfo(info);
		}
		progressIncrement();
	}

	progressRelease();

	return iReturnCode;
}
