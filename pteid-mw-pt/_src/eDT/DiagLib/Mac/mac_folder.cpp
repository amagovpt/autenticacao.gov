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
#include <iostream>
#include "folder.h"
#include "file.h"

#include "error.h"
#include "log.h"

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PRIVATE FUNCTIONS DECLARATION ////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PUBLIC FUNCTIONS /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
int folderGetPath(const Folder_TYPE folder, Folder_ID *path, bool bInternalUse)
{
	int iReturnCode = DIAGLIB_OK;

	if(path==NULL)
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;
	}

	path->clear();

	switch(folder)
	{
	case FOLDER_WINDOWS:
		if(!bInternalUse) LOG_ERROR(L"Windows folder not available on mac");
		iReturnCode = DIAGLIB_ERR_NOT_AVAILABLE;
		break;
	case FOLDER_SYSTEM:
		path->assign(L"/usr/local/lib/");
		break;
	case FOLDER_HOME:
		path->assign(L"~/");
		break;
	case FOLDER_APP:
		path->assign(L"/Applications/Portugal Identity Card/");
		break;
	default:
		path->assign(L"/tmp/");
		break;
	}

	return iReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
int folderGetTempFileName(File_ID *file, bool bInternalUse)
{
	int iReturnCode = DIAGLIB_OK;

	if(file==NULL)
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;
	}

	file->clear();

	//Get the temp folder
	if(DIAGLIB_OK != (iReturnCode=folderGetPath(FOLDER_TEMP, file, bInternalUse)))
	{
        iReturnCode = DIAGLIB_ERR_INTERNAL;
    }
	else
	{
		File_ID tmp;
		int iTemp=0;
		wchar_t wzTempFile[15];
		bool exist = true;
		srand(time(NULL));

		for(int i=0;i<0xFF;i++)
		{
			iTemp = rand() % 0x1000000;
			if( 0 > swprintf(wzTempFile,15,L"%x.tmp",iTemp))
			{
				if(!bInternalUse) LOG_ERROR(L"sprintf failed\n");
			}
			tmp.assign(file->c_str());
			tmp.append(wzTempFile);

			if(DIAGLIB_OK != fileExists(tmp,&exist))
			{
				if(!bInternalUse) LOG_ERROR(L"fileExists failed\n");
			}
			if(!exist)
			{
				if(DIAGLIB_OK != fileCreate(tmp))
				{
					if(!bInternalUse) LOG_ERROR(L"fileCreate failed\n");
				}
				else
				{
					break;
				}
			}
		}

		if(!exist)
		{
			file->assign(tmp);
		}
		else
		{
			iReturnCode = DIAGLIB_ERR_INTERNAL;
		}
	}

	return iReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PRIVATE FUNCTIONS ////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
