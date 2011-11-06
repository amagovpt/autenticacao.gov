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
/**
 * This prog put a MOZILLA_CERTIFICATE_FOLDER env. variable with
 * value ~//Library/OOo.default in ~/.MacOSX/environment.plist.
 * That is:
 * - if the file doesn't exist, it's created and filled in
 * - if the entry already exists, nothing is done
 * - if another value for MOZILLA_CERTIFICATE_FOLDER exists,
 *     an error is generated
 * - otherwise the entry is inserted at the end of the file
 *
 * It would be better to use perl for this, if only I knew perl...
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define PREFS_DIR "/.MacOSX"
#define PREFS_FILE "/.MacOSX/environment.plist"

static int MakeNewPrefsFile(const char *path, const char *entry);

static int InsertInPrefsFile(const char *path,
	const char *filebuf, size_t filebuflen, const char *entry);

int main()
{
	char path[200];
	char cmd[500];
	char entry[500];
	struct stat fileinfo;
	FILE *fil = NULL;
	char *home;
	size_t filebuflen;
	size_t filelen;
	char *filebuf = NULL;
	int r;

	home = getenv("HOME");

	// Create the ~/.MacOSX dir
	strncpy(cmd, "/bin/mkdir -p ", sizeof(cmd));
	strncat(cmd, home, sizeof(cmd));
	strncat(cmd, "/.MacOSX", sizeof(cmd));
	cmd[sizeof(cmd) - 1] = '\0';
	system(cmd);

	// Copy the OOo.default dir in not yet existing
	strncpy(cmd, "/bin/test -d ", sizeof(cmd));
	strncat(cmd, home, sizeof(cmd));
	strncat(cmd, "/Library/OOo.default || /bin/cp -r /usr/local/lib/OOo.default ", sizeof(cmd));
	strncat(cmd, home, sizeof(cmd));
	strncat(cmd, "/Library/OOo.default", sizeof(cmd));
	cmd[sizeof(cmd) - 1] = '\0';
	system(cmd);

	// Fix access conditions in the OOo.default dir
	strncpy(cmd, "/bin/chmod 600 ", sizeof(cmd));
	strncat(cmd, home, sizeof(cmd));
	strncat(cmd, "/Library/OOo.default/*", sizeof(cmd));
	cmd[sizeof(cmd) - 1] = '\0';
	system(cmd);

	// The entry to be added to the environment.plist file
	strncpy(entry, "\t<key>MOZILLA_CERTIFICATE_FOLDER</key>\n", sizeof(entry));
	strncat(entry, "\t<string>", sizeof(entry));
	strncat(entry, home, sizeof(entry));
	strncat(entry, "/Library/OOo.default</string>\n", sizeof(entry));
	entry[sizeof(entry) - 1] = '\0';

	// Path of the environment.plist file
	strncpy(path, home, sizeof(path));
	strncat(path, PREFS_FILE, sizeof(path));
	path[sizeof(path) - 1] = '\0';

	r = stat(path, &fileinfo);
	if (r != 0)
		return MakeNewPrefsFile(path, entry);

	fil = fopen(path, "r");
	if (fil == NULL)
		return MakeNewPrefsFile(path, entry);

	r = -1;

	filebuflen = fileinfo.st_size + sizeof(entry) + 10;
	if (filebuflen < fileinfo.st_size)
		goto cleanup; // integer overflow
	filebuf = (char *) malloc(filebuflen);
	if (filebuf == NULL)
		goto cleanup; // malloc() failed

	// Read the file
	filelen = fread(filebuf, 1, fileinfo.st_size, fil);
	filebuf[filelen] = '\0';

	if (strstr(filebuf, "OOo.default") != NULL)
	{
		r = 0; // entry already seems to exist: OK
		goto cleanup;
	}

	if (strstr(filebuf, "MOZILLA_CERTIFICATE_FOLDER") != NULL)
	{
		printf("*******************************************************************************\n");
		printf("Another MOZILLA_CERTIFICATE_FOLDER environment variable has already been set\n");
		printf("in file \"%s\"\n", path);
		printf("So it looks like support for another smart card has already been configured\n");
		printf("this way. Pls. use the manual procedure to configure OOo for both smart cards\n");
		printf("*******************************************************************************\n");
		goto cleanup;
	}

	fclose(fil);
	fil = NULL;

	InsertInPrefsFile(path, filebuf, filebuflen, entry);

cleanup:
	if (fil != NULL)
		fclose(fil);
	if (filebuf != NULL)
		free(filebuf);

	return r;
}

static int MakeNewPrefsFile(const char *path, const char *entry)
{
	char filebuf[1000];
	FILE *f;

	f = fopen(path, "wb");
	if (f == NULL)
		return -1;

	fprintf(f, "%s%s%s%s%s%s%s",
		"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n",
		"<!DOCTYPE plist PUBLIC \"-//Apple Computer//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n",
		"<plist version=\"1.0\">\n",
		"<dict>\n",
		entry,
		"</dict>\n",
		"</plist>\n"
	);

	fclose(f);

	return 0; // OK
}

static int InsertInPrefsFile(const char *path,
	const char *filebuf, size_t filebuflen, const char *entry)
{
	FILE *f;
	size_t entrylen;

	char *insertat = strstr(filebuf, "</dict>");
	if (insertat == NULL || insertat <= filebuf)
		return -1;

	// Insert
	entrylen = strlen(entry);
	memmove(insertat + entrylen, insertat, entrylen);
	memcpy(insertat, entry, entrylen);

	f = fopen(path, "wb");
	if (f == NULL)
		return -1;

	fprintf(f, "%s", filebuf);

	fclose(f);

	return 0; // OK
}
