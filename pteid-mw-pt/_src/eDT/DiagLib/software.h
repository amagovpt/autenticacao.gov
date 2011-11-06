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
#ifndef __DIAGLIB_SOFTWARE_H__
#define __DIAGLIB_SOFTWARE_H__

#include <iostream>
#include <string.h>
#include <vector>

#include "report.h"

typedef enum e_Soft_TYPE
{
	PER_USER_SOFT_TYPE,
	PER_MACHINE_SOFT_TYPE
} Soft_TYPE;

typedef std::wstring Soft_GUID;

typedef struct t_Soft_ID {
	Soft_GUID Guid;
	Soft_TYPE Type;
} Soft_ID;

typedef std::vector<Soft_ID> Soft_LIST;

typedef struct t_Soft_INFO {
	Soft_ID id;
	std::wstring DisplayName;
	std::wstring DisplayVersion;
	std::wstring UninstallString;
	std::wstring Publisher;
	std::wstring InstallLocation;
	int Version;
} Soft_INFO;

int softwareGetList(Soft_LIST *softList);
int softwareGetIDs(Soft_GUID software, Soft_LIST *softList);
int softwareGetInfo(Soft_ID software, Soft_INFO *info);

int softwareUninstall(Soft_ID software);

int softwareReportInfo(Report_TYPE type, const Soft_INFO &info);
int softwareReportList(Report_TYPE type, const Soft_LIST &softwareList, const wchar_t *Title=NULL);

#endif //__DIAGLIB_SOFTWARE_H__

