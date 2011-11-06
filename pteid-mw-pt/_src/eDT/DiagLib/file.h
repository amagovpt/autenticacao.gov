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
#ifndef __DIAGLIB_FILE_H__
#define __DIAGLIB_FILE_H__

#include <iostream>
#include <string.h>
#include <vector>

#include "report.h"

typedef std::wstring File_ID;
typedef std::vector<File_ID> File_LIST;

typedef struct t_File_INFO
{
	File_ID			id;
    std::wstring	ProductVersion;
    std::wstring	FileVersion;
    size_t			FileSize;
	unsigned char	md5sum[16];
} File_INFO;

int fileExists(const File_ID file, bool *exist);
int fileGetInfo(const File_ID file, File_INFO *info);

int fileCreate(const File_ID file);
int fileDelete(const File_ID file);

int fileReportInfo(Report_TYPE type, const File_INFO &info);
int fileReportList(Report_TYPE type, const File_LIST &fileList, const wchar_t *TitleIn);

#endif //__DIAGLIB_FILE_H__

