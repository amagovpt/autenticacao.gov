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
#ifndef __DIAGLIB_SERVICES_H__
#define __DIAGLIB_SERVICES_H__

#include <string.h>
#include <vector>

#include "report.h"
#include "process.h"

typedef std::wstring Service_ID;
typedef std::vector<Service_ID> Service_LIST;
typedef std::wstring Service_NAME;

typedef struct t_Service_INFO {
	Service_ID id;
    Service_NAME DisplayName;
	int Status;
	std::wstring StatusLabel;
	Proc_ID ProcessId;
} Service_INFO;

int serviceGetList (Service_LIST *serviceList);
int serviceGetIDs (Service_NAME service, Service_LIST *serviceList);
int serviceGetInfo (Service_ID service, Service_INFO *info);

int serviceStart (Service_ID service, unsigned long lTimeout=60);
int serviceStop (Service_ID service, unsigned long lTimeout=60);
int serviceRemove (Service_ID service);

void serviceContributeInfo(const Service_INFO &info);
int serviceReportInfo(Report_TYPE type, const Service_INFO &info);
int serviceReportList(Report_TYPE type, const Service_LIST &serviceList, const wchar_t *Title=NULL);

#endif //__DIAGLIB_SERVICES_H__
