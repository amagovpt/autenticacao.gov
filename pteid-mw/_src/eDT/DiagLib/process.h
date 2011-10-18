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
#ifndef __DIAGLIB_PROCESS_H__
#define __DIAGLIB_PROCESS_H__

#include <iostream>
#include <string.h>
#include <string>
#include <vector>
#include <set>
#include <map>
#include "report.h"
#include "module.h"

typedef unsigned long						Proc_ID;
typedef std::wstring						Proc_NAME;
typedef std::set<Proc_NAME>					ProcNameSet;
typedef std::vector<Proc_ID>				Proc_LIST;
typedef std::wstring						ModuleName;
typedef std::set<ModuleName>				ModuleSet;
typedef ModuleSet::const_iterator			ModuleIterator;

typedef struct t_Proc_INFO
{
	Proc_ID			id;
    Proc_NAME		Name;
	std::wstring	Path;
	std::wstring	FullPath;
	ModuleSet		modulesLoaded;
} Proc_INFO;

typedef std::wstring Lib_ID;

int processGetList(Proc_LIST *processList);
int processGetIDs(Proc_NAME process, Proc_LIST *processList);
int processUsingLibrary(Lib_ID library, Proc_LIST *processlist);
int processGetInfo(Proc_ID process, Proc_INFO *info);

int processKill(Proc_ID process);
int processKillByName(Proc_NAME process);
int processStart(Proc_NAME process, Proc_ID *id, int waitTimeSecs = 0, int *exitCode = NULL);
int processStartAsAdmin(Proc_NAME process, Proc_ID *id, int waitTimeSecs = 0, int *exitCode = NULL);
int processWait(Proc_ID process, int *exitCode, int waitTimeSecs = 0);

int processReportInfo(Report_TYPE type, const Proc_INFO &info);
int processReportList(Report_TYPE type, const Proc_LIST &processList, const wchar_t *Title=NULL);

#endif //__DIAGLIB_PROCESS_H__
