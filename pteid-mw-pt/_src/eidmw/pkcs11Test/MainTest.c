/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2009-2010 FedICT.
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

/*
 * Integration test for the PKCS#11 library.
 * Required interaction: none.
 */

#include <stdio.h>
#include <MainTest.h>
#include "logtest.h"

#define NUMBER_OF_TESTS 10

int main() {
	int i;
	int testCounter = 0;
	char* testDescription[NUMBER_OF_TESTS];
	int result[NUMBER_OF_TESTS];

	initLog();

	testDescription[testCounter] = "Tests opening and closing of a session in a single thread";
	result[testCounter] = test_open_close_session();
	testCounter++;
	testDescription[testCounter] = "Shows info on the mechanisms supported by the card";
	result[testCounter] = test_getmechanisms();
	testCounter++;
	testDescription[testCounter] = "Test multiple finalize/initialize sessions in different threads";
	result[testCounter] = test_finalize_initialize();
	testCounter++;
	testDescription[testCounter] = "tests getting the signature key from the card";
	result[testCounter] = test_getprivatekeys();
	testCounter++;
	testDescription[testCounter] = "tests getting all the objects from the card";
	result[testCounter] = test_getallobjects();
	testCounter++;
	testDescription[testCounter] = "tests signing with the card";
	result[testCounter] = test_sign();
	testCounter++;

	testlog(LVL_NOLEVEL,"\n\n_______________________________________________\n");
	for (i = 0; i < testCounter; i++)
	{
		testlog(LVL_NOLEVEL,"\nTest %d %s \n", i, testDescription[i]);
		if(result[i] != CKR_OK)
		{
			testlog(LVL_NOLEVEL,"FAILED\n", result[i]);	
			testlog(LVL_NOLEVEL,"Result : 0x%.8x \n", result[i]);
		}
		else
		{
			testlog(LVL_NOLEVEL,"PASSED\n");
		}
		testlog(LVL_NOLEVEL,"\n_______________________________________________\n");
	}
	
	
	endLog();
	// Wait for user to end this test
	getchar();
}

