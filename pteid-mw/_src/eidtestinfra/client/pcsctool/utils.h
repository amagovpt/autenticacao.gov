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
#include <stdlib.h>
#include <string>

int hex2bin(const char *hex, unsigned char *buf, size_t *len);
void dumphex(const char *msg, const unsigned char *buf, size_t len);

void delayMS(int x);

void StoreAPDUs(const char *chipNr, int idx,
	const unsigned char *sendBuf, size_t sendBufLen,
	const unsigned char *recvBuf, size_t recvBufLen);
int compareAPDUS(const char *cmdApdu, const unsigned char *respApdu, size_t respApduLen,
	const char *chipnrForCompare, int idx);

#define CHECK_PCSC_RET(func, ret) \
	if (SCARD_S_SUCCESS != ret)	  \
		printf("ERR line %d: %s returned 0x%0x (=%d) (%s)\n", __LINE__, func, (unsigned int) ret, (unsigned int) ret, pcscerr2string(ret));

#define CHECK_PCSC_RET_PASS(toReturn) \
	if (SCARD_S_SUCCESS != ret)		  \
		returnValue = toReturn;

#define CHECK_PCSC_RET_FAIL(toReturn) \
	if (SCARD_S_SUCCESS == ret)		  \
		returnValue = toReturn;

