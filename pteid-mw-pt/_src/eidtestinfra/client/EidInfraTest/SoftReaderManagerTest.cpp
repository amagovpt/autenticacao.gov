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
#ifdef _WIN32
#include <windows.h>
#else
#define WINAPI
#include <wintypes.h>
#endif
#include <time.h>
#include <ctime>
#include "SoftReaderManager.h"
#include "SoftReader.h"
#include "SoftCard.h"
#include "Control.h"
#include "virtualCardAccess.h"
#include "UnitTest++.h"
#include "xml.h"
#include "util.h"
using namespace EidInfra;
using namespace std;

using namespace eidmw::pcscproxy;

#pragma pack(1)

DWORD WINAPI SoftReaderThread1(LPVOID);
DWORD WINAPI SoftReaderThread2(LPVOID);

#define THREAD_STACK_SIZE     1024
#define READER_NAME_LENGTH    1024

/*
 * This is the nap length. There are always two threads in the test. One issuing a transaction request and
 * then sleeps followed by relenquishing the transaction.
 * The other non-sleeping thread is supposed to wait on this sleeping thread under different scenarios.
 * When a thread is not specified to sleep it will nevertheless take a small nap all the same.
 * The reason is to allow enough time for the thread that IS specified to sleep to actually
 * issue a transaction request followed by a long sleep (to simulate a lengthy task).
 * Since both threads start simulteneously, this short nap will ensure that the non-sleeping thread
 * will be blocked by the sleeping thread during transaction request.
 *
 *
 *               Sleeping thread      Non-sleeping thread
 *               ---------------      --------------------
 *                      |                      |
 *                     -|--start both threads--|-
 *                      |                      |\begin nap
 *  transaction request-|----------------------| \
 *                      |                      |  |
 *                      |                      |--end nap
 *                      |                      |request transaction...(blocked)
 *          start sleep-|----------------------|
 *                      .                      .
 *                      .                      .
 *                      .                      .
 *            end sleep-|----------------------|
 *                      |                      |
 *      end transaction-|                      |
 *                      |                      |unblocked
 *                      |                      |start transaction
 *                      .                      .
 *                      .                      .
 */

const size_t NAP_LENGTH = 50;

class ThreadParams
{
public:
DWORD sleepValue;
bool  activateTransaction;
int   startHour, startMinute, startSecond;
int   endHour, endMinute, endSecond;

ThreadParams(DWORD sleep, bool tx)
{
	sleepValue          = sleep;
	activateTransaction = tx;
}
};

TEST(Readers)
{
	SoftReaderManager srmng;
	DWORD             buflen = READER_NAME_LENGTH;
	char              buf[1024];

	const char        *myHardName = "MySoftReader";
	DWORD             len         = (DWORD) strlen(myHardName);
	memcpy(buf, myHardName, len + 1);
	buf[len + 1] = '\0';

	srmng.createSoftReaders(buf, &buflen);

	char   softReaderName[READER_NAME_LENGTH + 1];
	strcpy_s(softReaderName, READER_NAME_LENGTH, (char*) &buf[0]);

	SoftReader *sr = srmng.getSoftReaderByName(softReaderName, N_SOFT);
	CHECK(sr->getSoftReaderName().length());
	CHECK(sr->getHardReaderName().length());
	SoftCard                            *sc = sr->createSoftCard(1234);
	DWORD                               bytes   = 7;
	BYTE                                data[7] = { 0x00, 0xA4, 0x02, 0x0C, 0x02, 0xDF, 0x00 };
	const eidmw::pcscproxy::APDURequest apduReq = eidmw::pcscproxy::APDURequest::createAPDURequest(data, bytes);

	if (sc == NULL)
		printf("ERR: SoftReader::createSoftCard() failed\n");
}
#ifdef _WIN32

TEST(Transactions)
{
	DWORD        iID[2];
	HANDLE       hThreads[2];

	DWORD        sleep1 = 2000;
	DWORD        sleep2 = 0;

	ThreadParams *p1, *p2;
	size_t       Reader1StartTime;
	size_t       Reader2EndTime;
	size_t       diffTime;

///////////////////////////////////////////// Test 1 //////////////////////////////////////////////////////

	// Reader 1 initiates transaction, reader 2 initiates transaction
	p1          = new ThreadParams(2000, true);
	p2          = new ThreadParams(0, true);
	hThreads[0] = CreateThread(NULL, THREAD_STACK_SIZE, SoftReaderThread1, p1, NULL, &iID[0]);
	hThreads[1] = CreateThread(NULL, THREAD_STACK_SIZE, SoftReaderThread2, p2, NULL, &iID[1]);
	CHECK_EQUAL(WAIT_OBJECT_0, WaitForMultipleObjects(2, hThreads, TRUE, INFINITE));

	Reader1StartTime = (p1->startHour * 60 * 60) + (p1->startMinute * 60) + p1->startSecond;
	Reader2EndTime   = (p2->endHour * 60 * 60) + (p2->endMinute * 60) + p2->endSecond;
	diffTime         = Reader2EndTime - Reader1StartTime;
	CHECK(diffTime >= 2);

	if (diffTime < 2) // for debug only, print the reason for failure
	{
		printf("Reader 1 start time %i:%i:%i\n", p1->startHour, p1->startMinute, p1->startSecond);
		printf("Reader 1 end time   %i:%i:%i\n", p1->endHour, p1->endMinute, p1->endSecond);
		printf("Reader 2 start time %i:%i:%i\n", p2->startHour, p2->startMinute, p2->startSecond);
		printf("Reader 2 end time   %i:%i:%i\n", p2->endHour, p2->endMinute, p2->endSecond);
	}

///////////////////////////////////////////// Test 2 //////////////////////////////////////////////////////

	//Reader 1 initiates transaction, reader 2 does not initiate transaction
	p1->activateTransaction = true;
	p2->activateTransaction = false;
	hThreads[0]             = CreateThread(NULL, THREAD_STACK_SIZE, SoftReaderThread1, p1, NULL, &iID[0]);
	hThreads[1]             = CreateThread(NULL, THREAD_STACK_SIZE, SoftReaderThread2, p2, NULL, &iID[1]);
	CHECK_EQUAL(WAIT_OBJECT_0, WaitForMultipleObjects(2, hThreads, TRUE, INFINITE));

	Reader1StartTime = (p1->startHour * 60 * 60) + (p1->startMinute * 60) + p1->startSecond;
	Reader2EndTime   = (p2->endHour * 60 * 60) + (p2->endMinute * 60) + p2->endSecond;
	diffTime         = Reader2EndTime - Reader1StartTime;
	CHECK(diffTime >= 2);

///////////////////////////////////////////// Test 3 //////////////////////////////////////////////////////

	// Reader 1  does not initiate transaction, reader 2 does not initiate transaction
	p1->activateTransaction = false;
	p2->activateTransaction = false;
	hThreads[0]             = CreateThread(NULL, THREAD_STACK_SIZE, SoftReaderThread1, p1, NULL, &iID[0]);
	hThreads[1]             = CreateThread(NULL, THREAD_STACK_SIZE, SoftReaderThread2, p2, NULL, &iID[1]);
	CHECK_EQUAL(WAIT_OBJECT_0, WaitForMultipleObjects(2, hThreads, TRUE, INFINITE));

	delete p1;
	delete p2;
}

DWORD WINAPI SoftReaderThread1(LPVOID p)
{
	ThreadParams * param = (ThreadParams *) p;

	// sanity check
	if ((((ThreadParams *) p)->sleepValue) > 10000)
		((ThreadParams *) p)->sleepValue = 10000;  // limit to 10 seconds

	SoftReaderManager srmng;
	DWORD             buflen = 1024;
	char              buf[1024];
	char              *myHardName = "MySoftReader";
	DWORD             len         = (DWORD) strlen(myHardName);
	memcpy(buf, myHardName, len + 1);
	buf[len + 1] = '\0';
	size_t        result = srmng.createSoftReaders(buf, &buflen);
	char          softReaderName[1024];
	strcpy_s(softReaderName, 1023, (char*) &buf[0]);
	SoftReader    *sr = srmng.getSoftReaderByName(softReaderName, N_SOFT);
	SoftCard      *sc = sr->createSoftCard(1234);

	struct tm     *current;
	time_t        now;

	unsigned char sendBuf[1024];
	DWORD         sendBufLen = sizeof(sendBuf);

	unsigned char recvBuf[1024];
	DWORD         recvBufLen = sizeof(recvBuf);

	clock_t       start = clock();
	long          tx;
	if (param->activateTransaction)
		tx = sr->beginTransaction();
	else
		Sleep(NAP_LENGTH); // allow the sleeping thread a little time to start sleeping
	time(&now);
	current            = localtime(&now);
	param->startHour   = current->tm_hour;
	param->startMinute = current->tm_min;
	param->startSecond = current->tm_sec;

	const eidmw::pcscproxy::APDURequest apduReq = eidmw::pcscproxy::APDURequest::createAPDURequest(sendBuf, sendBufLen);

	Sleep(((ThreadParams *) p)->sleepValue);
	sr->transmit(apduReq, recvBuf, &recvBufLen);

	time(&now);
	current          = localtime(&now);
	param->endHour   = current->tm_hour;
	param->endMinute = current->tm_min;
	param->endSecond = current->tm_sec;

	if (param->activateTransaction)
		sr->endTransaction(tx);

	return 0;
}

DWORD WINAPI SoftReaderThread2(LPVOID p)
{
	ThreadParams * param = (ThreadParams *) p;

	// sanity check
	if ((((ThreadParams *) p)->sleepValue) > 10000)
		((ThreadParams *) p)->sleepValue = 10000;  // limit to 10 seconds

	SoftReaderManager srmng;
	DWORD             buflen = 1024;
	char              buf[1024];
	char              *myHardName = "MySoftReader";
	DWORD             len         = (DWORD) strlen(myHardName);
	memcpy(buf, myHardName, len + 1);
	buf[len + 1] = '\0';
	size_t                              result = srmng.createSoftReaders(buf, &buflen);
	char                                softReaderName[1024];
	strcpy_s(softReaderName, 1023, (char*) &buf[0]);
	SoftReader                          *sr = srmng.getSoftReaderByName(softReaderName, N_SOFT);
	if (sr == NULL)
		printf("ERR: srmng.getSoftReaderByName() returned NULL (visibility in control file != HIDE_REAL ?)\n");
	SoftCard                            *sc = sr->createSoftCard(1234);

	struct tm                           *current;
	time_t                              now;

	unsigned char                       sendBuf[1024];
	DWORD                               sendBufLen = sizeof(sendBuf);

	unsigned char                       recvBuf[1024];
	DWORD                               recvBufLen = sizeof(recvBuf);

	const eidmw::pcscproxy::APDURequest apduReq = eidmw::pcscproxy::APDURequest::createAPDURequest(sendBuf, sendBufLen);
	long                                tx;
	if (param->activateTransaction)
		tx = sr->beginTransaction();
	else
		Sleep(NAP_LENGTH); // allow the sleeping thread a little time to start sleeping

	time(&now);
	current = localtime(&now);

	param->startHour   = current->tm_hour;
	param->startMinute = current->tm_min;
	param->startSecond = current->tm_sec;

	Sleep(((ThreadParams *) p)->sleepValue);

	sr->transmit(apduReq, recvBuf, &recvBufLen);

	if (param->activateTransaction)
		sr->endTransaction(tx);

	time(&now);
	current = localtime(&now);

	param->endHour   = current->tm_hour;
	param->endMinute = current->tm_min;
	param->endSecond = current->tm_sec;

	return 0;
}

#endif

// TODO: port tests to Linux/Mac

