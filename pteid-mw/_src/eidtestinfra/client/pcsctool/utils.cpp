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

#include "utils.h"
#include <cstring>
#ifdef _WIN32
#include <windows.h>
#endif

#define ISHEXCHAR(c)    ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))

static int hexchar2bin(char h)
{
	if (h >= '0' && h <= '9')
		return h - '0';
	if (h >= 'a' && h <= 'f')
		return h - 'a' + 10;
	return h - 'A' + 10;
}

int hex2bin(const char *hex, unsigned char *buf, size_t *len)
{
	size_t count = 0;

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
			return -1;
		}

		hex += 2;
	}

	if (count >= *len)
	{
		printf("ERR: hex string too large (should be max %d bytes\n", (int) *len);
		return -1;
	}

	*len = count;

	return 0;
}

void dumphex(const char *msg, const unsigned char *buf, size_t len)
{
	if (NULL != msg)
		printf("%s", msg);
	for (size_t i = 0; i < len; i++)
		printf("%02X ", buf[i]);
	printf("\n");
}

int compareAPDUS(const char *cmdApdu, const unsigned char *respApdu, size_t respApduLen,
	const char *chipnrForCompare, int idx)
{
	// 1. First compare the command APDUS to make sure they are the same

	unsigned char buf1[280];
	size_t        buf1Len = sizeof(buf1);
	unsigned char buf2[280];
	size_t        buf2Len = sizeof(buf2);

	if (0 != hex2bin(cmdApdu, buf1, &buf1Len))
		return -1;

	char filename[100];
	sprintf(filename, "%s_sent%d", chipnrForCompare, idx); // fixme: use a safe function
	FILE *f = fopen(filename, "rb");                       // fixme: use a safe function
	if (NULL == f)
	{
		printf("Couldn't open file \"%s\", can't continue the test\n", filename);
		return -1;
	}
	buf2Len = fread(buf2, 1, sizeof(buf2), f);
	fclose(f);

	if (buf1Len != buf2Len || memcmp(buf1, buf2, buf1Len) != 0)
	{
		printf("The command APDUS differ, can't continue the test\n");
		dumphex(" - ", buf1, buf1Len);
		dumphex(" - ", buf2, buf2Len);
		return -1;
	}

	// 2. Compare the response APDUS

	sprintf(filename, "%s_recv%d", chipnrForCompare, idx); // fixme: use a safe function
	f = fopen(filename, "rb");                             // fixme: use a safe function
	if (NULL == f)
	{
		printf("Couldn't open file \"%s\", can't continue the test\n", filename);
		return -1;
	}
	buf2Len = fread(buf2, 1, sizeof(buf2), f);
	fclose(f);

	if (respApduLen != buf2Len || memcmp(respApdu, buf2, respApduLen) != 0)
	{
		printf("ERR: different response to \"%s\"\n", cmdApdu);
		dumphex("  - virt. card: ", buf2, buf2Len);
		dumphex("  - real card:  ", respApdu, respApduLen);
		return 1;
	}

	return 0;
}

void StoreAPDUs(const char *chipNr, int idx,
	const unsigned char *sendBuf, size_t sendBufLen,
	const unsigned char *recvBuf, size_t recvBufLen)
{
	char filename[100];
	sprintf(filename, "%s_sent%d", chipNr, idx); // fixme: use a safe function
	FILE *f = fopen(filename, "wb");             // fixme: use a safe function
	if (f)
	{
		size_t len = fwrite(sendBuf, 1, sendBufLen, f);
		len = 0; // to avoid compiler warning
		fclose(f);
	}
	sprintf(filename, "%s_recv%d", chipNr, idx); // fixme: use a safe function
	f = fopen(filename, "wb");                   // fixme: use a safe function
	if (f)
	{
		size_t len = fwrite(recvBuf, 1, recvBufLen, f);
		len = 0; // to avoid compiler warning
		fclose(f);
	}
}

void delayMS(int x)
{
#ifdef _WIN32
	Sleep(x);
#else
	usleep(1000 * x);
#endif
}
