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
#ifndef __BWC_FILE_UTIL_H__
#define __BWC_FILE_UTIL_H__

#include <stdio.h>

#define SUFFIX_OLD "old"
#define SUFFIX_NEW "new"

FILE *FileOpen(const char *name, const char *folder, int bVerify, int ocsp, int crl);
void FileClose(FILE *f);
int FileVerify(const char *name, const char *folder, int bVerify, int ocsp, int crl);


#endif //__BWC_FILE_UTIL_H__
