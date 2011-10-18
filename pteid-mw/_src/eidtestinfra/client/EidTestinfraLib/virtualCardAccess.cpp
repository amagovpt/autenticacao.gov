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

#include "virtualCardAccess.h"
#include "util.h"
#include "Logger.h"

namespace EidInfra
{
VirtualCard::VirtualCard(void)
{
}

//! Function:    VirtualCard.
//! Description: contructor updates all internal data.
//! Parameter:   path = path where the virtual card resides.
//! Return:      none.

VirtualCard::VirtualCard(const char *path)
{
	errno_t err;

	err = fopen_s(&m_file, path, "r");
	if (err != 0)
	{
		//TO_DO should be an exception
		logError("Could not open virtual card file \"%s\" for reading (err = %d)\n", path, err);
		return;
	}

	char content[MAX_CONTENT + 1];

	FindElement(m_file, "type", content, MAX_CONTENT);
	strcpy_s(m_type, 31, content);

	FindElement(m_file, "testCardVersion", content, MAX_CONTENT);
	strcpy_s(m_testCardVersion, 15, content);

	FindElement(m_file, "friendlyName", content, MAX_CONTENT);
	strcpy_s(m_friendlyName, 31, content);

	FindElement(m_file, "chipNumber", content, MAX_CONTENT);
	strcpy_s(m_chipNr, 63, content);

	FindElement(m_file, "physicalCardChipNumber", content, MAX_CONTENT);
	strcpy_s(m_devCardChipNr, 63, content);

	FindElement(m_file, "versionNumber", content, MAX_CONTENT);
	strcpy_s(m_versionNumbers, 31, content);

	FindElement(m_file, "atr", content, MAX_CONTENT);
	strcpy_s(m_atr, 31, content);

	char id[MAX_CONTENT];
	char content2[MAX_CONTENT + 1];
	char dictionaryLine[MAX_CONTENT + 1];

	while (FindElement(m_file, "id", content, MAX_CONTENT))
	{
		strcpy_s(id, MAX_CONTENT, content);
		FindElement(m_file, "content", content2, MAX_CONTENT);
		sprintf_s(dictionaryLine, sizeof(dictionaryLine), "%s=%s", id, content2);
		m_contents.push_back(dictionaryLine);
	}
	fclose(m_file);
}

VirtualCard::~VirtualCard(void)
{
}

//! Function: GetAllData
//! Description: Get all the data on a virtual card into a vector
//!              Only elements with contents associated to them are returned
//! Parameter:   path = path where the virtual card resides
//! Return:      A vector of all Xml elements in the form ELEMENT_NAME:CONTENT

std::vector<string> VirtualCard::GetAllData(const char *path)
{
	errno_t              err;
	FILE                 * inFile;

	std:: vector<string> data;

	err = fopen_s(&inFile, path, "r");
	if (err != 0)
	{
		logError("Could not open virtual card file \"%s\" for reading all data (err = %d)\n", path, err);
		return data;
	}

	char element[MAX_ELEMENT + 1];
	char content[MAX_CONTENT + 1];
	char temp[MAX_CONTENT + 1];

	while (GetNextElement(inFile, element, MAX_ELEMENT, content, MAX_CONTENT))
	{
		if (strlen(content))
		{
			sprintf_s(temp, sizeof(temp), "%s:%s", element, content);
			data.push_back(temp);
		}
	}
	return data;
}

//! Function: GetAllData2
//! Description: Get all the files portion of virtual card as an xml vector
//!              Only elements with contents associated to them are returned
//! Parameter:   path = path where the virtual card resides
//! Return:      A  vetor of all the xml lines beginning with first <file> line
//               return values are in the form <ELEMENT_NAME>,<CONTENT>

std::vector<string> VirtualCard::GetAllFiles2(const char *path)
{
	errno_t              err;
	FILE                 * inFile;

	std:: vector<string> data;

	err = fopen_s(&inFile, path, "r");
	if (err != 0)
	{
		logError("Could not open virtual cardfile \"%s\" for reading all files (err = %d)\n", path, err);
		return data;
	}

	char result[MAX_CONTENT + 1];
	char content[MAX_CONTENT + 1];
	char id[MAX_CONTENT + 1];

	while (FindElement(inFile, "file", content, MAX_CONTENT))
	{
		FindElement(inFile, "id", id, MAX_CONTENT);
		FindElement(inFile, "content", content, MAX_CONTENT);
		sprintf_s(result, sizeof(result), "%s,%s", id, content);
		data.push_back(result);
	}

	fclose(inFile);

	return data;
}

//! Function: GetData
//! Description: Get data on a virtual card referenced by passed in id
//! Parameter: id = required id
//! Parameter: result the contents of id
//! Parameter: maxDataLength Maximum acceptable data length

void VirtualCard::GetData(const char *id, char * result, size_t maxDataLength)
{
	char *buffer = new char[maxDataLength + 1];
	char *ptr;
	*result = '\0';

	for (int i = 0; i < (int) m_contents.size(); i++)
	{
		strcpy_s(buffer, maxDataLength, m_contents[i].c_str());
		ptr    = strchr(buffer, '=');
		*ptr++ = 0;
		if (strcmp(id, buffer) == 0)
		{
			if (strlen(ptr) <= maxDataLength)
				strcpy_s(result, maxDataLength, ptr);
			delete[] buffer;
			return;
		}
	}
	delete buffer;
}
}
