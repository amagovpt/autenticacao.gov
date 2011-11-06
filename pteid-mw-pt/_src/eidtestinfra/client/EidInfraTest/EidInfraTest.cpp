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

#include "UnitTest++.h"
#include "xml.h"
#include <string.h>
#include <stdio.h>
#include "util.h"

using namespace EidInfra;

char VIRTUAL_CARD_PATH[1024];
char RELATIVE_PATH_PREFIX[32];

int main(int argc, char* argv[])
{
	printf("************************************************************************\n");
	printf("NOTE: make sure the visibility in your control file are set to HIDE_REAL\n");
	printf("************************************************************************\n");

	FILE *f = NULL;
	if (fopen_s(&f, "../../_DocsInternal/PteidTestCtrl.xml", "r") == 0)
		fclose(f);
	else
	{
		printf("\nERR: can't run the unit tests: ../../_DocsInternal doesn't exist\n\n");
		return -1;
	}

	strcpy_s(RELATIVE_PATH_PREFIX, 31, "../../");

	int result = UnitTest::RunAllTests();

	return result;
}

// Help function
bool writeFile(const char *filename, const void *contents, size_t contentsLen)
{
	FILE *f = fopen(filename, "wb");
	if (f == NULL) {
		printf("ERR: couldn't open file \"%s\" to write to\n", filename);
		return false;
	}

	fwrite(contents, 1, contentsLen, f);
	fclose(f);

	return true;
}
