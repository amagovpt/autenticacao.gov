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
#ifndef __DIAGLIB_HW_H__
#define __DIAGLIB_HW_H__

#ifdef WIN32

#include <iostream>
#include <string.h>
#include <vector>

#include "report.h"

typedef struct t_Hardware_INFO
{
	std::wstring systemManufacturer;
	std::wstring systemProductName;
} Hardware_INFO;

int hardwareGetInfo(Hardware_INFO *info);
int hardwareReportInfo(Report_TYPE type, const Hardware_INFO *info);
int hardwareReport(Report_TYPE type);

#endif
#endif //__DIAGLIB_HW_H__