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
#ifndef __CHECKSUM_MODULE_H__
#define __CHECKSUM_MODULE_H__

#include "MD5Sum.h"

int checkSumReportInfo(Report_TYPE type,ModuleName name,ProcNameSet names);
void checkSumContributeInfo(ModuleName name,ProcNameSet names);
int checkSumReportList(Report_TYPE type, const Proc_LIST& processList, const wchar_t *TitleIn=NULL);

#endif
