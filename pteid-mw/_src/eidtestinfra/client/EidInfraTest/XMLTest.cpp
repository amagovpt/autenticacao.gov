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

#include <stdio.h>
#include <memory>
#include "util.h"
#include "xml.h"
#include "UnitTest++.h"

using namespace EidInfra;
using namespace std;
extern char * createRelativePath(const char * fileName);

const char * FILE_NAME = "TestGetNextElement.xml";

bool writeFile(const char *filename, const void *contents, size_t contentsLen);

TEST(GetNextElement)
{
	const char *conts =
		"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
		"<control>\n"
		"\t<One>Data 1</One>\n"
		"\t<Two>Data 2</Two>\n"
		"\t<Three>Data 3</Three>\n"
		"</control>";

	if (writeFile(FILE_NAME, conts, strlen(conts)))
	{
		Xml     xml;
		char element[MAX_ELEMENT + 1];
		char content[MAX_CONTENT + 1];

		FILE *inFile = fopen(FILE_NAME, "r");

		CHECK(xml.GetNextElement(inFile, element, MAX_ELEMENT, content, MAX_CONTENT));
		CHECK_EQUAL("?xml version=\"1.0\" encoding=\"UTF-8\"?", element);

		CHECK(xml.GetNextElement(inFile, element, MAX_ELEMENT, content, MAX_CONTENT));
		CHECK_EQUAL("control", element);

		CHECK(xml.GetNextElement(inFile, element, MAX_ELEMENT, content, MAX_CONTENT));
		CHECK_EQUAL("One", element);
		CHECK_EQUAL("Data 1", content);

		CHECK(xml.GetNextElement(inFile, element, MAX_ELEMENT, content, MAX_CONTENT));
		CHECK_EQUAL("Two", element);
		CHECK_EQUAL("Data 2", content);

		CHECK(xml.GetNextElement(inFile, element, MAX_ELEMENT, content, MAX_CONTENT));
		CHECK_EQUAL("Three", element);
		CHECK_EQUAL("Data 3", content);

		CHECK(xml.GetNextElement(inFile, element, MAX_ELEMENT, content, MAX_CONTENT));
		CHECK_EQUAL("/control", element);

		fclose(inFile);
	}
}

TEST(FindElement)
{
	const char *conts =
		"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
		"<control>\n"
		"\t<One>Data 1</One>\n"
		"\t<Two>Data 2</Two>\n"
		"\t<Three>Data 3</Three>\n"
		"</control>";

	if (writeFile(FILE_NAME, conts, strlen(conts)))
	{
		Xml     xml;
		char content[MAX_CONTENT + 1];

		FILE *inFile = fopen(FILE_NAME, "r");

		CHECK(xml.FindElement(inFile, "Two", content, MAX_CONTENT));
		CHECK_EQUAL("Data 2", content);

		CHECK(xml.FindElement(inFile, "Three", content, MAX_CONTENT));
		CHECK_EQUAL("Data 3", content);

		fclose(inFile);
	}
}

TEST(seekToContentElement)
{
	const char *conts =
		"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
		"<control>\n"
		"\t<One>Data 1</One>\n"
		"\t<Two>Data 2</Two>\n"
		"\t<Three>Data 3</Three>\n"
		"</control>";

	if (writeFile(FILE_NAME, conts, strlen(conts)))
	{
		Xml     xml;
		char content[MAX_CONTENT + 1];
		char element[MAX_ELEMENT + 1];

		FILE *inFile = fopen(FILE_NAME, "r");

		CHECK(xml.seekToContentElement(inFile, "One", "Data 1"));

		CHECK(xml.GetNextElement(inFile, element, MAX_ELEMENT, content, MAX_CONTENT));

		CHECK_EQUAL("Data 2", content);

		fclose(inFile);
	}
}

TEST(BigData)
{
	char *conts = new char[10000];

	strcpy(conts, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
	strcat(conts, "<control>\n");
	strcat(conts, "\t<One>");
	size_t len = strlen(conts);
	memset(conts + len, 9500, 'a');
	conts[len + 9500] = '\0';
	strcat(conts, "</One>\n</control>");

	if (writeFile(FILE_NAME, conts, strlen(conts)))
	{
		Xml     xml;
		int k1 = 123456;
		char content[MAX_CONTENT + 2];
		int k2 = 234567;
		char element[MAX_ELEMENT + 1];

		FILE *inFile = fopen(FILE_NAME, "r");

		CHECK(xml.GetNextElement(inFile, element, MAX_ELEMENT, content, MAX_CONTENT));
		CHECK_EQUAL("?xml version=\"1.0\" encoding=\"UTF-8\"?", element);

		CHECK(xml.GetNextElement(inFile, element, MAX_ELEMENT, content, MAX_CONTENT));
		CHECK_EQUAL("control", element);

		content[MAX_CONTENT + 1] = 'x';
		CHECK(xml.GetNextElement(inFile, element, MAX_ELEMENT, content, MAX_CONTENT));
		CHECK_EQUAL('x', content[MAX_CONTENT + 1]);
		CHECK_EQUAL(123456, k1);
		CHECK_EQUAL(234567, k2);

		fclose(inFile);

		//////////////////////////////////////

		inFile = fopen(FILE_NAME, "r");

		CHECK(xml.GetNextElement(inFile, element, MAX_ELEMENT, content, MAX_CONTENT));
		CHECK_EQUAL("?xml version=\"1.0\" encoding=\"UTF-8\"?", element);

		CHECK(xml.GetNextElement(inFile, element, MAX_ELEMENT, content, MAX_CONTENT));
		CHECK_EQUAL("control", element);

		char contentBig[9502];
		contentBig[9501] = 'x';
		CHECK(xml.GetNextElement(inFile, element, MAX_ELEMENT, contentBig, 9500));
		CHECK_EQUAL('x', contentBig[9501]);

		fclose(inFile);
	}

	/////////////////////////////////////////////////////

	strcpy(conts, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
	strcat(conts, "<control>\n");
	strcat(conts, "\t<");
	len = strlen(conts);
	memset(conts + len, 9500, 'a');
	conts[len + 9500] = '\0';
	strcat(conts, ">Data 1</One>\n</control>");

	if (writeFile(FILE_NAME, conts, strlen(conts)))
	{
		Xml     xml;
		char content[MAX_CONTENT + 1];
		int k1 = 123456;
		char element[MAX_ELEMENT + 2];
		int k2 = 234567;

		FILE *inFile = fopen(FILE_NAME, "r");

		CHECK(xml.GetNextElement(inFile, element, MAX_ELEMENT, content, MAX_CONTENT));
		CHECK_EQUAL("?xml version=\"1.0\" encoding=\"UTF-8\"?", element);

		CHECK(xml.GetNextElement(inFile, element, MAX_ELEMENT, content, MAX_CONTENT));
		CHECK_EQUAL("control", element);

		element[MAX_ELEMENT + 1] = 'x';
		CHECK(!xml.GetNextElement(inFile, element, MAX_ELEMENT, content, MAX_CONTENT));
		CHECK_EQUAL('\0', content[0]);
		CHECK_EQUAL('\0', element[0]);
		CHECK_EQUAL('x', element[MAX_ELEMENT + 1]);
		CHECK_EQUAL(123456, k1);
		CHECK_EQUAL(234567, k2);
	}
}
