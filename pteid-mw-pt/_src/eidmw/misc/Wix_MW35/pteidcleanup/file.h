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
#pragma once

enum FolderType
{
	FOLDER_SYSTEM32,
	FOLDER_TEMP,
	FOLDER_APP,
	FOLDER_WOWSYS64
};

int GetFolderPath(FolderType folder,wchar_t *path,unsigned int pathsize);
int GetTempFileName(wchar_t *path,unsigned int pathsize);
int DeleteFile(FolderType folder, const wchar_t *file);