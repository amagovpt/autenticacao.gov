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

/**
 * Test tool for PCSC commands and for the APDUs inside an SCardTransmit().
 */
#include "utils.h"
#include "pcscerr2string.h"
#ifdef _WIN32
#include <windows.h>
#else
#define SCARD_READERSTATEA       SCARD_READERSTATE
#define SCardListReadersA        SCardListReaders
#define SCardConnectA            SCardConnect
#define SCardGetStatusChangeA    SCardGetStatusChange
#define SCardStatusA             SCardStatus
#include <wintypes.h>
#include <cstring>
#endif
#include <winscard.h>
#include <stdio.h>

typedef struct {
	char       command;
	int        readerNr;
	int        apduCount;
	const char **apdus;
	const char *chipnr;
	DWORD      timeout;
	int        test;
} Params;

#define CHIP_NR_LEN    16

//#define LOOP_LIST_READERS

static int printHelp(const char *msg);
static int parseCommandLine(int argc, const char **argv, Params *params);
static const char *listReaders(SCARDCONTEXT ctx, char *readerList, size_t readerListSize,
	bool printList, int readerNr);
static void getStatusChange(SCARDCONTEXT ctx, const char *readerList, DWORD timeout);
static long sendAPDUS(SCARDCONTEXT ctx, const char *readerName, int apduCount, const char **apdus);
static long sendAPDU(SCARDHANDLE hCard, const char *apdu, unsigned char *recvBuf, DWORD *recvBufLen,
	const char *chipNr, int idx, bool doDump);
static long sendAPDU(SCARDHANDLE hCard, const char *apdu,
	unsigned char *recvBuf, DWORD *recvBufLen);
static int testAPDUS(SCARDCONTEXT ctx, const char *readerName, const char *chipnr);
static const char *GetChipNrFromCard(SCARDHANDLE hCard, char *chipNrBuf);
static void printState(const SCARD_READERSTATEA *state);
static int testCardFunctionality(SCARDCONTEXT ctx, const char *readerName);
static int testLongFileRead(SCARDCONTEXT hCard);
static int testGetResponse(SCARDCONTEXT hCard);
static int testMisc(SCARDCONTEXT hCard);
static int testStatus(SCARDCONTEXT ctx, const char *readerName, const char *readerList);
static int testConnect(SCARDCONTEXT ctx, const char *readerName);
static int testTransaction(SCARDCONTEXT ctx, const char *readerName);
static int testTransmit(SCARDCONTEXT ctx, const char *readerName, const char **apdus);

int main(int argc, const char **argv)
{
	Params       params;
	SCARDCONTEXT ctx;

	int          returnValue = 0;

	if (0 != parseCommandLine(argc, argv, &params))
		return -1;

	long ret = SCardEstablishContext(SCARD_SCOPE_USER, NULL, NULL, &ctx);
	CHECK_PCSC_RET("SCardEstablishContext", ret);
	if (SCARD_S_SUCCESS == ret)
	{
		char       readerList[2000];

#ifdef LOOP_LIST_READERS
again:
#endif

		const char *readerName = listReaders(ctx, readerList, sizeof(readerList),
			'l' == params.command, params.readerNr);

#ifdef LOOP_LIST_READERS
		if (params.command == 'l')
		{
			getchar();
			goto again;
		}
#endif

		if (NULL != readerName)
		{
			switch (params.command) {
			case 's':
				getStatusChange(ctx, readerList, params.timeout);
				break;
			case 'l':
				// already done in listReaders()
				break;
			case 'a':
				sendAPDUS(ctx, readerName, params.apduCount, params.apdus);
				break;
			case 'f':
				testCardFunctionality(ctx, readerName);
				break;
			case 't':
				testAPDUS(ctx, readerName, params.chipnr);
				break;
			case 'x':
				switch (params.test) {
				case 1:
					returnValue = testStatus(ctx, readerName, readerList);
					break;
				case 2:
					returnValue = testConnect(ctx, readerName);
					break;
				case 3:
					returnValue = testTransaction(ctx, readerName);
					break;
				case 4:
					returnValue = testTransmit(ctx, readerName, params.apdus);
					break;
				default:
					printf("Unknown -x parameter '%d', exiting\n", params.test);
				}

				break;
			default:
				printf("Unknown command '%c', exiting\n", params.command);
			}
		}
	}

	ret = SCardReleaseContext(ctx);

	return returnValue;
}

static int parseCommandLine(int argc, const char **argv, Params *params)
{
	memset(params, 0, sizeof(Params));

	if (argc < 2 || argv[1][0] != '-')
		return printHelp("Invalid command");

	int        index = 1;
	const char *arg  = argv[index];

	if (!strcmp(arg, "-l"))
	{
		if (argc != 2)
			return printHelp("No arguments are allowed after '-l'\n");
		params->command = 'l';
		return 0;
	}

	if (!strcmp(arg, "-r"))
	{
		if (argc < 3)
			return printHelp("No reader number are specified after '-r'\n");
		params->readerNr = atoi(argv[2]);

		index += 2;
		arg    = argv[index];
	}

	if (!strcmp(arg, "-s"))
	{
		if (argc - index != 2)
			return printHelp("Only 1 argument allowed after '-s'\n");
		params->command = 's';
		params->timeout = atoi(argv[index + 1]);
		return 0;
	}

	if (!strcmp(arg, "-a"))
	{
		if (argc - index < 2)
			return printHelp("No arguments specified after '-a'\n");
		params->command   = 'a';
		params->apduCount = argc - index - 1;
		params->apdus     = argv + (index + 1);
	}
	else if (!strcmp(arg, "-t"))
	{
		if (argc - index > 2)
			return printHelp("Only 1 argument allowed after '-t'\n");
		params->command = 't';
		if (argc - index == 2)
			params->chipnr = argv[index + 1];
	}
	else if (!strcmp(arg, "-f"))
	{
		if (argc - index > 1)
			return printHelp("No arguments are allowed after '-f'\n");
		params->command = 'f';
	}
	else if (!strcmp(arg, "-x"))
	{
		if (argc < 3)
			return printHelp("No tests specified '-x'\n");
		params->command = 'x';
		params->test    = atoi(argv[2]);
	}

	else
		return printHelp("Unknown command\n");

	return 0;
}

static int printHelp(const char *msg)
{
	if (NULL != msg)
		printf("%s", msg);
	printf("Usage:\n");
	printf("pcsctool -l                                  : list readers\n");
	printf("pcsctool {-r <readernr>} -s <timeout> ...    : status change testing\n");
	printf("pcsctool -x                                  : get soft card status\n");
	printf("  timeout is the timeout in in msec, precify -1 for an infinite timeout\n");
	printf("pcsctool {-r <readernr>} -a <apdu1> ...      : send APDUs\n");
	printf("pcsctool {-r <readernr>} -t {<chipNr>}       : test + optionally compare\n");
	printf("pcsctool {-r <readernr>} -f                  : functional tests\n");
	printf("  readernr is the number before the reader name returned by '-l'\n");
	printf("  if no readernr is specified then the 1st reader (readernr = 0) is used\n");
	printf("\nExamples:\n");
	printf("pcsctool -a 00:A4:04:0C:0C:A0:00:00:01:77:50:4B:43:53:2D:31:35 80E400001C\n");
	printf("pcsctool -t\n");
	printf("pcsctool -r 1 -t 534C494E336600296CFF2623660B0826\n");
	printf("pcsctool -r 0 -f\n");

	return -1;
}

static const char *listReaders(SCARDCONTEXT ctx, char *readerList, size_t readerListSize,
	bool printList, int readerNr)
{
	DWORD      dwLen       = (DWORD) readerListSize;
	const char *readerName = NULL;

	long       ret = SCardListReadersA(ctx, NULL, readerList, &dwLen);
	CHECK_PCSC_RET("SCardListReaders", ret);
	if (SCARD_S_SUCCESS == ret)
	{
		if (dwLen < 2)
			printf("No reader found, exiting\n");
		else
		{
			if (printList)
				printf("Reader list:\n");
			int readerCount = 0;
			while (readerList[0] != '\0')
			{
				if (printList)
					printf("  %d : %s\n", readerCount, readerList);
				if (readerCount == readerNr)
					readerName = readerList;
				readerList += strlen(readerList) + 1;
				readerCount++;
			}
		}
	}

	if (readerNr > 0 && NULL == readerName)
		printf("ERR: readernr (%d) too high, not enough readers present\n", readerNr);

	return readerName;
}

const static int MAX_READER_STATES = 16;

static void getStatusChange(SCARDCONTEXT ctx, const char *readerList, DWORD timeout)
{
	SCARD_READERSTATEA readerStates[MAX_READER_STATES];
	int                readerCount = 0;

	while (readerList[0] != '\0' && readerCount < MAX_READER_STATES)
	{
		readerStates[readerCount].szReader       = readerList;
		readerStates[readerCount].dwCurrentState = SCARD_STATE_UNAWARE;

		readerList += strlen(readerList) + 1;
		readerCount++;
	}

	if (timeout == (DWORD) -1)
	{
		timeout = INFINITE;
		printf("Remove insert card(s) (or not) and hit ENTER\n");
		printf("Or type 'q' stop\n\n");
	}
	else
		printf("Remove insert card(s) or type Ctrl-C to stop\n\n");


	char keybd = 0;
	while (keybd != 'q' && keybd != 'Q')
	{
		long ret = SCardGetStatusChangeA(ctx, timeout, readerStates, (DWORD) readerCount);
		if ((long) SCARD_S_SUCCESS != ret && (long) SCARD_E_TIMEOUT != ret)
		{
			CHECK_PCSC_RET("SCardGetStatusChange", ret);
			break;
		}

		if ((long) SCARD_E_TIMEOUT == ret)
			printf(" - timeout (nothing changed)\n");
		else
		{
			for (int i = 0; i < readerCount; i++)
			{
				printState(&readerStates[i]);
				readerStates[i].dwCurrentState = readerStates[i].dwEventState &
												 ~SCARD_STATE_CHANGED & ~SCARD_STATE_UNKNOWN;
			}
		}

		// Wait until the user hits a key
		if (timeout != (DWORD) -1)
			keybd = getchar();
	}
}

static long sendAPDUS(SCARDCONTEXT ctx, const char *readerName, int apduCount, const char **apdus)
{
	SCARDHANDLE hCard;
	DWORD       protocol;

	printf("Using reader \"%s\"\n\n", readerName);

	long ret = SCardConnectA(ctx, readerName,
		SCARD_SHARE_SHARED, SCARD_PROTOCOL_T0, &hCard, &protocol);
	CHECK_PCSC_RET("SCardConnect", ret);

	delayMS(200);

	if (SCARD_S_SUCCESS == ret)
	{
		unsigned char recvBuf[258];
		DWORD         recvBufLen;
		for (int i = 0; i < apduCount; i++)
		{
			recvBufLen = (DWORD) sizeof(recvBuf);
			sendAPDU(hCard, apdus[i], recvBuf, &recvBufLen, NULL, 0, true);
		}

		ret = SCardDisconnect(hCard, SCARD_LEAVE_CARD);
		CHECK_PCSC_RET("SCardDisconnect", ret);
	}

	return 0;
}

static long sendAPDU(SCARDHANDLE hCard, const char *apdu,
	unsigned char *recvBuf, DWORD *recvBufLen,
	const char *chipNr, int idx, bool doDump)
{
	unsigned char sendBuf[280];
	size_t        sendBufLen = sizeof(sendBuf);

	// Hex string -> byte array
	if (0 == hex2bin(apdu, sendBuf, &sendBufLen))
	{
		// Check the APDU
		if (sendBufLen < 4)
			printf("ERR: APDU should be at least 4 bytes\n");
		else if (sendBufLen > 5 && ((size_t) (5 + sendBuf[4]) != sendBufLen))
			printf("ERR: wrong P3 byte in case 3 APDU\n");
		else
		{
			if (doDump)
				dumphex("  - sending ", sendBuf, sendBufLen);

			delayMS(50);

			long ret = SCardTransmit(hCard,
				&g_rgSCardT0Pci, sendBuf, (DWORD) sendBufLen,
				NULL, recvBuf, recvBufLen);
			CHECK_PCSC_RET("SCardTransmit", ret);

			if (SCARD_S_SUCCESS == ret)
			{
				if (doDump)
				{
					dumphex("    received ", recvBuf, *recvBufLen);
					printf("\n");
				}

				if (NULL != chipNr)
					StoreAPDUs(chipNr, idx, sendBuf, sendBufLen, recvBuf, *recvBufLen);

				return 0; // success
			}
		}
	}

	return -1; // failed
}

static long sendAPDU(SCARDHANDLE hCard, const char *apdu,
	unsigned char *recvBuf, DWORD *recvBufLen)
{
	return sendAPDU(hCard, apdu, recvBuf, recvBufLen, NULL, 0, false);
}

const static char *fixedTestAPDUS[] = {
	"00:A4:04:0C:0C:A0:00:00:01:77:50:4B:43:53:2D:31:35",          // Select file by AID
	"00:A4:04:0C:0C:A0:00:00:01:77:50:4B:43:53:2D:31:36",          // Select file by AID bad AID
	"00:A4:04:0C:0F:A0:00:00:01:77:50:4B:43:53:2D:31:35:99:88:99", // Select file by AID bad AID
	"00:A4:04:0C:0B:A0:00:00:01:77:50:4B:43:53:2D:31",             // Select file by AID bad AID
	"00:A4:00:0C:02:12:34",                                        // Select file by ? wrong param
	"00:A4:02:0C:02:12:34",                                        // Select file file not found
	"00:A4:02:0C:03:12:34:45",                                     // Select file by ID not found
	"00:A4:02:0C:02:DF:00",                                        // Select directory DF00
	"00:A4:02:0C:02:50:31",                                        // Select file 5031
	"00:A4:08:0C:04:3F:00:2F:00",                                  // Select file by path
	"00:A4:08:0C:06:3F:00:DF:00:50:31",                            // Select file by path

	"00:B0:00:00:FC",                                              // ReadBinary 252 bytes (le > la)
	"00:B0:00:00:24",                                              // ReadBinary 36 bytes (le == la)
	"00:B0:00:00:20",                                              // ReadBinary 32 bytes (le < la)
	"00:C0:00:00:02",                                              // Get Response 2 bytes on empty resp buffer
	"00:C0:00:00:00",                                              // Get Response 0 bytes on empty resp buffer
	"00:B0:00:24:01",                                              // ReadBinary offset > la
	"00:B0:00:25:01",                                              // ReadBinary offset > la
	"00:B0:00:25:00",                                              // ReadBinary offset > la
	"00:B0:00:26:00",                                              // ReadBinary offset > la

	"00:A4:02:0C:02:DF:00",                                        // Select directory DF00 cannot select higher level dir
	"00:A4:02:0C:02:3F:00",                                        // Select directory MF root
	"00:A4:02:0C:02:DF:00",                                        // Select directory DF00
	"00:B0:00:00:01",                                              // ReadBinary from a directory

	"80:E4:00:00:00",                                              // Get Card Data 0 bytes
	"80:E4:00:00:1D",                                              // Get Card Data 29 bytes (la > le)
	"80:E4:09:00:1C",                                              // Get Card Data wrong param P1
	"80:E4:00:0B:1C",                                              // Get Card Data wrong param P2
	"80:E4:00:00:0A",                                              // Get Card Data 10 bytes (le < la)

	"40:E4:00:00:1C",                                              // NON EXISTING COMMAND
	"80:C9:00:00:1C",                                              // NON EXISTING COMMAND

	NULL
};

static const char HEX_TABLE[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

static int testAPDUS(SCARDCONTEXT ctx, const char *readerName, const char *chipnrForCompare)
{
	SCARDHANDLE hCard;
	DWORD       protocol;
	int         err      = 0;
	int         errCount = 0;

	printf("Using reader \"%s\"\n\n", readerName);

	long ret = SCardConnectA(ctx, readerName,
		SCARD_SHARE_SHARED, SCARD_PROTOCOL_T0, &hCard, &protocol);

	CHECK_PCSC_RET("SCardConnect", ret);
	if (SCARD_S_SUCCESS == ret)
	{
		delayMS(200);

		unsigned char recvBuf[258];
		DWORD         recvBufLen = sizeof(recvBuf);

		// If chipnrForCompare == NULL then we retrieve the chip number
		// of this card to store all APDUs
		const char *thisChipNr = NULL;
		char       chipNrBuf[2 * CHIP_NR_LEN + 1];
		if (NULL == chipnrForCompare)
		{
			thisChipNr = GetChipNrFromCard(hCard, chipNrBuf);
			if (NULL == thisChipNr)
			{
				SCardDisconnect(hCard, SCARD_LEAVE_CARD);
				return -1;
			}
		}

		// Send all APDUs
		for (int i = 0; fixedTestAPDUS[i] != NULL; i++)
		{
			recvBufLen = (DWORD) sizeof(recvBuf);
			ret        = sendAPDU(hCard, fixedTestAPDUS[i],
				recvBuf, &recvBufLen, thisChipNr, i, NULL == chipnrForCompare);

			if (0 == ret && NULL != chipnrForCompare)
			{
				err = compareAPDUS(fixedTestAPDUS[i], recvBuf, recvBufLen,
					chipnrForCompare, i);
				if (err < 0)
					break;
				errCount += err;
			}
		}

		ret = SCardDisconnect(hCard, SCARD_LEAVE_CARD);
		CHECK_PCSC_RET("SCardDisconnect", ret);

		if (NULL == chipnrForCompare)
			printf("Done, stored APDUs to files who's names start with %s\n", thisChipNr);
		else if (err >= 0)
			printf("APDU Tests done, %d differences with chip %s)\n", errCount, chipnrForCompare);
	}

	return 0;
}

static int testCardFunctionality(SCARDCONTEXT ctx, const char *readerName)
{
	SCARDHANDLE hCard;
	DWORD       protocol;
	int         errCount = 0;

	printf("Using reader \"%s\"\n\n", readerName);

	long ret = SCardConnectA(ctx, readerName,
		SCARD_SHARE_SHARED, SCARD_PROTOCOL_T0, &hCard, &protocol);

	CHECK_PCSC_RET("SCardConnect", ret);
	if (SCARD_S_SUCCESS == ret)
	{
		delayMS(200);

		printf("--- Get Response tests ---\n");
		errCount += testGetResponse(hCard);

		printf("\n--- Get Response tests within a transaction ---\n");
		ret = SCardBeginTransaction(hCard);
		CHECK_PCSC_RET("SCardBeginTransaction", ret);
		errCount += testGetResponse(hCard);
		ret       = SCardEndTransaction(hCard, SCARD_LEAVE_CARD);
		CHECK_PCSC_RET("SCardBeginTransaction", ret);

		printf("\n--- Test reading a long file ---\n");
		errCount += testLongFileRead(hCard);

		printf("\n--- Test reading a long file within a transaction ---\n");
		ret = SCardBeginTransaction(hCard);
		CHECK_PCSC_RET("SCardBeginTransaction", ret);
		errCount += testLongFileRead(hCard);
		ret       = SCardEndTransaction(hCard, SCARD_LEAVE_CARD);
		CHECK_PCSC_RET("SCardBeginTransaction", ret);

		printf("\n--- Misc tests ---\n");
		errCount += testMisc(hCard);

		ret = SCardDisconnect(hCard, SCARD_LEAVE_CARD);
		CHECK_PCSC_RET("SCardDisconnect", ret);
	}

	if (errCount == 0)
		printf("\nFunctional tests done, no errors\n");
	else
		printf("\nFunctional tests done, %d errors\n", errCount);

	return 0;
}

static int testGetResponse(SCARDCONTEXT hCard)
{
	int           ret      = 0;
	int           errCount = 0;
	unsigned char recvBuf[258];
	DWORD         recvBufLen = sizeof(recvBuf);

	////////////////////// Tests on the virtual card ////////////////////

	// If the 1st command is a "Get Card Data", it's no possible to
	// do a "Get Response" to it. So we first do a "Select".
	ret = sendAPDU(hCard, "00:A4:04:0C:0C:A0:00:00:01:77:50:4B:43:53:2D:31:35", recvBuf, &recvBufLen);
	CHECK_PCSC_RET("sendAPDU(Get Card Data)", ret);

	// Get Card Data, only 16 bytes out of the 28
	recvBufLen = sizeof(recvBuf);
	ret        = sendAPDU(hCard, "80:E4:00:00:10", recvBuf, &recvBufLen);
	CHECK_PCSC_RET("sendAPDU(Get Card Data)", ret);
	if (recvBufLen != 18 || recvBuf[recvBufLen - 2] != 0x61 || recvBuf[recvBufLen - 1] != 0x0C)
	{
		printf("ERR: Get Response didn't return 61 0C\n");
		errCount++;
	}

	// Get Response, 11 bytes (so 1 byte left afterwards)
	ret = sendAPDU(hCard, "00:C0:00:00:0B", recvBuf, &recvBufLen);
	CHECK_PCSC_RET("sendAPDU(Get Card Data)", ret);
	if (recvBufLen != 13 || recvBuf[recvBufLen - 2] != 0x61 || recvBuf[recvBufLen - 1] != 0x01)
	{
		printf("ERR: Get Response didn't return 61 01\n");
		errCount++;
	}

	// Get Response, 1 byte (so no byte left afterwards)
	ret = sendAPDU(hCard, "00:C0:00:00:01", recvBuf, &recvBufLen);
	CHECK_PCSC_RET("sendAPDU(Get Card Data)", ret);
	if (recvBufLen != 3 || recvBuf[recvBufLen - 2] != 0x90 || recvBuf[recvBufLen - 1] != 0x00)
	{
		printf("ERR: Get Response didn't return 90 00\n");
		errCount++;
	}

	// Get Response, 1 byte (but no bytes left)
	ret = sendAPDU(hCard, "00:C0:00:00:01", recvBuf, &recvBufLen);
	CHECK_PCSC_RET("sendAPDU(Get Card Data)", ret);
	if (recvBufLen != 2 || recvBuf[0] != 0x6D || recvBuf[1] != 0x00)
	{
		printf("ERR: Get Response didn't return 6D 00\n");
		errCount++;
	}

	/////////////////////// Tests on the real card /////////////////////

	ret = sendAPDU(hCard, "00:20:00:01:08:24:12:34:FF:FF:FF:FF:FF", recvBuf, &recvBufLen);
	CHECK_PCSC_RET("sendAPDU(Verify PIN)", ret);

	ret = sendAPDU(hCard, "00:22:41:B6:05:04:80:01:84:82", recvBuf, &recvBufLen);
	CHECK_PCSC_RET("sendAPDU(MSE SET)", ret);

	ret = sendAPDU(hCard, "00:2A:9E:9A:08:11:22:33:44:55:66:77:88", recvBuf, &recvBufLen);
	CHECK_PCSC_RET("sendAPDU(Sign)", ret);

	// Get Response
	recvBufLen = sizeof(recvBuf);
	ret        = sendAPDU(hCard, "00:C0:00:00:80", recvBuf, &recvBufLen);
	CHECK_PCSC_RET("sendAPDU(Get Card Data)", ret);
	if (recvBufLen != 0x82 || recvBuf[recvBufLen - 2] != 0x90 || recvBuf[recvBufLen - 1] != 0x00)
	{
		printf("ERR: Get Response didn't return 6D 00\n");
		errCount++;
	}

	return errCount;
}

static int testLongFileRead(SCARDCONTEXT hCard)
{
	int           ret      = 0;
	int           errCount = 0;
	unsigned char recvBuf[258];
	DWORD         recvBufLen = sizeof(recvBuf);

	// Select the file
	ret = sendAPDU(hCard, "00:A4:04:0C:0C:A0:00:00:01:77:50:4B:43:53:2D:31:35", recvBuf, &recvBufLen);
	CHECK_PCSC_RET("sendAPDU(select MF)", ret);
	ret = sendAPDU(hCard, "00:A4:02:0C:02:50:38", recvBuf, &recvBufLen);
	CHECK_PCSC_RET("sendAPDU(select 5038)", ret);

	// Get the file size
	char apdu[20];
	int  offs    = 0;
	int  fileLen = 0;
	for (; offs < 5000; offs += 0xfc)
	{
		sprintf(apdu, "00B0%02X%02XFC", offs / 256, offs % 256);
		recvBufLen = sizeof(recvBuf);
		ret        = sendAPDU(hCard, apdu, recvBuf, &recvBufLen);
		CHECK_PCSC_RET("sendAPDU(Read Binary)", ret);
		if (recvBufLen == 2 && recvBuf[0] == 0x6c)
		{
			fileLen = offs + recvBuf[1];
			break;
		}
	}

	// Read 1 byte at filesize - 1
	sprintf(apdu, "00B0%02X%02X01", (fileLen - 1) / 256, (fileLen - 1) % 256);
	recvBufLen = sizeof(recvBuf);
	ret        = sendAPDU(hCard, apdu, recvBuf, &recvBufLen);
	CHECK_PCSC_RET("sendAPDU(Read Binary)", ret);
	if (recvBufLen != 3 || recvBuf[1] != 0x90 || recvBuf[2] != 0x00)
	{
		printf("ERR: ReadBinary(offset = filesize-1, len = 1) didn't return 1 byte\n");
		errCount++;
	}

	// Read 1 byte at filesize
	sprintf(apdu, "00B0%02X%02X01", (fileLen) / 256, (fileLen) % 256);
	recvBufLen = sizeof(recvBuf);
	ret        = sendAPDU(hCard, apdu, recvBuf, &recvBufLen);
	CHECK_PCSC_RET("sendAPDU(Read Binary)", ret);
	if (recvBufLen != 2 || recvBuf[0] != 0x6B || recvBuf[1] != 0x00)
	{
		printf("ERR: ReadBinary(offset = filesize, len = 1) didn't return 6B 00\n");
		errCount++;
	}

	// Read 1 byte at filesize + 1
	sprintf(apdu, "00B0%02X%02X01", (fileLen + 1) / 256, (fileLen + 1) % 256);
	recvBufLen = sizeof(recvBuf);
	ret        = sendAPDU(hCard, apdu, recvBuf, &recvBufLen);
	CHECK_PCSC_RET("sendAPDU(Read Binary)", ret);
	if (recvBufLen != 2 || recvBuf[0] != 0x6B || recvBuf[1] != 0x00)
	{
		printf("ERR: ReadBinary(offset = filesize+1, len = 1) didn't return 6B 00\n");
		errCount++;
	}

	return errCount;
}

static int testMisc(SCARDCONTEXT hCard)
{
	int           ret      = 0;
	int           errCount = 0;
	unsigned char recvBuf[258];
	DWORD         recvBufLen = sizeof(recvBuf);

	ret = sendAPDU(hCard, "80:E4:00:00:1C", recvBuf, &recvBufLen);
	CHECK_PCSC_RET("sendAPDU(Get Card Data)", ret);
	if (recvBufLen != 0x1E || recvBuf[recvBufLen - 2] != 0x90 || recvBuf[recvBufLen - 1] != 0x00)
	{
		printf("ERR: Get Card Data didn't return 90 00\n");
		errCount++;
	}

	ret = sendAPDU(hCard, "80:E4:00:00:1B", recvBuf, &recvBufLen);
	CHECK_PCSC_RET("sendAPDU(Get Card Data)", ret);
	if (recvBufLen != 0x1D || recvBuf[recvBufLen - 2] != 0x61 || recvBuf[recvBufLen - 1] != 0x01)
	{
		printf("ERR: Get Card Data didn't return 61 01\n");
		errCount++;
	}

	return errCount;
}

static const char *GetChipNrFromCard(SCARDHANDLE hCard, char *chipNrBuf)
{
	unsigned char recvBuf[258];
	DWORD         recvBufLen = (DWORD) sizeof(recvBuf);

	memset(chipNrBuf, 0, 2 * CHIP_NR_LEN + 1);

	// Send "Get Card Data" command, the first 16 bytes that are returned is the chip nr
	long ret = sendAPDU(hCard, "80:E4:00:00:1C", recvBuf, &recvBufLen, NULL, 0, false);
	if (0 != ret)
		return NULL;
	else if (recvBufLen != 28 + 2)
	{
		printf("ERR: Get Card Data command returned %d bytes instead of 30\n", (int) recvBufLen);
		return NULL;
	}

	// Convert the chip nr into a hex string
	for (int i = 0; i < 16; i++)
	{
		chipNrBuf[2 * i + 0] = HEX_TABLE[recvBuf[i] / 16];
		chipNrBuf[2 * i + 1] = HEX_TABLE[recvBuf[i] % 16];
	}

	return chipNrBuf;
}

#ifndef SCARD_STATE_UNPOWERED
#define SCARD_STATE_UNPOWERED    0x0400
#endif

static void printState(const SCARD_READERSTATEA *state)
{
	DWORD eventState = state->dwEventState;

	printf(" - %30s: 0x%08x (", state->szReader, (unsigned int) eventState);

	if (eventState & SCARD_STATE_IGNORE)
		printf("IGNORE");
	else
	{
		if (eventState & SCARD_STATE_UNPOWERED)
			printf("UNPOWERED ");
		if (eventState & SCARD_STATE_MUTE)
			printf("MUTE ");
		if (eventState & SCARD_STATE_INUSE)
			printf("INUSE ");
		if (eventState & SCARD_STATE_EXCLUSIVE)
			printf("EXCLUSIVE ");
		if (eventState & SCARD_STATE_ATRMATCH)
			printf("ATRMATCH ");
		if (eventState & SCARD_STATE_PRESENT)
			printf("PRESENT ");
		if (eventState & SCARD_STATE_EMPTY)
			printf("EMPTY ");
		if (eventState & SCARD_STATE_UNAVAILABLE)
			printf("UNAVAILABLE ");
		if (eventState & SCARD_STATE_UNKNOWN)
			printf("UNKNOWN ");
		if (eventState & SCARD_STATE_CHANGED)
			printf("CHANGED ");
	}
	printf(")\n");
}

static int testStatus(SCARDCONTEXT ctx, const char *readerName, const char *readerList)
{
	SCARDHANDLE hCard;
	DWORD       pcchReaderLen = 2048; //can this be calculated beforehand!?
	DWORD       pdwState;
	DWORD       pdwProtocol;
	BYTE        pbAtr[32];
	DWORD       pcbAtrLen = 32;
	char readerName2[2048];
	int  returnValue = 0;

	long        ret = SCardConnectA(ctx, readerName,
		SCARD_SHARE_SHARED, SCARD_PROTOCOL_T0, &hCard, &pdwProtocol);

	CHECK_PCSC_RET("SCardConnect", ret);
	if (SCARD_S_SUCCESS == ret)
	{
		delayMS(200);

		// Normal case: fail position ----1
		ret = SCardStatusA(hCard, readerName2, &pcchReaderLen, &pdwState, &pdwProtocol, (LPBYTE) &pbAtr, &pcbAtrLen);
		if (ret != SCARD_S_SUCCESS)
			returnValue |= 1;

		// ReaderLen = 0: fail position ---1-
		pcchReaderLen  = NULL;
		readerName2[0] = '\0'; //make sure refreshed with new values
		ret            = SCardStatusA(hCard, (LPSTR) readerName2, &pcchReaderLen, &pdwState, &pdwProtocol, (LPBYTE) &pbAtr, &pcbAtrLen);
		if (ret != SCARD_S_SUCCESS)
			returnValue |= 2;

		// ReaderLen = 1: fail position ---1--
		pcchReaderLen  = 1;
		readerName2[0] = '\0'; //make sure refreshed with new values
		ret            = SCardStatusA(hCard, (LPSTR) readerName2, &pcchReaderLen, &pdwState, &pdwProtocol, (LPBYTE) &pbAtr, &pcbAtrLen);

		CHECK_PCSC_RET_PASS(0x4)

		//AtrLen = 0: fail position --1---
		pcchReaderLen = 2048;
		pcbAtrLen     = 0;
		ret           = SCardStatusA(hCard, (LPSTR) readerName2, &pcchReaderLen, &pdwState, &pdwProtocol, (LPBYTE) &pbAtr, &pcbAtrLen);
		if (ret != SCARD_S_SUCCESS)
			returnValue |= 8;

		//AtrLen = 1: fail position -1----
		pcbAtrLen = 1;
		ret       = SCardStatusA(hCard, (LPSTR) readerName2, &pcchReaderLen, &pdwState, &pdwProtocol, (LPBYTE) &pbAtr, &pcbAtrLen);
		if (ret != SCARD_S_SUCCESS)
			returnValue |= 16;

		//AtrLen = 33 (bigger than returned): fail position 1-----
		pcbAtrLen = 33;
		ret       = SCardStatusA(hCard, (LPSTR) readerName2, &pcchReaderLen, &pdwState, &pdwProtocol, (LPBYTE) &pbAtr, &pcbAtrLen);
		if (ret != SCARD_S_SUCCESS)
			returnValue |= 32;
	}

	return returnValue;
}

static int testConnect(SCARDCONTEXT ctx, const char *readerName)
{
	int         returnValue = 0;
	SCARDHANDLE hCard;
	DWORD       protocol;

	long        ret = SCardConnectA(ctx, readerName,
		SCARD_SHARE_SHARED, SCARD_PROTOCOL_T0, &hCard, &protocol);

	if (SCARD_S_SUCCESS != ret)
		returnValue |= 0x1; //-------1

	if (SCARD_S_SUCCESS == ret)
	{
		delayMS(200);

		unsigned char recvBuf[258];
		DWORD         recvBufLen;
		recvBufLen = (DWORD) sizeof(recvBuf);
		sendAPDU(hCard, "00:A4:04:0C:0C:A0:00:00:01:77:50:4B:43:53:2D:31:35", recvBuf, &recvBufLen, NULL, 0, true);

		if (!(recvBufLen == 2))
			returnValue |= 0x2; //------1-
		if (!(recvBuf[0] == 0x90 && recvBuf[1] == 0x00))
			returnValue |= 0x2; //------1-

		ret = SCardDisconnect(hCard, SCARD_LEAVE_CARD);
		if (SCARD_S_SUCCESS != ret)
			returnValue |= 0x4; //-----1--

		ret = SCardDisconnect(1111, SCARD_LEAVE_CARD);
		if (SCARD_S_SUCCESS == ret)
			returnValue |= 0x4; //-----1--
	}

	return returnValue;
}

static int testTransaction(SCARDCONTEXT ctx, const char *readerName)
{
	int         returnValue = 0;
	SCARDHANDLE hCard;
	DWORD       protocol;

	long        ret = SCardConnectA(ctx, readerName,
		SCARD_SHARE_SHARED, SCARD_PROTOCOL_T0, &hCard, &protocol);
	CHECK_PCSC_RET_PASS(0x1) //-------1
	if (SCARD_S_SUCCESS == ret)
	{
		delayMS(200);

		ret = SCardBeginTransaction(hCard);
		CHECK_PCSC_RET_PASS(0x2) //------1-
		ret = SCardEndTransaction(hCard, SCARD_LEAVE_CARD);
		CHECK_PCSC_RET_PASS(0x2) //-----1--
		ret = SCardEndTransaction(1111, SCARD_LEAVE_CARD);
		CHECK_PCSC_RET_FAIL(0x2) //-----1--
	}

	return returnValue;
}

static int testTransmit(SCARDCONTEXT ctx, const char *readerName, const char **apdus)
{
	int           returnValue = 0;

	unsigned char sendBuf[280];
	size_t        sendBufLen = sizeof(sendBuf);
	unsigned char recvBuf[258];
	DWORD         recvBufLen;
	SCARDHANDLE   hCard;
	DWORD         protocol;
	SCARD_IO_REQUEST IORequest;

	long          ret = SCardConnectA(ctx, readerName,
		SCARD_SHARE_SHARED, SCARD_PROTOCOL_T0, &hCard, &protocol);
	CHECK_PCSC_RET("SCardConnect", ret);
	IORequest.cbPciLength = 8;
	IORequest.dwProtocol  = protocol;

	delayMS(200);

	ret = SCardTransmit(hCard, &IORequest, sendBuf,
		(DWORD) sendBufLen, NULL, recvBuf, &recvBufLen);

	CHECK_PCSC_RET_PASS(0x1) //-------1
	if (SCARD_S_SUCCESS == ret)
	{
		ret = SCardTransmit(NULL, (LPCSCARD_IO_REQUEST) apdus, sendBuf,
			(DWORD) sendBufLen, NULL, recvBuf, &recvBufLen);
		CHECK_PCSC_RET_PASS(0x1) //------1-

		ret = SCardTransmit(1111, (LPCSCARD_IO_REQUEST) apdus, sendBuf,
			(DWORD) sendBufLen, NULL, recvBuf, &recvBufLen);
		CHECK_PCSC_RET_PASS(0x1) //-----1--
	}

	return returnValue;
}
