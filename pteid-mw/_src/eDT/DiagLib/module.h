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
#ifndef __DIAGLIB_MODULE_H__
#define __DIAGLIB_MODULE_H__

#include <iostream>
#include <string.h>
#include <string>
#include <vector>
#include <set>
#include <map>
#include "report.h"
#include "process.h"

typedef std::wstring						ModuleName;
typedef std::wstring						ModulePath;
typedef std::wstring						Proc_NAME;
typedef std::set<Proc_NAME>					ProcNameSet;
typedef std::map<ModuleName,ProcNameSet>	ModuleMap;

typedef unsigned long						Proc_ID;
typedef std::vector<Proc_ID>				Proc_LIST;

int moduleReportInfo(Report_TYPE type,ModuleName name,ProcNameSet names);
void moduleContributeInfo(ModuleName name,ProcNameSet names);
int moduleReportList(Report_TYPE type, const Proc_LIST& processList, const wchar_t *TitleIn=NULL);

#endif
