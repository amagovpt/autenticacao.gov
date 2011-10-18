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

#include "util.h"
#include "utils2.h"
#include "CommandTestHeader.h"

#define ISHEXCHAR(c)    ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))

static int hexchar2bin3(char h)
{
	if (h >= '0' && h <= '9')
		return h - '0';
	if (h >= 'a' && h <= 'f')
		return h - 'a' + 10;
	return h - 'A' + 10;
}

int hex2bin3(const char *hex, unsigned char *buf, size_t *len)
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
			buf[count++] = (unsigned char)(16 * hexchar2bin3(*hex) + hexchar2bin3(hex[1]));
		else
		{
			sprintf_s(debugString, 2047, "ERR: invalid input in hex string: expected a hex char after '%c'\n", *hex);
			DebugMessage(debugString);
			return -1;
		}

		hex += 2;
	}

	if (count >= *len)
	{
		sprintf_s(debugString, 2047, "ERR: hex string too large (should be max %d bytes\n", *len);
		DebugMessage(debugString);
		return -1;
	}

	*len = count;

	return 0;
}

void dumphex3(const char *msg, const unsigned char *buf, size_t len)
{
	if (NULL != msg)
	{
		printf("%s", msg);
	}
	for (size_t i = 0; i < len; i++)
	{
		printf("%02X ", buf[i]);
	}
	printf("\n");
}

