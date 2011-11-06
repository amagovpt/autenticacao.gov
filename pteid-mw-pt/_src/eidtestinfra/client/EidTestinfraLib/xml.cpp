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
#include <iostream>
#include <vector>
#include <string>

#include "util.h"
#include "xml.h"

using namespace std;

namespace EidInfra
{
Xml::Xml(void)
{
}

Xml::~Xml(void)
{
}

//! Function:    GetNextElement
//! Description: Gets the contents and data of the next sequential element
//! Parameter:   file             => handle for xml file. The record pointer will return changed
//! Parameter:   element          => The found element's name
//! Parameter:   maxElementLength =>  Maximum acceptable element length
//! Parameter:   content          => The contents of found element
//! Parameter:   maxContentLength => Maximum acceptable content length
//! Return:      bool             => true if an element was found

bool Xml::GetNextElement(FILE * file, char *element, size_t maxElementLength, char *content, size_t maxContentLength) const
{
	if (file == NULL || element == NULL || content == NULL || maxElementLength <= 1 || maxContentLength <= 1)
		return false;

	char *buffer = new char[maxContentLength + 1];
	*element = 0;
	*content = 0;
	char *next_token;

	if (fgets(buffer, (int) maxContentLength, file) == NULL)
	{
		delete[] buffer;
		return false;
	}

	//remove carriage return and line feeds;
	strtok_s(buffer, "\n\r", &next_token);

	// get the element name
	char *ptr = strchr(buffer, '<');
	if (!ptr)
	{
		delete[] buffer;
		return false;
	}

	ptr = strtok_s(++ptr, ">", &next_token);
	if (!ptr)
	{
		delete[] buffer;
		return false;
	}

	if (strlen(ptr) >= maxElementLength)
	{
		delete[] buffer;
		return false;
	}

	strcpy_s(element, maxElementLength, ptr);
	ptr += (strlen(ptr) + 1);

	// get element terminator
	ptr = strtok_s(NULL, "<", &next_token);
	if (next_token && *next_token) // if '<' exists then content is valid
	{
		strtok_s(ptr, "\n\r", &next_token);
		strcpy_s(content, maxContentLength, ptr);
	}

	delete[] buffer;
	return true;
}

//! Function:    FindElement
//! Description: Find the next available element matching passed in value together with contents
//! Parameter:   file             => handle to xml file, the record pointer will return altered.
//! Parameter:   element          => required element.
//! Parameter:   content          => will contain the contents if found xml element has content.
//! Parameter:   maxContentLength => Maximum acceptable content length.
//! Return:      bool             => true if successfully found matching element.

bool Xml::FindElement(FILE * file, const char *element, char *content, size_t maxContentLength) const
{
	if (file == NULL || element == NULL || maxContentLength <= 1)
		return false;

	bool result;
	char element2[MAX_ELEMENT + 1];

	do
	{
		result = GetNextElement(file, element2, MAX_ELEMENT, content, maxContentLength);
		if (*element2 == 0) // see if not even an element maybe because EOF
		{
			return false;
		}
	} while (result && strcmp(element, element2) != 0);

	return result;
}

//! Function:    seekToContentElement
//! Description: Find the next element that has matching element name and content.
//!              Used to position pointer at known position (element & content ) but
//!              when subsequent elements/contents are of interest.
//! Parameter:   file    => handle to xml file, the record pointer will return altered.
//! Parameter:   element => required element.
//! Parameter:   content => required content.
//! Return:      bool    => true if successfully found matching element/content pair.

bool Xml::seekToContentElement(FILE * file, const char *element, const char *content) const
{
	if (file == NULL || element == NULL || content == NULL)
		return false;

	bool result;

	char content2[MAX_CONTENT + 1];

	do
	{
		result = FindElement(file, element, content2, MAX_CONTENT);
	} while (result && strcmp(content, content2) != 0);

	return result;
}
}
