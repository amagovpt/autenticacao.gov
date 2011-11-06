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

#include <iostream>
#include "sys/stat.h"

#include "file.h"

#include "diaglib.h"
#include "error.h"
#include "log.h"
#include "progress.h"

#include "Repository.h"

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PRIVATE FUNCTIONS DECLARATION ////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PUBLIC FUNCTIONS /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
int fileExists(const File_ID file, bool *exist)
{
	int iReturnCode = DIAGLIB_OK;
	
	*exist=false;

	if(exist==NULL)
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;
	}

	struct _stat buffer;
	if(0 == _wstat(file.c_str(),&buffer))
	{
		*exist=true;
	}

	return iReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
int fileCreate(const File_ID file)
{
	int iReturnCode = DIAGLIB_OK;

	bool exist;

	//Check if the file already exist
	if(DIAGLIB_OK != (iReturnCode=fileExists(file,&exist)))
	{
		return RETURN_LOG_ERROR(DIAGLIB_ERR_INTERNAL);
	}
	else
	{
		//If exist return
		if(exist)
		{
			return RETURN_LOG_ERROR(DIAGLIB_ERR_FILE_ALREADY_EXIST);
		}
		else
		{
			//Create the file
			FILE *f=NULL;
			_wfopen_s(&f, file.c_str(), L"w");
			if (!f)
			{
				return RETURN_LOG_ERROR(DIAGLIB_ERR_FILE_CREATE_FAILED);
			}

			fclose(f);
			f=NULL;
		}
	}

	return iReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
int fileReportInfo(Report_TYPE type, const File_INFO &info)
{
	int iReturnCode = DIAGLIB_OK;

	reportPrint(type,L"			     id = %ls\n",info.id.c_str());
	reportPrint(type,L" Product Version = %ls\n",info.ProductVersion.c_str());
	reportPrint(type,L"    File Version = %ls\n",info.FileVersion.c_str());
	reportPrint(type,L"            Size = %ld\n",info.FileSize);
	reportPrintSeparator(type, REPORT_FILE_SEPARATOR);
	
	return iReturnCode;
}

void fileContributeInfo(const File_INFO &info)
{
	REP_PREFIX(							info.id.c_str());
	REP_CONTRIBUTE(L"id",				info.id.c_str());
	REP_CONTRIBUTE(L"ProductVersion",	info.ProductVersion.c_str());
	REP_CONTRIBUTE(L"FileVersion",		info.FileVersion.c_str());
	REP_CONTRIBUTE(L"Size",L"%ld\n",	info.FileSize);
	REP_UNPREFIX();
}

////////////////////////////////////////////////////////////////////////////////////////////////
int fileReportList(Report_TYPE type, const File_LIST &fileList, const wchar_t *TitleIn)
{
	int iReturnCode = DIAGLIB_OK;

	std::wstring Title;

	if(TitleIn!=NULL)
		Title=TitleIn;
	else
		Title=L"File list";
	
	Title.append(L" (#");
	wchar_t buf[10];
	if(-1==swprintf_s(buf,10,L"%ld",fileList.size()))
	{
		Title.append(L"???");
		LOG_ERROR(L"swprintf_s failed");
	}
	else
	{
		Title.append(buf);
	}
	Title.append(L")");

	reportPrintHeader2(type, Title.c_str(), REPORT_FILE_SEPARATOR);

	File_INFO info;

	progressInit((int)fileList.size());

	File_LIST::const_iterator itr;
	for(itr=fileList.begin();itr!=fileList.end();itr++)
	{
		if(DIAGLIB_OK == fileGetInfo(*itr,&info))
		{
			fileReportInfo(type,info);
			fileContributeInfo(info);
		}
		progressIncrement();
	}

	progressRelease();

	return iReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PRIVATE FUNCTIONS ////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
