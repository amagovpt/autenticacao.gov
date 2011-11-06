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

#include "reader.h"
#include "error.h"
#include "log.h"
#include "progress.h"

#include "device.h"
#include "pcsc.h"
#include "middleware.h"

#include "Repository.h"

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PRIVATE FUNCTIONS DECLARATION ////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
const wchar_t *getSourceName(Reader_SOURCE source);

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PUBLIC FUNCTIONS /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
int readerGetListFromDevice(Reader_LIST *readersList)
{
	return deviceGetReaderList(readersList);
}

////////////////////////////////////////////////////////////////////////////////////////////////
int readerGetListFromPcsc(Reader_LIST *readersList)
{
	return pcscGetReaderList(readersList);
}

////////////////////////////////////////////////////////////////////////////////////////////////
int readerGetListFromMW(Reader_LIST *readersList)
{
	return mwGetReaderList(readersList);
} 

////////////////////////////////////////////////////////////////////////////////////////////////
int readerGetInfo(Reader_ID reader, Reader_INFO *info)
{
	int iReturnCode = DIAGLIB_OK;

	if(info == NULL)
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;
	}

	info->id=reader;	

	return iReturnCode;
}


////////////////////////////////////////////////////////////////////////////////////////////////
int readerGetCardInfo(Card_ID id, Card_INFO *info)
{
	int iReturnCode = DIAGLIB_OK;

	if(info == NULL)
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;
	}

	switch(id.Reader.Source)
	{
	case PCSC_READER_SOURCE:
		return pcscGetCardInfo(id, info);
	case MW_READER_SOURCE:
		return mwGetCardInfo(id, info);
	default:
		iReturnCode = DIAGLIB_ERR_NOT_AVAILABLE;
		break;
	}

	return iReturnCode;
} 

////////////////////////////////////////////////////////////////////////////////////////////////
int readerReportInfo(Report_TYPE type, const Reader_INFO &info)
{
	reportPrint(type,L"          id = %ls (%ls view)\n",info.id.Name.c_str(),getSourceName(info.id.Source));
	reportPrintSeparator(type, REPORT_READER_SEPARATOR);

	REP_PREFIX(L"%ls",					info.id.Name.c_str());
	REP_CONTRIBUTE(L"id",				info.id.Name.c_str());
	REP_CONTRIBUTE(L"sourceName",		getSourceName(info.id.Source));
	REP_UNPREFIX();
	return DIAGLIB_OK;
}

/* DEAD CODE - not called from anywhere..
void readerContributeInfo(const Reader_INFO &info)
{
	REP_PREFIX(							info.id.Name.c_str());
	REP_CONTRIBUTE(L"id",				info.id.Name.c_str());
	REP_CONTRIBUTE(L"sourceName",		getSourceName(info.id.Source));
	REP_UNPREFIX();
}

////////////////////////////////////////////////////////////////////////////////////////////////
int readerReportList(Report_TYPE type, const Reader_LIST &readerList, const wchar_t *TitleIn)
{
	int iReturnCode = DIAGLIB_OK;

	std::wstring Title;

	if(TitleIn!=NULL)
		Title=TitleIn;
	else
		Title=L"Reader list";
	
	Title.append(L" (#");
	wchar_t buf[10];
	if(-1==swprintf_s(buf,10,L"%ld",readerList.size()))
	{
		Title.append(L"???");
		LOG_ERROR(L"swprintf_s failed");
	}
	else
	{
		Title.append(buf);
	}
	Title.append(L")");

	reportPrintHeader2(type, Title.c_str(), REPORT_READER_SEPARATOR);

	Reader_INFO info;

	progressInit((int)readerList.size());

	Reader_LIST::const_iterator itr;
	for(itr=readerList.begin();itr!=readerList.end();itr++)
	{
		if(DIAGLIB_OK == readerGetInfo(*itr,&info))
		{
			readerReportInfo(type,info);
			readerContributeInfo(info);
		}
		progressIncrement();
	}

	progressRelease();

	return iReturnCode;
}

*/
////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PRIVATE FUNCTIONS ////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
const wchar_t *getSourceName(Reader_SOURCE source)
{
	switch(source)
	{
	case DEVICE_READER_SOURCE:
		return L"Device";
	case PCSC_READER_SOURCE:
		return L"Pcsc";
	case MW_READER_SOURCE:
		return L"Middleware";
	default:
		return L"???";
	}

}