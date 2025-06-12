/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2009 FedICT.
 * Copyright (C) 2019 Caixa Magica Software.
 * Copyright (C) 2011 Vasco Silva - <vasco.silva@caixamagica.pt>
 * Copyright (C) 2012 lmcm - <lmcm@caixamagica.pt>
 * Copyright (C) 2012 Rui Martinho - <rui.martinho@ama.pt>
 * Copyright (C) 2016-2018 André Guerreiro - <aguerreiro1985@gmail.com>
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

#ifndef CARDLAYERCONST_H
#define CARDLAYERCONST_H
namespace eIDMW {
/**
 * The maximum number of readers; if more readers are present
 * only the first MAX_READERS that are found will used.
 */
const static unsigned long MAX_READERS = 8;

#define _UNBLOCK_FLAG_NEW_PIN 1
#define _UNBLOCK_FLAG_PUK_MERGE 2 // Only on pinpad readers

typedef enum {
	DISCONNECT_LEAVE_CARD = 0,
	DISCONNECT_RESET_CARD = 1,
} tDisconnectMode;

typedef enum {
	FILE_OP_READ,
	FILE_OP_WRITE,
} tFileOperation;

typedef enum { PIN_OP_VERIFY, PIN_OP_CHANGE, PIN_OP_RESET, PIN_OP_RESET_NO_PUK } tPinOperation;

enum PaceSecretType {
	PACEMRZ = 1, // Only PACE MRZ and CAN can be used with IAS v5 cards
	PACECAN,
	PACEPIN, // Not available
	PACEPUK, // Not available
	PACERAW,
};

const unsigned long SIGN_ALGO_RSA_RAW = 0x01;
const unsigned long SIGN_ALGO_RSA_PKCS = 0x02;
const unsigned long SIGN_ALGO_MD5_RSA_PKCS = 0x04;
const unsigned long SIGN_ALGO_SHA1_RSA_PKCS = 0x08;
const unsigned long SIGN_ALGO_SHA256_RSA_PKCS = 0x10;
const unsigned long SIGN_ALGO_SHA384_RSA_PKCS = 0x20;
const unsigned long SIGN_ALGO_SHA512_RSA_PKCS = 0x40;
const unsigned long SIGN_ALGO_RIPEMD160_RSA_PKCS = 0x80;
const unsigned long SIGN_ALGO_RSA_PSS = 0x100;

const unsigned long SIGN_ALGO_ECDSA = 0x200;
const unsigned long SIGN_ALGO_ECDSA_SHA1 = 0x400;
const unsigned long SIGN_ALGO_ECDSA_SHA224 = 0x800;
const unsigned long SIGN_ALGO_ECDSA_SHA256 = 0x1000;
const unsigned long SIGN_ALGO_ECDSA_SHA384 = 0x2000;
const unsigned long SIGN_ALGO_ECDSA_SHA512 = 0x4000;

const unsigned int SHA1_LEN = 20;
const unsigned int SHA256_LEN = 32;
const unsigned int SHA384_LEN = 48;
const unsigned int SHA512_LEN = 64;

static const unsigned char PTEID_1_APPLET_AID[] = {0x60, 0x46, 0x32, 0xFF, 0x00, 0x00, 0x02};
static const unsigned char PTEID_2_APPLET_NATIONAL_DATA[] = {0x60, 0x46, 0x32, 0xFF, 0x00, 0x00, 0x05};
static const unsigned char PTEID_2_APPLET_EID[] = {0x60, 0x46, 0x32, 0xFF, 0x00, 0x00, 0x04};
static const unsigned char ICAO_APPLET_MRTD[] = {0xA0, 0x00, 0x00, 0x02, 0x47, 0x10, 0x01};
static const unsigned char MULTIPASS_APPLET[] = {0x60, 0x46, 0x32, 0xFF, 0x00, 0x00, 0x06};

static const unsigned char PTEID_CONTACTLESS_ATR[] = {0x3B, 0x8F, 0x80, 0x01, 0x80, 0x31, 0x80, 0x65, 0xB0, 0x85,
													  0x05, 0x00, 0x11, 0x12, 0x0F, 0xFF, 0x82, 0x90, 0x00, 0x8B};

static const unsigned char SELECT_ADF[] = {0x00, 0xA4, 0x02, 0x0C};
static const unsigned char SELECT_MF[] = {0x00, 0xA4, 0x00, 0x0C, 0x02, 0x3F, 0x00};
typedef enum {
	PIN_ENC_ASCII,
	PIN_ENC_BCD,
	PIN_ENC_GP, // Global platform encoding e.g. "12345" -> {0x25, 0x12, 0x34, 0x5F, 0xFF, ... 0xFF}
} tPinEncoding;

typedef enum {
	CARD_PTEID_IAS07,  // v3 v4
	CARD_PTEID_IAS101, // legacy
	CARD_PTEID_IAS5,   // v5 (CC 2.0)
	CARD_ICAO,
	CARD_UNKNOWN,
} tCardType;

typedef enum {
	CARD_INSERTED,
	CARD_NOT_PRESENT,
	CARD_STILL_PRESENT,
	CARD_REMOVED,
	CARD_OTHER,
} tCardStatus;

const unsigned int PERSODATAFILESIZE = 1000;

const unsigned long TIMEOUT_INFINITE = 0xFFFFFFFF; // used in CCardLayer::GetStatusChange()

const unsigned long ALL_READERS = 0xFFFFFFFF; // used in CCardLayer::GetStatusChange()

const unsigned long FULL_FILE = 0xFFFFFFFF; // used in CReader::ReadFile()

const unsigned long PIN_STATUS_UNKNOWN = 0xFFFFFFFE; // used in CReader::PinStatus()


} // namespace eIDMW

#endif
