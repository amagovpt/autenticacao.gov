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
#ifndef __DIAGLIB_FOLDER_H__
#define __DIAGLIB_FOLDER_H__

#include <string.h>
#include "file.h"

typedef std::wstring Folder_ID;

//////////////////////////////////////////

typedef enum e_Folder_TYPE
{
	FOLDER_WINDOWS,
	FOLDER_SYSTEM,
	FOLDER_TEMP,
	FOLDER_HOME,
	FOLDER_APP
} Folder_TYPE;

int folderGetPath(const Folder_TYPE folder, Folder_ID *path, bool bInternalUse=false);
int folderGetTempFileName(File_ID *file, bool bInternalUse=false);

#endif //__DIAGLIB_FOLDER_H__
