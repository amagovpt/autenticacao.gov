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

#include <string>
#include "Control.h"
#include <sys/stat.h>
#include  <stdlib.h>
#include <stdio.h>
#include <util.h>
#include "Logger.h"

using namespace eidmw::pcscproxy;

Control::Control()
{
	EidInfra::CAutoMutex autoMutex(&controlMutex);

#ifdef WIN32
	size_t len = sizeof(filePath);
	if (0 != getenv_s(&len, filePath, len, "ALLUSERSPROFILE"))
	{
		// if env variable not found then hard code the path.
		strcpy_s(filePath, sizeof(filePath), "C:\\Documents and Settings\\All Users");
	}

	strcat_s(filePath, sizeof(filePath), "\\PTeidTestCtrl.xml");
#else
	strcpy_s(filePath, sizeof(filePath), "/usr/share/PTeidTestCtrl.xml");
#endif

	fileBuffer = NULL;
	GetFileSize(filePath);
	getVisibility();
}

Control::~Control()
{
	if (fileBuffer)
		delete[] fileBuffer;
}


void Control::getVisibility()
{
	itsListReaderOrder = REAL_LAST;

	errno_t err;
	FILE    * inFile;
	err = fopen_s(&inFile, (char *) filePath, "r");
	if (err != 0)
	{
		logError("Could not open control file %s for reading (err = %d)\n", filePath, err);
	}
	else
	{
		char content[MAX_CONTENT + 1];

		if (fileSize)
		{
			fileBuffer[0] = '\0';

			while (fgets(content, MAX_CONTENT, inFile))
			{
				strcat_s(fileBuffer, fileSize + 1, content);
			}
			fclose(inFile);
			bool result = true;

			char * ptr = strstr(fileBuffer, "<show>");
			if (!ptr)
			{
				result = false;
			}
			else
			{
				ptr = strchr(ptr, '>');
				if (!ptr)
				{
					result = false;
				}
			}

			if (result)
			{
				ptr++;
				char   *next_token;
				ptr = strtok_s(ptr, "<", &next_token);
				if (ptr == NULL)
					logError("Control file parsing: no </visibility>");
				else
				{
					string s = ptr; //easier to use!
					if (s == "HIDE_VIRTUAL")
						itsListReaderOrder = HIDE_VIRTUAL;
					else if (s == "HIDE_REAL")
						itsListReaderOrder = HIDE_REAL;
					else if (s == "REAL_FIRST")
						itsListReaderOrder = REAL_FIRST;
					else
						itsListReaderOrder = REAL_LAST;

					logInfo("Using control file \"%s\", visibility = %s\n", filePath, s.c_str());
				}
			}
		}
	}
}

Control::Control(std::string file)
{
	EidInfra::CAutoMutex autoMutex(&controlMutex);

	if (file.size() >= sizeof(filePath))
		filePath[0] = '\0';
	else
		strcpy_s(filePath, sizeof(filePath), file.c_str());

	fileBuffer = NULL;

	GetFileSize((char *) file.c_str());

	getVisibility();
}

//! Function:    getVirtualFilePaths
//! Description: get file paths of virtual card of given hard chip number as an hex-ascii string.
//! Parameter:   hardchipnr = hard chip number passed in as a Hex-Ascii string (no spaces + all upper-case)
//! Return:      string containg paths

string Control::GetVirtualFilePath(const char *hardchipnr)
{
	EidInfra::CAutoMutex autoMutex(&controlMutex);

	errno_t              err;
	FILE                 * inFile;

	string               result = "";

	err = fopen_s(&inFile, (char *) filePath, "r");

	if (err != 0)
	{
		logError("Could not open control file %s for reading virtual file path (err = %d)\n", filePath, err);
		return result;
	}

	char content[MAX_CONTENT + 1];

	fileBuffer[0] = '\0';

	while (fgets(content, MAX_CONTENT, inFile))
	{
		strcat_s(fileBuffer, fileSize + 1, content);
	}

	char *ptr = strstr(fileBuffer, hardchipnr);
	if (!ptr)
	{
		fclose(inFile);
		return result;
	}

	ptr = strstr(ptr, "<file>");
	if (!ptr)
	{
		fclose(inFile);
		return result;
	}

	ptr = strchr(ptr, '>');
	if (!ptr)
	{
		fclose(inFile);
		return result;
	}

	ptr++;
	char *next_token;
	ptr    = strtok_s(ptr, "<", &next_token);
	result = ptr;
	fclose(inFile);
	return result;
}

//! Function:    getVirtualFilePaths
//! Description: get file paths of virtual card of given hard chip number as a binary byte array.
//! Parameter:   hardchipnr = hard chip number passed in as a binary array
//! Return:      string containg paths

std::string Control::getSoftCardFileLocation(const BYTE* const chipNr) const
{
	char asciiHexChipNr[33];

	for (int i = 0; i < 16; i++)
	{
		sprintf_s(&asciiHexChipNr[i * 2], 3, "%02X", (BYTE) chipNr[i]); // build an ascii string
	}

	errno_t err;
	FILE    * inFile;

	string  result = "";

	err = fopen_s(&inFile, (char *) filePath, "r");

	if (err != 0)
	{
		logError("Could not open control file %s for reading soft card file location (err = %d)\n", filePath, err);
		return result;
	}

	char content[MAX_CONTENT + 1];

	fileBuffer[0] = '\0';

	while (fgets(content, MAX_CONTENT, inFile))
	{
		strcat_s(fileBuffer, fileSize + 1, content);
	}

	char *ptr = strstr(fileBuffer, asciiHexChipNr);
	if (!ptr)
	{
		fclose(inFile);
		return result;
	}

	ptr = strstr(ptr, "<file>");
	if (!ptr)
	{
		fclose(inFile);
		return result;
	}

	ptr = strchr(ptr, '>');
	if (!ptr)
	{
		fclose(inFile);
		return result;
	}

	ptr++;
	char *next_token;
	ptr    = strtok_s(ptr, "<", &next_token);
	result = ptr;
	fclose(inFile);
	return result;
}

ListReaderOrder Control::getListReaderOrder()
{
	getVisibility();

	return itsListReaderOrder;
}

void Control::GetFileSize(const char * filePath)
{
	EidInfra::CAutoMutex autoMutex(&controlMutex);

	FILE                 *inFile;
	errno_t              err;
	fileSize = 0;
	err      = fopen_s(&inFile, (char *) filePath, "r");
	if (err != 0)
	{
		logError("Could not open control file %s for reading (err = %d)\n", filePath, err);
	}
	else
	{
		fseek(inFile, 0L, SEEK_END);
		// In case the file grows while we are working with it.
		// TODO in some future version: implement something better
		fileSize = ftell(inFile) + 20000;
		fclose(inFile);
	}

	if (fileBuffer)
		delete[] fileBuffer;
	fileBuffer = new char[fileSize + 1];
}


