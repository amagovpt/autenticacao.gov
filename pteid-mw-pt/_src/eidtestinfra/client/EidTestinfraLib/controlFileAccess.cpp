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

#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <errno.h>

#include "Logger.h"
#include "util.h"
#include "controlFileAccess.h"

using std::string;
using namespace std;
namespace EidInfra
{
//! Function:    ControlFile
//! Description: constructor
//! Parameter:   none
//! Return:      none

ControlFile::ControlFile(const char *file, showType aType)
{
	strcpy_s((char *) filePath, 2047, file);
	m_type = aType;
	m_readers.push_back("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
	m_readers.push_back("<control>");
	string type = "";
	switch (aType) {
	case HIDE_VIRTUAL:
		type = "HIDE_VIRTUAL";
		break;

	case HIDEREAL:
		type = "HIDEREAL";
		break;

	case REAL_FIRST:
		type = "REAL_FIRST";
		break;

	case REAL_LAST:
		type = "REAL_LAST";
		break;

	default:
		printf("Bad showType (%d) passed to ControlFile, defaulting to REAL_LAST\n", aType);
		type = "REAL_LAST";
		break;
	}
	type = "<show>" + type + "</show>";
	m_readers.push_back(type);

	logInfo("Using config file \"%s\", visibility = \"%s\"\n", file, type.c_str());
}



ControlFile::~ControlFile()
{
}

//! only a debug function
void ControlFile::Add(const char *hardchipnr, const char *file)
{
	char buffer[128];

	m_readers.push_back("  <virtualcard>");
	sprintf_s(buffer, sizeof(buffer), "    <hardchipnr>%s</hardchipnr>", hardchipnr);
	m_readers.push_back(buffer);
	sprintf_s(buffer, sizeof(buffer), "    <file>%s</file>", file);
	m_readers.push_back(buffer);
	m_readers.push_back("  </virtualcard>");
}

//! only a debug function
void ControlFile::Show()
{
	int i;

	for (i = 0; i < (int) m_readers.size(); i++)
	{
		cout << m_readers[i] << endl;
	}

	cout << "</control>";
}

//! only a debug function
void ControlFile::Save(const char * fileName)
{
	errno_t err;
	FILE    * outFile;

	err = fopen_s(&outFile, fileName, "w");
	if (err != 0)
	{
		printf("Could not open file \"%s\" for writing", fileName);
		return;
	}

	strcpy_s((char *) filePath, 2048, fileName);

	int i;

	for (i = 0; i < (int) m_readers.size(); i++)
	{
		fprintf(outFile, "%s\n", m_readers[i].c_str());
	}

	fprintf(outFile, "</control>");
	fclose(outFile);
}

//! Function:    getVirtualFilePaths
//! Description: get file paths of virtual card of given hard chip number
//! Parameter:   hardchipnr = hard chip number
//! Return:      string containg paths

string ControlFile::GetVirtualFilePath(const char *hardchipnr)
{
	errno_t err;
	FILE    * inFile;

	string  result = "";

	err = fopen_s(&inFile, (char *) filePath, "r");
	if (err != 0)
	{
		logError("Could not open file \"%s\" for reading (err = %d)", filePath, err);
		return result;
	}

	char content[MAX_CONTENT + 1];

	if (!seekToContentElement(inFile, "hardchipnr", hardchipnr))
	{
		fclose(inFile);
		return result;
	}

	FindElement(inFile, "file", content, MAX_CONTENT);
	result = content;

	fclose(inFile);

	return result;
}

//! Function:    GetShowType
//! Description: get the ShowType for this card
//! Return:      ShowType for this card

showType ControlFile::GetShowType()
{
	errno_t err;
	FILE    * inFile;

	err = fopen_s(&inFile, (char *) filePath, "r");
	if (err != 0)
	{
		logError("Could not open file \"%s\" for reading (err = %d)", filePath, err);
		return NON_SHOW_TYPE;
	}

	char content[MAX_ELEMENT + 1];

	bool result = FindElement(inFile, "show", content, MAX_ELEMENT);

	fclose(inFile);
	if (result)
	{
		string s = content;     //easier to use!
		if (s == "HIDE_VIRTUAL") return HIDE_VIRTUAL;
		else if (s == "HIDEREAL") return HIDEREAL;
		else if (s == "REAL_FIRST") return REAL_FIRST;
		else return REAL_LAST;
	}
	else
	{
		logError("Element <show> not found in control file \"%s\"", filePath);
		return REAL_LAST;
	}
}
}
