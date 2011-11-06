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

#include "middleware.h"
#include "error.h"
#include "log.h"
#include "util.h"
#include "progress.h"

#include "pteidlib.h"

#include "Repository.h"

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PRIVATE FUNCTIONS DECLARATION ////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
int mwFillList(MW_LIST *middlewareList);

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PUBLIC FUNCTIONS /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
int mwGetList(MW_LIST *middlewareList)
{
	return mwFillList(middlewareList);
} 

////////////////////////////////////////////////////////////////////////////////////////////////
int mwIsPteidlibAvailable(bool *available)
{
	int iReturnCode = DIAGLIB_OK;

	if(available == NULL)
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;
	}

	*available = false;

	const PteidlibWrapper *pteidlib = NULL;
	
	if(NULL == (pteidlib = loadPteidLibWrapper()))
	{
		LOG_ERROR(L"loadPteidLibWrapper failed");
		return DIAGLIB_ERR_LIBRARY_NOT_FOUND;
	}
	
	return pteidlib->pteidlibIsAvailable(available);
}

////////////////////////////////////////////////////////////////////////////////////////////////
int mwGetReaderList(Reader_LIST *readerList)
{
	int iReturnCode = DIAGLIB_OK;

	if(readerList == NULL)
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;
	}

	readerList->clear();

	const PteidlibWrapper *pteidlib = NULL;
	
	if(NULL == (pteidlib = loadPteidLibWrapper()))
	{
		LOG_ERROR(L"loadPteidLibWrapper failed");
		return DIAGLIB_ERR_LIBRARY_NOT_FOUND;
	}
	
	return pteidlib->pteidlibGetReaderList(readerList);
} 

////////////////////////////////////////////////////////////////////////////////////////////////
int mwGetCardList(Card_LIST *cardList)
{
	int iReturnCode = DIAGLIB_OK;

	if(cardList == NULL)
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;
	}

	cardList->clear();

	const PteidlibWrapper *pteidlib = NULL;
	
	if(NULL == (pteidlib = loadPteidLibWrapper()))
	{
		LOG_ERROR(L"loadPteidLibWrapper failed");
		return DIAGLIB_ERR_LIBRARY_NOT_FOUND;
	}
	
	return pteidlib->pteidlibGetCardList(cardList);
} 


////////////////////////////////////////////////////////////////////////////////////////////////
int mwGetCardInfo (Card_ID id, Card_INFO *info)
{
	int iReturnCode = DIAGLIB_OK;

	if(info == NULL)
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;
	}

	const PteidlibWrapper *pteidlib = NULL;
	
	if(NULL == (pteidlib = loadPteidLibWrapper()))
	{
		LOG_ERROR(L"loadPteidLibWrapper failed");
		return DIAGLIB_ERR_LIBRARY_NOT_FOUND;
	}
	
	return pteidlib->pteidlibGetCardInfo(id, info);
} 

////////////////////////////////////////////////////////////////////////////////////////////////
int mwReportInfo(Report_TYPE type, const MW_INFO &info)
{
	int iReturnCode = DIAGLIB_OK;

	reportPrint(type,L"          id = %ls (%ls)\n",info.id.Guid.c_str(),info.id.Type==PER_USER_SOFT_TYPE?L"Per-User":L"Per-Machine");
	reportPrint(type,L"     Version = %ls\n", info.LabelVersion.c_str());
	reportPrint(type,L" DisplayName = %ls\n", info.DisplayName.c_str());
	reportPrintSeparator(type, REPORT_MW_SEPARATOR);

	return iReturnCode;
}

void mwContributeInfo(const MW_INFO &info)
{
	REP_PREFIX(L"%ls",						info.id.Guid.c_str());
	REP_CONTRIBUTE(L"id",				info.id.Guid.c_str());
	REP_CONTRIBUTE(L"PerUserSoftType",	info.id.Type==PER_USER_SOFT_TYPE?L"PerUser":L"PerMachine");
	REP_CONTRIBUTE(L"Version",			info.LabelVersion.c_str());
	REP_CONTRIBUTE(L"DisplayName",		info.DisplayName.c_str());
	REP_UNPREFIX();
}


////////////////////////////////////////////////////////////////////////////////////////////////
int mwReportList(Report_TYPE type, const MW_LIST &middlewareList, const wchar_t *TitleIn)
{
	int iReturnCode = DIAGLIB_OK;

	std::wstring Title;

	if(TitleIn!=NULL)
		Title=TitleIn;
	else
		Title=L"Middleware list";
	
	Title.append(L" (#");
	wchar_t buf[10];
	if(-1==swprintf_s(buf,10,L"%ld",middlewareList.size()))
	{
		Title.append(L"???");
		LOG_ERROR(L"swprintf_s failed");
	}
	else
	{
		Title.append(buf);
	}
	Title.append(L")");

	reportPrintHeader2(type, Title.c_str(), REPORT_MW_SEPARATOR);

	MW_INFO info;

	progressInit((int)middlewareList.size());

	MW_LIST::const_iterator itr;
	for(itr=middlewareList.begin();itr!=middlewareList.end();itr++)
	{
		if(DIAGLIB_OK == mwGetInfo(*itr,&info))
		{
			mwReportInfo(type,info);
			mwContributeInfo(info);
		}
		progressIncrement();
	}
	
	progressRelease();

	return iReturnCode;
}
////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PRIVATE FUNCTIONS ////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
