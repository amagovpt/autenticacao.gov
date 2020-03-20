/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2009 FedICT.
 * Copyright (C) 2019 Caixa Magica Software.
 * Copyright (C) 2011 Vasco Silva - <vasco.silva@caixamagica.pt>
 * Copyright (C) 2011-2012, 2019 André Guerreiro - <aguerreiro1985@gmail.com>
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

#ifndef __PINPAD2_H__
#define __PINPAD2_H__

#ifdef  __cplusplus
extern "C" {
#endif

#ifdef __APPLE__
#include <PCSC/winscard.h>
#include <PCSC/wintypes.h>
#else
#include <winscard.h>
#endif



/////////////////////////////////// CCID things ///////////////////////////////
// More info:
// http://www.pcscworkgroup.com/specifications/files/pcsc10_v2.01.6.pdf
// http://www.usb.org/developers/devclass_docs/DWG_Smart-Card_CCID_Rev110.pdf
///////////////////////////////////////////////////////////////////////////////

#ifndef SCARD_CTL_CODE
#define SCARD_CTL_CODE(code)     (0x42000000 + (code))
#endif
#define CCID_IOCTL_GET_FEATURE_REQUEST  SCARD_CTL_CODE(3400)

#define CCID_VERIFY_START    0x01
#define CCID_VERIFY_FINISH   0x02
#define CCID_VERIFY_DIRECT   0x06
#define CCID_CHANGE_START    0x03
#define CCID_CHANGE_FINISH   0x04
#define CCID_CHANGE_DIRECT   0x07

/* Big endian encoding to 2 bytes */
inline void ToUchar2(unsigned long ulIn, unsigned char *pucOut2)
{
	pucOut2[0] = (unsigned char) (ulIn % 256);
	pucOut2[1] = (unsigned char) (ulIn / 256);
}

/* Big endian encoding to 4 bytes */
inline void ToUchar4(unsigned long ulIn, unsigned char *pucOut4)
{
	pucOut4[0] = (unsigned char) (ulIn % 256);
	ulIn /= 256;
	pucOut4[1] = (unsigned char) (ulIn % 256);
	ulIn /= 256;
	pucOut4[2] = (unsigned char) (ulIn % 256);
	pucOut4[3] = (unsigned char) (ulIn / 256);
}

// The structs below need packing with 1-byte alignment
#pragma pack(push, pinpad2, 1)

#define PP_APDU_MAX_LEN  40

/**
 * Data to be sent during a PIN verification
 * (or unblock without PIN change).
 */
typedef struct
{
	unsigned char bTimerOut;	// timeout in seconds (0: default timeout)
	unsigned char bTimerOut2; // timeout in seconds after first key stroke
	unsigned char bmFormatString; // formatting options
	unsigned char bmPINBlockString; // length (in bytes) of the PIN block
	unsigned char bmPINLengthFormat; // where (if needed) to put the PIN length
	unsigned char wPINMaxExtraDigit[2]; // min and max number of PIN digits
	unsigned char bEntryValidationCondition; // e.g. 0x02: "OK" button pressed
	unsigned char bNumberMessage; // number of messages to display
	unsigned char wLangId[2]; // LANG_ID code
	unsigned char bMsgIndex; // Message index (should be 00)
	unsigned char bTeoPrologue[3]; // T=1 block prologue field to use (fill with 00)
	unsigned char ulDataLength[4]; // length of the following field
	unsigned char abData[PP_APDU_MAX_LEN]; // APDU to send to the card (to be completed by the reader)
} EIDMW_PP_VERIFY_CCID;

/**
 * Data to be sent during a PIN change
 * (or unblock with PIN change).
 */
typedef struct
{
	unsigned char bTimerOut;	// timeout is seconds (00 means use default timeout)
	unsigned char bTimerOut2; // timeout in seconds after first key stroke
	unsigned char bmFormatString; // formatting options
	unsigned char bmPINBlockString; // length (in bytes) of the PIN block
	unsigned char bmPINLengthFormat; // where (if needed) to put the PIN length
	unsigned char bInsertionOffsetOld; //insertion position offset in bytes for the current PIN
	unsigned char bInsertionOffsetNew; // insertion position offset in bytes for the new PIN
	unsigned char wPINMaxExtraDigit[2]; // min and max number of PIN digits
	unsigned char bConfirmPIN; // flags governing need for confirmation of new PIN
	unsigned char bEntryValidationCondition; // e.g. 0x02: "OK" button pressed
	unsigned char bNumberMessage; // number of messages to display
	unsigned char wLangId[2]; // LANG_ID code to specify the language
	unsigned char bMsgIndex1; // index of 1st prompting message
	unsigned char bMsgIndex2; // index of 2d prompting message
	unsigned char bMsgIndex3; // index of 3d prompting message
	unsigned char bTeoPrologue[3]; // T=1 block prologue field to use (fill with 00)
	unsigned char ulDataLength[4]; // length of the following field
	unsigned char abData[PP_APDU_MAX_LEN]; // APDU to send to the card (to be completed by the reader)
} EIDMW_PP_CHANGE_CCID;

#pragma pack(pop, pinpad2)

#ifdef  __cplusplus
}
#endif

#endif
