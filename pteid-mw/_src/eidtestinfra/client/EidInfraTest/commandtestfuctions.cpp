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

#define TRANSMIT_DEBUG    0

#include "pcscerr2string.h"
#include "utils.h"
#include "util.h"
#include "CommandTestHeader.h"
#ifndef _WIN32
#define SCARD_READERSTATEA       SCARD_READERSTATE
#define SCardListReadersA        SCardListReaders
#define SCardConnectA            SCardConnect
#define SCardGetStatusChangeA    SCardGetStatusChange
#include <wintypes.h>
#endif

#define CHIP_NR_LEN    16

void dumphex2(const char *msg, const unsigned char *buf, size_t len);
static long SCardTransmit2(IN SCARDHANDLE hCard,
	IN LPCSCARD_IO_REQUEST pioSendPci, IN LPCBYTE pbSendBuffer, IN DWORD cbSendLength,
	IN OUT LPSCARD_IO_REQUEST pioRecvPci, OUT LPBYTE pbRecvBuffer, IN OUT LPDWORD pcbRecvLength);
static const char *GetChipNrFromCard(SCARDHANDLE hCard, char *chipNrBuf);

static const char *listReaders(SCARDCONTEXT ctx, char *readerList, size_t readerListSize,
	bool printList, int readerNr)
{
	DWORD      dwLen       = (DWORD) readerListSize;
	const char *readerName = NULL;
	char       debugString[2048];

	long       ret = SCardListReadersA(ctx, NULL, readerList, &dwLen);
	CHECK_PCSC_RET("SCardListReaders", ret);
	if (SCARD_S_SUCCESS == ret)
	{
		if (dwLen < 2)
		{
			printf("No reader found, exiting\n");
			DebugMessage("No reader found, exiting\n");
		}
		else
		{
			if (printList)
			{
				printf("Reader list:\n");
				DebugMessage("Reader list:\n");
			}
			int readerCount = 0;
			while (readerList[0] != '\0')
			{
				if (printList)
				{
					printf("  %d : %s\n", readerCount, readerList);
					sprintf_s(debugString, 2047, "  %d : %s\n", readerCount, readerList);
					DebugMessage(debugString);
				}
				if (readerCount == readerNr)
					readerName = readerList;
				readerList += strlen(readerList) + 1;
				readerCount++;
			}
		}
	}

	if (readerNr > 0 && NULL == readerName)
	{
		printf("ERR: readernr (%d) too high, not enough readers present\n", readerNr);
		sprintf_s(debugString, 2047, "ERR: readernr (%d) too high, not enough readers present\n", readerNr);
		DebugMessage(debugString);
	}

	return readerName;
}

#if CHECK_COMMANDS

static long sendAPDUS(SCARDCONTEXT ctx, const char *readerName, int apduCount, const char **apdus)
{
	SCARDHANDLE hCard;
	DWORD       protocol;
	char        debugString[2048];

	printf("Using reader \"%s\"\n\n", readerName);
	sprintf_s(debugString, 2047, "Using reader \"%s\"\n\n", readerName);
	DebugMessage(debugString);

	long ret = SCardConnectA(ctx, readerName,
		SCARD_SHARE_SHARED, SCARD_PROTOCOL_T0, &hCard, &protocol);
	CHECK_PCSC_RET("SCardConnect", ret);
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
		{
			printf("ERR: APDU should be at least 4 bytes\n");
			DebugMessage("ERR: APDU should be at least 4 bytes\n");
		}
		else if (sendBufLen > 5 && (5 + sendBuf[4] != sendBufLen))
		{
			printf("ERR: wrong P3 byte in case 3 APDU\n");
			DebugMessage("ERR: wrong P3 byte in case 3 APDU\n");
		}
		else
		{
			if (doDump)
				dumphex2("  - sending ", sendBuf, sendBufLen);

#if TRANSMIT_DEBUG
			long ret = SCardTransmit2(hCard, &g_rgSCardT0Pci, sendBuf, (DWORD) sendBufLen, NULL, recvBuf, recvBufLen);
#else
			long ret = SCardTransmit(hCard, &g_rgSCardT0Pci, sendBuf, (DWORD) sendBufLen, NULL, recvBuf, recvBufLen);
#endif
			CHECK_PCSC_RET("SCardTransmit", ret);

			if (SCARD_S_SUCCESS == ret)
			{
				if (doDump)
				{
					dumphex2("    received ", recvBuf, *recvBufLen);
					printf("\n");
					DebugMessage("\n");
				}

				if (NULL != chipNr)
					StoreAPDUs(chipNr, idx, sendBuf, sendBufLen, recvBuf, *recvBufLen);

				return 0; // success
			}
		}
	}

	return -1; // failed
}
#endif //#if CHECK_COMMANDS

static std:: vector<BYTE> sendAPDU2(SCARDHANDLE hCard, const char *apdu,
	unsigned char *recvBuf, DWORD *recvBufLen,
	const char *chipNr, int idx, bool doDump)
{
	unsigned char      sendBuf[280];
	size_t             sendBufLen = sizeof(sendBuf);
	std:: vector<BYTE> result;

	// Hex string -> byte array
	if (0 == hex2bin(apdu, sendBuf, &sendBufLen))
	{
		// Check the APDU
		if (sendBufLen < 4)
		{
			printf("ERR: APDU should be at least 4 bytes\n");
			DebugMessage("ERR: APDU should be at least 4 bytes\n");
		}
		else if (sendBufLen > 5 && (size_t) (5 + sendBuf[4]) != sendBufLen)
		{
			printf("ERR: wrong P3 byte in case 3 APDU\n");
			DebugMessage("ERR: wrong P3 byte in case 3 APDU\n");
		}
		else
		{
			if (doDump)
				dumphex2("  - sending ", sendBuf, sendBufLen);

			//long ret = SCardTransmit(hCard,&g_rgSCardT0Pci, sendBuf, (DWORD) sendBufLen,NULL, recvBuf, recvBufLen);
			long ret = SCardTransmit2(hCard, &g_rgSCardT0Pci, sendBuf, (DWORD) sendBufLen, NULL, recvBuf, recvBufLen);

			CHECK_PCSC_RET("SCardTransmit", ret);

			if (SCARD_S_SUCCESS == ret)
			{
				for (int i = 0; i < (int) *recvBufLen; i++)
				{
					result.push_back(recvBuf[i]);
				}
				dumphex2("  - received ", recvBuf, (int) *recvBufLen);

				return result; // success
			}
		}
	}

	return result; // failed
}



static std:: vector<BYTE> sendAPDU(const char *apdu)
{
	SCARDHANDLE   hCard;
	unsigned char *recvBuf;
	//char debugString[2048];

	unsigned char      sendBuf[280];
	size_t             sendBufLen = sizeof(sendBuf);
	std:: vector<BYTE> result;
	SCARDCONTEXT       ctx;
	const char         *readerName;

	long               ret = SCardEstablishContext(SCARD_SCOPE_USER, NULL, NULL, &ctx);
	CHECK_PCSC_RET("SCardEstablishContext", ret);
	if (SCARD_S_SUCCESS == ret)
	{
		char readerList[2000];
		readerName = listReaders(ctx, readerList, sizeof(readerList), true, -1);

		DWORD protocol;
		char  *chipnrForCompare = NULL;

		long  ret = SCardConnectA(ctx, readerName,
			SCARD_SHARE_SHARED, SCARD_PROTOCOL_T0, &hCard, &protocol);
		CHECK_PCSC_RET("SCardConnect", ret);
		if (SCARD_S_SUCCESS == ret)
		{
			DWORD         recvBufLen = sizeof(recvBuf);
			unsigned char recvBuf[258];

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
					//return -1;
				}
			} //if (NULL == chipnrForCompare)

			recvBufLen = (DWORD) sizeof(recvBuf); //0x00000102

			if (0 == hex2bin(apdu, sendBuf, &sendBufLen))
			{
				// Check the APDU
				if (sendBufLen < 4)
				{
					printf("ERR: APDU should be at least 4 bytes\n");
					DebugMessage("ERR: APDU should be at least 4 bytes\n");
				}
				else if (sendBufLen > 5 && (size_t) (5 + sendBuf[4]) != sendBufLen)
				{
					printf("ERR: wrong P3 byte in case 3 APDU\n");
					DebugMessage("ERR: wrong P3 byte in case 3 APDU\n");
				}
				else
				{
					long ret = SCardTransmit2(hCard, &g_rgSCardT0Pci, sendBuf, (DWORD) sendBufLen, NULL, recvBuf, &recvBufLen);

					if (SCARD_S_SUCCESS == ret)
					{
						for (int i = 0; i < (int) recvBufLen; i++)
						{
							result.push_back(recvBuf[i]);
						}
						return result; // success
					}
				}
			}
		}
	}
	return result; // failed
}


static const char HEX_TABLE[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };


static const char *GetChipNrFromCard(SCARDHANDLE hCard, char *chipNrBuf)
{
	unsigned char recvBuf[258];
	DWORD         recvBufLen = (DWORD) sizeof(recvBuf);

	memset(chipNrBuf, 0, 2 * CHIP_NR_LEN + 1);

	// Send "Get Card Data" command, the first 16 bytes that are returned is the chip nr
	std:: vector<BYTE> ret = sendAPDU2(hCard, "80:E4:00:00:1C", recvBuf, &recvBufLen, NULL, 0, false);
	if (ret.size() == 0) //== -1 w
		return NULL;
	else if (recvBufLen != 28 + 2)
	{
		printf("ERR: Get Card Data command returned %d bytes instead of 30\n", (int) recvBufLen);
		DebugMessage("ERR: Get Card Data command returned %d bytes instead of 30\n");

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

void dumphex2(const char *msg, const unsigned char *buf, size_t len)
{
	char debugString[4096];
	if (NULL != msg)
	{
		printf("%s", msg);
		DebugMessage(msg);
	}
	for (size_t i = 0; i < len; i++)
	{
		printf("%02X ", buf[i]);
		sprintf_s(debugString, 2047, "%02X ", buf[i]);
		DebugMessage(debugString);
	}
	printf("\n");
	DebugMessage("\n");
}

int hex2bin2(const char *hex, unsigned char *buf, size_t *len)
{
	size_t count = 0;
	char   debugString[2048];

	while (*hex != '\0' && count < *len)
	{
		// skip all that is not a hex char
		while (!ISHEXCHAR(*hex) && *hex != '\0')
			hex++;
		if (*hex == '\0')
			break;

		// The nex char should be a hex char too
		if (ISHEXCHAR(hex[1]))
			buf[count++] = (unsigned char)(16 * hexchar2bin(*hex) + hexchar2bin(hex[1]));
		else
		{
			printf("ERR: invalid input in hex string: expected a hex char after '%c'\n", *hex);
			sprintf_s(debugString, 2047, "ERR: invalid input in hex string: expected a hex char after '%c'\n", *hex);
			DebugMessage(debugString);
			return -1;
		}

		hex += 2;
	}

	if (count >= *len)
	{
		printf("ERR: hex string too large (should be max %d bytes\n", (int) *len);
		sprintf_s(debugString, 2047, "ERR: hex string too large (should be max %d bytes\n", *len);
		DebugMessage(debugString);
		return -1;
	}

	*len = count;

	return 0;
}

int hexchar2bin(char h)
{
	if (h >= '0' && h <= '9')
		return h - '0';
	if (h >= 'a' && h <= 'f')
		return h - 'a' + 10;
	return h - 'A' + 10;
}

#if CHECK_COMMANDS

LONG SCardConnectA2(IN SCARDCONTEXT hContext, IN LPCSTR szReader, IN DWORD dwShareMode, IN DWORD dwPreferredProtocols,
	OUT LPSCARDHANDLE phCard, OUT LPDWORD pdwActiveProtocol)
{
	typedef LONG (__stdcall * pS)(IN SCARDCONTEXT hContext, IN LPCSTR szReader, IN DWORD dwShareMode, IN DWORD dwPreferredProtocols,
								  OUT LPSCARDHANDLE phCard, OUT LPDWORD pdwActiveProtocol);
	pS   pps = (pS) p[7];

	LONG result = 0;

	//lookup SoftReader by soft name
	//eidmw::pcscproxy::SoftReader *sr = srmngr->getSoftReaderByName(szReader, eidmw::pcscproxy::N_SOFT);
	//		SoftReader *sr = new SoftReader(*iter, softname);

	std::string                  hardname = "ACS ACR38U 0";
	std::string                  softname = "!Virtual ACS ACR38U 0";
	eidmw::pcscproxy::SoftReader *sr      = new SoftReader(hardname, softname);

	if (sr)
	{
		const std::string hardname = sr->getHardReaderName();
		//result = pps(hContext, hardname.c_str(), dwShareMode, dwPreferredProtocols, phCard, pdwActiveProtocol);
		*phCard            = 3925934080;
		*pdwActiveProtocol = 1;
		if (SCARD_S_SUCCESS == result)
		{
			//connect to hard card was successful so now create a soft card
			*phCard = sr->createSoftCard(*phCard)->getSoftHandle();
		}
	}
	else
	{
		//apparently szReader is a hard name so we send the request to the
		//original dll and be done with it
		result = pps(hContext, szReader, dwShareMode, dwPreferredProtocols, phCard, pdwActiveProtocol);
	}

	return result;
}
#endif //#if CHECK_COMMANDS

// SCardTransmit2
static long SCardTransmit2(IN SCARDHANDLE hCard, IN LPCSCARD_IO_REQUEST pioSendPci, IN LPCBYTE pbSendBuffer, IN DWORD cbSendLength,
	IN OUT LPSCARD_IO_REQUEST pioRecvPci, OUT LPBYTE pbRecvBuffer, IN OUT LPDWORD pcbRecvLength)
{
	LONG result = 0;

	BYTE hardNumber[16] =
	{
		0x53,
		0x4C,
		0x49,
		0x4E,
		0x33,
		0x66,
		0x00,
		0x29,
		0x6C,
		0xFF,
		0x23,
		0x2C,
		0xF7,
		0x13,
		0x10,
		0x30
	};

	SoftReaderManager srmng;
	DWORD             buflen = 1024;
	char              buf[1024];

	const char        *myHardName = "MySoftReader";
	DWORD             len         = (DWORD) strlen(myHardName);
	memcpy(buf, myHardName, len + 1);
	buf[len + 1] = '\0';

	result = (LONG) srmng.createSoftReaders(buf, &buflen);

	char softReaderName[1024];
	strcpy_s(softReaderName, 1023, (char*) &buf[0]);

	SoftReader                 *sr = srmng.getSoftReaderByName(softReaderName, N_SOFT);
	eidmw::pcscproxy::SoftCard *sc = sr->createSoftCard(1234);
	if (sc == NULL)
		printf("ERR: SoftReader::createSoftCard() failed\n");

	if (sr)
	{
		const eidmw::pcscproxy::APDURequest apduReq = eidmw::pcscproxy::APDURequest::createAPDURequest(pbSendBuffer, cbSendLength);

		if (apduReq.isGetCardData())
		{
			/*
			   if(SCARD_S_SUCCESS != result)
			   {
				return result;
			   }
			 */
			//if no virt data loaded, then load it from file
//			if(!sr->getSoftCard()->isDataLoaded())
			{
				sr->getSoftCard()->loadFromFile("..\\_DocsInternal\\virtual_stephen.xml", hardNumber);
				/*
				   if(SCARD_S_SUCCESS != result)
				   {
					return result;
				   }
				 */
			}
			//send to the SOFT reader
			//Stephen new code change result = sr->getSoftCard()->transmit(pioSendPci, apduReq, pioRecvPci, pbRecvBuffer, pcbRecvLength);
			result = sr->transmit(apduReq, pbRecvBuffer, pcbRecvLength);
		}
		else if (apduReq.isReadBinary() || apduReq.isSelectFile() || apduReq.isGetResponse())
		{
			//if no data loaded then first do GET_CARD_DATA
			if (!sr->getSoftCard()->isDataLoaded())
			{
				// unsigned char localBuf[258];
				//TO_DO $$$ !!!!  std::string file = srmngr->getControl()->getSoftCardFileLocation(localBuf);
				sr->getSoftCard()->loadFromFile("..\\_DocsInternal\\virtual_stephen.xml", hardNumber);
			}
			//send to the SOFT reader
			//Stephen change to new code result = sr->getSoftCard()->transmit(pioSendPci, apduReq, pioRecvPci, pbRecvBuffer, pcbRecvLength);
			result = sr->transmit(apduReq, pbRecvBuffer, pcbRecvLength);
		}
		else
		{
			//an apdu the soft card does not process
			//send to the HARD reader
			return -1;
		}
	}
	else
	{
		//there is no softreader
		//send to the HARD reader
		// $$$!!!! result = pps(hCard, pioSendPci, pbSendBuffer, cbSendLength, pioRecvPci, pbRecvBuffer, pcbRecvLength);
	}

	return result;
}


