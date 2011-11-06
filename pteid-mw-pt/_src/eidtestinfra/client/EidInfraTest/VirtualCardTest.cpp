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
#include "Control.h"
#include "virtualCardAccess.h"
#include "UnitTest++.h"

using namespace EidInfra;
using namespace std;
using namespace eidmw::pcscproxy;

static const BYTE StephensCard[16] = { 0x53, 0x4C, 0x49, 0x4E, 0x33, 0x66, 0x00, 0x29, 0x6C, 0xFF, 0x23, 0x2C, 0xF7, 0x13, 0x10, 0x30 };
static const BYTE RogiersCard[16] = { 0x53, 0x4C, 0x49, 0x4E, 0x33, 0x66, 0x00, 0x29, 0x6C, 0xFF, 0x27, 0x0D, 0xF4, 0x01, 0x13, 0x18 };
static const BYTE FloresCard[16] = { 0x53, 0x4C, 0x49, 0x4E, 0x33, 0x66, 0x00, 0x29, 0x6C, 0xFF, 0x26, 0x23, 0x68, 0x19, 0x2F, 0x16 };

extern char * createRelativePath(const char * fileName);
bool TestTags(const char * fileName);

TEST(Properties)
{
#ifdef _WIN32
	Control ctl(createRelativePath("_DocsInternal\\PteidTestCtrl.xml"));
#else
	Control ctl(createRelativePath("_DocsInternal/PteidTestCtrl.xml"));
#endif

	string      Stephen = ctl.getSoftCardFileLocation(StephensCard);
	VirtualCard stephen(Stephen.c_str());
	CHECK(TestTags(Stephen.c_str()));

	string      Rogier = ctl.getSoftCardFileLocation(RogiersCard);
	VirtualCard rogier(Rogier.c_str());
	CHECK(TestTags(Rogier.c_str()));

	string      Flores = ctl.getSoftCardFileLocation(FloresCard);
	VirtualCard flores(Flores.c_str());
	CHECK(TestTags(Flores.c_str()));

	CHECK_EQUAL("pteid", stephen.GetType());
	CHECK_EQUAL("1", stephen.GetTestCardVersion());
	CHECK_EQUAL("Virtual Stephen", stephen.GetFriendlyName());
	CHECK_EQUAL("534C494E336600296CFF232CF7131031", stephen.GetChipnr());
	CHECK_EQUAL("534C494E336600296CFF232CF7131030", stephen.GetDevCardChipNr());
	CHECK_EQUAL("A503010101110002000101", stephen.GetVersionnumbers());
	CHECK_EQUAL("3B9813400AA503010101AD1311", stephen.GetAtr());

	CHECK_EQUAL("pteid", rogier.GetType());
	CHECK_EQUAL("1", rogier.GetTestCardVersion());
	CHECK_EQUAL("Virtual Rogier", rogier.GetFriendlyName());
	CHECK_EQUAL("534C494E336600296CFF270DF4011319", rogier.GetChipnr());
	CHECK_EQUAL("534C494E336600296CFF270DF4011318", rogier.GetDevCardChipNr());
	CHECK_EQUAL("A503010101110002000101", rogier.GetVersionnumbers());
	CHECK_EQUAL("3B9813400AA503010101AD1311", rogier.GetAtr());


	char result[8192 * 2];
	stephen.GetData("3F00DF005031", result, MAX_CONTENT);
	CHECK_EQUAL("A00A300804063F00DF005035A40A300804063F00DF005037A80A300804063F00DF005034", result);

	stephen.GetData("3F00DF005035", result, MAX_CONTENT);
	CHECK_EQUAL("303A30170C0E41757468656E7469636174696F6E030206C0040101300F04010203020520030203B80202008"
		"2A10E300C300604043F00DF0002020400303930150C095369676E6174757265030206C00401010201013010"
		"0401030303060040030203B802020083A10E300C300604043F00DF0002020400", result);
}

bool TestTags(const char * fileName)
{
	Xml     xml;

	errno_t err;
	FILE    * inFile;
	char    element[MAX_ELEMENT + 1], content[MAX_CONTENT + 1];

	err = fopen_s(&inFile, fileName, "r");
	if (err != 0)
	{
		printf("Could not open file '%s' for reading", fileName);
		return false;
	}

	xml.GetNextElement(inFile, element, MAX_ELEMENT, content, MAX_CONTENT);
	if (strcmp("?xml version=\"1.0\" encoding=\"UTF-8\"?", element) != 0)
		return false;
	xml.GetNextElement(inFile, element, MAX_ELEMENT, content, MAX_CONTENT);
	if (strcmp("card", element) != 0)
		return false;
	xml.GetNextElement(inFile, element, MAX_ELEMENT, content, MAX_CONTENT);
	if (strcmp("type", element) != 0)
		return false;
	xml.GetNextElement(inFile, element, MAX_ELEMENT, content, MAX_CONTENT);
	if (strcmp("testCardVersion", element) != 0)
		return false;
	xml.GetNextElement(inFile, element, MAX_ELEMENT, content, MAX_CONTENT);
	if (strcmp("friendlyName", element) != 0)
		return false;
	xml.GetNextElement(inFile, element, MAX_ELEMENT, content, MAX_CONTENT);
	if (strcmp("chipNumber", element) != 0)
		return false;
	xml.GetNextElement(inFile, element, MAX_ELEMENT, content, MAX_CONTENT);
	if (strcmp("physicalCardChipNumber", element) != 0)
		return false;
	xml.GetNextElement(inFile, element, MAX_ELEMENT, content, MAX_CONTENT);
	if (strcmp("versionNumber", element) != 0)
		return false;
	xml.GetNextElement(inFile, element, MAX_ELEMENT, content, MAX_CONTENT);
	if (strcmp("atr", element) != 0)
		return false;
	return true;
}
