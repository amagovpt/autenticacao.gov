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
#include <string.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	// Get the %ALLUSERSPROFILE% dir
	char ctrFile[300];
	size_t len = sizeof(ctrFile);
	if (0 != getenv_s(&len, ctrFile, len, "ALLUSERSPROFILE"))
	{
		printf("ERR: can't get %ALLUSERSPROFILE% environment variable\n");
	}
	else
	{
		printf("ctrFile = %s\n", ctrFile);
		// Append the file name
		strcat_s(ctrFile, sizeof(ctrFile), "\\BeidTestCtrl.xml");

		// Check if the file already exists
		FILE *f;
		if (0 == fopen_s(&f, ctrFile, "r"))
		{
			fclose(f);
			printf("File \"%s\" already exists, doing nothing\n", ctrFile);
		}
		else
		{
			// File doesn't exist yet, create it ...
			if (0 != fopen_s(&f, ctrFile, "w"))
			{
				printf("ERR: can't create file \"%s\"\n", ctrFile);
			}
			else
			{
				// ... and write to it ...
				const char *conts =
					"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n"
					"<control xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"http://env.dev.eid.belgium.be/eidtestinfra http://env.dev.eid.belgium.be/schemas/control.xsd\">\r\n"
					"	<show>REAL_LAST</show>\r\n"
					"</control>\r\n";
				fprintf(f, "%s", conts);
				fclose(f);

				// ... and set the access conditions
				system("cacls \"%ALLUSERSPROFILE%\\BeidTestCtrl.xml\" /E /G Users:F");

				printf("Created file \"%s\" and set the ACs\n", ctrFile);
			}
		}
	}

	return 0;
}
