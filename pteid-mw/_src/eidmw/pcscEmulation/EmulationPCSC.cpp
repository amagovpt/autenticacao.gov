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
#ifdef UNICODE
#undef UNICODE
#endif

#ifdef WIN32
#include <windows.h>
#else
#define WINAPI
#define WINSCARDAPI
#endif

#include <winscard.h>

#include "EmulationPCSC.h"
#include "EmulationCardFactory.h"
#include "../common/Thread.h"
#include "../common/Mutex.h"
#include "../common/MWException.h"
#include "../common/eidErrors.h"
#include <memory>
#include <stdio.h>
#include <cstring>

#define IOCTL_SMARTCARD_SET_CARD_TYPE	SCARD_CTL_CODE(2060)

#ifndef LPSCARD_READERSTATEA
#define LPSCARD_READERSTATEA LPSCARD_READERSTATE_A

// needed for pcsclite version earlier than 1.4
#ifndef SCARD_E_NO_READERS_AVAILABLE
#define SCARD_E_NO_READERS_AVAILABLE 0x8010002E /** Cannot find smart card reader */
#endif
#ifndef SCARD_PROTOCOL_UNDEFINED
#define SCARD_PROTOCOL_UNDEFINED 0x00
#endif
#ifndef SCARD_STATE_UNPOWERED
#define SCARD_STATE_UNPOWERED 0x0400
#endif
#ifndef SCARD_CTL_CODE
#define SCARD_CTL_CODE(code)     (0x42000000 + (code))
#endif
#endif

using namespace eIDMW;

static unsigned long READER_COUNT = 10;

static SCARDCONTEXT contextCounter = 0;

// Trick: the handle for cards in reader x start from x * 10000,
// so if a handle = y, you know it's for reader (y / 10000)
static SCARDHANDLE tHandleCounters[10] = {0, 10000, 20000, 30000, 40000, 50000, 60000, 70000, 80000, 90000};

static SCARDHANDLE tLockedBy[10] = {0};
static CMutex tMutexes[10];

static unsigned int tuiStateChangeCount[10] = {0};
static unsigned int tuiLastInserted[10] = {0};
static unsigned int tuiLastRemoved[10] = {0};
static unsigned int tuiLastConnected[10] = {0};

static CEmulationCard *poPTeidV1Test1 = NULL;
static CEmulationCard *poPTeidV1Test2 = NULL;
static CEmulationCard *poPTeidV1Test3 = NULL;
static CEmulationCard *poPTeidV2Test1 = NULL;
static CEmulationCard *poSISTest1 = NULL;

static bool tbCardPresent[10];
static CEmulationCard *tpoCards[10];

static bool bInitializationDone = false;

////////////////////////////////////////////////////////////////////////////////


void Init(){

  if(bInitializationDone)
	  return;

  poPTeidV1Test1 = CEmulationCardFactory::getInstance(PTEID_V1_TEST1);
  poPTeidV1Test2 = CEmulationCardFactory::getInstance(PTEID_V1_TEST2);
  poPTeidV1Test3 = CEmulationCardFactory::getInstance(PTEID_V1_TEST3);
  poPTeidV2Test1 = CEmulationCardFactory::getInstance(PTEID_V2_TEST1);
  poSISTest1 = CEmulationCardFactory::getInstance(SIS_TEST1);

  tpoCards[0] = poPTeidV1Test1;
  tpoCards[1] = NULL;
  tpoCards[2] = poPTeidV1Test2;
  tpoCards[3] = poPTeidV2Test1;
  tpoCards[4] = poSISTest1;
  tpoCards[5] = poPTeidV1Test3;
  tpoCards[6] = NULL;
  tpoCards[7] = NULL;
  tpoCards[8] = NULL;
  tpoCards[9] = NULL;

  for (int i = 0; i < 10; i++)
	  tbCardPresent[i] = (tpoCards[i] != NULL);

  bInitializationDone = true;
}

void SetReaders(unsigned long ulCount)
{
  if( ! bInitializationDone )
	  Init();

	READER_COUNT = ulCount;

	if (READER_COUNT > 10)
		READER_COUNT = 10;
}

CEmulationCard *SetCard(unsigned long ulReader, CEmulationCard *poCard)
{
  if( ! bInitializationDone )
	  Init();

	if (ulReader < READER_COUNT)
	{
		if (tbCardPresent[ulReader])
		{
			//SetCardPresent(ulReader, false);
			//SetCardPresent(ulReader, true);
		}
		tpoCards[ulReader] = poCard;
		return poCard;
	}

	return NULL;
}

void SetCardPresent(unsigned long ulReader, bool bPresent)
{
  if( ! bInitializationDone )
	  Init();

	if ((ulReader < READER_COUNT) && (tbCardPresent[ulReader] != bPresent))
	{
		tbCardPresent[ulReader] = bPresent;
		tuiStateChangeCount[ulReader]++;
		if (bPresent)
			tuiLastInserted[ulReader] = tuiStateChangeCount[ulReader];
		else
			tuiLastRemoved[ulReader] = tuiStateChangeCount[ulReader];
	}
}

////////////////////////////////////////////////////////////////////////////////

// Which handle has locked the reader (SCardBeginTransaction, SCardReleaseTransaction)
SCARDHANDLE tLockHandles[10] = {0}; // 0 means 'not locked'

WINSCARDAPI LONG WINAPI SCardEstablishContext(
    IN  DWORD dwScope,
    IN  LPCVOID pvReserved1,
    IN  LPCVOID pvReserved2,
    OUT LPSCARDCONTEXT phContext)
{
  if( ! bInitializationDone )
	  Init();

	*phContext = contextCounter++;

	return SCARD_S_SUCCESS;
}

WINSCARDAPI LONG WINAPI SCardReleaseContext(
    IN      SCARDCONTEXT hContext)
{
  if( ! bInitializationDone )
		return SCARD_E_INVALID_HANDLE;

	if (hContext > contextCounter)
		return SCARD_E_INVALID_HANDLE;

	return SCARD_S_SUCCESS;
}

#if (! defined(WIN32)) && (! defined(SCardConnectA))
WINSCARDAPI LONG WINAPI SCardListReaders(
	IN     SCARDCONTEXT hContext,
	IN     LPCSTR mszGroups,
	OUT    LPTSTR mszReaders,
	IN OUT LPDWORD pcchReaders)
#else
WINSCARDAPI LONG WINAPI SCardListReadersA(
	IN     SCARDCONTEXT hContext,
	IN     LPCSTR mszGroups,
	OUT    LPTSTR mszReaders,
	IN OUT LPDWORD pcchReaders)
#endif
{

  if( ! bInitializationDone )
		return SCARD_E_INVALID_HANDLE;

	unsigned long i;

	if (hContext > contextCounter)
		return SCARD_E_INVALID_HANDLE;

	if (READER_COUNT == 0)
		return SCARD_E_NO_READERS_AVAILABLE;

	if (mszReaders == NULL)
		*pcchReaders = READER_COUNT * 18 + 1;
	else if (*pcchReaders < 18 * READER_COUNT + 1)
		return SCARD_E_INSUFFICIENT_BUFFER;
	else
	{
		for (i = 0; i < READER_COUNT; i++)
		{
			memcpy(mszReaders + 18 * i, "Emulated reader ", 16);
			mszReaders[18 * i + 16] = (char) (0x30 + i);
			mszReaders[18 * i + 17] = '\0';
		}
		mszReaders[18 * READER_COUNT] = '\0';
	}

	return SCARD_S_SUCCESS;
}

#if (! defined(WIN32)) && (! defined(SCardConnectA))
WINSCARDAPI LONG WINAPI SCardGetStatusChange(
	IN      SCARDCONTEXT hContext,
	IN      DWORD dwTimeout,
	IN  OUT LPSCARD_READERSTATEA rgReaderStates,
	IN      DWORD cReaders)
#else
WINSCARDAPI LONG WINAPI SCardGetStatusChangeA(
	IN      SCARDCONTEXT hContext,
	IN      DWORD dwTimeout,
	IN  OUT LPSCARD_READERSTATEA rgReaderStates,
	IN      DWORD cReaders)
#endif
{
	bool bSomethingChanged = false;

	if( ! bInitializationDone )
		return SCARD_E_INVALID_HANDLE;

	if (hContext > contextCounter)
		return SCARD_E_INVALID_HANDLE;

	for (DWORD count = 0; count <= dwTimeout; count++)
	{
		for (DWORD i = 0; i < cReaders; i++)
		{
			bool bPresent = tbCardPresent[i];
			bool bChanged = ((rgReaderStates[i].dwCurrentState & SCARD_STATE_PRESENT) == SCARD_STATE_PRESENT) ^ bPresent;
			bool bConnected = (tuiLastConnected[i] >= tuiStateChangeCount[i]);
			bSomethingChanged |= bChanged | (rgReaderStates[i].dwCurrentState == 0);
	
			rgReaderStates[i].dwEventState |= (bConnected ? 0 : SCARD_STATE_UNPOWERED);
#ifdef WIN32
			rgReaderStates[i].dwEventState |= SCARD_STATE_CHANGED;
			if (bChanged && (rgReaderStates[i].dwCurrentState & SCARD_STATE_PRESENT) && bPresent)
			{
				rgReaderStates[i].dwEventState = SCARD_STATE_EMPTY;
				rgReaderStates[i].dwEventState += 0x10000 * (tuiStateChangeCount[i] - 1);
			}
			else
			{
				rgReaderStates[i].dwEventState = bPresent ? SCARD_STATE_PRESENT : SCARD_STATE_EMPTY;
				rgReaderStates[i].dwEventState += 0x10000 * tuiStateChangeCount[i];
			}
#else
			rgReaderStates[i].dwEventState = (bChanged ? SCARD_STATE_CHANGED : 0);
			rgReaderStates[i].dwEventState |= (bPresent ? SCARD_STATE_PRESENT : SCARD_STATE_EMPTY);
			//rgReaderStates[i] += 0x10000 * tuiStateChangeCount[i]; // to be checked
#endif
		}

		if (bSomethingChanged)
			break;
		if (count < dwTimeout)
			CThread::SleepMillisecs(1);
	}

	return bSomethingChanged ? SCARD_S_SUCCESS : SCARD_E_TIMEOUT;
}

#if (! defined(WIN32)) && (! defined(SCardConnectA))
WINSCARDAPI LONG WINAPI SCardConnect(
	IN  SCARDCONTEXT hContext,
	IN  LPCSTR szReader,
	IN  DWORD dwShareMode,
	IN  DWORD dwPreferredProtocols,
	OUT LPSCARDHANDLE phCard,
	OUT LPDWORD pdwActiveProtocol)
#else
WINSCARDAPI LONG WINAPI SCardConnectA(
	IN  SCARDCONTEXT hContext,
	IN  LPCSTR szReader,
	IN  DWORD dwShareMode,
	IN  DWORD dwPreferredProtocols,
	OUT LPSCARDHANDLE phCard,
	OUT LPDWORD pdwActiveProtocol)
#endif
{

  if( ! bInitializationDone )
		return SCARD_E_INVALID_HANDLE;

	if (hContext > contextCounter)
		return SCARD_E_INVALID_HANDLE;

	if (szReader == NULL)
		return SCARD_E_INVALID_PARAMETER;

	int reader = szReader[strlen(szReader) - 1] - 0x30;
	if (reader < 0 || reader > 9)
		return SCARD_E_INVALID_PARAMETER; // szReader should end with 0 .. 9

	if (!tbCardPresent[reader])
		return SCARD_W_REMOVED_CARD;

	*phCard = ++tHandleCounters[reader];

	tuiLastConnected[reader] = tuiStateChangeCount[reader];

	return SCARD_S_SUCCESS;
}

WINSCARDAPI LONG WINAPI SCardDisconnect(
    IN      SCARDHANDLE hCard,
    IN      DWORD dwDisposition)
{

  if( ! bInitializationDone )
		return SCARD_E_INVALID_HANDLE;

	int reader = (int) (hCard / 10000);

	if (hCard > tHandleCounters[reader])
		return SCARD_E_INVALID_HANDLE;

	return SCARD_S_SUCCESS;
}

#if (! defined(WIN32)) && (! defined(SCardConnectA))
WINSCARDAPI LONG WINAPI SCardStatus(
	IN     SCARDHANDLE hCard,
	OUT    LPTSTR szReaderName,
	IN OUT LPDWORD pcchReaderLen,
	OUT    LPDWORD pdwState,
	OUT    LPDWORD pdwProtocol,
	OUT    LPBYTE pbAtr,
	IN     OUT LPDWORD pcbAtrLen)
#else
WINSCARDAPI LONG WINAPI SCardStatusA(
	IN     SCARDHANDLE hCard,
	OUT    LPTSTR szReaderName,
	IN OUT LPDWORD pcchReaderLen,
	OUT    LPDWORD pdwState,
	OUT    LPDWORD pdwProtocol,
	OUT    LPBYTE pbAtr,
	IN     OUT LPDWORD pcbAtrLen)
#endif
{

  if( ! bInitializationDone )
		return SCARD_E_INVALID_HANDLE;

	int reader = (int) (hCard / 10000);

	if (hCard > tHandleCounters[reader])
		return SCARD_E_INVALID_HANDLE;

	if (*pcchReaderLen < 18 && szReaderName != NULL)
		return SCARD_E_INVALID_PARAMETER;
	if (szReaderName != NULL)
	{
		*pcchReaderLen = 18;
		memcpy(szReaderName, "Emulated reader ", 16);
		szReaderName[16] = (char) (0x30 + reader);
		szReaderName[17] = '\0';
	}

	*pdwState = tbCardPresent[reader] ? SCARD_PRESENT : SCARD_ABSENT;

	*pdwProtocol = SCARD_PROTOCOL_T0;

	if (tbCardPresent[reader])
	{
		CByteArray oATR = tpoCards[reader]->GetATR();
		if (*pcbAtrLen < oATR.Size())
			return SCARD_E_INVALID_PARAMETER;
		*pcbAtrLen = oATR.Size();
		memcpy(pbAtr, oATR.GetBytes(), *pcbAtrLen);
	}
	else
		*pcbAtrLen = 0;

	return SCARD_S_SUCCESS;
}

WINSCARDAPI LONG WINAPI SCardBeginTransaction(
    IN      SCARDHANDLE hCard)
{

  if( ! bInitializationDone )
		return SCARD_E_INVALID_HANDLE;

	int reader = (int) (hCard / 10000);

	if (hCard > tHandleCounters[reader])
		return SCARD_E_INVALID_HANDLE;

	if (!tbCardPresent[reader])
		return SCARD_W_REMOVED_CARD;

	bool bWait = true;
	while (bWait)
	{
		bWait = false;
		tMutexes[reader].Lock();
		if (tLockedBy[reader] == 0)
			tLockedBy[reader] = hCard; // not locked yet -> take the lock
		else if (tLockedBy[reader] != hCard)
		{
			// locked by another card -> wait a little and try again
			CThread::SleepMillisecs(100);
			bWait = true;
		}
		// else // already locked by this handle -> what to do ???
		tMutexes[reader].Unlock();
	}

	return SCARD_S_SUCCESS;
}

WINSCARDAPI LONG WINAPI SCardEndTransaction(
    IN      SCARDHANDLE hCard,
    IN      DWORD dwDisposition)
{

  if( ! bInitializationDone )
		return SCARD_E_INVALID_HANDLE;

	int reader = (int) (hCard / 10000);

	if (hCard > tHandleCounters[reader])
		return SCARD_E_INVALID_HANDLE;

	if (!tbCardPresent[reader])
		return SCARD_W_REMOVED_CARD;

	if (tHandleCounters[reader] != hCard)
		return SCARD_E_NOT_TRANSACTED;

	tMutexes[reader].Lock();
	tLockedBy[reader] = 0;
	tMutexes[reader].Unlock();

	return SCARD_S_SUCCESS;
}

WINSCARDAPI LONG WINAPI SCardTransmit(
    IN SCARDHANDLE hCard,
    IN LPCSCARD_IO_REQUEST pioSendPci,
    IN LPCBYTE pbSendBuffer,
    IN DWORD cbSendLength,
    IN OUT LPSCARD_IO_REQUEST pioRecvPci,
    OUT LPBYTE pbRecvBuffer,
    IN OUT LPDWORD pcbRecvLength)
{

  if( ! bInitializationDone )
		return SCARD_E_INVALID_HANDLE;

	int reader = (int) (hCard / 10000);

	if (hCard > tHandleCounters[reader])
		return SCARD_E_INVALID_HANDLE;

	if (!tbCardPresent[reader])
		return SCARD_W_REMOVED_CARD;

	if (tLockedBy[reader] != 0 && tLockedBy[reader] != hCard)
	{
		while (tLockedBy[reader] != 0)
			CThread::SleepMillisecs(20);
	}

	return tpoCards[reader]->SCardTransmit(pbSendBuffer, cbSendLength,
		pbRecvBuffer, pcbRecvLength);
}

WINSCARDAPI LONG WINAPI SCardControl(
    IN      SCARDHANDLE hCard,
    IN      DWORD dwControlCode,
    IN      LPCVOID lpInBuffer,
    IN      DWORD nInBufferSize,
    OUT     LPVOID lpOutBuffer,
    IN      DWORD nOutBufferSize,
    OUT     LPDWORD lpBytesReturned)
{
  if( ! bInitializationDone )
		return SCARD_E_INVALID_HANDLE;

	int reader = (int) (hCard / 10000);

	if (hCard > tHandleCounters[reader])
		return SCARD_E_INVALID_HANDLE;

	if (!tbCardPresent[reader])
		return SCARD_W_REMOVED_CARD;

	// This code is sent by the CAL and is intended for the PF
	// Normally the PF broker catches this code the PF is not
	// running so it's not sent to the real PCSC function.
	// But since this lib is called without the PF broker,
	// we have to do here the same as in the PF broker..
	if (dwControlCode == 0xe1dcace)
	{
		((unsigned char *) lpOutBuffer)[0] = 0x90;
		((unsigned char *) lpOutBuffer)[1] = 0x00;
		*lpBytesReturned = 2;

		return SCARD_S_SUCCESS;
	}

	// For SIS cards in a ACR38U reader, this code is sent
	// to change the 'mode' of the reader
	if (dwControlCode == IOCTL_SMARTCARD_SET_CARD_TYPE || dwControlCode == 0)
	{
		((unsigned char *) lpOutBuffer)[0] = 0x90;
		((unsigned char *) lpOutBuffer)[1] = 0x00;
		*lpBytesReturned = 2;

		return SCARD_S_SUCCESS;
	}

	printf("\nEmulationPCSC.SCardControl(): dunno what to do with control code = 0x%0x\n\n", dwControlCode);

	return SCARD_E_UNSUPPORTED_FEATURE;
}

