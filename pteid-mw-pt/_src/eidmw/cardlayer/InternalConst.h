/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2009 FedICT.
 * Copyright (C) 2019 Caixa Magica Software.
 * Copyright (C) 2011 Vasco Silva - <vasco.silva@caixamagica.pt>
 * Copyright (C) 2012 lmcm - <lmcm@caixamagica.pt>
 * Copyright (C) 2016 André Guerreiro - <aguerreiro1985@gmail.com>
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
#pragma once

#include "InternalConst.h"

namespace eIDMW {

const unsigned long PP_CAP_VERIFY = 0x01;
const unsigned long PP_CAP_CHANGE = 0x02;

const static unsigned long READERS_BUF_LEN = 600;

typedef struct {
	unsigned long lFileLen;
	unsigned long lReadPINRef;	// 0 means 'no PIN needed' or 'unknown'
	unsigned long lWritePINRef; // 0 means 'no PIN needed' or 'unknown'
} tFileInfo;

const unsigned long MAX_APDU_READ_LEN = 256;
const unsigned long MAX_APDU_WRITE_LEN = 255;
// Max APDU size of the IAS applet
const unsigned long MAX_APDU_LEN = 256;
// Some readers may need a larger buffer because of weird Windows drivers
const unsigned long APDU_BUF_LEN = 1024;

const unsigned long CTRL_BUF_LEN = 258; // Fixme: this won't be enough for a pinpad init !!!

typedef enum {
	DONT_CACHE,
	SIMPLE_CACHE,	// for data that can never be modified
	CERT_CACHE,		// for certs that can be modified
	CHECK_16_CACHE, // for data where checking 16 bytes is enough
	CHECK_SERIAL,	// for data where the card serial number should be present
} tCacheAction;

typedef struct {
	tCacheAction action;
	unsigned long ulOffset; // for CHECK_16_CACHE
} tCacheInfo;

typedef enum {
	DONT_SELECT_APPLET,
	ALW_SELECT_APPLET, // Always select the applet before doing a Select file
	TRY_SELECT_APPLET, // Try to select the applet in case of problems;
					   // and if that worked then do it all the time
} tSelectAppletMode;

} // namespace eIDMW
