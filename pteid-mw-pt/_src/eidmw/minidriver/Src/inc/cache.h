/*-****************************************************************************

 * Copyright (C) 2011 André Guerreiro - <aguerreiro1985@gmail.com>
 *
 * Licensed under the EUPL V.1.2

****************************************************************************-*/

/* ****************************************************************************

* Portuguese eID Middleware Project.
* Copyright (C) 2011 Caixa Magica Software
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
/****************************************************************************************************/

#ifndef __CACHE__H__
#define __CACHE__H__

extern BOOL	CacheCertificate(const char * Path, PBYTE data, int data_len);
extern BOOL getCacheFilePath(char *filename_bin, char* path, DWORD path_len);
extern BOOL readFromCache(const char *cache_path, char * Buf);

#endif