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

#include "globmdrv.h"

#include "log.h"
#include "util.h"
#include "cache.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <Strsafe.h>
#include <direct.h>
#include <errno.h>

BOOL getCacheFilePath(char *filename_bin, char* Path, size_t Path_max)
{
	char ascii_filename[18*2];

	bin2AsciiHex(filename_bin, ascii_filename, 17);
	GetEnvironmentVariableA("LOCALAPPDATA", (LPSTR)Path, Path_max);
	
    strncat(Path, "\\pteidmdrv\\", 12);
	//Create cache directory if not existent
	if(_mkdir(Path) != 0)
	{
		if(errno == ENOENT) {
			//printf("Mkdir failed!\n");
			return FALSE;  /* Oops, giving up */

		}
	}

	//Append filename (Card serial number + FileID)
	strncat(Path, ascii_filename, strlen(ascii_filename));
	return TRUE;
}

#define WHERE "readFromCache"
BOOL readFromCache(const char *cache_path, char * Buf)
{
	
	struct _stat st;
	FILE * fp = NULL;

	LogTrace(LOGTYPE_INFO, WHERE ,"Opening file ", cache_path);
	if(_stat(cache_path, &st) != 0)
	{
		LogTrace(LOGTYPE_INFO, WHERE, "Cache Miss for file %s", cache_path);
		return FALSE;
	}
	LogTrace(LOGTYPE_INFO, WHERE, "Found cache file, size=%d",st.st_size);
	fp = fopen(cache_path, "rb");
	
	if ( fp != NULL )
	{
		if(fread(Buf, sizeof(char), st.st_size, fp) < st.st_size)
		{
			LogTrace(LOGTYPE_ERROR, WHERE, "Error reading cached file: errno=%d", errno);
		}

		fclose(fp);
		return TRUE;
	}
	return FALSE;

}
#undef WHERE
#define WHERE "CacheCertificate"
BOOL CacheCertificate(const char * Path, PBYTE data, int data_len)
{
	FILE *fp = NULL;
	
	LogTrace(LOGTYPE_INFO, WHERE ,"Opening cache file %s", Path);
	fp = fopen(Path, "wb");

	if ( fp != NULL )
	{
		fwrite(data, sizeof(char), data_len, fp);
		fclose(fp);
	}
	else
	{
		LogTrace(LOGTYPE_ERROR, WHERE, "Failed to open cache file: %s", Path);
		return FALSE;
	}

	return TRUE;

}
#undef WHERE