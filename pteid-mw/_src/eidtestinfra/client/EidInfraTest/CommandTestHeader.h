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

#ifndef COMMAND_TEST_HEADER_H
#define COMMAND_TEST_HEADER_H

/**
 * Test tool for PCSC commands and for the APDUs inside an SCardTransmit().
 */
#include "utils.h"

#include "pcscerr2string.h"
#ifdef _WIN32
#include <windows.h>
#else
#include <wintypes.h>
#endif
#include <winscard.h>
#include <vector>
#include "SoftReaderManager.h"
#include "SoftReader.h"
#include "SoftCard.h"
#include <UnitTest++.h>
#include "Control.h"

#pragma pack(1)

#define CHECK_COMMANDS    0

#define ISHEXCHAR(c)              ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))

#define DebugMessage(str)         OutputDebugString(str)

#ifndef _WIN32
#define OutputDebugString(str)    printf("%s", str);
#endif

using namespace std;
using namespace eidmw::pcscproxy;

int hexchar2bin(char h);

enum READER {
	VIRTUAL_CARD,
	PHYSICAL_CARD
};

typedef struct {
	char       command;
	int        readerNr;
	int        apduCount;
	const char **apdus;
	const char *chipnr;
} Params;

#define COMMAND_LENGTH        2408
#define RESULT_LENGTH         8192
#define READER_NAME_LENGTH    1024

#endif
