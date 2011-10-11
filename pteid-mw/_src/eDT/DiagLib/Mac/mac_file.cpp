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
#include "mac_helper.h"
#include <iostream>
#include <string.h>

#include "file.h"

#include "error.h"
#include "log.h"
#include "util.h"

#include "sys/stat.h"

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PRIVATE FUNCTIONS DECLARATION ////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
int fileGetVersion(const File_ID file, std::wstring *version);
int fileReadLink(const File_ID link, File_ID *source);
int fileGetSourceOfLink(const File_ID link, File_ID *source);

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PUBLIC FUNCTIONS /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
int fileGetInfo(const File_ID file, File_INFO *info)
{
	int iReturnCode = DIAGLIB_OK;

	if(info==NULL)
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;
	}

	info->id.assign(file);

	bool exist;

	//Check if the file exist
	if(DIAGLIB_OK != (iReturnCode=fileExists(file,&exist)))
	{
		return RETURN_LOG_ERROR(DIAGLIB_ERR_INTERNAL);
	}
	else
	{
		//If not exist return
		if(!exist)
		{
			return RETURN_LOG_ERROR(DIAGLIB_ERR_FILE_NOT_FOUND);
		}
	}

	//Get the size of the file
	struct _stat buff;
	if(0 != _wstat(file.c_str(),&buff))
	{
		return RETURN_LOG_ERROR(DIAGLIB_ERR_INTERNAL);
	}

	info->FileSize=buff.st_size;

	fileGetVersion(file, &info->FileVersion);
	info->ProductVersion.clear();

	return iReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
int fileDelete(const File_ID file)
{
	int iReturnCode = DIAGLIB_OK;

	bool exist;

	//Check if the file exist
	if(DIAGLIB_OK != (iReturnCode=fileExists(file,&exist)))
	{
		return RETURN_LOG_ERROR(DIAGLIB_ERR_INTERNAL);
	}
	else
	{
		//If not exist return
		if(!exist)
		{
			return RETURN_LOG_ERROR(DIAGLIB_ERR_FILE_NOT_FOUND);
		}
		else
		{
			if(0 != remove(string_From_wstring(file).c_str()))
			{
				//If error occur
				if(errno==ENOENT)
				{
					return RETURN_LOG_ERROR(DIAGLIB_ERR_FILE_NOT_FOUND);
				}
				else
				{
					LOG_LASTERROR(L"remove failed");
					iReturnCode = DIAGLIB_ERR_FILE_DELETE_FAILED;
				}
			}
		}
	}

	return iReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PRIVATE FUNCTIONS ////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
#define G_BUFFER_SIZE 1024
static char g_fileversion_buffer[G_BUFFER_SIZE];

int fileGetVersion(const File_ID file, std::wstring *version)
{
	int iReturnCode = DIAGLIB_OK;

	if(version==NULL)
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;
	}

	version->clear();

	File_ID source;

	if(DIAGLIB_OK != (iReturnCode = fileGetSourceOfLink(file, &source)))
	{
		return RETURN_LOG_ERROR(iReturnCode);
	}


	//If this is a path, we just take the last part (file name)
	std::vector<std::wstring> path;
	TokenizeW(source.c_str(),path,L"/");
	std::wstring referenceName;
	if(path.size() == 0)
	{
		referenceName.assign(source);
	}
	else
	{
		referenceName.assign(path[path.size()-1]);
	}

	std::vector<std::wstring> filepart;
	TokenizeW(referenceName.c_str(),filepart,L".");
	if(filepart.size() >= 3)
	{
		version->assign(filepart[1]);
	}
	if(filepart.size() >= 4)
	{
		version->append(L".");
		version->append(filepart[2]);
	}
	if(filepart.size() >= 5)
	{
		version->append(L".");
		version->append(filepart[3]);
	}

	return iReturnCode;

}

////////////////////////////////////////////////////////////////////////////////////////////////
int fileReadLink(const File_ID link, File_ID *source)
{
	int iReturnCode = DIAGLIB_OK;

	if(source==NULL)
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;
	}

	source->clear();

	FILE *			pF;
	size_t			bytesRead = 0;

	std::string command;
	command.assign("readlink \"");
	command.append(string_From_wstring(link));
	command.append("\"");

	pF = popen(command.c_str(), "r");
	if (pF == NULL )
	{
		LOG_LASTERROR1(L"popen '%s' failed", command.c_str());
		return RETURN_LOG_INTERNAL_ERROR;
	}

	// Read the stream into a memory buffer
	if(0 == (bytesRead = fread(g_fileversion_buffer, sizeof(char), G_BUFFER_SIZE, pF)))
	{
		pclose (pF);
		return DIAGLIB_ERR_FILE_NOT_FOUND;
	}
	// Close the stream
	pclose (pF);

	// add EOL to stream
	g_fileversion_buffer[bytesRead-1] = 0x00;

	source->assign(wstring_From_string(g_fileversion_buffer));

	return iReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
int fileGetSourceOfLink(const File_ID link, File_ID *source)
{
	int iReturnCode = DIAGLIB_OK;

	if(source==NULL)
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;
	}

	source->clear();

	bool exist=false;
	if(DIAGLIB_OK != (iReturnCode = fileExists(link,&exist)))
	{
		return RETURN_LOG_ERROR(DIAGLIB_ERR_INTERNAL);
	}
	else
	{
		//If not exist return
		if(!exist)
		{
			return RETURN_LOG_ERROR(DIAGLIB_ERR_FILE_NOT_FOUND);
		}
	}

	int err = DIAGLIB_OK;
	File_ID start = link;

	while(1)
	{
		if(DIAGLIB_OK != (err = fileReadLink(start,source)))
		{
			if(err == DIAGLIB_ERR_FILE_NOT_FOUND)
			{
				source->assign(start);
				break;
			}
			else
			{
				return RETURN_LOG_ERROR(err);
			}
		}
		else
		{
			start.assign(*source);
		}
	}

	if(DIAGLIB_OK != (iReturnCode = fileExists(*source,&exist)))
	{
		return RETURN_LOG_ERROR(DIAGLIB_ERR_INTERNAL);
	}
	else
	{
		//If not exist return
		if(!exist)
		{
			return RETURN_LOG_ERROR(DIAGLIB_ERR_FILE_NOT_FOUND);
		}
	}

	return iReturnCode;
}