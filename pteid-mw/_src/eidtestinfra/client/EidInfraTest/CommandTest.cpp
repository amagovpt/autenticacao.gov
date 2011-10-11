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

#ifdef UNICODE
#undef UNICODE
#endif

#include <algorithm>
#include <cstring>
#include <string.h>
#include "FileStructure.h"
#include "SoftReaderManager.h"
#include "CommandTestHeader.h"
#include "SoftReader.h"
#include "SoftCard.h"
#include "Control.h"
#include "virtualCardAccess.h"
#include "UnitTest++.h"
#include "xml.h"
#include "util.h"
#include "utils.h"
#include "utils2.h"
using namespace EidInfra;
using namespace std;
using namespace eidmw::pcscproxy;

#pragma pack(1)

extern char VIRTUAL_CARD_PATH[];
extern char RELATIVE_PATH_PREFIX[];

char * createRelativePath(const char * fileName);

static const BYTE        hardNumber[16] = { 0x53, 0x4C, 0x49, 0x4E, 0x33, 0x66, 0x00, 0x29, 0x6C, 0xFF, 0x23, 0x2C, 0xF7, 0x13, 0x10, 0x30 };
std::vector<std::string> files;
static const BYTE        p2[4] = { 0xDF, 0x01, 0x40, 0x38 };
unsigned char            sendBuf[280];
size_t                   sendBufLen = sizeof(sendBuf);
unsigned char            recvBuf[258];
FileStructure            fs;

/*
 * Ok to add/delete commands to the list below
 * Format: "CommandToSend|ExpectedResult"
 */
const static char *fixedTestAPDUS2[] =
{
	"00:A4:02:0C:02:DF:00|90:00",

	"00:A4:08:0C:04:3F:00:2F:00|90:00",
	"00:A4:08:0C:06:3F:00:DF:00:50:31|90:00",

	"00:A4:02:0C:02:50:31|90:00",
	"00:B0:00:00:FC|6C:24",
	"00:B0:00:00:24|A0:0A:30:08:04:06:3F:00:DF:00:50:35:A4:0A:30:08:04:06:3F:00:DF:00:50:37:A8:0A:30:08:04:06:3F:00:DF:00:50:34:90:00",
	"00:B0:00:00:20|A0:0A:30:08:04:06:3F:00:DF:00:50:35:A4:0A:30:08:04:06:3F:00:DF:00:50:37:A8:0A:30:08:04:06:3F:00:90:00",
	"00:B0:00:00:02|A0:0A:90:00",
	"00:A4:04:0C:0C:A0:00:00:01:77:50:4B:43:53:2D:31:35|90:00",
	"00:B0:00:0C:05|69:86",
	"00:A4:04:0C:0C:A0:00:00:01:77:50:4B:43:53:2D:31:36|6A:82",
	"80:E4:00:00:1C|53:4C:49:4E:33:66:00:29:6C:FF:23:2C:F7:13:10:31:A5:03:01:01:01:11:00:02:00:01:01:0F:90:00",
	"80:E4:00:00:1B|53:4C:49:4E:33:66:00:29:6C:FF:23:2C:F7:13:10:31:A5:03:01:01:01:11:00:02:00:01:01:61:01",
	"80:E4:00:00:1D|6C:1C",
	"80:E4:09:00:1C|6B:00",
	"80:E4:00:0B:1C|6B:00",
	"00:A4:02:0C:02:2F:00|6A:82",
	"00:A4:02:0C:02:3F:00|90:00",
	"00:A4:02:0C:02:2F:00|90:00",
	"00:B0:00:00:FC|6C:25",
	"00:B0:00:00:25|61:23:4F:0C:A0:00:00:01:77:50:4B:43:53:2D:31:35:50:06:42:45:4C:50:49:43:51:04:3F:00:DF:00:73:05:06:03:60:38:02:90:00",
	"00:B0:00:24:01|02:90:00",
	"00:B0:00:25:01|6B:00",
	"00:B0:00:25:00|6B:00",
	"00:B0:00:26:00|6B:00",
	"00:A4:02:0C:02:DF:01|90:00",
	"00:A4:02:0C:02:DF:00|6A:82",
	"00:A4:04:0C:0C:A0:00:00:01:77:50:4B:43:53:2D:31:35|90:00",

	NULL
};

TEST(Transmit)
{
	SoftReaderManager srmng;
	DWORD             buflen = READER_NAME_LENGTH;
	char              buf[READER_NAME_LENGTH + 1];
	const char        *myHardName = "MySoftReader";
	DWORD             len         = (DWORD) strlen(myHardName);
	memcpy(buf, myHardName, len + 1);
	buf[len + 1] = '\0';
	size_t                     result2 = srmng.createSoftReaders(buf, &buflen);
	char                       softReaderName[READER_NAME_LENGTH + 1];
	strcpy_s(softReaderName, READER_NAME_LENGTH, (char*) &buf[0]);
	SoftReader                 *sr = srmng.getSoftReaderByName(softReaderName, N_SOFT);
	eidmw::pcscproxy::SoftCard *sc = sr->createSoftCard(1234);
	sc->loadFromFile(createRelativePath("_DocsInternal/virtual_stephen.xml"), hardNumber);
	size_t                     commandLength, resultLength;
	fs.addAll(files);
	fs.selectByPath(p2, sizeof(p2));
	char nextCommand[COMMAND_LENGTH + 1];
	char nextResult[RESULT_LENGTH + 1];
	char *ptr;
	char *next_token;

	// Send all APDUs
	for (int i = 0; fixedTestAPDUS2[i] != NULL; i++)
	{
		strcpy_s(nextCommand, COMMAND_LENGTH, fixedTestAPDUS2[i]);
		ptr = strtok_s(nextCommand, "|", &next_token);
		strcpy(nextCommand, ptr);
		ptr = strtok_s(NULL, "|", &next_token);
		strcpy(nextResult, ptr);
		resultLength = strlen(nextResult);
		hex2bin(nextResult, (unsigned char *) nextResult, &resultLength);
		commandLength = strlen(nextCommand);
		hex2bin(nextCommand, sendBuf, &commandLength);

		const eidmw::pcscproxy::APDURequest apduReq     = eidmw::pcscproxy::APDURequest::createAPDURequest(sendBuf, (DWORD) commandLength);
		DWORD                               recvBufLen2 = (DWORD) sizeof(recvBuf);;
		result2 = sr->transmit(apduReq, recvBuf, &recvBufLen2);

		if (memcmp(nextResult, recvBuf, recvBufLen2) != 0)
		{
			printf("  \n");
			printf("--- Send error at APDU %d ---:\n", i + 1);
			dumphex("Expected: ", (unsigned char *) nextResult, resultLength);
			dumphex("     Got: ", recvBuf, recvBufLen2);
			printf("  \n");
		}
	}
}

char * createRelativePath(const char * fileName)
{
	strcpy(VIRTUAL_CARD_PATH, RELATIVE_PATH_PREFIX);
	strcat(VIRTUAL_CARD_PATH, fileName);
	return VIRTUAL_CARD_PATH;
}

