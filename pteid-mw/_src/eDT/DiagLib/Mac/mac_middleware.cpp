/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2009 FedICT.
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
#include "mac_helper.h"

#include "middleware.h"
#include "error.h"
#include "log.h"
#include "util.h"
#include "progress.h"

#include "folder.h"

#include "beidlib.h"

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PRIVATE FUNCTIONS DECLARATION ////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
int mwFillList(MW_LIST *middlewaretList);

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PUBLIC FUNCTIONS /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
int mwGetInfo (MW_ID middleware, MW_INFO *info)
{
	int iReturnCode = DIAGLIB_OK;

	if(info == NULL)
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;
	}

	info->id=middleware;

	Soft_INFO softInfo;
	if(DIAGLIB_OK != (iReturnCode=softwareGetInfo(middleware,&softInfo)))
	{
		LOG_ERROR(L"softwareGetInfo failed");
		return iReturnCode;
	}

	info->Version=MW_VERSION_MAC;
	info->BuildNumber=0;
	info->DisplayName=softInfo.DisplayName;
	info->LabelVersion=softInfo.DisplayVersion;

	return iReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
int mwUninstall (MW_ID middleware)
{
	int iReturnCode = DIAGLIB_OK;

	return RETURN_LOG_ERROR(DIAGLIB_ERR_NOT_AVAILABLE);

	return iReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PRIVATE FUNCTIONS ////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
int mwFillList(MW_LIST *middlewaretList)
{
	int iReturnCode = DIAGLIB_OK;

	if(middlewaretList == NULL)
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;
	}

	middlewaretList->clear();

	MW_ID id;
	Soft_LIST softList;

	if( DIAGLIB_OK != (iReturnCode = softwareGetIDs(L"\\SPApplicationsDataType\\eID-Viewer", &softList)))
	{
		LOG_ERROR(L"softwareGetIDs failed");
	}
	else
	{
		Soft_LIST::const_iterator itrSoft;
		for(itrSoft=softList.begin();itrSoft!=softList.end();itrSoft++)
		{
			id=*itrSoft;
			middlewaretList->push_back(id);
		}
	}

	return iReturnCode;
}

